// step 9,10
grammar MiniDecaf;

import CommonLex;

prog
    : (func | decl ';')+
    ;

func
    : ty Ident '(' paramList ')' block # funcDef
    | ty Ident '(' paramList ')' ';' # funcDecl
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
    | block # blockStmt
    | 'for' '(' init=decl ';' ctrl=expr? ';' post=expr? ')' stmt # forDeclStmt
    | 'for' '(' init=expr? ';' ctrl=expr? ';' post=expr? ')' stmt # forStmt
    ;

expr
    : asgn
    ;

decl
    : ty Ident ('=' expr)?
    ;

block
    : '{' stmt* '}'
    ;

paramList
    : (decl (',' decl)*)?
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
    | Ident '(' argList ')' # atomCall
    ;

argList
    : (expr (',' expr)*)?
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

