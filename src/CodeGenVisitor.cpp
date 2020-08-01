#include "CodeGenVisitor.h"

std::string CodeGenVisitor::genCode(const std::shared_ptr<ASTNode> &op) {
    os << ".global main\n"
          "main:\n";
    (*this)(op);
    os << "addi sp, sp, 8\n"
          "ret\n";
    return os.str();
}

void CodeGenVisitor::visit(const IntegerNode *op) {
    Visitor::visit(op);
    os << "ori a0, x0, " << op->literal_ << "\n" << push;
}

void CodeGenVisitor::visit(const AddNode *op) {
    Visitor::visit(op);
    os << pop2 << "add a0, t0, t1\n" << push;
}

void CodeGenVisitor::visit(const SubNode *op) {
    Visitor::visit(op);
    os << pop2 << "sub a0, t0, t1\n" << push;
}

void CodeGenVisitor::visit(const MulNode *op) {
    Visitor::visit(op);
    os << pop2 << "mul a0, t0, t1\n" << push;
}

void CodeGenVisitor::visit(const DivNode *op) {
    Visitor::visit(op);
    os << pop2 << "div a0, t0, t1\n" << push;
}

void CodeGenVisitor::visit(const LTNode *op) {
    Visitor::visit(op);
    os << pop2 << "slt a0, t0, t1\n" << push;
}

void CodeGenVisitor::visit(const LENode *op) {
    Visitor::visit(op);
    os << pop2 << "sgt a0, t0, t1\n"
                  "xori a0, a0, 1\n" << push;
}

void CodeGenVisitor::visit(const GTNode *op) {
    Visitor::visit(op);
    os << pop2 << "sgt a0, t0, t1\n" << push;
}

void CodeGenVisitor::visit(const GENode *op) {
    Visitor::visit(op);
    os << pop2 << "slt a0, t0, t1\n"
                  "xori a0, a0, 1\n" << push;
}

void CodeGenVisitor::visit(const EQNode *op) {
    Visitor::visit(op);
    os << pop2 << "sub t0, t0, t1\n"
                  "seqz a0, t0\n" << push;
}

void CodeGenVisitor::visit(const NENode *op) {
    Visitor::visit(op);
    os << pop2 << "sub t0, t0, t1\n"
                  "snez a0, t0\n" << push;
}

