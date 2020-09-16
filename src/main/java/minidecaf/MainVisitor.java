package minidecaf;

import minidecaf.MiniDecafParser.*;
import minidecaf.Type.*;

import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.tree.*;

import java.util.*;

/**
 * 这里实现了编译器的主要逻辑。为了简单，本框架仅通过在分析树上单次遍历来生成目标汇编代码。
 *
 * 遍历时会为每一个分析树节点计算一个类型（Type），对于没有类型的节点用 NoType 留空。
 * 这样做的目的在前 10 个 step 并不会体现出来，是为了方便 step 11 和 12 所需的对于表达式的类型推导。
 *
 * @author   Namasikanam
 * @since    2020-09-11
 */
public final class MainVisitor extends MiniDecafBaseVisitor<Type> {
    private StringBuilder sb; // 生成的目标汇编代码
    MainVisitor(StringBuilder sb) {
        this.sb = sb;
    }

    @Override
    public Type visitProg(ProgContext ctx) {
        visit(ctx.func());

        if (!containsMain) reportError("no main function", ctx);

        return new NoType();
    }
    
    @Override
    public Type visitFunc(FuncContext ctx) {
        currentFunc = ctx.IDENT().getText();
        if (currentFunc.equals("main")) containsMain = true;

        sb.append("\t.text\n") // 表示以下内容在 text 段中
          .append("\t.global " + currentFunc + "\n") // 让该 label 对链接器可见
          .append(currentFunc + ":\n");
        
        sb.append("# prologue\n");
        push("ra");
        push("fp");
        sb.append("\tmv fp, sp\n");
        int backtracePos = sb.length();
        localCount = 0;

        for (var blockItem: ctx.blockItem())
            visit(blockItem);
        
        // 为了实现方便，我们默认 return 0
        sb.append("# return 0 as default\n")
          .append("\tli t1, 0\n")
          .append("\taddi sp, sp, -4\n")
          .append("\tsw t1, 0(sp)\n");
        
        // 根据局部变量的数量，回填所需的栈空间
        sb.insert(backtracePos, "\taddi sp, sp, " + (-4 * localCount) + "\n");

        sb.append("# epilogue\n")
          .append(".exit." + currentFunc + ":\n")
          .append("\tlw a0, 0(sp)\n")
          .append("\tmv sp, fp\n");
        pop("fp");
        pop("ra");
        sb.append("\tret\n\n");

        return new NoType();
    }

    @Override
    public Type visitLocalDecl(LocalDeclContext ctx) {
        String name = ctx.IDENT().getText();
        if (symbolTable.get(name) != null)
            reportError("try declaring a declared variable", ctx);
        
        // 加入符号表
        symbolTable.put(name,
            new Symbol(name, -4 * ++localCount, new IntType()));
        
        // 如果有初始化表达式的话，求出初始化表达式的值
        var expr = ctx.expr();
        if (expr != null) {
            visit(expr);
            pop("t0");
            sb.append("\tsw t0, " + (-4 * localCount) + "(fp)\n");
        }
        
        return new NoType();
    }

    @Override
    public Type visitExprStmt(ExprStmtContext ctx) {
        var expr = ctx.expr();
        if (expr != null) {
            visit(ctx.expr());
            pop("t0"); // 这个表达式不再会被使用，需要将其从栈里弹出
        }
        return new NoType();
    }

    @Override
    public Type visitReturnStmt(ReturnStmtContext ctx) {
        visit(ctx.expr());
        // 函数返回，返回值存在 a0 中
        sb.append("\tj .exit." + currentFunc + "\n");
        return new NoType();
    }

    @Override
    public Type visitIfStmt(IfStmtContext ctx) {
        int currentCondNo = condNo++;
        sb.append("# # if\n"); // "#if" 是宏，所以这里需要多给一个"#"

        // 根据条件表达式的值判断是否要直接跳转至 else 分支
        visit(ctx.expr());
        pop("t0");
        sb.append("\tbeqz t0, .else" + currentCondNo + "\n");

        visit(ctx.stmt(0));
        sb.append("\tj .afterCond" + currentCondNo + "\n") // 在 then 分支结束后直接跳至分支语句末尾
          .append(".else" + currentCondNo + ":\n"); // 标记 else 分支开始部分的 label
        if (ctx.stmt().size() > 1)
            visit(ctx.stmt(1));
        sb.append(".afterCond" + currentCondNo + ":\n");
        return new NoType();
    }

