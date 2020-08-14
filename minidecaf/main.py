import sys
import argparse
from antlr4 import *

from .generated.MiniDecafLexer import MiniDecafLexer
from .generated.MiniDecafParser import MiniDecafParser
from .ir import IREmitter
from .ir.irgen import StackIRGen
from .ir.namer import Namer
from .asm import AsmEmitter
from .asm.riscv import RISCVAsmGen as AsmGen


def parseArgs(argv):
    parser = argparse.ArgumentParser(description="MiniDecaf compiler")
    parser.add_argument("infile", type=str,
                       help="the input C file")
    parser.add_argument("outfile", type=str, nargs="?",
                       help="the output assembly file")
    parser.add_argument("-ir", action="store_true", help="emit ir rather than asm")
    parser.add_argument("-ni", action="store_true", help="emit result of name resulution")
    return parser.parse_args()


def nameInfoGenerator(tree):
    namer = Namer()
    namer.visit(tree)
    nameInfo = namer.nameInfo
    if args.ni:
        print(nameInfo)
        exit(0)
    return nameInfo


def irGenerator(tree, nameInfo):
    irEmitter = IREmitter()
    StackIRGen(irEmitter, nameInfo).visit(tree)
    ir = irEmitter.getIR()
    if args.ir:
        print(ir)
        exit(0)
    else:
        return ir


def asmGenerator(ir, outfile):
    asmEmitter = AsmEmitter(outfile)
    AsmGen(asmEmitter).gen(ir)
    asmEmitter.close()


def main():
    global args
    args = parseArgs(sys.argv)
    inputStream = FileStream(args.infile)
    lexer = MiniDecafLexer(inputStream)
    tokenStream = CommonTokenStream(lexer)
    parser = MiniDecafParser(tokenStream)
    tree = parser.prog()
    nameInfo = nameInfoGenerator(tree)
    ir = irGenerator(tree, nameInfo)
    asmGenerator(ir, args.outfile)
