use std::io::{Result, Write};
use crate::{ast::{UnaryOp::*, BinaryOp::*}, ir::*};

pub fn write_asm(p: &IrProg, w: &mut impl Write) -> Result<()> {
  for g in &p.globs {
    writeln!(w, ".data")?;
    writeln!(w, "{}:", g.0)?;
    match g.1 {
      Ok(init) => writeln!(w, "  .word {}", init)?,
      Err(size) => writeln!(w, "  .zero {}", size * 4)?,
    }
    writeln!(w)?;
  }
  for f in &p.funcs {
    writeln!(w, ".global {}", f.name)?;
    writeln!(w, "{}:", f.name)?;
    // 函数的prologue部分，保存s0和ra寄存器的值，为局部变量开辟空间
    // 进入函数时，0(sp)的位置指向最后一个参数(如果存在的话)
    // 参数的id范围是0..param_cnt，局部变量的id范围是param_cnt..param_cnt + var_cnt，为了能让id简单地对应到偏移量，要让局部变量直接保存在参数后面这片区域
    writeln!(w, "  sw s0, -{}(sp)", (f.var_cnt + 1) * 4)?;
    writeln!(w, "  sw ra, -{}(sp)", (f.var_cnt + 2) * 4)?;
    writeln!(w, "  add s0, sp, {}", f.param_cnt * 4)?;
    writeln!(w, "  add sp, sp, -{}", (f.var_cnt + 2) * 4)?;
    // prologue结束后，栈的布局是这样的：
    //          <-- s0，访问参数0用-4(s0)
    // 参数0
    // 参数1
    // ..
    // 参数m
    // 局部变量0 <-- 访问它用-4 - 4 * m(s0)
    // 局部变量1
    // ...
    // 局部变量n
    // 保存的s0
    // 保存的ra <-- sp
    // <运算栈>
    //
    // 运算栈上可能会进行各种弹栈，压栈操作，但是栈指针始终不会越过prologue结束时的栈指针的位置，上面这些内容与运算栈互不干扰
    for s in &f.stmts {
      writeln!(w, "  # {:?}", s)?; // 输出一条注释，表示下面的汇编对应IR中的什么指令，方便调试
      match s {
        IrStmt::Const(x) => {
          writeln!(w, "  li t0, {}", x)?; // 将常数值保存到t0中
          writeln!(w, "  sw t0, -4(sp)")?; // 以下两句将t0的值压入栈中
          writeln!(w, "  add sp, sp, -4")?;
        }
        IrStmt::Unary(op) => {
          // 逻辑上这里应该分成三部：弹出运算数，运算，压入结果，其中弹出值和压入结果都会修改栈指针
          // 容易发现弹出运算数和压入结果这两步让栈大小+1和-1，最终栈大小没有变，而且运算也不依赖于栈，所以可以省去这两次修改
          writeln!(w, "  lw t0, 0(sp)")?; // 从栈顶读出运算数
          let op = match op { Neg => "neg", BNot => "not", LNot => "seqz" };
          writeln!(w, "  {} t0, t0", op)?; // 对运算数进行一元运算，用neg计算负，not计算按位非，seqz计算逻辑非
          writeln!(w, "  sw t0, 0(sp)")?; // 保存回栈顶的位置
        }
        IrStmt::Binary(op) => {
          // 与Unary类似，这里的翻译过程可以做一点简化，不用修改三次栈指针，只用修改一次：最终栈大小-1
          writeln!(w, "  lw t0, 0(sp)")?; // 从栈顶读出右操作数
          writeln!(w, "  lw t1, 4(sp)")?; // 从栈顶"下面一个"读出左操作数，注意栈往下走地址实际上是变高的
          writeln!(w, "  add sp, sp, 4")?; // 栈大小-1
          // 这个match根据op来执行运算，结果保存到t0中，有些运算可以用一条指令完成，有些复杂一点，具体细节可以查阅RISC-V的资料
          match op {
            Add => writeln!(w, "  add t0, t1, t0")?,
            Sub => writeln!(w, "  sub t0, t1, t0")?,
            Mul => writeln!(w, "  mul t0, t1, t0")?,
            Div => writeln!(w, "  div t0, t1, t0")?,
            Mod => writeln!(w, "  rem t0, t1, t0")?,
            Lt => writeln!(w, "  slt t0, t1, t0")?,
            Le => {
              writeln!(w, "  slt t0, t0, t1")?;
              writeln!(w, "  xor t0, t0, 1")?;
            }
            Ge => {
              writeln!(w, "  slt t0, t1, t0")?;
              writeln!(w, "  xor t0, t0, 1")?;
            }
            Gt => writeln!(w, "  slt t0, t0, t1")?,
            Eq => {
              writeln!(w, "  xor t0, t0, t1")?;
              writeln!(w, "  seqz t0, t0")?;
            }
            Ne => {
              writeln!(w, "  xor t0, t0, t1")?;
              writeln!(w, "  snez t0, t0")?;
            }
            // And/Or不能直接用and/or指令完成，因为这两条指令执行的是按位运算，但是语言要求进行逻辑运算
            And => {
              writeln!(w, "  snez t0, t0")?;
              writeln!(w, "  snez t1, t1")?;
              writeln!(w, "  and t0, t0, t1")?;
            }
            Or => {
              writeln!(w, "  or t0, t0, t1")?;
              writeln!(w, "  snez t0, t0")?;
            }
          };
          writeln!(w, "  sw t0, 0(sp)")?; // 保存回栈顶的位置
        }
        IrStmt::LocalAddr(x) => {
          writeln!(w, "  add t0, s0, -{}", (x + 1) * 4)?; // 计算变量x的地址，保存到t0中
          writeln!(w, "  sw t0, -4(sp)")?; // 以下两句将t0的值压入栈中
          writeln!(w, "  add sp, sp, -4")?;
        }
        IrStmt::GlobAddr(x) => {
          writeln!(w, "  la t0, {}", p.globs[*x as usize].0)?; // 计算变量x的地址，保存到t0中
          writeln!(w, "  sw t0, -4(sp)")?; // 以下两句将t0的值压入栈中
          writeln!(w, "  add sp, sp, -4")?;
        }
        IrStmt::Load => {
          // 与Unary类似，这里的翻译过程可以做一点简化，不用修改栈指针
          writeln!(w, "  lw t0, 0(sp)")?; // 从栈顶读出load的地址
          writeln!(w, "  lw t0, 0(t0)")?; // 执行load操作
          writeln!(w, "  sw t0, 0(sp)")?; // 保存回栈顶的位置
        }
        IrStmt::Store => {
          // 与Unary类似，这里的翻译过程可以做一点简化，不用修改三次栈指针，只用修改一次：最终栈大小-1
          writeln!(w, "  lw t0, 0(sp)")?; // 从栈顶读出store的值
          writeln!(w, "  lw t1, 4(sp)")?; // 从栈顶"下面一个"读出store的地址
          writeln!(w, "  sw t1, 0(t0)")?; // 执行store操作
          writeln!(w, "  add sp, sp, 4")?; // 栈大小-1
        }
        IrStmt::Label(x) => writeln!(w, ".L.{}.{}:", f.name, x)?, // label名字的格式是.L.{函数名}.{label编号}
        IrStmt::Bz(x) => {
          writeln!(w, "  lw t0, 0(sp)")?;
          writeln!(w, "  add sp, sp, 4")?; // 以上两句将栈顶的值弹出，存入t0
          writeln!(w, "  beqz t0, .L.{}.{}", f.name, x)?; // 若t0为0，则跳转到对应label，否则继续执行下一句
        }
        IrStmt::Bnz(x) => { // 与IrStmt::Bz完全类似
          writeln!(w, "  lw t0, 0(sp)")?;
          writeln!(w, "  add sp, sp, 4")?;
          writeln!(w, "  bnez t0, .L.{}.{}", f.name, x)?;
        }
        IrStmt::Jump(x) => writeln!(w, "  j .L.{}.{}", f.name, x)?,
        IrStmt::Call(x) => {
          writeln!(w, "  jal {}", p.funcs[*x as usize].name)?;
          writeln!(w, "  sw a0, -4(sp)")?; // a0保存了函数的返回值，但是还不在运算栈中，调用者负责把它压入栈中
          writeln!(w, "  add sp, sp, -4")?;
        }
        IrStmt::Swap => { // 交换栈顶和次栈顶的两个元素，先把它们从各自的位置读出来，再写入对方的位置
          writeln!(w, "  lw t0, 0(sp)")?;
          writeln!(w, "  lw t1, 4(sp)")?;
          writeln!(w, "  sw t0, 4(sp)")?;
          writeln!(w, "  sw t1, 0(sp)")?;
        }
        IrStmt::Pop => writeln!(w, "  add sp, sp, 4")?,
        IrStmt::Ret => {
          writeln!(w, "  lw a0, 0(sp)")?; // 从栈顶读出返回值，不用修改栈指针，因为下面会恢复栈指针
          // 函数的epilogue部分，恢复栈指针，s0，ra。被调用者负责清除栈上的参数
          writeln!(w, "  mv sp, s0")?; // s0的值是函数prologue中设定的，参数都在它下面，令sp = s0就清除了所有参数
          writeln!(w, "  lw s0, -{}(sp)", (f.param_cnt + f.var_cnt + 1) * 4)?;
          writeln!(w, "  lw ra, -{}(sp)", (f.param_cnt + f.var_cnt + 2) * 4)?;
          writeln!(w, "  ret")?; // a0保存了函数的返回值
        }
      }
    }
  }
  Ok(())
}