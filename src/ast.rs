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
  // 这里的Stmt实际不可能是Stmt::Decl，parser不会生成这样的结构
  If(Expr<'a>, Box<Stmt<'a>>, Option<Box<Stmt<'a>>>),
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
  // 三个Box<Expr<'a>>分别是a ? b : c中的a，b，c
  Condition(Box<Expr<'a>>, Box<Expr<'a>>, Box<Expr<'a>>),
}