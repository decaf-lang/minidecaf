#include "CodeGenVisitor.h"
#include "utils.h"

// @brief: Start visiting the syntax tree from root node Prog
// @ret: Generated asm code
antlrcpp::Any CodeGenVisitor::visitProg(MiniDecafParser::ProgContext *ctx) {
    labelOrder = 0;
    code_ << ".section .text\n"
        << ".globl main\n";
    data_ << ".section .data\n";
    bss_ << ".section .bss\n";
    visitChildren(ctx);
    return code_.str() + data_.str() + bss_.str();
}

// @brief: Visit Func node, in this step we only support function call with no parameters
// @ret: Function return type
antlrcpp::Any CodeGenVisitor::visitFunc(MiniDecafParser::FuncContext *ctx) {
    // Get function name
    if (!ctx->Semicolon()) {
        curFunc = ctx->Identifier(0)->getText();
        retState = false;
        blockDep = 0; blockOrder = 0;
        // Calling convention: saving ra(return address), caller fp and allocating stack memory for local variable
        code_ << curFunc << ":\n"
            << "\tsw ra, -4(sp)\n"
            << "\taddi sp, sp, -4\n"
            << "\tsw fp, -4(sp)\n"
            << "\taddi fp, sp, -4\n"
            << "\taddi sp, fp, ";

        int capacity = 0;
        for (auto& var : Singleton::getInstance().symbolTable) {
            if (var.first.substr(0, curFunc.length()) == curFunc) {
                capacity += Singleton::getInstance().symbolTable[var.first].size();
            }
        }
        code_ << -4 * capacity << "\n";

        // Calling convection: How to pass funcion args
        // When the number of arguments is greater than 7, we use stack to pass them, otherwise we use register a0-a6 instead.
        for (int i = 1; i < ctx->Identifier().size(); ++i) {
            if (ctx->Identifier().size() > 8) {
                code_ << "\tlw t0, " << 8 + 4 * Singleton::getInstance().symbolTable[curFunc][ctx->Identifier(i)->getText()].get()->getOffset() << "(fp)\n";
                code_ << "\tsw t0, " << -4 - 4 * Singleton::getInstance().symbolTable[curFunc][ctx->Identifier(i)->getText()].get()->getOffset() << "(fp)\n";
            } else {
                code_ << "\tsw a" << i-1 << ", " << -4 - 4 * Singleton::getInstance().symbolTable[curFunc][ctx->Identifier(i)->getText()].get()->getOffset() << "(fp)\n";
            }
        } 
        visitChildren(ctx);

        // Dealing with no return in main()
        if (!retState) {
            code_ << "\tli a0, 0\n"
                << "\taddi sp, fp, 4\n"
                << "\tlw ra, (sp)\n" 
                << "\tlw fp, -4(sp)\n"
                << "\tret\n";
        }
    }
    return retType::UNDEF;
}

//@brief: Visit FuncCall node
antlrcpp::Any CodeGenVisitor::visitFuncCall(MiniDecafParser::FuncCallContext *ctx) {
    // Before calling a function, we should compute the arguments and pass them using stack or register
    if (ctx->expr().size() > 7) { 
        for (int i = ctx->expr().size()-1; i >= 0; --i) {
            retType type = visit(ctx->expr(i));
            if (type == retType::LEFT) {
                code_ << popReg("t0")
                      << "\tlw t0, (t0)\n"
                      << pushReg("t0");
            }
        }
    } else {
        for (int i = ctx->expr().size()-1; i >= 0; --i) {
            retType type = visit(ctx->expr(i));
            if (type == retType::LEFT) {
                code_ << popReg("a0")
                      << "\tlw a0, (a0)\n";
            }
            code_ << "\tmv a" << i << ", a0\n";
        }
    }
    code_ << "\tcall " << ctx->Identifier()->getText() << "\n"
        << "\taddi sp, sp, " << 4 + 4 * ctx->expr().size() << "\n"
        << pushReg("a0");
    return retType::RIGHT;
}

