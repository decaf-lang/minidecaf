grammar MiniDecaf;

import CommonLex;

prog
    : func EOF
    ;

func
    : type Identifier '(' ')' '{' stmt* '}'
    ;

stmt
    : 'return' expr ';'                                             # returnStmt
    | expr ';'                                                      # singleExpr
    | type Identifier ('=' expr)? ';'                               # varDef
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
    | Identifier '=' expr                                           # assign
    | Identifier                                                    # Identifier
    | Interger                                                      # integer
    ;

type
    : 'int'
    ;