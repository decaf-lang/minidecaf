#include "Allocator.h"

//@brief: Visit the root node of the generated syntax tree
antlrcpp::Any Allocator::visitProg(MiniDecafParser::ProgContext *ctx) {
    offset = 0;
    visitChildren(ctx);
    return varTab;
}

antlrcpp::Any Allocator::visitFunc(MiniDecafParser::FuncContext *ctx) {
    // Update the function scope
    curFunc = ctx->Identifier()->getText();
    visitChildren(ctx);
    return retType::INT;
}

antlrcpp::Any Allocator::visitVarDef(MiniDecafParser::VarDefContext *ctx) {
    // allocate the defined varible
    std::string varName = ctx->Identifier()->getText();
    varTab[curFunc][varName] = offset++;
    return retType::INT;
}
