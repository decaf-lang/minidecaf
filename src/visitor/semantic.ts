import { Token, ParserRuleContext } from "antlr4ts";
import { AbstractParseTreeVisitor, TerminalNode } from "antlr4ts/tree";
import MiniDecafParser = require("../gen/MiniDecafParser");
import { MiniDecafVisitor } from "../gen/MiniDecafVisitor";
import { SemanticError } from "../error";
import { Type, FuncType, Variable, Function } from "../type";
import { Scope, ScopeStack } from "../scope";

/** 支持的最大整数字面量 */
const MAX_INT_LITERAL = 0x7fff_ffff;

/** 返回值都是 {@link ParserRuleContext}，以便直接获取子节点的属性 */
type Result = ParserRuleContext | undefined;

/** 语义检查器 */
export class SemanticCheck
    extends AbstractParseTreeVisitor<Result>
    implements MiniDecafVisitor<Result> {
    /** 作用域栈 */
    private scopes: ScopeStack = new ScopeStack();
    /** 嵌套的循环语句构成的栈 */
    private loopStack: ParserRuleContext[] = [];

    defaultResult(): Result {
        return undefined;
    }

    visitProgram(ctx: MiniDecafParser.ProgramContext): Result {
        this.scopes.open(Scope.newGlobal());
        this.visitChildren(ctx);
        this.scopes.close();
        return ctx;
    }

    visitParamList(ctx: MiniDecafParser.ParamListContext): Result {
        let params: Token[] = [];
        let paramTypes: Type[] = [];
        for (let i = 0; i < ctx.childCount; i += 3) {
            let typeNode = ctx.getChild(i);
            let identNode = ctx.getChild(i + 1) as TerminalNode;
            params.push(identNode.symbol);
            paramTypes.push(typeNode.text as Type);
        }
        ctx["params"] = params;
        ctx["paramTypes"] = paramTypes;
        return ctx;
    }

    visitFunc(ctx: MiniDecafParser.FuncContext): Result {
        let ident = ctx.Ident();
        let fnName = ident.text;

        let paramNode = ctx.paramList().accept(this);
        let params = paramNode["params"];
        let paramTypes = paramNode["paramTypes"];
        let funcType = new FuncType(paramTypes, ctx.Int().text as Type);
        ctx["paramCount"] = params.length;

        /** 首先查找该函数名是否已在符号表中 */
        let symbol = this.scopes.find(fnName);
        if (symbol) {
            if (symbol instanceof Function) {
                if (!symbol.type.equal(funcType)) {
                    // 类型不匹配，报错
                    throw new SemanticError(
                        ident.symbol,
                        `conflicting types for function '${fnName}'`,
                    );
                }
            } else {
                // 不是函数类型，报错
                throw new SemanticError(ident.symbol, `symbol '${fnName}' is already declared`);
            }
        }

        if (ctx.Semi()) {
            // 声明
            if (!symbol) {
                this.scopes.declareFunc(fnName, funcType);
            }
        } else {
            // 定义
            if (symbol) {
                // 已声明
                let f = symbol as Function;
                if (f.defined) {
                    // 重复定义，报错
                    throw new SemanticError(ident.symbol, `redefinition of function '${fnName}'`);
                }
                f.defined = true;
            } else {
                // 未声明
                this.scopes.declareFunc(fnName, funcType, true);
            }

            this.scopes.open(Scope.newFunc(fnName)); // 打开一个函数作用域，用于定义参数
            for (let i = 0; i < params.length; i++) {
                let name = params[i].text;
                if (this.scopes.canDeclare(name)) {
                    this.scopes.declareParam(name, paramTypes[i], i);
                } else {
                    throw new SemanticError(params[i], `parameter '${name}' is already declared`);
                }
            }
            this.scopes.open(Scope.newLocal()); // 打开一个局部作用域，用于定义局部变量
            ctx.blockItem().forEach((item) => item.accept(this));
            this.scopes.close();
            ctx["localVarSize"] = this.scopes.currentFunc().localVarSize;
            this.scopes.close();
        }
        return ctx;
    }

    visitDecl(ctx: MiniDecafParser.DeclContext): Result {
        let name = ctx.Ident().text;
        if (this.scopes.canDeclare(name)) {
            let type = ctx.Int().text as Type;
            let isGlobal = !this.scopes.currentFunc(); // 如果当前函数作用域为空，则当前是全局作用域
            let expr = ctx.expr();
            if (expr) {
                expr.accept(this);
                if (isGlobal) {
                    // 全局变量初值只能是整数常量
                    if (/^\d+$/.test(expr.text)) {
                        ctx["const"] = parseInt(expr.text);
                    } else {
                        throw new SemanticError(
                            expr.start,
                            `initial value of global variable '${name}' is not integer`,
                        );
                    }
                }
            }
            ctx["variable"] = this.scopes.declareVar(name, type, isGlobal);
        } else {
            throw new SemanticError(ctx.Ident().symbol, `symbol '${name}' is already declared`);
        }
        return ctx;
    }

    visitForStmt(ctx: MiniDecafParser.ForStmtContext): Result {
        this.loopStack.push(ctx);
        this.scopes.open(Scope.newLocal()); // for 循环需要打开一个新的局部作用域
        this.visitChildren(ctx);
        this.scopes.close();
        this.loopStack.pop();
        return ctx;
    }

    visitWhileStmt(ctx: MiniDecafParser.WhileStmtContext): Result {
        this.loopStack.push(ctx);
        this.visitChildren(ctx);
        this.loopStack.pop();
        return ctx;
    }

    visitDoStmt(ctx: MiniDecafParser.DoStmtContext): Result {
        this.loopStack.push(ctx);
        this.visitChildren(ctx);
        this.loopStack.pop();
        return ctx;
    }

    visitBreakStmt(ctx: MiniDecafParser.BreakStmtContext): Result {
        if (this.loopStack.length) {
            // 记录 break 语句对应哪个循环
            ctx["loop"] = this.loopStack[this.loopStack.length - 1];
        } else {
            // 不在循环语句中使用 break，报错
            throw new SemanticError(ctx.start, `cannot use '${ctx.text}' out of loop`);
        }
        return ctx;
    }

    visitContinueStmt(ctx: MiniDecafParser.ContinueStmtContext): Result {
        if (this.loopStack.length) {
            // 记录 continue 语句对应哪个循环
            ctx["loop"] = this.loopStack[this.loopStack.length - 1];
        } else {
            // 不在循环语句中使用 continue，报错
            throw new SemanticError(ctx.start, `cannot use '${ctx.text}' out of loop`);
        }
        return ctx;
    }

    visitBlockStmt(ctx: MiniDecafParser.BlockStmtContext): Result {
        this.scopes.open(Scope.newLocal()); // 块语句需要打开一个新的局部作用域
        this.visitChildren(ctx);
        this.scopes.close();
        return ctx;
    }

    visitAssignExpr(ctx: MiniDecafParser.AssignExprContext): Result {
        let name = ctx.Ident().text;
        let v = this.scopes.find(name);
        if (v instanceof Variable) {
            ctx["variable"] = v;
            ctx.expr().accept(this);
        } else {
            throw new SemanticError(ctx.Ident().symbol, `variable '${name}' is not declared`);
        }
        return ctx;
    }

    visitIntExpr(ctx: MiniDecafParser.IntExprContext): Result {
        let int = parseInt(ctx.Integer().text);
        ctx["integer"] = int;
        if (int > MAX_INT_LITERAL) {
            throw new SemanticError(ctx.Integer().symbol, `integer '${int}' is too large`);
        }
        return ctx;
    }

    visitIdentExpr(ctx: MiniDecafParser.IdentExprContext): Result {
        let name = ctx.Ident().text;
        let v = this.scopes.find(name);
        if (v instanceof Variable) {
            ctx["variable"] = v;
        } else {
            throw new SemanticError(ctx.Ident().symbol, `variable '${name}' is not declared`);
        }
        return ctx;
    }

    visitFuncCall(ctx: MiniDecafParser.FuncCallContext): Result {
        let ident = ctx.Ident();
        let fnName = ident.text;
        let f = this.scopes.find(fnName);
        if (f instanceof Function) {
            if (ctx.expr().length != f.type.paramCount()) {
                // 参数个数不匹配，报错
                throw new SemanticError(
                    ident.symbol,
                    `mismatched arguments number of function '${fnName}'`,
                );
            }
            this.visitChildren(ctx);
        } else {
            // 函数没有被声明定义，或不是函数类型，报错
            throw new SemanticError(ident.symbol, `function '${fnName}' is not declared`);
        }
        return ctx;
    }
}
