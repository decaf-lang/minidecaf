from .instr import IRInstr

class IRProg:
    def __init__(self, instrs:[IRInstr]):
        self.instrs = instrs

    def __str__(self):
        f = lambda x: f"\t\t\t{x}" if type(x) is instr.Comment else f"{x}"
        return "main:\n\t" + '\n\t'.join(map(f, self.instrs))

class IREmitter:
    def __init__(self):
        self.instrs = []

    def emit(self, irs:[IRInstr]):
        self.instrs += irs

    def getIR(self):
        return IRProg(self.instrs)

    def __call__(self, irs:[IRInstr]):
        self.emit(irs)

