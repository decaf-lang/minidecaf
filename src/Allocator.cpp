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
    blockDep = 0; blockOrder = 0;
    // Detect redefinition error
    if (varTab.count(curFunc) > 0) {
        std::cerr << "[ERROR] Redefinition of function " << curFunc << "\n";
        exit(1);
    }
    visitChildren(ctx);
    return retType::INT;
}

//@brief: Visit block statement, build a new scope in symbol table
antlrcpp::Any Allocator::visitBlock(MiniDecafParser::BlockContext *ctx) {
    /*
        As we add block statement to our grammar system, the definition of
        scope is important. In our implementation, we use a single symbol
        table to store different variables in different scopes. In order
        to distinguish them, we define "blockDep" and "blockOrder".
        eg. 
        int main() {
            int a = 0;
            {
                int a = 1;
            }
        }
        -----------------------------------------------------------------
        <function>@<blockOrder>@<blockDep>
        symbol table:
        main@0@0: 0
        main@1@1: 1
        When we retrieve the symbol table for certain symbol, the depth
        and order of scope is combined into generated key, which is directly
        pointed to the required symbol.
    */
    ++blockDep;
    curFunc += "@" + std::to_string(blockOrder) + std::to_string(blockDep);
    for (auto item : ctx->blockItem()) {
        visit(item);
    }
    if (--blockDep == 0) {
        ++blockOrder;
    }
    int pos = curFunc.find_last_of('@');
    curFunc = curFunc.substr(0, pos);
    return retType::UNDEF;
}

antlrcpp::Any Allocator::visitVarDef(MiniDecafParser::VarDefContext *ctx) {
    // allocate the defined varible
    std::string varName = ctx->Identifier()->getText();
    if (varTab[curFunc].count(varName) > 0) {
        std::cerr << "[ERROR] Redefinition of variable " << varName << "\n";
        exit(1);
    }
    /* Solve the late declaration of a variable by counting the line number
       @TODO: Note that the implementation here only considers line number, so
       when you write your codes in one line, you also need to check the column number. 
    */
    if (varTab[curFunc].count(varName+"#") > 0) {
        varTab[curFunc][varName+"#"] = ctx->start->getLine();
    }
    varTab[curFunc][varName] = offset++;
    return retType::INT;
}

//@brief: Make sure that a variable must be defined before using it
antlrcpp::Any Allocator::visitIdentifier(MiniDecafParser::IdentifierContext *ctx) {
    std::string varName = ctx->Identifier()->getText();
    std::string tmpFunc = curFunc;
    // Search the symbol table from inner scope to the outer, for the target symbol
    for (int i = 0; i <= blockDep; ++i) {
        if (varTab[tmpFunc].count(varName) == 0) {
            int pos = tmpFunc.find_last_of('@');
            tmpFunc = tmpFunc.substr(0,pos);
            continue;
        }
        if (varTab[curFunc].count(varName) == 0) {
            varTab[curFunc][varName + "#"] = 0;
        }
        return retType::INT;
    }
    std::cerr << "[ERROR] Variable " << varName << " is used without definition\n";
    exit(1);
}

//@brief: Make sure that a variable must be defined before assigning it
antlrcpp::Any Allocator::visitAssign(MiniDecafParser::AssignContext *ctx) {
    std::string varName = ctx->Identifier()->getText();
    std::string tmpFunc = curFunc;
    // Search the symbol table from inner scope to the outer, for the target symbol
    for (int i = 0; i <= blockDep; ++i) {
        if (varTab[tmpFunc].count(varName) == 0) {
            int pos = tmpFunc.find_last_of('@');
            tmpFunc = tmpFunc.substr(0,pos);
            continue;
        }
        if (varTab[curFunc].count(varName) == 0) {
            varTab[curFunc][varName + "#"] = 0;
        }
        return retType::INT;
    }
    std::cerr << "[ERROR] Variable " << varName << " is used without definition\n";
    exit(1);
}

//@brief: A forloop is also a new scope
antlrcpp::Any Allocator::visitForLoop(MiniDecafParser::ForLoopContext *ctx) {
    curFunc += "@" + std::to_string(blockOrder) + std::to_string(++blockDep);
    visitChildren(ctx);
    if (--blockDep == 0) {
        ++blockOrder;
    }
    int pos = curFunc.find_last_of('@');
    curFunc = curFunc.substr(0, pos);
    return retType::UNDEF;
}
