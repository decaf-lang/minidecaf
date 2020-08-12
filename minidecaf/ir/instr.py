from ..utils import *

class IRInstr:
    def __repr__(self):
        return self.__str__()


class Const(IRInstr):
    def __init__(self, v:int):
        assert MIN_INT < v < MAX_INT
        self.v = v

    def __str__(self):
        return f"const {self.v}"


class Ret(IRInstr):
    def __str__(self):
        return f"ret"


class Unary(IRInstr):
    def __init__(self, op:str):
        assert op in unaryOps
        self.op = op

    def __str__(self):
        return strOfUnaryOp[self.op]


class Binary(IRInstr):
    def __init__(self, op:str):
        assert op in binaryOps
        self.op = op

    def __str__(self):
        return strOfBinaryOp[self.op]
