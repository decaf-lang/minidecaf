#include "Allocator.h"


antlrcpp::Any Allocator::visitProg(MiniDecafParser::ProgContext *ctx) {
    visitChildren(ctx);
    return varTab;
}

antlrcpp::Any Allocator::visitFunc(MiniDecafParser::FuncContext *ctx) {
    curFunc = ctx->Identifier()->getText();
    visitChildren(ctx);
    return retType::INT;
}

antlrcpp::Any Allocator::visitVarDef(MiniDecafParser::VarDefContext *ctx) {
    std::string varName = ctx->Identifier()->getText();
    varTab[curFunc][varName] = offset++;
    return retType::INT;
}
