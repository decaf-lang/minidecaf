#ifndef VAR_ALLOC_VISITOR_H_
#define VAR_ALLOC_VISITOR_H_

#include <unordered_map>

#include "Visitor.h"

class VarAllocVisitor : public Visitor {
public:
    template <class T>
    using Map = std::unordered_map<std::string, T>;

    Map<Map<int>> allocVar(const std::shared_ptr<ASTNode> &op);

protected:
    virtual void visit(const FunctionNode *op) override;
    virtual void visit(const VarNode *op) override;
    virtual void visit(const VarDefNode *op) override;

private:
    int offset_;
    std::string curFunc_;
    Map<Map<int>> varMap_;  // function -> var name -> stack offset
};

#endif  // VAR_ALLOC_VISITOR_H_
