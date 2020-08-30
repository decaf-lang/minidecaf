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
  { assoc = 'right', terms = ['Assign'] },
  { assoc = 'right', terms = ['Question', 'Colon'] },
  { assoc = 'left', terms = ['Or'] },
  { assoc = 'left', terms = ['And'] },
  { assoc = 'left', terms = ['Eq', 'Ne'] },
  { assoc = 'left', terms = ['Lt', 'Le', 'Ge', 'Gt'] },
  { assoc = 'left', terms = ['Add', 'Sub'] },
  { assoc = 'left', terms = ['Mul', 'Div', 'Mod'] },
  { assoc = 'left', terms = ['BNot', 'LNot'] },
  { assoc = 'left', terms = ['Else'] },
]

[lexical]
'int' = 'Int'
'return' = 'Return'
'if' = 'If'
'else' = 'Else'
'while' = 'While'
'do' = 'Do'
'for' = 'For'
'break' = 'Break'
'continue' = 'Continue'
';' = 'Semi' # Semi是Semicolon的简称
'\(' = 'LPar' # Par是Parenthesis的简称
'\)' = 'RPar'
'\{' = 'LBrc' # Brc是Brace的简称
'\}' = 'RBrc'
'=' = 'Assign'
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
'\?' = 'Question'
':' = 'Colon'
'\s+' = '_Eps'
'\d+' = 'IntConst'
'[a-zA-Z_]\w*' = 'Id' # 以字母或_开头，后面跟0或多个数字，字母或_
"#]
impl<'p> Parser {
  #[rule = "Prog -> Func"]
  fn prog(func: Func<'p>) -> Prog<'p> { Prog { func } }

  #[rule = "Decl -> Int Id"]
  fn decl0(_i: Token, name: Token) -> Decl<'p> { Decl { name: name.str(), init: None } }
  #[rule = "Decl -> Int Id Assign Expr"]
  fn decl1(_i: Token, name: Token, _a: Token, init: Expr<'p>) -> Decl<'p> { Decl { name: name.str(), init: Some(init) } }

  #[rule = "Func -> Int Id LPar RPar LBrc Stmts RBrc"]
  fn func(_i: Token, name: Token, _lp: Token, _rp: Token, _lb: Token, stmts: Vec<Stmt<'p>>, _rb: Token) -> Func<'p> {
    Func { name: name.str(), stmts }
  }

  #[rule = "Stmts ->"]
  fn stmts0() -> Vec<Stmt<'p>> { Vec::new() }
  #[rule = "Stmts -> Stmts Stmt"]
  fn stmts1(mut l: Vec<Stmt<'p>>, r: Stmt<'p>) -> Vec<Stmt<'p>> { (l.push(r), l).1 }
  #[rule = "Stmts -> Stmts Decl Semi"] // 只有Stmt序列中允许出现Decl，一般的Stmt则不允许，这样就拒绝了if (1) int x;这样的程序
  fn stmts2(mut l: Vec<Stmt<'p>>, d: Decl<'p>, _s: Token) -> Vec<Stmt<'p>> { (l.push(Stmt::Decl(d)), l).1 }

  #[rule = "Stmt -> Semi"]
  fn stmt_empty(_s: Token) -> Stmt<'p> { Stmt::Empty }
  #[rule = "Stmt -> Return Expr Semi"]
  fn stmt_ret(_r: Token, e: Expr<'p>, _s: Token) -> Stmt<'p> { Stmt::Ret(e) }
  #[rule = "Stmt -> Expr Semi"]
  fn stmt_expr(e: Expr<'p>, _s: Token) -> Stmt<'p> { Stmt::Expr(e) }
  #[rule = "Stmt -> If LPar Expr RPar Stmt MaybeElse"]
  fn stmt_if(_i: Token, _l: Token, cond: Expr<'p>, _r: Token, t: Stmt<'p>, f: Option<Box<Stmt<'p>>>) -> Stmt<'p> { Stmt::If(cond, Box::new(t), f) }
  #[rule = "Stmt -> LBrc Stmts RBrc"]
  fn stmt_block(_l: Token, stmts: Vec<Stmt<'p>>, _r: Token) -> Stmt<'p> { Stmt::Block(stmts) }
  #[rule = "Stmt -> While LPar Expr RPar Stmt"]
  fn stmt_while(_w: Token, _l: Token, cond: Expr<'p>, _r: Token, body: Stmt<'p>) -> Stmt<'p> { Stmt::For { cond: Some(cond), update: None, body: Box::new(body) } }
  #[rule = "Stmt -> Do Stmt While LPar Expr RPar Semi"]
  fn stmt_do_while(_d: Token, body: Stmt<'p>, _w: Token, _l: Token, cond: Expr<'p>, _r: Token, _s: Token) -> Stmt<'p> { Stmt::DoWhile(Box::new(body), cond) }
  #[rule = "Stmt -> For LPar MaybeExpr Semi MaybeExpr Semi MaybeExpr RPar Stmt"]
  fn stmt_for0(_f: Token, _l: Token, init: Option<Expr<'p>>, _s1: Token, cond: Option<Expr<'p>>, _s2: Token, update: Option<Expr<'p>>, _r: Token, body: Stmt<'p>) -> Stmt<'p> {
    Stmt::Block(vec![init.map_or(Stmt::Empty, |x| Stmt::Expr(x)), Stmt::For { cond, update, body: Box::new(body) }])
  }
  #[rule = "Stmt -> For LPar Decl Semi MaybeExpr Semi MaybeExpr RPar Stmt"]
  fn stmt_for1(_f: Token, _l: Token, d: Decl<'p>, _s1: Token, cond: Option<Expr<'p>>, _s2: Token, update: Option<Expr<'p>>, _r: Token, body: Stmt<'p>) -> Stmt<'p> {
    Stmt::Block(vec![Stmt::Decl(d), Stmt::For { cond, update, body: Box::new(body) }])
  }
  #[rule = "Stmt -> Break Semi"]
  fn stmt_break(_b: Token, _s: Token) -> Stmt<'p> { Stmt::Break }
  #[rule = "Stmt -> Continue Semi"]
  fn stmt_continue(_c: Token, _s: Token) -> Stmt<'p> { Stmt::Continue }

  #[rule = "MaybeElse ->"]
  #[prec = "LNot"] // 这个优先级比较随意，只要比MaybeElse -> Else Stmt产生式的优先级低就可以了，也就是比Else的优先级低
  fn maybe_else0() -> Option<Box<Stmt<'p>>> { None }
  #[rule = "MaybeElse -> Else Stmt"]
  fn maybe_else1(_e: Token, s: Stmt<'p>) -> Option<Box<Stmt<'p>>> { Some(Box::new(s)) }

  #[rule = "MaybeExpr ->"]
  fn maybe_expr0() -> Option<Expr<'p>> { None }
  #[rule = "MaybeExpr -> Expr"]
  fn maybe_expr1(e: Expr<'p>) -> Option<Expr<'p>> { Some(e) }

  #[rule = "Expr -> LPar Expr RPar"] // AST中直接忽略括号结构
  fn expr_par(_l: Token, e: Expr<'p>, _r: Token) -> Expr<'p> { e }
  #[rule = "Expr -> IntConst"]
  fn expr_int(i: Token) -> Expr<'p> { Expr::Int(i.parse()) }
  #[rule = "Expr -> Id"]
  fn expr_var(name: Token) -> Expr<'p> { Expr::Var(name.str()) }
  #[rule = "Expr -> Id Assign Expr"]
  fn expr_assign(name: Token, _a: Token, r: Expr<'p>) -> Expr<'p> { Expr::Assign(name.str(), Box::new(r)) }
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
  #[rule = "Expr -> Expr Question Expr Colon Expr"]
  fn expr_condition(cond: Expr<'p>, _q: Token, t: Expr<'p>, _c: Token, f: Expr<'p>) -> Expr<'p> { Expr::Condition(Box::new(cond), Box::new(t), Box::new(f)) }
}