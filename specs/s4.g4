// step 6,7,8
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
    | 'if' '(' expr ')' th=stmt ('else' el=stmt)? # IfStmt
    | '{' stmt* '}' # blockStmt
    | 'for' '(' init=decl ';' ctrl=expr ';' post=expr? ')' stmt # forDeclStmt
    | 'for' '(' init=expr? ';' ctrl=expr ';' post=expr? ')' stmt # forStmt
    ;

expr
    : asgn
    ;

decl
    : ty Ident ('=' expr)?
    ;

asgn
    : cond # tAsgn
    | unary asgnOp asgn # cAsgn
    ;

cond
    : lor # tCond
    | lor '?' expr ':' cond # cCond
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
    | Ident # aotmIdent
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

