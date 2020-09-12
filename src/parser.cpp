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
// 某个函数全部的局部变量
static std::list<VarPtr> local_vars;

void next_token() {   
    used_toks_.push_back(token_);
    assert(!toks_->empty());     
    token_ = toks_->front();   
    toks_->pop_front();
}

inline TKPtr last_token() {
    assert(!used_toks_.empty());
    return used_toks_.back();
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
    node->lexpr = expr;
    return node;
}

NDPtr new_num(TKPtr tok, int val) {
    NDPtr node = new_node(tok, ND_NUM);
    node->val = val;
    return node;
}

NDPtr new_stmt(TKPtr tok, NodeKind kind, NDPtr expr) {
    NDPtr node = new_node(tok, kind);
    node->lexpr = expr;
    return node;
}

NDPtr new_binary(TKPtr tok, NodeKind kind, NDPtr lexpr, NDPtr rexpr) {
    NDPtr node = new_node(tok, kind);
    node->lexpr = lexpr;
    node->rexpr = rexpr;
    return node;
}

NDPtr new_var(TKPtr tok, VarPtr var) {
    NDPtr node = new_node(tok, ND_VAR);
    node->var = var;
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

VarPtr find_local_var(char* name) {
    int len = strnlen(name, 1000);
    for(auto v = local_vars.begin(); v != local_vars.end(); ++v) {
        VarPtr var = *v;
        if(strnlen(var->name, 1000) == len && !strncmp(var->name, name, len))
            return var;
    }
    return NULL;
}

NDPtr expr();

// 尝试解析一个非终结符，由于目前没有 type system, 返回值仅表示成功或者失败
// 类似名称的函数与 NodeKind 基本一一对应，与非终结符大致一一对应
bool type() {
    if(parse_reserved("int")) {
        return true;
    }
    return false;
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
        return new_var(tok, find_local_var(name));
    }
    return num();
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
    return primary();
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

// 对应同名非终结符
NDPtr additive() {
    TKPtr tok;
    NDPtr node = multiplicative();
    while(1) {
        if (tok = parse_reserved("+"))
            node = new_binary(tok, ND_ADD, node, multiplicative());
        else if (tok = parse_reserved("-"))
            node = new_binary(tok, ND_SUB, node, multiplicative());
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
        return new_binary(tok, ND_EQ, node, relational());
    if (tok = parse_reserved("!="))
        return new_binary(tok, ND_NEQ, node, relational());
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

NDPtr assign() {
    TKPtr tok;
    NDPtr node = logor();
    while(tok = parse_reserved("=")) {
        node = new_binary(tok, ND_ASSIGN, node, assign());
    }
    return node;
}

NDPtr expr() {
    return assign();
}

void add_local(VarPtr var, TKPtr tok) {
    if(find_local_var(var->name)) {
        error_tok(tok, "variable redefined\n");
    }
    var->offset = local_vars.size();
    local_vars.push_front(var);
}

NDPtr declaration() {
    TKPtr tok;
    char* name;
    // type() 已经在 stmt() 中完成
    tok = parse_ident(name);
    VarPtr var = std::make_shared<Var>();
    var->name = name;
    var->tok = tok;
    add_local(var, tok);
    if (tok = parse_reserved("=")) {
        var->init = expr();
    }
    assert(parse_reserved(";"));
    NDPtr node = new_stmt(tok, ND_DECL, NULL);
    node->var = var;
    return node;
}

// 对应非终结符 statement
NDPtr stmt() {
    NDPtr node = NULL;
    TKPtr tok;
    // Return statement
    if (tok = parse_reserved("return")) {   
        node = new_stmt(tok, ND_RETURN, expr());
        assert(node->lexpr);
        assert(parse_reserved(";"));
        return node;
    }
    // Local var declaration
    if (type()) {
        return declaration();
    }
    // Unused expr
    node = expr();
    tok = node == NULL ? token_ : node->tok;
    assert(parse_reserved(";"));
    return new_stmt(tok, ND_UNUSED_EXPR, node);
}

// 对应非终结符 function
FNPtr function() {
    assert(type());
    char *name;
    assert(parse_ident(name));
    FNPtr fn = std::make_shared<Function>();
    fn->name = name;

    assert(parse_reserved("("));
    assert(parse_reserved(")"));
    assert(parse_reserved("{"));

    // Read function body
    while (!parse_reserved("}")) {
        fn->stmts.push_back(stmt());
    }
    fn->locals = std::move(local_vars);
    fn->stack_size = (fn->locals.size() + 2) * 4;
    return fn;
}

// 顶层解析函数，其实也对应 program 的解析 
Program* parsing(std::list<TKPtr>* tokens) {
    toks_ = tokens;
    next_token();
    Program* prog = new Program();
    FNPtr fn = function();
    prog->funcs.push_back(fn);
    assert(token_->kind == TK_EOF);
    return prog;
}
