#include <cstring>
#include <cassert>
#include <cstdio>
#include "parser.h"

// lex 得到的 token list，没分析一个 token 就 pop 一个
static std::list<TKPtr>* toks_;
// 已经分析过的 token
static std::list<TKPtr> used_toks_;
// 当前正在分析的 token
static TKPtr token_ = NULL;
// 目标汇编的字长
const int POINTER_WIDTH = 4;
// LOG_2(字长)
const int POINTER_WIDTH_LOG = 2;
// 局部变量栈，保存当前存活的局部变量
static std::list<VarPtr> lvar_stack;
// 局部变量栈中的变量对应的作用域深度，其实可以和局部变量栈写成一个栈
static std::list<int> lvar_stack_depth;
// 局部变量栈曾达到的最大长度
static int max_stack_size = 0;  // 注意这个变量含义变了
// 局部变量栈中当前变量累计长度
static int stack_size = 0;
// 当前作用域嵌套的深度
static int scope_depth = 0;
// 当前正在处理的函数
FNPtr hot_func = NULL;
// 记录已经声明的函数
static std::list<FNPtr> funcs;
// 记录全局变量
static std::list<VarPtr> global_vars;

// 转入处理下一个 token
void next_token() {   
    used_toks_.push_back(token_);
    assert(!toks_->empty());     
    token_ = toks_->front();   
    toks_->pop_front();
}

// 反悔了，处理上一个 token
void checkout_token() {
    toks_->push_front(token_);
    token_ = used_toks_.back();
    used_toks_.pop_back();
}

inline TKPtr last_token() {
    assert(!used_toks_.empty());
    return used_toks_.back();
}

inline void push_scope() {
    ++scope_depth;
}

inline void pop_scope() {
    --scope_depth;
    while(!lvar_stack.empty() && lvar_stack_depth.front() > scope_depth) {
        stack_size -= lvar_stack.front()->type->size / POINTER_WIDTH;
        lvar_stack.pop_front();
        lvar_stack_depth.pop_front();
    }
}

// 相当于 Node 的构造函数
NDPtr new_node(TKPtr tok, NodeKind kind) {
    NDPtr node = std::make_shared<Node>();
    node->kind = kind;
    node->tok = tok;
    return node;
}

// 相当于 unary 的构造函数
NDPtr new_unary(TKPtr tok, NodeKind kind, NDPtr expr) {
    NDPtr node = new_node(tok, kind);
    if(kind == ND_BITNOT || kind == ND_NEG)
        assert(is_integer(expr->type));
    node->lexpr = expr;
    node->type = int_type();
    return node;
}

NDPtr new_unary_ptr(TKPtr tok, NodeKind kind, NDPtr expr) {
    assert(expr->type);
    NDPtr node = new_node(tok, kind);
    node->lexpr = expr;
    if(kind == ND_DEREF) {
        assert(is_ptr(expr->type));
        node->type = expr->type->base;
    } else {
        node->type = pointer_to(expr->type);
    }
    return node;
}

NDPtr new_binary_ptr(TKPtr tok, NodeKind kind, NDPtr lexpr, NDPtr rexpr, TYPtr ty) {
    NDPtr node = new_node(tok, kind);
    node->lexpr = lexpr;
    node->rexpr = rexpr;
    node->type = ty;
    return node;
}

NDPtr new_add(TKPtr tok, NDPtr lexpr, NDPtr rexpr) {
    if (is_integer(lexpr->type) && is_integer(rexpr->type))
        return new_binary_ptr(tok, ND_ADD, lexpr, rexpr, int_type());
    if (is_ptr(lexpr->type) && is_integer(rexpr->type))
        return new_binary_ptr(tok, ND_PTR_ADD, lexpr, rexpr, lexpr->type);
    if (is_integer(lexpr->type) && is_ptr(rexpr->type))
        return new_binary_ptr(tok, ND_PTR_ADD, rexpr, lexpr, rexpr->type);
    assert(false);
    return NULL;
}

