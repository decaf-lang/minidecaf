#include "Allocator.h"
#include "utils.h"

//@brief: Visit the root node of the generated syntax tree
antlrcpp::Any Allocator::visitFunc(MiniDecafParser::FuncContext *ctx) {
    // Update the function scope
    curFunc = ctx->Identifier(0)->getText();
    blockDep = 0; blockOrder = 0; offset = 0;
    // Detect redefinition error
    if (!ctx->Semicolon()) {
        if (Singleton::getInstance().Singleton::getInstance().funcTable.count(curFunc) > 0) {
            if (Singleton::getInstance().Singleton::getInstance().funcTable[curFunc].get()->initialized()) {
                std::cerr << "line " << ctx->start->getLine() << ": ";
                std::cerr << "[ERROR] Redefinition of function " << curFunc << "\n";
                exit(1);
            } else {
                /* 
                    For the existence of current function in symbol table.
                    Note that in minidecaf we distinguish function decl from
                    definition (for detailed information, please read our tutorial),
                    so in function symbol table, we mark the existence of current function, 
                */
                Singleton::getInstance().Singleton::getInstance().funcTable[curFunc].get()->initialize();
                // Initialize the parameter list & the return value 
                for (auto i = 1; i < ctx->Identifier().size(); ++i) {
                    std::string varName = ctx->Identifier(i)->getText();
                    Singleton::getInstance().symbolTable[curFunc][varName] = std::make_shared<Symbol>(varName, offset++, Singleton::getInstance().funcTable[curFunc].get()->getArgType(i-1));
                }
                for (auto item : ctx->blockItem()) {
                    visit(item);
                }
            }
        } else {
            std::vector<std::shared_ptr<Type> > argType;
            // Initialize the parameter list & the return value 
            for (auto i = 1; i < ctx->Identifier().size(); ++i) {
                std::string varName = ctx->Identifier(i)->getText();
                argType.push_back(visit(ctx->type(i)));
                Singleton::getInstance().symbolTable[curFunc][varName] = std::make_shared<Symbol>(varName, offset++, argType[i-1]);
            }
            Singleton::getInstance().funcTable[curFunc] = std::make_shared<FuncSymbol>(curFunc, visit(ctx->type(0)), argType, true);
            for (auto item : ctx->blockItem()) {
                visit(item);
            }
        }
    } else {
        // Only declaraion is detected, eg. int add(int a, int b);
        // So we only need to mark the existence of function and record the parameter list
        std::vector<std::shared_ptr<Type> > argType;
        for (auto i = 1; i < ctx->type().size(); ++i) {
            argType.push_back(visit(ctx->type(i)));
        }
        Singleton::getInstance().funcTable[curFunc] = std::make_shared<FuncSymbol>(curFunc, visit(ctx->type(0)), argType);
    }
    std::shared_ptr<Type> type = std::make_shared<NoneType>();
    return type;
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
    std::shared_ptr<Type> type = std::make_shared<NoneType>();
    return type;
}

antlrcpp::Any Allocator::visitGlobalArrDef(MiniDecafParser::GlobalArrDefContext *ctx) {
    std::string varName = ctx->Identifier()->getText();
    if (Singleton::getInstance().symbolTable["global"].count(varName) > 0) {
        std::cerr << "line " << ctx->start->getLine() << ": ";
        std::cerr << "[ERROR] Redefinition of global variable " << varName << "\n";
        exit(1);
    }
    // Array dimension cannot be 0
    int volume = 1;
    std::shared_ptr<Type> baseType = visit(ctx->type());
    baseType.get()->setValueType(1);
    for (int i = ctx->Interger().size()-1; i >= 0; --i) {
        int arrDim = std::stoi(ctx->Interger(i)->getText());
        if (arrDim == 0) {
            std::cerr << "line " << ctx->start->getLine() << ": ";
            std::cerr << "[ERROR] Array " << varName << " has 0 dimension\n";
            exit(1);
        }
        volume *= arrDim;
        baseType = std::make_shared<ArrayType>(baseType, volume);
    }
    Singleton::getInstance().symbolTable["global"][varName] = std::make_shared<Symbol>(varName, 0, baseType, ctx->start->getLine(), volume);
    std::shared_ptr<Type> type = std::make_shared<NoneType>();
    return type;
}

