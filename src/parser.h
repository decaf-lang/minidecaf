#ifndef __PARSER_H__
#define __PARSER_H__
#include <string.h>
#include <iostream>
#include "token.h"
#include "ast.h"
using std::string;
class Parser{
	vector<Token>& tokenlist;
	int pos;
	ProgramAst* ast;
public:
	Parser(vector<Token>& lists): tokenlist(lists) {pos = 0;}
	ProgramAst* getAst(){return ast;}
	void matchToken(string expected){
		if (tokenlist[pos].label() == expected){
			pos++;
		}else{
			std::cout << "expected " << expected << " when matching "<< tokenlist[pos].label() <<
			" token in row " << tokenlist[pos].row() << " column " << tokenlist[pos].column() << 
			std::endl;
			exit(-1) ;
		}
	}

	bool lookForward(string expected){
		if (tokenlist[pos].label() == expected)
			return true;
		return false;
	}

	void parserProgram(){
		ast = new ProgramAst(tokenlist[pos].row(), tokenlist[pos].column());
		FunctionAst* function_ast = parserFunction();
		ast->additem(function_ast);
	}

	FunctionAst* parserFunction(){
		FunctionAst* function_ast = new FunctionAst(tokenlist[pos].row(), tokenlist[pos].column());
		matchToken("int");
		matchToken("id");
		string name = tokenlist[pos-1].value();
		matchToken("(");
		matchToken(")");
		matchToken("{");
		StmtAst* stmt_ast = parserStmt();
		matchToken("}");
		function_ast->additem(name, stmt_ast);
		return function_ast;
	}

	StmtAst* parserStmt(){
		StmtAst* stmt_ast;
		stmt_ast = parserReturnStmt();
		return stmt_ast;
	}

	ReturnStmtAst* parserReturnStmt(){
		ReturnStmtAst* return_stmt_ast = new ReturnStmtAst(tokenlist[pos].row(), tokenlist[pos].column());
		matchToken("return");
		ExprAst* expr_ast = parserExpr();
		matchToken(";");
		return_stmt_ast->additem(expr_ast);
		return return_stmt_ast;
	}

	ExprAst* parserExpr(){
		ExprAst* expr_ast;
		expr_ast = parserConstant();
		return expr_ast;
	}

	ConstantAst* parserConstant(){
		ConstantAst* constant_ast = new ConstantAst(tokenlist[pos].row(), tokenlist[pos].column());
		matchToken("num");
		constant_ast->additem(tokenlist[pos-1].intvalue());
		return constant_ast;
	}
};

#endif