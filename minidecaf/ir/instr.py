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
    """Requires: sp -> | rhs | lhs | ...
    Ensures: sp -> | result | ... """
    def __init__(self, op:str):
        assert op in binaryOps
        self.op = op

    def __str__(self):
        return strOfBinaryOp[self.op]


class Comment(IRInstr):
    def __init__(self, msg:str):
        self.msg = msg

    def __str__(self):
        return f"# {self.msg}"


class Pop(IRInstr):
    def __str__(self):
        return f"pop"


class Load(IRInstr):
    def __str__(self):
        return "load"


class Store(IRInstr):
    """Requires: sp -> | addr | value | ...
    Ensures: sp -> | value | ... """
    def __str__(self):
        return "store"


class FrameSlot(IRInstr):
    """Denotes a region within the current stackframe. No length information
    is stored; only the starting address (the lowest address in the region) is
    stored.

    The structure of a stackframe is shown below (upper rows are of lower addresses)

        sp --------------> + ------------------------
                           |
                           | temporaries when computing an expression
                           |
        sp at beginning -> + ------------------------
          of a statement   |
                           | local variables
                           |
        fp --------------> + ------------------------
                           | old fp  at 0(fp)
                           + ------------------------
                           | old ra  at 8(fp)
                           + ------------------------

    FrameSlot(offset) will be translated to something like `x = fp + offset ; push x`
    """
    def __init__(self, fpOffset:int):
        assert fpOffset < 0
        self.offset = fpOffset

    def __str__(self):
        return f"frameslot {self.offset}"
