from ..utils import *
from ..ir.instr import *
from . import *

def Instrs(f):
    def g(*args, **kwargs):
        instrs = f(*args, *kwargs)
        return [(AsmInstr(x) if type(x) is not AsmInstr else x) for x in instrs]
    return g

@Instrs
def _push(val):
    if type(val) is int:
        return [f"addi sp, sp, -8", f"li t1, {val}", f"sd t1, 0(sp)"] # push int
    else:
        return [f"addi sp, sp, -8", f"sd {val}, 0(sp)"] # push register

def push(*vals):
    return flatten(map(_push, vals))

@Instrs
def _pop(reg):
    return ([f"ld {reg}, 0(sp)"] if reg is not None else []) + [f"addi sp, sp, 8"]

def pop(*regs):
    return flatten(map(_pop, regs))

@Instrs
def unary(op):
    op = {'-': "neg", '!': "seqz", '~': "not"}[op]
    return pop("t1") + [f"{op} t1, t1"] + push("t1")

@Instrs
def binary(op):
    b1 = { "+": "add", "-": "sub", "*": "mul", "/": "div", "%": "rem" }
    if op in b1:
        return pop("t2", "t1") + [f"{b1[op]} t1, t1, t2"] + push("t1")
    b2 = { "==": "seqz", "!=": "snez" }
    if op in b2:
        return pop("t2", "t1") + [f"sub t1, t1, t2", f"{b2[op]} t1, t1"] + push("t1")
    b3 = { "<": "slt", ">": "sgt" }
    if op in b3:
        return pop("t2", "t1") + [f"{b3[op]} t1, t1, t2"] + push("t1")
    if op == "||":
        return pop("t2", "t1") + [f"or t1, t1, t2", f"snez t1, t1"] + push("t1")
    if op == "&&":
        return pop("t2") + unary("!") + push("t2") + unary("!") + binary("||") + unary("!")
    if op == "<=":
        return binary(">") + unary("!")
    if op == ">=":
        return binary("<") + unary("!")

@Instrs
def frameSlot(offset):
    return push("fp", offset) + binary("+")

@Instrs
def load():
    return pop("t1") + [f"ld t1, 0(t1)"] + push("t1")

@Instrs
def store():
    return pop("t2", "t1") + [f"sd t1, 0(t2)"] + push("t1")

@Instrs
def ret(func:str):
    return [f"beqz x0, {func}_exit"]

class RISCVAsmGen:
    def __init__(self, emitter):
        self._E = emitter

    def genRet(self, instr:Ret):
        self._E(ret("main"))

    def genConst(self, instr:Const):
        self._E(push(instr.v))

    def genUnary(self, instr:Unary):
        self._E(unary(instr.op))

    def genBinary(self, instr:Binary):
        self._E(binary(instr.op))

    def genFrameSlot(self, instr:FrameSlot):
        self._E(frameSlot(instr.offset))

    def genLoad(self, instr:Load):
        self._E(load())

    def genStore(self, instr:Store):
        self._E(store())

    def genPop(self, instr:Pop):
        self._E(pop(None))

    def genPrologue(self, func:str):
        self._E([
            AsmBlank(),
            AsmDirective(".text"),
            AsmDirective(f".globl {func}"),
            AsmLabel(f"{func}")] +
            push("ra", "fp") + [
            AsmInstr("mv fp, sp"),
            AsmComment("END PROLOGUE"),
            AsmBlank()])

    def genEpilogue(self, func:str):
        self._E([
            AsmBlank(),
            AsmComment("BEGIN EPOLOGUE")] +
            push(0) + [
            AsmLabel(f"{func}_exit"),
            AsmInstr("ld a0, 0(sp)"),
            AsmInstr("mv sp, fp")] +
            pop("fp", "ra") + [
            AsmInstr("jr ra"),
            AsmBlank()])

    def gen(self, ir):
        self.genPrologue("main")
        for instr in ir.instrs:
            self._E([
                AsmComment(instr)])
            _g[type(instr)](self, instr)
        self.genEpilogue("main")

_g = { Ret: RISCVAsmGen.genRet, Const: RISCVAsmGen.genConst, Unary: RISCVAsmGen.genUnary,
        Binary: RISCVAsmGen.genBinary, Comment: noOp, FrameSlot: RISCVAsmGen.genFrameSlot,
        Load: RISCVAsmGen.genLoad, Store: RISCVAsmGen.genStore, Pop: RISCVAsmGen.genPop }

