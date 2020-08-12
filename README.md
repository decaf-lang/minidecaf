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
    直接在 CST 上面做的……
  - name & type（名字解析 / 类型检查）
  - irgen（生成栈 ir）
  - asmgen（从栈 ir 生成汇编）

是否必要？？
