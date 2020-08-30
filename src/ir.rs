use std::collections::{HashMap, hash_map::Entry};
use crate::ast::*;

#[derive(Debug)]
pub struct IrProg<'a> {
  pub funcs: Vec<IrFunc<'a>>,
}

#[derive(Debug)]
pub struct IrFunc<'a> {
  pub name: &'a str,
  // 本函数接受参数的数目
  pub param_cnt: u32,
  // 本函数局部变量的数目(参数不属于局部变量)
  pub var_cnt: u32,
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
  // 将对应id的局部变量的实际地址压入栈顶
  LocalAddr(u32),
  // 弹出栈顶元素，把它作为一个地址，从这个地址中load出结果，把结果压入栈顶
  Load,
  // 依次弹出栈顶的两个元素，分别作为store的地址和store的值(地址在栈顶，值是下面一个)，把值store到地址中，然后再把值重新压回栈中
  // (之所以有这个"压回"的操作，是为了实现表达式a = b的结果为b的值的语义)
  Store,
  // 定义一个标号，不涉及任何操作
  Label(u32),
  // 弹出栈顶的值，如果它等于0，则跳转到对应标号，否则继续执行下一条语句
  Bz(u32),
  // 弹出栈顶的值，如果它不等于0，则跳转到对应标号，否则继续执行下一条语句
  Bnz(u32),
  // 跳转到对应标号
  Jump(u32),
  // 调用IrProg.funcs对应下标的函数，调用前运算栈中需要从左到右依次压入参数(最右边的参数在栈顶
  // 调用后这些参数都被弹出，且运算栈中压入函数的返回值
  // 注意这里并不涉及调用约定的细节，比如到底是caller还是callee把参数弹出运算栈，把返回值压入运算栈的，只是说整个调用结束后结果应该是这样
  Call(u32),
  // 弹出栈顶元素
  Pop,
  // 弹出栈顶元素，将其作为返回值返回当前函数
  Ret,
}

pub fn ast2ir<'a>(p: &'a Prog<'a>) -> IrProg<'a> {
  // funcs用来表示函数名到符号的映射，funcs1用来保存所有函数定义的ir
  // funcs的V类型是(u32, &Func)，这个u32表示本函数的ir在funcs1中的下标
  let (mut funcs, mut funcs1) = (HashMap::new(), Vec::new());
  for f in &p.funcs {
    match funcs.entry(f.name) {
      Entry::Vacant(v) => {
        v.insert((funcs1.len() as u32, f));
        // 这个f可能只是一个函数声明，这样构造出来的ir只有空架子，后续遇到同名的函数定义时会更新
        funcs1.push(func(f, &funcs));
      }
      Entry::Occupied(o) => {
        let (old_id, old_f) = *o.get();
        // stmts.is_some()为true则是函数定义，为false则是函数声明
        // 如果两个重名的函数都是函数定义，或者它们的参数不匹配，则不合法
        if (old_f.stmts.is_some() && f.stmts.is_some()) || old_f.params.len() != f.params.len() {
          panic!("conflict function definition `{}` in current context", f.name)
        }
        // 如果老函数是函数声明，新函数是函数定义，则更新ir中对应下标的函数
        if f.stmts.is_some() { funcs1[old_id as usize] = func(f, &funcs); }
      }
    }
  }
  IrProg { funcs: funcs1 }
}

// 将变量的名字映射到(变量的id，变量的定义)，这个Decl中目前还没有保存有用的信息，之后会用到它的
// 这个id基本可以理解成变量在栈上的offset
type SymbolMap<'a> = HashMap<&'a str, (u32, &'a Decl<'a>)>;
type FuncMap<'a> = HashMap<&'a str, (u32, &'a Func<'a>)>;

// 为一个函数生成IR的过程中维护的一些信息
struct FuncCtx<'a, 'b> {
  // 每个语句块对应一个SymbolMap，进入一个语句块时往其中压入一个新的SymbolMap，离开一个语句块时弹出最后的SymbolMap
  names: Vec<SymbolMap<'a>>,
  stmts: Vec<IrStmt>,
  // 遇到一个循环时往其中压入一对值，分别是(这个循环中break要跳转的位置，这个循环中continue要跳转的位置)，离开循环时就弹出这个值
  // 处理break/continue时总会访问最后一个元素，如果最后一个元素不存在，就意味着break/continue在循环外
  loops: Vec<(u32, u32)>,
  funcs: &'b FuncMap<'a>,
  // 当前局部变量的数目
  var_cnt: u32,
  // 当前标号的数目
  label_cnt: u32,
}

