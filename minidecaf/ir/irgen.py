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

    def visitExpr(self, ctx:MiniDecafParser.ExprContext):
        v = int(text(ctx.Integer()))
        self._E(instr.Const(v))
