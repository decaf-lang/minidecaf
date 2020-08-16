from ..utils import *

class Type:
    def __repr__(self):
        return self.__str__()

    def sizeof(self):
        raise Exception("abstract type is unsized")


class VoidType:
    def __init__(self):
        pass

    def __str__(self):
        return "void"

    def __eq__(self, other):
        return isinstance(other, VoidType)


class IntType(Type):
    def __init__(self):
        pass

    def __str__(self):
        return "int"

    def __eq__(self, other):
        return isinstance(other, IntType)

    def sizeof(self):
        return INT_BYTES


class PtrType(Type):
    def __init__(self, base:Type):
        self.base = base

    def __str__(self):
        return f"{self.base}*"

    def __eq__(self, other):
        if not isinstance(other, PtrType):
            return False
        return self.base == other.base

    def sizeof(self):
        return INT_BYTES


class ArrayType(Type):
    def __init__(self, base:Type, len:int):
        self.base = base
        self.len = len

    def __str__(self):
        return f"[{self.len}]{self.base}"

    def __eq__(self, other):
        if not isinstance(other, ArrayType):
            return False
        return self.base == other.base and self.len == other.len

    def make(base:Type, dims:list):
        for len in dims:
            base = ArrayType(base, len)
        return base

    def sizeof(self):
        return self.base.sizeof() * self.len


class ZeroType(IntType, PtrType):
    def __init__(self):
        pass

    def __str__(self):
        return "zerotype"

    def __eq__(self, other):
        return isinstance(other, IntType) or isinstance(other, PtrType)


def TypeRule(f):
    """A type rule is a function: (ctx, *inputTypes) -> {outputType | errStr | None}.
    The ctx parameter is only used for error reporting."""
    def g(ctx, *inTy):
        res = f(ctx, *inTy)
        if type(res) is str:
            raise MiniDecafTypeError(ctx, f"{f.__name__}: {res}")
        if res is None:
            raise MiniDecafTypeError(ctx, f"{f.__name__}: type error")
        return res
    g.__name__ = f.__name__ # black magic
    return g


def tryEach(name="tryEach", *fs):
    """Combine multiple type rules `fs`, returns result the first that does not fail."""
    @TypeRule
    def g(ctx, *inTy):
        errs = []
        for f in fs:
            try:
                return f(ctx, *inTy)
            except MiniDecafTypeError as e:
                errs += [e.msg]
        return f"{name}:\n\t" + '\n\t'.join(map(str, errs))
    g.__name__ = name # black magic
    return g


@TypeRule
def condRule(ctx, cond, tr, fal):
    if cond == IntType() and tr == fal:
        return tr

@TypeRule
def intBinopRule(ctx, lhs, rhs):
    if lhs == IntType() and rhs == IntType():
        return IntType()
    return f"integer expected, got {lhs} and {rhs}"

@TypeRule
def intUnaopRule(ctx, ty):
    if ty == IntType():
        return IntType()
    return f"integer expected, got {ty}"

@TypeRule
def ptrArithRule(ctx, lhs, rhs):
    if lhs == IntType() and isinstance(rhs, PtrType):
        return rhs
    if rhs == IntType() and isinstance(lhs, PtrType):
        return lhs
    return f"pointer and integer, got {lhs} and {rhs}"

@TypeRule
def ptrDiffRule(ctx, lhs, rhs):
    if lhs == rhs and isinstance(rhs, PtrType):
        return IntType()
    return f"two pointers of the same type, got {lhs} and {rhs}"

@TypeRule
def derefRule(ctx, ty):
    if isinstance(ty, PtrType):
        return ty.base
    return f"pointer expected, got {ty}"

@TypeRule
def addrofRule(ctx, ty):
    return PtrType(ty)

@TypeRule
def eqrelRule(ctx, lhs, rhs):
    if lhs != rhs:
        return f"cannot equate or compare {lhs} to {rhs}"
    if lhs != IntType() and not isinstance(lhs, PtrType):
        return f"expected integer or pointer types, found {lhs}"
    return IntType()

@TypeRule
def asgnRule(ctx, lhs, rhs):
    if lhs != rhs:
        return f"cannot assign {rhs} to {lhs}"
    return lhs

@TypeRule
def retRule(ctx, funcRetTy, ty):
    if funcRetTy != ty:
        return f"return {funcRetTy} expected, {ty} found"
    return VoidType()

@TypeRule
def stmtCondRule(ctx, ty):
    if ty != IntType():
        return f"integer expected, {ty} found"
    return VoidType()


@TypeRule
def arrayRule(ctx, arr, idx):
    if not isinstance(arr, ArrayType) and not isinstance(arr, PtrType):
        return f"array/pointer expected, {arr} found"
    if idx != IntType():
        return f"index must be an integer, {idx} found"
    return arr.base
