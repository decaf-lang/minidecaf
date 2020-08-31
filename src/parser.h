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
			std::cout << "find " << expected << " when matching "<< tokenlist[pos].label() <<
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
		ExprAst* expr_ast = parserRelationalExpr();
		while (lookForward("!=") || lookForward("==")){
			RelationalAst* relational_ast = new RelationalAst(tokenlist[pos].row(), tokenlist[pos].column(), tokenlist[pos].label());
			matchToken(tokenlist[pos].label());
			ExprAst* expr_ast2 = parserRelationalExpr();
			relational_ast->additem(expr_ast, expr_ast2);
			expr_ast = relational_ast;
		}
		return expr_ast;
	}

	ExprAst* parserRelationalExpr(){
		ExprAst* expr_ast = parserAdditiveExpr();
		while (lookForward("<") || lookForward(">") || lookForward("<=") || lookForward(">=")){
			AdditiveAst* additive_ast = new AdditiveAst(tokenlist[pos].row(), tokenlist[pos].column(), tokenlist[pos].label());
			matchToken(tokenlist[pos].label());
			ExprAst* expr_ast2 = parserAdditiveExpr();
			additive_ast->additem(expr_ast, expr_ast2);
			expr_ast = additive_ast;
		}
		return expr_ast;
	}

	ExprAst* parserAdditiveExpr(){
		ExprAst* expr_ast = parserTerm();
		while (lookForward("+") || lookForward("-")){
			TermAst* term_ast = new TermAst(tokenlist[pos].row(), tokenlist[pos].column(), tokenlist[pos].label());
			matchToken(tokenlist[pos].label());
			ExprAst* expr_ast2 = parserTerm();
			term_ast->additem(expr_ast, expr_ast2);
			expr_ast = term_ast;
		}
		return expr_ast;
	}

	ExprAst* parserTerm(){
		ExprAst* expr_ast = parserFactor();
		while (lookForward("*") || lookForward("/") || lookForward("%")){
			FactorAst* factor_ast = new FactorAst(tokenlist[pos].row(), tokenlist[pos].column(), tokenlist[pos].label());
			matchToken(tokenlist[pos].label());
			ExprAst* expr_ast2 = parserTerm();
			factor_ast->additem(expr_ast, expr_ast2);
			expr_ast = factor_ast;
		}
		return expr_ast;
	}

	ExprAst* parserFactor(){
		ExprAst* expr_ast;
		if (lookForward("!") || lookForward("~") || lookForward("-") || lookForward("+"))
			expr_ast = parserUnary();
		else if (lookForward("(")){
			matchToken("(");
			expr_ast = parserExpr();
			matchToken(")");
		}else
			expr_ast = parserConstant();
		return expr_ast;
	}

	ConstantAst* parserConstant(){
		ConstantAst* constant_ast = new ConstantAst(tokenlist[pos].row(), tokenlist[pos].column());
		matchToken("num");
		constant_ast->additem(tokenlist[pos-1].intvalue());
		return constant_ast;
	}

	ExprAst* parserUnary(){
		UnaryAst* unary_ast = new UnaryAst(tokenlist[pos].row(), tokenlist[pos].column(), tokenlist[pos].label());
		matchToken(tokenlist[pos].label());
		ExprAst* expr_ast = parserFactor();
		unary_ast->additem(expr_ast);
		return unary_ast;
	}
};

#endif