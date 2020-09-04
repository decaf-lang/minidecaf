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
	int local_variable_num;
public:
	Parser(vector<Token>& lists): tokenlist(lists) {pos = 0; local_variable_num = 0;}
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

	bool lookForward(string expected, int x = 0){
		if (tokenlist[pos+x].label() == expected)
			return true;
		return false;
	}

	void parserProgram(){
		ast = new ProgramAst(tokenlist[pos].row(), tokenlist[pos].column());
		while (pos < tokenlist.size()){
			if (lookForward("(",2)){
				FunctionAst* function_ast = parserFunction();
				ast->additem(function_ast);
			}else{
				TypeAst* type_ast = parserType();
				matchToken("id");
				string name = tokenlist[pos-1].value();
				int value = 0;
				if (lookForward("=")){
					matchToken("=");
					matchToken("num");
					value = tokenlist[pos-1].intvalue();
				}
				matchToken(";");
				ast->additem(type_ast, name, value);
			}
		}
	}

	FunctionAst* parserFunction(){
		local_variable_num = 0;
		FunctionAst* function_ast = new FunctionAst(tokenlist[pos].row(), tokenlist[pos].column());
		TypeAst* type_ast = parserType();
		matchToken("id");
		string name = tokenlist[pos-1].value();
		matchToken("(");
		int k = 0;
		while (!lookForward(")")){
			if (k > 0)
				matchToken(",");
			TypeAst* new_type_ast = parserType();
			matchToken("id");
			function_ast->additem(new_type_ast, tokenlist[pos-1].value());
			k++;
		}
		matchToken(")");
		StmtAst* stmt_ast = NULL;
		if (lookForward(";"))
			matchToken(";");
		else{
			matchToken("{");
			stmt_ast = parserBlock();
			matchToken("}");
		}
		function_ast->additem(type_ast, name, stmt_ast, local_variable_num);
		return function_ast;
	}

	TypeAst* parserType(){
		TypeAst* type_ast = new TypeAst(tokenlist[pos].row(), tokenlist[pos].column());
		matchToken("int");
		while (lookForward("*"))
			matchToken("*");
		return type_ast;
	}

	StmtAst* parserBlock(){
		BlockAst* block_ast = new BlockAst(tokenlist[pos].row(), tokenlist[pos].column());
		while (!lookForward("}")){
			if (lookForward("int")){
				LocalVariableAst* stmt_ast = parserLocalVariable();
				block_ast->additem(stmt_ast, stmt_ast->getname());
			}else{
				StmtAst *stmt_ast = parserStmt();
				block_ast->additem(stmt_ast);
			}
		}
		return block_ast;
	}

	StmtAst* parserStmt(){
		StmtAst* stmt_ast;
		if (lookForward("return"))
			stmt_ast = parserReturnStmt();
		else if (lookForward("if"))
			stmt_ast = parserIfStmt();
		else if (lookForward("for"))
			stmt_ast = parserForStmt();
		else if (lookForward("while"))
			stmt_ast = parserWhileStmt();
		else if (lookForward("do"))
			stmt_ast = parserDoStmt();
		else if (lookForward("continue"))
			stmt_ast = parserContinueStmt();
		else if (lookForward("break"))
			stmt_ast = parserBreakStmt();
		else if (lookForward("{")){
			matchToken("{");
			stmt_ast = parserBlock();
			matchToken("}");
		}else
			stmt_ast = parserExprSemicolonStmt();
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

	LocalVariableAst* parserLocalVariable(){
		LocalVariableAst* local_variable_ast = new LocalVariableAst(tokenlist[pos].row(), tokenlist[pos].column());
		TypeAst* type_ast = parserType();
		matchToken("id");
		string name = tokenlist[pos-1].value();
		ExprAst* expr_ast = NULL;
		if (lookForward("=")){
			matchToken("=");
			expr_ast = parserExpr();
		}
		matchToken(";");
		local_variable_num++;
		local_variable_ast->additem(type_ast, name, local_variable_num, expr_ast);
		return local_variable_ast;
	}

	ExprStmtAst* parserExprSemicolonStmt(){
		ExprStmtAst* expr_stmt_ast = new ExprStmtAst(tokenlist[pos].row(), tokenlist[pos].column());
		ExprAst* expr_ast = NULL;
		if (lookForward(";")) 
			matchToken(";");
		else{
			expr_ast = parserExpr();
			matchToken(";");
		}
		expr_stmt_ast->additem(expr_ast);
		return expr_stmt_ast;
	}

	ExprCloseStmtAst* parserExprCloseStmt(){
		ExprCloseStmtAst* expr_close_stmt_ast = new ExprCloseStmtAst(tokenlist[pos].row(), tokenlist[pos].column());
		ExprAst* expr_ast = NULL;
		if (lookForward(")")) 
			matchToken(")");
		else{
			expr_ast = parserExpr();
			matchToken(")");
		}
		expr_close_stmt_ast->additem(expr_ast);
		return expr_close_stmt_ast;
	}

	StmtAst* parserIfStmt(){
		IfAst* if_ast = new IfAst(tokenlist[pos].row(), tokenlist[pos].column());
		matchToken("if");
		matchToken("(");
		ExprAst* expr_ast = parserExpr();
		matchToken(")");
		StmtAst* stmt_ast1 = parserStmt();
		StmtAst* stmt_ast2 = NULL;
		if (lookForward("else")){
			matchToken("else");
			stmt_ast2 = parserStmt();
		}
		if_ast->additem(expr_ast, stmt_ast1, stmt_ast2);
		return if_ast;
	}

	StmtAst* parserForStmt(){
		ForAst* for_ast = new ForAst(tokenlist[pos].row(), tokenlist[pos].column());
		matchToken("for");
		matchToken("(");
		if (lookForward("int")){
			LocalVariableAst* stmt_ast1 = parserLocalVariable();
			ExprStmtAst* stmt_ast2 = parserExprSemicolonStmt();
			ExprCloseStmtAst* stmt_ast3 = parserExprCloseStmt();
			StmtAst* stmt_ast4 = parserStmt();
			for_ast->additem(stmt_ast1->getname(), stmt_ast1, stmt_ast2, stmt_ast3, stmt_ast4);
		}else{
			ExprStmtAst* stmt_ast1 = parserExprSemicolonStmt();
			ExprStmtAst* stmt_ast2 = parserExprSemicolonStmt();
			ExprCloseStmtAst* stmt_ast3 = parserExprCloseStmt();
			StmtAst* stmt_ast4 = parserStmt();
			for_ast->additem(stmt_ast1, stmt_ast2, stmt_ast3, stmt_ast4);
		}
		return for_ast;
	}

	StmtAst* parserWhileStmt(){
		WhileAst* while_ast = new WhileAst(tokenlist[pos].row(), tokenlist[pos].column());
		matchToken("while");
		matchToken("(");
		ExprAst* expr_ast = parserExpr();
		matchToken(")");
		StmtAst* stmt_ast = parserStmt();
		while_ast->additem(expr_ast, stmt_ast);
		return while_ast;
	}

	StmtAst* parserDoStmt(){
		DoAst* do_ast = new DoAst(tokenlist[pos].row(), tokenlist[pos].column());
		matchToken("do");
		StmtAst* stmt_ast = parserStmt();
		matchToken("while");
		matchToken("(");
		ExprAst* expr_ast = parserExpr();
		matchToken(")");
		matchToken(";");
		do_ast->additem(stmt_ast, expr_ast);
		return do_ast;
	}

	StmtAst* parserBreakStmt(){
		BreakAst* break_ast = new BreakAst(tokenlist[pos].row(), tokenlist[pos].column());
		matchToken("break");
		return break_ast;
	}

	StmtAst* parserContinueStmt(){
		ContinueAst* contionue_ast = new ContinueAst(tokenlist[pos].row(), tokenlist[pos].column());
		matchToken("continue");
		return contionue_ast;
	}

	ExprAst* parserExpr(){
		ExprAst* expr_ast;
		int id = pos;
		expr_ast = parserFactor();
		if (lookForward("=")){
			pos = id;
			expr_ast = parserAssign();
		}else{
			pos = id;
			expr_ast = parserConditionalExpr();
		}
		return expr_ast;
	}

	ExprAst* parserAssign(){
		AssignAst* expr_ast = new AssignAst(tokenlist[pos].row(), tokenlist[pos].column());
		ExprAst* expr_ast1 = parserFactor();
		matchToken("=");
		ExprAst* expr_ast2 = parserExpr();
		expr_ast->additem(expr_ast1, expr_ast2);
		return expr_ast;
	}

	ExprAst* parserConditionalExpr(){
		ConditionalAst* conditional_ast = new ConditionalAst(tokenlist[pos].row(), tokenlist[pos].column());
		ExprAst* expr_ast1 = parserLogicOrExpr();	
		ExprAst* expr_ast2 = NULL;	
		ExprAst* expr_ast3 = NULL;	
		if (lookForward("?")){
			matchToken("?");
			expr_ast2 = parserExpr();
			matchToken(":");
			expr_ast3 = parserConditionalExpr();
		}
		conditional_ast->additem(expr_ast1, expr_ast2, expr_ast3);
		return conditional_ast;
	}

	ExprAst* parserLogicOrExpr(){
		ExprAst* expr_ast = parserLogicAndExpr();
		while (lookForward("||")){
			LogicAndAst* logic_and_ast = new LogicAndAst(tokenlist[pos].row(), tokenlist[pos].column());
			matchToken(tokenlist[pos].label());
			ExprAst* expr_ast2 = parserLogicAndExpr();
			logic_and_ast->additem(expr_ast, expr_ast2);
			expr_ast = logic_and_ast;
		}
		return expr_ast;
	}

	ExprAst* parserLogicAndExpr(){
		ExprAst* expr_ast = parserEqualityExpr();
		while (lookForward("&&")){
			EqualityAst* equality_ast = new EqualityAst(tokenlist[pos].row(), tokenlist[pos].column());
			matchToken(tokenlist[pos].label());
			ExprAst* expr_ast2 = parserEqualityExpr();
			equality_ast->additem(expr_ast, expr_ast2);
			expr_ast = equality_ast;
		}
		return expr_ast;
	}

	ExprAst* parserEqualityExpr(){
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
		if (lookForward("!") || lookForward("~") || lookForward("-") || lookForward("*") || lookForward("&") || lookForward("+"))
			expr_ast = parserUnary();
		else if (lookForward("(") && lookForward("int",1)){
			matchToken("(");
			parserType();
			expr_ast = parserExpr();
			matchToken(")");
		}else if (lookForward("(")){
			matchToken("(");
			expr_ast = parserExpr();
			matchToken(")");
		}else if (lookForward("id") && lookForward("(", 1)){
			expr_ast = parserCallExpr();
		}else if (lookForward("id")){
			expr_ast = parserId();
		}else
			expr_ast = parserConstant();
		return expr_ast;
	}

	ExprAst* parserCallExpr(){
		CallAst* call_ast = new CallAst(tokenlist[pos].row(), tokenlist[pos].column());
		matchToken("id");
		string name = tokenlist[pos-1].value();
		matchToken("(");
		int k = 0;
		while (!lookForward(")")){
			if (k > 0)
				matchToken(",");
			ExprAst* expr_ast = parserExpr();
			call_ast->additem(expr_ast);
			++k;
		}
		matchToken(")");
		call_ast->additem(name);
		return call_ast;
	}

	ExprAst* parserId(){
		IdAst* id_ast = new IdAst(tokenlist[pos].row(), tokenlist[pos].column());
		matchToken("id");
		string name = tokenlist[pos-1].value();
		id_ast->additem(name);
		return id_ast;
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