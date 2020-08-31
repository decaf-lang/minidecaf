#include <fstream>
#include "lexer.h"
#include "parser.h"
#include "ast.h"
using namespace std;

int main(int argc, char** argv){
	Lexer lexer(argv[1]);
	vector<Token>& tokenlist = lexer.analyse();
	Parser parser(tokenlist);
	parser.parserProgram();
	ProgramAst* ast = parser.getAst();
	ofstream fout(argv[2]);
	ast->printto(fout, argv[1]);
	fout.close();
}