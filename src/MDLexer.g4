lexer grammar MDLexer;

WhiteSpaces: [ \t\r\n]+ -> skip;

Integer:    [0-9]+;
PLUS:       '+';
MINUS:      '-';
STAR:       '*';
SLASH:      '/';
SEMICOLON:  ';';

