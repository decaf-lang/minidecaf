#include <fstream>
#include <iostream>

#include "MDLexer.h"
#include "MDParser.h"

void printHeader() {
    std::cout << ".global main" << std::endl;
    std::cout << "main:" << std::endl;
}

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <source file>" << std::endl;
        return 1;
    }

    std::ifstream stream;
    stream.open(argv[1]);
    antlr4::ANTLRInputStream input(stream);
    MDLexer lexer(&input);
    antlr4::CommonTokenStream tokens(&lexer);
    MDParser parser(&tokens);

    printHeader();
    parser.program();

    return 0;
}

