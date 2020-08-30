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
    : orExpr
    ;

orExpr
    : andExpr
    | orExpr '||' andExpr
    ;

andExpr
    : equalExpr
    | andExpr '&&' equalExpr
    ;

equalExpr
    : relExpr
    | equalExpr ('==' | '!=') relExpr
    ;

relExpr
    : addExpr
    | relExpr ('<' | '>' | '<=' | '>=') addExpr
    ;

addExpr
    : mulExpr
    | addExpr ('+' | '-') mulExpr
    ;

mulExpr
    : factor
    | mulExpr ('*' | '/' | '%') factor
    ;

factor
    : Integer                   # IntExpr
    | '(' expr ')'              # NestedExpr
    | ('-' | '~' | '!') factor  # UnaryExpr
    ;
