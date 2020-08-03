#include <stdexcept>

#include "VarAllocVisitor.h"

VarAllocVisitor::Map<VarAllocVisitor::Map<int>> VarAllocVisitor::allocVar(const std::shared_ptr<ASTNode> &op) {
    (*this)(op);
    return varMap_;
}

void VarAllocVisitor::visit(const FunctionNode *op) {
    curFunc_ = op->name_;
    offset_ = 0;
    varMap_[curFunc_] = Map<int>();
    for (auto &&arg : op->args_) {
        varMap_[curFunc_][arg->name_] = offset_++;  // copy the args to another var
    }
    (*this)(op->body_);
}

void VarAllocVisitor::visit(const VarNode *op) {
    if (!varMap_.at(curFunc_).count(op->name_)) {
        throw std::runtime_error("Var " + op->name_ + " is used before definition");
    }
}

void VarAllocVisitor::visit(const AssignNode *op) {
    if (!varMap_.at(curFunc_).count(op->var_->name_)) {
        varMap_[curFunc_][op->var_->name_] = offset_++;
    }
    Visitor::visit(op);
}

