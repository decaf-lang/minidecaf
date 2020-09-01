import { ParserRuleContext } from "antlr4ts";
import { AbstractParseTreeVisitor } from "antlr4ts/tree";
import MiniDecafParser = require("../gen/MiniDecafParser");
import { MiniDecafVisitor } from "../gen/MiniDecafVisitor";
import { Label, VariableOp, Ir } from "../ir";
import { Type, ArrayType, Variable } from "../type";

/** 语法树到 IR 的生成器 */
export class IrGen extends AbstractParseTreeVisitor<void> implements MiniDecafVisitor<void> {
    ir: Ir;

    constructor() {
        super();
        this.ir = new Ir();
        Label.initAllocation();
    }

    defaultResult() {}

    visitFunc(ctx: MiniDecafParser.FuncContext) {
        if (ctx.Semi()) {
            // 不对函数声明生成代码
            return;
        }
        this.ir.newFunc(ctx.Ident().text, ctx["paramCount"], ctx["localVarSize"]);
        ctx.blockItem().forEach((item) => item.accept(this));
        // 如果函数没有返回语句，默认返回 0
        if (this.ir.missReturn()) {
            this.ir.emitImmediate(0, "r0");
            this.ir.emitReturn();
        }
    }

    visitDecl(ctx: MiniDecafParser.DeclContext) {
        let v = ctx["variable"] as Variable;
        if (v.isGlobal) {
            this.ir.newGlobalData(v.name, v.type.sizeof(), ctx["const"]);
        } else if (ctx.expr()) {
            ctx.expr().accept(this);
            this.emitVariable(VariableOp.Store, v);
        }
    }

    visitReturnStmt(ctx: MiniDecafParser.ReturnStmtContext) {
        ctx.expr().accept(this);
        this.ir.emitReturn();
    }

    visitIfStmt(ctx: MiniDecafParser.IfStmtContext) {
        ctx.expr().accept(this);
        let labelFalse = Label.alloc();
        this.ir.emitBeqz(labelFalse);
        ctx.stmt(0).accept(this);
        if (ctx.Else()) {
            let labelNext = Label.alloc();
            this.ir.emitJump(labelNext);
            this.ir.emitLabel(labelFalse);
            ctx.stmt(1).accept(this);
            this.ir.emitLabel(labelNext);
        } else {
            this.ir.emitLabel(labelFalse);
        }
    }

    visitForStmt(ctx: MiniDecafParser.ForStmtContext) {
        let labelLoop = Label.alloc();
        let labelPost = Label.alloc();
        let labelNext = Label.alloc();
        ctx["breakLabel"] = labelNext; // 设置 `break` 语句跳转到哪个标签
        ctx["continueLabel"] = labelPost; // 设置 `continue` 语句跳转到哪个标签
        if (ctx.decl()) {
            ctx.decl().accept(this);
        } else {
            ctx._init?.accept(this);
        }
        // 用 while 循环实现 for 循环
        this.emitWhileLoop(
            ctx._cond && (() => ctx._cond.accept(this)),
            () => {
                ctx.stmt().accept(this);
                this.ir.emitLabel(labelPost);
                ctx._post?.accept(this);
            },
            labelLoop,
            labelNext,
        );
    }

    visitWhileStmt(ctx: MiniDecafParser.WhileStmtContext) {
        let labelLoop = Label.alloc();
        let labelNext = Label.alloc();
        ctx["breakLabel"] = labelNext;
        ctx["continueLabel"] = labelLoop;
        this.emitWhileLoop(
            () => ctx.expr().accept(this),
            () => ctx.stmt().accept(this),
            labelLoop,
            labelNext,
        );
    }

    visitDoStmt(ctx: MiniDecafParser.DoStmtContext) {
        let labelLoop = Label.alloc();
        let labelNext = Label.alloc();
        ctx["breakLabel"] = labelNext;
        ctx["continueLabel"] = labelLoop;
        this.emitDoLoop(
            () => ctx.expr().accept(this),
            () => ctx.stmt().accept(this),
            labelLoop,
            labelNext,
        );
    }

    visitBreakStmt(ctx: MiniDecafParser.BreakStmtContext) {
        this.ir.emitJump(ctx["loop"]["breakLabel"]);
    }

    visitContinueStmt(ctx: MiniDecafParser.ContinueStmtContext) {
        this.ir.emitJump(ctx["loop"]["continueLabel"]);
    }

    visitAssignExpr(ctx: MiniDecafParser.AssignExprContext) {
        ctx.unary().accept(this);
        this.ir.emitPush("r0");
        ctx.expr().accept(this);
        this.ir.emitPop("r1");
        this.ir.emitStore("r0", "r1");
    }

    visitCondExpr(ctx: MiniDecafParser.CondExprContext) {
        ctx.orExpr().accept(this);
        if (ctx.Question()) {
            let labelFalse = Label.alloc();
            let labelNext = Label.alloc();
            this.ir.emitBeqz(labelFalse);
            ctx.expr().accept(this);
            this.ir.emitJump(labelNext);
            this.ir.emitLabel(labelFalse);
            ctx.condExpr().accept(this);
            this.ir.emitLabel(labelNext);
        }
    }

