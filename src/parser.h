#ifndef _PARSER_H_
#define _PARSER_H_

#include <list>
#include "lexer.h"
#include "error.h"

enum NodeKind{
    // Statement
    ND_RETURN,      // Return statement
    ND_DECL,        // Local variable declaration
    ND_UNUSED_EXPR, // Statements := <expr> ";"
    // Expression
    ND_NUM,         // 数字字面量
    ND_NOT,         // Unary !
    ND_BITNOT,      // Unary ~
    ND_NEG,         // Unary -
    ND_ADD,         // Binary +
    ND_SUB,         // Binary -
    ND_MUL,         // Binary *
    ND_DIV,         // Binary /
    ND_MOD,         // Binary %
    // ND_GT,       // Binary > 
    // ND_GTE,      // Binary >=
    ND_LT,          // Binary <
    ND_LTE,         // Binary <=
    ND_EQ,          // Binary ==
    ND_NEQ,         // Binary !=
    ND_LOGAND,      // Binary &&
    ND_LOGOR,       // Binary ||
    ND_VAR,         // Local variable
    ND_ASSIGN,      // Binary =
};

struct Node;

typedef std::shared_ptr<Node> NDPtr;

struct Var {
    char *name;
    int offset;     // Offset from %fp
    NDPtr init;
    TKPtr tok;
};

typedef std::shared_ptr<Var> VarPtr;

struct Node {
    NodeKind kind;
    int val;                        // ND_NUM 的值
    NDPtr lexpr;                    // left expression
    NDPtr rexpr;                    // right expr, 一个已经不够用了
    VarPtr var;                     // ND_VAR 对应的变量
    TKPtr tok;                      // 为了报错，不做要求，可以忽略
};

struct Function {
    char *name;
    // 语句
    std::list<NDPtr> stmts;
    // 局部变量，目前没什么用
    std::list<VarPtr> locals;
    // 栈帧大小， 目前 = (locals.size()+2) * POINTER_WIDTH
    int stack_size;
};

typedef std::shared_ptr<Function> FNPtr;

struct Program {
    std::list<FNPtr> funcs;
};

Program* parsing(std::list<TKPtr>* toks);

extern const int POINTER_WIDTH;
extern const int POINTER_WIDTH_LOG;

#endif // _PARSER_H_