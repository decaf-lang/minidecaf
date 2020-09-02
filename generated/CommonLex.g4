lexer grammar CommonLex;

// keyword
Int 
    : 'int'
    ;

Return
    : 'return'
    ;

Main
    : 'main'
    ;

// operator
Lparen : '(' ;
Rparen : ')' ;
Lbrkt : '[' ;
Rbrkt : ']' ;
Lbrace : '{' ;
Rbrace : '}' ;
Comma : ',' ;
Semicolon : ';' ;

Punctuator
    : Lparen
    | Rparen
    | Lbrkt
    | Rbrkt
    | Lbrace
    | Rbrace
    | Comma
    | Semicolon
    ;


// integer, identifier
Interger
    : [0-9]+
    ;

WS : 
    [ \t\r\n] -> skip
    ;
