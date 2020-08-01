#ifndef CODE_GEN_VISITOR_H_
#define CODE_GEN_VISITOR_H_

#include <sstream>

#include "Visitor.h"

class CodeGenVisitor : public Visitor {
public:
    std::string genCode(const std::shared_ptr<ASTNode> &op);

protected:
    virtual void visit(const IntegerNode *op) override;
    virtual void visit(const AddNode *op) override;
    virtual void visit(const SubNode *op) override;
    virtual void visit(const MulNode *op) override;
    virtual void visit(const DivNode *op) override;
    virtual void visit(const LTNode *op) override;
    virtual void visit(const LENode *op) override;
    virtual void visit(const GTNode *op) override;
    virtual void visit(const GENode *op) override;
    virtual void visit(const EQNode *op) override;
    virtual void visit(const NENode *op) override;

private:
    const char *push =  "addi sp, sp, -8\n"
                        "sd a0, (sp)\n";
    const char *pop =   "ld t0, (sp)\n"
                        "addi sp, sp, 8\n";
    const char *pop2 =  "ld t0, 8(sp)\n"
                        "ld t1, (sp)\n"
                        "addi sp, sp, 16\n";

    std::ostringstream os;
};

#endif  // CODE_GEN_VISITOR_H_
