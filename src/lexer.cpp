#include <cstring>
#include <cstdlib>
#include <cctype>
#include <cstdio>
#include <cassert>
#include "lexer.h"
#include "error.h"

static std::list<TKPtr> toks;

// 入队一个 token
TKPtr gen_token(TokenKind kind, char *str, int len) {
    auto tok = std::make_shared<Token>();
    tok->kind = kind;
    tok->str = str;
    tok->len = len;
    toks.push_back(tok);
    return tok;
}

bool startswith(const char *p, const char *q) {
    return strncmp(p, q, strlen(q)) == 0;
}

bool is_alpha(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

bool is_alnum(char c) {
    return is_alpha(c) || ('0' <= c && c <= '9');
}

// 暴力比较是否为保留字
const char* read_reserved(char *p) {
    // Keyword
    static const char *kw[] = {"return", "int"};

    for (int i = 0; i < sizeof(kw) / sizeof(*kw); i++) {
        int len = strlen(kw[i]);
        if (startswith(p, kw[i]) && !is_alnum(p[len]))
        return kw[i];
    }

    return NULL;
}

// 尝试识别一个数字字面量
TKPtr read_int_literal(char *start) {
    char *p = start;
    long val = strtol(p, &p, 10);
    assert(val < INT32_MAX);
    auto tok = gen_token(TK_NUM, start, p - start);
    tok->val = val;
    return tok;
}

// 从 user_input 得到 token list 
std::list<TKPtr>* lexing(char* user_input) {
    char* p = user_input;

    while (*p) {
        // 跳过空白字符
        if (isspace(*p)) {
            p++;
            continue;
        }

        // 保留字
        const char *kw = read_reserved(p);
        if (kw) {
            int len = strlen(kw);
            gen_token(TK_RESERVED, p, len);
            p += len;
            continue;
        }

        // 保留字中的单字母符号，其实这些符号都是保留的
        if (ispunct(*p)) {
            gen_token(TK_RESERVED, p, 1);
            p++;
            continue;
        }
    
        // 标识符
        if (is_alpha(*p)) {
            char *start = p;
            while (is_alnum(*p))
                p++;
            gen_token(TK_IDENT, start, p - start);
            continue;
        }

        // 数字字面量
        if (isdigit(*p)) {
            TKPtr tok = read_int_literal(p);
            p += tok->len;
            continue;
        }

        // 错误
        char* q = p++;
        while(*q++);
        *q = '\0';
        error_at(p, "invalid input string");
    }

    gen_token(TK_EOF, p, 0);

    return &toks;
}