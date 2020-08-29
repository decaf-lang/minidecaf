#[derive(Debug)]
pub struct Prog<'a> {
  pub func: Func<'a>,
}

// 前几个spec中函数名字必须是main，而且把main作为一个关键字，我不愿意这样实现，反正之后总是要允许任意的名字的
// parser里也是一样，识别通用的标识符并且保存下来了，而不是识别字符串main
#[derive(Debug)]
pub struct Func<'a> {
  pub name: &'a str,
  pub stmt: Stmt<'a>,
}

#[derive(Debug)]
pub enum Stmt<'a> {
  Ret(Expr<'a>),
}

#[derive(Debug, Copy, Clone, Ord, PartialOrd, Eq, PartialEq)]
pub enum UnaryOp { Neg, BNot, LNot }

#[derive(Debug, Copy, Clone, Ord, PartialOrd, Eq, PartialEq)]
pub enum BinaryOp { Add, Sub, Mul, Div, Mod }

// 之后的表达式中会用上这个生命周期参数，这里提前加上以免后面大量改动。由于Rust不允许未使用的生命周期参数，这里使用PhantomData
#[derive(Debug)]
pub enum Expr<'a> {
  Int(i32, std::marker::PhantomData<&'a ()>),
  Unary(UnaryOp, Box<Expr<'a>>),
  Binary(BinaryOp, Box<Expr<'a>>, Box<Expr<'a>>),
}