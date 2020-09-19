#include <iostream>

#include "MiniDecafLexer.h"
#include "MiniDecafParser.h"
#include "Typer.h"
#include "Allocator.h"
#include "CodeGenVisitor.h"

using namespace antlr4;
using namespace std;

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        std::cerr << "[error] Input sourcefile required: ./MiniDecaf <sourcefile>\n";
        return 1;
    }
    ifstream sourceFile;
    sourceFile.open(argv[1]);

    // lexer & parser part of antlr4
    ANTLRInputStream input(sourceFile);
    MiniDecafLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    MiniDecafParser parser(&tokens);
    MiniDecafParser::ProgContext* treeNode = parser.prog();

    // customized pass: allocator, typer, codegen and etc.
    Typer typeVisitor;
    Allocator allocatorVisitor;
    CodeGenVisitor codeGenVisitor;

    typeVisitor.visitProg(treeNode); 
    allocatorVisitor.visitProg(treeNode);
    string asmCode = codeGenVisitor.visitProg(treeNode);

    // We get the asm code!
    cout << asmCode << endl;
    return 0;
}

