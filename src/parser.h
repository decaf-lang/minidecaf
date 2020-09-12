#ifndef _PARSER_H_
#define _PARSER_H_

#include <list>
#include "lexer.h"
#include "error.h"

enum NodeKind{
    ND_RETURN,      // Return 语句
    ND_NUM,         // 数字字面量
};

struct Node {
    NodeKind kind;
    int val;        // ND_NUM 的值
    std::shared_ptr<Node> expr; // return 语句的返回值对应的节点

    TKPtr tok;      // 为了报错，不做要求，可以忽略
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