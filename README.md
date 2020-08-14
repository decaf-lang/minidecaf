# minidecaf (antlr-py)
依赖
* antlr-4.8
* python >= 3.5
* minidecaf/requirements.txt

用法
```
# 运行编译，生成源代码（默认 i.c）对应的汇编（默认 o.s）
make [i=i.c] [o=o.s]

# 显示源代码的具体语法树
make cst [i=i.c]
```

大致结构
* 多 pass
  - lex/parse: antlr
    以下直接在 CST 上面做的…… 是否需要生成 AST？
  - name & type（名字解析 / 类型检查）
  - irgen（生成栈 ir）
  - asmgen（从栈 ir 生成汇编）

## name & type
输出是一个 dict: ident ctx -> var(identStr, uniqueID)。

本质是一个 alpha conversion 做了 renaming。

这一部分计算 var 的 frameslot。

## irgen
不需要考虑类型，没啥可说的. 栈 IR 和 mashplant 的很像。

## asmgen
暴力展开，注意处理 prologue 和 epilogue。
