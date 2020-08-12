from .instr import IRInstr

class IRProg:
    def __init__(self, instrs):
        self.instrs = instrs

    def __str__(self):
        return "main:\n\t" + '\n\t'.join(map(str, self.instrs))

class IREmitter:
    def __init__(self):
        self.instrs = []

    def emit(self, ir:IRInstr):
        self.instrs += [ir]

    def getIR(self):
        return IRProg(self.instrs)

    def __call__(self, ir:IRInstr):
        self.emit(ir)

