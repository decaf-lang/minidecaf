import sys
from antlr4 import *

from .utils import *
from .generated import MiniDecafLexer, MiniDecafParser
from .frontend import irGen, nameGen, typeCheck
from .asm import asmGen


class PrematureDone(Exception):
    pass


def NameGen(tree):
    nameInfo = nameGen(tree)
    if args.ni:
        print(nameInfo)
        raise PrematureDone()
    return nameInfo


def TypeCheck(tree, nameInfo):
    typeInfo = typeCheck(tree, nameInfo)
    if args.ty:
        print(typeInfo)
        raise PrematureDone()
    return typeInfo


def IRGen(tree, nameInfo, typeInfo):
    ir = irGen(tree, nameInfo, typeInfo)
    if args.ir:
        print(ir)
        raise PrematureDone()
    return ir


def AsmGen(ir, outfile):
    if outfile is not None:
        with open(outfile, 'w') as fout:
            return asmGen(ir, fout)
    else:
        return asmGen(ir, sys.stdout)

def Lexer(inputStream):
    lexer = MiniDecafLexer(inputStream)
    if args.lex:
        dumpLexerTokens(lexer)
        raise PrematureDone()
    return CommonTokenStream(lexer)


def Parser(tokenStream):
    parser = MiniDecafParser(tokenStream)
    tree = parser.prog()
    if args.parse:
        print(tree.toStringTree(recog=parser))
        raise PrematureDone()
    return tree


def main(_args):
    try:
        global args
        args = _args

        if hasattr(args, 'input'):
            inputStream = InputStream(args.input)
        else:
            inputStream = FileStream(args.infile)
        tokenStream = Lexer(inputStream)
        tree = Parser(tokenStream)
        nameInfo = NameGen(tree)
        typeInfo = TypeCheck(tree, nameInfo)
        ir = IRGen(tree, nameInfo, typeInfo)
        AsmGen(ir, args.outfile)
        return 0
    except PrematureDone:
        return
    except MiniDecafError as e:
        if args.backtrace:
            raise e
        print(e, file=sys.stderr)
        return 1
