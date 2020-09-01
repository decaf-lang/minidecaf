import { Token, ParserRuleContext } from "antlr4ts";
import { AbstractParseTreeVisitor, TerminalNode } from "antlr4ts/tree";
import MiniDecafParser = require("../gen/MiniDecafParser");
import { MiniDecafVisitor } from "../gen/MiniDecafVisitor";
import { SemanticError } from "../error";
import { Type, BaseType, PointerType, FuncType, Variable, Function } from "../type";
import { Scope, ScopeStack } from "../scope";

/** 支持的最大整数字面量 */
const MAX_INT_LITERAL = 0x7fff_ffff;

/** 返回值都是 {@link ParserRuleContext}，以便直接获取子节点的属性 */
type Result = ParserRuleContext | undefined;

/** 一元运算的结果类型 */
function unaryOpType(op: Token, factor: Type): Type {
    if (factor instanceof BaseType) {
        return factor;
    }
    // 只能对整数类型进行一元运算
    throw new SemanticError(op, `incompatible operand: ${op.text} '${factor}'`);
}

/** 二元运算的结果类型 */
function binaryOpType(op: Token, lhs: Type, rhs: Type): Type {
    if (lhs instanceof BaseType && rhs instanceof BaseType) {
        return lhs;
    }
    if (lhs instanceof PointerType && rhs instanceof PointerType) {
        // 指针类型只允许 == 和 != 运算
        if (["==", "!="].includes(op.text) && lhs.equal(rhs)) {
            return BaseType.Int;
        }
    }
    throw new SemanticError(op, `incompatible operands: '${lhs}' ${op.text} '${rhs}'`);
}

