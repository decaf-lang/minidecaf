#ifndef __AST_H__
#define __AST_H__
#include<vector>
#include<fstream>
#include<string>
#include<map>
using std::string;
using std::vector;
using std::endl;
using std::map;
using std::ofstream;
class Ast{
protected:
	int row, column;
	static int indent;
	static int branchnum;
	static map<string, vector<int>> exprnum;
	static string function_name;
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
string Ast::function_name = "";
int Ast::branchnum = 0;
map<string, vector<int>> Ast::exprnum = {};

class ExprAst: public Ast{
protected:
	bool isVariable_;
	string variable_;
public:
	ExprAst(int row, int column) : Ast(row, column), isVariable_(false), variable_(""){}
	ExprAst(int row, int column, bool isVariable, string s_) : Ast(row, column), isVariable_(isVariable), variable_(s_){}
	bool isVariable() { return isVariable_;}
	string variable() { return variable_; };
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
	string str;
public:
	UnaryAst(int row, int column, string s) : ExprAst(row, column), str(s){}
	void additem(ExprAst* item){
		expr = item;
	}
	void printto(ofstream &fout){
		expr->printto(fout);
		if (str == "!")
			printstream(fout, "seqz a5,a5");
		else if (str == "~")
			printstream(fout, "not a5,a5");
		else if (str == "-")
			printstream(fout, "neg a5,a5");
		else
			{}
	}
};

class FactorAst: public ExprAst{
	ExprAst* expr1;
	ExprAst* expr2;
	string str;
public:
	FactorAst(int row, int column, string s) : ExprAst(row, column), str(s){}
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
		printstream(fout, "addi sp, sp, 4");
		if (str == "*")
			printstream(fout, "mul a5, a4, a5");
		else if (str == "/")
			printstream(fout, "div a5, a4, a5");
		else if (str == "%")
			printstream(fout, "rem  a5, a4, a5");

	}
};

class TermAst: public ExprAst{
	ExprAst* expr1;
	ExprAst* expr2;
	string str;
public:
	TermAst(int row, int column, string s) : ExprAst(row, column), str(s){}
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
		printstream(fout, "addi sp, sp, 4");
		if (str == "+")
			printstream(fout, "add a5, a4, a5");
		else if (str == "-")
			printstream(fout, "sub a5, a4, a5");

	}
};

class AdditiveAst: public ExprAst{
	ExprAst* expr1;
	ExprAst* expr2;
	string str;
public:
	AdditiveAst(int row, int column, string s) : ExprAst(row, column), str(s){}
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
		printstream(fout, "addi sp, sp, 4");
		if (str == "<")
			printstream(fout, "slt a5, a4, a5");
		else if (str == ">")
			printstream(fout, "sgt a5, a4, a5");
		else if (str == "<="){
			printstream(fout, "sgt a5, a4, a5");
			printstream(fout, "xor a5, a5, 1");
		}else {
			printstream(fout, "slt a5, a4, a5");
			printstream(fout, "xor a5, a5, 1");
		}
	}
};

class RelationalAst: public ExprAst{
	ExprAst* expr1;
	ExprAst* expr2;
	string str;
public:
	RelationalAst(int row, int column, string s) : ExprAst(row, column), str(s){}
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
		printstream(fout, "addi sp, sp, 4");
		if (str == "!="){
			printstream(fout, "xor a5, a4, a5");
			printstream(fout, "snez a5, a5");
		}else{
			printstream(fout, "xor a5, a4, a5");
			printstream(fout, "seqz a5, a5");
		}
	}
};

class EqualityAst: public ExprAst{
	ExprAst* expr1;
	ExprAst* expr2;
public:
	EqualityAst(int row, int column) : ExprAst(row, column){}
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
		printstream(fout, "addi sp, sp, 4");
		printstream(fout, "snez a4, a4");
		printstream(fout, "snez a5, a5");
		printstream(fout, "and a5, a5, a4");
	}
};

