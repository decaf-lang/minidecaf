export enum CompilerTarget {
    Riscv32Asm = "riscv32-asm",
    Executed = "executed",
}

export class CompilerOption {
    target: CompilerTarget;
}

export function compile(input: string, option: CompilerOption): string {
    if (option.target === CompilerTarget.Riscv32Asm) {
        return "nop";
    } else if (option.target === CompilerTarget.Executed) {
        return "0";
    }
}
