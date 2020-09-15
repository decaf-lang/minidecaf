package minidecaf;

import minidecaf.MiniDecafParser.*;
import minidecaf.Type.*;

import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.tree.*;

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
        visit(ctx.stmt());

        return new NoType();
    }

    @Override
    public Type visitStmt(StmtContext ctx) {
        visit(ctx.expr());
        // 函数返回，返回值存在 a0 中
        sb.append("# ret\n");
        pop("a0");
        sb.append("\tret\n");
        return new NoType();
    }

    @Override
    public Type visitExpr(ExprContext ctx) {
        return visit(ctx.unary());
    }

    @Override
    public Type visitUnary(UnaryContext ctx) {
        if (ctx.children.size() == 1) { // 数字字面量
            TerminalNode num = ctx.NUM();

            // 数字字面量不能超过整型的最大值
            if (compare(Integer.toString(Integer.MAX_VALUE), num.getText()) == -1)
                reportError("too large number", ctx);

            sb.append("# number " + num.getText() + "\n")
              .append("\tli t0, " + num.getText() + "\n");
            push("t0");
            return new IntType();
        } else { // 被一个一元操作符操作的表达式
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
        }
    }

    /* 函数相关 */
    private String currentFunc;
    private boolean containsMain = false;

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