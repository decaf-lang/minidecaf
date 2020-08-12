MAX_INT = 2**63 - 1
MIN_INT = -2**63

class MiniDecafError(Exception):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, *kwargs)

def text(x):
    if type(x) is str:
        return x
    if x is not None:
        return str(x.getText())

def flatten(l):
    r = []
    for i in l:
        if type(i) is list:
            r += flatten(i)
        else:
            r += [i]
    return r

unaryOps = ['-', '!', '~']
unaryOpStrs = ["neg", 'lnot', "not"]
strOfUnaryOp = {o: s for (o, s) in zip(unaryOps, unaryOpStrs)}

binaryOps = ['+', '-', '*', '/', '%', "==", "!=", "<", "<=", ">", ">=", "&&", "||"]
binaryOpStrs = ["add", "sub", "mul", "div", "rem", "eq", "ne", "lt", "le", "gt", "ge", "land", "lor"]
strOfBinaryOp = {o: s for (o, s) in zip(binaryOps, binaryOpStrs)}
