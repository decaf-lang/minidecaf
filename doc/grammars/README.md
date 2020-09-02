# 改动
从 step1 开始就有 type，不再要求 main（而是放到语义检查）

提前放空语句，step5 开始是
    stmt : expr? ';'
而不是
    stmt : expr ';'
了

引入了一些非中介结符，如 compound_statement, parameter_list



# gspec
和 txt 一样，但是用 BEGINCHANGED 和 ENDCHANGED 标识改动位置

语法高亮是 gspec.vim

# makehtml
需要你安装 `gspec.vim` 语法到 vim，并且保证你 vim 的 term 是 256 色的（例如 tmux 里 `export TERM=screen-256color`）。

用法：
```
./makehtml 10
```

然后就会生成 10.html

（为什么这么麻烦：因为单纯的语法高亮似乎不太好设置某些背景要灰色。）
