from ..ir import *
from .namer import GlobInfo, Namer
from .irgen import StackIRGen
from .typer import Typer


def _mkIRGlob(globInfo: GlobInfo):
        assert globInfo.var.offset is None
        return IRGlob(globInfo.var.ident, globInfo.size, globInfo.init)


def nameGen(tree):
    namer = Namer()
    namer.visit(tree)
    return namer.nameInfo


def typeCheck(tree, nameInfo):
    typer = Typer(nameInfo)
    typer.visit(tree)
    return typer.typeInfo


def irGen(tree, nameInfo, typeInfo):
    irEmitter = IREmitter()
    StackIRGen(irEmitter, nameInfo, typeInfo).visit(tree)
    return irEmitter.getIR()


class IREmitter:
    def __init__(self):
        self.funcs = []
        self.globs = []
        self.curName = None
        self.curNParams = None
        self.curInstrs = []

    def enterFunction(self, name:str, nParams:int):
        self.curName = name
        self.curNParams = nParams
        self.curInstrs = []

    def exitFunction(self):
        self.funcs.append(IRFunc(self.curName, self.curNParams, self.curInstrs))

    def emit(self, irs:[IRInstr]):
        self.curInstrs += irs

    def getIR(self):
        return IRProg(self.funcs, self.globs)

    def emitGlobal(self, globInfo:GlobInfo):
        self.globs += [_mkIRGlob(globInfo)]

    def __call__(self, irs:[IRInstr]):
        self.emit(irs)

