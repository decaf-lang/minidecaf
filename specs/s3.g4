// step 5
grammar MiniDecaf;

import CommonLex;

prog
    : func
    ;

func
    : ty 'main' '(' ')' '{' stmt* '}'
    ;

ty
    : 'int'
    ;

stmt
    : 'return' expr ';' # returnStmt
    | decl ';' # declStmt
    | expr ';' # exprStmt
    | ';' # nullStmt
    ;

expr
    : asgn
    ;

decl
    : ty Ident ('=' expr)?
    ;

asgn
    : lor # tAsgn
    | unary asgnOp asgn # cAsgn
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
    | Ident # atomIdent
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

asgnOp
    : '='
    ;

