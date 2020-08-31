grammar MiniDecaf;

import Lexer;

program
    : func EOF
    ;

func
    : Int Ident '(' ')' '{' stmt* '}'
    ;

stmt
    : Return expr ';'               # ReturnStmt
    | expr ';'                      # ExprStmt
    | Int Ident ('=' expr)? ';'     # Decl
    ;

expr
    : orExpr
    | assignExpr
    ;

assignExpr
    : Ident '=' expr
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
    | Ident                     # IdentExpr
    | '(' expr ')'              # NestedExpr
    | ('-' | '~' | '!') factor  # UnaryExpr
    ;
