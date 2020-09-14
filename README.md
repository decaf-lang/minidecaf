# MiniDecaf (Antlr + Java)

MiniDecaf 是清华大学 2020 年秋季学期编译原理课程所用的教学语言，它是在 C 的一小部分语言特性上修改而来。

## 下载

```bash
git clone git@github.com:decaf-lang/minidecaf.git -b md-xxy
cd minidecaf/
```

如果此框架有所更新，你可以通过以下方式更新本地的框架

```bash
git fetch origin md-xxy
git reset --hard origin/md-xxy
```

## 环境依赖

- OS: *nix（Mac 或者 Linux）
- JDK 14
- [RISCV 交叉编译器和模拟器](https://decaf-lang.github.io/minidecaf-tutorial-deploy/docs/lab0/env.html)

> 注：本框架使用 Gradle 的 ANTLR 插件，你可以无需单独下载 ANTLR。

## 编译

你可以使用一款你喜欢的 IDE 来编译和调试本项目，或者可以直接使用命令行。

本项目的编译方式是 gradle 的 build 任务。

> 我们的 parser 代码由 [ANTLR](https://www.antlr.org/) 自动生成（synthesis），为了让 IDE 能够解析到其中的符号，在第一次开发本项目以及后续更改文法时及时 build 更新 parser 代码。

### 命令行

```bash
./gradlew build
```

jar 包会生成于 `build/libs/minidecaf.jar`。

你也可以通过以下方式单独生成 parser，

```bash
./gradlew generateGrammarSource
```

生成的 parser 代码位于 `build/generated-src/antlr/main/minidecaf/*`。

### IntelliJ IDEA

1. 可以参考 [这里](https://www.jetbrains.com/help/idea/gradle.html#gradle_import_project_start) 载入本项目；
2. 可以通过 `View > Tool Windows > Gradle` 打开 Gradle 窗口；
3. 可以参考 [这里](https://www.jetbrains.com/help/idea/work-with-gradle-tasks.html#gradle_tasks) 来运行 Gradle 的 `build > build` 任务，从而编译本项目，生成所需的 parser 代码（`build/generated-src/antlr/main/minidecaf/*`）和 jar 包（在`build/libs/minidecaf.jar`）。

### Eclipse

1. 可以通过 `File > Import > Gradle > Existing Gradle Project` 载入本项目；
2. 可以运行 gradle 任务 `ide > eclipse`，以生成 Eclipse 所需的配置文件；
3. 可以运行 gradle 任务 `build > build`，以生成 parser 代码；
4. 可以通过 `File > Refresh` 来刷新项目。

> 如果报错`Unbound classpath container: 'JRE System Library [...]' in project 'minidecaf'`，可以参考[这里](https://stackoverflow.com/questions/19696613/getting-jre-system-library-unbound-error-in-build-path)。但也有可能不是 JRE System Library 的问题，只需把它删掉重新添加即可。

> 如果你看到注释是乱码，可以参考[这里](https://stackoverflow.com/questions/9180981/how-to-support-utf-8-encoding-in-eclipse)把编码方式改为 UTF-8。

## 运行

```bash
java -ea -jar build/libs/minidecaf.jar <input minidecaf file> <output riscv assembly file>
```

## 测试

测例及脚本可见于[这里](https://github.com/decaf-lang/minidecaf-tests)。

## 步骤

供你参考的代码在最后 12 个 commit，你可以通过 `git log` 查看、`git checkout` 切换，或者直接在 GitHub 中查看。
