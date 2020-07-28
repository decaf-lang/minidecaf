## step1 基本文法
只有正整数，大致形式是 ` 0;` 或 `5;`

## 基本文法
```
prog : decl+
decl : vardel | funcdel
vardecl : 'var' ident ':' type ('=' initval)? ';'
type : 'int' | 'bool' | '[' type ']'
funcdecl : 'def' ident '(' argList ')' (':' retTy=type)? blockstmt
argList : (argbinding (',' argbinding)*)?
argbinding : ident ':' type
blockstmt : '{' (stmt (';' stmt)*)? '}'
stmt : blockstmt | vardecl | asgnstmt | skipstmt | ifstmt | whilestmt | exprstmt | returnstmt | intrinsicstmt
asgnstmt : lvalue '=' expr ';'
skipstmt : 'skip'  ';' // i.e. pass in python
ifstmt : 'if' '(' expr ')' stmt ('else' stmt)?
whilestmt : 'while' '(' expr ')' stmt
exprstmt : expr ';'
intrinsicstmt : ('print'|'read') exprList ';'
returnstnt : 'return' expr? ';'
lvalue : expr ('[' expr ']')
exprList : (expr (',' expr)*)?
expr : unary / binary / relop / loadstore / arr / paren / cast / literal / newarray / call
```

## feature文法
小 feature 如 ternary expr ; var 以外加上 val ; 允许 var 做类型推断。
