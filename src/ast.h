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
int functionvalue = 8;
vector<int> array;
class Ast{
protected:
	int row, column;
	static int indent;
	static int branchnum;
	static map<string, vector<vector<int>>> exprnum;
	static map<string, vector<bool>> exprisarray;
	static map<string, vector<bool>> exprisaddress;
	static string function_name;
	static vector<string> printans;
	static vector<string> continue_list;
	static vector<string> break_list;
public:
	Ast(int row, int column) : row(row), column(column){ } 
	void addIndent() { indent ++; }
	void decIndent() { indent --; }
	void printstream(ofstream &fout, string st){
		string s = "";
		for (int i = 0; i < indent; ++i)
			s = s + "\t" ;
		s = s + st;
		printans.push_back(s);

	}
	void pop(){
		printans.pop_back();
	}
	void printall(ofstream &fout){
		for (int i = 0; i < printans.size(); ++i)
			fout << printans[i] << std::endl;
	}
	virtual void printto(ofstream &fout){
		std::cout << "AStprint  ERROR" << std::endl;
	}
};

int Ast::indent = 0;
string Ast::function_name = "";
int Ast::branchnum = 0;
map<string, vector<vector<int>>> Ast::exprnum = {};
map<string, vector<bool>> Ast::exprisarray = {};
map<string, vector<bool>> Ast::exprisaddress = {};
vector<string> Ast::printans = {};
vector<string> Ast::continue_list = {};
vector<string> Ast::break_list = {};

class TypeAst: public Ast{
public:
	bool isAddress;
	TypeAst(int row, int column) : Ast(row, column){}
	void additem(bool b){
		isAddress = b;
	}
	void printto(ofstream &fout){
	}
};

class ExprAst: public Ast{
protected:
	bool isId_;         //是否是idast
	bool isAddress_;    //是否是地址值
	string variable_;
public:
	ExprAst(int row, int column) : Ast(row, column), isAddress_(false), isId_(false){}
	void setAddress(bool a) { isAddress_ = a;  }
	bool isAddress() { return isAddress_;}
	void setId() { isId_ = true;}
	bool isId() { return isId_;}
};

