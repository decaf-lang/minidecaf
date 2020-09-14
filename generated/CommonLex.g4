lexer grammar CommonLex;

// keyword
Int 
    : 'int'
    ;

Return
    : 'return'
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

Minus : '-';
Exclamation : '!';
Tilde : '~';
Addition : '+';
Multiplication : '*';
Division : '/';
Modulo : '%';
LAND : '&&';
LOR : '||';
EQ : '==';
NEQ : '!=';
LT : '<';
LE : '<=';
GT : '>';
GE : '>=';


// integer, identifier
Interger
    : [0-9]+
    ;

Identifier
    : [a-zA-Z_][a-zA-Z_0-9]*
    ;

WS : 
    [ \t\r\n] -> skip
    ;
