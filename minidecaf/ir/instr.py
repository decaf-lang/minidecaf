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

    def accept(self, visitor):
        visitor.visitConst(self)


class Ret(IRInstr):
    def __str__(self):
        return f"ret"

    def accept(self, visitor):
        visitor.visitRet(self)


class Unary(IRInstr):
    def __init__(self, op:str):
        assert op in unaryOps
        self.op = op

    def __str__(self):
        return strOfUnaryOp[self.op]

    def accept(self, visitor):
        visitor.visitUnary(self)


class Binary(IRInstr):
    """Requires: sp -> | rhs | lhs | ...
    Ensures: sp -> | result | ... """
    def __init__(self, op:str):
        assert op in binaryOps
        self.op = op

    def __str__(self):
        return strOfBinaryOp[self.op]

    def accept(self, visitor):
        visitor.visitBinary(self)


class Comment(IRInstr):
    def __init__(self, msg:str):
        self.msg = msg

    def __str__(self):
        return f"# {self.msg}"

    def accept(self, visitor):
        visitor.visitComment(self)


class Pop(IRInstr):
    def __str__(self):
        return f"pop"

    def accept(self, visitor):
        visitor.visitPop(self)


class Load(IRInstr):
    def __str__(self):
        return "load"

    def accept(self, visitor):
        visitor.visitLoad(self)


class Store(IRInstr):
    """Requires: sp -> | addr | value | ...
    Ensures: sp -> | value | ... """
    def __str__(self):
        return "store"

    def accept(self, visitor):
        visitor.visitStore(self)


class Label(IRInstr):
    def __init__(self, label:str):
        self.label = label

    def __str__(self):
        return f"{self.label}:"

    def accept(self, visitor):
        visitor.visitLabel(self)


class Branch(IRInstr):
    """Consumes 0 (for br), 1 (for beqz) or 2 (for beq) args.
    Uses the same lhs/rhs rule as binary."""
    def __init__(self, op, label:str):
        assert op in branchOps
        self.op = op
        self.label = label

    def __str__(self):
        return f"{self.op} {self.label}"

    def accept(self, visitor):
        visitor.visitBranch(self)


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
                           | old ra  at 4(fp)
                           + ------------------------

    FrameSlot(offset) will be translated to something like `x = fp + offset ; push x`
    """
    def __init__(self, fpOffset:int):
        assert fpOffset < 0
        self.offset = fpOffset

    def __str__(self):
        return f"frameslot {self.offset}"

    def accept(self, visitor):
        visitor.visitFrameSlot(self)


class GlobalSymbol(IRInstr):
    def __init__(self, sym:str):
        self.sym = sym

    def __str__(self):
        return f"globalsymbol {self.sym}"

    def accept(self, visitor):
        visitor.visitGlobalSymbol(self)


class Call(IRInstr):
    """Before call, arguments need be pushed (from right to left)."""
    def __init__(self, func:str):
        self.func = func

    def __str__(self):
        return f"call {self.func}"

    def accept(self, visitor):
        visitor.visitCall(self)

