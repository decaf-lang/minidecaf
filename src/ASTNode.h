#ifndef AST_NODE_
#define AST_NODE_

#include <vector>
#include <memory>
#include <stdexcept>

enum class ASTNodeType : int {
    Program,
    Function,
    StmtSeq,
    Integer, Var,
    Assign, Invoke,
    IfThenElse, While,
    Return,
    Call,
    Add, Sub, Mul, Div,
    LT, LE, GT, GE, EQ, NE,
};

struct ASTNode {
    virtual ASTNodeType nodeType() const = 0;
    virtual ~ASTNode() {}
};

#define DEFINE_NODE_TRAIT(name) \
    virtual ASTNodeType nodeType() const override { \
        return ASTNodeType::name; \
    }

struct StmtNode : public ASTNode {};

struct ExprNode : public ASTNode {};

struct VarNode;
struct FunctionNode : public ASTNode {
    std::string name_;
    std::vector<std::shared_ptr<VarNode>> args_;
    std::shared_ptr<StmtNode> body_;

    FunctionNode(const std::string &name,
            const std::vector<std::shared_ptr<VarNode>> &args, const std::shared_ptr<StmtNode> &body)
        : name_(name), args_(args), body_(body) {}

    static std::shared_ptr<FunctionNode> make(const std::string &name,
            const std::vector<std::shared_ptr<VarNode>> &args, const std::shared_ptr<StmtNode> &body) {
        return std::make_shared<FunctionNode>(name, args, body);
    }

    DEFINE_NODE_TRAIT(Function);
};

struct ProgramNode : public ASTNode {
    std::vector<std::shared_ptr<FunctionNode>> funcs_;

    ProgramNode(const std::vector<std::shared_ptr<FunctionNode>> &funcs) : funcs_(funcs) {}

    static std::shared_ptr<ProgramNode> make(const std::vector<std::shared_ptr<FunctionNode>> &funcs) {
        return std::make_shared<ProgramNode>(funcs);
    }

    DEFINE_NODE_TRAIT(Program)
};

struct StmtSeqNode : public StmtNode {
    std::vector<std::shared_ptr<StmtNode>> stmts_;

    StmtSeqNode(const std::vector<std::shared_ptr<StmtNode>> &stmts) : stmts_(stmts) {}

    static std::shared_ptr<StmtSeqNode> make(const std::vector<std::shared_ptr<StmtNode>> &stmts) {
        return std::make_shared<StmtSeqNode>(stmts);
    }

    DEFINE_NODE_TRAIT(StmtSeq);
};

struct IntegerNode : public ExprNode {
    int literal_;

    IntegerNode(int literal) : literal_(literal) {}

    static std::shared_ptr<IntegerNode> make(int literal) {
        return std::make_shared<IntegerNode>(literal);
    }

    DEFINE_NODE_TRAIT(Integer)
};

struct VarNode : public ExprNode {
    std::string name_;

    VarNode(const std::string &name) : name_(name) {}

    static std::shared_ptr<VarNode> make(const std::string &name) {
        return std::make_shared<VarNode>(name);
    }

    DEFINE_NODE_TRAIT(Var)
};

/// Invoke a pure expression
struct InvokeNode : public StmtNode {
    std::shared_ptr<ExprNode> expr_;

    InvokeNode(const std::shared_ptr<ExprNode> &expr) : expr_(expr) {}

    static std::shared_ptr<InvokeNode> make(const std::shared_ptr<ExprNode> &expr) {
        return std::make_shared<InvokeNode>(expr);
    }

    DEFINE_NODE_TRAIT(Invoke)
};

struct AssignNode : public StmtNode {
    std::shared_ptr<VarNode> var_;
    std::shared_ptr<ExprNode> expr_;

    AssignNode(const std::shared_ptr<VarNode> &var, const std::shared_ptr<ExprNode> &expr)
        : var_(var), expr_(expr) {}

    static std::shared_ptr<AssignNode> make(std::shared_ptr<VarNode> var, std::shared_ptr<ExprNode> expr) {
        return std::make_shared<AssignNode>(var, expr);
    }

    DEFINE_NODE_TRAIT(Assign)
};

struct IfThenElseNode : public StmtNode {
    std::shared_ptr<ExprNode> cond_;
    std::shared_ptr<StmtNode> thenCase_, elseCase_;

    IfThenElseNode(const std::shared_ptr<ExprNode> &cond,
            const std::shared_ptr<StmtNode> &thenCase, const std::shared_ptr<StmtNode> &elseCase)
        : cond_(cond), thenCase_(thenCase), elseCase_(elseCase) {}

    static std::shared_ptr<IfThenElseNode> make(const std::shared_ptr<ExprNode> &cond,
            const std::shared_ptr<StmtNode> &thenCase, const std::shared_ptr<StmtNode> &elseCase=nullptr) {
        return std::make_shared<IfThenElseNode>(cond, thenCase, elseCase);
    }

    DEFINE_NODE_TRAIT(IfThenElse)
};

struct WhileNode : public StmtNode {
    std::shared_ptr<ExprNode> cond_;
    std::shared_ptr<StmtNode> body_;

    WhileNode(const std::shared_ptr<ExprNode> &cond, const std::shared_ptr<StmtNode> &body)
        : cond_(cond), body_(body) {}

    static std::shared_ptr<WhileNode> make(
            const std::shared_ptr<ExprNode> &cond, const std::shared_ptr<StmtNode> &body) {
        return std::make_shared<WhileNode>(cond, body);
    }

    DEFINE_NODE_TRAIT(While)
};

struct ReturnNode : public StmtNode {
    std::shared_ptr<ExprNode> expr_;

    ReturnNode(const std::shared_ptr<ExprNode> &expr) : expr_(expr) {}

    static std::shared_ptr<ReturnNode> make(const std::shared_ptr<ExprNode> &expr) {
        return std::make_shared<ReturnNode>(expr);
    }

    DEFINE_NODE_TRAIT(Return);
};

struct CallNode : public ExprNode {
    std::string callee_;
    std::vector<std::shared_ptr<ExprNode>> args_;

    CallNode(const std::string &callee, const std::vector<std::shared_ptr<ExprNode>> &args)
        : callee_(callee), args_(args) {}

    static std::shared_ptr<CallNode> make(const std::string &callee, const std::vector<std::shared_ptr<ExprNode>> &args) {
        return std::make_shared<CallNode>(callee, args);
    }

    DEFINE_NODE_TRAIT(Call)
};

#define DEFINE_BINARY_NODE(name) \
    struct name##Node : public ExprNode { \
        std::shared_ptr<ExprNode> lhs_, rhs_; \
        \
        name##Node(std::shared_ptr<ExprNode> lhs, std::shared_ptr<ExprNode> rhs) : lhs_(lhs), rhs_(rhs) {} \
        \
        static std::shared_ptr<name##Node> make(std::shared_ptr<ExprNode> lhs, std::shared_ptr<ExprNode> rhs) { \
            return std::make_shared<name##Node>(lhs, rhs); \
        } \
        \
        DEFINE_NODE_TRAIT(name) \
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
