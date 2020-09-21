#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <tuple>
#include <iostream>

template<typename T>
using symTab = std::unordered_map<std::string, std::unordered_map<std::string, T> >;

class Type {
public:
    virtual std::string getType() = 0;
    virtual std::shared_ptr<Type> typeCast(int _valueType) = 0;
    virtual int getStarNum() { return starNum; }
    void setValueType(int _valueType) { valueType = _valueType; }
    int getValueType() { return valueType; }
    bool typeCheck(std::shared_ptr<Type> srcType) {
        if (srcType.get()->getType() == typeLiteral && srcType.get()->getStarNum() == starNum) {
            return true;
        }
        return false;
    }
    bool typeCheckLiteral(std::string srcType) { 
        if (srcType == typeLiteral) {
            return true;
        }
        return false;
    }
    virtual ~Type() {}
protected:
    int valueType;
    int starNum;
    std::string typeLiteral;
};

class IntType : public Type {
public:
    IntType() {
        valueType = 0; typeLiteral = "Int"; starNum = 0;
    }
    IntType(int _valueType) {
        valueType = _valueType; typeLiteral = "Int", starNum = 0;
    }
    std::shared_ptr<Type> typeCast(int _valueType) {
        std::shared_ptr<Type> type = std::make_shared<IntType>(_valueType);
        return type;
    }
    std::string getType() { return typeLiteral; }
};

class IntptrType : public Type {
public:
    IntptrType(int _starNum) {
        starNum = _starNum; typeLiteral = "Intptr"; valueType = 0;
    }
    IntptrType(int _starNum, int _valueType) {
        starNum = _starNum; typeLiteral = "Intptr"; valueType = _valueType;
    }
    std::shared_ptr<Type> typeCast(int _valueType) {
        std::shared_ptr<Type> type = std::make_shared<IntptrType>(_valueType);
        return type;
    }
    std::string getType() { return typeLiteral; }
};

class NoneType : public Type {
public:
    NoneType() {
        valueType = 0; typeLiteral = "None";
    }
    std::shared_ptr<Type> typeCast(int _valueType) {
        std::cerr << "[Warning] Casting Nonetype symbol " << typeLiteral << "\n";
        return std::shared_ptr<Type>(this);
    }
    std::string getType() { return typeLiteral; }
};

class Symbol {
public:
    Symbol(std::string _literal, int _offset, std::shared_ptr<Type> _type, int _line=-1) {
        literal = _literal; offset = _offset; type = _type; line = _line; 
    }
    std::string getLiteral() { return literal; }
    int getOffset() { return offset; }
    std::shared_ptr<Type> getType() { return type; }
    int getLineNum() { return line; }

    void setLineNum(int _line) { line = _line; }

protected:
    std::string literal;
    int offset, line;
    std::shared_ptr<Type> type;
};

class FuncSymbol {
public:
    FuncSymbol(std::string _literal, std::shared_ptr<Type> _retType, 
    std::vector<std::shared_ptr<Type> > _argType, bool _init=false) {
        literal = _literal; retType = _retType; argType = _argType; init = _init;
    }
    std::string getLiteral() { return literal; }
    std::shared_ptr<Type> getRetType() { return retType; }
    std::shared_ptr<Type> getArgType(int i) { return argType[i]; }
    bool initialized() { return init; }
    void initialize() { init = true; }

protected:
    bool init;
    std::string literal;
    std::vector<std::shared_ptr<Type> > argType;
    std::shared_ptr<Type> retType;
};

/*
    Symbol table, which stores function, scope & varible information.
    The table is organized as follows:
    ------------------------------------------------------
    | Function | Variable1 | Offset in stack | Data type |
    |          | Variable2 | ...             | ...       |
    |          | ...       | ...             | ...       |
    ------------------------------------------------------
*/

class Singleton {
public:
    symTab<std::shared_ptr<Symbol> > symbolTable;
    std::unordered_map<std::string, std::shared_ptr<FuncSymbol> > funcTable;
    static Singleton& getInstance() {
        static Singleton pInstance;
        return pInstance;
    }
private:
    Singleton() {}
    Singleton(const Singleton& other) {};
};