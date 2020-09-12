#ifndef _LEXER_H_
#define _LEXER_H_

#include <list>
#include <memory>

enum TokenKind {
    TK_RESERVED, // 保留字与符号
    TK_IDENT,    // 标识符
    TK_NUM,      // 数字字面量
    TK_EOF,      // 结束标志
};

struct Token {
    TokenKind kind;
    int val;        // 如果是 TK_NUM，其代表的值
    char *str;      // Token 字符串
    int len;        // Token 字符串长度
};

typedef std::shared_ptr<Token> TKPtr;

std::list<TKPtr>* lexing(char*);

#endif // _LEXER_H_