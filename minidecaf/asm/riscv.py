from ..utils import *
from ..ir.instr import *
from ..ir import *
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

@Instrs
def branch(op, label:str):
    b1 = { "br": (2, "beq"), "beqz": (1, "beq"), "bnez": (1, "bne") }
    if op in b1:
        naux, op = b1[op]
        return push(*[0]*naux) + branch(op, label)
    return pop("t2", "t1") + [f"{op} t1, t2, {label}"]

@Instrs
def label(label:str):
    return [f"{label}:"]

@Instrs
def call(func:str, paramNum:int):
    return [f"call {func}"] + pop(*[None]*paramNum) + push("a0")

@Instrs
def globalSymbol(sym:str):
    return [f"la t1, {sym}"] + push("t1")

class RISCVAsmGen:
    def __init__(self, emitter):
        self._E = emitter
        self.curFunc = None
        self.curParamInfo = None

    def genRet(self, instr:Ret):
        self._E(ret(self.curFunc))

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

    def genBranch(self, instr:Branch):
        self._E(branch(instr.op, instr.label))

    def genLabel(self, instr:Label):
        self._E(label(instr.label))

    def genCall(self, instr:Call):
        _, func = listFind(lambda func: func.name == instr.func, self.ir.funcs)
        self._E(call(func.name, func.paramInfo.paramNum))

    def genGlobalSymbol(self, instr:GlobalSymbol):
        self._E(globalSymbol(instr.sym))

    def genPrologue(self, func:str, paramInfo:ParamInfo):
        self._E([
            AsmBlank(),
            AsmDirective(".text"),
            AsmDirective(f".globl {func}"),
            AsmLabel(f"{func}")] +
            push("ra", "fp") + [
            AsmInstr("mv fp, sp"),
            AsmComment("copy args:")])
        for i in range(paramInfo.paramNum):
            fr, to = 8*(i+2), -8*(i+1)
            self._E([
                AsmInstr(f"ld t1, {fr}(fp)")] +
                push("t1"))
        self._E([
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

    def genFunc(self, func:IRFunc):
        self.curFunc = func.name
        self.genPrologue(f"{func.name}", func.paramInfo)
        for instr in func.instrs:
            self._E([
                AsmComment(instr)])
            _g[type(instr)](self, instr)
        self.genEpilogue(f"{func.name}")

    def genGlob(self, glob:IRGlob):
        if glob.init is None:
            self._E([AsmDirective(f".comm {glob.sym},{glob.size},{glob.align}")])
        else:
            self._E([
                AsmDirective(".data"),
                AsmDirective(f".globl {glob.sym}"),
                AsmDirective(f".align {glob.align}"),
                AsmDirective(f".size {glob.sym}, {glob.size}"),
                AsmLabel(f"{glob.sym}"),
                AsmDirective(f".quad {glob.init}")])

    def gen(self, ir):
        self.ir = ir
        for glob in ir.globs:
            self.genGlob(glob)
        for func in ir.funcs:
            self.genFunc(func)


_g = { Ret: RISCVAsmGen.genRet, Const: RISCVAsmGen.genConst, Unary: RISCVAsmGen.genUnary,
        Binary: RISCVAsmGen.genBinary, Comment: noOp, FrameSlot: RISCVAsmGen.genFrameSlot,
        Load: RISCVAsmGen.genLoad, Store: RISCVAsmGen.genStore, Pop: RISCVAsmGen.genPop,
        Branch: RISCVAsmGen.genBranch, Label: RISCVAsmGen.genLabel, Call: RISCVAsmGen.genCall,
        GlobalSymbol: RISCVAsmGen.genGlobalSymbol }

