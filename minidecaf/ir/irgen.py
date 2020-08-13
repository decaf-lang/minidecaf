from . import *
from .instr import *
from ..utils import *
from ..generated.MiniDecafParser import MiniDecafParser
from ..generated.MiniDecafVisitor import MiniDecafVisitor


class OffsetManager:
    def __init__(self):
        self._off = {}
        self._top = 0

    def __getitem__(self, var):
        return self._off[var]

    def newSlot(self, var=None):
        self._top -= INT_BYTES
        if var is not None:
            self._off[var] = self._top
        return self._top


class LabelManager:
    def __init__(self):
        self.nlabels = {}

    def newLabel(self, scope="_L"):
        if scope not in self.nlabels:
            self.nlabels[scope] = 1
        else:
            self.nlabels[scope] += 1
        return f"{scope}_{self.nlabels[scope]}"


class StackIRGen(MiniDecafVisitor):
    def __init__(self, emitter:IREmitter):
        self._E = emitter
        self.off = OffsetManager()
        self.lbl = LabelManager()

    def visitReturnStmt(self, ctx:MiniDecafParser.ReturnStmtContext):
        self._E([Comment("[ir-stmt] Ret")])
        self.visitChildren(ctx)
        self._E([Ret()])

    def visitExprStmt(self, ctx:MiniDecafParser.ExprStmtContext):
        self._E([Comment("[ir-stmt] Expr")])
        self.visitChildren(ctx)
        self._E([Pop()])

    def visitDeclStmt(self, ctx:MiniDecafParser.DeclStmtContext):
        self._E([Comment("[ir-stmt] Decl")])
        self.visitChildren(ctx)

    def visitIfStmt(self, ctx:MiniDecafParser.IfStmtContext):
        self._E([Comment("[ir-stmt] If")])
        ctx.expr().accept(self)
        exitLabel = self.lbl.newLabel("if_end")
        elseLabel = self.lbl.newLabel("if_else")
        if ctx.el is not None:
            self._E([Branch("beqz", elseLabel)])
            ctx.th.accept(self)
            self._E([Branch("br", exitLabel), Label(elseLabel)])
            ctx.el.accept(self)
            self._E([Label(exitLabel)])
        else:
            self._E([Branch("beqz", exitLabel)])
            ctx.th.accept(self)
            self._E([Label(exitLabel)])

    def visitAtomInteger(self, ctx:MiniDecafParser.AtomIntegerContext):
        self._E([Const(int(text(ctx.Integer())))])

    def visitCUnary(self, ctx:MiniDecafParser.CUnaryContext):
        self.visitChildren(ctx)
        self._E([Unary(text(ctx.unaryOp()))])

    def _binaryExpr(self, ctx, op):
        self.visitChildren(ctx)
        self._E([Binary(text(op))])
    def visitCAdd(self, ctx:MiniDecafParser.CAddContext):
        self._binaryExpr(ctx, ctx.addOp())
    def visitCMul(self, ctx:MiniDecafParser.CMulContext):
        self._binaryExpr(ctx, ctx.mulOp())
    def visitCRel(self, ctx:MiniDecafParser.CRelContext):
        self._binaryExpr(ctx, ctx.relOp())
    def visitCEq(self, ctx:MiniDecafParser.CEqContext):
        self._binaryExpr(ctx, ctx.eqOp())
    def visitCLand(self, ctx:MiniDecafParser.CLandContext):
        self._binaryExpr(ctx, "&&")
    def visitCLor(self, ctx:MiniDecafParser.CLorContext):
        self._binaryExpr(ctx, "||")

    def visitDecl(self, ctx:MiniDecafParser.DeclContext):
        var = text(ctx.Ident())
        if ctx.expr() is not None:
            ctx.expr().accept(self)
        else:
            self._E([Const(0)])
        self.off.newSlot(var)
        self._E([Comment(f"[ir-offset]: {var} -> {self.off[var]}")])

    def visitAtomIdent(self, ctx:MiniDecafParser.AtomIdentContext):
        var = text(ctx.Ident())
        self._E([FrameSlot(self.off[var]), Load()])

    def _computeAddr(self, lvalue:Unary):
        if isinstance(lvalue, MiniDecafParser.TUnaryContext):
            return self._computeAddr(lvalue.atom())
        if isinstance(lvalue, MiniDecafParser.AtomIdentContext):
            return self._E([FrameSlot(self.off[text(lvalue)])])
        elif isinstance(lvalue, MiniDecafParser.AtomParenContext):
            return self._computeAddr(lvalue.expr())
        raise MiniDecafLocatedError(lvalue, f"{text(lvalue)} is not a lvalue")

    def visitCAsgn(self, ctx:MiniDecafParser.CAsgnContext):
        ctx.asgn().accept(self)
        self._computeAddr(ctx.unary())
        self._E([Store()])

    def visitCCond(self, ctx:MiniDecafParser.CCondContext):
        ctx.lor().accept(self)
        exitLabel = self.lbl.newLabel("cond_end")
        elseLabel = self.lbl.newLabel("cond_else")
        self._E([Branch("beqz", elseLabel)])
        ctx.expr().accept(self)
        self._E([Branch("br", exitLabel), Label(elseLabel)])
        ctx.cond().accept(self)
        self._E([Label(exitLabel)])
