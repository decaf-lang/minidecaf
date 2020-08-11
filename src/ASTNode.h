#ifndef AST_NODE_
#define AST_NODE_

#include <vector>
#include <memory>

#include "error.h"

enum class ASTNodeType : int {
    Program,
    Function,
    StmtSeq,
    Integer, Var,
    VarDef,
    Assign, Invoke,
    IfThenElse, While,
    Return,
    Call,
    Cast,
    Add, Sub, Mul, Div,
    LNot,
    LT, LE, GT, GE, EQ, NE,
    LAnd, LOr, BAnd, BOr, BXor, SLL, SRA,
    Select,
};

struct ASTNode {
    virtual ASTNodeType nodeType() const = 0;
    virtual ~ASTNode() {}
};

template <class T, ASTNodeType TType, class U>
std::shared_ptr<T> as(const std::shared_ptr<U> &ptr) {
    ASSERT(ptr->nodeType() == TType);
    return std::static_pointer_cast<T>(ptr);
}

#define AS(ptr, name) as<name##Node, ASTNodeType::name>(ptr)

#define DEFINE_NODE_TRAIT(name) \
    virtual ASTNodeType nodeType() const override { \
        return ASTNodeType::name; \
    }

struct StmtNode : public ASTNode {};

enum class ExprType : int {
    Unknown,
    Int,
    Bool,
};

struct ExprNode : public ASTNode {
    ExprNode(ExprType type) : type_(type) {}
    ExprType type_;
};

struct FunctionNode : public ASTNode {
    ExprType type_;
    std::string name_;
    std::vector<std::pair<ExprType, std::string>> args_;
    std::shared_ptr<StmtNode> body_;

    FunctionNode(ExprType type, const std::string &name,
            const std::vector<std::pair<ExprType, std::string>> &args, const std::shared_ptr<StmtNode> &body)
        : type_(type), name_(name), args_(args), body_(body) {}

    static std::shared_ptr<FunctionNode> make(ExprType type, const std::string &name,
            const std::vector<std::pair<ExprType, std::string>> &args, const std::shared_ptr<StmtNode> &body) {
        return std::make_shared<FunctionNode>(type, name, args, body);
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

    IntegerNode(int literal) : ExprNode(ExprType::Int), literal_(literal) {}

    static std::shared_ptr<IntegerNode> make(int literal) {
        return std::make_shared<IntegerNode>(literal);
    }

    DEFINE_NODE_TRAIT(Integer)
};

struct VarNode : public ExprNode {
    std::string name_;

    VarNode(ExprType type, const std::string &name) : ExprNode(type), name_(name) {}

    static std::shared_ptr<VarNode> make(ExprType type, const std::string &name) {
        return std::make_shared<VarNode>(type, name);
    }

    DEFINE_NODE_TRAIT(Var)
};

struct VarDefNode : public StmtNode {
    ExprType type_;
    std::string name_;

    VarDefNode(ExprType type, const std::string &name) : type_(type), name_(name) {}

    static std::shared_ptr<VarDefNode> make(ExprType type, const std::string name) {
        return std::make_shared<VarDefNode>(type, name);
    }

    DEFINE_NODE_TRAIT(VarDef);
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
    std::string var_;
    std::shared_ptr<ExprNode> expr_;

    AssignNode(const std::string &var, const std::shared_ptr<ExprNode> &expr)
        : var_(var), expr_(expr) {}

    static std::shared_ptr<AssignNode> make(std::string var, std::shared_ptr<ExprNode> expr) {
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

    CallNode(ExprType type, const std::string &callee, const std::vector<std::shared_ptr<ExprNode>> &args)
        : ExprNode(type), callee_(callee), args_(args) {}

    static std::shared_ptr<CallNode> make(
            ExprType type, const std::string &callee, const std::vector<std::shared_ptr<ExprNode>> &args) {
        return std::make_shared<CallNode>(type, callee, args);
    }

    DEFINE_NODE_TRAIT(Call)
};

struct CastNode : public ExprNode {
    std::shared_ptr<ExprNode> expr_;

    CastNode(ExprType type, const std::shared_ptr<ExprNode> &expr) : ExprNode(type), expr_(expr) {}

    static std::shared_ptr<CastNode> make(ExprType type, const std::shared_ptr<ExprNode> &expr) {
        return std::make_shared<CastNode>(type, expr);
    }

    DEFINE_NODE_TRAIT(Cast)
};

struct LNotNode : public ExprNode {
    std::shared_ptr<ExprNode> expr_;

    LNotNode(const std::shared_ptr<ExprNode> &expr) : ExprNode(ExprType::Bool), expr_(expr) {}

    static std::shared_ptr<LNotNode> make(const std::shared_ptr<ExprNode> &expr) {
        return std::make_shared<LNotNode>(CastNode::make(ExprType::Bool, expr));
    }

    DEFINE_NODE_TRAIT(LNot)
};

struct SelectNode : public ExprNode {
    std::shared_ptr<ExprNode> cond_, thenCase_, elseCase_;

    SelectNode(const std::shared_ptr<ExprNode> &cond,
            const std::shared_ptr<ExprNode> &thenCase, const std::shared_ptr<ExprNode> &elseCase)
        : ExprNode(thenCase->type_), cond_(cond), thenCase_(thenCase), elseCase_(elseCase) {}

    static std::shared_ptr<SelectNode> make(const std::shared_ptr<ExprNode> &cond,
            const std::shared_ptr<ExprNode> &thenCase, const std::shared_ptr<ExprNode> &elseCase) {
        return std::make_shared<SelectNode>(cond, thenCase, elseCase);
    }

    DEFINE_NODE_TRAIT(Select)
};

#define DEFINE_BINARY_NODE(name, opType, retType) \
    struct name##Node : public ExprNode { \
        std::shared_ptr<ExprNode> lhs_, rhs_; \
        \
        name##Node(std::shared_ptr<ExprNode> lhs, std::shared_ptr<ExprNode> rhs) \
            : ExprNode(ExprType::retType), lhs_(lhs), rhs_(rhs) {} \
        \
        static std::shared_ptr<name##Node> make(std::shared_ptr<ExprNode> lhs, std::shared_ptr<ExprNode> rhs) { \
            return std::make_shared<name##Node>(CastNode::make(ExprType::opType, lhs), CastNode::make(ExprType::opType, rhs)); \
        } \
        \
        DEFINE_NODE_TRAIT(name) \
    };

DEFINE_BINARY_NODE(Add, Int, Int)
DEFINE_BINARY_NODE(Sub, Int, Int)
DEFINE_BINARY_NODE(Mul, Int, Int)
DEFINE_BINARY_NODE(Div, Int, Int)
DEFINE_BINARY_NODE(BAnd, Int, Int)
DEFINE_BINARY_NODE(BOr, Int, Int)
DEFINE_BINARY_NODE(BXor, Int, Int)
DEFINE_BINARY_NODE(SLL, Int, Int)
DEFINE_BINARY_NODE(SRA, Int, Int)
DEFINE_BINARY_NODE(LT, Int, Bool)
DEFINE_BINARY_NODE(LE, Int, Bool)
DEFINE_BINARY_NODE(GT, Int, Bool)
DEFINE_BINARY_NODE(GE, Int, Bool)
DEFINE_BINARY_NODE(EQ, Int, Bool)
DEFINE_BINARY_NODE(NE, Int, Bool)
DEFINE_BINARY_NODE(LAnd, Bool, Bool)
DEFINE_BINARY_NODE(LOr, Bool, Bool)

#endif  // AST_NODE_