    @Override
    public Type visitExpr(ExprContext ctx) {
        if (ctx.children.size() > 1) {
            String name = ctx.IDENT().getText();
            Optional<Symbol> optionSymbol = lookupSymbol(name);
            if (!optionSymbol.isEmpty()) {
                visit(ctx.expr());
                Symbol symbol = optionSymbol.get();

                pop("t0");
                sb.append("# read variable\n")
                  .append("\tsw t0, " + symbol.offset + "(fp)\n");
                push("t0");
                return symbol.type;
            } else {
                reportError("use variable that is not defined", ctx);
                return new NoType();
            }
        } else return visit(ctx.ternary());
    }

    @Override
    public Type visitTernary(TernaryContext ctx) {
        if (ctx.children.size() > 1) {
            int currentCondNo = condNo++;
            sb.append("# ternary conditional\n");
            visit(ctx.lor());

            // 根据条件表达式判断是否要跳转至 else 分支
            pop("t0");
            sb.append("\tbeqz t0, .else" + currentCondNo + "\n");
            visit(ctx.expr());
            sb.append("\tj .afterCond" + currentCondNo + "\n") // 在 then 分支结束后直接跳至分支语句末尾
              .append(".else" + currentCondNo + ":\n"); // 在 else 分支结束后直接跳至分支语句末尾
            visit(ctx.ternary());
            sb.append(".afterCond" + currentCondNo + ":\n");

            return new IntType();
        } else return visit(ctx.lor());
    }

    @Override
    public Type visitLor(LorContext ctx) {
        if (ctx.children.size() > 1) {
            visit(ctx.lor(0));
            visit(ctx.lor(1));
            
            pop("t1");
            pop("t0");
            sb.append("\tsnez t0, t0\n")
              .append("\tsnez t1, t1\n")
              .append("\tor t0, t0, t1\n");
            push("t0");
            
            return new IntType();
        } else {
            return visit(ctx.land());
        }
    }

    @Override
    public Type visitLand(LandContext ctx) {
        if (ctx.children.size() > 1) {
            visit(ctx.land(0));
            visit(ctx.land(1));

            pop("t1");
            pop("t0");
            sb.append("\tsnez t0, t0\n")
              .append("\tsnez t1, t1\n")
              .append("\tand t0, t0, t1\n");
            push("t0");
            
            return new IntType();
        } else {
            return visit(ctx.equ());
        }
    }

    @Override
    public Type visitEqu(EquContext ctx) {
        if (ctx.children.size() > 1) {
            visit(ctx.equ(0));
            visit(ctx.equ(1));
            
            pop("t1");
            pop("t0");
            String op = ctx.children.get(1).getText();
            if (op.equals("==")) {
                sb.append("# eq\n")
                  .append("\tsub t0, t0, t1\n")
                  .append("\tseqz t0, t0\n");
                push("t0");
            } else {
                assert op.equals("!=");
                sb.append("# ne\n")
                  .append("\tsub t0, t0, t1\n")
                  .append("\tsnez t0, t0\n");
                push("t0");
            }

            return new IntType();
        } else {
            return visit(ctx.rel());
        }
    }

    @Override
    public Type visitRel(RelContext ctx) {
        if (ctx.children.size() > 1) {
            visit(ctx.rel(0));
            visit(ctx.rel(1));

            pop("t1");
            pop("t0");
            String op = ctx.children.get(1).getText();
            if (op.equals("<")) {
                sb.append("# <\n")
                  .append("\tslt t0, t0, t1\n");
            } else if (op.equals("<=")) {
                sb.append("# <=\n")
                  .append("\tsgt t0, t0, t1\n")
                  .append("\txori t0, t0, 1\n");
            } else if (op.equals(">")) {
                sb.append("# >\n")
                  .append("\tsgt t0, t0, t1\n");
            } else {
                assert op.equals(">=");
                sb.append("# >=\n")
                  .append("\tslt t0, t0, t1\n")
                  .append("\txori t0, t0, 1\n");
            }
            push("t0");

            return new IntType();
        } else {
            return visit(ctx.add());
        }
    }

    @Override
    public Type visitAdd(AddContext ctx) {
        if (ctx.children.size() > 1) {
            visit(ctx.add(0));
            visit(ctx.add(1));
            pop("t1");
            pop("t0");
            if (ctx.children.get(1).getText().equals("+")) {
                sb.append("# int + int\n")
                  .append("\tadd t0, t0, t1\n");
            } else {
                sb.append("# int - int\n")
                  .append("\tsub t0, t0, t1\n");
            }
            push("t0");
            return new IntType();
        } else return visit(ctx.mul());
    }

