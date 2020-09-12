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

void gen_push(const char* reg) {
    debug("PUSH %s\n", reg);
    printf("  addi sp, sp, -%d\n", POINTER_WIDTH);
    printf("  sw %s, 0(sp)\n", reg);
}

void gen_pop(const char* reg) {
    debug("POP %s\n", reg);
    printf("  lw %s, 0(sp)\n", reg);
    printf("  addi sp, sp, %d\n", POINTER_WIDTH);
}

void gen(NDPtr node) {
    if(!node) 
        return;
    switch (node->kind) {
    case ND_RETURN:
        debug("RETURN\n");
        gen(node->expr);
        gen_pop("a0");
        printf("  ret\n");
        return;
    case ND_NUM:
        debug("NUM\n");
        printf("  li t0, %d\n", node->val);
        gen_push("t0");
        return;
    default:
        assert(false);
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
