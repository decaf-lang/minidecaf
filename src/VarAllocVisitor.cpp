#include <stdexcept>

#include "VarAllocVisitor.h"

std::unordered_map<std::string, int> VarAllocVisitor::allocVar(const std::shared_ptr<ASTNode> &op) {
    (*this)(op);
    return varMap_;
}

void VarAllocVisitor::visit(const VarNode *op) {
    if (!varMap_.count(op->name_)) {
        throw std::runtime_error("Var " + op->name_ + " is used before definition");
    }
}

void VarAllocVisitor::visit(const AssignNode *op) {
    if (!varMap_.count(op->var_->name_)) {
        varMap_[op->var_->name_] = offset++;
    }
    Visitor::visit(op);
}

