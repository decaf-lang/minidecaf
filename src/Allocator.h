#pragma once

#include "MiniDecafBaseVisitor.h"
#include "utils.h"

/*
    This is a unique pass apart from the code generation pass, we build the minidecaf
    compiler using modern multi-pass architecture. Different passes are designed to
    finish different tasks, perform different actions on the generated syntax tree.
*/
class Allocator : public MiniDecafBaseVisitor {
public:
    antlrcpp::Any visitProg(MiniDecafParser::ProgContext *ctx);
    antlrcpp::Any visitFunc(MiniDecafParser::FuncContext *ctx);
    antlrcpp::Any visitFuncCall(MiniDecafParser::FuncCallContext *ctx);
    antlrcpp::Any visitBlock(MiniDecafParser::BlockContext *ctx);

    antlrcpp::Any visitGlobalVar(MiniDecafParser::GlobalVarContext *ctx);
    antlrcpp::Any visitVarDef(MiniDecafParser::VarDefContext *ctx);
    antlrcpp::Any visitIdentifier(MiniDecafParser::IdentifierContext *ctx);
    antlrcpp::Any visitAssign(MiniDecafParser::AssignContext *ctx);

    antlrcpp::Any visitForLoop(MiniDecafParser::ForLoopContext *ctx);

private:
    /*
        Symbol table, which stores function, scope & varible information.
        The table is organized as follows:
        ------------------------------------------------------
        | Function | Variable1 | Offset in stack | Data type |
        |          | Variable2 | ...             | ...       |
        |          | ...       | ...             | ...       |
        ------------------------------------------------------
    */
    symTab<int> varTab;
    enum retType {UNDEF, INT};

    // Current function scope
    std::string curFunc;
    int offset;
    /* 
        BlockOrder is used to distinguish different block statement
        BlockDep is used to deal with nested block statement
    */
    int blockOrder, blockDep;
};