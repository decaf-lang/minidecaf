#ifndef AST_NODE_
#define AST_NODE_

#include <vector>
#include <memory>
#include <stdexcept>

enum class ASTNodeType : int {
    StmtSeq,
    Integer, Var,
    Assign, Invoke,
    IfThenElse,
    Add, Sub, Mul, Div,
    LT, LE, GT, GE, EQ, NE,
};

struct ASTNode {
    virtual ASTNodeType nodeType() const = 0;
    virtual ~ASTNode() {}
};

struct StmtNode : public ASTNode {};

struct ExprNode : public ASTNode {};

struct StmtSeqNode : public StmtNode {
    std::vector<std::shared_ptr<StmtNode>> stmts_;

    StmtSeqNode(const std::vector<std::shared_ptr<StmtNode>> &stmts) : stmts_(stmts) {}

    virtual ASTNodeType nodeType() const override {
        return ASTNodeType::StmtSeq;
    }

    static std::shared_ptr<StmtSeqNode> make(const std::vector<std::shared_ptr<StmtNode>> &stmts) {
        return std::make_shared<StmtSeqNode>(stmts);
    }
};

struct IntegerNode : public ExprNode {
    int literal_;

    IntegerNode(int literal) : literal_(literal) {}

    virtual ASTNodeType nodeType() const override {
        return ASTNodeType::Integer;
    }

    static std::shared_ptr<IntegerNode> make(int literal) {
        return std::make_shared<IntegerNode>(literal);
    }
};

struct VarNode : public ExprNode {
    std::string name_;

    VarNode(const std::string &name) : name_(name) {}

    virtual ASTNodeType nodeType() const override {
        return ASTNodeType::Var;
    }

    static std::shared_ptr<VarNode> make(const std::string &name) {
        return std::make_shared<VarNode>(name);
    }
};

/// Invoke a pure expression
struct InvokeNode : public StmtNode {
    std::shared_ptr<ExprNode> expr_;

    InvokeNode(const std::shared_ptr<ExprNode> &expr) : expr_(expr) {}

    virtual ASTNodeType nodeType() const override {
        return ASTNodeType::Invoke;
    }

    static std::shared_ptr<InvokeNode> make(const std::shared_ptr<ExprNode> &expr) {
        return std::make_shared<InvokeNode>(expr);
    }
};

struct AssignNode : public StmtNode {
    std::shared_ptr<VarNode> var_;
    std::shared_ptr<ExprNode> expr_;

    AssignNode(const std::shared_ptr<VarNode> &var, const std::shared_ptr<ExprNode> &expr)
        : var_(var), expr_(expr) {}

    virtual ASTNodeType nodeType() const override {
        return ASTNodeType::Assign;
    }

    static std::shared_ptr<AssignNode> make(std::shared_ptr<VarNode> var, std::shared_ptr<ExprNode> expr) {
        return std::make_shared<AssignNode>(var, expr);
    }
};

struct IfThenElseNode : public StmtNode {
    std::shared_ptr<ExprNode> cond_;
    std::shared_ptr<StmtNode> thenCase_, elseCase_;

    IfThenElseNode(const std::shared_ptr<ExprNode> &cond,
            const std::shared_ptr<StmtNode> &thenCase, const std::shared_ptr<StmtNode> &elseCase)
        : cond_(cond), thenCase_(thenCase), elseCase_(elseCase) {}

    virtual ASTNodeType nodeType() const override {
        return ASTNodeType::IfThenElse;
    }

    static std::shared_ptr<IfThenElseNode> make(const std::shared_ptr<ExprNode> &cond,
            const std::shared_ptr<StmtNode> &thenCase, const std::shared_ptr<StmtNode> &elseCase=nullptr) {
        return std::make_shared<IfThenElseNode>(cond, thenCase, elseCase);
    }
};

#define DEFINE_BINARY_NODE(name) \
    struct name##Node : public ExprNode { \
        std::shared_ptr<ExprNode> lhs_, rhs_; \
        \
        name##Node(std::shared_ptr<ExprNode> lhs, std::shared_ptr<ExprNode> rhs) : lhs_(lhs), rhs_(rhs) {} \
        \
        virtual ASTNodeType nodeType() const override { \
            return ASTNodeType::name; \
        } \
        \
        static std::shared_ptr<name##Node> make(std::shared_ptr<ExprNode> lhs, std::shared_ptr<ExprNode> rhs) { \
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