class LogicAndAst: public ExprAst{
	ExprAst* expr1;
	ExprAst* expr2;
public:
	LogicAndAst(int row, int column) : ExprAst(row, column){}
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
		printstream(fout, "addi sp, sp, 4");
		printstream(fout, "or a5, a5, a4");
		printstream(fout, "snez a5, a5");
	}
};

class AssignAst : public ExprAst{
	string name;
	ExprAst* expr;
public:
	AssignAst(int row, int column) : ExprAst(row, column){}
	void additem(string name_, ExprAst* e){
		expr = e;
		name = name_;
	}
	void printto(ofstream &fout){
		expr->printto(fout);
		printstream(fout, "sw a5, -"+std::to_string(exprnum[name].back())+"(s0)");
	}
};

class IdAst : public ExprAst{
	string name;
public:
	IdAst(int row, int column) : ExprAst(row, column){}
	void additem(string name_){
		name = name_;
	}
	void printto(ofstream &fout){
		printstream(fout, "lw a5, -"+std::to_string(exprnum[name].back())+"(s0)");
	}
};

class ConditionalAst: public ExprAst{
	ExprAst* expr1;
	ExprAst* expr2;
	ExprAst* expr3;
public:
	ConditionalAst(int row, int column) : ExprAst(row, column){}
	void additem(ExprAst* item1, ExprAst* item2, ExprAst* item3){
		expr1 = item1;
		expr2 = item2;
		expr3 = item3;
	}
	void printto(ofstream &fout){
		expr1->printto(fout);
		if (expr2 != NULL){
			printstream(fout, "beqz a5, .L"+std::to_string(branchnum));
			expr2->printto(fout);
			printstream(fout, "j .L"+std::to_string(branchnum+1));
			decIndent();
			printstream(fout, ".L"+std::to_string(branchnum)+":");
			branchnum++;
			addIndent();
			expr3->printto(fout);
			decIndent();
			printstream(fout, ".L"+std::to_string(branchnum)+":");
			branchnum++;
			addIndent();
		}
	}
};

class CallAst: public ExprAst{
	string name;
	vector<ExprAst*> expr_list;
public:
	CallAst(int row, int column) : ExprAst(row, column){}
	void additem(ExprAst* item){
		expr_list.push_back(item);
	}
	void additem(string item){
		name = item;
	}
	void printto(ofstream &fout){
		for (int i = expr_list.size()-1; i >= 0 ; --i){
			expr_list[i]->printto(fout);
			printstream(fout, "addi sp, sp, -4");
			printstream(fout, "sw a5, 0(sp)");
		}
		if (expr_list.size() < 8){
			for (int i = 0; i < expr_list.size(); ++i){
				printstream(fout, "lw a"+std::to_string(i)+", "+std::to_string(4*i)+"(sp)");
			}
			printstream(fout, "addi sp, sp, "+std::to_string(4*expr_list.size()));
		}
		printstream(fout, "call "+name);
		printstream(fout, "mv	a5,a0");

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
		printstream(fout, "j ."+function_name+"_exit");
	}
};

class ExprStmtAst: public StmtAst{
	ExprAst* expr;
public:
	ExprStmtAst(int row, int column) : StmtAst(row, column){}
	void additem(ExprAst* item){
		expr = item;
	}
	bool hasExpr(){
		return expr != NULL;
	}
	void printto(ofstream &fout){
		if (expr != NULL)
			expr->printto(fout);
	}
};

class ExprCloseStmtAst: public StmtAst{
	ExprAst* expr;
public:
	ExprCloseStmtAst(int row, int column) : StmtAst(row, column){}
	void additem(ExprAst* item){
		expr = item;
	}
	bool hasExpr(){
		return expr != NULL;
	}
	void printto(ofstream &fout){
		if (expr != NULL)
			expr->printto(fout);
	}
};

class LocalVariableAst: public StmtAst{
	string name;
	ExprAst* expr;
	int position;
public:
	LocalVariableAst(int row, int column) : StmtAst(row, column){}
	string getname() {return name;}
	void additem(string name, int num, ExprAst* item){
		this->name = name;
		expr = item;
		this->position = num*4+4;
	}
	void printto(ofstream &fout){
		exprnum[name].push_back(this->position);
		if (expr!=NULL){
			expr->printto(fout);
			printstream(fout, "sw a5, -"+ std::to_string(exprnum[name].back())+"(s0)");
		}
	}
};

