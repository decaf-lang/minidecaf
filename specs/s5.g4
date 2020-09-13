// step 9,10
grammar MiniDecaf;

import CommonLex;

prog
    : externalDecl+ EOF
    ;

externalDecl
    : func # funcExternalDecl
    | decl ';' # declExternalDecl
    ;

func
    : ty Ident '(' paramList ')' block # funcDef
    | ty Ident '(' paramList ')' ';' # funcDecl
    ;

ty
    : 'int' # intType
    ;

stmt
    : 'return' expr ';' # returnStmt
    | expr ';' # exprStmt
    | ';' # nullStmt
    | 'if' '(' expr ')' th=stmt ('else' el=stmt)? # IfStmt
    | block # blockStmt
    | 'for' '(' init=decl ';' ctrl=expr? ';' post=expr? ')' stmt # forDeclStmt
    | 'for' '(' init=expr? ';' ctrl=expr? ';' post=expr? ')' stmt # forStmt
    | 'while' '(' expr ')' stmt # whileStmt
    | 'do' stmt 'while' '(' expr ')' ';' # doWhileStmt
    | 'break' ';' # breakStmt
    | 'continue' ';' # continueStmt
    ;

expr
    : asgn
    ;

decl
    : ty Ident ('=' expr)?
    ;

blockItem
    : stmt # blockItemStmt
    | decl ';' # blockItemDecl
    ;

block
    : '{' blockItem* '}'
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

