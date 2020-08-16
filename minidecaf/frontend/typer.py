from ..utils import *
from ..generated.MiniDecafParser import MiniDecafParser
from ..generated.MiniDecafVisitor import MiniDecafVisitor
from ..ir.instr import *
from .namer import *


class Type:
    def __repr__(self):
        return self.__str__()


class VoidType:
    def __init__(self):
        pass

    def __str__(self):
        return "void"

    def __eq__(self, other):
        return isinstance(other, VoidType)


class IntType(Type):
    def __init__(self):
        pass

    def __str__(self):
        return "int"

    def __eq__(self, other):
        return isinstance(other, IntType)


class PtrType(Type):
    def __init__(self, base:Type):
        self.base = base

    def __str__(self):
        return f"{self.base}*"

    def __eq__(self, other):
        if not isinstance(other, PtrType):
            return False
        return self.base == other.base


class ZeroType(IntType, PtrType):
    def __init__(self):
        pass

    def __str__(self):
        return "zerotype"

    def __eq__(self, other):
        return isinstance(other, IntType) or isinstance(other, PtrType)


def TypeRule(f):
    """A type rule is a function: (ctx, *inputTypes) -> {outputType | errStr | None}.
    The ctx parameter is only used for error reporting."""
    def g(ctx, *inTy):
        res = f(ctx, *inTy)
        if type(res) is str:
            raise MiniDecafTypeError(ctx, f"{f.__name__}: {res}")
        if res is None:
            raise MiniDecafTypeError(ctx, f"{f.__name__}: type error")
        return res
    g.__name__ = f.__name__ # black magic
    return g


def tryEach(name="tryEach", *fs):
    """Combine multiple type rules `fs`, returns result the first that does not fail."""
    @TypeRule
    def g(ctx, *inTy):
        errs = []
        for f in fs:
            try:
                return f(ctx, *inTy)
            except MiniDecafTypeError as e:
                errs += [e.msg]
        return f"{name}:\n\t" + '\n\t'.join(map(str, errs))
    g.__name__ = name # black magic
    return g


@TypeRule
def condRule(ctx, cond, tr, fal):
    if cond == IntType() and tr == fal:
        return tr

@TypeRule
def intBinopRule(ctx, lhs, rhs):
    if lhs == IntType() and rhs == IntType():
        return IntType()
    return f"integer expected, got {lhs} and {rhs}"

@TypeRule
def intUnaopRule(ctx, ty):
    if ty == IntType():
        return IntType()
    return f"integer expected, got {ty}"

@TypeRule
def ptrArithRule(ctx, lhs, rhs):
    if lhs == IntType() and isinstance(rhs, PtrType):
        return rhs
    if rhs == IntType() and isinstance(lhs, PtrType):
        return lhs
    return f"pointer and integer, got {lhs} and {rhs}"

@TypeRule
def derefRule(ctx, ty):
    if isinstance(ty, PtrType):
        return ty.base
    return f"pointer expected, got {ty}"

@TypeRule
def addrofRule(ctx, ty):
    return PtrType(ty)

@TypeRule
def eqrelRule(ctx, lhs, rhs):
    if lhs != rhs:
        return f"cannot equate or compare {lhs} to {rhs}"
    if lhs != IntType() and not isinstance(lhs, PtrType):
        return f"expected integer or pointer types, found {lhs}"
    return IntType()

@TypeRule
def asgnRule(ctx, lhs, rhs):
    if lhs != rhs:
        return f"cannot assign {rhs} to {lhs}"
    return lhs

@TypeRule
def retRule(ctx, funcRetTy, ty):
    if funcRetTy != ty:
        return f"return {funcRetTy} expected, {ty} found"
    return VoidType()

@TypeRule
def stmtCondRule(ctx, ty):
    if ty != IntType():
        return f"integer expected, {ty} found"
    return VoidType()


class TypeInfo:
    def __init__(self):
        self.loc = {} # ExprContext -> (IRInstr|ExprContext)+
        self.funcs = {} # str -> FuncTypeInfo

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


