from .instr import IRInstr
from ..frontend.namer import ParamInfo


class IRFunc:
    def __init__(self, name:str, paramInfo:ParamInfo, instrs:[IRInstr]):
        self.name = name
        self.paramInfo = paramInfo
        self.instrs = instrs

    def __str__(self):
        def f(i):
            if type(i) is instr.Comment:
                return f"\t\t\t\t{i}"
            if type(i) is instr.Label:
                return f"{i}"
            return f"\t{i}"
        body = '\n'.join(map(f, self.instrs))
        return f"{self.name}({self.paramInfo}):\n{body}"


class IRGlob:
    def __init__(self, sym:str, size:int, init=None, align=8):
        self.sym = sym
        self.size = size
        self.init = init # byte array
        self.align = align

    def __str__(self):
        return f"{self.sym}:\n\tsize={self.size}, align={self.align}\n\t{self.initStr()}"

    def initStr(self):
        if self.init is None:
            return "uninitialized"
        else:
            return f"initializer={self.init}"


class IRProg:
    def __init__(self, funcs:[IRFunc], globs:[IRGlob]):
        self.funcs = funcs
        self.globs = globs

    def __str__(self):
        globs = "\n".join(map(str, self.globs))
        funcs = "\n\n".join(map(str, self.funcs))
        res = "========Globs:\n" + globs
        res += "\n\n========Funcs:\n" + funcs
        return res

