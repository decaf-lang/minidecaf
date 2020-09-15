# MiniDecaf Web

Web version of the [MiniDecaf](https://github.com/decaf-lang/minidecaf) compiler. Implemented in TypeScript.

## Getting Started

### Install Dependencies

```sh
npm install
```

### Run on Command Line

```sh
npm run build
npm run cli <input_file> -- [options]
```

```
Options:
  -V, --version               output the version number
  -s, --asm                   generate the RISC-V assembly code instead of executing
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
