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
    : 'int'
    ;

stmt
    : 'return' expr ';' # returnStmt
    ;

expr
    : Integer
    ;

