grammar MiniDecaf;

prog:
	func EOF;

func: type IDENT '(' ')' '{' stmt '}';

type: 'int';

stmt: 'return' expr ';';

expr: add;

add: add ('+' | '-') add | mul;

mul: mul ('*' | '/' | '%') mul | unary;

unary: ('-' | '!' | '~') unary | primary;

primary:
	NUM # numPrimary
	| '(' expr ')' # parenthesizedPrimary
	;

/* lexer */
WS: [ \t\r\n\u000C] -> skip;

IDENT: [a-zA-Z_] [a-zA-Z_0-9]*;
NUM: [0-9]+;

// comment
// The specification of minidecaf doesn't allow commenting,
// but we provide the comment feature here for the convenience of debugging.
COMMENT: '/*' .*? '*/' -> skip;
LINE_COMMENT: '//' ~[\r\n]* -> skip;
