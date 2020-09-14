#pragma once

#include "MiniDecafBaseVisitor.h"
#include <string>
#include <unordered_map>
#include <tuple>

template<typename T>
using symTab = std::unordered_map<std::string, std::unordered_map<std::string, T> >;

/*
    This is a unique pass apart from the code generation pass, we build the minidecaf
    compiler using modern multi-pass architecture. Different passes are designed to
    finish different tasks, perform different actions on the generated syntax tree.
*/
class Allocator : public MiniDecafBaseVisitor {
public:
    antlrcpp::Any visitProg(MiniDecafParser::ProgContext *ctx);
    antlrcpp::Any visitFunc(MiniDecafParser::FuncContext *ctx);
    antlrcpp::Any visitVarDef(MiniDecafParser::VarDefContext *ctx);

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
};