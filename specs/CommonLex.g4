// 祖传 lex
lexer grammar CommonLex;

Integer: Digit+;

Whitespace: [ \t\n\r]+ -> skip;

Ident: IdentLead WordChar*;

fragment IdentLead: [a-zA-Z_];
fragment WordChar: [0-9a-zA-Z_];
fragment Digit: [0-9];
