from .command import *
from .riscv import RISCVAsmGen as AsmGen


def asmGen(ir, fout):
    asmEmitter = AsmEmitter(fout)
    AsmGen(asmEmitter).gen(ir)


class AsmEmitter:
    def __init__(self, fout):
        self.f = fout

    def emit(self, com:AsmCommand):
        print(f"{com}", file=self.f)

    def __call__(self, coms:[AsmCommand]):
        for com in coms:
            self.emit(com)
