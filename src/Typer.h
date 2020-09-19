#pragma once

#include "MiniDecafBaseVisitor.h"
#include "utils.h"

class Typer : public MiniDecafBaseVisitor {
public:
    antlrcpp::Any visitProg(MiniDecafParser::ProgContext *ctx);
private:
    enum retType {UNDEF, INT};

    // Current function scope
    std::string curFunc;
    /* 
        BlockOrder is used to distinguish different block statement
        BlockDep is used to deal with nested block statement
    */
    int blockOrder, blockDep;
};