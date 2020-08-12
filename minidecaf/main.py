import sys
from antlr4 import *
from .generated.MiniDecafLexer import MiniDecafLexer
from .generated.MiniDecafParser import MiniDecafParser


def parseArgs(argv):
    if len(argv) not in {2,3}:
        print(f"Usage: {argv[0]} infile [outfile]")
        exit(1)
    infile = argv[1]
    if len(argv) == 3:
        outfile = argv[2]
    else:
        outfile = argv[1]
        if outfile.endswith(".c"):
            outfile = f"{outfile[:-2]}.s"
        else:
            outfile += ".s"
    return (infile, outfile)


def main():
    infile, outfile = parseArgs(sys.argv)

    input_stream = FileStream(infile)
    lexer = MiniDecafLexer(input_stream)
    token_stream = CommonTokenStream(lexer)
    parser = MiniDecafParser(token_stream)
    tree = parser.prog()
    print(tree.toStringTree(recog=parser))
    # Next: visitor & asmgen to outfile
