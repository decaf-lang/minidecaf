#include "CodeGenVisitor.h"

std::string CodeGenVisitor::genCode(
        const std::shared_ptr<ASTNode> &op,
        const std::unordered_map<std::string, int> &varMap) {
    varMap_ = &varMap;
    os << ".global main\n"
          "main:\n"
          "sd fp, -8(sp)\n"
          "mv fp, sp\n";
    (*this)(op);
    os << "mv sp, fp\n"
          "ld fp, -8(sp)\n"
          "ret\n";
    return os.str();
}

void CodeGenVisitor::visit(const VarNode *op) {
    Visitor::visit(op);
    os << "ld a0, " << (-16 - 8 * varMap_->at(op->name_)) << "(fp)  # Load from " << op->name_ << "\n" << push;
}

void CodeGenVisitor::visit(const AssignNode *op) {
    CHECK_NODE_TYPE(op->lhs_, Var);
    const VarNode *var = static_cast<const VarNode*>(op->lhs_.get());
    os << "addi sp, fp, " << (-8 - 8 * (int)varMap_->size()) << "\n";
    (*this)(op->rhs_);
    os << "sd a0, " << (-16 - 8 * varMap_->at(var->name_)) << "(fp)  # Store to " << var->name_ << "\n";
}

void CodeGenVisitor::visit(const InvokeNode *op) {
    os << "addi sp, fp, " << (-8 - 8 * (int)varMap_->size()) << "\n";
    Visitor::visit(op);
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

