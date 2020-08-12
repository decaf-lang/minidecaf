MAX_INT = 2**63 - 1
MIN_INT = -2**63

class MiniDecafError(Exception):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, *kwargs)

def text(x):
    if x is not None:
        return str(x.getText())

