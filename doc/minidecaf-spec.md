# 第一种选择
缺点：step1-10的input还不是正常的C程序

- * means the preceding element is repeated zero or more times
- + means one or more repeats
- ? means optional(zero or one)

## 步骤1：有整数表示
只有正整数，大致形式是 `0` 或 `5`
```
digit  =  [0-9]
```
## 步骤2：可执行加减
```
expr= num ("+" num | "-" num)*
num = digit+
digit  =  [0-9]
```
## 步骤3：递归下降语法解析
```
expr= num ("+" num | "-" num)*
num = digit+
digit  =  [0-9]
```
## 步骤4：有一定的错误提示
```
expr= num ("+" num | "-" num)*
num = digit+
digit  =  [0-9]
```
## 步骤5：stack computer on RV64
```
expr= num ("+" num | "-" num)*
num = digit+
digit  =  [0-9]
```
## 步骤6：可执行乘除
```
expr = mul ("+" mul | "-" mul)*
mul  = num ("*" num | "/" num)*
num = digit+
digit  =  [0-9]
```
## 步骤7：支持括号操作
```
expr    = mul ("+" mul | "-" mul)*
mul     = primary ("*" primary | "/" primary)*
primary = num | "(" expr ")"
num = digit+
digit  =  [0-9]
```
## 步骤8：支持一元操作符

```
expr    = mul ("+" mul | "-" mul)*
mul     = unary ("*" unary | "/" unary)*
unary   = ("+" | "-")? primary
primary = num | "(" expr ")"
num = digit+
digit  =  [0-9]
```

## 步骤9：支持比较操作

```
expr       = equality
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = ("+" | "-")? primary
primary    = num | "(" expr ")"
num = digit+
digit  =  [0-9]
```
## 步骤10：支持变量与赋值语句
```
program    = stmt*
stmt       = expr ";"
expr       = assign
assign     = equality ("=" assign)?
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = ("+" | "-")? primary
primary    = num | ident | "(" expr ")"
num = digit+
digit  =  [0-9]
ident = [a-z]+[a-z0-9_]*
```

## 步骤11：支持返回语句
```
program    = stmt*
stmt    = expr ";"
        | "return" expr ";"
expr       = assign
assign     = equality ("=" assign)?
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = ("+" | "-")? primary
primary    = num | ident | "(" expr ")"
num = digit+
digit  =  [0-9]
ident = [a-z]+[a-z0-9_]*
```

## 步骤12：支持条件语句
```
program    = stmt*
stmt    = expr ";"
        | "return" expr ";"
        | "if" "(" expr ")" stmt ("else" stmt)?
        | "while" "(" expr ")" stmt
        | "for" "(" expr? ";" expr? ";" expr? ")" stmt
expr       = assign
assign     = equality ("=" assign)?
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = ("+" | "-")? primary
primary    = num | ident | "(" expr ")"
num = digit+
digit  =  [0-9]
ident = [a-z]+[a-z0-9_]*
```

## 步骤13：支持语句块和函数
支持语句块
```
program    = stmt*
stmt    = expr ";"
        | "return" expr ";"
        | "if" "(" expr ")" stmt ("else" stmt)?
        | "while" "(" expr ")" stmt
        | "for" "(" expr? ";" expr? ";" expr? ")" stmt
        | "{" stmt* "}"
expr       = assign
assign     = equality ("=" assign)?
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = ("+" | "-")? primary
primary    = num | ident | "(" expr ")"
num = digit+
digit  =  [0-9]
ident = [a-z]+[a-z0-9_]*
```
支持函数
```
program    = stmt*
stmt    = expr ";"
        | "return" expr ";"
        | "if" "(" expr ")" stmt ("else" stmt)?
        | "while" "(" expr ")" stmt
        | "for" "(" expr? ";" expr? ";" expr? ")" stmt
        | "{" stmt* "}"
expr       = assign
assign     = equality ("=" assign)?
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = ("+" | "-")? primary
primary = num
        | ident ("(" ")")?
        | "(" expr ")"
num = digit+
digit  =  [0-9]
ident = [a-z]+[a-z0-9_]*
```


## 步骤14/15/.../X：支持字符串操作/支持数组操作等
需要如下步骤
1. 一元`&`：返回变量地址和一元`*`：返回地址指向的值
1. 删除隐式变量定义并引入关键字int
1. 实现指针加法和减法
1. sizeof运算符
1. 实现数组
1. 实现数组下标
1. 全局变量
1. 字符类型
1. 字符串字面量
1. ...

```
program    = toplv*
toplv      = typ ident ("(" (typ ident ("," typ ident)*)? ")" "{" stmt* "}" | ("[" num "]")* ";")
stmt       = expr ";"
           | "{" stmt* "}"
           | "if" "(" expr ")" stmt ("else" stmt)?
           | "while" "(" expr ")" stmt
           | "for" "(" expr? ";" expr? ";" expr? ")" stmt
           | "return" expr ";"
           | typ ident ("[" num "]")* ";"
expr       = assign
assign     = equality ("=" assign)?
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = "+"? primary
           | "-"? primary
           | "&" unary
           | "*" unary
           | "sizeof" unary
           | primary "[" expr "]"
primary    = num
           | ident ("(" (expr ("," expr)*)? ")")?
           | "(" expr ")"
typ        = ("int" | "char") "*"*
num = digit+
digit  =  [0-9]
ident = [a-z]+[a-z0-9_]*
```