NDPtr new_sub(TKPtr tok, NDPtr lexpr, NDPtr rexpr) {
    if (is_integer(lexpr->type) && is_integer(rexpr->type))
        return new_binary_ptr(tok, ND_SUB, lexpr, rexpr, int_type());
    if (is_ptr(lexpr->type) && is_integer(rexpr->type))
        return new_binary_ptr(tok, ND_PTR_SUB, lexpr, rexpr, lexpr->type);
    if (is_ptr(lexpr->type) && is_ptr(rexpr->type))
        return new_binary_ptr(tok, ND_PTR_DIFF, lexpr, rexpr, int_type());
    assert(false);
    return NULL;
}

NDPtr new_num(TKPtr tok, int val) {
    NDPtr node = new_node(tok, ND_NUM);
    node->val = val;
    node->type = int_type();
    return node;
}

NDPtr new_stmt(TKPtr tok, NodeKind kind, NDPtr expr) {
    NDPtr node = new_node(tok, kind);
    node->lexpr = expr;
    return node;
}

// 目前语义规则中，除了 assign/equal 双目运算符的操作数必须都是 int，结果也一定是 int
// 为了方便，特殊处理 assign/equal
NDPtr new_binary(TKPtr tok, NodeKind kind, NDPtr lexpr, NDPtr rexpr) {
    NDPtr node = new_node(tok, kind);
    node->lexpr = lexpr;
    node->rexpr = rexpr;
    node->type = int_type();
    return node;
}

NDPtr new_assgin(TKPtr tok, NDPtr lexpr, NDPtr rexpr) {
    NDPtr node = new_node(tok, ND_ASSIGN);
    if(is_integer(lexpr->type))
        assert(is_integer(rexpr->type));
    else
        assert(type_equal(lexpr->type, rexpr->type) || (rexpr->kind == ND_NUM && rexpr->val == 0));    
    node->lexpr = lexpr;
    node->rexpr = rexpr;
    node->type = lexpr->type;
    return node;
}

NDPtr new_equal(TKPtr tok, NodeKind kind, NDPtr lexpr, NDPtr rexpr) {
    NDPtr node = new_node(tok, kind);
    assert(type_equal(lexpr->type, rexpr->type));
    node->lexpr = lexpr;
    node->rexpr = rexpr;
    node->type = int_type();
    return node;
}

NDPtr new_var(TKPtr tok, VarPtr var) {
    NDPtr node = new_node(tok, ND_VAR);
    node->var = var;
    node->type = var->type;
    return node;
}

NDPtr new_cast(TKPtr tok, NDPtr ori, TYPtr ty) {
    NDPtr node = new_node(tok, ND_TYPE_CAST);
    node->lexpr = ori;
    node->type = ty;
    return node;
}

// 判断当前 token 是否为指定保留字
bool expect_reserved(const char *s) {
    return token_->kind == TK_RESERVED && strlen(s) == token_->len && !strncmp(token_->str, s, token_->len);
}

bool expect_ident() {
    return token_->kind == TK_IDENT;
}

bool expect_int_literal() {
    return token_->kind == TK_NUM;
}

// 尝试解析一个数字字面量的 token 出来，失败则返回 NULL,成功返回对应 token
// parser_xxx 都对应一个终结符的解析
TKPtr parse_int_literal(int &val) {
    if(!expect_int_literal())
        return NULL;
    val = token_->val;
    next_token();
    return last_token();
}

TKPtr parse_reserved(const char* s) {
    if(!expect_reserved(s))
        return NULL;
    next_token();
    return last_token();
}

TKPtr parse_ident(char* &name) {
    if(!expect_ident())
        return NULL;
    name = new char[256];
    assert(token_->len < 256);
    strncpy(name, token_->str, token_->len);
    name[token_->len] = 0;

    next_token();
    return last_token();
}

VarPtr search_varlist(char* name, std::list<VarPtr> &list) {
    int len = strnlen(name, 1000);
    for(auto v = list.begin(); v != list.end(); ++v) {
        VarPtr var = *v;
        if(strnlen(var->name, 1000) == len && !strncmp(var->name, name, len))
            return var;
    }
    return NULL;
}   

