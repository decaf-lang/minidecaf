from .command import *

class AsmEmitter:
    def __init__(self, outfile:str):
        self.f = open(outfile, 'w')

    def emit(self, com:AsmCommand):
        print(f"{com}", file=self.f)

    def close(self):
        self.f.close()

    def __call__(self, coms:[AsmCommand]):
        for com in coms:
            self.emit(com)