class BlockAst: public StmtAst{
	vector<StmtAst*> stmt_list;
	vector<string> name_list;
public:
	BlockAst(int row, int column) : StmtAst(row, column){}
	void additem(StmtAst* item){
		stmt_list.push_back(item);
	}
	void additem(StmtAst* item, string name){
		stmt_list.push_back(item);
		name_list.push_back(name);
	}
	void printto(ofstream &fout){
		for (int i = 0; i < stmt_list.size(); ++i)
			stmt_list[i]->printto(fout);
		for (int i = 0; i < name_list.size(); ++i)
			exprnum[name_list[i]].pop_back();
	}
};

class IfAst: public StmtAst{
	ExprAst* expr;
	StmtAst* stmt1;
	StmtAst* stmt2;
public:
	IfAst(int row, int column) : StmtAst(row, column){}
	void additem(ExprAst* item, StmtAst* item1, StmtAst* item2){
		expr = item;
		stmt1 = item1;
		stmt2 = item2;
	}
	void printto(ofstream &fout){
		expr->printto(fout);
		printstream(fout, "beqz a5, .L"+std::to_string(branchnum));
		stmt1->printto(fout);
		if (stmt2!=NULL)
			printstream(fout, "j .L"+std::to_string(branchnum+1));
		decIndent();
		printstream(fout, ".L"+std::to_string(branchnum)+":");
		branchnum++;
		addIndent();
		if (stmt2!=NULL){
			stmt2->printto(fout);
			decIndent();
			printstream(fout, ".L"+std::to_string(branchnum)+":");
			branchnum++;
			addIndent();
		}
	}
};

class ForAst: public StmtAst{
	StmtAst* stmt1;
	ExprStmtAst* stmt2;
	ExprCloseStmtAst* stmt3;
	StmtAst* stmt;
	string declaration;
	bool isdec;
public:
	ForAst(int row, int column) : StmtAst(row, column){}
	void additem(StmtAst* item1, ExprStmtAst* item2, ExprCloseStmtAst* item3, StmtAst* item){
		stmt1 = item1;
		stmt2 = item2;
		stmt3 = item3;
		stmt = item;
		isdec = false;
	}
	void additem(string dec, StmtAst* item1, ExprStmtAst* item2, ExprCloseStmtAst* item3, StmtAst* item){
		stmt1 = item1;
		stmt2 = item2;
		stmt3 = item3;
		stmt = item;
		declaration = dec;
		isdec = true;
	}
	void printto(ofstream &fout){

		stmt1->printto(fout);
		decIndent();
		printstream(fout, ".L"+std::to_string(branchnum)+":");
		branchnum++;
		addIndent();
		if (stmt2->hasExpr()){
			stmt2->printto(fout);
			printstream(fout, "beqz a5, .L"+std::to_string(branchnum));
		}
		stmt->printto(fout);
		stmt3->printto(fout);
		printstream(fout, "j .L"+std::to_string(branchnum-1));
		decIndent();
		printstream(fout, ".L"+std::to_string(branchnum)+":");
		branchnum++;
		addIndent();
		if (isdec)
			exprnum[declaration].pop_back();
	}
};

class WhileAst: public StmtAst{
	ExprAst* expr;
	StmtAst* stmt;
public:
	WhileAst(int row, int column) : StmtAst(row, column){}
	void additem(ExprAst* item1, StmtAst* item2){
		expr = item1;
		stmt = item2;
	}
	void printto(ofstream &fout){
		decIndent();
		printstream(fout, ".L"+std::to_string(branchnum)+":");
		branchnum++;
		addIndent();

		expr->printto(fout);
		printstream(fout, "beqz a5, .L"+std::to_string(branchnum));
		stmt->printto(fout);
		printstream(fout, "j .L"+std::to_string(branchnum-1));
		
		decIndent();
		printstream(fout, ".L"+std::to_string(branchnum)+":");
		branchnum++;
		addIndent();
	}
};

