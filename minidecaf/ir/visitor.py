from .instr import *

class IRVisitor:
    def visitConst(self, instr:Const):
        pass

    def visitRet(self, instr:Ret):
        pass

    def visitUnary(self, instr:Unary):
        pass

    def visitBinary(self, instr:Binary):
        pass

    def visitComment(self, instr:Comment):
        pass

    def visitPop(self, instr:Pop):
        pass

    def visitLoad(self, instr:Load):
        pass

    def visitStore(self, instr:Store):
        pass

    def visitLabel(self, instr:Label):
        pass

    def visitBranch(self, instr:Branch):
        pass

    def visitFrameSlot(self, instr:FrameSlot):
        pass

    def visitGlobalSymbol(self, instr:GlobalSymbol):
        pass

    def visitCall(self, instr:Call):
        pass

