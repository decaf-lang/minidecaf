use crate::ast::{*, UnaryOp::*, BinaryOp::*};

pub struct Parser {}

// Token是过程宏parser_macros::lalr1生成的一个结构体，piece字段表示这个token在原字符串中的片段，类型是&[u8]
impl<'p> Token<'p> {
  fn str(&self) -> &'p str { std::str::from_utf8(self.piece).unwrap() }
  fn parse<T>(&self) -> T where T: std::str::FromStr, <T as std::str::FromStr>::Err: std::fmt::Debug {
    self.str().parse().expect("failed to parse")
  }
}

// 详细用法参见 https://mashplant.online/2020/08/17/lalr1-introduction/
#[parser_macros::lalr1(Prog)]
#[lex = r#"
priority = [
  { assoc = 'left', terms = ['Or'] },
  { assoc = 'left', terms = ['And'] },
  { assoc = 'left', terms = ['Eq', 'Ne'] },
  { assoc = 'left', terms = ['Lt', 'Le', 'Ge', 'Gt'] },
  { assoc = 'left', terms = ['Add', 'Sub'] },
  { assoc = 'left', terms = ['Mul', 'Div', 'Mod'] },
  { assoc = 'left', terms = ['BNot', 'LNot'] },
]

[lexical]
'int' = 'Int'
'return' = 'Return'
';' = 'Semi' # Semi是Semicolon的简称
'\(' = 'LPar' # Par是Parenthesis的简称
'\)' = 'RPar'
'\{' = 'LBrc' # Brc是Brace的简称
'\}' = 'RBrc'
'\+' = 'Add'
'-' = 'Sub'
'\*' = 'Mul'
'/' = 'Div'
'<' = 'Lt'
'<=' = 'Le'
'>=' = 'Ge'
'>' = 'Gt'
'==' = 'Eq'
'!=' = 'Ne'
'&&' = 'And'
'\|\|' = 'Or'
'%' = 'Mod'
'~' = 'BNot'
'!' = 'LNot'
'\s+' = '_Eps'
'\d+' = 'IntConst'
'[a-zA-Z_]\w*' = 'Id' # 以字母或_开头，后面跟0或多个数字，字母或_
"#]
impl<'p> Parser {
  #[rule = "Prog -> Func"]
  fn prog(func: Func<'p>) -> Prog<'p> { Prog { func } }

  #[rule = "Func -> Int Id LPar RPar LBrc Stmt RBrc"]
  fn func(_i: Token, name: Token, _lp: Token, _rp: Token, _lb: Token, stmt: Stmt<'p>, _rb: Token) -> Func<'p> {
    Func { name: name.str(), stmt }
  }

  #[rule = "Stmt -> Return Expr Semi"]
  fn stmt_ret(_r: Token, e: Expr<'p>, _s: Token) -> Stmt<'p> { Stmt::Ret(e) }

  #[rule = "Expr -> LPar Expr RPar"] // AST中直接忽略括号结构
  fn expr_par(_l: Token, e: Expr<'p>, _r: Token) -> Expr<'p> { e }
  #[rule = "Expr -> IntConst"]
  fn expr_int(i: Token) -> Expr<'p> { Expr::Int(i.parse(), std::marker::PhantomData) }
  #[rule = "Expr -> Sub Expr"]
  #[prec = "LNot"] // 本条产生式的优先级不是与Sub相同，而是与LNot相同，比二元运算符高
  fn expr_neg(_: Token, e: Expr<'p>) -> Expr<'p> { Expr::Unary(Neg, Box::new(e)) }
  #[rule = "Expr -> BNot Expr"]
  fn expr_bnot(_: Token, e: Expr<'p>) -> Expr<'p> { Expr::Unary(BNot, Box::new(e)) }
  #[rule = "Expr -> LNot Expr"]
  fn expr_lnot(_: Token, e: Expr<'p>) -> Expr<'p> { Expr::Unary(LNot, Box::new(e)) }
  #[rule = "Expr -> Expr Add Expr"]
  fn expr_add(l: Expr<'p>, _: Token, r: Expr<'p>) -> Expr<'p> { Expr::Binary(Add, Box::new(l), Box::new(r)) }
  #[rule = "Expr -> Expr Sub Expr"]
  fn expr_sub(l: Expr<'p>, _: Token, r: Expr<'p>) -> Expr<'p> { Expr::Binary(Sub, Box::new(l), Box::new(r)) }
  #[rule = "Expr -> Expr Mul Expr"]
  fn expr_mul(l: Expr<'p>, _: Token, r: Expr<'p>) -> Expr<'p> { Expr::Binary(Mul, Box::new(l), Box::new(r)) }
  #[rule = "Expr -> Expr Div Expr"]
  fn expr_div(l: Expr<'p>, _: Token, r: Expr<'p>) -> Expr<'p> { Expr::Binary(Div, Box::new(l), Box::new(r)) }
  #[rule = "Expr -> Expr Mod Expr"]
  fn expr_mod(l: Expr<'p>, _: Token, r: Expr<'p>) -> Expr<'p> { Expr::Binary(Mod, Box::new(l), Box::new(r)) }
  #[rule = "Expr -> Expr Lt Expr"]
  fn expr_lt(l: Expr<'p>, _: Token, r: Expr<'p>) -> Expr<'p> { Expr::Binary(Lt, Box::new(l), Box::new(r)) }
  #[rule = "Expr -> Expr Le Expr"]
  fn expr_le(l: Expr<'p>, _: Token, r: Expr<'p>) -> Expr<'p> { Expr::Binary(Le, Box::new(l), Box::new(r)) }
  #[rule = "Expr -> Expr Ge Expr"]
  fn expr_ge(l: Expr<'p>, _: Token, r: Expr<'p>) -> Expr<'p> { Expr::Binary(Ge, Box::new(l), Box::new(r)) }
  #[rule = "Expr -> Expr Gt Expr"]
  fn expr_gt(l: Expr<'p>, _: Token, r: Expr<'p>) -> Expr<'p> { Expr::Binary(Gt, Box::new(l), Box::new(r)) }
  #[rule = "Expr -> Expr Eq Expr"]
  fn expr_eq(l: Expr<'p>, _: Token, r: Expr<'p>) -> Expr<'p> { Expr::Binary(Eq, Box::new(l), Box::new(r)) }
  #[rule = "Expr -> Expr Ne Expr"]
  fn expr_ne(l: Expr<'p>, _: Token, r: Expr<'p>) -> Expr<'p> { Expr::Binary(Ne, Box::new(l), Box::new(r)) }
  #[rule = "Expr -> Expr And Expr"]
  fn expr_and(l: Expr<'p>, _: Token, r: Expr<'p>) -> Expr<'p> { Expr::Binary(And, Box::new(l), Box::new(r)) }
  #[rule = "Expr -> Expr Or Expr"]
  fn expr_or(l: Expr<'p>, _: Token, r: Expr<'p>) -> Expr<'p> { Expr::Binary(Or, Box::new(l), Box::new(r)) }
}