//@brief: Visit BlockStmt node, update the scope information
antlrcpp::Any CodeGenVisitor::visitBlock(MiniDecafParser::BlockContext *ctx) {
    // When entering a new scope, update block depth
    ++blockDep;
    curFunc += "@" + std::to_string(blockOrder) + std::to_string(blockDep);
    for (auto item : ctx->blockItem()) {
        visit(item);
    }
    // When exiting a scope, update the block depth & block order(only top level)
    if (--blockDep == 0) {
        ++blockOrder;
    }
    int pos = curFunc.find_last_of('@');
    curFunc = curFunc.substr(0, pos);
    return retType::UNDEF;
}

// @brief: Visit ReturnStmt node, son of Stmt node
antlrcpp::Any CodeGenVisitor::visitReturnStmt(MiniDecafParser::ReturnStmtContext *ctx) {
    retType type = visit(ctx->expr());
    code_ << popReg("a0");
    if (type == retType::LEFT) {
        code_ << "\tlw a0, (a0)\n";
    }
    code_ << "\taddi sp, fp, 4\n"
        << "\tlw ra, (sp)\n" 
        << "\tlw fp, -4(sp)\n"
        << "\tret\n";
    retState = true;
    return retType::UNDEF;
}

// @brief: Visit Integer node, which loads an immediate number into register
// @ret: Expr type: Int
antlrcpp::Any CodeGenVisitor::visitInteger(MiniDecafParser::IntegerContext *ctx) {
    code_ << "\tli a0, " << ctx->getText() << "\n"
          << pushReg("a0");
    return retType::RIGHT;
}

// @brief: Visit unaryOp node, like '-1', '~12', '!89' and etc. 
// @ret: Expr type: Int
antlrcpp::Any CodeGenVisitor::visitUnaryOp(MiniDecafParser::UnaryOpContext *ctx) {
    retType factorType = visit(ctx->factor());
    code_ << popReg("t0");
    if (ctx->AND()) {
        code_ << pushReg("t0");
        return retType::RIGHT;
    }
    if (factorType == retType::LEFT) {
        code_ << "\tlw t0, (t0)\n";
    }
    /* 
        ctx-><token_name>() is a ptr point to the token
        ptr == nullptr indicates that the token is undeclared
    */
    if (ctx->Minus()) {
        code_ << "\tsub a0, x0, t0\n"
            << pushReg("a0");
        return retType::RIGHT;
    } else if(ctx->Exclamation()) {
        code_ << "\tseqz a0, t0\n"
            << pushReg("a0"); 
        return retType::RIGHT;
    } else if(ctx->Tilde()) {
        code_ << "\tnot a0, t0\n"
            << pushReg("a0");
        return retType::RIGHT;
    } else if(ctx->Multiplication()) {
        code_ << pushReg("t0");
        return retType::LEFT;
    }
    return retType::UNDEF;
}

//@brief: Visit Expr with parenthesis, like '3-(1+2)'
//@ret: Expr type: Inner expr
antlrcpp::Any CodeGenVisitor::visitAtomParen(MiniDecafParser::AtomParenContext *ctx) {
    /*
        Cause the parenthesis only specifies the priority of the expr,
        what we need to do is visit the inner Expr.
        While the parsing process of antlr4 defines the priority.
    */
    return visit(ctx->expr());
}

//@brief: visit AddSub node, including addition & subtraction between expressions
//@ret: Expr type: depending on the left & right expressions
antlrcpp::Any CodeGenVisitor::visitAddSub(MiniDecafParser::AddSubContext *ctx) {
    // Get the type of left & right expressions
    retType lExpr = visit(ctx->add(0)), rExpr = visit(ctx->add(1));
    /*
        The use of token ptr is the same as FUNC<visitUnaryOp>.
        In this step, stack machine first pops the 2 operators at the top of stack,
        then computes the result, finally pushes the result to the stack.
    */
    code_ << popReg("t0", "t1");
    if (lExpr == retType::LEFT) {
        code_ << "\tlw t0, (t0)\n";
    }
    if (rExpr == retType::LEFT) {
        code_ << "\tlw t1, (t1)\n";
    }
    if (ctx->Addition()) {
        code_ << "\tadd a0, t0, t1\n"
              << pushReg("a0");
        return retType::RIGHT;
    } else if (ctx->Minus()) {
        code_ << "\tsub a0, t0, t1\n"
              << pushReg("a0");
        return retType::RIGHT;
    }
    return retType::UNDEF;
}

