// step 1
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
    : Integer
    ;

