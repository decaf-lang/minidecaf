#pragma once

#include "MiniDecafBaseVisitor.h"
#include <string>
#include <iostream>

template<typename T>
using symTab = std::unordered_map<std::string, std::unordered_map<std::string, T> >;

// Code generation pass
class CodeGenVisitor : public MiniDecafBaseVisitor {
public:
    antlrcpp::Any visitProg(MiniDecafParser::ProgContext *ctx, symTab<int>& symbol_);
    antlrcpp::Any visitFunc(MiniDecafParser::FuncContext *ctx);
    antlrcpp::Any visitReturnStmt(MiniDecafParser::ReturnStmtContext *ctx);
    antlrcpp::Any visitIfStmt(MiniDecafParser::IfStmtContext *ctx);
    antlrcpp::Any visitCondExpr(MiniDecafParser::CondExprContext *ctx);

    antlrcpp::Any visitUnaryOp(MiniDecafParser::UnaryOpContext *ctx);
    antlrcpp::Any visitAtomParen(MiniDecafParser::AtomParenContext *ctx);
    antlrcpp::Any visitAddSub(MiniDecafParser::AddSubContext *ctx);
    antlrcpp::Any visitMulDiv(MiniDecafParser::MulDivContext *ctx);
    antlrcpp::Any visitEqual(MiniDecafParser::EqualContext *ctx);
    antlrcpp::Any visitLessGreat(MiniDecafParser::LessGreatContext *ctx);
    antlrcpp::Any visitLand(MiniDecafParser::LandContext *ctx);
    antlrcpp::Any visitLor(MiniDecafParser::LorContext *ctx);

    antlrcpp::Any visitInteger(MiniDecafParser::IntegerContext *ctx);
    antlrcpp::Any visitIdentifier(MiniDecafParser::IdentifierContext *ctx);
    antlrcpp::Any visitVarDef(MiniDecafParser::VarDefContext *ctx);
    antlrcpp::Any visitAssign(MiniDecafParser::AssignContext *ctx);


private:
    /*
        Stringstream used to store generated codes
    */
    std::ostringstream code_;

    std::string curFunc;
    symTab<int> varTab;
    bool retState;
    int labelOrder;
    /* 
        A simple stack machine model 
        Support basic push, pop1 & pop2 operations
    */
    const char* push = "\taddi sp, sp, -4\n"
                       "\tsw a0, (sp)\n";
    const char* pop2 = "\tlw t0, 4(sp)\n"
                       "\tlw t1, (sp)\n"
                       "\taddi sp, sp, 8\n";
    const char* pop1 = "\tlw t0, (sp)\n"
                       "\taddi sp, sp, 4\n";
    /*
        Specify return type of each operation
    */
    enum retType {UNDEF, INT};
};