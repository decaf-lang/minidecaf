grammar MiniDecaf;

prog:
	func* EOF;

func:
	type IDENT '(' (type IDENT (',' type IDENT)*)? ')' '{' blockItem* '}'	# definedFunc
	| type IDENT '(' (type IDENT (',' type IDENT)*)? ')' ';'				# declaredFunc;

type: 'int';

blockItem: localDecl | stmt;

localDecl: type IDENT ('=' expr)? ';';

stmt:
	expr? ';' # exprStmt
	| 'return' expr ';' # returnStmt
	| 'if' '(' expr ')' stmt ('else' stmt)? # ifStmt
	| '{' blockItem* '}' # blockStmt
	| 'while' '(' expr ')' stmt # whileStmt
	| 'for' '(' (localDecl | expr? ';') expr? ';' expr? ')' stmt	# forStmt
	| 'do' stmt 'while' '(' expr ')' ';' # doStmt
	| 'break' ';' # breakStmt
	| 'continue' ';' # continueStmt
	;

expr: IDENT '=' expr | ternary;

ternary: lor '?' expr ':' ternary | lor;

lor: lor '||' lor | land;

land: land '&&' land | equ;

equ: equ ('==' | '!=') equ | rel;

rel: rel ('<'|'>'|'<='|'>=') rel | add;

add: add ('+' | '-') add | mul;

mul: mul ('*' | '/' | '%') mul | unary;

unary: ('-' | '!' | '~') unary | postfix;

postfix: IDENT '(' (expr (',' expr)*)? ')' | primary;

primary:
	NUM # numPrimary
	| IDENT # identPrimary
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