    @Override
    public Type visitMul(MulContext ctx) {
        if (ctx.children.size() > 1) {
            visit(ctx.mul(0));
            visit(ctx.mul(1));
            String op = ctx.children.get(1).getText();
            op = op.equals("*") ? "mul" : op.equals("/") ? "div" : "rem";
            pop("t1");
            pop("t0");
            sb.append("# " + op + "\n")
              .append("\t" + op + " t0, t0, t1\n");
            push("t0");
            return new IntType();
        } else return visit(ctx.unary());
    }

    @Override
    public Type visitUnary(UnaryContext ctx) {
        if (ctx.children.size() > 1) {
            visit(ctx.unary());
            String op = ctx.children.get(0).getText();
            
            sb.append("# " + op + " int\n");
            pop("t0");
            if (op.equals("-")) {
                sb.append("\tneg t0, t0\n");
            } else if (op.equals("!")) {
                sb.append("\tseqz t0, t0\n");
            } else {
                assert(op.equals("~"));
                sb.append("\tnot t0, t0\n");
            }
            push("t0");
            return new IntType();
        } else return visit(ctx.primary());
    }

    @Override
    public Type visitNumPrimary(NumPrimaryContext ctx) {
        TerminalNode num = ctx.NUM();

        // 数字字面量不能超过整型的最大值
        if (compare(Integer.toString(Integer.MAX_VALUE), num.getText()) == -1)
            reportError("too large number", ctx);

        sb.append("# number " + num.getText() + "\n")
          .append("\tli t0, " + num.getText() + "\n");
        push("t0");
        return new IntType();
    }

    @Override
    public Type visitIdentPrimary(IdentPrimaryContext ctx) {
        String name = ctx.IDENT().getText();
        Optional<Symbol> optionSymbol = lookupSymbol(name);
        if (!optionSymbol.isEmpty()) {
            Symbol symbol = optionSymbol.get();
            sb.append("# read variable\n")
              .append("\tlw t0, " + symbol.offset + "(fp)\n");
            push("t0");
            return symbol.type;
        } else {
            reportError("use variable that is not defined", ctx);
            return new NoType();
        }
    }

    @Override
    public Type visitParenthesizedPrimary(ParenthesizedPrimaryContext ctx) {
        return visit(ctx.expr());
    }

    /* 函数相关 */
    private String currentFunc;
    private boolean containsMain = false;

    /* 符号相关 */
    private int localCount;
    private Map<String, Symbol> symbolTable = new HashMap<>(); // 符号表，目前我们只有一个符号域

    private Optional<Symbol> lookupSymbol(String v) {
        if (symbolTable.containsKey(v))
            return Optional.of(symbolTable.get(v));
        else
            return Optional.empty();
    }

    /* 控制语句相关 */
    private int condNo = 0; // 用于给条件语句和条件表达式所用的 label 编号，避免 label 名称冲突

    /* 一些工具方法 */
    /**
     * 比较大整数 s 和 t 的大小，可能的结果为小于（-1）、等于（0）或大于（1）。
     * 这里 s 和 t 以字符串的形式给出，要求它们仅由数字 0-9 组成。
     */
    private int compare(String s, String t) {
        if (s.length() != t.length())
            return s.length() < t.length() ? -1 : 1;
        else {
            for (int i = 0; i < s.length(); ++i)
                if (s.charAt(i) != t.charAt(i))
                    return s.charAt(i) < t.charAt(i) ? -1 : 1;
            return 0;
        }
    }

    /**
     * 报错，并输出错误信息和错误位置。
     * 
     * @param s 错误信息
     * @param ctx 发生错误的环境，用于确定错误的位置
     */
    private void reportError(String s, ParserRuleContext ctx) {
        throw new RuntimeException("Error("
            + ctx.getStart().getLine() + ", "
            + ctx.getStart().getCharPositionInLine() + "): " + s + ".\n");
    }

    /**
     * 将寄存器的值压入栈中。
     * 
     * @param reg 待压栈的寄存器
     */
    private void push(String reg) {
        sb.append("# push " + reg + "\n")
          .append("\taddi sp, sp, -4\n")
          .append("\tsw " + reg + ", 0(sp)\n");
    }

    /**
     * 将栈顶的值弹出到寄存器中。
     *
     * @param reg 用于存储栈顶值的寄存器
     */
    private void pop(String reg) {
        sb.append("# pop " + reg + "\n")
          .append("\tlw " + reg + ", 0(sp)\n")
          .append("\taddi sp, sp, 4\n");
    }
}