antlrcpp::Any Allocator::visitLocalArrDef(MiniDecafParser::LocalArrDefContext *ctx) {
    std::string varName = ctx->Identifier()->getText();
    if (Singleton::getInstance().symbolTable[curFunc].count(varName) > 0) {
        std::cerr << "line " << ctx->start->getLine() << ": ";
        std::cerr << "[ERROR] Redefinition of global variable " << varName << "\n";
        exit(1);
    }
    // Array dimension cannot be 0
    int volume = 1;
    std::shared_ptr<Type> baseType = visit(ctx->type());
    baseType.get()->setValueType(1);
    for (int i = ctx->Interger().size()-1; i >= 0; --i) {
        int arrDim = std::stoi(ctx->Interger(i)->getText());
        if (arrDim == 0) {
            std::cerr << "line " << ctx->start->getLine() << ": ";
            std::cerr << "[ERROR] Array " << varName << " has 0 dimension\n";
            exit(1);
        }
        volume *= arrDim;
        baseType = std::make_shared<ArrayType>(baseType, arrDim);
    }
    offset += volume;
    Singleton::getInstance().symbolTable[curFunc][varName] = std::make_shared<Symbol>(varName, offset-1, baseType, ctx->start->getLine(), volume);
    std::shared_ptr<Type> type = std::make_shared<NoneType>();
    return type;
}

antlrcpp::Any Allocator::visitGlobalVar(MiniDecafParser::GlobalVarContext *ctx) {
    // Global varibles are stored in global scope, the outermost scope 
    std::string varName = ctx->Identifier()->getText();
    if (Singleton::getInstance().symbolTable["global"].count(varName) > 0) {
        std::cerr << "line " << ctx->start->getLine() << ": ";
        std::cerr << "[ERROR] Redefinition of global variable " << varName << "\n";
        exit(1);
    }
    Singleton::getInstance().symbolTable["global"][varName] = std::make_shared<Symbol>(varName, 0, visit(ctx->type()), ctx->start->getLine());
    Singleton::getInstance().symbolTable["global"][varName].get()->getType()->setValueType(1);
    std::shared_ptr<Type> type = std::make_shared<NoneType>();
    return type;
}

antlrcpp::Any Allocator::visitVarDef(MiniDecafParser::VarDefContext *ctx) {
    // allocate the defined varible
    std::string varName = ctx->Identifier()->getText();
    if (Singleton::getInstance().symbolTable[curFunc].count(varName) > 0) {
        std::cerr << "line " << ctx->start->getLine() << ": ";
        std::cerr << "[ERROR] Redefinition of variable " << varName << "\n";
        exit(1);
    }
    /* Solve the late declaration of a variable by counting the line number
       @TODO: Note that the implementation here only considers line number, so
       when you write your codes in one line, you also need to check the column number. 
    */
    std::shared_ptr<Type> lType = visit(ctx->type());
    if (ctx->expr()) {
        std::shared_ptr<Type> rType = visit(ctx->expr());
        if (!lType.get()->typeCheck(rType)) {
            std::cerr << "line " << ctx->start->getLine() << ": ";
            std::cerr << "[ERROR] Incompatible parameter type for vardef operation\n";
            exit(1);
        }
    }
    Singleton::getInstance().symbolTable[curFunc][varName] = std::make_shared<Symbol>(varName, offset++, lType, ctx->start->getLine());
    Singleton::getInstance().symbolTable[curFunc][varName].get()->getType()->setValueType(1);
    std::shared_ptr<Type> type = std::make_shared<NoneType>();
    return type;
}



//@brief: Make sure that a variable must be defined before assigning it
antlrcpp::Any Allocator::visitAssign(MiniDecafParser::AssignContext *ctx) {
    std::string tmpFunc = curFunc;
    std::shared_ptr<Type> lType = visit(ctx->unary());
    std::shared_ptr<Type> rType = visit(ctx->expr());
    // The two operators must have the same data type.
    if (!lType.get()->typeCheck(rType)) {
        std::cerr << "line " << ctx->start->getLine() << ": ";
        std::cerr << "[ERROR] Incompatible parameter type for assign operation\n";
        exit(1);
    }
    if (lType.get()->getValueType() == 0) {
        std::cerr << "line " << ctx->start->getLine() << ": ";
        std::cerr << "[ERROR] Assign to right value is forbiddened\n";
        exit(1);
    }
    return lType;
}

