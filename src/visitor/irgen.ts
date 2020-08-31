import { ParserRuleContext } from "antlr4ts";
import { AbstractParseTreeVisitor } from "antlr4ts/tree";
import MiniDecafParser = require("../gen/MiniDecafParser");
import { MiniDecafVisitor } from "../gen/MiniDecafVisitor";
import { Label, Ir } from "../ir";

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
        this.ir.newFunc(ctx.Ident().text, ctx["localVarSize"]);
        ctx.blockItem().forEach((item) => item.accept(this));
        // 如果函数没有返回语句，默认返回 0
        if (this.ir.missReturn()) {
            this.ir.emitImmediate(0);
            this.ir.emitReturn();
        }
    }

    visitDecl(ctx: MiniDecafParser.DeclContext) {
        if (ctx.expr()) {
            ctx.expr().accept(this);
            this.ir.emitStoreVar(ctx["variable"].offset);
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

    visitAssignExpr(ctx: MiniDecafParser.AssignExprContext) {
        ctx.expr().accept(this);
        this.ir.emitStoreVar(ctx["variable"].offset);
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
        this.ir.emitImmediate(ctx["integer"]);
    }

    visitIdentExpr(ctx: MiniDecafParser.IdentExprContext) {
        this.ir.emitLoadVar(ctx["variable"].offset);
    }

    visitUnaryExpr(ctx: MiniDecafParser.UnaryExprContext) {
        ctx.factor().accept(this);
        this.ir.emitUnary(ctx.getChild(0).text);
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