VarPtr find_local_var(char* name) {
    VarPtr var;
    if(var = search_varlist(name, lvar_stack)) {
        return var;
    }
    return search_varlist(name, hot_func->args);
}

VarPtr find_global_var(char* name) {
    return search_varlist(name, global_vars);
}

VarPtr find_var(char* name) {
    VarPtr var;
    if(var = find_local_var(name))
        return var;
    return find_global_var(name);
}

FNPtr find_func(char* name) {
    int len = strnlen(name, 1000);
    // 其实查找顺序无所谓，目前不支持局部函数
    for(auto f = funcs.rbegin(); f != funcs.rend(); ++f) {
        FNPtr func = *f;
        if(strnlen(func->name, 1000) == len && !strncmp(func->name, name, len))
            return func;
    }
    return NULL;
}

NDPtr expr();

TYPtr parse_basetype() {
    TYPtr ty = NULL;
    if(parse_reserved("int"))
        ty = int_type();
    return ty;
}

// 尝试解析一个非终结符，由于目前没有 type system, 返回值仅表示成功或者失败
// 类似名称的函数与 NodeKind 基本一一对应，与非终结符大致一一对应
TYPtr type() {
    TYPtr ty = parse_basetype();
    if(!ty)
        return NULL;
    while(parse_reserved("*")) {
        ty = pointer_to(ty);
    }
    return ty;
}

// 对应 Integer，返回一个 TK_NUM 的节点
NDPtr num() {
    NDPtr node = NULL; 
    int val; 
    TKPtr tok;
    
    if(tok = parse_int_literal(val)) {
        node = new_num(tok, val);
    }
    return node;
}

std::shared_ptr<FuncCall> func_call(char* name) {
    std::shared_ptr<FuncCall> fc = std::make_shared<FuncCall>();
    fc->name = name;
    parse_reserved("(");
    if(parse_reserved(")"))
        return fc;
    fc->args.push_back(expr());
    while(!parse_reserved(")")) {
        assert(parse_reserved(","));
        fc->args.push_back(expr());
    }
    return fc;
}

// 对应同名非终结符
NDPtr primary() {
    TKPtr tok;
    if (parse_reserved("(")) {
        NDPtr fac = expr();
        parse_reserved(")");
        return fac;
    }
    char* name;
    if (tok = parse_ident(name)) {
        // function call
        if(parse_reserved("(")) {
            NDPtr node = new_node(tok, ND_FUNC_CALL);
            assert(node->func_call = func_call(name));
            // 调用未声明函数
            FNPtr fn;
            assert(fn = find_func(node->func_call->name));
            // 参数数量相同
            assert(fn->args.size() == node->func_call->args.size());
            // 参数类型相同
            auto arg = fn->args.begin();
            auto call = node->func_call->args.begin();
            for(; arg != fn->args.end(); ++arg, ++call) {
                assert(type_equal((*arg)->type, (*call)->type));
            }
            node->type = fn->ret_type;
            return node;
        }
        return new_var(tok, find_var(name));
    }
    return num();
}

// 对应同名非终结符
NDPtr postfix() {
    NDPtr node = primary();
    if(node && (node->type->kind == TY_ARR || node->type->kind == TY_PTR) && expect_reserved("[")) {
        TKPtr tok = node->tok;
        NDPtr postfix = node->type->kind == TY_ARR ? new_node(tok, ND_ARR_INDEX) : new_node(tok, ND_PTR_INDEX);
        postfix->lexpr = node;
        while(parse_reserved("[")) {
            postfix->arr_index.push_back(expr());
            parse_reserved("]");
        }
        TYPtr ty = node->type;
        for(int i = 0; i < postfix->arr_index.size(); i++) {
            ty = ty->base;
            assert(ty);
        }
        postfix->type = ty;
        return postfix;
    }
    return node;
}

