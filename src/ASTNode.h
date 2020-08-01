#ifndef AST_NODE_
#define AST_NODE_

#include <memory>

enum class ASTNodeType : int {
    Integer,
    Add, Sub, Mul, Div,
    LT, LE, GT, GE, EQ, NE,
};

struct ASTNode {
    virtual ASTNodeType nodeType() const = 0;
};

struct IntegerNode : public ASTNode {
    int literal_;

    IntegerNode(int literal) : literal_(literal) {}

    virtual ASTNodeType nodeType() const {
        return ASTNodeType::Integer;
    }

    static std::shared_ptr<IntegerNode> make(int literal) {
        return std::make_shared<IntegerNode>(literal);
    }
};

#define DEFINE_BINARY_NODE(name) \
    struct name##Node : public ASTNode { \
        std::shared_ptr<ASTNode> lhs_, rhs_; \
        \
        name##Node(std::shared_ptr<ASTNode> lhs, std::shared_ptr<ASTNode> rhs) : lhs_(lhs), rhs_(rhs) {} \
        \
        virtual ASTNodeType nodeType() const { \
            return ASTNodeType::name; \
        } \
        \
        static std::shared_ptr<name##Node> make(std::shared_ptr<ASTNode> lhs, std::shared_ptr<ASTNode> rhs) { \
            return std::make_shared<name##Node>(lhs, rhs); \
        } \
    };

DEFINE_BINARY_NODE(Add)
DEFINE_BINARY_NODE(Sub)
DEFINE_BINARY_NODE(Mul)
DEFINE_BINARY_NODE(Div)
DEFINE_BINARY_NODE(LT)
DEFINE_BINARY_NODE(LE)
DEFINE_BINARY_NODE(GT)
DEFINE_BINARY_NODE(GE)
DEFINE_BINARY_NODE(EQ)
DEFINE_BINARY_NODE(NE)

#endif  // AST_NODE_