## 步骤X：完整的基本文法

mini-decaf几乎就是一个C子集
下面的内容可能有bug
```
program    = toplv*
toplv      = typ ident ("(" (typ ident ("," typ ident)*)? ")" "{" stmt* "}" | ("[" num "]")* ";")
stmt       = expr ";"
           | "{" stmt* "}"
           | "if" "(" expr ")" stmt ("else" stmt)?
           | "while" "(" expr ")" stmt
           | "for" "(" expr? ";" expr? ";" expr? ")" stmt
           | "return" expr ";"
           | typ ident ("[" num "]")* ";"
expr       = assign
assign     = equality ("=" assign)?
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = "+"? primary
           | "-"? primary
           | "&" unary
           | "*" unary
           | "sizeof" unary
           | primary "[" expr "]"
primary    = num
           | ident ("(" (expr ("," expr)*)? ")")?
           | "(" expr ")"
typ        = ("int" | "char") "*"*
```


# 第二种选择

特点：每个步骤的input都是一个正常的C程序，可以用于所写编译器的编译出的目标程序的运行结果与gcc等正常编译器的编译出的目标程序的运行结果进行直接比较

还缺 指针，数组，string等相关的spec

## 步骤1：Integers
只有正整数，大致形式是 `0` 或 `5`，语言语法为：

```
<program> ::= <function>
<function> ::= "int" "main" "(" ")" "{" <statement> "}"
<statement> ::= "return" Integer ";"
<exp> ::= Integer
```

例子：

```
int main() {
    return 2;
}
```

语义规定：

- 为尽可能兼容指针，本实验中所有`int`类型均为64位长度。

对应的Token为：

```
Open brace {
Close brace }
Open parenthesis \(
Close parenthesis \)
Semicolon ;
Int keyword int
Return keyword return
Integer literal [0-9]+
Main keyword main
```
对应的AST为：
```
program = Program(function_declaration)
function_declaration = Function("main", statement)
statement = Return(exp) | Assign(variable, exp)
exp = Constant(int)
```

## 步骤2：Unary Operators
有"- ~ !"操作，语言语法为：

```
<program> ::= <function>
<function> ::= "int" "main" "(" ")" "{" <statement> "}"
<statement> ::= "return" <exp> ";"
<exp> ::= <unary_op> <exp> | Integer
<unary_op> ::= "!" | "~" | "-"
```

例子：
```
int main() {
    return -2;
}
```

对应的Token为：
```
Open brace {
Close brace }
Open parenthesis \(
Close parenthesis \)
Semicolon ;
Int keyword int
Return keyword return
Integer literal [0-9]+
Negation -
Bitwise complement ~
Logical negation !
Main keyword main
```

对应的AST为：
```
program = Program(function_declaration)
function_declaration = Function("main", statement)
statement = Return(exp)
exp = UnOp(operator, exp) | Constant(int)
```

## 步骤3：Binary Operators and Parenthesis for ( expr )
支持

```
Addition +
Subtraction -
Multiplication *
Division /
Modular %
```

语言语法为：

```
<program> ::= <function>
<function> ::= "int" "main" "(" ")" "{" <statement> "}"
<statement> ::= "return" <exp> ";"
<exp> ::= <term> { ("+" | "-") <term> }
<term> ::= <factor> { ("*" | "/" | "%") <factor> }
<factor> ::= "(" <exp> ")" | <unary_op> <factor> | Integer
<unary_op> ::= "!" | "~" | "-"
```

对应的Token为：
```
Open brace {
Close brace }
Open parenthesis (
Close parenthesis )
Semicolon ;
Int keyword int
Return keyword return
Integer literal [0-9]+
Minus -
Bitwise complement ~
Logical negation !
Addition +
Multiplication *
Division /
Main keyword main
```

对应的AST为：
```
program = Program(function_declaration)
function_declaration = Function("main", statement)
statement = Return(exp)
exp = BinOp(binary_operator, exp, exp)
    | UnOp(unary_operator, exp)
    | Constant(int)
```

## 步骤4：Logical Binary Operators
支持

```
Logical AND &&
Logical OR ||
Equal to ==
Not equal to !=
Less than <
Less than or equal to <=
Greater than >
Greater than or equal to >=
```
语言语法为：

```
<program> ::= <function>
<function> ::= "int" "main" "(" ")" "{" <statement> "}"
<statement> ::= "return" <exp> ";"
<exp> ::= <logical-and-exp> { "||" <logical-and-exp> }
<logical-and-exp> ::= <equality-exp> { "&&" <equality-exp> }
<equality-exp> ::= <relational-exp> { ("!=" | "==") <relational-exp> }
<relational-exp> ::= <additive-exp> { ("<" | ">" | "<=" | ">=") <additive-exp> }
<additive-exp> ::= <term> { ("+" | "-") <term> }
<term> ::= <factor> { ("*" | "/" | "%") <factor> }
<factor> ::= "(" <exp> ")" | <unary_op> <factor> | Integer
<unary_op> ::= "!" | "~" | "-"
```
语义规定：

- 本实验中是否实现短路求值不做要求，即属于未定义行为。例如，在后续步骤引入局部变量后可能出现如下程序：

  ```
  int x = 0;
  int a = 1 || (x = 1);
  return x;
  ```

  返回 0 或 1 均被接受。

对应的Token为：

