from .instr import IRInstr
from .namer import ParamInfo


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


class IRProg:
    def __init__(self, funcs:[IRFunc]):
        self.funcs = funcs

    def __str__(self):
        return "\n\n".join(map(str, self.funcs))


class IREmitter:
    def __init__(self):
        self.funcs = []
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
        return IRProg(self.funcs)

    def __call__(self, irs:[IRInstr]):
        self.emit(irs)

