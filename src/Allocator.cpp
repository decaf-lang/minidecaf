#include "Allocator.h"

//@brief: Visit the root node of the generated syntax tree
antlrcpp::Any Allocator::visitProg(MiniDecafParser::ProgContext *ctx) {
    offset = 0;
    visitChildren(ctx);
    return varTab;
}

antlrcpp::Any Allocator::visitFunc(MiniDecafParser::FuncContext *ctx) {
    // Update the function scope
    curFunc = ctx->Identifier(0)->getText();
    blockDep = 0; blockOrder = 0; offset = 0;
    // Detect redefinition error
    if (!ctx->Semicolon()) {
        if (varTab.count(curFunc) > 0 && varTab[curFunc]["###"] == 1) {
            std::cerr << "[ERROR] Redefinition of function " << curFunc << "\n";
            exit(1);
        } else {
            /* 
                For the existence of current function in symbol table.
                Note that in minidecaf we distinguish function decl from
                definition (for detailed information, please read our tutorial),
                so in symbol table, we add a non-variable symbol "###" to mark
                the existence of current function, while 0 stands for declared
                but not defined, 1 stands for already defined.
            */
            varTab[curFunc]["###"] = 1;
            for (auto i = 1; i < ctx->Identifier().size(); ++i) {
                std::string varName = ctx->Identifier(i)->getText();
                varTab[curFunc][varName] = offset++;
            }
            visitChildren(ctx);
        }
    } else {
        varTab[curFunc]["###"] = 0;
    }
    return retType::INT;
}

//@brief: Make sure functions are declared before calling them
antlrcpp::Any Allocator::visitFuncCall(MiniDecafParser::FuncCallContext *ctx) {
    std::string funcName = ctx->Identifier()->getText();
    if (varTab.count(funcName) == 0) {
        std::cerr << "[ERROR] Use of undefined function " << curFunc << "\n";
        exit(1);
    }
    return retType::UNDEF;
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

antlrcpp::Any Allocator::visitGlobalVar(MiniDecafParser::GlobalVarContext *ctx) {
    // Global varibles are stored in global scope, the outermost scope 
    std::string varName = ctx->Identifier()->getText();
    if (varTab["global"].count(varName) > 0) {
        std::cerr << "[ERROR] Redefinition of global variable " << varName << "\n";
        exit(1);
    }
    varTab["global"][varName] = 0;
    return retType::INT;
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
    // Search in global scope at last
    if (varTab["global"].count(varName) == 0) {
        std::cerr << "[ERROR] Variable " << varName << " is used without definition\n";
        exit(1);
    }
    return retType::INT;
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
    // Search in global scope at last
    if (varTab["global"].count(varName) == 0) {
        std::cerr << "[ERROR] Variable " << varName << " is used without definition\n";
        exit(1);
    }
    return retType::INT;
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
