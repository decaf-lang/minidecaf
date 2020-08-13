class AsmCommand:
    def __init__(self, s):
        self.s = s

    def __repr__(self):
        return self.__str__()


class AsmLabel(AsmCommand):
    def __str__(self):
        return f"{self.s}:"

class AsmInstr(AsmCommand):
    def __str__(self):
        return f"\t{self.s}"

class AsmDirective(AsmCommand):
    def __str__(self):
        return f"\t{self.s}"

class AsmComment(AsmCommand):
    def __str__(self):
        return f"\t\t\t\t# {self.s}"

class AsmBlank(AsmCommand):
    def __init__(self):
        pass

    def __str__(self):
        return f""