    visitOrExpr(ctx: MiniDecafParser.OrExprContext) {
        this.visitBinary(ctx);
    }

    visitAndExpr(ctx: MiniDecafParser.AndExprContext) {
        this.visitBinary(ctx);
    }

    visitEqualExpr(ctx: MiniDecafParser.EqualExprContext) {
        this.visitBinary(ctx);
    }

    visitRelExpr(ctx: MiniDecafParser.RelExprContext) {
        this.visitBinary(ctx);
    }

    visitAddExpr(ctx: MiniDecafParser.AddExprContext) {
        this.visitBinary(ctx);
    }

    visitMulExpr(ctx: MiniDecafParser.MulExprContext) {
        this.visitBinary(ctx);
    }

    visitIntExpr(ctx: MiniDecafParser.IntExprContext) {
        this.ir.emitImmediate(ctx["integer"], "r0");
    }

    visitIdentExpr(ctx: MiniDecafParser.IdentExprContext) {
        this.emitVariable(
            ctx["lvalue"] || ctx["ty"] instanceof ArrayType ? VariableOp.AddrOf : VariableOp.Load,
            ctx["variable"],
        );
    }

    visitUnaryExpr(ctx: MiniDecafParser.UnaryExprContext) {
        ctx.unary().accept(this);
        let op = ctx.getChild(0).text;
        if (op == "*") {
            if (!ctx["lvalue"]) {
                this.ir.emitLoad("r0", "r0");
            }
        } else if (op != "&") {
            this.ir.emitUnary(op);
        }
    }

    visitIndexExpr(ctx: MiniDecafParser.IndexExprContext) {
        let type = ctx["ty"] as Type;
        ctx.postfix().accept(this); // 计算要取下标的表达式的值，结果存在 `r0`
        this.ir.emitPush("r0"); // 将结果 `r0` 保存到栈顶
        ctx.expr().accept(this); // 计算下标表达式的值，结果存在 `r0`
        this.ir.emitImmediate(type.sizeof(), "r1"); // `r1` 为类型大小
        this.ir.emitBinary("*"); // 生成运算 `r1 * r0`
        this.ir.emitPop("r1"); // 从栈顶弹出 postfix 的值到 `r1`
        this.ir.emitBinary("+"); // 生成运算 `r1 + r0`
        if (!(type instanceof ArrayType) && !ctx["lvalue"]) {
            this.ir.emitLoad("r0", "r0");
        }
    }

    visitFuncCall(ctx: MiniDecafParser.FuncCallContext) {
        let fnName = ctx.Ident().text;
        let args = ctx.expr();
        // 从右到左向栈中压入参数
        for (let i = args.length - 1; i >= 0; i--) {
            args[i].accept(this);
            this.ir.emitPush("r0");
        }
        this.ir.emitCall(fnName, args.length);
    }

    /** 生成对变量的操作，操作类型见 {@link VariableOp} */
    private emitVariable(varOp: VariableOp, v: Variable) {
        let offOrName = v.isGlobal ? v.name : v.offset;
        switch (varOp) {
            case VariableOp.Load:
                this.ir.emitLoadVar(offOrName, v.kind);
                break;
            case VariableOp.Store:
                this.ir.emitStoreVar(offOrName, v.kind);
                break;
            case VariableOp.AddrOf:
                this.ir.emitAddrVar(offOrName, v.kind);
                break;
        }
    }

    /**
     * 生成 while 循环。
     *
     * @param cond 生成条件表达式的函数
     * @param body 生成循环体的函数
     * @param labelLoop 循环开始标签
     * @param labelNext 循环结束标签
     */
    private emitWhileLoop(cond: () => void, body: () => void, labelLoop: Label, labelNext: Label) {
        this.ir.emitLabel(labelLoop);
        if (cond) {
            cond();
            this.ir.emitBeqz(labelNext);
        }
        body();
        this.ir.emitJump(labelLoop);
        this.ir.emitLabel(labelNext);
    }

    /**
     * 生成 do-while 循环。
     *
     * @param cond 生成条件表达式的函数
     * @param body 生成循环体的函数
     * @param labelLoop 循环开始标签
     * @param labelNext 循环结束标签
     */
    private emitDoLoop(cond: () => void, body: () => void, labelLoop: Label, labelNext: Label) {
        this.ir.emitLabel(labelLoop);
        body();
        cond();
        this.ir.emitBnez(labelLoop);
        this.ir.emitLabel(labelNext);
    }

    private visitBinary(ctx: ParserRuleContext) {
        if (ctx.childCount == 1) {
            ctx.getChild(0).accept(this);
        } else {
            let op = ctx.getChild(1);
            ctx.getChild(0).accept(this); // 计算左操作数，结果存在 `r0`
            this.ir.emitPush("r0"); // 将结果 `r0` 保存到栈顶
            ctx.getChild(2).accept(this); // 计算右操作数，结果存在 `r0`
            this.ir.emitPop("r1"); // 从栈顶弹出左操作数的结果到 `r1`
            this.ir.emitBinary(op.text); // 生成运算 `r1` op `r0`
        }
    }
}
