from .instr import IRInstr
from .namer import ParamInfo, GlobInfo


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

    def fromGlobInfo(globInfo:GlobInfo):
        assert globInfo.var.offset is None
        return IRGlob(globInfo.var.ident, globInfo.size, globInfo.init)

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


class IREmitter:
    def __init__(self):
        self.funcs = []
        self.globs = []
        self.curName = None
        self.curParamInfo = None
        self.curInstrs = []

    def enterFunction(self, name:str, paramInfo:ParamInfo):
        self.curName = name
        self.curParamInfo = paramInfo
        self.curInstrs = []

    def exitFunction(self):
        self.funcs.append(IRFunc(self.curName, self.curParamInfo, self.curInstrs))

    def emit(self, irs:[IRInstr]):
        self.curInstrs += irs

    def getIR(self):
        return IRProg(self.funcs, self.globs)

    def emitGlobal(self, globInfo:GlobInfo):
        self.globs += [IRGlob.fromGlobInfo(globInfo)]

    def __call__(self, irs:[IRInstr]):
        self.emit(irs)

