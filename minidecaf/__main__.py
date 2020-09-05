"""This module is invoked when minidecaf is run as a module.
When run on web, don't use this module, see webutils.py.
"""
import sys
import argparse
from .main import main
from .utils import *

assert not WEB_ENVIRONMENT

def parseArgs(argv):
    parser = argparse.ArgumentParser(description="MiniDecaf compiler")
    parser.add_argument("infile", type=str,
                       help="the input C file")
    parser.add_argument("outfile", type=str, nargs="?",
                       help="the output assembly file")
    parser.add_argument("-ir", action="store_true", help="emit ir rather than asm")
    parser.add_argument("-ni", action="store_true", help="emit result of name resulution")
    parser.add_argument("-ty", action="store_true", help="emit type check information")
    parser.add_argument("-lex", action="store_true", help="emit tokens produced by lexing")
    parser.add_argument("-parse", action="store_true", help="emit cst produced by parsing (use `make cst` for graphical view)")
    parser.add_argument("-backtrace", action="store_true", help="emit backtrace information (for debugging)")
    return parser.parse_args()

args = parseArgs(sys.argv)
sys.exit(main(args))
