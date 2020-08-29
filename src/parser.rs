use crate::ast::*;

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
priority = []

[lexical]
'int' = 'Int'
'return' = 'Return'
';' = 'Semi' # Semi是Semicolon的简称
'\(' = 'LPar' # Par是Parenthesis的简称
'\)' = 'RPar'
'\{' = 'LBrc' # Brc是Brace的简称
'\}' = 'RBrc'
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

  #[rule = "Expr -> IntConst"]
  fn expr_int(i: Token) -> Expr<'p> { Expr::Int(i.parse(), std::marker::PhantomData) }
}