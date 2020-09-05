"""Deployment on web with brython:

1. Copy minidecaf/minidecaf to brython project top

2. Make workarounds to generated/MiniDecafParser.py (if #1488 is not yet fixed).

3. Driver code:
    from minidecaf import main as md_main
    from minidecaf.webutils import *

    def main():
        with MockStdout() as stdout, MockStderr() as stderr:
            stage = ... # read from form widget
            input = ... # read from form widget
            md_main(MockArgs(stage, input))
            return (stdout.text, stderr.text) # Display them on some element
"""

import sys
if "brython" in sys.version.lower():
    # Global definition for web minidecaf
    import sys

    class MockArgs:
        def __init__(self, stage, input="int main(){return 0;}"):
            self.infile = None
            self.outfile = None
            self.lex = (stage == "lex")
            self.parse = (stage == "parse")
            self.ni = (stage == "ni")
            self.ty = (stage == "ty")
            self.ir = (stage == "ir")
            self.input = input
            self.backtrace = False

    class MockStdout:
        def __init__(self):
            self.text = ""
        def __enter__(self):
            self._stdout = sys.stdout
            sys.stdout = self
            return self
        def write(self, s):
            self.text += s
        def __exit__(self, _a, _b, _c):
            sys.stdout = self._stdout

    class MockStderr:
        def __init__(self):
            self.text = ""
        def __enter__(self):
            self._stderr = sys.stderr
            sys.stderr = self
            return self
        def write(self, s):
            self.text += s
        def __exit__(self, _a, _b, _c):
            sys.stderr = self._stderr