```
Open brace {
Close brace }
Open parenthesis (
Close parenthesis )
Semicolon ;
Int keyword int
Return keyword return
Integer literal [0-9]+
Minus -
Bitwise complement ~
Logical negation !
Addition +
Multiplication *
Division /
AND &&
OR ||
Equal ==
Not Equal !=
Less than <
Less than or equal <=
Greater than >
Greater than or equal >=
Main keyword main
```
对应的AST为：
```
program = Program(function_declaration)
function_declaration = Function("main", statement)
statement = Return(exp)
exp = BinOp(binary_operator, exp, exp)
    | UnOp(unary_operator, exp)
    | Constant(int)
```

## 步骤5：Local Variables
支持Local Variables

```
int a;
a=3;
...
```
语言语法为：

```
<program> ::= <function>
<function> ::= "int" Identifier "(" ")" "{" { <statement> } "}"
<statement> ::= "return" <exp> ";"
              | <exp> ";"
              | "int" Identifier [ "=" <exp>] ";"
<exp> ::= Identifier "=" <exp> | <logical-or-exp>
<logical-or-exp> ::= <logical-and-exp> { "||" <logical-and-exp> }
<logical-and-exp> ::= <equality-exp> { "&&" <equality-exp> }
<equality-exp> ::= <relational-exp> { ("!=" | "==") <relational-exp> }
<relational-exp> ::= <additive-exp> { ("<" | ">" | "<=" | ">=") <additive-exp> }
<additive-exp> ::= <term> { ("+" | "-") <term> }
<term> ::= <factor> { ("*" | "/" | "%") <factor> }
<factor> ::= "(" <exp> ")" | <unary_op> <factor> | Integer | Identifier
<unary_op> ::= "!" | "~" | "-"
```
> 注意：本步骤中 `main` 函数的函数名不再是一个关键字，而是与变量名同属于 ID。这意味着名为 `main` 的局部变量是合法的。

语义规定：

- 若函数中缺少`return` 语句，则一律返回0。

对应的Token为：

```
Open brace {
Close brace }
Open parenthesis (
Close parenthesis )
Semicolon ;
Int keyword int
Return keyword return
Identifier [a-zA-Z_][a-zA-Z0-9_]*
Integer literal [0-9]+
Minus -
Bitwise complement ~
Logical negation !
Addition +
Multiplication *
Division /
AND &&
OR ||
Equal ==
Not Equal !=
Less than <
Less than or equal <=
Greater than >
Greater than or equal >=
Assignment =
```

对应的AST为：
```
program = Program(function_declaration)
function_declaration = Function(string, statement list) //string is the function name
statement = Return(exp)
          | Declare(string, exp option) //string is variable name
                                        //exp is optional initializer
          | Exp(exp)
exp = Assign(string, exp)
    | Var(string) //string is variable name
    | BinOp(binary_operator, exp, exp)
    | UnOp(unary_operator, exp)
    | Constant(int)
```

## 步骤6：Conditional statements & expressions
例如

```
if (flag) {
  int e = a ? b : c;
  return a*2;
}
```
，语言语法为：

```
<program> ::= <function>
<function> ::= "int" Identifier "(" ")" "{" { <block-item> } "}"
<block-item> ::= <statement> | <declaration>
<declaration> ::= "int" Identifier [ "=" <exp> ] ";"
<statement> ::= "return" <exp> ";"
              | <exp> ";"
              | "if" "(" <exp> ")" <statement> [ "else" <statement> ]


<exp> ::= Identifier "=" <exp> | <conditional-exp>
<conditional-exp> ::= <logical-or-exp> [ "?" <exp> ":" <conditional-exp> ]
<logical-or-exp> ::= <logical-and-exp> { "||" <logical-and-exp> }
<logical-and-exp> ::= <equality-exp> { "&&" <equality-exp> }
<equality-exp> ::= <relational-exp> { ("!=" | "==") <relational-exp> }
<relational-exp> ::= <additive-exp> { ("<" | ">" | "<=" | ">=") <additive-exp> }
<additive-exp> ::= <term> { ("+" | "-") <term> }
<term> ::= <factor> { ("*" | "/" | "%") <factor> }
<factor> ::= "(" <exp> ")" | <unary_op> <factor> | Integer | Identifier
<unary_op> ::= "!" | "~" | "-"
```
> 注意： `<declaration>` 和 `<statement>` 的定义细节与上一步骤不完全相同。这是因为形如 `if (1) int x;` 的语句是非法的。

语义规定：

- `?:`操作符应具有短路性质。例如

  ```
  int x = 0;
  int a = 1 ? 2 : (x = 1);
  return x;
  ```

  应返回 0。

对应的Token为：

```
Open brace {
Close brace }
Open parenthesis (
Close parenthesis )
Semicolon ;
Int keyword int
Return keyword return
Identifier [a-zA-Z_][a-zA-Z0-9_]*
Integer literal [0-9]+
Minus -
Bitwise complement ~
Logical negation !
Addition +
Multiplication *
Division /
AND &&
OR ||
Equal ==
Not Equal !=
Less than <
Less than or equal <=
Greater than >
Greater than or equal >=
Assignment =
If keyword if
Else keyword else
Colon :
Question mark ?
```
对应的AST为：
```
program = Program(function_declaration)

function_declaration = Function(string, block_item list) //string is the function name

block_item = Statement(statement) | Declaration(declaration)

declaration = Declare(string, exp option) //string is variable name
                                          //exp is optional initializer

statement = Return(exp)
          | Exp(exp)
          | Conditional(exp, statement, statement option) //exp is controlling condition
                                                          //first statement is 'if' block
                                                          //second statement is optional 'else' block

exp = Assign(string, exp)
    | Var(string) //string is variable name
    | BinOp(binary_operator, exp, exp)
    | UnOp(unary_operator, exp)
    | Constant(int)
    | CondExp(exp, exp, exp) //the three expressions are the condition, 'if' expression and 'else' expression, respectively
```