//@brief: A forloop is also a new scope
antlrcpp::Any Allocator::visitForLoop(MiniDecafParser::ForLoopContext *ctx) {
    curFunc += "@" + std::to_string(blockOrder) + std::to_string(++blockDep);
    int exprBase = -1;
    // for (<decl|expr>; <expr>; <expr>) <stmt>
    // The Branch model is nearly the same as Dowhile
    if (ctx->decl()) {
        visit(ctx->decl());
    } else if (ctx->expr(0)) {
        visit(ctx->expr(0));
        exprBase = 0;
    }
    if (ctx->expr(exprBase+1)) {
        visit(ctx->expr(exprBase+1));
    } 
    visit(ctx->stmt());
    if (ctx->expr(exprBase+2)) {
        visit(ctx->expr(exprBase+2));
    }
    if (--blockDep == 0) {
        ++blockOrder;
    }
    int pos = curFunc.find_last_of('@');
    curFunc = curFunc.substr(0, pos);
    
    std::shared_ptr<Type> type = std::make_shared<NoneType>();
    return type;
}

//@brief: Most of the work is type checking
antlrcpp::Any Allocator::visitEqual(MiniDecafParser::EqualContext *ctx) {
    std::shared_ptr<Type> ltype = visit(ctx->equ(0));
    std::shared_ptr<Type> rtype = visit(ctx->equ(1));
    if (!ltype.get()->typeCheck(rtype)) {
        std::cerr << "line " << ctx->start->getLine() << ": ";
        std::cerr << "[ERROR] Incompatible parameter type for equal operation\n";
        exit(1);
    } 
    std::shared_ptr<Type> type = std::make_shared<IntType>();
    return type;
}

//@brief: Most of the work is type checking
antlrcpp::Any Allocator::visitAddSub(MiniDecafParser::AddSubContext *ctx) {
    std::shared_ptr<Type> firstAdder = visit(ctx->add(0));
    std::shared_ptr<Type> secondAdder = visit(ctx->add(1));
    Singleton::getInstance().typeQueue.push(firstAdder);
    Singleton::getInstance().typeQueue.push(secondAdder);
    std::shared_ptr<Type> ansType = std::make_shared<NoneType>();
    if (firstAdder.get()->getType() == "Intptr" && secondAdder.get()->getType() == "Intptr") {
        if (ctx->Addition()) {
            std::cerr << "line " << ctx->start->getLine() << ": ";
            std::cerr << "[ERROR] Add operation between pointers is not supported\n";
            exit(1);
        } else if (ctx->Minus()) {
            ansType = std::make_shared<IntType>();
        }
    } else if (firstAdder.get()->getType() == "Intptr") {
        ansType = std::make_shared<IntptrType>(firstAdder.get()->getStarNum());
    } else if (secondAdder.get()->getType() == "Intptr") {
        if (ctx->Addition()) {
            ansType = std::make_shared<IntptrType>(secondAdder.get()->getStarNum());
        } else if (ctx->Minus()) {
            std::cerr << "line " << ctx->start->getLine() << ": ";
            std::cerr << "[ERROR] Integer minus pointer is not supported\n";
            exit(1);
        }
    } else {
        ansType = std::make_shared<IntType>();
    }
    return ansType;
}

//@brief: Most of the work is type checking
antlrcpp::Any Allocator::visitUnaryOp(MiniDecafParser::UnaryOpContext *ctx) {
    // Undef behavior
    // if (ctx->Exclamation() || ctx->Minus() || ctx->Tilde()) 
    std::shared_ptr<Type> src = visit(ctx->unary());
    std::shared_ptr<Type> type = std::make_shared<IntType>();
    int starNum = src.get()->getStarNum();
    // '*' operator must operate on pointer type
    if (ctx->Multiplication()) {
        if (src.get()->typeCheckLiteral("Intptr")) {
            if (starNum == 1) {
                type = std::make_shared<IntType>(1);
            } else {
                type = std::make_shared<IntptrType>(starNum-1, 1);
            }
        } else {
            std::cerr << "line " << ctx->start->getLine() << ": ";
            std::cerr << "[ERROR] Invalid operation '*' on non-pointer type\n";
            exit(1);
        }
    } else if (ctx->AND()) {
        // '&' operator must operate on left value 
        if (src.get()->getValueType() == 0) {
            std::cerr << "line " << ctx->start->getLine() << ": ";
            std::cerr << "[ERROR] Invalid operation '&' on right value\n";
            exit(1);
        } else {
            type = std::make_shared<IntptrType>(starNum+1);
        }
    }
    return type;
}