class ConstantAst: public ExprAst{
	int intvalue;
public:
	ConstantAst(int row, int column) : ExprAst(row, column){}
	void additem(int item){
		intvalue = item;
	}
	void printto(ofstream &fout){
		if (isAddress())
			printstream(fout, "li a5,"+std::to_string(intvalue*4));
		else
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
		if (str == "*"){
			expr->setAddress(true);
		}else if  (str == "&"){
			expr->setAddress(false);
		}else 
			expr->setAddress(isAddress());

		expr->printto(fout);
		if (str == "!")
			printstream(fout, "seqz a5,a5");
		else if (str == "~")
			printstream(fout, "not a5,a5");
		else if (str == "-")
			printstream(fout, "neg a5,a5");
		else if (str == "&"){
			pop();
			this->setAddress(true);
		}else if (str == "*"){
			printstream(fout, "lw a5,0(a5)");
		}else
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
		expr1->setAddress(this->isAddress());
		expr2->setAddress(this->isAddress());

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

class TypeFactor: public ExprAst{
	TypeAst* type;
	ExprAst* expr;
public:
	TypeFactor(int row, int column) : ExprAst(row, column){}
	void additem(TypeAst* item1, ExprAst* item2){
		type = item1;
		expr = item2;
	}
	void printto(ofstream &fout){
		expr->printto(fout);
		if (array.size() > 2){
			while (array.size() > 1)
				array.pop_back();
			array.push_back(1);
			array.push_back(1);
		}
		if (type->isAddress)
			this->setAddress(true);
	}
};

class PostfixAst: public ExprAst{
	ExprAst* expr1;
	ExprAst* expr2;
public:
	PostfixAst(int row, int column) : ExprAst(row, column){}
	void additem(ExprAst* item1, ExprAst* item2){
		expr1 = item1;
		expr2 = item2;
	}
	void printto(ofstream &fout){
		expr1->setAddress(this->isAddress());
		expr2->setAddress(false);

		expr2->printto(fout);
		printstream(fout, "sw a5, -4(sp)");
		printstream(fout, "addi sp, sp, -4");

		expr1->printto(fout);
		printstream(fout, "lw a4, 0(sp)");
		printstream(fout, "addi sp, sp, 4");

		printstream(fout, "li a3, 4");
		printstream(fout, "mul a4, a4, a3");
		printstream(fout, "li a3, "+std::to_string(array[array.back()]));
		printstream(fout, "mul a4, a4, a3");
		printstream(fout, "add a5, a5, a4");
		if (array.back() >= array.size()-2){
			printstream(fout, "lw a5, 0(a5)");
			array[array.size()-1] = 1;
		}else
			array[array.size()-1] = array[array.size()-1] + 1;
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
		expr1->setAddress(this->isAddress());
		expr2->setAddress(this->isAddress());

		expr1->printto(fout);
		printstream(fout, "sw a5, -4(sp)");
		printstream(fout, "addi sp, sp, -4");
		expr2->printto(fout);
		printstream(fout, "lw a4, 0(sp)");
		printstream(fout, "addi sp, sp, 4");
		// std::cout << expr1->isAddress() << " TermAst " << expr2->isAddress() << std::endl;
		// std::cout << row << " TermAst row " << column << std::endl;
		if (expr1->isAddress() && !expr2->isAddress()){
			printstream(fout, "li a3, 4");
			printstream(fout, "mul a5, a5, a3");
			this->setAddress(true);
		}else if (!expr1->isAddress() && expr2->isAddress()){
			printstream(fout, "li a3, 4");
			printstream(fout, "mul a4, a4, a3");
			this->setAddress(true);
		}else if (expr1->isAddress() && expr2->isAddress()){
			this->setAddress(true);
		}
		if (str == "+"){
			printstream(fout, "add a5, a4, a5");
		}
		else if (str == "-"){
			printstream(fout, "sub a5, a4, a5");
		}

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
		expr1->setAddress(this->isAddress());
		expr2->setAddress(this->isAddress());

		expr1->printto(fout);
		printstream(fout, "sw a5, -4(sp)");
		printstream(fout, "addi sp, sp, -4");
		expr2->printto(fout);
		printstream(fout, "lw a4, 0(sp)");
		printstream(fout, "addi sp, sp, 4");

		if (expr1->isAddress() && !expr2->isAddress()){
			printstream(fout, "li a3, 4");
			printstream(fout, "mul a5, a5, a3");
		}else if (!expr1->isAddress() && expr2->isAddress()){
			printstream(fout, "li a3, 4");
			printstream(fout, "mul a4, a4, a3");
		}

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
		expr1->setAddress(this->isAddress());
		expr2->setAddress(this->isAddress());

		// std::cout << expr1->isAddress() << " RelationalAst 1 " << expr2->isAddress() << std::endl;
		expr1->printto(fout);
		printstream(fout, "sw a5, -4(sp)");
		printstream(fout, "addi sp, sp, -4");

		// std::cout << expr1->isAddress() << " RelationalAst 2 " << expr2->isAddress() << std::endl;
		expr2->printto(fout);
		printstream(fout, "lw a4, 0(sp)");
		printstream(fout, "addi sp, sp, 4");
		// std::cout << expr1->isAddress() << " " << expr2->isAddress() << std::endl;
		if (expr1->isAddress() && !expr2->isAddress()){
			printstream(fout, "li a3, 4");
			printstream(fout, "mul a5, a5, a3");
		}else if (!expr1->isAddress() && expr2->isAddress()){
			printstream(fout, "li a3, 4");
			printstream(fout, "mul a4, a4, a3");
		}


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
		expr1->setAddress(this->isAddress());
		expr2->setAddress(this->isAddress());

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
		expr1->setAddress(this->isAddress());
		expr2->setAddress(this->isAddress());

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
	ExprAst* expr1;
	ExprAst* expr2;
public:
	AssignAst(int row, int column) : ExprAst(row, column){}
	void additem(ExprAst* e1, ExprAst* e2){
		expr1 = e1;
		expr2 = e2;
	}
	void printto(ofstream &fout){
		expr1->setAddress(this->isAddress());
		expr2->setAddress(this->isAddress());

		expr2->printto(fout);
		printstream(fout, "sw a5, -4(sp)");
		printstream(fout, "addi sp, sp, -4");

		expr1->printto(fout);
		pop();
		printstream(fout, "lw a4, 0(sp)");
		printstream(fout, "addi sp, sp, 4");
		printstream(fout, "sw a4, 0(a5)");
		printstream(fout, "mv a5, a4");
	}
};

class IdAst : public ExprAst{
	string name;
public:
	IdAst(int row, int column) : ExprAst(row, column){}
	void additem(string name_){
		name = name_;
		this->setId();
	}
	void printto(ofstream &fout){
		vector<int> ids = exprnum[name].back();
		array = exprnum[name].back();
		array.push_back(1);
		if (ids[0] < 0){
			printstream(fout, "lui a4,%hi("+name+")");
			printstream(fout, "addi a5, a4, %lo("+name+")");
			if (!exprisarray[name].back()){
				printstream(fout, "lw a5,0(a5)");
				if (!exprisaddress[name].back() && this->isAddress()){
					printstream(fout, "li a3,4");
					printstream(fout, "mul a5,a3, a5");
				}
			}
		}else{
			printstream(fout, "addi a5, s0, -"+std::to_string(ids[0]));
			if (!exprisarray[name].back()){
				printstream(fout, "lw a5,0(a5)");
				if (!exprisaddress[name].back() && this->isAddress()){
					printstream(fout, "li a3,4");
					printstream(fout, "mul a5,a3, a5");
				}
			}
		}
		if (exprisaddress[name].back())
			this->setAddress(true);
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
		expr1->setAddress(this->isAddress());
		expr1->printto(fout);
		if (expr2 != NULL){
			expr2->setAddress(this->isAddress());
			expr3->setAddress(this->isAddress());
			int branch = branchnum;
			branchnum++;
			printstream(fout, "beqz a5, .L"+std::to_string(branch));

			expr2->printto(fout);
			int branch2 = branchnum;
			branchnum++;
			printstream(fout, "j .L"+std::to_string(branch2));
			decIndent();
			printstream(fout, ".L"+std::to_string(branch)+":");
			addIndent();
			expr3->printto(fout);
			decIndent();
			printstream(fout, ".L"+std::to_string(branch2)+":");
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
			expr_list[i]->setAddress(this->isAddress());
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
		if (expr->isAddress()){
			printstream(fout, "li a4,4");
			printstream(fout, "div a5,a5,a4");
		}
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
	TypeAst* type;
	string name;
	ExprAst* expr;
	vector<int> position;
	bool isArray;
public:
	LocalVariableAst(int row, int column) : StmtAst(row, column){}
	string getname() {return name;}
	void additem(TypeAst* type, string name, vector<int> num, ExprAst* item, bool b){
		this->type = type;
		this->name = name;
		expr = item;
		position = num;
		position[0] = (position[0]*4+functionvalue+4);
		isArray = b;
	}
	void printto(ofstream &fout){
		exprnum[name].push_back(this->position);
		exprisarray[name].push_back(isArray);
		exprisaddress[name].push_back(type->isAddress);
		if (expr!=NULL){
			expr->printto(fout);
			printstream(fout, "sw a5, -"+ std::to_string(exprnum[name].back()[0])+"(s0)");
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
		for (int i = 0; i < name_list.size(); ++i){
			exprnum[name_list[i]].pop_back();
			exprisarray[name_list[i]].pop_back();
			exprisaddress[name_list[i]].pop_back();
		}
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
		int branch = branchnum;
		branchnum++;
		printstream(fout, "beqz a5, .L"+std::to_string(branch));
		stmt1->printto(fout);
		int branch2 = branchnum;
		if (stmt2!=NULL){
			printstream(fout, "j .L"+std::to_string(branch2));
			branchnum++;
		}
		decIndent();
		printstream(fout, ".L"+std::to_string(branch)+":");
		addIndent();
		if (stmt2!=NULL){
			stmt2->printto(fout);
			decIndent();
			printstream(fout, ".L"+std::to_string(branch2)+":");
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
		int branch = branchnum;
		printstream(fout, ".L"+std::to_string(branch)+":");
		branchnum++;
		addIndent();


		int branch2 = branchnum;
		branchnum++;

		break_list.push_back(".L"+std::to_string(branch2));

		int branch3 = branchnum;
		branchnum++;

		continue_list.push_back(".L"+std::to_string(branch3));

		if (stmt2->hasExpr()){
			stmt2->printto(fout);
			printstream(fout, "beqz a5, .L"+std::to_string(branch2));
		}
		stmt->printto(fout);
		decIndent();
		printstream(fout, ".L"+std::to_string(branch3)+":");
		addIndent();
		stmt3->printto(fout);
		printstream(fout, "j .L"+std::to_string(branch));
		decIndent();
		printstream(fout, ".L"+std::to_string(branch2)+":");
		addIndent();
		if (isdec){
			exprnum[declaration].pop_back();
			exprisarray[declaration].pop_back();
			exprisaddress[declaration].pop_back();
		}
		continue_list.pop_back();
		break_list.pop_back();
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
		int branch = branchnum;
		branchnum++;
		printstream(fout, ".L"+std::to_string(branch)+":");
		addIndent();

		int branch2 = branchnum;
		branchnum++;


		continue_list.push_back(".L"+std::to_string(branch));
		break_list.push_back(".L"+std::to_string(branch2));

		expr->printto(fout);
		printstream(fout, "beqz a5, .L"+std::to_string(branch2));
		stmt->printto(fout);
		printstream(fout, "j .L"+std::to_string(branch));
		
		decIndent();
		printstream(fout, ".L"+std::to_string(branch2)+":");
		addIndent();

		continue_list.pop_back();
		break_list.pop_back();
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
		int branch = branchnum;
		branchnum++;
		printstream(fout, ".L"+std::to_string(branch)+":");
		addIndent();


		int branch2 = branchnum;
		branchnum++;

		continue_list.push_back(".L"+std::to_string(branch));
		break_list.push_back(".L"+std::to_string(branch2));
		stmt->printto(fout);
		expr->printto(fout);
		printstream(fout, "beqz a5, .L"+std::to_string(branch2));
		printstream(fout, "j .L"+std::to_string(branch));
		
		decIndent();
		printstream(fout, ".L"+std::to_string(branch2)+":");
		addIndent();
		continue_list.pop_back();
		break_list.pop_back();
	}
};

class ContinueAst: public StmtAst{
public:
	ContinueAst(int row, int column) : StmtAst(row, column){}
	void printto(ofstream &fout){
		printstream(fout, "j "+continue_list.back());
	}
};

class BreakAst: public StmtAst{
public:
	BreakAst(int row, int column) : StmtAst(row, column){}
	void printto(ofstream &fout){
		printstream(fout, "j "+break_list.back());
	}
};

class FunctionAst: public Ast{
	TypeAst* type;
	string name;
	vector<TypeAst*> expr_type;
	vector<string> expr_name;
	StmtAst* stmt;
	int variable_num;
public:
	FunctionAst(int row, int column) : Ast(row, column){}
	void additem(TypeAst* type, string name, StmtAst* item, int num){
		this->type = type;
		this->name = name;
		stmt = item;
		variable_num = num;
	}
	void additem(TypeAst* type, string item){
		expr_type.push_back(type);
		expr_name.push_back(item);
	}
	void printto(ofstream &fout){
		if (stmt==NULL)
			return;
		function_name = name;
		printstream(fout, ".globl "+name);
		printstream(fout, ".type "+name+", @function");
		decIndent();
		printstream(fout, name+":");
		addIndent();
		int num = variable_num + expr_name.size();
		printstream(fout, "addi	sp,sp,-"+std::to_string(functionvalue+num*4));
		printstream(fout, "sw	ra,"+std::to_string(4+num*4)+"(sp)");
		printstream(fout, "sw	s0,"+std::to_string(num*4)+"(sp)");
		printstream(fout, "addi	s0,sp,"+std::to_string(functionvalue+num*4));
		if (expr_name.size() < 8){
			for (int i = 0; i < expr_name.size(); ++i){
				vector<int> position;
				position.push_back(functionvalue+variable_num*4+4+4*i);
				exprnum[expr_name[i]].push_back(position);
				exprisarray[expr_name[i]].push_back(false);
				exprisaddress[expr_name[i]].push_back(expr_type[i]->isAddress);
				printstream(fout, "sw	a"+std::to_string(i)+",-"+std::to_string(functionvalue+variable_num*4+4+4*i)+"(s0)");
			}
		}
		if (stmt != NULL)
			stmt->printto(fout);

		printstream(fout, "li	a5, 0");
		printstream(fout, "mv	a0, a5");
		decIndent();
		printstream(fout, "."+name+"_exit:");
		addIndent();
		printstream(fout, "lw	s0,"+std::to_string(num*4)+"(sp)");
		printstream(fout, "lw	ra,"+std::to_string(4+num*4)+"(sp)");
		printstream(fout, "addi	sp,sp,"+std::to_string(functionvalue+num*4));
		printstream(fout, "jr	ra");
		decIndent();
		addIndent();
		printstream(fout, ".size "+name+", .-"+name);
		printstream(fout, ".ident	\"GCC: (xPack GNU RISC-V Embedded GCC, 64-bit) 8.3.0\"");
		for (int i = 0; i < expr_name.size(); ++i){
			exprnum[expr_name[i]].pop_back();
			exprisarray[expr_name[i]].pop_back();
			exprisaddress[expr_name[i]].pop_back();
		}
	}
};

class ProgramAst: public Ast{
	vector<FunctionAst*> function_list;
	vector<string> expr_name;
	vector<TypeAst*> expr_type;
	vector<int> expr_length;
	vector<vector<int>> expr_num;
	vector<int> expr_value;
	vector<bool> expr_array;
public:
	ProgramAst(int row, int column) : Ast(row, column){}
	void additem(FunctionAst* func){
		function_list.push_back(func);
	}
	void additem(TypeAst* type, string s, vector<int> num, int length, int v, int b = false){
		expr_name.push_back(s);
		expr_value.push_back(v);
		expr_length.push_back(length);
		expr_num.push_back(num);
		expr_type.push_back(type);
		expr_array.push_back(b);
	}
	void printto(ofstream &fout, string filename){
		addIndent();
		printstream(fout, ".file	\""+filename+"\"");
		printstream(fout, ".option nopic");
		if (expr_name.size() > 0){
			printstream(fout, ".data");
			for (int i = 0; i < expr_name.size(); ++i){
				if (!expr_array[i]){
					printstream(fout, ".globl "+expr_name[i]);
					printstream(fout, ".align  2");
					decIndent();
					printstream(fout, expr_name[i]+":");
					addIndent();
					printstream(fout, ".word "+std::to_string(expr_value[i]));
					vector<int> position;
					position.push_back(-1);
					exprnum[expr_name[i]].push_back(position);
					exprisarray[expr_name[i]].push_back(false);
					exprisaddress[expr_name[i]].push_back(expr_type[i]->isAddress);
				}else{
					printstream(fout, ".globl "+expr_name[i]);
					printstream(fout, ".align  2");
					decIndent();
					printstream(fout, expr_name[i]+":");
					addIndent();
					printstream(fout, ".zero "+std::to_string(4*expr_length[i]));
					exprnum[expr_name[i]].push_back(expr_num[i]);
					exprisarray[expr_name[i]].push_back(true);
					exprisaddress[expr_name[i]].push_back(expr_type[i]->isAddress);
				}
			}
		}
		printstream(fout, ".text");
		printstream(fout, ".align	1");
		for (int i = 0; i < function_list.size(); ++i)
			function_list[i]->printto(fout);
		decIndent();

		printall(fout);
	}
};

#endif