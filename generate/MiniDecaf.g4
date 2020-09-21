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
    | expr? ';'                                                      # singleExpr
    | 'if' '(' expr ')' stmt ('else' stmt)?                         # ifStmt
    | '{' blockItem* '}'                                            # block
    | 'while' '(' expr ')' stmt                                     # whileLoop
    | 'do' stmt 'while' '(' expr ')' ';'                            # doWhile
    | 'for' '(' (decl | expr)? ';' (expr)? ';' (expr)? ')' stmt     # forLoop
    | 'break' ';'                                                   # break
    | 'continue' ';'                                                # continue
    ;

expr
    : factor '=' expr                                               # assign
    | cond                                                          # cond_nop
    ;

cond
    : lor_op '?' expr ':' cond                                      # condExpr
    | lor_op                                                        # lor_nop 
    ;

lor_op
    : lor_op '||' lor_op                                            # lor
    | land_op                                                       # land_nop
    ;

land_op
    : land_op '&&' land_op                                          # land
    | equ                                                           # equ_nop
    ;

equ
    : equ ('==' | '!=') equ                                         # equal
    | rel                                                           # rel_nop
    ;

rel
    : rel ('<' | '<=' | '>' | '>=') rel                             # lessGreat
    | add                                                           # add_nop
    ;

add
    : add ('+' | '-') add                                           # addSub
    | mul                                                           # mul_nop
    ;

mul
    : mul ('*' | '/' | '%') mul                                     # mulDiv
    | factor                                                        # factor_nop
    ;

factor
    : ('!' | '~' | '-' | '*' | '&') factor                          # unaryOp
    | '(' type ')' factor                                           # cast
    | Identifier '(' (expr ',')* (expr)? ')'                        # funcCall
    | '(' expr ')'                                                  # atomParen
    | Identifier                                                    # Identifier
    | Interger                                                      # integer
    ;

type
    : 'int' '*'*                                                    # intType
    ;