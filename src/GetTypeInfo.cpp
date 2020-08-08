#include "GetTypeInfo.h"

std::unordered_map<std::string, ExprType> GetTypeInfo::get(const std::shared_ptr<ASTNode> &op) {
    (*this)(op);
    return types_;
}

void GetTypeInfo::visit(const FunctionNode *op) {
    curFunc_ = op->name_;
    types_[op->name_] = op->type_;
    for (auto &&arg : op->args_) {
        types_[curFunc_ + "/" + arg.second] = arg.first;
    }
    Visitor::visit(op);
}

void GetTypeInfo::visit(const VarDefNode *op) {
    types_[curFunc_ + "/" + op->name_] = op->type_;
    Visitor::visit(op);
}

