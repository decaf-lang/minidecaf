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

    def visitCAdd(self, ctx:MiniDecafParser.CAddContext):
        self.visitChildren(ctx)
        self._E(instr.Binary(text(ctx.addOp())))

    def visitCMul(self, ctx:MiniDecafParser.CMulContext):
        self.visitChildren(ctx)
        self._E(instr.Binary(text(ctx.mulOp())))

