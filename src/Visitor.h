#ifndef VISITOR_H_
#define VISITOR_H_

#include <stdexcept>

#include "ASTNode.h"

class Visitor {
public:
    virtual void operator()(const std::shared_ptr<ASTNode> op) {
        switch (op->nodeType()) {

#define DISPATCH_CASE(name) \
            case ASTNodeType::name: \
                visit(static_cast<name##Node*>(op.get())); \
                break;

            DISPATCH_CASE(Integer)
            DISPATCH_CASE(Add)
            DISPATCH_CASE(Sub)
            DISPATCH_CASE(Mul)
            DISPATCH_CASE(Div)
            DISPATCH_CASE(LT)
            DISPATCH_CASE(LE)
            DISPATCH_CASE(GT)
            DISPATCH_CASE(GE)
            DISPATCH_CASE(EQ)
            DISPATCH_CASE(NE)

            default:
                throw std::runtime_error("Unrecognized ASTNodeType");
        }
    }

protected:
    virtual void visit(const IntegerNode *op) {}

    virtual void visit(const AddNode *op) {
        (*this)(op->lhs_);
        (*this)(op->rhs_);
    }

    virtual void visit(const SubNode *op) {
        (*this)(op->lhs_);
        (*this)(op->rhs_);
    }

    virtual void visit(const MulNode *op) {
        (*this)(op->lhs_);
        (*this)(op->rhs_);
    }

    virtual void visit(const DivNode *op) {
        (*this)(op->lhs_);
        (*this)(op->rhs_);
    }

    virtual void visit(const LTNode *op) {
        (*this)(op->lhs_);
        (*this)(op->rhs_);
    }

    virtual void visit(const LENode *op) {
        (*this)(op->lhs_);
        (*this)(op->rhs_);
    }

    virtual void visit(const GTNode *op) {
        (*this)(op->lhs_);
        (*this)(op->rhs_);
    }

    virtual void visit(const GENode *op) {
        (*this)(op->lhs_);
        (*this)(op->rhs_);
    }

    virtual void visit(const EQNode *op) {
        (*this)(op->lhs_);
        (*this)(op->rhs_);
    }

    virtual void visit(const NENode *op) {
        (*this)(op->lhs_);
        (*this)(op->rhs_);
    }

};

#endif  // VISITOR_H_
