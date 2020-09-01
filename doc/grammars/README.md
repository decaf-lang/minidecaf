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
用法：
```
./makehtml 10
```

然后就会生成 10.html
