from ..ir.instr import *
from . import *

def Instrs(f):
    def g(*args, **kwargs):
        instrs = f(*args, *kwargs)
        return [AsmInstr(x) for x in instrs]
    return g

@Instrs
def push(val):
    return [f"addi sp, sp, -8", f"li t1, {val}", f"sd t1, 0(sp)"]

@Instrs
def pop(reg):
    return [f"ld {reg}, 0(sp)", f"addi sp, sp, 8"]


class RISCVAsmGen:
    def __init__(self, emitter):
        self._E = emitter

    def genRet(self, instr:Ret):
        self._E(pop("a0"))

    def genConst(self, instr:Const):
        self._E(push(instr.v))

    def gen(self, ir):
        self._E([
            AsmDirective(".text"),
            AsmDirective(".globl main"),
            AsmLabel("main")])
        for instr in ir.instrs:
            _g[type(instr)](self, instr)
        self._E([
            AsmInstr("jr ra")])

_g = { Ret: RISCVAsmGen.genRet, Const: RISCVAsmGen.genConst }