## 步骤7：Compound Statements and Definition Scope
例如

```
int main() {
    int a = 2;
    {
        int a;
        a = 4;  // here a == 4
    }
    // here a == 2
}
```
语言语法为：

```
<program> ::= <function>
<function> ::= "int" Identifier "(" ")" "{" { <block-item> } "}"
<block-item> ::= <statement> | <declaration>
<declaration> ::= "int" Identifier [ "=" <exp> ] ";"
<statement> ::= "return" <exp> ";"
              | <exp> ";"
              | "if" "(" <exp> ")" <statement> [ "else" <statement> ]
              | "{" { <block-item> } "}
<exp> ::= Identifier "=" <exp> | <conditional-exp>
<conditional-exp> ::= <logical-or-exp> [ "?" <exp> ":" <conditional-exp> ]
<logical-or-exp> ::= <logical-and-exp> { "||" <logical-and-exp> }
<logical-and-exp> ::= <equality-exp> { "&&" <equality-exp> }
<equality-exp> ::= <relational-exp> { ("!=" | "==") <relational-exp> }
<relational-exp> ::= <additive-exp> { ("<" | ">" | "<=" | ">=") <additive-exp> }
<additive-exp> ::= <term> { ("+" | "-") <term> }
<term> ::= <factor> { ("*" | "/" | "%") <factor> }
<factor> ::= "(" <exp> ")" | <unary_op> <factor> | Integer | Identifier
<unary_op> ::= "!" | "~" | "-"
```
语义说明：

- 局部变量的作用域开始于**局部变量定义之处**（而不是语句块开始处），结束于**所在语句块末尾**。因此，程序片段  `int a = 1; { return a; int a = 2;}`  应返回 1 而不是 2。

对应的Token为（不变）：

```
Open brace {
Close brace }
Open parenthesis (
Close parenthesis )
Semicolon ;
Int keyword int
Return keyword return
Identifier [a-zA-Z_][a-zA-Z0-9_]*
Integer literal [0-9]+
Minus -
Bitwise complement ~
Logical negation !
Addition +
Multiplication *
Division /
AND &&
OR ||
Equal ==
Not Equal !=
Less than <
Less than or equal <=
Greater than >
Greater than or equal >=
Assignment =
If keyword if
Else keyword else
Colon :
Question mark ?
```
对应的AST为：
```
program = Program(function_declaration)

function_declaration = Function(string, block_item list) //string is the function name

block_item = Statement(statement) | Declaration(declaration)

declaration = Declare(string, exp option) //string is variable name
                                          //exp is optional initializer

statement = Return(exp)
          | Exp(exp)
          | Conditional(exp, statement, statement option) //exp is controlling condition
                                                          //first statement is 'if' block
                                                          //second statement is optional 'else' block
          | Compound(block_item list)

exp = Assign(string, exp)
    | Var(string) //string is variable name
    | BinOp(binary_operator, exp, exp)
    | UnOp(unary_operator, exp)
    | Constant(int)
    | CondExp(exp, exp, exp) //the three expressions are the condition, 'if' expression and 'else' expression, respectively
```

## 步骤8：Loops
例如

```
int i;
for (i = 0; i < 10; i = i + 1) {
    //do something
}

while (i < 10) {
    i  = i + 1;
}
```
语言语法为：

```
<program> ::= <function>
<function> ::= "int" Identifier "(" ")" "{" { <block-item> } "}"
<block-item> ::= <statement> | <declaration>
<declaration> ::= "int" Identifier [ "=" <exp> ] ";"
<statement> ::= "return" <exp> ";"
              | <exp-option-semicolon> // null statement
              | "if" "(" <exp> ")" <statement> [ "else" <statement> ]
              | "{" { <block-item> } "}
              | "for" "(" <exp-option-semicolon> <exp-option-semicolon> <exp-option-close-paren> <statement>
              | "for" "(" <declaration> <exp-option-semicolon> <exp-option-close-paren> <statement>
              | "while" "(" <exp> ")" <statement>
              | "do" <statement> "while" "(" <exp> ")" ";"
              | "break" ";"
              | "continue" ";"
<exp-option-semicolon> ::= <exp> ";" | ";"
<exp-option-close-paren> ::= <exp> ")" | ")"
<exp> ::= Identifier "=" <exp> | <conditional-exp>
<conditional-exp> ::= <logical-or-exp> [ "?" <exp> ":" <conditional-exp> ]
<logical-or-exp> ::= <logical-and-exp> { "||" <logical-and-exp> }
<logical-and-exp> ::= <equality-exp> { "&&" <equality-exp> }
<equality-exp> ::= <relational-exp> { ("!=" | "==") <relational-exp> }
<relational-exp> ::= <additive-exp> { ("<" | ">" | "<=" | ">=") <additive-exp> }
<additive-exp> ::= <term> { ("+" | "-") <term> }
<term> ::= <factor> { ("*" | "/" | "%") <factor> }
<factor> ::= "(" <exp> ")" | <unary_op> <factor> | Integer | Identifier
<unary_op> ::= "!" | "~" | "-"
```
语义说明：

