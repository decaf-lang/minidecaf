grammar MiniDecaf;

import Lexer;

program
    : func EOF
    ;

func
    : Int Ident '(' ')' '{' blockItem* '}'
    ;

blockItem
    : stmt
    | decl
    ;

decl
    : Int Ident ('=' expr)? ';'
    ;

stmt
    : Return expr ';'                       # ReturnStmt
    | expr ';'                              # ExprStmt
    | If '(' expr ')' stmt ('else' stmt)?   # IfStmt
    | '{' blockItem* '}'                    # BlockStmt
    ;

expr
    : condExpr
    | assignExpr
    ;

assignExpr
    : Ident '=' expr
    ;

condExpr
    : orExpr ('?' expr ':' condExpr)?
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
