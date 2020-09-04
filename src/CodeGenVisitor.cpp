#include "CodeGenVisitor.h"

// @brief: Start visiting the syntax tree from root node Prog
// @ret: Generated asm code
antlrcpp::Any CodeGenVisitor::visitProg(MiniDecafParser::ProgContext *ctx) {
    code_ << ".section .text\n"
        << ".globl main\n"
        << "main:\n"; 
    visitChildren(ctx);
    return code_.str();
}

// @brief: Visit ReturnStmt node, son of Stmt node
antlrcpp::Any CodeGenVisitor::visitReturnStmt(MiniDecafParser::ReturnStmtContext *ctx) {
    visitChildren(ctx);
    code_ << "\tret";
    return retType::UNDEF;
}

// @brief: Visit Integer node, which loads an immediate number into register
// @ret: Expr type: Int
antlrcpp::Any CodeGenVisitor::visitInteger(MiniDecafParser::IntegerContext *ctx) {
    code_ << "\tli a0, " << ctx->getText() << "\n"
          << push;
    return retType::INT;
}

// @brief: Visit unaryOp node, like '-1', '~12', '!89' and etc. 
// @ret: Expr type: Int
antlrcpp::Any CodeGenVisitor::visitUnaryOp(MiniDecafParser::UnaryOpContext *ctx) {
    visitChildren(ctx);
    /* 
        ctx-><token_name>() is a ptr point to the token
        ptr == nullptr indicates that the token is undeclared
    */
    if (ctx->Minus()) {
        code_ << pop1
            << "\tsub a0, x0, t0\n"
            << push;
        return retType::INT;
    } else if(ctx->Exclamation()) {
        code_ << pop1
            << "\tseqz a0, t0\n"
            << push; 
        return retType::INT;
    } else if(ctx->Tilde()) {
        code_ << pop1
            << "\tnot a0, t0\n"
            << push;
        return retType::INT;
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
    retType lExpr = visit(ctx->expr(0)), rExpr = visit(ctx->expr(1));
    /*
        The use of token ptr is the same as FUNC<visitUnaryOp>.
        In this step, stack machine first pops the 2 operators at the top of stack,
        then computes the result, finally pushes the result to the stack.
    */
    code_ << pop2;
    if (ctx->Addition()) {
        code_ << "\tadd a0, t0, t1\n"
              << push;
        return retType::INT;
    } else if (ctx->Minus()) {
        code_ << "\tsub a0, t0, t1\n"
              << push;
        return retType::INT;
    }
    return retType::UNDEF;
}

//@brief: visit MulDiv node, including multiplication, division & modular between expressions
//@ret: Expr type: depending on the left & right expressions
antlrcpp::Any CodeGenVisitor::visitMulDiv(MiniDecafParser::MulDivContext *ctx) {
    retType lExpr = visit(ctx->expr(0)), rExpr = visit(ctx->expr(1));
    /*
        Totally the same as FUNC<visitAddSub>.
    */
    code_ << pop2;
    if (ctx->Multiplication()) {
        code_ << "\tmul a0, t0, t1\n"
              << push;
        return retType::INT;
    } else if (ctx->Division()) {
        code_ << "\tdiv a0, t0, t1\n"
              << push;
        return retType::INT;
    } else if (ctx->Modular()) {
        code_ << "\trem a0, t0, t1\n"
              << push;
        return retType::INT;
    }
    return retType::UNDEF;
}