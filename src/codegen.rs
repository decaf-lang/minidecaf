use std::io::{Result, Write};
use crate::{ast::UnaryOp::*, ir::*};

pub fn write_asm(p: &IrProg, w: &mut impl Write) -> Result<()> {
  let f = &p.func;
  writeln!(w, ".global {}", f.name)?;
  writeln!(w, "{}:", f.name)?;
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
      IrStmt::Ret => {
        writeln!(w, "  lw a0, 0(sp)")?;
        writeln!(w, "  add sp, sp, 4")?; // 以上两句将栈顶的值弹出，存入a0
        writeln!(w, "  ret")?; // a0保存了函数的返回值
      }
    }
  }
  Ok(())
}