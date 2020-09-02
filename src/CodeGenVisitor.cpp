#include "CodeGenVisitor.h"

// @brief: Start visiting the syntax tree from root node Prog
// @ret: Generated asm code
antlrcpp::Any CodeGenVisitor::visitProg(MiniDecafParser::ProgContext *ctx) {
    code_ << ".section .text\n"
        << ".globl main\n"
        << "main:\n"; 
    visitChildren(ctx);
    return code_.str();
}

// @brief: Visit ReturnStmt node, son of Stmt node
antlrcpp::Any CodeGenVisitor::visitReturnStmt(MiniDecafParser::ReturnStmtContext *ctx) {
    visitChildren(ctx);
    code_ << "\tret";
    return retType::UNDEF;
}

// @brief: Visit Expr node, which is a single Integer node in this step
// @ret: Expr type
antlrcpp::Any CodeGenVisitor::visitExpr(MiniDecafParser::ExprContext *ctx) {
    
    code_ << "\tli a0, " << ctx->Interger()->getText() << "\n"
          << push;
    return retType::INT;
}