#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include "error.h"

static char* filename = NULL;
static char* user_input = NULL;

void init_error(char* name, char* input) {
    filename = name;
    user_input = input;
}

void error(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// 按照以下格式报错
//
// foo.c:10: x = y + 1;
//               ^ <error message here>
static void verror_at(char *loc, const char *fmt, va_list ap) {
    // 找到包含 loc 的行，line为行首
    char *line = loc;
    while (user_input < line && line[-1] != '\n')
        line--;

    char *end = loc;
    while (*end != '\n')
        end++;

    // 得到行号
    int line_num = 1;
    for (char *p = user_input; p < line; p++)
        if (*p == '\n')
            line_num++;

    // 输出该行内容
    int indent = fprintf(stderr, "%s:%d: ", filename, line_num);
    fprintf(stderr, "%.*s\n", (int)(end - line), line);

    // 输出错误信息
    int pos = loc - line + indent;
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
}

void error_at(char *loc, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    verror_at(loc, fmt, ap);
    exit(1);
}

void error_tok(TKPtr tok, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    verror_at(tok->str, fmt, ap);
    exit(1);
}