//@brief: visit MulDiv node, including multiplication, division & modulo between expressions
//@ret: Expr type: depending on the left & right expressions
antlrcpp::Any CodeGenVisitor::visitMulDiv(MiniDecafParser::MulDivContext *ctx) {
    // Get the type of left & right expressions
    retType lExpr = visit(ctx->mul(0)), rExpr = visit(ctx->mul(1));
    /*
        Totally the same as FUNC<visitAddSub>.
    */
    code_ << popReg("t0", "t1");
    if (lExpr == retType::LEFT) {
        code_ << "\tlw t0, (t0)\n";
    }
    if (rExpr == retType::LEFT) {
        code_ << "\tlw t1, (t1)\n";
    }
    if (ctx->Multiplication()) {
        code_ << "\tmul a0, t0, t1\n"
              << pushReg("a0");
        return retType::RIGHT;
    } else if (ctx->Division()) {
        code_ << "\tdiv a0, t0, t1\n"
              << pushReg("a0");
        return retType::RIGHT;
    } else if (ctx->Modulo()) {
        code_ << "\trem a0, t0, t1\n"
              << pushReg("a0");
        return retType::RIGHT;
    }
    return retType::UNDEF;
}

//@brief: visit Equal node, including '==' & '!=' between expressions
//@ret: Expr type: INT
antlrcpp::Any CodeGenVisitor::visitEqual(MiniDecafParser::EqualContext *ctx) {
    // Get the type of left & right expressions
    retType lExpr = visit(ctx->equ(0)), rExpr = visit(ctx->equ(1));
    /*
        Nearly the same as FUNC<visitAddSub>
    */
    code_ << popReg("t0", "t1");
    if (lExpr == retType::LEFT) {
        code_ << "\tlw t0, (t0)\n";
    }
    if (rExpr == retType::LEFT) {
        code_ << "\tlw t1, (t1)\n";
    }
    code_ << "\tsub t0, t0, t1\n";
    if (ctx->EQ()) {
        code_ << "\tseqz a0, t0\n";
    } else if (ctx->NEQ()) {
        code_ << "\tsnez a0, t0\n";
    }
    code_ << pushReg("a0");
    return retType::RIGHT;
}

//@brief: visit LessGreat node, including '<=', '<', '>' & '>=' between expressions
//@ret: Expr type: INT
antlrcpp::Any CodeGenVisitor::visitLessGreat(MiniDecafParser::LessGreatContext *ctx) {
    // Get the type of left & right expressions
    retType lExpr = visit(ctx->rel(0)), rExpr = visit(ctx->rel(1));
    /*
        Nearly the same as FUNC<visitAddSub>
    */
    code_ << popReg("t0", "t1");
    if (lExpr == retType::LEFT) {
        code_ << "\tlw t0, (t0)\n";
    }
    if (rExpr == retType::LEFT) {
        code_ << "\tlw t1, (t1)\n";
    }
    if (ctx->LE()) {
        code_ << "\tsgt a0, t0, t1\n"
              << "\txori a0, a0, 1\n";
    } else if (ctx->LT()) {
        code_ << "\tslt a0, t0, t1\n";
    } else if (ctx->GE()) {
        code_ << "\tslt a0, t0, t1\n"
              << "\txori a0, a0, 1\n";
    } else if (ctx->GT()) {
        code_ << "\tsgt a0, t0, t1\n";
    }
    code_ << pushReg("a0");
    return retType::RIGHT;
}

