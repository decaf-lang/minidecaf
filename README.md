# MiniDecaf Python-ANTLR 参考实现

------------------------------------------------------------------------------

# 环境配置
* OS: *nix（Mac 或 Linux）

* make: 因为有 Makefile

* Python 至少 3.6，运行 `python3 --version` 检查
  - ubuntu 下安装命令是 `sudo apt install python3`

* pip 版本随意，但须是 python3 的。运行 `pip --version`，最后应该是 python 3.x。
  - ubuntu 下安装命令是 `sudo apt install python3-pip`

* Java 至少 11：用来运行 ANTLR 工具
  - ubuntu 下安装命令是 `sudo apt install default-jdk`

* ANTLR 工具：按照 https://www.antlr.org/ 的 Quick Start 安装即可
  - 因为网络原因可能 Quick Start 那块加载不出来，那么请看[实验指导书](https://decaf-lang.github.io/minidecaf-tutorial/docs/lab1/part2.html)的截图。
  - 如果你没有完全按照官网指导把 antlr 的 jar 放到系统目录，你还需要在 `~/.bashrc`（或 `~/.zshrc`）中 `export ANTLR_JAR="/path/to/your/antlr.jar"`，注意加上双引号。

* Python 的 ANTLR API：
  - `pip3 install antlr4-python3-runtime`
  - 或者 `pip3 install -r minidecaf/requirements.txt`

# 用法
```
# 运行编译，生成源代码（默认 i.c）对应的汇编（默认 o.s）
make [i=i.c] [o=o.s]

# 显示源代码的具体语法树
make cst [i=i.c]
```

# 常见问题
## 运行测例 minidecaf-tests 不通过
1. 运行测例之前，先在本项目下 `make grammar-py`

## 没找到 python3
```
make: python3: Command not found
```

先确认你安装了 python 3，至少 3.6：
```
$ python -V
Python 3.8.2
```

然后如果你没有 python3，可以手动建立一个软连接
```
$ sudo ln -s /usr/bin/python /usr/bin/python3
$ make
cd minidecaf && java -jar /usr/local/lib/antlr-4.8-complete.jar -Dlanguage=Python3 -visitor -o generated MiniDecaf.g4
python3 -m minidecaf  i.c o.s
riscv64-unknown-elf-gcc  -march=rv32im -mabi=ilp32 o.s
qemu-riscv32 a.out ; echo $?
233
```

## 没找到 pip3
首先确认你安装了 python3 的 pip，检查：
```
$ pip -V
pip 20.2.3 from /path/to/pip (python 3.8)
```

然后用 pip 安装依赖即可 `pip install antlr4-python3-runtime`。

## No such file or directory: 'i.c'
你得自己建立 `i.c` 文件，内容是你的输入 MiniDecaf 程序。

## syntax error: '..' came as a complete surprise to me
使用 WSL 或虚拟机的同学，请在 Linux 环境下执行 git clone 和 make。

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

