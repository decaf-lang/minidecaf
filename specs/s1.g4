// step 1
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
    : Integer
    ;

