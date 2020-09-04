grammar MiniDecaf;

import CommonLex;

prog
    : func EOF
    ;

func
    : 'int' 'main' '(' ')' '{' stmt '}'
    ;

stmt
    : 'return' expr ';'                                             # returnStmt
    ;

expr
    : ('!' | '~' | '-') expr                                        # unaryOp
    | expr ('*' | '/' | '%') expr                                   # mulDiv
    | expr ('+' | '-') expr                                         # addSub
    | '(' expr ')'                                                  # atomParen
    | Interger                                                      # integer 
    ;
