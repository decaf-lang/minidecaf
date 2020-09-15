grammar MiniDecaf;

import CommonLex;

prog
    : func EOF
    ;

func
    : type Identifier '(' ')' '{' blockItem* '}'
    ;

blockItem
    : stmt
    | decl ';'
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
    : ('!' | '~' | '-') expr                                        # unaryOp
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