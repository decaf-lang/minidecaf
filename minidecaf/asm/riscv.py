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

class RISCVAsmGen:
    def __init__(self, emitter):
        self._E = emitter

    def genRet(self, instr:Ret):
        self._E(pop("a0"))

    def genConst(self, instr:Const):
        self._E(push(instr.v))

    def genUnary(self, instr:Unary):
        self._E(unary(instr.op))

    def genBinary(self, instr:Binary):
        self._E(binary(instr.op))

    def gen(self, ir):
        self._E([
            AsmDirective(".text"),
            AsmDirective(".globl main"),
            AsmLabel("main")])
        for instr in ir.instrs:
            self._E([
                AsmComment(instr)])
            _g[type(instr)](self, instr)
        self._E([
            AsmInstr("jr ra")])

_g = { Ret: RISCVAsmGen.genRet, Const: RISCVAsmGen.genConst, Unary: RISCVAsmGen.genUnary,
        Binary:RISCVAsmGen.genBinary }

