from .instr import IRInstr

class IRProg:
    def __init__(self, instrs:[IRInstr]):
        self.instrs = instrs

    def __str__(self):
        def f(i):
            if type(i) is instr.Comment:
                return f"\t\t\t\t{i}"
            if type(i) is instr.Label:
                return f"{i}"
            return f"\t{i}"
        return "main:\n" + '\n'.join(map(f, self.instrs))


class IREmitter:
    def __init__(self):
        self.instrs = []

    def emit(self, irs:[IRInstr]):
        self.instrs += irs

    def getIR(self):
        return IRProg(self.instrs)

    def __call__(self, irs:[IRInstr]):
        self.emit(irs)

