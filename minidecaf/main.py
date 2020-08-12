import sys
import argparse
from antlr4 import *

from .generated.MiniDecafLexer import MiniDecafLexer
from .generated.MiniDecafParser import MiniDecafParser
from .ir import IREmitter
from .ir.irgen import StackIRGen
from .asm import AsmEmitter
from .asm.riscv import RISCVAsmGen as AsmGen


def parseArgs(argv):
    parser = argparse.ArgumentParser(description="MiniDecaf compiler")
    parser.add_argument("infile", type=str,
                       help="the input C file")
    parser.add_argument("outfile", type=str, nargs="?",
                       help="the output assembly file")
    parser.add_argument("-ir", action="store_true", help="emit ir rather than asm")
    return parser.parse_args()


def irGenerator(tree):
    irEmitter = IREmitter()
    StackIRGen(irEmitter).visit(tree)
    return irEmitter.getIR()


def asmGenerator(ir, outfile):
    asmEmitter = AsmEmitter(outfile)
    AsmGen(asmEmitter).gen(ir)
    asmEmitter.close()


def main():
    args = parseArgs(sys.argv)
    inputStream = FileStream(args.infile)
    lexer = MiniDecafLexer(inputStream)
    tokenStream = CommonTokenStream(lexer)
    parser = MiniDecafParser(tokenStream)
    tree = parser.prog()
    ir = irGenerator(tree)
    if args.ir:
        print(ir)
    else:
        asmGenerator(ir, args.outfile)
