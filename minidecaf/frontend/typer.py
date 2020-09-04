from ..utils import *
from ..generated import MiniDecafVisitor, MiniDecafParser
from ..ir.instr import *
from .namer import *
from .types import *


class TypeInfo:
    def __init__(self):
        self.loc = {} # ExprContext -> (IRInstr|ExprContext)+
        self.funcs = {} # str -> FuncTypeInfo
        self._t = {} # ExprContext -> Type

    def lvalueLoc(self, ctx):
        return self.loc[ctx]

    def setLvalueLoc(self, ctx, loc:list):
        self.loc[ctx] = loc

    def __str__(self):
        res = "Lvalue analysis result: (location of expr at lhs == value of rhs):\n\t"
        def p(c):
            return f"{c.start.line},{c.start.column}~{c.stop.line},{c.stop.column}"
        def g(locStep):
            if isinstance(locStep, IRInstr):
                return f"{locStep}"
            else:
                return f"[{p(locStep)}]"
        def f(cl):
            ctx, loc = cl
            ctxStr = f"{p(ctx)}"
            locStr = " :: ".join(map(g, loc))
            return f"{ctxStr:>32} : {locStr}"
        res += "\n\t".join(map(f, self.loc.items()))
        res += "\n\nType info for funcs:\n\t"
        def f(nf):
            name, funcInfo = nf
            return f"{name:>32} : ({funcInfo.paramTy}) -> {funcInfo.retTy}"
        res += "\n\t".join(map(f, self.funcs.items()))
        return res

    def __getitem__(self, ctx):
        return self._t[ctx]


class FuncTypeInfo:
    def __init__(self, retTy:Type, paramTy:list):
        self.retTy = retTy
        self.paramTy = paramTy

    def compatible(self, other):
        return self.retTy == other.retTy and self.paramTy == other.paramTy

    def call(self):
        @TypeRule
        def callRule(ctx, argTy:list):
            if self.paramTy == argTy:
                return self.retTy
            return f"bad argument types"
        return callRule


def SaveType(f):
    def g(self, ctx):
        ty = f(self, ctx)
        self.typeInfo._t[ctx] = ty
        return ty
    return g


