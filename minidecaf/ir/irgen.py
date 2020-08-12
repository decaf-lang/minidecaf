from . import *
from ..utils import *
from ..generated.MiniDecafParser import MiniDecafParser
from ..generated.MiniDecafVisitor import MiniDecafVisitor

class StackIRGen(MiniDecafVisitor):
    def __init__(self, emitter:IREmitter):
        self._E = emitter

    def visitReturnStmt(self, ctx:MiniDecafParser.ReturnStmtContext):
        self.visitChildren(ctx)
        self._E(instr.Ret())

    def visitAtomInteger(self, ctx:MiniDecafParser.AtomIntegerContext):
        self._E(instr.Const(int(text(ctx.Integer()))))

    def visitCUnary(self, ctx:MiniDecafParser.CUnaryContext):
        self.visitChildren(ctx)
        self._E(instr.Unary(text(ctx.unaryOp())))

    def _binaryExpr(self, ctx, op):
        self.visitChildren(ctx)
        self._E(instr.Binary(text(op)))
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