- 每个`for`循环意味着一个隐式的局部变量作用域。`...; for (...; ...; ...) { ... } ...;` 事实上等价于 `...; { for (...; ...; ...) { ... } } ...;` 。

> 注意：程序片段 `for(int i;;) { int i; }` 的两个局部变量分属内外两个不同的作用域内，因此是合法的。

对应的Token为：

```
{
}
(
)
;
int
return
Identifier [a-zA-Z_][a-zA-Z0-9_]*
Integer literal [0-9]+
-
~
!
+
*
/
&&
||
==
!=
<
<=
>
>=
=
if
else
:
?
for
while
do
break
continue
```
对应的AST为：
```
program = Program(function_declaration)

function_declaration = Function(string, block_item list) //string is the function name

block_item = Statement(statement) | Declaration(declaration)

declaration = Declare(string, exp option) //string is variable name
                                          //exp is optional initializer

statement = Return(exp)
          | Exp(exp option)
          | Conditional(exp, statement, statement option) // exp is controlling condition
                                                          // first statement is 'if' block
                                                          // second statement is optional 'else' block
          | Compound(block_item list)
          | For(exp option, exp option, exp option, statement) // initial expression, condition, post-expression, body
          | ForDecl(declaration, exp option, exp option, statement) // initial declaration, condition, post-expression, body
          | While(expression, statement) // condition, body
          | Do(statement, expression) // body, condition
          | Break
          | Continue

exp = Assign(string, exp)
    | Var(string) //string is variable name
    | BinOp(binary_operator, exp, exp)
    | UnOp(unary_operator, exp)
    | Constant(int)
    | CondExp(exp, exp, exp) //the three expressions are the condition, 'if' expression and 'else' expression, respectively
```
## 步骤9：Functions
例如

```
int three() {
    return 3;
}

int main() {
    return three();
}
```
语言语法为：

```
<program> ::= { <function> }
<function> ::= "int" Identifier "(" [ "int" Identifier { "," "int" Identifier } ] ")" ( "{" { <block-item> } "}" | ";" )
<block-item> ::= <statement> | <declaration>
<declaration> ::= "int" Identifier [ "=" <exp> ] ";"
<statement> ::= "return" <exp> ";"
              | <exp-option-semicolon> // null statement
              | "if" "(" <exp> ")" <statement> [ "else" <statement> ]
              | "{" { <block-item> } "}
              | "for" "(" <exp-option-semicolon> <exp-option-semicolon> <exp-option-close-paren> ")" <statement>
              | "for" "(" <declaration> <exp-option-semicolon> <exp-option-close-paren> ")" <statement>
              | "while" "(" <exp> ")" <statement>
              | "do" <statement> "while" <exp> ";"
              | "break" ";"
              | "continue" ";"
<exp-option-semicolon> ::= <exp> ";" | ";"
<exp-option-close-paren> ::= <exp> ")" | ")"
<exp> ::= Identifier "=" <exp> | <conditional-exp>
<conditional-exp> ::= <logical-or-exp> [ "?" <exp> ":" <conditional-exp> ]
<logical-or-exp> ::= <logical-and-exp> { "||" <logical-and-exp> }
<logical-and-exp> ::= <equality-exp> { "&&" <equality-exp> }
<equality-exp> ::= <relational-exp> { ("!=" | "==") <relational-exp> }
<relational-exp> ::= <additive-exp> { ("<" | ">" | "<=" | ">=") <additive-exp> }
<additive-exp> ::= <term> { ("+" | "-") <term> }
<term> ::= <factor> { ("*" | "/" | "%") <factor> }
<factor> ::= <function-call> | "(" <exp> ")" | <unary_op> <factor> | Integer | Identifier
<function-call> ::= Identifier "(" [ <exp> { "," <exp> } ] ")"
<unary_op> ::= "!" | "~" | "-"
```
语义说明：

- 本实验对于同名函数的重复声明不做要求，即属于未定义行为。测例中不会出现同名函数重复声明的情况。

对应的Token为：

```
{
}
(
)
;
int
return
Identifier [a-zA-Z_][a-zA-Z0-9_]*
Integer literal [0-9]+
-
~
!
+
*
/
&&
||
==
!=
<
<=
>
>=
=
if
else
:
?
for
while
do
break
continue
,
```
对应的AST为：
```
program = Program(function_declaration list)

function_declaration = Function(string, block_item list) //string is the function name

block_item = Statement(statement) | Declaration(declaration)

declaration = Declare(string, exp option) //string is variable name
                                          //exp is optional initializer

statement = Return(exp)
          | Exp(exp option)
          | Conditional(exp, statement, statement option) // exp is controlling condition
                                                          // first statement is 'if' block
                                                          // second statement is optional 'else' block
          | Compound(block_item list)
          | For(exp option, exp option, exp option, statement) // initial expression, condition, post-expression, body
          | ForDecl(declaration, exp option, exp option, statement) // initial declaration, condition, post-expression, body
          | While(expression, statement) // condition, body
          | Do(statement, expression) // body, condition
          | Break
          | Continue

exp = Assign(string, exp)
    | Var(string) //string is variable name
    | BinOp(binary_operator, exp, exp)
    | UnOp(unary_operator, exp)
    | Constant(int)
    | CondExp(exp, exp, exp) //the three expressions are the condition, 'if' expression and 'else' expression, respectively
    | FunCall(string, exp list) // string is the function name
```

