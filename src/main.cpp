#include <fstream>
#include <iostream>

#include "MDLexer.h"
#include "MDParser.h"
#include "CodeGenVisitor.h"

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
    parser.program();

    CodeGenVisitor codeGen;
    auto code = codeGen.genCode(parser.getExpr());
    std::cout << code;

    return 0;
}