// 对应同名非终结符
NDPtr unary() {
    TKPtr tok;
    if (tok = parse_reserved("-"))
        return new_unary(tok, ND_NEG, unary());
    if (tok = parse_reserved("!"))
        return new_unary(tok, ND_NOT, unary());
    if (tok = parse_reserved("~"))
        return new_unary(tok, ND_BITNOT, unary());
    // 这些需要特殊的类型判断
    if (tok = parse_reserved("*"))
        return new_unary_ptr(tok, ND_DEREF, unary());
    if (tok = parse_reserved("&"))
        return new_unary_ptr(tok, ND_REF, unary());
    // type-cast，这里作弊了，非 LL1
    if (tok = parse_reserved("(")) {
        TYPtr ty;
        if(ty = type()) {
            assert(parse_reserved(")"));
            NDPtr node = new_cast(tok, unary(), ty);
            return node;
        }
        checkout_token();
    }   
    return postfix();
}

// 对应同名非终结符
NDPtr multiplicative() {
    TKPtr tok;
    NDPtr node = unary();
    while(1) {
        if (tok = parse_reserved("*"))
            node = new_binary(tok, ND_MUL, node, unary());
        else if (tok = parse_reserved("/"))
            node = new_binary(tok, ND_DIV, node, unary());
        else if (tok = parse_reserved("%"))
            node = new_binary(tok, ND_MOD, node, unary());
        else 
            break;
    }
    return node;
}

// 还不能指针加减，这就把好多任务推到 lab1d2 了
// 对应同名非终结符
NDPtr additive() {
    TKPtr tok;
    NDPtr node = multiplicative();
    while(1) {
        if (tok = parse_reserved("+"))
            node = new_add(tok, node, multiplicative());
        else if (tok = parse_reserved("-"))
            node = new_sub(tok, node, multiplicative());
        else 
            break;
    }
    return node;
}

// 对应同名非终结符
NDPtr relational() {
    TKPtr tok;
    NDPtr node = additive();
    if (tok = parse_reserved(">"))
        return new_binary(tok, ND_LT, additive(), node);
    if (tok = parse_reserved(">="))
        return new_binary(tok, ND_LTE, additive(), node);
    if (tok = parse_reserved("<"))
        return new_binary(tok, ND_LT, node, additive());
    if (tok = parse_reserved("<="))
        return new_binary(tok, ND_LTE, node, additive());
    return node;
}

// 对应同名非终结符
NDPtr equality() {
    TKPtr tok;
    NDPtr node = relational();
    if (tok = parse_reserved("=="))
        return new_equal(tok, ND_EQ, node, relational());
    if (tok = parse_reserved("!="))
        return new_equal(tok, ND_NEQ, node, relational());
    return node;
}

// 对应同名非终结符
NDPtr logand() {
    TKPtr tok;
    NDPtr node = equality();
    while(tok = parse_reserved("&&")) {
        node = new_binary(tok, ND_LOGAND, node, equality());
    }
    return node;
}

// 对应同名非终结符
NDPtr logor() {
    TKPtr tok;
    NDPtr node = logand();
    while(tok = parse_reserved("||")) {
        node = new_binary(tok, ND_LOGOR, node, logand());
    }
    return node;
}

NDPtr conditional() {
    TKPtr tok;
    NDPtr node = logor();
    if(!(tok = parse_reserved("?")))
        return node;
    NDPtr tern = new_node(tok, ND_TERNARY);
    tern->cond = node;
    tern->then = expr();
    assert(parse_reserved(":"));
    tern->els = conditional();
    tern->type = tern->cond->type;
    return tern;
}

NDPtr assign() {
    TKPtr tok;
    NDPtr node = conditional();
    while(tok = parse_reserved("=")) {
        node = new_assgin(tok, node, assign());
    }
    return node;
}

NDPtr expr() {
    return assign();
}

#define max(a, b) (a > b ? a : b)

void add_local(VarPtr var, TKPtr tok) {
    VarPtr v;
    // 注意更改变量重定义的条件
    if((v = find_local_var(var->name)) && v->scope_depth == scope_depth) {
        error_tok(tok, "variable redefined\n");
    }
    var->offset = stack_size;
    // 注意 push 顺序和查找顺序的对应
    lvar_stack.push_front(var);
    lvar_stack_depth.push_front(scope_depth);
    stack_size += var->type->size / POINTER_WIDTH;
    max_stack_size = max(max_stack_size, stack_size);
}

