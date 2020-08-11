
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
<function> ::= "int" <id> "(" ")" "{" <statement> "}"
<<statement> ::= "return" <int> ";" | "int" <id> "=" <int> ";"
<exp> ::= <int>
```

例子：
```
int main() {
    return 2;
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
Identifier [a-zA-Z]\w*
Integer literal [0-9]+
```
对应的AST为：
```
program = Program(function_declaration)
function_declaration = Function(string, statement) //string is the function name
statement = Return(exp) | Assign(variable, exp)
exp = Constant(int)
```

## 步骤2：Unary Operators
有"- ~ !"操作，语言语法为：

```
<program> ::= <function>
<function> ::= "int" <id> "(" ")" "{" <statement> "}"
<statement> ::= "return" <exp> ";"
<exp> ::= <unary_op> <exp> | <int>
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
Identifier [a-zA-Z]\w*
Integer literal [0-9]+
Negation -
Bitwise complement ~
Logical negation !
```

对应的AST为：
```
program = Program(function_declaration)
function_declaration = Function(string, statement) //string is the function name
statement = Return(exp)
exp = UnOp(operator, exp) | Constant(int)
```

## 步骤3：Binary Operators and  Parenthesis for ( expr )
支持
```
Addition +
Subtraction -
Multiplication *
Division /
```

语言语法为：

```
<program> ::= <function>
<function> ::= "int" <id> "(" ")" "{" <statement> "}"
<statement> ::= "return" <exp> ";"
<exp> ::= <term> { ("+" | "-") <term> }
<term> ::= <factor> { ("*" | "/") <factor> }
<factor> ::= "(" <exp> ")" | <unary_op> <factor> | <int>
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
Identifier [a-zA-Z]\w*
Integer literal [0-9]+
Minus -
Bitwise complement ~
Logical negation !
Addition +
Multiplication *
Division /
```

对应的AST为：
```
program = Program(function_declaration)
function_declaration = Function(string, statement) //string is the function name
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
<function> ::= "int" <id> "(" ")" "{" <statement> "}"
<statement> ::= "return" <exp> ";"
<exp> ::= <logical-and-exp> { "||" <logical-and-exp> }
<logical-and-exp> ::= <equality-exp> { "&&" <equality-exp> }
<equality-exp> ::= <relational-exp> { ("!=" | "==") <relational-exp> }
<relational-exp> ::= <additive-exp> { ("<" | ">" | "<=" | ">=") <additive-exp> }
<additive-exp> ::= <term> { ("+" | "-") <term> }
<term> ::= <factor> { ("*" | "/") <factor> }
<factor> ::= "(" <exp> ")" | <unary_op> <factor> | <int>
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
Identifier [a-zA-Z]\w*
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
```
对应的AST为：
```
program = Program(function_declaration)
function_declaration = Function(string, statement) //string is the function name
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
<function> ::= "int" <id> "(" ")" "{" { <statement> } "}"
<statement> ::= "return" <exp> ";"
              | <exp> ";"
              | "int" <id> [ = <exp>] ";"
<exp> ::= <id> "=" <exp> | <logical-or-exp>
<logical-or-exp> ::= <logical-and-exp> { "||" <logical-and-exp> }
<logical-and-exp> ::= <equality-exp> { "&&" <equality-exp> }
<equality-exp> ::= <relational-exp> { ("!=" | "==") <relational-exp> }
<relational-exp> ::= <additive-exp> { ("<" | ">" | "<=" | ">=") <additive-exp> }
<additive-exp> ::= <term> { ("+" | "-") <term> }
<term> ::= <factor> { ("*" | "/") <factor> }
<factor> ::= "(" <exp> ")" | <unary_op> <factor> | <int> | <id>
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
Identifier [a-zA-Z]\w*
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
<function> ::= "int" <id> "(" ")" "{" { <block-item> } "}"
<block-item> ::= <statement> | <declaration>
<declaration> ::= "int" <id> [ = <exp> ] ";"
<statement> ::= "return" <exp> ";"
              | <exp> ";"
              | "if" "(" <exp> ")" <statement> [ "else" <statement> ]


<exp> ::= <id> "=" <exp> | <conditional-exp>
<conditional-exp> ::= <logical-or-exp> [ "?" <exp> ":" <conditional-exp> ]
<logical-or-exp> ::= <logical-and-exp> { "||" <logical-and-exp> }
<logical-and-exp> ::= <equality-exp> { "&&" <equality-exp> }
<equality-exp> ::= <relational-exp> { ("!=" | "==") <relational-exp> }
<relational-exp> ::= <additive-exp> { ("<" | ">" | "<=" | ">=") <additive-exp> }
<additive-exp> ::= <term> { ("+" | "-") <term> }
<term> ::= <factor> { ("*" | "/") <factor> }
<factor> ::= "(" <exp> ")" | <unary_op> <factor> | <int> | <id>
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
Identifier [a-zA-Z]\w*
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

## 步骤7：Compound Statements
例如
```
int main() {
    int a;
    {
        //this is also a compound statement!
        a = 4;
    }
}
```
语言语法为：

```
<program> ::= <function>
<function> ::= "int" <id> "(" ")" "{" { <block-item> } "}"
<block-item> ::= <statement> | <declaration>
<declaration> ::= "int" <id> [ = <exp> ] ";"
<statement> ::= "return" <exp> ";"
              | <exp> ";"
              | "if" "(" <exp> ")" <statement> [ "else" <statement> ]
              | "{" { <block-item> } "}
<exp> ::= <id> "=" <exp> | <conditional-exp>
<conditional-exp> ::= <logical-or-exp> [ "?" <exp> ":" <conditional-exp> ]
<logical-or-exp> ::= <logical-and-exp> { "||" <logical-and-exp> }
<logical-and-exp> ::= <equality-exp> { "&&" <equality-exp> }
<equality-exp> ::= <relational-exp> { ("!=" | "==") <relational-exp> }
<relational-exp> ::= <additive-exp> { ("<" | ">" | "<=" | ">=") <additive-exp> }
<additive-exp> ::= <term> { ("+" | "-") <term> }
<term> ::= <factor> { ("*" | "/") <factor> }
<factor> ::= "(" <exp> ")" | <unary_op> <factor> | <int> | <id>
<unary_op> ::= "!" | "~" | "-"
```
对应的Token为（不变）：
```
Open brace {
Close brace }
Open parenthesis (
Close parenthesis )
Semicolon ;
Int keyword int
Return keyword return
Identifier [a-zA-Z]\w*
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
语言语法为：(注意语法和AST定义之间存在差异;语法允许for循环中的控制表达式为空，但AST不允许。)

```
<program> ::= <function>
<function> ::= "int" <id> "(" ")" "{" { <block-item> } "}"
<block-item> ::= <statement> | <declaration>
<declaration> ::= "int" <id> [ = <exp> ] ";"
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
<exp> ::= <id> "=" <exp> | <conditional-exp>
<conditional-exp> ::= <logical-or-exp> [ "?" <exp> ":" <conditional-exp> ]
<logical-or-exp> ::= <logical-and-exp> { "||" <logical-and-exp> }
<logical-and-exp> ::= <equality-exp> { "&&" <equality-exp> }
<equality-exp> ::= <relational-exp> { ("!=" | "==") <relational-exp> }
<relational-exp> ::= <additive-exp> { ("<" | ">" | "<=" | ">=") <additive-exp> }
<additive-exp> ::= <term> { ("+" | "-") <term> }
<term> ::= <factor> { ("*" | "/") <factor> }
<factor> ::= "(" <exp> ")" | <unary_op> <factor> | <int> | <id>
<unary_op> ::= "!" | "~" | "-"
```
对应的Token为：
```
{
}
(
)
;
int
return
Identifier [a-zA-Z]\w*
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
          | For(exp option, exp, exp option, statement) // initial expression, condition, post-expression, body
          | ForDecl(declaration, exp, exp option, statement) // initial declaration, condition, post-expression, body
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
<function> ::= "int" <id> "(" [ "int" <id> { "," "int" <id> } ] ")" ( "{" { <block-item> } "}" | ";" )
<block-item> ::= <statement> | <declaration>
<declaration> ::= "int" <id> [ = <exp> ] ";"
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
<exp> ::= <id> "=" <exp> | <conditional-exp>
<conditional-exp> ::= <logical-or-exp> [ "?" <exp> ":" <conditional-exp> ]
<logical-or-exp> ::= <logical-and-exp> { "||" <logical-and-exp> }
<logical-and-exp> ::= <equality-exp> { "&&" <equality-exp> }
<equality-exp> ::= <relational-exp> { ("!=" | "==") <relational-exp> }
<relational-exp> ::= <additive-exp> { ("<" | ">" | "<=" | ">=") <additive-exp> }
<additive-exp> ::= <term> { ("+" | "-") <term> }
<term> ::= <factor> { ("*" | "/") <factor> }
<factor> ::= <function-call> | "(" <exp> ")" | <unary_op> <factor> | <int> | <id>
<function-call> ::= id "(" [ <exp> { "," <exp> } ] ")"
<unary_op> ::= "!" | "~" | "-"
```
对应的Token为：
```
{
}
(
)
;
int
return
Identifier [a-zA-Z]\w*
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
          | For(exp option, exp, exp option, statement) // initial expression, condition, post-expression, body
          | ForDecl(declaration, exp, exp option, statement) // initial declaration, condition, post-expression, body
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
<function> ::= "int" <id> "(" [ "int" <id> { "," "int" <id> } ] ")" ( "{" { <block-item> } "}" | ";" )
<block-item> ::= <statement> | <declaration>
<declaration> ::= "int" <id> [ = <exp> ] ";"
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
<exp> ::= <id> "=" <exp> | <conditional-exp>
<conditional-exp> ::= <logical-or-exp> [ "?" <exp> ":" <conditional-exp> ]
<logical-or-exp> ::= <logical-and-exp> { "||" <logical-and-exp> }
<logical-and-exp> ::= <equality-exp> { "&&" <equality-exp> }
<equality-exp> ::= <relational-exp> { ("!=" | "==") <relational-exp> }
<relational-exp> ::= <additive-exp> { ("<" | ">" | "<=" | ">=") <additive-exp> }
<additive-exp> ::= <term> { ("+" | "-") <term> }
<term> ::= <factor> { ("*" | "/") <factor> }
<factor> ::= <function-call> | "(" <exp> ")" | <unary_op> <factor> | <int> | <id>
<function-call> ::= id "(" [ <exp> { "," <exp> } ] ")"
<unary_op> ::= "!" | "~" | "-"
```

对应的Token为(不变)：
```
{
}
(
)
;
int
return
Identifier [a-zA-Z]\w*
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
          | For(exp option, exp, exp option, statement) // initial expression, condition, post-expression, body
          | ForDecl(declaration, exp, exp option, statement) // initial declaration, condition, post-expression, body
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

## 步骤11 (WIP) 指针

指针和数组是否要做？

请完善

主要改动部分

```
func
    : ty Ident '(' paramList ')'  (block | ';')     // 参数本质就是 decl
    ;

ty
    : 'int'
    | ty '*'
    ;

decl
    : ty Ident ('=' expr)?      // 去掉了分号，之前用到的地方记得加分号
    ;

paramList
    : (decl (',' decl)*)?       // 逗号隔开的 decl，并且语义检查 decl 不能有初始值
    ;

factor
    : ...
    | '&' factor                // 要做类型和 lvalue 检查，不允许 &(2+3) 这种
                                // lvalue : Ident | '*' expr
    | '*' factor                // 要做类型检查
    ;
```

其中取地址算符 `&` 要求
* 如果 `& Ident` 那直接返回 Ident 地址
* `&*expr` 等价于 `expr`，（但是不再是 lvalue 了）

## 步骤12 (WIP) 数组

请完善

主要改动部分

```
decl
    : ty Ident  ('[' Integer ']')*  ('=' expr)?     // 语义检查：数组 decl 不能给初始值
    ;

factor
    : ...
    | '&' factor                // lvalue : Ident | '*' expr | expr '[' expr ']'
    | expr '[' expr ']'         // 要做类型检查，要求第一个 expr 类型是 ty* 或者数组
                                // a: int[2][3]，那么 a[1]: int[3]，a[1][2]: int
                                // b: int*[2], 那么 b[1]: int*, b[1][3]: int
```

并且：
* 没有变长数组 `int a[n];` 也没有不定长数组 `int a[];`
  - 否则预留空间和计算偏移量更麻烦——可以作为较复杂的小练习
* 没有越界检查，有负长度检查
* 指针不能指向数组，不存在 `int (*)[2]` 这种东西
  - 否则声明会很麻烦，如 `int (*a)[2]` 和 `int *a[2]`
* 数组初始化只能是默认零初始化，没字面量也不能 `int a[2] = {2, 3}`
  - 否则麻烦
* 数组可以在栈上或者 .bss（全局变量）里
* 如果数组作为函数参数出现，不预留空间，否则预留空间并且零初始化

关于数组和指针的隐式转换
* 只有一个地方允许指针和数组之间的隐式转换：传参，其他地方只能有数组到指针的显式类型转换
* 如果形参类型是数组（如 `int* param[2][3]`），那么实参必须是同样的数组（`int *arg[2][3]`），或者同样基类型的指针（`int **arg`）
* 如果形参类型是指针（如 `int *param`），那么实参必须是同样的指针（`int *arg`），或者同样基类型的数组（`int arg[2]` 或 `int arg[3][10]`）

例子：
```
int a[2][2]; int b[3][3]; int c[2][2];
int *p; int *q;

void matmul2(int a[2][2], int b[2][2], int c[2][2]); // ok
matmul2(a, a, c);                                    // ok
matmul2(a, b, c);                                    // bad 长度不匹配
p = (int*) b; q = (int*) c; matmul2(a, p, q);        // ok，有两个 int* -> int[2][2] 的隐式转换

int *row2 = a[1];                                    // ok
row2 = a[1];                                         // bad，这里 int[2] -> int* 转换必须显式

a = p;                                               // bad，数组不能赋值
a[1] = p;                                            // bad，数组不能赋值
a[1][1] = (int) p;                                   // ok

int **ptr2level = a;                                 // bad，类型不对
int x = a;                                           // bad，类型不对

int *p1 = a;     // ok
int *p2 = &a;    // ok
p1 = (int*) a;   // ok
p1 = (int*) &a;  // ok，这四个取到的值是一样的
p1 = (int*) &&a; // bad，不是 lvalue
p1 = a;          // bad，必须显式转换
p1 = &a;         // bad，必须显式转换
```

## 步骤13 结构体
是否必须？工作量是否过大？



# feature文法
小 feature 如 ternary expr ; var 以外加上 val ; 允许 var 做类型推断。
