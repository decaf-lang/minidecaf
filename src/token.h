#ifndef __TOKEN_H__
#define __TOKEN_H__
#include <string.h>
#include <map>
using std::string;
using std::map;
class Token{
	string label_;
	string value_;
	int intvalue_, row_, column_; 
public:
	static map<string, int> tokenid;

	string label(){return label_;}
	int row(){return row_;}
	int column(){return column_;}

	Token(string label, string value, int row, int column): label_(label), value_(value), row_(row), column_(column){
		if (label == "num"){
			intvalue_ = 0;
			for (int i = 0; i < value.length(); ++i)
				intvalue_ = intvalue_ * 10 + int(value[i] - '0');
		}else {}
	}

	string value() {return value_;}
	int intvalue() {return intvalue_;}
};

map<string, int> Token::tokenid= {
		{"int", 0}, {"id", 1},  {"(", 2}, {")", 3}, {"{", 4}, 
		{"}", 5}, {"return", 6}, {"num", 7},  {";", 8}
	};
#endif