//@brief: visit Land node, including '&&' between expressions
//@ret: Expr type: INT
antlrcpp::Any CodeGenVisitor::visitLand(MiniDecafParser::LandContext *ctx) {
    // Get the type of left & right expressions
    retType lExpr = visit(ctx->land_op(0)), rExpr = visit(ctx->land_op(1));
    /*
        Nearly the same as FUNC<visitAddSub>
    */
    code_ << popReg("t0", "t1");
    if (lExpr == retType::LEFT) {
        code_ << "\tlw t0, (t0)\n";
    }
    if (rExpr == retType::LEFT) {
        code_ << "\tlw t1, (t1)\n";
    }
    code_ << "\tmul a0, t0, t1\n"
          << "\tsnez a0, a0\n"
          << pushReg("a0");
    return retType::RIGHT;
}

//@brief: visit Lor node, including '||' between expressions
//@ret: Expr type: INT
antlrcpp::Any CodeGenVisitor::visitLor(MiniDecafParser::LorContext *ctx) {
    // Get the type of left & right expressions
    retType lExpr = visit(ctx->lor_op(0)), rExpr = visit(ctx->lor_op(1));
    /*
        Nearly the same as FUNC<visitAddSub>
    */
    code_ << popReg("t0", "t1");
    if (lExpr == retType::LEFT) {
        code_ << "\tlw t0, (t0)\n";
    }
    if (rExpr == retType::LEFT) {
        code_ << "\tlw t1, (t1)\n";
    }
    code_ << "\tor a0, t0, t1\n"
          << pushReg("a0");
    return retType::RIGHT;
}

//@brief: visit GlobalVar node
antlrcpp::Any CodeGenVisitor::visitGlobalVar(MiniDecafParser::GlobalVarContext *ctx) {
    /*
       Global varibles are stored in .bss or .data segments
       Initialized: .data; Uninitialized: .bss
       They can be accessed by their label
    */
    std::string varName = ctx->Identifier()->getText();
    if (ctx->Interger()) {
        data_ << ".globl " << varName << "\n"
              << varName << ":\n" << "\t.word " << ctx->Interger()->getText() << "\n";
    } else {
        bss_ << ".globl " << varName << "\n"
             << varName << ":\n" << ".space 4\n";
    }
    return retType::UNDEF;
}

//@brief: visit VarDef node, using rvalue to initialize the defined variable 
//@ret: Variable type
antlrcpp::Any CodeGenVisitor::visitVarDef(MiniDecafParser::VarDefContext *ctx) {
    std::string varName = ctx->Identifier()->getText();
    if (ctx->expr()) {
        retType type = visit(ctx->expr());
        // Store the rvalue to the address of current variable
        code_ << popReg("a0");
        if (type == retType::LEFT) {
            code_ << "\tlw a0, (a0)\n";
        }
        code_ << "\tsw a0, " << -4 - 4 * Singleton::getInstance().symbolTable[curFunc][varName].get()->getOffset() << "(fp)\n";
    }
    return retType::UNDEF;
}

//@brief: visit Identifier node, we read the value of variable from stack 
//@ret: Variable type
antlrcpp::Any CodeGenVisitor::visitIdentifier(MiniDecafParser::IdentifierContext *ctx) {
    std::string varName = ctx->Identifier()->getText();
    std::string tmpFunc = curFunc;
    int tmpDep = blockDep;
    int line = ctx->start->getLine();
    for (int i = 0; i <= tmpDep; ++i) {
        if (Singleton::getInstance().symbolTable[tmpFunc].count(varName) == 0 || Singleton::getInstance().symbolTable[tmpFunc][varName].get()->getLineNum() > line) {
            int pos = tmpFunc.find_last_of('@');
            tmpFunc = tmpFunc.substr(0, pos);
            continue;
        } 
        // Load the value from stack
        code_ << "\taddi a0, fp, " << -4 - 4 * Singleton::getInstance().symbolTable[tmpFunc][varName].get()->getOffset() << "\n"
              << pushReg("a0");
        return retType::LEFT;
    }
    // Finally, we retrieve target varible from global scope
    // "la t0, <label>", load the label address to the register
    if (Singleton::getInstance().symbolTable["global"].count(varName) > 0) {
        code_ << "\tla a0, " << varName << "\n"
              << pushReg("a0");
    }
    return retType::LEFT;
}