impl<'a> FuncCtx<'a, '_> {
  fn new_label(&mut self) -> u32 { (self.label_cnt, self.label_cnt += 1).0 }

  // 在当前环境中查找对应名称的变量，如果找到了就返回它的id，否则就panic
  fn lookup(&self, name: &str) -> u32 {
    // 在所有SymbolMap中逆序查找，这样就会优先找到本条语句所在的语句块中定义的变量，越往外优先级越低
    for map in self.names.iter().rev() {
      if let Some(x) = map.get(name) { return x.0; }
    }
    panic!("variable `{}` not defined in current context", name)
  }
}

fn func<'a>(f: &Func<'a>, funcs: &FuncMap<'a>) -> IrFunc<'a> {
  let mut ctx = FuncCtx { names: vec![HashMap::new()], stmts: Vec::new(), loops: Vec::new(), funcs, var_cnt: 0, label_cnt: 0 };
  // 参数和局部变量一起参与id的分配，参数0的id是0，第一个局部变量的id是参数的数目。后面生成汇编代码的时候，也会设法保证这个排布肾虚
  for p in &f.params {
    assert!(p.init.is_none(), "function param has init");
    decl(&mut ctx, p);
  }
  for s in f.stmts.as_deref().unwrap_or(&[]) { stmt(&mut ctx, s); }
  // 如果函数的指令序列不以Ret结尾，则生成一条return 0
  match ctx.stmts.last() {
    Some(IrStmt::Ret) => {}
    _ => {
      ctx.stmts.push(IrStmt::Const(0));
      ctx.stmts.push(IrStmt::Ret);
    }
  }
  // 现在的ctx.var_cnt是包含了参数数目在内的，要得到真正的局部变量的数目需要减去参数数目
  let param_cnt = f.params.len() as u32;
  IrFunc { name: f.name, param_cnt, var_cnt: ctx.var_cnt - param_cnt, stmts: ctx.stmts }
}

// 在当前环境中定义一个变量
fn decl<'a>(ctx: &mut FuncCtx<'a, '_>, d: &'a Decl<'a>) {
  let id = ctx.var_cnt;
  // 只在最后一个SymbolMap，也就是当前语句所在的语句块的SymbolMap中定义这个变量
  if ctx.names.last_mut().unwrap().insert(d.name, (id, d)).is_some() {
    panic!("variable `{}` redefined in current context", d.name)
  }
  ctx.var_cnt = id + 1;
  if let Some(x) = &d.init {
    // 这一串操作其实就是执行一次Stmt::Expr(Expr::Assign)
    expr(ctx, x);
    ctx.stmts.push(IrStmt::LocalAddr(id));
    ctx.stmts.push(IrStmt::Store);
    ctx.stmts.push(IrStmt::Pop);
  }
}

fn stmt<'a>(ctx: &mut FuncCtx<'a, '_>, s: &'a Stmt<'a>) {
  match s {
    Stmt::Empty => {}
    Stmt::Ret(e) => {
      // 为了翻译一条return语句，先翻译它return的表达式，这样栈顶就是这个表达式的值，再生成一条Ret指令弹出它作为返回值
      expr(ctx, e);
      ctx.stmts.push(IrStmt::Ret);
    }
    Stmt::Decl(d) => decl(ctx, d),
    Stmt::Expr(e) => {
      expr(ctx, e);
      // 上面生成的代码执行完后运算栈的大小会+1，但是Stmt不应该改变栈的大小，所以生成一条Pop来恢复运算栈
      ctx.stmts.push(IrStmt::Pop);
    }
    Stmt::If(cond, t, f) => {
      expr(ctx, cond);
      let (before_f, after_f) = (ctx.new_label(), ctx.new_label());
      ctx.stmts.push(IrStmt::Bz(before_f)); // 依据cond的结果进行跳转，如果为cond为0，则跳到else分支，否则进入then分支
      stmt(ctx, t);
      ctx.stmts.push(IrStmt::Jump(after_f)); // then分支结束后需要跳过else分支
      ctx.stmts.push(IrStmt::Label(before_f)); // else分支的开始位置
      if let Some(f) = f { stmt(ctx, f); }
      ctx.stmts.push(IrStmt::Label(after_f)); // else分支的结束位置
    }
    Stmt::Block(stmts) => {
      ctx.names.push(HashMap::new());
      for s in stmts { stmt(ctx, s); }
      ctx.names.pop();
    }
    Stmt::DoWhile(body, cond) => {
      let (before_body, before_cond, after_cond) = (ctx.new_label(), ctx.new_label(), ctx.new_label());
      ctx.loops.push((after_cond, before_cond));
      ctx.stmts.push(IrStmt::Label(before_body)); // 循环体的开头
      stmt(ctx, body); // 循环体一开始先执行循环中的语句
      ctx.stmts.push(IrStmt::Label(before_cond)); // continue的位置
      expr(ctx, cond);
      ctx.stmts.push(IrStmt::Bnz(before_body)); // 如果cond非0则跳转到循环体的开头，否则离开循环
      ctx.stmts.push(IrStmt::Label(after_cond)); // break的位置
      ctx.loops.pop();
    }
    // init语句不在这里处理
    Stmt::For { cond, update, body } => {
      let (before_cond, before_update, after_body) = (ctx.new_label(), ctx.new_label(), ctx.new_label());
      ctx.loops.push((after_body, before_update));
      ctx.stmts.push(IrStmt::Label(before_cond)); // 循环体的开头
      if let Some(cond) = cond { // 如果没有cond，也就是for (init;;update)这样的代码，语义就是不检查，直接执行循环中的语句
        expr(ctx, cond);
        ctx.stmts.push(IrStmt::Bz(after_body)); // 如果cond为0则跳出循环，否则执行循环中的语句
      }
      stmt(ctx, body);
      ctx.stmts.push(IrStmt::Label(before_update)); // continue的位置，continue后仍要执行本次循环的update语句
      if let Some(update) = update {
        expr(ctx, update);
        ctx.stmts.push(IrStmt::Pop); // 这里和Stmt::Expr一样需要一个Pop
      }
      ctx.stmts.push(IrStmt::Jump(before_cond)); // 跳转回到循环体的开头
      ctx.stmts.push(IrStmt::Label(after_body)); // break的位置
      ctx.loops.pop();
    }
    Stmt::Break => ctx.stmts.push(IrStmt::Jump(ctx.loops.last().expect("break out of loop").0)),
    Stmt::Continue => ctx.stmts.push(IrStmt::Jump(ctx.loops.last().expect("continue out of loop").1)),
  }
}

