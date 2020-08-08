#ifndef ANNOTATE_TYPE_INFO_
#define ANNOTATE_TYPE_INFO_

#include <unordered_map>

#include "Mutator.h"

class AnnotateTypeInfo : public Mutator {
public:
    std::shared_ptr<ProgramNode> annotate(
            const std::shared_ptr<ProgramNode> &op, const std::unordered_map<std::string, ExprType> &types);

protected:
    std::shared_ptr<FunctionNode> mutate(const FunctionNode *op) override;
    std::shared_ptr<ExprNode> mutate(const CallNode *op) override;
    std::shared_ptr<ExprNode> mutate(const VarNode *op) override;

private:
    const std::unordered_map<std::string, ExprType> *types_;
    std::string curFunc_;
};

#endif  // ANNOTATE_TYPE_INFO_
