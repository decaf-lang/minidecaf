#ifndef VAR_ALLOC_VISITOR_H_
#define VAR_ALLOC_VISITOR_H_

#include <unordered_map>

#include "Visitor.h"

class VarAllocVisitor : public Visitor {
public:
    std::unordered_map<std::string, int> allocVar(const std::shared_ptr<ASTNode> &op);

protected:
    virtual void visit(const VarNode *op) override;
    virtual void visit(const AssignNode *op) override;

private:
    int offset = 0;
    std::unordered_map<std::string, int> varMap_;
};

#endif  // VAR_ALLOC_VISITOR_H_
