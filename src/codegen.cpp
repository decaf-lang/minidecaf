#include <cassert>
#include <cstdio>
#include <cstdarg>
#include "codegen.h"

static bool debug_ = false;

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
    default:
        assert(false);
    }
    push("t0");
    debug("BINARY END\n");
}

void gen(NDPtr node) {
    if(!node) 
        return;
    switch (node->kind) {
    case ND_RETURN:
        debug("RETURN\n");
        gen(node->lexpr);
        pop("a0");
        printf("  ret\n");
        return;
    case ND_NUM:
        debug("NUM\n");
        printf("  li t0, %d\n", node->val);
        push("t0");
        return;
    case ND_NOT:
        debug("NOT\n");
        gen(node->lexpr);
        pop("t0");
        printf("  seqz t0, t0\n");
        push("t0");
        return;
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
    default:
        gen_binary(node);
    }
}

void gen_text(std::list<FNPtr> &func) {
    printf("  .text\n");
    assert(func.size() == 1);
    for (auto f = func.begin(); f != func.end(); ++f) {
        FNPtr fn = *f;
        printf("  .global %s\n", fn->name);
        printf("%s:\n", fn->name);
        // Emit code
        for (auto n = fn->nodes.begin(); n != fn->nodes.end(); ++n)
            gen(*n);
    }
}

void codegen(Program* prog, bool debug) {
    debug_ = debug;
    gen_text(prog->funcs);
}
