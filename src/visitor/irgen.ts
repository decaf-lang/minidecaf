import { AbstractParseTreeVisitor } from "antlr4ts/tree";
import MiniDecafParser = require("../gen/MiniDecafParser");
import { MiniDecafVisitor } from "../gen/MiniDecafVisitor";
import { SemanticError } from "../error";
import { Ir } from "../ir";

/** 支持的最大整数字面量 */
const MAX_INT_LITERAL = 0x7fff_ffff;

/** 语法树到 IR 的生成器 */
export class IrGen extends AbstractParseTreeVisitor<void> implements MiniDecafVisitor<void> {
    ir: Ir = new Ir();

    defaultResult() {}

    visitStmt(ctx: MiniDecafParser.StmtContext) {
        ctx.expr().accept(this);
        this.ir.emitReturn();
    }

    visitExpr(ctx: MiniDecafParser.ExprContext) {
        let int = parseInt(ctx.Integer().text);
        if (int > MAX_INT_LITERAL) {
            throw new SemanticError(ctx.Integer().symbol, `integer '${int}' is too large`);
        }
        this.ir.emitImmediate(int);
    }
}
