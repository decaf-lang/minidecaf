#include "AnnotateTypeInfo.h"
#include "error.h"

std::shared_ptr<ProgramNode> AnnotateTypeInfo::annotate(
        const std::shared_ptr<ProgramNode> &op, const std::unordered_map<std::string, ExprType> &types) {
    types_ = &types;
    return (*this)(op);
}

std::shared_ptr<FunctionNode> AnnotateTypeInfo::mutate(const FunctionNode *op) {
    curFunc_ = op->name_;
    return Mutator::mutate(op);
}

std::shared_ptr<ExprNode> AnnotateTypeInfo::mutate(const CallNode *op) {
    auto ret = AS(Mutator::mutate(op), Call);
    return CallNode::make(types_->at(ret->callee_), ret->callee_, ret->args_);
}

std::shared_ptr<ExprNode> AnnotateTypeInfo::mutate(const VarNode *op) {
    auto ret = AS(Mutator::mutate(op), Var);
    return VarNode::make(types_->at(curFunc_ + "/" + ret->name_), ret->name_);
}