TYPtr suffix(TYPtr base) {
    if(parse_reserved("[")) {
        int len;
        assert(parse_int_literal(len));
        assert(parse_reserved("]"));
        TYPtr ty = suffix(base);
        return arr_of(ty, len);
    }
    return base;
}

NDPtr declaration(TYPtr ty) {
    TKPtr tok;
    char* name;
    // type() 必须在之前完成
    tok = parse_ident(name);
    ty = suffix(ty);
    VarPtr var = std::make_shared<Var>();
    var->name = name;
    var->scope_depth = scope_depth;
    var->tok = tok;
    var->type = ty;
    add_local(var, tok);
    if (tok = parse_reserved("=")) {
        var->init = expr();
    }
    assert(parse_reserved(";"));
    NDPtr node = new_stmt(tok, ND_DECL, NULL);
    node->var = var;
    return node;
}

NDPtr compound_stmt();
NDPtr block_item();

// 对应非终结符 statement
NDPtr stmt() {
    NDPtr node = NULL;
    TKPtr tok;
    TYPtr ty;
    // Return statement
    if (tok = parse_reserved("return")) {   
        node = new_stmt(tok, ND_RETURN, expr());
        assert(node->lexpr);
        assert(parse_reserved(";"));
        return node;
    }
    // IF statement
    if (tok = parse_reserved("if")) {
        assert(parse_reserved("("));
        node = new_node(tok, ND_IF);
        node->cond = expr();
        assert(parse_reserved(")"));
        node->then = stmt();
        if(parse_reserved("else"))
            node->els = stmt();
        return node;
    }
    // Compound stmt
    if(expect_reserved("{"))
        return compound_stmt();
    // For statement
    if (tok = parse_reserved("for")) {
        NDPtr node = new_node(tok, ND_FOR);
        assert(parse_reserved("("));
        push_scope();
        if(ty = type())
            node->init = declaration(ty);
        else {
            // assign 语句是比较容易出错的一个语句，如果有不使用的 expr，一定要使用 UNUSED_EXPR 包装
            node->init = new_stmt(tok, ND_UNUSED_EXPR, expr());
            assert(parse_reserved(";"));
        }
        if (!parse_reserved(";")) {
            node->cond = expr();
            assert(parse_reserved(";"));
        }
        if (!parse_reserved(")")) {
            // 使用 UNUSED_EXPR 包装，弹出无用的值
            NDPtr inc = expr();
            node->inc = new_stmt(inc->tok, ND_UNUSED_EXPR, inc);
            assert(parse_reserved(")"));
        }
        push_scope();
        node->then = stmt();
        pop_scope();
        pop_scope();
        return node;
    }
    // do-while statement
    if (tok = parse_reserved("do")) {
        NDPtr node = new_node(tok, ND_DOWHILE);
        node->then = stmt();
        assert(parse_reserved("while"));
        assert(parse_reserved("("));
        node->cond = expr();
        assert(parse_reserved(")"));
        assert(parse_reserved(";"));
        return node;
    }
    // while-do statement
    if (tok = parse_reserved("while")) {
        NDPtr node = new_node(tok, ND_WHILEDO);
        assert(parse_reserved("("));
        node->cond = expr();
        assert(parse_reserved(")"));
        node->then = stmt();
        return node;
    }
    // Break
    if (tok = parse_reserved("break")) {
        assert(parse_reserved(";"));
        return new_node(tok, ND_BREAK);
    }
    // Continue
    if (tok = parse_reserved("continue")) {
        assert(parse_reserved(";"));
        return new_node(tok, ND_CONTINUE);
    }
    // Unused expr
    node = expr();
    tok = node == NULL ? token_ : node->tok;
    if(!parse_reserved(";")) {
        error_tok(tok, "unknown error\n");
    }
    return new_stmt(tok, ND_UNUSED_EXPR, node);
}

// 对应非终结符 statement
NDPtr block_item() {
    TYPtr ty;
    // Local var declaration
    if (ty = type()) {
        return declaration(ty);
    }
    return stmt();
}

