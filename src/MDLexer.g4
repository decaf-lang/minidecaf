lexer grammar MDLexer;

WhiteSpaces: [ \t\r\n]+ -> skip;

Integer:    [0-9]+;
PLUS:       '+';
MINUS:      '-';
STAR:       '*';
SLASH:      '/';
EQ:         '==';
NE:         '!=';
LT:         '<';
GT:         '>';
LE:         '<=';
GE:         '>=';
SEMICOLON:  ';';
LPAREN:     '(';
RPAREN:     ')';