//@brief: Visit Assign node, eg. for expr "a = 3", we first find a's position in stack,
// then store 3 into specified memory.
//@ret: Variable type
antlrcpp::Any CodeGenVisitor::visitAssign(MiniDecafParser::AssignContext *ctx) {
    retType lType = visit(ctx->factor());
    retType rType = visit(ctx->expr());
    code_ << popReg("t1");
    if (rType == retType::LEFT) {
        code_ << "\tlw t1, (t1)\n";
    } 
    code_ << popReg("t0")
          << "\tsw t1, (t0)\n"
          << pushReg("t0");
    return retType::LEFT;
}

//@brief: Visit Ifstmt node, generate corresponding branch & jump instructions
antlrcpp::Any CodeGenVisitor::visitIfStmt(MiniDecafParser::IfStmtContext *ctx) {
    retType type = visit(ctx->expr());
    code_ << popReg("a0");
    if (type == retType::LEFT) {
        code_ << "\tlw a0, (a0)\n";
    }
    // If statement with else branch
    if (ctx->Else()) {
        /* Different branch label, in RV-32 assembly, a branch operaion is like:
            ...
            beqz a0, label1
            label0:
                <instructions>
            label1:
                <instructions>
            ...
        */
        int elseBranch = labelOrder++;
        int endBranch = labelOrder++;
        code_ << "\tbeqz a0, label_" << elseBranch << "\n";
        visit(ctx->stmt(0));
        code_ << "\tj label_" << endBranch << "\n";
        code_ << "label_" << elseBranch << ":\n";
        visit(ctx->stmt(1));
        code_ << "label_" << endBranch << ":\n";
    } else {
        // If statement without else branch
        int endBranch = labelOrder++;
        code_ << "\tbeqz a0, label_" << endBranch << "\n";
        visit(ctx->stmt(0));
        code_ << "label_" << endBranch << ":\n";
    }
    return retType::UNDEF;
}

antlrcpp::Any CodeGenVisitor::visitCondExpr(MiniDecafParser::CondExprContext *ctx) {
    /*
        Conditional tenary expression (x = a : b ? c) can be translated as:
        if (a) x = b; else x = c;
        Noted that the tenary expression has short-circuit property, which means
        if a is true, expression c cannot be reached.
    */
    // Code generation part is nearly the same as <ifStmt>
    retType type = visit(ctx->lor_op());
    int elseBranch = labelOrder++;
    int endBranch = labelOrder++;
    code_ << popReg("a0");
    if (type == retType::LEFT) {
        code_ << "\tlw a0, (a0)\n";
    }
    code_ << "\tbeqz a0, label_" << elseBranch << "\n";
    visit(ctx->expr());
    code_ << "\tj label_" << endBranch << "\n";
    code_ << "label_" << elseBranch << ":\n";
    visit(ctx->cond());
    code_ << "label_" << endBranch << ":\n";
    return retType::RIGHT;
}

//@brief: Visit forloop node, generate branch model in RV32
antlrcpp::Any CodeGenVisitor::visitForLoop(MiniDecafParser::ForLoopContext *ctx) {
    // According to our SPEC, forloop is also a new scope
    curFunc += "@" + std::to_string(blockOrder) + std::to_string(++blockDep);
    int startBranch = labelOrder++, endBranch = labelOrder++, continueBranch = labelOrder++;
    breakTarget.push_back(endBranch);
    continueTarget.push_back(continueBranch);

    int exprBase = -1;
    // for (<decl|expr>; <expr>; <expr>) <stmt>
    // The Branch model is nearly the same as Dowhile
    if (ctx->decl()) {
        visit(ctx->decl());
    } else if (ctx->expr(0)) {
        visit(ctx->expr(0));
        exprBase = 0;
    }
    code_ << "label_" << startBranch << ":\n";
    if (ctx->expr(exprBase+1)) {
        retType type = visit(ctx->expr(exprBase+1));
        code_ << popReg("a0");
        if (type == retType::LEFT) {
            code_ << "\tlw a0, (a0)\n";
        }
        code_ << "\tbeqz a0, label_" << endBranch << "\n";
    } 
    visit(ctx->stmt());
    code_ << "label_" << continueBranch << ":\n";
    if (ctx->expr(exprBase+2)) {
        visit(ctx->expr(exprBase+2));
    }
    code_ << "\tj label_" << startBranch << "\n"
                << "label_" << endBranch << ":\n";
    breakTarget.pop_back();
    continueTarget.pop_back();
    // When exit for loop scope, maintain the block depth & block order
    if (--blockDep == 0) {
        ++blockOrder;
    }
    int pos = curFunc.find_last_of('@');
    curFunc = curFunc.substr(0, pos);
    return retType::UNDEF;
}

