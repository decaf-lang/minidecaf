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
static int max_stack_size = 0;
// 当前作用域嵌套的深度
static int scope_depth = 0;

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

inline void push_scope() {
    ++scope_depth;
}

inline void pop_scope() {
    --scope_depth;
    while(!lvar_stack.empty() && lvar_stack_depth.front() > scope_depth) {
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
    return search_varlist(name, lvar_stack);
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
    return tern;
}

NDPtr assign() {
    TKPtr tok;
    NDPtr node = conditional();
    while(tok = parse_reserved("=")) {
        node = new_binary(tok, ND_ASSIGN, node, assign());
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
    var->offset = lvar_stack.size();
    // 注意 push 顺序和查找顺序的对应
    lvar_stack.push_front(var);
    lvar_stack_depth.push_front(scope_depth);
    max_stack_size = max(max_stack_size, lvar_stack.size());
}

NDPtr declaration() {
    TKPtr tok;
    char* name;
    // type() 必须在之前完成
    tok = parse_ident(name);
    VarPtr var = std::make_shared<Var>();
    var->name = name;
    var->scope_depth = scope_depth;
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

NDPtr compound_stmt();
NDPtr block_item();

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
        if(type())
            node->init = declaration();
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
    // Local var declaration
    if (type()) {
        return declaration();
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
// 对应非终结符 function
FNPtr function() {
    assert(type());
    char *name;
    assert(parse_ident(name));
    FNPtr fn = std::make_shared<Function>();
    fn->name = name;

    assert(parse_reserved("("));
    assert(parse_reserved(")"));
    fn->stmts = compound_stmt();
    fn->stack_size = (max_stack_size + 2) * 4;
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
