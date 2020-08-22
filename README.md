# MiniDecaf Python-ANTLR 参考实现

------------------------------------------------------------------------------

# 环境配置
* OS: *nix（Mac 或 Linux）

* make: 因为有 Makefile

* Python 至少 3.6，运行 `python --version` 检查
  - ubuntu 下安装命令是 `sudo apt install python3`

* pip 版本随意，但须是 python3 的。运行 `pip --version`，最后应该是 python 3.x。
  - ubuntu 下安装命令是 `sudo apt install python3-pip`

* ANTLR 工具：按照 https://www.antlr.org/ 的 Quick Start 安装即可

* Python 的 ANTLR API：
  - `pip install antlr4-python3-runtime`
  - 或者 `pip install -r minidecaf/requirements.txt`


# 用法
```
# 运行编译，生成源代码（默认 i.c）对应的汇编（默认 o.s）
make [i=i.c] [o=o.s]

# 显示源代码的具体语法树
make cst [i=i.c]
```

# 代码结构
多遍编译器，支持输出中间结果。pass 有：

* lex/parse：直接使用 ANTLR
  - 没有构建抽象语法树，直接在 ANTLR 的具体语法树上做接下来的处理
  - `make lex`，`make parse` 以及 `make cst` 来看 lex/parse 结果

* namer：名称解析，解析完成以后做一遍变量重命名（alpha conversion）
  - `make ni` 看 namer 结果

* typer：类型检查
  - `make ti` 看 typer 结果

* irgen：生成栈式机 IR
  - `make ir` 看生成的 IR

* asmgen：从 IR 生成汇编
  - `make asm` 然后看 `o.s`（或者 `make o=OUTFILE` 的输出文件）
