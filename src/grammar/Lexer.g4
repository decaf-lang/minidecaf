lexer grammar Lexer;

Return: 'return';
Int: 'int';
Main: 'main';

LeftParen: '(';
RightParen: ')';
LeftBrace: '{';
RightBrace: '}';

Semi: ';';

Integer: [0-9]+;

Whitespace: [ \t]+ -> skip;
Newline: ('\r' '\n'? | '\n') -> skip;
LineComment: '//' ~[\r\n]* -> skip;

Invalid: .;
