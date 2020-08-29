# MiniDecaf Rust-lalr1 参考实现

本实验框架的词法 & 语法解析部分基于我编写的parser generator [lalr1](https://github.com/MashPlant/lalr1)自动生成，你可以阅读这份[文档](https://mashplant.online/2020/08/17/lalr1-introduction)来学习lalr1的用法。

# 环境配置

仅需要配置nightly版本的Rust工具链。没错，lalr1是不需要额外的安装步骤的，读了上面的文档你就能够理解了

安装Rust可以参考[https://www.rust-lang.org/tools/install](https://www.rust-lang.org/tools/install)，安装完后进一步安装nightly版本的Rust的指令是：

```bash
$ rustup toolchain install nightly
$ rustup default nightly
```

具体Rust版本没有要求，我使用的版本是`1.46.0-nightly`，理论上与它相同或者比它更新都应该可以。

你可以选择你喜欢的编辑器或IDE来编写Rust代码，如VSCode + Rust插件等。我个人使用的是CLion + Rust插件，并且强烈推荐大家使用。使用什么编辑器或IDE与本实验没有任何关系，只是关系到编写和调试代码时的体验。

# 用法

编译生成汇编：

```bash
$ cargo run <输入的minidecaf文件路径>
```

这样会把汇编直接打印在标准输出上，如果要输出到文件中，可以使用重定向等方式。

不支持更多的功能了，例如输出AST，中间代码等。如果你想调试这些中间结果，可以自己修改代码，在合适的地方加上输出。

# 代码结构

- `ast.rs`: 定义AST中的各个节点
- `codegen.rs`: 将IR翻译为RISC-V汇编
- `ir.rs`: 定义IR，将AST翻译为IR
- `parser.rs`: 基于lalr1定义lexer和parser