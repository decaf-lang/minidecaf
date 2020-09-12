#ifndef _PARSER_H_
#define _PARSER_H_

#include <list>
#include "lexer.h"
#include "error.h"

enum NodeKind{
    ND_RETURN,      // Return 语句
    ND_NUM,         // 数字字面量
    ND_NOT,         // Unary !
    ND_BITNOT,      // Unary ~
    ND_NEG,         // Unary -
    ND_ADD,         // Binary +
    ND_SUB,         // Binary -
    ND_MUL,         // Binary *
    ND_DIV,         // Binary /
    ND_MOD,         // Binary %
    // ND_GT,       // Binary >     // 为啥被注释了？
    // ND_GTE,      // Binary >=
    ND_LT,          // Binary <
    ND_LTE,         // Binary <=
    ND_EQ,          // Binary ==
    ND_NEQ,         // Binary !=
    ND_LOGAND,      // Binary &&
    ND_LOGOR,       // Binary ||
};

struct Node {
    NodeKind kind;
    int val;                        // ND_NUM 的值
    std::shared_ptr<Node> lexpr;    // left expression
    std::shared_ptr<Node> rexpr;    // right expr, 一个已经不够用了
    TKPtr tok;                      // 为了报错，不做要求，可以忽略
};

typedef std::shared_ptr<Node> NDPtr;

struct Function {
    char *name;
    std::list<NDPtr> nodes;
};

typedef std::shared_ptr<Function> FNPtr;

struct Program {
    std::list<FNPtr> funcs;
};

Program* parsing(std::list<TKPtr>* toks);

extern const int POINTER_WIDTH;
extern const int POINTER_WIDTH_LOG;

#endif // _PARSER_H_