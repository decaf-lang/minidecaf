#include <fstream>
#include <iostream>

#include "MDLexer.h"
#include "MDParser.h"
#include "VarAllocVisitor.h"
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

    std::shared_ptr<ASTNode> program = parser.program()->node;

    auto varMap = VarAllocVisitor().allocVar(program);
    auto code = CodeGenVisitor().genCode(program, varMap);
    std::cout << code;

    return 0;
}

