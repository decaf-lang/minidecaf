#include "Allocator.h"

//@brief: Visit the root node of the generated syntax tree
antlrcpp::Any Allocator::visitProg(MiniDecafParser::ProgContext *ctx) {
    offset = 0;
    visitChildren(ctx);
    return varTab;
}

antlrcpp::Any Allocator::visitFunc(MiniDecafParser::FuncContext *ctx) {
    // Update the function scope
    curFunc = ctx->Identifier()->getText();
    // Detect redefinition error
    if (varTab.count(curFunc) > 0) {
        std::cerr << "[ERROR] Redefinition of function " << curFunc << "\n";
        exit(1);
    }
    visitChildren(ctx);
    return retType::INT;
}

antlrcpp::Any Allocator::visitVarDef(MiniDecafParser::VarDefContext *ctx) {
    // allocate the defined varible
    std::string varName = ctx->Identifier()->getText();
    varTab[curFunc][varName] = offset++;
    return retType::INT;
}

//@brief: Make sure that a variable must be defined before using it
antlrcpp::Any Allocator::visitIdentifier(MiniDecafParser::IdentifierContext *ctx) {
    std::string varName = ctx->Identifier()->getText();
    if (varTab[curFunc].count(varName) == 0) {
        std::cerr << "[ERROR] Variable " << varName << " is used without definition\n";
        exit(1);
    } 
    return retType::INT;
}

//@brief: Make sure that a variable must be defined before assigning it
antlrcpp::Any Allocator::visitAssign(MiniDecafParser::AssignContext *ctx) {
    std::string varName = ctx->Identifier()->getText();
    if (varTab[curFunc].count(varName) == 0) {
        std::cerr << "[ERROR] Variable " << varName << " is assigned without definition\n";
        exit(1);
    }
    return retType::INT;
}