// 一条表达式执行完后最终总会往栈中压入一个元素，即这个表达式的值
fn expr<'a>(ctx: &mut FuncCtx<'a, '_>, e: &Expr) {
  match e {
    Expr::Int(x) => ctx.stmts.push(IrStmt::Const(*x)),
    Expr::Unary(op, x) => {
      // 为了翻译一个unary表达式，先翻译它的操作数，这样栈顶就是操作数的值，再生成一条Unary指令基于栈顶的值进行计算
      expr(ctx, x);
      ctx.stmts.push(IrStmt::Unary(*op));
    }
    Expr::Binary(op, l, r) => {
      // 为了翻译一个binary表达式，先翻译它的左操作数，再翻译它的右操作数
      // 这样栈顶就是右操作数的值，栈顶下面一个就是左操作数的值，再生成一条Binary指令基于这两个值进行计算
      expr(ctx, l);
      expr(ctx, r);
      ctx.stmts.push(IrStmt::Binary(*op));
    }
    Expr::Var(name) => {
      let id = ctx.lookup(name);
      ctx.stmts.push(IrStmt::LocalAddr(id));
      ctx.stmts.push(IrStmt::Load);
    }
    Expr::Assign(name, rhs) => {
      // 为了翻译一个assign表达式，先翻译它的右操作数，即赋值的值，再往栈中压入赋值的目标地址
      // 执行Store后就会完成这个assign的操作，同时在栈中留下右操作数的值
      expr(ctx, rhs);
      let id = ctx.lookup(name);
      ctx.stmts.push(IrStmt::LocalAddr(id));
      ctx.stmts.push(IrStmt::Store);
    }
    Expr::Condition(cond, t, f) => {
      // 依据cond的结果进行跳转，如果为cond为0，则计算表达式f，否则计算表达式t；整体的实现方式与Stmt::If是完全类似的
      expr(ctx, cond);
      let (before_f, after_f) = (ctx.new_label(), ctx.new_label());
      ctx.stmts.push(IrStmt::Bz(before_f));
      expr(ctx, t);
      ctx.stmts.push(IrStmt::Jump(after_f));
      ctx.stmts.push(IrStmt::Label(before_f));
      expr(ctx, f);
      ctx.stmts.push(IrStmt::Label(after_f));
    }
    Expr::Call(func, args) => {
      let (id, f) = *ctx.funcs.get(func).expect("function not defined in current context");
      assert_eq!(args.len(), f.params.len(), "function call args params mismatch");
      for a in args { expr(ctx, a); } // 从左到右依次计算参数，也依次压入运算栈中
      ctx.stmts.push(IrStmt::Call(id)); // 执行调用，结束后参数被清除，栈顶是函数的返回值
    }
  }
}