#ifndef __AST_H__
#define __AST_H__
#include<vector>
#include<fstream>
#include<string>
using std::string;
using std::vector;
using std::endl;
using std::ofstream;
class Ast{
protected:
	int row, column;
	static int indent;
public:
	Ast(int row, int column) : row(row), column(column){ } 
	void addIndent() { indent ++; }
	void decIndent() { indent --; }
	void printstream(ofstream &fout, string st){
		for (int i = 0; i < indent; ++i)
			fout << "\t" ;
		fout << st << endl;
	}
	virtual void printto(ofstream &fout){
		std::cout << "AStprint  ERROR" << std::endl;
	}
};

int Ast::indent = 0;

class ExprAst: public Ast{
protected:
public:
	ExprAst(int row, int column) : Ast(row, column){}
};

class ConstantAst: public ExprAst{
	int intvalue;
public:
	ConstantAst(int row, int column) : ExprAst(row, column){}
	void additem(int item){
		intvalue = item;
	}
	void printto(ofstream &fout){
		printstream(fout, "li a5,"+std::to_string(intvalue));
	}
};

class UnaryAst: public ExprAst{
	ExprAst* expr;
	char ch;
public:
	UnaryAst(int row, int column, char c) : ExprAst(row, column), ch(c){}
	void additem(ExprAst* item){
		expr = item;
	}
	void printto(ofstream &fout){
		expr->printto(fout);
		if (ch == '!')
			printstream(fout, "seqz a5,a5");
		else if (ch == '~')
			printstream(fout, "not a5,a5");
		else if (ch == '-')
			printstream(fout, "neg a5,a5");
		else
			{}
	}
};

class FactorAst: public ExprAst{
	ExprAst* expr1;
	ExprAst* expr2;
	char ch;
public:
	FactorAst(int row, int column, char c) : ExprAst(row, column), ch(c){}
	void additem(ExprAst* item1, ExprAst* item2){
		expr1 = item1;
		expr2 = item2;
	}
	void printto(ofstream &fout){
		expr1->printto(fout);
		printstream(fout, "sw a5, -4(sp)");
		printstream(fout, "addi sp, sp, -4");
		expr2->printto(fout);
		printstream(fout, "lw a4, 0(sp)");
		printstream(fout, "addi sp, sp, 8");
		if (ch == '*')
			printstream(fout, "mul a5, a4, a5");
		else if (ch == '/')
			printstream(fout, "div a5, a4, a5");
		else if (ch == '%')
			printstream(fout, "rem  a5, a4, a5");

	}
};

class StmtAst: public Ast{
public:
	StmtAst(int row, int column) : Ast(row, column){}
};

class ReturnStmtAst: public StmtAst{
	ExprAst* expr;
public:
	ReturnStmtAst(int row, int column) : StmtAst(row, column){}
	void additem(ExprAst* item){
		expr = item;
	}
	void printto(ofstream &fout){
		expr->printto(fout);
		printstream(fout, "mv a0,a5");
	}
};

class FunctionAst: public Ast{
	string name;
	StmtAst* stmt;
public:
	FunctionAst(int row, int column) : Ast(row, column){}
	void additem(string name, StmtAst* item){
		this->name = name;
		stmt = item;
	}
	void printto(ofstream &fout){
		printstream(fout, ".globl "+name);
		printstream(fout, ".type "+name+", @function");
		decIndent();
		printstream(fout, name+":");
		addIndent();
		int num_ = 16;
		printstream(fout, "addi	sp,sp,-"+std::to_string(num_));
		printstream(fout, "sw	s0,"+std::to_string(num_-4)+"(sp)");
		printstream(fout, "addi	s0,sp,"+std::to_string(num_));
		stmt->printto(fout);
		printstream(fout, "lw	s0,"+std::to_string(num_-4)+"(sp)");
		printstream(fout, "addi	sp,sp,"+std::to_string(num_));
		printstream(fout, "jr	ra");
		decIndent();
		addIndent();
		printstream(fout, ".size "+name+", .-"+name);
		printstream(fout, ".ident	\"GCC: (xPack GNU RISC-V Embedded GCC, 64-bit) 8.3.0\"");
	}
};

class ProgramAst: public Ast{
	FunctionAst* function;
public:
	ProgramAst(int row, int column) : Ast(row, column){}
	void additem(FunctionAst* func){
		function = func;
	}
	void printto(ofstream &fout, string filename){
		addIndent();
		printstream(fout, ".file	\""+filename+"\"");
		printstream(fout, ".option nopic");
		printstream(fout, ".text");
		printstream(fout, ".align	1");
		function->printto(fout);
		decIndent();
	}
};

#endif