#include <stdexcept>

#include "VarAllocVisitor.h"

VarAllocVisitor::Map<int> VarAllocVisitor::allocVar(const std::shared_ptr<ASTNode> &op) {
    (*this)(op);
    return varMap_;
}

void VarAllocVisitor::visit(const FunctionNode *op) {
    curFunc_ = op->name_;
    offset_ = 0;
    for (auto &&arg : op->args_) {
        varMap_[curFunc_ + "/" + arg.second] = offset_++;  // copy the args to another var
    }
    (*this)(op->body_);
}

void VarAllocVisitor::visit(const VarNode *op) {
    if (!varMap_.count(curFunc_ + "/" + op->name_)) {
        throw std::runtime_error("Var " + op->name_ + " is used before definition");
    }
}

void VarAllocVisitor::visit(const VarDefNode *op) {
    if (!varMap_.count(curFunc_ + "/" + op->name_)) {
        varMap_[curFunc_ + "/" + op->name_] = offset_++;
    } else {
        throw std::runtime_error("Var " + op->name_ + " is already defined");
    }
    Visitor::visit(op);
}

