lexer grammar MDLexer;

WhiteSpaces: [ \t\r\n]+ -> skip;

Integer:    [0-9]+;
Identifier: [a-zA-Z_][a-zA-Z0-9_]*;

ASSIGN:     '=';
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

