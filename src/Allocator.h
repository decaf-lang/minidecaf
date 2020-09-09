#pragma once

#include "MiniDecafBaseVisitor.h"
#include <string>
#include <unordered_map>
#include <tuple>

template<typename T>
using symTab = std::unordered_map<std::string, std::unordered_map<std::string, T> >;

class Allocator : public MiniDecafBaseVisitor {
public:
    antlrcpp::Any visitProg(MiniDecafParser::ProgContext *ctx);
    antlrcpp::Any visitFunc(MiniDecafParser::FuncContext *ctx);
    antlrcpp::Any visitVarDef(MiniDecafParser::VarDefContext *ctx);

private:
    symTab<int> varTab;
    enum retType {UNDEF, INT};

    std::string curFunc;
    int offset;
};