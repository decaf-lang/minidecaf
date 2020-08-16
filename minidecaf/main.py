import sys
import argparse
from antlr4 import *

from .utils import *
from .generated.MiniDecafLexer import MiniDecafLexer
from .generated.MiniDecafParser import MiniDecafParser
from .frontend import irGen, nameGen, typeCheck
from .asm import asmGen


def parseArgs(argv):
    parser = argparse.ArgumentParser(description="MiniDecaf compiler")
    parser.add_argument("infile", type=str,
                       help="the input C file")
    parser.add_argument("outfile", type=str, nargs="?",
                       help="the output assembly file")
    parser.add_argument("-ir", action="store_true", help="emit ir rather than asm")
    parser.add_argument("-ni", action="store_true", help="emit result of name resulution")
    parser.add_argument("-ty", action="store_true", help="emit type check information")
    parser.add_argument("-backtrace", action="store_true", help="emit backtrace information (for debugging)")
    return parser.parse_args()


def NameGen(tree):
    nameInfo = nameGen(tree)
    if args.ni:
        print(nameInfo)
        exit(0)
    return nameInfo


def TypeCheck(tree, nameInfo):
    typeInfo = typeCheck(tree, nameInfo)
    if args.ty:
        print(typeInfo)
        exit(0)
    return typeInfo


def IRGen(tree, nameInfo, typeInfo):
    ir = irGen(tree, nameInfo, typeInfo)
    if args.ir:
        print(ir)
        exit(0)
    return ir


def AsmGen(ir, outfile):
    return asmGen(ir, outfile)


def main():
    try:
        global args
        args = parseArgs(sys.argv)
        inputStream = FileStream(args.infile)
        lexer = MiniDecafLexer(inputStream)
        tokenStream = CommonTokenStream(lexer)
        parser = MiniDecafParser(tokenStream)
        tree = parser.prog()
        nameInfo = NameGen(tree)
        typeInfo = TypeCheck(tree, nameInfo)
        ir = IRGen(tree, nameInfo, typeInfo)
        AsmGen(ir, args.outfile)
        return 0
    except MiniDecafError as e:
        if args.backtrace:
            raise e
        print(e, file=sys.stderr)
        return 1
