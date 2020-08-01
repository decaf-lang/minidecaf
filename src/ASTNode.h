#ifndef AST_NODE_
#define AST_NODE_

#include <vector>
#include <memory>
#include <stdexcept>

enum class ASTNodeType : int {
    StmtSeq,
    Integer, Var,
    Assign, Invoke,
    Add, Sub, Mul, Div,
    LT, LE, GT, GE, EQ, NE,
};

struct ASTNode {
    virtual ASTNodeType nodeType() const = 0;
    virtual ~ASTNode() {}
};

#define CHECK_NODE_TYPE(node, type) \
    if (node->nodeType() != ASTNodeType::type) { \
        throw std::runtime_error("Wrong AST node type"); \
    }

struct StmtSeqNode : public ASTNode {
    std::vector<std::shared_ptr<ASTNode>> stmts_;

    StmtSeqNode(const std::vector<std::shared_ptr<ASTNode>> &stmts) : stmts_(stmts) {}

    virtual ASTNodeType nodeType() const {
        return ASTNodeType::StmtSeq;
    }

    static std::shared_ptr<StmtSeqNode> make(const std::vector<std::shared_ptr<ASTNode>> &stmts) {
        return std::make_shared<StmtSeqNode>(stmts);
    }
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

struct VarNode : public ASTNode {
    std::string name_;

    VarNode(const std::string &name) : name_(name) {}

    virtual ASTNodeType nodeType() const {
        return ASTNodeType::Var;
    }

    static std::shared_ptr<VarNode> make(const std::string &name) {
        return std::make_shared<VarNode>(name);
    }
};

/// Invoke a pure expression
struct InvokeNode : public ASTNode {
    std::shared_ptr<ASTNode> expr_;

    InvokeNode(const std::shared_ptr<ASTNode> &expr) : expr_(expr) {}

    virtual ASTNodeType nodeType() const {
        return ASTNodeType::Invoke;
    }

    static std::shared_ptr<InvokeNode> make(const std::shared_ptr<ASTNode> &expr) {
        return std::make_shared<InvokeNode>(expr);
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

DEFINE_BINARY_NODE(Assign)
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
