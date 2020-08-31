import { AbstractParseTreeVisitor } from "antlr4ts/tree";
import MiniDecafParser = require("../gen/MiniDecafParser");
import { MiniDecafVisitor } from "../gen/MiniDecafVisitor";
import { SemanticError } from "../error";
import { Type, Variable } from "../type";
import { Scope, ScopeStack } from "../scope";

/** 支持的最大整数字面量 */
const MAX_INT_LITERAL = 0x7fff_ffff;

/** 语义检查器 */
export class SemanticCheck
    extends AbstractParseTreeVisitor<void>
    implements MiniDecafVisitor<void> {
    /** 作用域栈 */
    private scopes: ScopeStack = new ScopeStack();

    defaultResult() {}

    visitProgram(ctx: MiniDecafParser.ProgramContext) {
        this.scopes.open(Scope.newGlobal());
        ctx.func().accept(this);
    }

    visitFunc(ctx: MiniDecafParser.FuncContext) {
        let fnName = ctx.Ident().text;
        this.scopes.open(Scope.newFunc(fnName)); // 打开一个函数作用域，用于定义参数（step9）
        this.scopes.open(Scope.newLocal()); // 打开一个局部作用域，用于定义局部变量
        ctx.blockItem().forEach((item) => item.accept(this));
        this.scopes.close();
        ctx["localVarSize"] = this.scopes.currentFunc().localVarSize;
        this.scopes.close();
    }

    visitDecl(ctx: MiniDecafParser.DeclContext) {
        let name = ctx.Ident().text;
        if (this.scopes.canDeclare(name)) {
            let type = ctx.Int().text as Type;
            ctx.expr()?.accept(this);
            ctx["variable"] = this.scopes.declareVar(name, type);
        } else {
            throw new SemanticError(ctx.Ident().symbol, `symbol '${name}' is already declared`);
        }
    }

    visitBlockStmt(ctx: MiniDecafParser.BlockStmtContext) {
        this.scopes.open(Scope.newLocal()); // 块语句需要打开一个新的局部作用域
        this.visitChildren(ctx);
        this.scopes.close();
    }

    visitAssignExpr(ctx: MiniDecafParser.AssignExprContext) {
        let name = ctx.Ident().text;
        let v = this.scopes.find(name);
        if (v instanceof Variable) {
            ctx["variable"] = v;
            ctx.expr().accept(this);
        } else {
            throw new SemanticError(ctx.Ident().symbol, `variable '${name}' is not declared`);
        }
    }

    visitIntExpr(ctx: MiniDecafParser.IntExprContext) {
        let int = parseInt(ctx.Integer().text);
        ctx["integer"] = int;
        if (int > MAX_INT_LITERAL) {
            throw new SemanticError(ctx.Integer().symbol, `integer '${int}' is too large`);
        }
    }

    visitIdentExpr(ctx: MiniDecafParser.IdentExprContext) {
        let name = ctx.Ident().text;
        let v = this.scopes.find(name);
        if (v instanceof Variable) {
            ctx["variable"] = v;
        } else {
            throw new SemanticError(ctx.Ident().symbol, `variable '${name}' is not declared`);
        }
    }
}
