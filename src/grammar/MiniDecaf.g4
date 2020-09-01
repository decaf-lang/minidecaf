grammar MiniDecaf;

import Lexer;

program
    : (decl | func)* EOF
    ;

type
    : Int
    | type '*'
    ;

paramList
    : (type Ident (',' type Ident)*)?
    ;

func
    : type Ident '(' paramList ')' ('{' blockItem* '}' | ';')
    ;

blockItem
    : stmt
    | decl
    ;

decl
    : type Ident ('[' Integer ']')* ('=' expr)? ';'
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
    : unary '=' expr
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
    : unary
    | mulExpr ('*' | '/' | '%') unary
    ;

unary
    : postfix                               # PostfixExpr
    | '(' type ')' unary                    # CastExpr
    | ('-' | '~' | '!' | '*' | '&') unary   # UnaryExpr
    ;

postfix
    : primary                           # PrimaryExpr
    | postfix '[' expr ']'              # IndexExpr
    | Ident '(' (expr (',' expr)*)? ')' # FuncCall
    ;

primary
    : Integer       # IntExpr
    | Ident         # IdentExpr
    | '(' expr ')'  # NestedExpr
    ;