class DoAst: public StmtAst{
	StmtAst* stmt;
	ExprAst* expr;
public:
	DoAst(int row, int column) : StmtAst(row, column){}
	void additem(StmtAst* item1, ExprAst* item2){
		stmt = item1;
		expr = item2;
	}
	void printto(ofstream &fout){
		decIndent();
		printstream(fout, ".L"+std::to_string(branchnum)+":");
		branchnum++;
		addIndent();

		stmt->printto(fout);
		expr->printto(fout);
		printstream(fout, "beqz a5, .L"+std::to_string(branchnum));
		printstream(fout, "j .L"+std::to_string(branchnum-1));
		
		decIndent();
		printstream(fout, ".L"+std::to_string(branchnum)+":");
		branchnum++;
		addIndent();
	}
};

class ContinueAst: public StmtAst{
public:
	ContinueAst(int row, int column) : StmtAst(row, column){}
	void printto(ofstream &fout){
		printstream(fout, "j .L"+std::to_string(branchnum-1));
	}
};

class BreakAst: public StmtAst{
public:
	BreakAst(int row, int column) : StmtAst(row, column){}
	void printto(ofstream &fout){
		printstream(fout, "j .L"+std::to_string(branchnum));
	}
};

class FunctionAst: public Ast{
	string name;
	vector<string> expr_name;
	StmtAst* stmt;
	int variable_num;
public:
	FunctionAst(int row, int column) : Ast(row, column){}
	void additem(string name, StmtAst* item, int num){
		this->name = name;
		stmt = item;
		variable_num = num;
	}
	void additem(string item){
		expr_name.push_back(item);
	}
	void printto(ofstream &fout){
		function_name = name;
		printstream(fout, ".globl "+name);
		printstream(fout, ".type "+name+", @function");
		decIndent();
		printstream(fout, name+":");
		addIndent();
		int num = variable_num + expr_name.size();
		printstream(fout, "addi	sp,sp,-"+std::to_string(8+num*4));
		printstream(fout, "sw	ra,"+std::to_string(4+num*4)+"(sp)");
		printstream(fout, "sw	s0,"+std::to_string(num*4)+"(sp)");
		printstream(fout, "addi	s0,sp,"+std::to_string(8+num*4));
		for (int i = 0; i < expr_name.size(); ++i){
			exprnum[expr_name[i]].push_back(12+4*i);
		}
		if (expr_name.size() < 8){
			for (int i = 0; i < expr_name.size(); ++i){
				exprnum[expr_name[i]].push_back(12+4*i);
				printstream(fout, "sw	a"+std::to_string(i)+",-"+std::to_string(12+4*i)+"(s0)");
			}
		}
		stmt->printto(fout);
		decIndent();
		printstream(fout, "."+name+"_exit:");
		addIndent();
		printstream(fout, "lw	s0,"+std::to_string(num*4)+"(sp)");
		printstream(fout, "lw	ra,"+std::to_string(4+num*4)+"(sp)");
		printstream(fout, "addi	sp,sp,"+std::to_string(8+num*4));
		printstream(fout, "jr	ra");
		decIndent();
		addIndent();
		printstream(fout, ".size "+name+", .-"+name);
		printstream(fout, ".ident	\"GCC: (xPack GNU RISC-V Embedded GCC, 64-bit) 8.3.0\"");
		for (int i = 0; i < expr_name.size(); ++i){
			exprnum[expr_name[i]].pop_back();
		}
	}
};

class ProgramAst: public Ast{
	vector<FunctionAst*> function_list;
public:
	ProgramAst(int row, int column) : Ast(row, column){}
	void additem(FunctionAst* func){
		function_list.push_back(func);
	}
	void printto(ofstream &fout, string filename){
		addIndent();
		printstream(fout, ".file	\""+filename+"\"");
		printstream(fout, ".option nopic");
		printstream(fout, ".text");
		printstream(fout, ".align	1");
		for (int i = 0; i < function_list.size(); ++i)
			function_list[i]->printto(fout);
		decIndent();
	}
};

#endif