## 步骤10：Global Variables
例如

```
int foo;

int fun1() {
    foo = 3;
    return 0;
}

int fun2() {
    return foo;
}

int main() {
    fun1();
    return fun2();
}
```
语言语法为：

```
<program> ::= { <function> | <declaration> }
<function> ::= "int" Identifier "(" [ "int" Identifier { "," "int" Identifier } ] ")" ( "{" { <block-item> } "}" | ";" )
<block-item> ::= <statement> | <declaration>
<declaration> ::= "int" Identifier [ "=" <exp> ] ";"
<statement> ::= "return" <exp> ";"
              | <exp-option-semicolon> // null statement
              | "if" "(" <exp> ")" <statement> [ "else" <statement> ]
              | "{" { <block-item> } "}
              | "for" "(" <exp-option-semicolon> <exp-option-semicolon> <exp-option-close-paren> ")" <statement>
              | "for" "(" <declaration> <exp-option-semicolon> <exp-option-close-paren> ")" <statement>
              | "while" "(" <exp> ")" <statement>
              | "do" <statement> "while" <exp> ";"
              | "break" ";"
              | "continue" ";"
<exp-option-semicolon> ::= <exp> ";" | ";"
<exp-option-close-paren> ::= <exp> ")" | ")"
<exp> ::= Identifier "=" <exp> | <conditional-exp>
<conditional-exp> ::= <logical-or-exp> [ "?" <exp> ":" <conditional-exp> ]
<logical-or-exp> ::= <logical-and-exp> { "||" <logical-and-exp> }
<logical-and-exp> ::= <equality-exp> { "&&" <equality-exp> }
<equality-exp> ::= <relational-exp> { ("!=" | "==") <relational-exp> }
<relational-exp> ::= <additive-exp> { ("<" | ">" | "<=" | ">=") <additive-exp> }
<additive-exp> ::= <term> { ("+" | "-") <term> }
<term> ::= <factor> { ("*" | "/" | "%") <factor> }
<factor> ::= <function-call> | "(" <exp> ")" | <unary_op> <factor> | Integer | Identifier
<function-call> ::= Identifier "(" [ <exp> { "," <exp> } ] ")"
<unary_op> ::= "!" | "~" | "-"
```

语义说明：

- 本实验对同名全局变量的重复定义不做要求，即属于未定义行为。测例中不会出现同名全局变量的重复定义。
- 全局变量若没有初始化，则默认初始化为0。
- 编译器只需实现用常数初始化全局变量，而无需实现表达式初始化。（但局部变量应能使用表达式初始化）

对应的Token为(不变)：

```
{
}
(
)
;
int
return
Identifier [a-zA-Z_][a-zA-Z0-9_]*
Integer literal [0-9]+
-
~
!
+
*
/
&&
||
==
!=
<
<=
>
>=
=
if
else
:
?
for
while
do
break
continue
,
```
对应的AST为：
```
toplevel_item = Function(function_declaration)
              | Variable(declaration)
toplevel = Program(toplevel_item list)
program = Program(function_declaration list)

function_declaration = Function(string, block_item list) //string is the function name

block_item = Statement(statement) | Declaration(declaration)

declaration = Declare(string, exp option) //string is variable name
                                          //exp is optional initializer

statement = Return(exp)
          | Exp(exp option)
          | Conditional(exp, statement, statement option) // exp is controlling condition
                                                          // first statement is 'if' block
                                                          // second statement is optional 'else' block
          | Compound(block_item list)
          | For(exp option, exp option, exp option, statement) // initial expression, condition, post-expression, body
          | ForDecl(declaration, exp option, exp option, statement) // initial declaration, condition, post-expression, body
          | While(expression, statement) // condition, body
          | Do(statement, expression) // body, condition
          | Break
          | Continue

exp = Assign(string, exp)
    | Var(string) //string is variable name
    | BinOp(binary_operator, exp, exp)
    | UnOp(unary_operator, exp)
    | Constant(int)
    | CondExp(exp, exp, exp) //the three expressions are the condition, 'if' expression and 'else' expression, respectively
    | FunCall(string, exp list) // string is the function name
```

## 步骤11：Pointers

主要改动部分