antlrcpp::Any Allocator::visitArrayIndex(MiniDecafParser::ArrayIndexContext *ctx) {
    std::shared_ptr<Type> postType = visit(ctx->postfix());
    std::shared_ptr<Type> type = visit(ctx->expr());
    Singleton::getInstance().typeQueue.push(postType);
    std::shared_ptr<Type> returnType;
    if (type.get()->getType() != "Int") {
        std::cerr << "line " << ctx->start->getLine() << ": ";
        std::cerr << "[ERROR] Array index must be integer\n";
        exit(1);
    }
    if (postType.get()->getType() == "Intptr") {
        int starNum = postType.get()->getStarNum();
        if (starNum == 1) {
            type = std::make_shared<IntType>(1);
        } else {
            type = std::make_shared<IntptrType>(starNum-1, 1);
        }
        return type;
    } else {
        return postType.get()->getBaseType();
    }
}

antlrcpp::Any Allocator::visitCast(MiniDecafParser::CastContext *ctx) {
    std::shared_ptr<Type> dst = visit(ctx->type());
    std::shared_ptr<Type> src = visit(ctx->unary());
    return dst.get()->typeCast(src.get()->getValueType());
}

//@brief: Make sure functions are declared before calling them
antlrcpp::Any Allocator::visitFuncCall(MiniDecafParser::FuncCallContext *ctx) {
    std::string funcName = ctx->Identifier()->getText();
    if (Singleton::getInstance().funcTable.count(funcName) == 0) {
        std::cerr << "line " << ctx->start->getLine() << ": ";
        std::cerr << "[ERROR] Use of undeclared function " << funcName << "\n";
        exit(1);
    }
    for (int i = ctx->expr().size()-1; i >= 0; --i) {
        if (!Singleton::getInstance().funcTable[funcName].get()->getArgType(i).get()->typeCheck(visit(ctx->expr(i)))) {
            std::cerr << "line " << ctx->start->getLine() << ": ";
            std::cerr << "[ERROR] Incompatible parameter type for function " << funcName << "\n";
            exit(1);
        }
    }
    return Singleton::getInstance().funcTable[funcName].get()->getRetType();
}

antlrcpp::Any Allocator::visitAtomParen(MiniDecafParser::AtomParenContext *ctx) {
    return visit(ctx->expr());
}

//@brief: Make sure that a variable must be defined before using it
antlrcpp::Any Allocator::visitIdentifier(MiniDecafParser::IdentifierContext *ctx) {
    std::string varName = ctx->Identifier()->getText();
    std::string tmpFunc = curFunc;
    // Search the symbol table from inner scope to the outer, for the target symbol
    for (int i = 0; i <= blockDep; ++i) {
        if (Singleton::getInstance().symbolTable[tmpFunc].count(varName) == 0) {
            int pos = tmpFunc.find_last_of('@');
            tmpFunc = tmpFunc.substr(0,pos);
            continue;
        }
        return Singleton::getInstance().symbolTable[tmpFunc][varName].get()->getType();
    }
    // Search in global scope at last
    if (Singleton::getInstance().symbolTable["global"].count(varName) == 0) {
        std::cerr << "line " << ctx->start->getLine() << ": ";
        std::cerr << "[ERROR] Variable " << varName << " is used without definition\n";
        exit(1);
    }
    return Singleton::getInstance().symbolTable["global"][varName].get()->getType();
}

antlrcpp::Any Allocator::visitInteger(MiniDecafParser::IntegerContext *ctx) {
    std::shared_ptr<Type> type = std::make_shared<IntType>(); 
    return type;
}

//@brief: Get the type
antlrcpp::Any Allocator::visitIntType(MiniDecafParser::IntTypeContext *ctx) {
    // Star number means the number of '*' in the typedef
    // Naturally, we treat int type as 0 star number type
    int starNum = ctx->Multiplication().size();
    std::shared_ptr<Type> type;
    if (starNum == 0) {
        type =  std::make_shared<IntType>();
    } else if (starNum > 0) {
        type = std::make_shared<IntptrType>(starNum);
    } else {
        type = std::make_shared<NoneType>();
    }
    return type;
}