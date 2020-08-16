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


class FuncNameInfo:
    def __init__(self, hasDef=True):
        self._v = {}    # term -> Variable
        self._pos = {}  # term -> (int, int)
        self.blockSlots = {} # BlockContext / ForDeclStmtContext -> int >= 0
        self.hasDef = hasDef

    def __str__(self):
        res = "name resolution:\n"
        def f(pv):
            pos, var = pv
            if var.offset is not None:
                loc = f"at frameslot {var.offset}"
            else:
                loc = f"global symbol"
            return f"{str(pos):>16} : {str(var):<10} {loc}"
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


class GlobInfo:
    def __init__(self, var:Variable, size:int, init=None):
        self.var = var
        self.size = size
        self.init = init # not a byte array -- that requires endian info

    def __str__(self):
        return f"{self.var}, size={self.size}, {self.initStr()}"

    def initStr(self):
        if self.init is None:
            return "uninitialized"
        else:
            return f"initializer={self.init}"


class NameInfo:
    def __init__(self):
        self.funcs = {} # str -> FuncNameInfo. Initialized by Def.
        self.globs = {} # str -> GlobInfo.

    def enterFunction(self, func:str, funcNameInfo: FuncNameInfo):
        self.funcs[func] = funcNameInfo

    def __str__(self):
        def f(fn):
            func, funcNameInfo = fn
            indentedFuncNameInfo = "\t" + str(funcNameInfo).replace("\n", "\n\t")
            return f"NameInfo for {func}:\n{indentedFuncNameInfo}"
        res = "\n--------\n\n".join(map(f, self.funcs.items()))
        res += "\n--------\n\nGlobInfos:\n\t"
        res += "\n\t".join(map(str, self.globs.values()))
        return res


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
        self._curFuncNameInfo = None # == self.nameInfo[curFunc]

    def defVar(self, ctx, term):
        self.curNSlots += 1
        var = self._v[text(term)] = Variable(text(term), -INT_BYTES * self.curNSlots)
        pos = (ctx.start.line, ctx.start.column)
        self._curFuncNameInfo.bind(term, var, pos)

    def useVar(self, ctx, term):
        var = self._v[text(term)]
        pos = (ctx.start.line, ctx.start.column)
        self._curFuncNameInfo.bind(term, var, pos)

    def enterScope(self, ctx):
        self._v.push()
        self._nSlots.append(self.curNSlots)

    def exitScope(self, ctx):
        self._curFuncNameInfo.blockSlots[ctx] = self.curNSlots - self._nSlots[-1]
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

    def visitForDeclStmt(self, ctx:MiniDecafParser.ForDeclStmtContext):
        self.enterScope(ctx)
        self.visitChildren(ctx)
        self.exitScope(ctx)

    def visitAtomIdent(self, ctx:MiniDecafParser.AtomIdentContext):
        var = text(ctx.Ident())
        if var not in self._v:
            raise MiniDecafLocatedError(ctx, f"{var} undeclared")
        self.useVar(ctx, ctx.Ident())

    def visitFuncDef(self, ctx:MiniDecafParser.FuncDefContext):
        func = text(ctx.Ident())
        if func in self.nameInfo.funcs and\
                self.nameInfo.funcs[func].hasDef:
            raise MiniDecafLocatedError(f"redefinition of function {func}")
        funcNameInfo = self._curFuncNameInfo = FuncNameInfo(hasDef=True)
        self.nameInfo.enterFunction(func, funcNameInfo)
        self.enterScope(ctx)
        ctx.paramList().accept(self)
        ctx.block().accept(self)
        self.exitScope(ctx)
        self._curFuncNameInfo = None

    def visitFuncDecl(self, ctx:MiniDecafParser.FuncDeclContext):
        func = text(ctx.Ident())
        funcNameInfo = FuncNameInfo(hasDef=False)
        if func not in self.nameInfo.funcs:
            self.nameInfo.enterFunction(func, funcNameInfo)

    def globalInitializer(self, ctx:MiniDecafParser.ExprContext):
        if ctx is None:
            return None
        try:
            initializer = eval(text(ctx), {}, {})
            return initializer
        except:
            raise MiniDecafLocatedError(ctx, "global initializers must be constants")

    def visitDeclExternalDecl(self, ctx:MiniDecafParser.DeclExternalDeclContext):
        ctx = ctx.decl()
        init = self.globalInitializer(ctx.expr())
        varStr = text(ctx.Ident())
        var = Variable(varStr, None)
        globInfo = GlobInfo(var, INT_BYTES, init)
        if varStr in self._v.peek():
            prevGlobInfo = self.nameInfo.globs[varStr]
            if prevGlobInfo.init is not None and globInfo.init is not None:
                raise MiniDecafLocatedError(ctx, f"redefinition of variable {varStr}")
            if globInfo.init is not None:
                self.nameInfo.globs[varStr].init = init
        else:
            self._v[varStr] = var
            self.nameInfo.globs[varStr] = globInfo

