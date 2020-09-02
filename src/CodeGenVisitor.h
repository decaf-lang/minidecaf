#pragma once

#include "MiniDecafBaseVisitor.h"
#include <string>
#include <iostream>

class CodeGenVisitor : public MiniDecafBaseVisitor {
public:
    antlrcpp::Any visitProg(MiniDecafParser::ProgContext *ctx);
    antlrcpp::Any visitReturnStmt(MiniDecafParser::ReturnStmtContext *ctx);
    antlrcpp::Any visitExpr(MiniDecafParser::ExprContext *ctx);

private:
    /*
        Stringstream used to store generated codes
    */
    std::ostringstream code_;
    /* 
        A simple stack machine model 
        Support basic push, pop1 & pop2 operations
    */
    const char* push = "\taddi sp, sp, -4\n"
                       "\tsw a0, (sp)\n";
    const char* pop2 = "\tlw t0, (sp)\n"
                       "\tlw t1, 4(sp)\n"
                       "\taddi sp, sp, 8\n";
    const char* pop1 = "\tlw a0, (sp)\n"
                       "\taddi sp, sp, 4\n";
    /*
        Specify return type of each operation
    */
    enum retType {UNDEF, INT};
};