//@brief: Nearly the same as DoWhile
antlrcpp::Any CodeGenVisitor::visitWhileLoop(MiniDecafParser::WhileLoopContext *ctx) {
    int startBranch = labelOrder++, endBranch = labelOrder++;
    breakTarget.push_back(endBranch);
    continueTarget.push_back(startBranch);
    code_ << "label_" << startBranch << ":\n";
    retType type = visit(ctx->expr());
    code_ << popReg("a0");
    if (type == retType::LEFT) {
        code_ << "\tlw a0, (a0)\n";
    }
    code_ << "\tbeqz a0, label_" << endBranch << "\n";
    visit(ctx->stmt());
    code_ << "\tj label_" << startBranch << "\n"
                << "label_" << endBranch << ":\n";
    breakTarget.pop_back();
    continueTarget.pop_back();
    return retType::UNDEF;
}

//@brief: do <some statements> while <expr>
antlrcpp::Any CodeGenVisitor::visitDoWhile(MiniDecafParser::DoWhileContext *ctx) {
    /*
        A loop model in RV32 assembly is like:
        ...
        label_0:
            <instructions>
            bnez a0, label_0
        label_1:
        ...
    */
    int startBranch = labelOrder++, endBranch = labelOrder++;
    breakTarget.push_back(endBranch);
    continueTarget.push_back(startBranch);

    code_ << "label_" << startBranch << ":\n";
    visit(ctx->stmt());
    retType type = visit(ctx->expr());
    code_ << popReg("a0");
    if (type == retType::LEFT) {
        code_ << "\tlw a0, (a0)\n";
    }
    code_ << "\tbnez a0, label_" << startBranch << "\n";
    code_ << "label_" << endBranch << ":\n";

    // We push the label into vector to deal with nested loop
    breakTarget.pop_back();
    continueTarget.pop_back();
    return retType::UNDEF;
}

//@brief: Break out of the current loop layer
antlrcpp::Any CodeGenVisitor::visitBreak(MiniDecafParser::BreakContext *ctx) {
    code_ << "\tj label_" << breakTarget.back() << "\n";
    return retType::UNDEF;
}

//@brief: Jump to the tail of the current loop layer
antlrcpp::Any CodeGenVisitor::visitContinue(MiniDecafParser::ContinueContext *ctx) {
    code_ << "\tj label_" << continueTarget.back() << "\n";
    return retType::UNDEF; 
}

//@brief: Pop unused element at the top of stack
antlrcpp::Any CodeGenVisitor::visitSingleExpr(MiniDecafParser::SingleExprContext *ctx) {
    if (ctx->expr()) {
        visit(ctx->expr());
        code_ << popReg("t0");
    }
    return retType::UNDEF;
}

//@brief: Deal with cast operation
antlrcpp::Any CodeGenVisitor::visitCast(MiniDecafParser::CastContext *ctx) {
    retType type = visit(ctx->factor());
    return type;
}

std::string CodeGenVisitor::pushReg(std::string reg) {
    return "\taddi sp, sp, -4\n\tsw " + reg + ", (sp)\n";
}

std::string CodeGenVisitor::popReg(std::string reg) {
    return "\tlw " + reg + ", (sp)\n\taddi sp, sp, 4\n";
}

std::string CodeGenVisitor::popReg(std::string reg0, std::string reg1) {
    return "\tlw " + reg0 + ", 4(sp)\n\tlw " + reg1 + ", (sp)\n\taddi sp, sp, 8\n";
}