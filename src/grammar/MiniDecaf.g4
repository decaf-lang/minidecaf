grammar MiniDecaf;

import Lexer;

program
    : func EOF
    ;

func
    : Int Main '(' ')' '{' stmt '}'
    ;

stmt
    : Return expr ';'
    ;

expr
    : Integer
    ;
