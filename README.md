# minidecaf

## 实验环境
本实验使用`C++ with Antlr`实现，因此你需要在根据实验手册安装交叉编译工具和模拟运行环境之后安装如下依赖：  
1. `Java`: 用于运行 Antlr 软件包 
2. `CMake`: 本项目使用 cmake 进行构建

## 项目构建

运行下述命令构建整个项目，首先你必须确保语法文件`*.g4`位于`generate/`路径，编译器代码文件位于`src/`路径下（由于第一次编译需要编译 antlr 库，可能会比较耗时）。

```shell
mkdir build && cd build
cmake ..
make
```

在开发过程中你可能想单独更新语法规则，我们在 cmake 中添加了单独的目标来执行这一操作，你只需要在`build`文件夹下运行`make update_antlr`即可。

## 运行

本项目会生成一个可执行文件`build/MiniDecaf`，运行方式为：

```shell
# path/to/MiniDecaf <filename>
# 如你想在 build 文件夹下运行编译器，编译 main.cpp 文件并输出汇编文件 main.S，你可以运行：
./MiniDecaf main.cpp > main.S
```

>  当然这一切是建立在你已经编译成功，生成了可执行文件的基础之上的。

## 测试

测例及脚本可见于[这里](https://github.com/decaf-lang/minidecaf-tests)。你需要按照参考书中的说明，将测试用例仓库克隆到指定位置，并确定测试脚本中的路径和环境等信息，然后运行 `check.sh`。

## 步骤

供你参考的代码在最后 12+ 个 commit（有些步骤之间跳跃性过强，我们做了些许拆分），你可以通过 `git log` 查看、`git checkout` 切换，或者直接在 GitHub 中查看。目前的版本还未针对失败测例进行调试。