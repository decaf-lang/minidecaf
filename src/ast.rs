#[derive(Debug)]
pub struct Prog<'a> {
  pub funcs: Vec<Func<'a>>,
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
  pub params: Vec<Decl<'a>>,
  // 函数定义中stmts为Some，函数声明中stmts为None
  pub stmts: Option<Vec<Stmt<'a>>>,
}

#[derive(Debug)]
pub enum Stmt<'a> {
  Empty,
  Ret(Expr<'a>),
  Decl(Decl<'a>),
  Expr(Expr<'a>),
  // 这里的Stmt实际不可能是Stmt::Decl，parser不会生成这样的结构，While/DoWhile/For中的Stmt也是一样的
  If(Expr<'a>, Box<Stmt<'a>>, Option<Box<Stmt<'a>>>),
  Block(Vec<Stmt<'a>>),
  // AST中没有While的结构，while语句直接用update为None的For来表示
  DoWhile(Box<Stmt<'a>>, Expr<'a>),
  // for的init语句不在这里，一条for (init; cond; update) body在AST中表示为：
  // Block(vec![init, For { cond, update, body }])
  // 而且init语句只能是三种：Empty/Decl/Expr，这也是parser决定的
  For { cond: Option<Expr<'a>>, update: Option<Expr<'a>>, body: Box<Stmt<'a>> },
  Continue,
  Break,
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
  Call(&'a str, Vec<Expr<'a>>),
}