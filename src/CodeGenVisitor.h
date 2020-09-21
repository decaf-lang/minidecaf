#pragma once

#include "MiniDecafBaseVisitor.h"

// Code generation pass
class CodeGenVisitor : public MiniDecafBaseVisitor {
public:
    antlrcpp::Any visitProg(MiniDecafParser::ProgContext *ctx);
    antlrcpp::Any visitFunc(MiniDecafParser::FuncContext *ctx);
    antlrcpp::Any visitFuncCall(MiniDecafParser::FuncCallContext *ctx);
    antlrcpp::Any visitBlock(MiniDecafParser::BlockContext *ctx);
    antlrcpp::Any visitReturnStmt(MiniDecafParser::ReturnStmtContext *ctx);
    antlrcpp::Any visitIfStmt(MiniDecafParser::IfStmtContext *ctx);
    antlrcpp::Any visitCondExpr(MiniDecafParser::CondExprContext *ctx);
    antlrcpp::Any visitSingleExpr(MiniDecafParser::SingleExprContext *ctx);

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
    antlrcpp::Any visitGlobalVar(MiniDecafParser::GlobalVarContext *ctx);
    antlrcpp::Any visitCast(MiniDecafParser::CastContext *ctx);

    antlrcpp::Any visitForLoop(MiniDecafParser::ForLoopContext *ctx);
    antlrcpp::Any visitWhileLoop(MiniDecafParser::WhileLoopContext *ctx);
    antlrcpp::Any visitDoWhile(MiniDecafParser::DoWhileContext *ctx);
    antlrcpp::Any visitBreak(MiniDecafParser::BreakContext *ctx);
    antlrcpp::Any visitContinue(MiniDecafParser::ContinueContext *ctx);
    /* 
        A simple stack machine model 
        Support basic push & pop operations
    */
    std::string pushReg(std::string reg);
    std::string popReg(std::string reg);
    std::string popReg(std::string reg0, std::string);
private:
    /*
        Stringstream used to store generated codes
        .text, .data and .bss segments
    */
    std::ostringstream code_;
    std::ostringstream data_;
    std::ostringstream bss_;
    /*
        Deal with nested break & continue statements
    */
    std::vector<int> breakTarget, continueTarget;

    std::string curFunc;
    bool retState;
    int labelOrder;
    int blockDep, blockOrder;


    /*
        Specify return type of each operation (left value or right value)
    */
    enum retType {LEFT, RIGHT, UNDEF};
};