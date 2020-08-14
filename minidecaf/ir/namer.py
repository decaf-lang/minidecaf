from ..utils import *
from ..generated.MiniDecafParser import MiniDecafParser
from ..generated.MiniDecafVisitor import MiniDecafVisitor


class Variable:
    _varcnt = {}
    def __init__(self, ident:str, offset:int):
        incOrInit(Variable._varcnt, ident)
        self.id = Variable._varcnt[ident]
        self.ident = ident
        self.offset = offset

    def __eq__(self, other):
        return self.id == other.id and self.ident == other.ident and\
            self.offset == other.offset

    def __str__(self):
        return f"{self.ident}({self.id})"

    def __repr__(self):
        return self.__str__()

    def __hash__(self):
        return hash((self.ident, self.id, self.offset))


class NameInfo:
    def __init__(self):
        self._v = {}    # term -> Variable
        self._pos = {}  # term -> (int, int)
        self.blockSlots = {} # BlockContext -> int >= 0

    def __str__(self):
        res = "name resolution:\n"
        def f(pv):
            pos, var = pv
            return f"{str(pos):>16} : {str(var):<10} at frameslot {var.offset}"
        res += "\n".join(map(f,
            [(self._pos[term], self._v[term]) for term in self._v]))

        res += "\nnumber of slots in each block:\n"
        def f(bs):
            ctx, slots = bs
            startPos = (ctx.start.line, ctx.start.column)
            stopPos = (ctx.stop.line, ctx.stop.column)
            region = f"{startPos} ~ {stopPos}"
            return f"{region:>32} : {slots}"
        res += "\n".join(map(f, 
            [(ctx, self.blockSlots[ctx]) for ctx in self.blockSlots]))

        return res

    def bind(self, term, var, pos):
        self._v[term] = var
        self._pos[term] = pos

    def __repr__(self):
        return self.__str__()

    def __getitem__(self, term):
        return self._v[term]


class Namer(MiniDecafVisitor):
    """Name resolution.
    It is basically an alpha conversion pass; different variables with the same
    name are resolved to different `Variable`s. The output is a NameInfo, mapping
    variable occurrence (i.e. its Ident's TerminalNodeImpl) to Variable/Offsets."""
    def __init__(self):
        self._v = stacked_dict() # str -> Variable
        self._nSlots = [] # number of frame slots on block entry
        self.curNSlots = 0
        self.nameInfo = NameInfo()

    def defVar(self, ctx, term):
        self.curNSlots += 1
        var = self._v[text(term)] = Variable(text(term), -INT_BYTES * self.curNSlots)
        pos = (ctx.start.line, ctx.start.column)
        self.nameInfo.bind(term, var, pos)

    def useVar(self, ctx, term):
        var = self._v[text(term)]
        pos = (ctx.start.line, ctx.start.column)
        self.nameInfo.bind(term, var, pos)

    def enterScope(self, ctx):
        self._v.push()
        self._nSlots.append(self.curNSlots)

    def exitScope(self, ctx):
        self.nameInfo.blockSlots[ctx] = self.curNSlots - self._nSlots[-1]
        self.curNSlots = self._nSlots[-1]
        self._v.pop()
        self._nSlots.pop()

    def visitBlock(self, ctx:MiniDecafParser.BlockContext):
        self.enterScope(ctx)
        self.visitChildren(ctx)
        self.exitScope(ctx)

    def visitDecl(self, ctx:MiniDecafParser.DeclContext):
        if ctx.expr() is not None:
            ctx.expr().accept(self)
        var = text(ctx.Ident())
        if var in self._v.peek():
            raise MiniDecafLocatedError(ctx, f"redefinition of {var}")
        self.defVar(ctx, ctx.Ident())

    def visitAtomIdent(self, ctx:MiniDecafParser.AtomIdentContext):
        var = text(ctx.Ident())
        if var not in self._v:
            raise MiniDecafLocatedError(ctx, f"{var} undeclared")
        self.useVar(ctx, ctx.Ident())
