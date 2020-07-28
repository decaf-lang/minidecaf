
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
expr= term ("+" term | "-" term)*
term = num
num = digit+
digit  =  [0-9]
```
## 步骤3：递归下降语法解析
```
factor	 = digit {addop digit}
digit  =  [0-9]
addop    = "+" | "-"
```
## 步骤4：有一定的错误提示
```
expr= term ("+" term | "-" term)*
term = num
num = digit+
digit  =  [0-9]
```
## 步骤5：stack computer on RV64
```
expr= term ("+" term | "-" term)*
term = num
num = digit+
digit  =  [0-9]
```
## 步骤6：可执行乘除
```
expr= term ("+" term | "-" term)*
term = factor ("*" factor | "/" factor)*
factor	 = num
num = digit+
digit  =  [0-9]
```
## 步骤7：支持括号操作
```
expr= term ("+" term | "-" term)*
term = factor ("*" factor | "/" factor)*
factor	 = num | "(" expr ")"
num = digit+
digit  =  [0-9]
```
## 步骤8：支持一元操作符

```
expr= term ("+" term | "-" term)*
term = factor ("*" factor | "/" factor)*
factor	 = unary? (num | "(" expr ")")
num = digit+
digit  =  [0-9]
unary = "+" | "-"
```

## 步骤9：支持比较操作

```
expr = expr "==" expr | expr "!=" expr | expr "<" expr | expr "<=" expr | expr ">" expr | expr ">=" expr | term ("+" term | "-" term)*
term = factor ("*" factor | "/" factor)*
factor	 = unary? (num | "(" expr ")")
num = digit+
digit  =  [0-9]
unary = "+" | "-"
```
## 步骤10：支持变量与赋值语句
```
stmt  =  (var "=" expr  ";")+ 
expr = expr "==" expr | expr "!=" expr | expr "<" expr | expr "<=" expr | expr ">" expr | expr ">=" expr | term ("+" term | "-" term)*
term = factor ("*" factor | "/" factor)*
factor	 = unary? (num | "(" expr ")")
num = digit+
digit  =  [0-9]
unary = "+" | "-"
var= ident "=" expr
ident = [a-z]+[a-z0-9_]*
```

## 步骤11：支持返回语句
```
stmt  = ("return" expr ";" | var "=" expr  ";")+ 
expr = expr "==" expr | expr "!=" expr | expr "<" expr | expr "<=" expr | expr ">" expr | expr ">=" expr | term ("+" term | "-" term)*
term = factor ("*" factor | "/" factor)*
factor	 = unary? (num | "(" expr ")")
num = digit+
digit  =  [0-9]
unary = "+" | "-"
var= ident "=" expr
ident = [a-z]+[a-z0-9_]*
```

## 步骤12：支持条件语句
```
stmt  = ("return" expr ";" | var "=" expr  ";" | "if" "(" expr ")" stmt ("else" stmt)? ";" | "{" stmt* "}" ";")+
expr = expr "==" expr | expr "!=" expr | expr "<" expr | expr "<=" expr | expr ">" expr | expr ">=" expr | term ("+" term | "-" term)*
term = factor ("*" factor | "/" factor)*
factor	 = unary? (num | "(" expr ")")
num = digit+
digit  =  [0-9]
unary = "+" | "-"
var= ident "=" expr
ident = [a-z]+[a-z0-9_]*
```

## 步骤13：支持函数
```
function   = ident "(" ")" "{" stmt "}"
stmt  = ("return" expr ";" | var "=" expr  ";" | "if" "(" expr ")" stmt ("else" stmt)? ";" | "{" stmt* "}" ";")+
expr = expr "==" expr | expr "!=" expr | expr "<" expr | expr "<=" expr | expr ">" expr | expr ">=" expr | term ("+" term | "-" term)*
term = factor ("*" factor | "/" factor)*
factor	 = unary? (num | "(" expr ")")
num = digit+
digit  =  [0-9]
unary = "+" | "-"
var= ident "=" expr
ident = [a-z]+[a-z0-9_]*
```

## 步骤14：支持字符串操作
缺 int, char* 定义和相关操作

## 步骤15：支持数组操作
缺 array的定义和相关操作

> 备注 对于第14、15步，需要实现：

1. 一元`&`：返回变量地址和一元`*`：返回地址指向的值
1. 删除隐式变量定义并引入关键字int
1. 实现指针加法和减法
1. sizeof运算符
1. 实现数组
1. 实现数组下标
1. 全局变量
1. 字符类型
1. 字符串字面量

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

## feature文法
小 feature 如 ternary expr ; var 以外加上 val ; 允许 var 做类型推断。
