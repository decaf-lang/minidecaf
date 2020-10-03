#pragma once

#include "MiniDecafBaseVisitor.h"

/*
    This is a unique pass apart from the code generation pass, we build the minidecaf
    compiler using modern multi-pass architecture. Different passes are designed to
    finish different tasks, perform different actions on the generated syntax tree.

    Attention: If you want to enhance your compiler, for example, supporting more
    data types like double, char etc. We recommend you to spilt namer, typer, locator 
    pass to 3 different files. (Though in this implementation, we merge them together for 
    simplicity ^_^)
*/


class Allocator : public MiniDecafBaseVisitor {
public:
    // top level
    antlrcpp::Any visitFunc(MiniDecafParser::FuncContext *ctx);
    antlrcpp::Any visitBlock(MiniDecafParser::BlockContext *ctx);
    antlrcpp::Any visitForLoop(MiniDecafParser::ForLoopContext *ctx);

    // variable level
    antlrcpp::Any visitGlobalVar(MiniDecafParser::GlobalVarContext *ctx);
    antlrcpp::Any visitVarDef(MiniDecafParser::VarDefContext *ctx);
    antlrcpp::Any visitGlobalArrDef(MiniDecafParser::GlobalArrDefContext *ctx);
    antlrcpp::Any visitLocalArrDef(MiniDecafParser::LocalArrDefContext *ctx);
    antlrcpp::Any visitAssign(MiniDecafParser::AssignContext *ctx);

    // concrete expr level
    antlrcpp::Any visitCondExpr(MiniDecafParser::CondExprContext *ctx);
    antlrcpp::Any visitLor(MiniDecafParser::LorContext *ctx);
    antlrcpp::Any visitLand(MiniDecafParser::LandContext *ctx);
    antlrcpp::Any visitEqual(MiniDecafParser::EqualContext *ctx);
    antlrcpp::Any visitLessGreat(MiniDecafParser::LessGreatContext *ctx);
    antlrcpp::Any visitAddSub(MiniDecafParser::AddSubContext *ctx);
    // antlrcpp::Any visitMulDiv(MiniDecafParser::MulDivContext *ctx);

    // unary level
    antlrcpp::Any visitUnaryOp(MiniDecafParser::UnaryOpContext *ctx);
    antlrcpp::Any visitCast(MiniDecafParser::CastContext *ctx);
    antlrcpp::Any visitFuncCall(MiniDecafParser::FuncCallContext *ctx);
    antlrcpp::Any visitArrayIndex(MiniDecafParser::ArrayIndexContext *ctx);
    antlrcpp::Any visitAtomParen(MiniDecafParser::AtomParenContext *ctx);
    antlrcpp::Any visitIdentifier(MiniDecafParser::IdentifierContext *ctx);
    antlrcpp::Any visitInteger(MiniDecafParser::IntegerContext *ctx);

    // type level
    antlrcpp::Any visitIntType(MiniDecafParser::IntTypeContext *ctx);
    // antlrcpp::Any visitReturnStmt(MiniDecafParser::ReturnStmtContext *ctx);

private:

    // Current function scope
    std::string curFunc;
    int offset;
    /* 
        BlockOrder is used to distinguish different block statement
        BlockDep is used to deal with nested block statement
    */
    int blockOrder, blockDep;
};