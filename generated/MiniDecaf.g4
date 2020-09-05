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
    | expr ('<' | '<=' | '>' | '>=') expr                           # lessGreat
    | expr ('==' | '!=') expr                                       # equal
    | expr '&&' expr                                                # land
    | expr '||' expr                                                # lor
    | expr ('+' | '-') expr                                         # addSub
    | '(' expr ')'                                                  # atomParen
    | Interger                                                      # integer
    ;