class Typer(MiniDecafVisitor):
    """Type checking.
    Run after name resolution, type checking computes the type of each
    expression, and also check for incompatibilities like int*+int*. Besides
    type checking, Typer also does lvalue analysis i.e. determine which
    expressions are lvalues and their address."""
    def __init__(self, nameInfo: NameInfo):
        self.vartyp = {} # Variable -> Type
        self.nameInfo = nameInfo
        self._curFuncNameInfo = None
        self.locator = Locator()
        self.typeInfo = TypeInfo()
        self._curFuncTypeInfo = None

    def _var(self, term):
        return self._curFuncNameInfo[term]

    def _declTyp(self, ctx:MiniDecafParser.DeclContext):
        return ctx.ty().accept(self)

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
        loc = self.locator.locate(self._curFuncNameInfo, ctx)
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
        b1 = { '*', '/', '%', *logicOps }
        if op in b1:
            rule = intBinopRule
            return rule(ctx, lhs, rhs)
        b2 = { *eqrelOps }
        if op in b2:
            rule = eqrelRule
            return rule(ctx, lhs, rhs)
        b3 = { '+', '-' }
        if op in b3:
            rule = tryEach('+', intBinopRule, ptrArithRule)
            return rule(ctx, lhs, rhs)
        if op == '=':
            rule = asgnRule
            return rule(ctx, lhs, rhs)

    def visitCUnary(self, ctx:MiniDecafParser.CUnaryContext):
        if text(ctx.unaryOp()) == '&':
            self.locate(ctx.unary())
        return self.checkUnary(ctx.unaryOp(), text(ctx.unaryOp()),
                ctx.unary().accept(self))

    def visitAtomParen(self, ctx:MiniDecafParser.AtomParenContext):
        return ctx.expr().accept(self)

    def visitCAdd(self, ctx:MiniDecafParser.CAddContext):
        return self.checkBinary(ctx.addOp(), text(ctx.addOp()),
                ctx.add().accept(self), ctx.mul().accept(self))

    def visitCMul(self, ctx:MiniDecafParser.CMulContext):
        return self.checkBinary(ctx.mulOp(), text(ctx.mulOp()),
                ctx.mul().accept(self), ctx.unary().accept(self))

    def visitCRel(self, ctx:MiniDecafParser.CRelContext):
        return self.checkBinary(ctx.relOp(), text(ctx.relOp()),
                ctx.rel().accept(self), ctx.add().accept(self))

    def visitCEq(self, ctx:MiniDecafParser.CEqContext):
        return self.checkBinary(ctx.eqOp(), text(ctx.eqOp()),
                ctx.eq().accept(self), ctx.rel().accept(self))

    def visitCLand(self, ctx:MiniDecafParser.CLandContext):
        return self.checkBinary(ctx, "&&",
                ctx.land().accept(self), ctx.eq().accept(self))

    def visitCLor(self, ctx:MiniDecafParser.CLorContext):
        return self.checkBinary(ctx, "||",
                ctx.lor().accept(self), ctx.land().accept(self))

    def visitCCond(self, ctx:MiniDecafParser.CCondContext):
        return condRule(ctx, ctx.lor().accept(self),
                ctx.expr().accept(self), ctx.cond().accept(self))

    def visitCAsgn(self, ctx:MiniDecafParser.CAsgnContext):
        self.locate(ctx.unary())
        return self.checkBinary(ctx.asgnOp(), text(ctx.asgnOp()),
                ctx.unary().accept(self), ctx.asgn().accept(self))

    def visitPostfixCall(self, ctx:MiniDecafParser.PostfixCallContext):
        argTy = self._argTy(ctx.argList())
        func = text(ctx.Ident())
        rule = self.typeInfo.funcs[func].call()
        return rule(ctx, argTy)

    def visitAtomInteger(self, ctx:MiniDecafParser.AtomIntegerContext):
        if safeEval(text(ctx)) == 0:
            return ZeroType()
        else:
            return IntType()

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
        self._curFuncNameInfo = self.nameInfo.funcs[func]
        self.checkFunc(ctx)
        self._curFuncTypeInfo = self.typeInfo.funcs[func]
        self.visitChildren(ctx)
        self._curFuncTypeInfo = None
        self._curFuncNameInfo = None

    def visitFuncDecl(self, ctx:MiniDecafParser.FuncDeclContext):
        func = text(ctx.Ident())
        self._curFuncNameInfo = self.nameInfo.funcs[func]
        self.checkFunc(ctx)
        self._curFuncNameInfo = None

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
        funcRetTy = self._curFuncTypeInfo.retTy
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
    def locate(self, curFuncNameInfo:FuncNameInfo, ctx):
        self._curFuncNameInfo = curFuncNameInfo
        res = ctx.accept(self)
        self._curFuncNameInfo = None
        return res

    def visitAtomIdent(self, ctx:MiniDecafParser.AtomIdentContext):
        var = self._curFuncNameInfo[ctx.Ident()]
        if var.offset is None:
            return [GlobalSymbol(var.ident)]
        else:
            return [FrameSlot(var.offset)]

    def visitCUnary(self, ctx:MiniDecafParser.CUnaryContext):
        op = text(ctx.unaryOp())
        if op == '*':
            return ctx.unary().accept(self) + [Load()]

    def visitAtomParen(self, ctx:MiniDecafParser.AtomParenContext):
        return ctx.expr().accept(self)
