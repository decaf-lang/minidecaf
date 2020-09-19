grammar MiniDecaf;

import CommonLex;

prog
    : (func | decl_global ';')* EOF
    ;

func
    : type Identifier '(' (type Identifier ',')* (type Identifier)? ')' ('{' blockItem* '}' | ';')
    ;

blockItem
    : stmt
    | decl ';'
    ;

decl_global
    : type Identifier ('=' Interger)?                               # globalVar
    ;

decl
    : type Identifier ('=' expr)?                                   # varDef
    ; 
    
stmt
    : 'return' expr ';'                                             # returnStmt
    | expr ';'                                                      # singleExpr
    | 'if' '(' expr ')' stmt ('else' stmt)?                         # ifStmt
    | '{' blockItem* '}'                                            # block
    | 'while' '(' expr ')' stmt                                     # whileLoop
    | 'do' stmt 'while' '(' expr ')' ';'                            # doWhile
    | 'for' '(' (decl | expr)? ';' (expr)? ';' (expr)? ')' stmt     # forLoop
    | 'break' ';'                                                   # break
    | 'continue' ';'                                                # continue
    | ';'                                                           # nop
    ;

expr
    : Identifier '(' (expr ',')* (expr)? ')'                        # funcCall
    | ('!' | '~' | '-') expr                                        # unaryOp
    | expr ('*' | '/' | '%') expr                                   # mulDiv
    | expr ('+' | '-') expr                                         # addSub
    | expr ('<' | '<=' | '>' | '>=') expr                           # lessGreat
    | expr ('==' | '!=') expr                                       # equal
    | expr '&&' expr                                                # land
    | expr '||' expr                                                # lor
    | expr '?' expr ':' expr                                        # condExpr
    | '(' expr ')'                                                  # atomParen
    | Identifier '=' expr                                           # assign
    | Identifier                                                    # Identifier
    | Interger                                                      # integer
    ;

type
    : 'int'
    ;