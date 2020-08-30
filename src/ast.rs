#[derive(Debug)]
pub struct Prog<'a> {
  pub func: Func<'a>,
}

#[derive(Debug)]
pub struct Decl<'a> {
  pub name: &'a str,
  // 一个可选的初始值
  pub init: Option<Expr<'a>>,
}

#[derive(Debug)]
pub struct Func<'a> {
  pub name: &'a str,
  pub stmts: Vec<Stmt<'a>>,
}

#[derive(Debug)]
pub enum Stmt<'a> {
  Empty,
  Ret(Expr<'a>),
  Decl(Decl<'a>),
  Expr(Expr<'a>),
}

#[derive(Debug, Copy, Clone, Ord, PartialOrd, Eq, PartialEq)]
pub enum UnaryOp { Neg, BNot, LNot }

#[derive(Debug, Copy, Clone, Ord, PartialOrd, Eq, PartialEq)]
pub enum BinaryOp { Add, Sub, Mul, Div, Mod, Lt, Le, Ge, Gt, Eq, Ne, And, Or }

#[derive(Debug)]
pub enum Expr<'a> {
  Int(i32),
  Unary(UnaryOp, Box<Expr<'a>>),
  Binary(BinaryOp, Box<Expr<'a>>, Box<Expr<'a>>),
  Var(&'a str),
  Assign(&'a str, Box<Expr<'a>>),
}