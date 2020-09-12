#include <cassert>
#include <cstdio>
#include <cstdarg>
#include "codegen.h"

static bool debug_ = false;
const char* FUNC_EXIT = "function_exit";
const char* ELSE = "else";
const char* THEN = "then";
const char* EXIT = "exit";
static FNPtr hot_func;
static NDPtr last_node;
static int label_seq;

void debug(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    if(debug_)
        printf(fmt, ap);
}

void push(const char* reg) {
    debug("PUSH %s\n", reg);
    printf("  addi sp, sp, -%d\n", POINTER_WIDTH);
    printf("  sw %s, 0(sp)\n", reg);
}

void pop(const char* reg) {
    debug("POP %s\n", reg);
    printf("  lw %s, 0(sp)\n", reg);
    printf("  addi sp, sp, %d\n", POINTER_WIDTH);
}

inline int var_offset(VarPtr var) {
    return -(3+var->offset);
}

inline void store(const char* reg, int offset) {
    printf("  sw %s, %d(fp)\n", reg, offset * POINTER_WIDTH);
}

inline void load(const char* reg, int offset) {
    printf("  lw %s, %d(fp)\n", reg, offset * POINTER_WIDTH);
}

void store_var(const char* reg, VarPtr var) {
    store(reg, var_offset(var));
}

void load_var(const char* reg, VarPtr var) {
    load(reg, var_offset(var));
}

inline void label(const char* type, int seq) {
    printf(".L.%s.%d:\n", type, seq);
}

inline void jmp(const char* type, int seq) {
    printf("  j .L.%s.%d\n", type, seq);
}

inline void beqz(const char* reg, const char* type, int seq) {
    printf("  beqz %s, .L.%s.%d\n", reg, type, seq);
}

inline void bnez(const char* reg, const char* type, int seq) {
    printf("  bnez %s, .L.%s.%d\n", reg, type, seq);
}

inline bool check_lvalue(NDPtr node) {
    return node->kind == ND_VAR;
}

void gen(NDPtr node);

void gen_binary(NDPtr node) {
    debug("BINARY\n");
    gen(node->lexpr);
    gen(node->rexpr);
    pop("t1");
    pop("t0");
    switch (node->kind) {
    case ND_ADD:
        printf("  add t0, t0, t1\n");
        break;
    case ND_SUB:
        printf("  sub t0, t0, t1\n");
        break;
    case ND_MUL:
        printf("  mul t0, t0, t1\n");
        break;
    case ND_DIV:
        printf("  div t0, t0, t1\n");
        break;
    case ND_MOD:
        printf("  rem t0, t0, t1\n");
        break;
    case ND_LT:
        printf("  slt t0, t0, t1\n");
        break;        
    case ND_LTE:
        printf("  sgt t0, t0, t1\n");
        printf("  seqz t0, t0\n");
        break;        
    case ND_EQ:
        printf("  sub t0, t0, t1\n");
        printf("  seqz t0, t0\n");
        break;        
    case ND_NEQ: 
        printf("  sub t0, t0, t1\n");
        printf("  snez t0, t0\n");
        break;       
    case ND_LOGAND:
        printf("  snez t0, t0\n");
        printf("  snez t1, t1\n");
        printf("  and t0, t0, t1\n");
        break;
    case ND_LOGOR:
        printf("  or t0, t0, t1\n");
        printf("  snez t0, t0\n");
        break;
    default:
        assert(false);
    }
    push("t0");
    debug("BINARY END\n");
}

void gen(NDPtr node) {
    if(!node) 
        return;
    int seq;
    switch (node->kind) {
    // Statement
    case ND_RETURN:
        debug("RETURN\n");
        gen(node->lexpr);
        pop("a0");
        printf("  j .L.%s.%s\n", FUNC_EXIT, hot_func->name);
        break;
    case ND_DECL:
        debug("DECL\n");
        assert(node->var);
        if(node->var->init) {
            gen(node->var->init);
            pop("t0");
            store_var("t0", node->var);
        }
        break;
    case ND_UNUSED_EXPR:    
        debug("UNUSED\n");
        gen(node->lexpr);
        pop("t0");
        break;
    case ND_IF: 
        debug("ND_IF");
        seq = label_seq++;
        gen(node->cond);
        pop("t0");
        if (node->els) {
            beqz("t0", ELSE, seq);
            gen(node->then);
            jmp(EXIT, seq);
            label(ELSE, seq);
            gen(node->els);
        } else {
            beqz("t0", EXIT, seq);
            gen(node->then);
        }
        label(EXIT, seq);
        break;
    case ND_BLOCK:
        for (auto n = node->body.begin(); n != node->body.end(); ++n)
            gen(*n);
        break;
    // Expression
    case ND_NUM:
        debug("NUM\n");
        printf("  li t0, %d\n", node->val);
        push("t0");
        break;
    case ND_NOT:
        debug("NOT\n");
        gen(node->lexpr);
        pop("t0");
        printf("  seqz t0, t0\n");
        push("t0");
        break;
    case ND_BITNOT:
        debug("BITNOT\n");
        gen(node->lexpr);
        pop("t0");
        printf("  not t0, t0\n");
        push("t0");
        break;
    case ND_NEG:
        debug("NEG\n");
        gen(node->lexpr);
        pop("t0");
        printf("  neg t0, t0\n");
        push("t0");
        break;
    case ND_VAR:
        debug("VAR\n");
        assert(node->var);
        load_var("t0", node->var);
        push("t0");
        break;
    case ND_ASSIGN:
        debug("ASSIGN\n");
        assert(check_lvalue(node->lexpr));
        // Left expr must be a variable
        gen(node->rexpr);
        pop("t0");
        store_var("t0", node->lexpr->var);
        push("t0");
        break;
    case ND_TERNARY:
        debug("TERNARY\n");
        seq = label_seq++;
        gen(node->cond);
        pop("t0");
        beqz("t0", ELSE, seq);
        gen(node->then);
        jmp(EXIT, seq);
        label(ELSE, seq);
        gen(node->els);
        label(EXIT, seq);
        break;
    default:
        gen_binary(node);
    }
    last_node = node;
}

void gen_text(std::list<FNPtr> &func) {
    printf("  .text\n");
    assert(func.size() == 1);
    for (auto f = func.begin(); f != func.end(); ++f) {
        FNPtr fn = hot_func = *f;
        last_node = NULL;
        printf("  .global %s\n", fn->name);
        printf("%s:\n", fn->name);
        // Prelogue
        printf("  addi sp, sp, -%d\n", fn->stack_size);
        printf("  sw ra, %d-4(sp)\n", fn->stack_size);
        printf("  sw fp, %d-8(sp)\n", fn->stack_size);
        printf("  addi fp, sp, %d\n", fn->stack_size);

        gen(fn->stmts);
        
        // Missing return
        if(!last_node || last_node->kind != ND_RETURN)
            printf("  li a0, 0\n");
        // Epilogue
        printf(".L.%s.%s:\n", FUNC_EXIT, fn->name);
        printf("  lw fp, %d-8(sp)\n", fn->stack_size);
        printf("  lw ra, %d-4(sp)\n", fn->stack_size);
        printf("  addi sp, sp, %d\n", fn->stack_size);
        printf("  ret\n");
    }
}

void codegen(Program* prog, bool debug) {
    debug_ = debug;
    gen_text(prog->funcs);
}
