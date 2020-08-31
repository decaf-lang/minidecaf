#ifndef __LEXER_H__
#define __LEXER_H__
#include "token.h"
#include <fstream>
#include <iostream>
#include <string.h>
#include <vector>
using std::string;
using std::vector;
using std::ifstream;
class Lexer{
	vector<Token> tokenlist;
	ifstream fin;
	int row;
public:
	Lexer(string filename){
		row = 0;
		fin.open(filename);
		tokenlist.clear();
	}

	bool isSpace(char ch){
		if(ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t' || ch == '\377')
        	return true;
        return false;
	}

	bool is0to9(char ch){
        return (ch >= '0' && ch <= '9');
	}

	string getNum(string &input, int &column, int length){
		string ans = "";
		while (column < length){
			if (is0to9(input[column]))
				ans.push_back(input[column]);
			else
				break;
			column++;
		}
		column--;
		return ans;
	}

	bool isatozAtoZ(char ch){
        return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'));
	}

	bool isLetter(char ch){
        return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch=='_');
	}

	string getLetter(string &input, int &column, int length){
		string ans = "";
		while (column < length){
			if (isLetter(input[column]))
				ans.push_back(input[column]);
			else
				break;
			column++;
		}
		column--;
		return ans;
	}

	bool isChar(char ch){
        return (ch == '(' || ch == ')' || ch == '{' || ch == '}' || ch == ';' || ch == '!' 
        	 || ch == '~' || ch == '-' || ch == '+' || ch == '*' || ch == '/' || ch == '%' 
        	 || ch == '>' || ch == '<' || ch == '=' || ch == '&' || ch == '|'
         	);
	}

	string getChar(string &input, int &column, int length){
		string ans = "";
		ans.push_back(input[column]);
		column++;
		while (column < length){
			if (isChar(input[column]))
				ans.push_back(input[column]);
			else
				break;
			if (ans == "=" || ans == "&" || ans == "|"){
				column++;
				continue;
			}
			if (Token::tokenid.count(ans) == 0){
				ans.pop_back();
				break;
			}
			column++;
		}
		column--;
		return ans;
	}

	vector<Token>& analyse(){
		string input;
    	while(getline(fin,input)){
    		for (int column = 0; column < input.length(); ++column){
    			if (isSpace(input[column]))
    				continue;
    			if (is0to9(input[column])){
    				int numcolumn = column;
    				int numrow = row;
    				string num = getNum(input, column, input.length());
    				tokenlist.push_back(Token("num", num, numrow, numcolumn));
    			}else if (isatozAtoZ(input[column])){
    				int numcolumn = column;
    				int numrow = row;
    				string letter = getLetter(input, column, input.length());
    				if (Token::tokenid.count(letter) == 0){
    					tokenlist.push_back(Token("id", letter, numrow, numcolumn));
    				}else
    					tokenlist.push_back(Token(letter, letter, numrow, numcolumn));
    			}else if (isChar(input[column])){
    				int numcolumn = column;
    				int numrow = row;
    				string chars = getChar(input, column, input.length());
    				if (Token::tokenid.count(chars) == 0){
    					std::cout << "Unrecognized character in Row " <<row << " column " << column <<" " << chars << std::endl;
    					exit(-1);
    				}
    				tokenlist.push_back(Token(chars, chars, numrow, numcolumn));
    			}else{
    				std::cout << "Unrecognized character in Row " <<row << " column " << column <<" " << input[column] << std::endl;
    				exit(-1);
    			}
			}
			row ++;
    	}
    	// for (int i = 0; i < tokenlist.size(); ++i){
    	// 	std::cout << tokenlist[i].label() << std::endl;
    	// }
		fin.close();
		return tokenlist;
	}
};
#endif