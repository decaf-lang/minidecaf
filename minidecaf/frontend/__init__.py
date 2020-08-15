from ..ir import *
from .namer import ParamInfo, GlobInfo, Namer
from .irgen import StackIRGen


def _mkIRGlob(globInfo: GlobInfo):
        assert globInfo.var.offset is None
        return IRGlob(globInfo.var.ident, globInfo.size, globInfo.init)


def nameGen(tree):
    namer = Namer()
    namer.visit(tree)
    return namer.nameInfo


def irGen(tree, nameInfo):
    irEmitter = IREmitter()
    StackIRGen(irEmitter, nameInfo).visit(tree)
    return irEmitter.getIR()


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
        self.globs += [_mkIRGlob(globInfo)]

    def __call__(self, irs:[IRInstr]):
        self.emit(irs)

