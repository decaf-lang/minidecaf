use std::io::{Result, Write};
use crate::ir::*;

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
      IrStmt::Ret => {
        writeln!(w, "  lw a0, 0(sp)")?;
        writeln!(w, "  add sp, sp, 4")?; // 以上两句将栈顶的值弹出，存入a0
        writeln!(w, "  ret")?; // a0保存了函数的返回值
      }
    }
  }
  Ok(())
}