class Typer(MiniDecafVisitor):
    """Type checking.
    Run after name resolution, type checking computes the type of each
    expression, and also check for incompatibilities like int*+int*. Besides
    type checking, Typer also does lvalue analysis i.e. determine which
    expressions are lvalues and their address."""
    def __init__(self, nameInfo: NameInfo):
        self.vartyp = {} # Variable -> Type
        self.nameInfo = nameInfo
        self.curFunc = None
        self.typeInfo = TypeInfo()
        self.locator = Locator(self.nameInfo, self.typeInfo)

    def visitChildren(self, ctx):
        ty = MiniDecafVisitor.visitChildren(self, ctx)
        self.typeInfo._t[ctx] = ty
        return ty

    def _var(self, term):
        return self.nameInfo[term]

    def _declTyp(self, ctx:MiniDecafParser.DeclContext):
        base = ctx.ty().accept(self)
        dims = [int(text(x)) for x in reversed(ctx.Integer())]
        if len(dims) == 0:
            return base
        else:
            return ArrayType.make(base, dims)

    def _funcTypeInfo(self, ctx):
        retTy = ctx.ty().accept(self)
        paramTy = self.paramTy(ctx.paramList())
        return FuncTypeInfo(retTy, paramTy)

    def _argTy(self, ctx:MiniDecafParser.ArgListContext):
        return list(map(lambda x: x.accept(self), ctx.expr()))

    def visitPtrType(self, ctx:MiniDecafParser.PtrTypeContext):
        return PtrType(ctx.ty().accept(self))

    def visitIntType(self, ctx:MiniDecafParser.IntTypeContext):
        return IntType()

    def locate(self, ctx):
        loc = self.locator.locate(self.curFunc, ctx)
        if loc is None:
            raise MiniDecafLocatedError(ctx, "lvalue expected")
        self.typeInfo.setLvalueLoc(ctx, loc)

    def checkUnary(self, ctx, op:str, ty:Type):
        if op in { '-', '!', '~' }:
            rule = intUnaopRule
        elif op == '&':
            rule = addrofRule
        elif op == '*':
            rule = derefRule
        return rule(ctx, ty)

    def checkBinary(self, ctx, op:str, lhs:Type, rhs:Type):
        if op in { '*', '/', '%', *logicOps }:
            rule = intBinopRule
        elif op in { *eqrelOps }:
            rule = eqrelRule
        elif op == '=':
            rule = asgnRule
        elif op == '+':
            rule = tryEach('+', intBinopRule, ptrArithRule)
        else:
            rule = tryEach('-', intBinopRule, ptrArithRule, ptrDiffRule)
        return rule(ctx, lhs, rhs)

    @SaveType
    def visitCCast(self, ctx:MiniDecafParser.CCastContext):
        ctx.cast().accept(self)
        return ctx.ty().accept(self)

    @SaveType
    def visitCUnary(self, ctx:MiniDecafParser.CUnaryContext):
        res = self.checkUnary(ctx.unaryOp(), text(ctx.unaryOp()),
                ctx.cast().accept(self))
        if text(ctx.unaryOp()) == '&':
            self.locate(ctx.cast())
        return res

    @SaveType
    def visitAtomParen(self, ctx:MiniDecafParser.AtomParenContext):
        return ctx.expr().accept(self)

    @SaveType
    def visitCAdd(self, ctx:MiniDecafParser.CAddContext):
        return self.checkBinary(ctx.addOp(), text(ctx.addOp()),
                ctx.add().accept(self), ctx.mul().accept(self))

    @SaveType
    def visitCMul(self, ctx:MiniDecafParser.CMulContext):
        return self.checkBinary(ctx.mulOp(), text(ctx.mulOp()),
                ctx.mul().accept(self), ctx.cast().accept(self))

    @SaveType
    def visitCRel(self, ctx:MiniDecafParser.CRelContext):
        return self.checkBinary(ctx.relOp(), text(ctx.relOp()),
                ctx.rel().accept(self), ctx.add().accept(self))

    @SaveType
    def visitCEq(self, ctx:MiniDecafParser.CEqContext):
        return self.checkBinary(ctx.eqOp(), text(ctx.eqOp()),
                ctx.eq().accept(self), ctx.rel().accept(self))

    @SaveType
    def visitCLand(self, ctx:MiniDecafParser.CLandContext):
        return self.checkBinary(ctx, "&&",
                ctx.land().accept(self), ctx.eq().accept(self))

    @SaveType
    def visitCLor(self, ctx:MiniDecafParser.CLorContext):
        return self.checkBinary(ctx, "||",
                ctx.lor().accept(self), ctx.land().accept(self))

    @SaveType
    def visitCCond(self, ctx:MiniDecafParser.CCondContext):
        return condRule(ctx, ctx.lor().accept(self),
                ctx.expr().accept(self), ctx.cond().accept(self))

    @SaveType
    def visitCAsgn(self, ctx:MiniDecafParser.CAsgnContext):
        res = self.checkBinary(ctx.asgnOp(), text(ctx.asgnOp()),
                ctx.unary().accept(self), ctx.asgn().accept(self))
        self.locate(ctx.unary())
        return res

    @SaveType
    def visitPostfixCall(self, ctx:MiniDecafParser.PostfixCallContext):
        argTy = self._argTy(ctx.argList())
        func = text(ctx.Ident())
        rule = self.typeInfo.funcs[func].call()
        return rule(ctx, argTy)

    @SaveType
    def visitPostfixArray(self, ctx:MiniDecafParser.PostfixArrayContext):
        return arrayRule(ctx,
                ctx.postfix().accept(self), ctx.expr().accept(self))

    @SaveType
    def visitAtomInteger(self, ctx:MiniDecafParser.AtomIntegerContext):
        if safeEval(text(ctx)) == 0:
            return ZeroType()
        else:
            return IntType()

    @SaveType
    def visitAtomIdent(self, ctx:MiniDecafParser.AtomIdentContext):
        var = self._var(ctx.Ident())
        return self.vartyp[var]

    def visitDecl(self, ctx:MiniDecafParser.DeclContext):
        var = self._var(ctx.Ident())
        ty = self._declTyp(ctx)
        self.vartyp[var] = ty
        if ctx.expr() is not None:
            initTyp = ctx.expr().accept(self)
            asgnRule(ctx, ty, initTyp)

    def checkFunc(self, ctx):
        funcTypeInfo = self._funcTypeInfo(ctx)
        func = text(ctx.Ident())
        if func in self.typeInfo.funcs:
            prevFuncTypeInfo = self.typeInfo.funcs[func]
            if not funcTypeInfo.compatible(prevFuncTypeInfo):
                raise MiniDecafLocatedError(ctx, f"conflicting types for {func}")
        else:
            self.typeInfo.funcs[func] = funcTypeInfo

    def visitFuncDef(self, ctx:MiniDecafParser.FuncDefContext):
        func = text(ctx.Ident())
        self.curFunc = func
        self.checkFunc(ctx)
        self.visitChildren(ctx)
        self.curFunc = None

    def visitFuncDecl(self, ctx:MiniDecafParser.FuncDeclContext):
        func = text(ctx.Ident())
        self.curFunc = func
        self.checkFunc(ctx)
        self.curFunc = None

    def paramTy(self, ctx:MiniDecafParser.ParamListContext):
        res = []
        for decl in ctx.decl():
            if decl.expr() is not None:
                raise MiniDecafLocatedError(decl, "parameter cannot have initializers")
            res += [self._declTyp(decl)]
        return res

    def visitDeclExternalDecl(self, ctx:MiniDecafParser.DeclExternalDeclContext):
        ctx = ctx.decl()
        var = self.nameInfo.globs[text(ctx.Ident())].var
        ty = self._declTyp(ctx)
        if var in self.vartyp:
            prevTy = self.vartyp[var]
            if prevTy != ty:
                raise MiniDecafLocatedError(ctx, f"conflicting types for {var.ident}")
        else:
            self.vartyp[var] = ty
        if ctx.expr() is not None:
            initTyp = ctx.expr().accept(self)
            asgnRule(ctx, ty, initTyp)

    def visitReturnStmt(self, ctx:MiniDecafParser.ReturnStmtContext):
        funcRetTy = self.typeInfo.funcs[self.curFunc].retTy
        ty = ctx.expr().accept(self)
        retRule(ctx, funcRetTy, ty)

    def visitIfStmt(self, ctx:MiniDecafParser.IfStmtContext):
        self.visitChildren(ctx)
        stmtCondRule(ctx, ctx.expr().accept(self)) # idempotent

    def visitForDeclStmt(self, ctx:MiniDecafParser.ForDeclStmtContext):
        self.visitChildren(ctx)
        if ctx.ctrl is not None: stmtCondRule(ctx, ctx.ctrl.accept(self))

    def visitForStmt(self, ctx:MiniDecafParser.ForStmtContext):
        self.visitChildren(ctx)
        if ctx.ctrl is not None: stmtCondRule(ctx, ctx.ctrl.accept(self))

    def visitWhileStmt(self, ctx:MiniDecafParser.WhileStmtContext):
        self.visitChildren(ctx)
        stmtCondRule(ctx, ctx.expr().accept(self))

    def visitDoWhileStmt(self, ctx:MiniDecafParser.DoWhileStmtContext):
        self.visitChildren(ctx)
        stmtCondRule(ctx, ctx.expr().accept(self))


class Locator(MiniDecafVisitor):
    def __init__(self, nameInfo:NameInfo, typeInfo:TypeInfo):
        self.nameInfo = nameInfo
        self.typeInfo = typeInfo

    def locate(self, func:str, ctx):
        self.func = func
        res = ctx.accept(self)
        self.func = None
        return res

    def visitAtomIdent(self, ctx:MiniDecafParser.AtomIdentContext):
        var = self.nameInfo[ctx.Ident()]
        if var.offset is None:
            return [GlobalSymbol(var.ident)]
        else:
            return [FrameSlot(var.offset)]

    def visitCUnary(self, ctx:MiniDecafParser.CUnaryContext):
        op = text(ctx.unaryOp())
        if op == '*':
            return [ctx.cast()]

    def visitPostfixArray(self, ctx:MiniDecafParser.PostfixArrayContext):
        fixupMult = self.typeInfo[ctx.postfix()].base.sizeof()
        return [ctx.postfix(), ctx.expr(), Const(fixupMult), Binary('*'), Binary('+')]

    def visitAtomParen(self, ctx:MiniDecafParser.AtomParenContext):
        return ctx.expr().accept(self)

