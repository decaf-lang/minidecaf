#include <iostream>
#include <cstdio>
#include <list>
#include <cstring>
#include <cstdlib>
#include "lexer.h"
#include "error.h"
#include "parser.h"
#include "codegen.h"

const int FILE_MAX = 100 * 1024;
static char user_input[FILE_MAX];

void read_input(const char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp)
        printf("cannot open %s\n", path);
    
    int size = fread(user_input, 1, FILE_MAX-2, fp);
    if (!feof(fp))
        printf("file too large.\n");

    if (size == 0 || user_input[size - 1] != '\n')
        user_input[size++] = '\n';
    user_input[size] = '\0';
}


int main(int argc, char **argv) {
    bool debug;
    if (argc > 2)
       debug = true;
    char* filename = argv[1];
    read_input(filename);
    // 报错是不要求的，写这些是为了方便调试
    init_error(filename, user_input);
    auto toks = lexing(user_input);
    auto prog = parsing(toks);
    codegen(prog, debug);
    return 0;
}