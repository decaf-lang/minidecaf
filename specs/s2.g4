// step 2,3,4
grammar MiniDecaf;

import CommonLex;

prog
    : func
    ;

func
    : ty 'main' '(' ')' '{' stmt '}'
    ;

ty
    : 'int'
    ;

stmt
    : 'return' expr ';' # returnStmt
    ;

expr
    : lor
    ;

lor
    : land # tLor
    | lor '||' land # cLor
    ;

land
    : rel # tLand
    | land '&&' rel # cLand
    ;

rel
    : add # tRel
    | add relOp add # cRel
    ;

add
    : mul # tAdd
    | add addOp mul # cAdd
    ;

mul
    : unary # tMul
    | mul mulOp unary # cMul
    ;

unary
    : atom # tUnary
    | unaryOp unary # cUnary
    ;

atom
    : Integer # atomInteger
    | '(' expr ')' # atomParen
    ;



unaryOp
    : '-' | '!'
    ;

addOp
    : '+' | '-'
    ;

mulOp
    : '*' | '/' | '%'
    ;

relOp
    : '==' | '!=' | '<' | '>' | '<=' | '>='
    ;
