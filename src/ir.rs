use crate::ast::*;

#[derive(Debug)]
pub struct IrProg<'a> {
  pub func: IrFunc<'a>,
}

#[derive(Debug)]
pub struct IrFunc<'a> {
  pub name: &'a str,
  pub stmts: Vec<IrStmt>,
}

#[derive(Debug)]
pub enum IrStmt {
  // 把一个常数压入栈中
  Const(i32),
  // 弹出栈顶元素，对其进行相应的UnaryOp后把结果压入栈顶
  Unary(UnaryOp),
  // 依次弹出栈顶的两个元素，分别作为右操作数和左操作数(右操作数在栈顶，左操作数是下面一个)，对其进行相应的BinaryOp后把结果压入栈顶
  Binary(BinaryOp),
  // 弹出栈顶元素，将其作为返回值返回当前函数
  Ret,
}

pub fn ast2ir<'a>(p: &Prog<'a>) -> IrProg<'a> {
  IrProg { func: func(&p.func) }
}

fn func<'a>(f: &Func<'a>) -> IrFunc<'a> {
  let mut stmts = Vec::new();
  match &f.stmt {
    Stmt::Ret(e) => {
      // 为了翻译一条return语句，先翻译它return的表达式，这样栈顶就是这个表达式的值，再生成一条Ret指令弹出它作为返回值
      expr(&mut stmts, e);
      stmts.push(IrStmt::Ret);
    }
  }
  IrFunc { name: f.name, stmts }
}

// 一条表达式执行完后最终总会往栈中压入一个元素，即这个表达式的值
fn expr(stmts: &mut Vec<IrStmt>, e: &Expr) {
  match e {
    Expr::Int(x, _) => stmts.push(IrStmt::Const(*x)),
    Expr::Unary(op, x) => {
      // 为了翻译一个unary表达式，先翻译它的操作数，这样栈顶就是操作数的值，再生成一条Unary指令基于栈顶的值进行计算
      expr(stmts, x);
      stmts.push(IrStmt::Unary(*op));
    }
    Expr::Binary(op, l, r) => {
      // 为了翻译一个binary表达式，先翻译它的左操作数，再翻译它的右操作数
      // 这样栈顶就是右操作数的值，栈顶下面一个就是左操作数的值，再生成一条Binary指令基于这两个值进行计算
      expr(stmts, l);
      expr(stmts, r);
      stmts.push(IrStmt::Binary(*op));
    }
  }
}