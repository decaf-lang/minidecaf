grammar MiniDecaf;

import Lexer;

program
    : (decl | func)* EOF
    ;

paramList
    : (Int Ident (',' Int Ident)*)?
    ;

func
    : Int Ident '(' paramList ')' ('{' blockItem* '}' | ';')
    ;

blockItem
    : stmt
    | decl
    ;

decl
    : Int Ident ('=' expr)? ';'
    ;

stmt
    : Return expr ';'                                                       # ReturnStmt
    | If '(' expr ')' stmt ('else' stmt)?                                   # IfStmt
    | For '(' (decl | init=expr? ';') cond=expr? ';' post=expr? ')' stmt    # ForStmt
    | While '(' expr ')' stmt                                               # WhileStmt
    | Do stmt While '(' expr ')' ';'                                        # DoStmt
    | Break ';'                                                             # BreakStmt
    | Continue ';'                                                          # ContinueStmt
    | '{' blockItem* '}'                                                    # BlockStmt
    | expr? ';'                                                             # ExprStmt
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
    : Integer                           # IntExpr
    | Ident                             # IdentExpr
    | '(' expr ')'                      # NestedExpr
    | ('-' | '~' | '!') factor          # UnaryExpr
    | Ident '(' (expr (',' expr)*)? ')' # FuncCall
    ;
