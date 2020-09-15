import { Command } from "commander";
import { version } from "../package.json";
import MiniDecaf = require("./minidecaf");
import fs from "fs";

const program = new Command();

program
    .version(version)
    .name("minidecaf")
    .arguments("<input_file>")
    .action((file) => {
        program.input = file;
    })
    .option("-s, --asm", "generate the RISC-V assembly code instead of executing")
    .option("-o, --output <output_file>", "save the output to file")
    .option("-d, --debug", "debug mode");

program.parse(process.argv);

let input = fs.readFileSync(program.input).toString();
let option: MiniDecaf.CompilerOption = {
    target: program.asm ? MiniDecaf.CompilerTarget.Riscv32Asm : MiniDecaf.CompilerTarget.Executed,
};

try {
    let output = MiniDecaf.compile(input, option);
    if (program.output) {
        fs.writeFileSync(program.output, output);
    } else {
        console.log(output);
    }
} catch (err) {
    console.error(program.debug ? err.stack : err.message);
}