```diff
 <program> ::= { <function> | <declaration> }
-<function> ::= "int" Identifier "(" [ "int" Identifier { "," "int" Identifier } ] ")" ( "{" { <block-item> } "}" | ";" )
+<type> ::= "int" | <type> "*"
+<param-list> ::= [ <type> Identifier { "," <type> Identifier } ]
+<function> ::= <type> Identifier "(" <param-list> ")" ( "{" { <block-item> } "}" | ";" )
 <block-item> ::= <statement> | <declaration>
-<declaration> ::= "int" Identifier [ "=" <exp> ] ";"
+<declaration> ::= <type> Identifier [ "=" <exp> ] ";"
 <statement> ::= "return" <exp> ";"
               | <exp-option-semicolon> // null statement
               | "if" "(" <exp> ")" <statement> [ "else" <statement> ]
               | "{" { <block-item> } "}
               | "for" "(" <exp-option-semicolon> <exp-option-semicolon> <exp-option-close-paren> ")" <statement>
               | "for" "(" <declaration> <exp-option-semicolon> <exp-option-close-paren> ")" <statement>
               | "while" "(" <exp> ")" <statement>
               | "do" <statement> "while" <exp> ";"
               | "break" ";"
               | "continue" ";"
 <exp-option-semicolon> ::= <exp> ";" | ";"
 <exp-option-close-paren> ::= <exp> ")" | ")"
-<exp> ::= Identifier "=" <exp> | <conditional-exp>
+<exp> ::= <factor> "=" <exp> | <conditional-exp>
 <conditional-exp> ::= <logical-or-exp> [ "?" <exp> ":" <conditional-exp> ]
 <logical-or-exp> ::= <logical-and-exp> { "||" <logical-and-exp> }
 <logical-and-exp> ::= <equality-exp> { "&&" <equality-exp> }
 <equality-exp> ::= <relational-exp> { ("!=" | "==") <relational-exp> }
 <relational-exp> ::= <additive-exp> { ("<" | ">" | "<=" | ">=") <additive-exp> }
 <additive-exp> ::= <term> { ("+" | "-") <term> }
 <term> ::= <factor> { ("*" | "/" | "%") <factor> }
-<factor> ::= <function-call> | "(" <exp> ")" | <unary_op> <factor> | Integer | Identifier
+<factor> ::= <function-call> | "(" <exp> ")" | <unary_op> <factor> | "(" <type> ")" <factor> | Integer | Identifier
 <function-call> ::= Identifier "(" [ <exp> { "," <exp> } ] ")"
-<unary_op> ::= "!" | "~" | "-"
+<unary_op> ::= "!" | "~" | "-" | "&" | "*"
```

一些说明：

1. `<factor> "=" <exp>` 和 `"&" <factor>` 中的 `<factor>` 要求是左值（lvalue），即不允许 `&(2+3)` 这种。lvalue 的定义如下：
    * `Identifier` 和 `"*" <factor>` 是 lvalue；
    * 如果 `<exp>` 是 lvalue，`"(" <exp> ")"` 也是 lvalue。
2. 取地址算符 `&`：
    * 后面只能跟 lvalue。
    * 对于 `& Ident`，直接返回 Ident 的地址，类型是以 Ident 类型为基类型的指针。
    * `&*expr` 等价于 `expr`，但是不再是 lvalue 了。
3. 解引用算符 `*`：
    * 后面只能跟指针类型，表示取出该指针指向的地址中的值，类型为指针的基类型。
    * 返回值是 lvalue。
    * 允许放在赋值号左边：`*p = 1`。
4. 指针运算：
    * 一元运算只支持 `&` 和 `*`。
    * 二元运算目前只支持 `==` 和 `!=`，要求两边都是相同基类型的指针类型。
    * 不支持大小比较、逻辑运算。
    * 这一步暂时不支持指针算术，因为没有数组。
5. 空指针：
    * 空指针指的是值为 0 的指针。
    * 空指针可以存在，对其解引用是未定义行为。
    * 只能用 `int *p = (int*) 0;` 定义。
    * 只能用类似 `if (p == (int*) 0) {}` 这样的判断是否为空。
6. 类型转换：
    * 没有隐式类型转换，所有赋值、运算、传参等都要求类型完全匹配。
    * 允许 int、不同类型的指针之间的任何显式转换。
7. 其他：
    * 所有指针类型的变量的大小都是 4 个字节（RV32）。
    * 如果某类型的指针不指向一个该类型的对象，而且它不是空指针，这就是一个未定义行为，哪怕不解引用。
    * 未对齐的指针是未定义行为。

例子：

```c
int a = 1;              // a == 1
*&a = 2;                // a == 2
(*(&(a))) = 3;          // a == 3

int *p = &a;
*p = 4;                 // a == 4

int **q = &p;
**q = 5;                // a == 5

q = &(&a);              // bad，&a 不是 lvalue
q = &(*p);              // bad，类型不对
p = &*p;                // ok

int *null = (int*) 0;   // 空指针
if (p == (int*) 0 || (int) p == 0) {
    // ...
} else if (p != null) {
    // ...
}
```

## 步骤12：Arrays

主要改动部分

