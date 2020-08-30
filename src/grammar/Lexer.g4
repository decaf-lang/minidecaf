lexer grammar Lexer;

Return: 'return';
Int: 'int';
Main: 'main';

LeftParen: '(';
RightParen: ')';
LeftBrace: '{';
RightBrace: '}';

Semi: ';';

Plus: '+';
Minus: '-';
Mul: '*';
Div: '/';
Mod: '%';
Not: '!';
BitNot: '~';
And: '&&';
Or: '||';
Equal: '==';
NotEqual: '!=';
Less: '<';
LessEqual: '<=';
Greater: '>';
GreaterEqual: '>=';

Integer: [0-9]+;

Whitespace: [ \t]+ -> skip;
Newline: ('\r' '\n'? | '\n') -> skip;
LineComment: '//' ~[\r\n]* -> skip;

Invalid: .;
