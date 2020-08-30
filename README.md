# MiniDecaf Web

Web version of the [MiniDecaf](https://github.com/decaf-lang/minidecaf) compiler. Implemented in TypeScript.

## Getting Started

### Install Dependencies

```sh
npm install
```

### Run on Command Line

```sh
npm run grammar     # generate ANTLR4 grammar parser
npm run build
npm run cli <input_file> -- [options]
```

```
Options:
  -V, --version               output the version number
  -s, --asm                   generate the RISC-V assembly code instead of executing
  -r, --ir                    generate the intermediate representation (IR)
  -o, --output <output_file>  save the output to file
  -d, --debug                 debug mode
  -h, --help                  display help for command
```

### Build Website & Run in Browsers

Build JavaScript files for browsers:

```sh
npm run build-web
```

Start a simple HTTP server:

```sh
npm run serve
```

Visit http://127.0.0.1:8080 to preview.

### Test

Test [one file](test/test.c):

```sh
npm run test    # or `npm run test-codegen`
```

Test all test cases in [test/minidecaf-tests](test/minidecaf-tests):

```sh
npm run test-all -- [options]
```

```
Options:
  -n <step>                   run all testcases starts from step1 to this step. default is `12`.
  -s                          generate the assembly code (RISC-V) and run in the simulator instead
                              of running as an interpreter.
```