/** 语义检查器 */
export class SemanticCheck
    extends AbstractParseTreeVisitor<Result>
    implements MiniDecafVisitor<Result> {
    /** 作用域栈 */
    private scopes: ScopeStack = new ScopeStack();
    /** 嵌套的循环语句构成的栈 */
    private loopStack: ParserRuleContext[] = [];
    /** 当前表达式是否是作为引用，需要先计算其地址 */
    private asReference: boolean = false;

    defaultResult(): Result {
        return undefined;
    }

    visitProgram(ctx: MiniDecafParser.ProgramContext): Result {
        this.scopes.open(Scope.newGlobal());
        this.visitChildren(ctx);
        this.scopes.close();
        return ctx;
    }

    visitType(ctx: MiniDecafParser.TypeContext): Result {
        if (ctx.Int()) {
            ctx["ty"] = BaseType.Int;
        } else {
            ctx["ty"] = new PointerType(ctx.type().accept(this)["ty"]);
        }
        return ctx;
    }

    visitParamList(ctx: MiniDecafParser.ParamListContext): Result {
        let params: Token[] = [];
        let paramTypes: Type[] = [];
        for (let i = 0; i < ctx.childCount; i += 3) {
            let typeNode = ctx.getChild(i);
            let identNode = ctx.getChild(i + 1) as TerminalNode;
            params.push(identNode.symbol);
            paramTypes.push(typeNode.accept(this)["ty"]);
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
        let funcType = new FuncType(paramTypes, ctx.type().accept(this)["ty"]);
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
            let type = ctx.type().accept(this)["ty"];
            let isGlobal = !this.scopes.currentFunc(); // 如果当前函数作用域为空，则当前是全局作用域
            let expr = ctx.expr();
            if (expr) {
                expr.accept(this);
                if (!type.equal(expr["ty"])) {
                    // 初值类型不匹配，报错
                    throw new SemanticError(
                        ctx.Assign().symbol,
                        `cannot assign '${expr["ty"]}' to '${type}'`,
                    );
                } else if (isGlobal) {
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

    visitReturnStmt(ctx: MiniDecafParser.ReturnStmtContext): Result {
        let fnName = this.scopes.currentFunc().name;
        let fn = this.scopes.currentGlobal().find(fnName) as Function;
        let e = ctx.expr().accept(this);
        if (!e["ty"].equal(fn.type.ret)) {
            // 返回值类型与函数定义中的不匹配，报错
            throw new SemanticError(
                e.start,
                `mismatched return type: expected '${fn.type.ret}', found '${e["ty"]}`,
            );
        }
        return ctx;
    }

    visitIfStmt(ctx: MiniDecafParser.IfStmtContext): Result {
        let cond = ctx.expr().accept(this);
        if (!cond["ty"].equal(BaseType.Int)) {
            throw new SemanticError(
                cond.start,
                `incompatible condition type '${cond["ty"]}' in 'if' statement`,
            );
        }
        ctx.stmt().forEach((stmt) => stmt.accept(this));
        return ctx;
    }

    visitForStmt(ctx: MiniDecafParser.ForStmtContext): Result {
        this.loopStack.push(ctx);
        this.scopes.open(Scope.newLocal()); // for 循环需要打开一个新的局部作用域
        this.visitChildren(ctx);
        let cond = ctx._cond;
        if (cond && !cond["ty"].equal(BaseType.Int)) {
            throw new SemanticError(
                cond.start,
                `incompatible condition type '${cond["ty"]}' in 'for' statement`,
            );
        }
        this.scopes.close();
        this.loopStack.pop();
        return ctx;
    }

    visitWhileStmt(ctx: MiniDecafParser.WhileStmtContext): Result {
        this.loopStack.push(ctx);
        let cond = ctx.expr().accept(this);
        if (!cond["ty"].equal(BaseType.Int)) {
            throw new SemanticError(
                cond.start,
                `incompatible condition type '${cond["ty"]}' in 'while' statement`,
            );
        }
        ctx.stmt().accept(this);
        this.loopStack.pop();
        return ctx;
    }

    visitDoStmt(ctx: MiniDecafParser.DoStmtContext): Result {
        this.loopStack.push(ctx);
        ctx.stmt().accept(this);
        let cond = ctx.expr().accept(this);
        if (!cond["ty"].equal(BaseType.Int)) {
            throw new SemanticError(
                cond.start,
                `incompatible condition type '${cond["ty"]}' in 'do-while' statement`,
            );
        }
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

    visitExpr(ctx: MiniDecafParser.ExprContext): Result {
        let e = ctx.getChild(0).accept(this);
        ctx["ty"] = e["ty"];
        ctx["lvalue"] = e["lvalue"];
        return ctx;
    }

    visitAssignExpr(ctx: MiniDecafParser.AssignExprContext): Result {
        this.asReference = true;
        let lv = ctx.unary().accept(this);
        this.asReference = false;
        let rv = ctx.expr().accept(this);
        if (!lv["lvalue"]) {
            // 不是给左值赋值，报错
            throw new SemanticError(
                ctx.Assign().symbol,
                "lvalue required as left operand of assignment",
            );
        } else if (!lv["ty"].equal(rv["ty"])) {
            // 类型不匹配，报错
            throw new SemanticError(
                ctx.Assign().symbol,
                `cannot assign '${lv["ty"]}' to '${rv["ty"]}'`,
            );
        }
        ctx["ty"] = lv["ty"];
        return ctx;
    }

    visitCondExpr(ctx: MiniDecafParser.CondExprContext): Result {
        let cond = ctx.orExpr().accept(this);
        if (ctx.Question()) {
            if (!cond["ty"].equal(BaseType.Int)) {
                throw new SemanticError(
                    cond.start,
                    `incompatible condition type '${cond["ty"]}' in conditional expression`,
                );
            }
            let t = ctx.expr().accept(this);
            let f = ctx.condExpr().accept(this);
            if (!t["ty"].equal(f["ty"])) {
                throw new SemanticError(
                    ctx.Colon().symbol,
                    `type '${t["ty"]}' and '${f["ty"]}' mismatched in conditional expression`,
                );
            }
            ctx["ty"] = t["ty"];
        } else {
            ctx["ty"] = cond["ty"];
            ctx["lvalue"] = cond["lvalue"];
        }
        return ctx;
    }

    visitOrExpr(ctx: MiniDecafParser.OrExprContext): Result {
        return this.visitBinary(ctx);
    }

    visitAndExpr(ctx: MiniDecafParser.AndExprContext): Result {
        return this.visitBinary(ctx);
    }

    visitEqualExpr(ctx: MiniDecafParser.EqualExprContext): Result {
        return this.visitBinary(ctx);
    }

    visitRelExpr(ctx: MiniDecafParser.RelExprContext): Result {
        return this.visitBinary(ctx);
    }

    visitAddExpr(ctx: MiniDecafParser.AddExprContext): Result {
        return this.visitBinary(ctx);
    }

    visitMulExpr(ctx: MiniDecafParser.MulExprContext): Result {
        return this.visitBinary(ctx);
    }

    visitIntExpr(ctx: MiniDecafParser.IntExprContext): Result {
        let int = parseInt(ctx.Integer().text);
        ctx["ty"] = BaseType.Int;
        ctx["integer"] = int;
        if (int > MAX_INT_LITERAL) {
            throw new SemanticError(ctx.Integer().symbol, `integer '${int}' is too large`);
        }
        return ctx;
    }

    visitIdentExpr(ctx: MiniDecafParser.IdentExprContext): Result {
        ctx["lvalue"] = this.asReference; // 成为左值还需 asReference 为真
        this.asReference = false;
        let name = ctx.Ident().text;
        let v = this.scopes.find(name);
        if (v instanceof Variable) {
            ctx["variable"] = v;
        } else {
            throw new SemanticError(ctx.Ident().symbol, `variable '${name}' is not declared`);
        }
        ctx["ty"] = v.type;
        return ctx;
    }

    visitNestedExpr(ctx: MiniDecafParser.NestedExprContext): Result {
        let e = ctx.expr().accept(this);
        ctx["ty"] = e["ty"];
        ctx["lvalue"] = e["lvalue"]; // 如果 e 是左值，(e) 也是左值
        return ctx;
    }

    visitPostfixExpr(ctx: MiniDecafParser.PostfixExprContext): Result {
        let p = ctx.postfix().accept(this);
        ctx["ty"] = p["ty"];
        ctx["lvalue"] = p["lvalue"];
        return ctx;
    }

    visitCastExpr(ctx: MiniDecafParser.CastExprContext): Result {
        let to = ctx.type().accept(this)["ty"] as Type;
        let from = ctx.unary().accept(this)["ty"] as Type;
        if (!from.canCast(to)) {
            throw new SemanticError(ctx.start, `cannot cast from '${from}' to '${to}'`);
        }
        ctx["ty"] = to;
        return ctx;
    }

    visitUnaryExpr(ctx: MiniDecafParser.UnaryExprContext): Result {
        let op = ctx.getChild(0) as TerminalNode;
        let f = ctx.unary();
        if (op.text == "&") {
            // 取地址
            this.asReference = true;
            f.accept(this);
            this.asReference = false;
            if (!f["lvalue"]) {
                throw new SemanticError(op.symbol, "lvalue required as unary '&' operand");
            }
            ctx["ty"] = f["ty"].ref();
        } else if (op.text == "*") {
            // 解引用
            ctx["lvalue"] = this.asReference; // 如果 asReference 为真，*e 也是左值
            this.asReference = false;
            f.accept(this);
            ctx["ty"] = f["ty"].deref();
            if (!ctx["ty"]) {
                throw new SemanticError(op.symbol, `cannot dereference type '${f["ty"]}'`);
            }
        } else {
            // 其他一元运算
            f.accept(this);
            ctx["ty"] = unaryOpType(op.symbol, f["ty"]);
        }
        return ctx;
    }

    visitPrimaryExpr(ctx: MiniDecafParser.PrimaryExprContext): Result {
        let p = ctx.primary().accept(this);
        ctx["ty"] = p["ty"];
        ctx["lvalue"] = p["lvalue"];
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
            for (let i = 0; i < f.type.paramCount(); i++) {
                let e = ctx.expr(i).accept(this);
                if (!e["ty"].equal(f.type.params[i])) {
                    // 参数类型不匹配，报错
                    throw new SemanticError(
                        e.start,
                        `mismatched argument type: expected '${f.type.params[i]}', found '${e["ty"]}'`,
                    );
                }
            }
            ctx["ty"] = f.type.ret;
        } else {
            // 函数没有被声明定义，或不是函数类型，报错
            throw new SemanticError(ident.symbol, `function '${fnName}' is not declared`);
        }
        return ctx;
    }

    private visitBinary(ctx: ParserRuleContext): Result {
        if (ctx.childCount == 1) {
            let e = ctx.getChild(0).accept(this);
            ctx["ty"] = e["ty"];
            ctx["lvalue"] = e["lvalue"];
        } else {
            let op = ctx.getChild(1) as TerminalNode;
            let lhs = ctx.getChild(0).accept(this)["ty"];
            let rhs = ctx.getChild(2).accept(this)["ty"];
            ctx["ty"] = binaryOpType(op.symbol, lhs, rhs);
        }
        return ctx;
    }
}
