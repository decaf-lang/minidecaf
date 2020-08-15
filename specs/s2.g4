// step 2,3,4
grammar MiniDecaf;

import CommonLex;

prog
    : func EOF
    ;

func
    : ty 'main' '(' ')' '{' stmt '}'
    ;

ty
    : 'int' # intType
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
    : eq # tLand
    | land '&&' eq # cLand
    ;

eq
    : rel # tEq
    | eq eqOp rel # cEq
    ;

rel
    : add # tRel
    | rel relOp add # cRel
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
    : '-' | '!' | '~'
    ;

addOp
    : '+' | '-'
    ;

mulOp
    : '*' | '/' | '%'
    ;

relOp
    : '<' | '>' | '<=' | '>='
    ;

eqOp
    : '==' | '!='
    ;