```diff
 <program> ::= { <function> | <declaration> }
 <type> ::= "int" | <type> "*"
 <param-list> ::= [ <type> Identifier { "," <type> Identifier } ]
 <function> ::= <type> Identifier "(" <param-list> ")" ( "{" { <block-item> } "}" | ";" )
 <block-item> ::= <statement> | <declaration>
-<declaration> ::= <type> Identifier [ "=" <exp> ] ";"
+<declaration> ::= <type> Identifier { "[" Integer "]" } [ "=" <exp> ] ";"
 <statement> ::= "return" <exp> ";"
               | <exp-option-semicolon> // null statement
               | "if" "(" <exp> ")" <statement> [ "else" <statement> ]
               | "{" { <block-item> } "}
               | "for" "(" <exp-option-semicolon> <exp-option-semicolon> <exp-option-close-paren> ")" <statement>
               | "for" "(" <declaration> <exp-option-semicolon> <exp-option-close-paren> ")" <statement>
               | "while" "(" <exp> ")" <statement>
               | "do" <statement> "while" <exp> ";"
               | "break" ";"
               | "continue" ";"
 <exp-option-semicolon> ::= <exp> ";" | ";"
 <exp-option-close-paren> ::= <exp> ")" | ")"
-<exp> ::= <factor> "=" <exp> | <conditional-exp>
+<exp> ::= <unary> "=" <exp> | <conditional-exp>
 <conditional-exp> ::= <logical-or-exp> [ "?" <exp> ":" <conditional-exp> ]
 <logical-or-exp> ::= <logical-and-exp> { "||" <logical-and-exp> }
 <logical-and-exp> ::= <equality-exp> { "&&" <equality-exp> }
 <equality-exp> ::= <relational-exp> { ("!=" | "==") <relational-exp> }
 <relational-exp> ::= <additive-exp> { ("<" | ">" | "<=" | ">=") <additive-exp> }
 <additive-exp> ::= <term> { ("+" | "-") <term> }
-<term> ::= <factor> { ("*" | "/" | "%") <factor> }
-<factor> ::= <function-call> | "(" <exp> ")" | <unary_op> <factor> | "(" <type> ")" <factor> | Integer | Identifier
+<term> ::= <unary> { ("*" | "/" | "%") <unary> }
+<unary> ::= <postfix> | "(" <type> ")" <unary> | <unary_op> <unary>
+<postfix> ::= <primary> | <function-call> | <postfix> "[" expr "]"
+<primary> := Integer | Identifier | "(" <exp> ")"
 <function-call> ::= Identifier "(" [ <exp> { "," <exp> } ] ")"
 <unary_op> ::= "!" | "~" | "-" | "&" | "*"
```

一些说明：

1. 数组定义：
    * 类型是数组类型。
    * 每一维大小不能是零或负数。
    * 没有变长数组 `int a[n];` 也没有不定长数组 `int a[];`。
    * `int *a[5];` 是 `int*` 的数组，不支持数组的指针（`int (*a)[5];`），即不支持对数组类型取地址。
    * 数组初始化只能是默认初始化，没字面量也不能 `int a[2] = {2, 3};`。
    * 数组可以作为局部变量或全局变量，在内存中是连续的，其中全局变量会进行零初始化，局部变量的初始化是未定义的。
2. 下标运算：
    * 由于下标运算为后缀形式，且优先级比一元运算符高，所以文法中要加一个 `<postfix>`。
    * 只允许指针或数组类型进行下标运算，其中指针的每次下标运算等价于加上偏移后进行解引用，数组相当于把所有下标合起来算偏移，然后只进行一次解引用。
    * 下标必须是 int 类型，不进行下标越界检查，越界是未定义行为。
    * 下标运算的结果是 lvalue，即可以 `a[1] = 2; int *p = &a[1];`。
    * 允许将数组的前几维单独提出，赋给一个指针：`int a[2][2][2]; int *p = (int*) a[0]; int *q = (int*) a[1][1];`，提出后的类型还是数组类型，需要显式转换。
3. 类型检查与转换：
    * 只允许数组到任意指针类型的显式转换，不支持任何隐式转换。
    * 数组只能进行下标运算，不能参与任何其他运算，如取地址、解引用、赋值、算术、比较、逻辑等等。
    * 没有数组类型的参数，传参时只能显式转换为指针。
3. 指针算术运算：
    * 指针加法：允许指针加 int、以及 int 加指针，结果类型同指针类型。
        - int 和指针运算的时候，运算前要乘上指针类型的基类型的 sizeof。
        - 如果越界然后解引用或进行下标运算，是未定义行为。
        - 越界指的是数组越界，如果是其他类型视为长度为 1 的数组。
    * 指针减法：允许指针减 int、以及指针减指针。
        - 指针减 int 结果类型同指针类型，类似指针加法。
        - 指针 `a` 减指针 `b` 要求 `a` 和 `b` 是同类型的指针，结果是一个 int `c`，满足 `a == b + c`。同上，不对齐指针是未定义行为。
        - 指针 `a` 减指针 `b`，只有当 `a` 和 `b` 是指向同一个数组内元素（不越界，或者刚刚超过最后那个元素一个位置）时才有意义，否则是未定义行为。
    * 空指针参与的任何指针算术都是未定义行为。

例子：

```c
int a[2][2]; int b[3][3]; int c[2][2];
int *p; int *q;

int matmul2(int *a, int *b, int *c);        // ok
matmul2((int *)a, (int *)a, (int *)c);      // ok，必须显式转换

int *row2 = (int*)a[1];                     // ok
row2 = a[1];                                // bad，int[2] -> int* 转换必须显式

a = p;                                      // bad，数组不能赋值
a[1] = p;                                   // bad，数组不能赋值
a[1][1] = (int) p;                          // ok

int **ptr2level = a;                        // bad，类型不对
int x = a;                                  // bad，类型不对

int *p1 = a;            // bad，必须显式转换
p1 = (int*) a;          // ok

int *p2 = &a;           // bad，不能对数组取地址
p2 = &a[0];             // bad，不能对数组取地址
p2 = &a[0][0];          // ok

int **p3 = (int **)a;   // ok，这里三个 ok 的 p1, p2, p3 值应该相同，都是 a 的地址

p1[0] = p2[1] = 1;      // ok
p3[0][0] = 2;           // ok，但不同于 a[0][0]，这里进行了两次解引用，是未定义行为
```

## 步骤13 结构体
是否必须？工作量是否过大？



# feature文法
小 feature 如 ternary expr ; var 以外加上 val ; 允许 var 做类型推断。
