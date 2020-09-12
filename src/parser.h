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
    ND_IF,          // If statement
    ND_BLOCK,       // Block statement
    ND_FOR,         // For statement
    ND_DOWHILE,     // Do-while statement
    ND_WHILEDO,     // While-do statement
    ND_BREAK,       // Break statement
    ND_CONTINUE,    // Continue statement
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
    ND_TERNARY,     // Ternary a ? b : c
    ND_FUNC_CALL,   // Function call
};

struct Node;

typedef std::shared_ptr<Node> NDPtr;

struct Var {
    char *name;
    int offset;         // Offset from %fp
    NDPtr init;
    int scope_depth;    // 变量声明时所在的作用域深度
    bool is_arg;        // 函数参数还是局部变量
    TKPtr tok;
};

typedef std::shared_ptr<Var> VarPtr;

struct FuncCall {
    char* name;
    std::list<NDPtr> args;
};

struct Node {
    NodeKind kind;
    int val;                        // ND_NUM 的值
    NDPtr lexpr;                    // Left expression
    NDPtr rexpr;                    // Right expr, 一个已经不够用了
    VarPtr var;                     // ND_VAR 对应的变量
    TKPtr tok;                      // 为了报错，不做要求，可以忽略
    NDPtr cond;
    NDPtr then;
    NDPtr els;                      // 各种条件语句、循环语句使用
    NDPtr init;
    NDPtr inc;                      // For 循环使用
    std::list<NDPtr> body;          // Compound statement 的子语句
    std::shared_ptr<FuncCall> func_call;
};

struct Function {
    char *name;
    // 语句, 一个 compound statement
    NDPtr stmts;
    // 栈帧大小
    int stack_size;
    // 函数参数
    std::list<VarPtr> args;
    // 是一个声明还是包含完整定义
    bool is_complete;
};

typedef std::shared_ptr<Function> FNPtr;

struct Program {
    std::list<FNPtr> funcs;
};

Program* parsing(std::list<TKPtr>* toks);

extern const int POINTER_WIDTH;
extern const int POINTER_WIDTH_LOG;

#endif // _PARSER_H_