NDPtr compound_stmt() {
    TKPtr tok;
    // Compound statement
    if (tok = parse_reserved("{")) {
        std::list<NDPtr> block_stmts;
        push_scope();
        while (!parse_reserved("}")) {
            block_stmts.push_back(block_item());
        }
        pop_scope();
        NDPtr node = new_node(tok, ND_BLOCK);
        node->body = std::move(block_stmts);
        return node;
    }
    return NULL;
}

void decl_func_arg(std::list<VarPtr> &args) {
    TYPtr ty;
    assert(ty = type());
    VarPtr var = std::make_shared<Var>();
    var->is_arg = true;
    var->type = ty;
    parse_ident(var->name);
    // assert(var->name);           //声明中参数可能没有名称
    var->offset = args.size();
    args.push_back(var);
}

int decl_func_args(std::list<VarPtr> &args) {
    assert(parse_reserved("("));
    if (parse_reserved(")")) {
        return 0;
    }
    decl_func_arg(args);
    while (!parse_reserved(")")) {
        assert(parse_reserved(","));
        decl_func_arg(args);
    }
    return 0;
}

// 如果函数不存在，则加入 funcs 并返回 NULL，否则直接返回同名的函数
FNPtr add_func(FNPtr func) {
    FNPtr f;
    if(f = find_func(func->name)) {
        return f;
    }
    funcs.push_back(func);
    return NULL;
}

// 对应非终结符 function
FNPtr function(char* name, TYPtr ty, TKPtr tok) {
    // type()
    // parse_ident() 已经在外部完成
    FNPtr fn = std::make_shared<Function>();
    fn->name = name;
    fn->tok = tok;
    fn->ret_type = ty;
    // 解析函数参数
    decl_func_args(fn->args);
    FNPtr f;
    // add_func 返回不是 NULL，表明已经有过声明
    if(f = add_func(fn)) {
        // 已有定义，则此项必须是一个声明。
        if(f->is_complete) {
            assert(parse_reserved(";"));
            return NULL;
        }
        // 仅有声明，必须有相同的参数（目前只要求数量相同）
        assert(f->args.size() == fn->args.size());
        // 可能有参数重命名，以现有函数参数为准
        f->args.clear();
        f->args = std::move(fn->args);
        fn = f;
    }
    // 这仅是一个声明，返回 NULL
    if(parse_reserved(";")) {
        return NULL;
    }
    fn->is_complete = true;
    hot_func = fn;
    fn->stmts = compound_stmt();
    fn->stack_size = (max_stack_size + 2) * 4;
    return fn;
}

bool is_func() {
    // 判断是函数还是全局变量：看 ident 之后是否紧跟 '('
    return expect_reserved("(");
}

VarPtr global_var(char* name, TYPtr ty, TKPtr tok) {
    // 不允许重定义
    assert(find_global_var(name) == NULL);
    ty = suffix(ty);
    VarPtr gvar = std::make_shared<Var>();
    gvar->name = name;
    gvar->tok = tok;
    gvar->is_global = true;
    gvar->type = ty;
    // 全局变量只允许使用字面量优化
    if(parse_reserved("=")) {
        gvar->init = num();
    }
    assert(parse_reserved(";"));
    return gvar;
}

// 顶层解析函数，其实也对应 program 的解析 
Program* parsing(std::list<TKPtr>* tokens) {
    toks_ = tokens;
    next_token();
    Program* prog = new Program();
    TKPtr tok;
    TYPtr ty;
    while (token_->kind != TK_EOF) {
        assert(ty = type());
        char *name;
        assert(tok = parse_ident(name));
        if(is_func()) {
            FNPtr fn = function(name, ty, tok);
            // 遇到声明先不加入函数列表
            if(!fn)
                continue;
            prog->funcs.push_back(fn);
            assert(scope_depth == 0);
            lvar_stack.clear();
            lvar_stack_depth.clear();
            max_stack_size = 0;
            stack_size = 0;
            hot_func = NULL;
        } else {
            VarPtr gvar = global_var(name, ty, tok);
            global_vars.push_back(gvar);
        }
    }
    prog->gvars = std::move(global_vars);
    return prog;
}
