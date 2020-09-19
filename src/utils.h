#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <tuple>
#include <iostream>

template<typename T>
using symTab = std::unordered_map<std::string, std::unordered_map<std::string, T> >;

symTab<std::shared_ptr<Type> > symbolTable;

class Type {
public:
    virtual std::string getType() = 0;
protected:
    int valueType;
    std::string typeLiteral;
};

class IntType : public Type {
public:
    IntType() {
        valueType = 0; typeLiteral = "Int";
    }
    IntType(int _valueType) {
        valueType = _valueType; typeLiteral = "Int";
    }
    std::string getType() { return typeLiteral; }
    int getValue() { return valueType; }
};

class IntptrType : public Type {
    IntptrType(int _starNum) {
        starNum = _starNum; typeLiteral = "Intptr"; valueType = 0;
    }
    IntptrType(int _starNum, int _valueType) {
        starNum = _starNum; typeLiteral = "Intptr"; valueType = _valueType;
    }
    std::string getType() { return typeLiteral; }
    int getStarNum() { return starNum; }
protected:
    int starNum;
};


class Symbol {
public:
    Symbol(std::string _literal, int _offset, std::shared_ptr<Type> _type) {
        literal = _literal; offset = _offset; _type = type;
    }
    std::string getLiteral() { return literal; }
    int getOffset() { return offset; }
    std::shared_ptr<Type> getType() { return type; }
protected:
    std::string literal;
    int offset;
    std::shared_ptr<Type> type;
};