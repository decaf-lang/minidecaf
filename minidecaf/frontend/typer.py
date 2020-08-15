from ..utils import *
from ..generated.MiniDecafParser import MiniDecafParser
from ..generated.MiniDecafVisitor import MiniDecafVisitor
from ..ir.instr import *
from .namer import *


class TypeInfo:
    def __init__(self):
        self.loc = {} # ExprContext -> (IRInstr|ExprContext)+

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
        return res


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

    def _var(self, term):
        return self._curFuncNameInfo[term]

    def _declTyp(self, ctx:MiniDecafParser.DeclContext):
        return ctx.ty().accept(self)

    def visitCAsgn(self, ctx:MiniDecafParser.CAsgnContext):
        loc = self.locator.locate(self._curFuncNameInfo, ctx.unary())
        if loc is None:
            raise MiniDecafLocatedError(ctx.unary(), "lvalue expected")
        self.typeInfo.setLvalueLoc(ctx.unary(), loc)
        self.visitChildren(ctx)

    def visitFuncDef(self, ctx:MiniDecafParser.FuncDefContext):
        func = text(ctx.Ident())
        self._curFuncNameInfo = self.nameInfo.funcNameInfos[func]
        self.visitChildren(ctx)
        self._curFuncNameInfo = None


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
