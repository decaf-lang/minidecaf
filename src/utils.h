#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <tuple>
#include <iostream>

template<typename T>
using symTab = std::unordered_map<std::string, std::unordered_map<std::string, T> >;

/*
    Type Class used to construct the type system, inherit abstract class Type to define
    different data type, such as Int, Intprt, None etc. 
*/
class Type {
public:
    virtual std::string getType() = 0;
    // Explicitly cast to dst type
    virtual std::shared_ptr<Type> typeCast(int _valueType) = 0;
    // Distinguish different pointer type by counting thier number of "*"
    virtual int getStarNum() { return starNum; }
    void setValueType(int _valueType) { valueType = _valueType; }
    int getValueType() { return valueType; }
    // When we define or assign to a varible, we need to check the src & dst type
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

/*
    Interger type
*/
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

/*
    Pointer of integer type
*/
class IntptrType : public Type {
public:
    IntptrType(int _starNum) {
        starNum = _starNum; typeLiteral = "Intptr"; valueType = 0;
    }
    IntptrType(int _starNum, int _valueType) {
        starNum = _starNum; typeLiteral = "Intptr"; valueType = _valueType;
    }
    std::shared_ptr<Type> typeCast(int _valueType) {
        std::shared_ptr<Type> type = std::make_shared<IntptrType>(starNum, _valueType);
        return type;
    }
    std::string getType() { return typeLiteral; }
};

/*
    None type. eg. The return type of a for loop, if statement.
*/
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

/*
    A symbol in the symbol table.
    Symbol stores a variable's name, offset in stack (if local), defined in which line and typeinfo.
*/
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

/*
    FuncSymbol stores information of defined functions;
    Function name, parameter's type, return type, initial state (used to differentiate declare & define) are considered.
*/
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

/*
    Singleton mode, defining a static, global map used by all the files in the project.
    We use map structure to store our symbol table.
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