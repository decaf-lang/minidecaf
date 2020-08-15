import sys
import argparse
from antlr4 import *

from .generated.MiniDecafLexer import MiniDecafLexer
from .generated.MiniDecafParser import MiniDecafParser
from .frontend import irGen, nameGen
from .asm import asmGen


def parseArgs(argv):
    parser = argparse.ArgumentParser(description="MiniDecaf compiler")
    parser.add_argument("infile", type=str,
                       help="the input C file")
    parser.add_argument("outfile", type=str, nargs="?",
                       help="the output assembly file")
    parser.add_argument("-ir", action="store_true", help="emit ir rather than asm")
    parser.add_argument("-ni", action="store_true", help="emit result of name resulution")
    return parser.parse_args()


def NameGen(tree):
    nameInfo = nameGen(tree)
    if args.ni:
        print(nameInfo)
        exit(0)
    return nameInfo


def IRGen(tree, nameInfo):
    return irGen(tree, nameInfo)
    if args.ir:
        print(ir)
        exit(0)
    return ir


def AsmGen(ir, outfile):
    return asmGen(ir, outfile)


def main():
    global args
    args = parseArgs(sys.argv)
    inputStream = FileStream(args.infile)
    lexer = MiniDecafLexer(inputStream)
    tokenStream = CommonTokenStream(lexer)
    parser = MiniDecafParser(tokenStream)
    tree = parser.prog()
    nameInfo = NameGen(tree)
    ir = IRGen(tree, nameInfo)
    AsmGen(ir, args.outfile)
