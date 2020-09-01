import { Label, VariableOp, IrInstr, IrFunc, IrVisitor } from "../ir";
import { OtherError } from "../error";
import { WORD_SIZE } from "../type";

/** 参数寄存器个数 */
const ARG_REGS_COUNT = 8;

/** 将 IR 的寄存器名映射到 RISC-V 的寄存器名 */
function irReg2rvReg(irReg: string): string {
    return { r0: "t0", r1: "t1" }[irReg];
}

/**
 * 一元运算指令。
 *
 * @param op 运算符
 * @param rd 目标寄存器
 * @param rs 源寄存器
 */
function unaryOp(op: string, rd: string, rs: string): string {
    switch (op) {
        case "-":
            return `neg ${rd}, ${rs}`;
        case "~":
            return `not ${rd}, ${rs}`;
        case "!":
            return `seqz ${rd}, ${rs}`;
        default:
            throw new OtherError(`unknown unary operator '${op}'`);
    }
}

/**
 * 二元运算指令。
 *
 * @param op 运算符
 * @param rd 目标寄存器
 * @param rs1 源寄存器 1，左操作数
 * @param rs2 源寄存器 2，右操作数
 */
function binaryOp(op: string, rd: string, rs1: string, rs2: string): string | string[] {
    switch (op) {
        case "||":
            return [`or ${rd}, ${rs1}, ${rs2}`, `snez ${rd}, ${rd}`];
        case "&&":
            return [`snez ${rs1}, ${rs1}`, `snez ${rs2}, ${rs2}`, `and ${rd}, ${rs1}, ${rs2}`];
        case "==":
            return [`sub ${rd}, ${rs1}, ${rs2}`, `seqz ${rd}, ${rd}`];
        case "!=":
            return [`sub ${rd}, ${rs1}, ${rs2}`, `snez ${rd}, ${rd}`];
        case "<":
            return `slt ${rd}, ${rs1}, ${rs2}`;
        case ">":
            return `slt ${rd}, ${rs2}, ${rs1}`;
        case "<=":
            return [`slt ${rd}, ${rs2}, ${rs1}`, `xori ${rd}, ${rd}, 1`];
        case ">=":
            return [`slt ${rd}, ${rs1}, ${rs2}`, `xori ${rd}, ${rd}, 1`];
        case "+":
            return `add ${rd}, ${rs1}, ${rs2}`;
        case "-":
            return `sub ${rd}, ${rs1}, ${rs2}`;
        case "*":
            return `mul ${rd}, ${rs1}, ${rs2}`;
        case "/":
            return `div ${rd}, ${rs1}, ${rs2}`;
        case "%":
            return `rem ${rd}, ${rs1}, ${rs2}`;
        default:
            throw new OtherError(`unknown binary operator '${op}'`);
    }
}

/**
 * 从内存地址 `base + offset` 读出数据，存到寄存器 `rd`。
 *
 * @param rd 目标寄存器
 * @param base 基址寄存器（访问全局符号时不需要）
 * @param offset 偏移量或全局符号
 */
function load(rd: string, base: string, offset: number | string = 0): string {
    return typeof offset == "string" ? `lw ${rd}, ${offset}` : `lw ${rd}, ${offset}(${base})`;
}

/**
 * 将寄存器 `rs` 中的数据存到内存地址 `base + offset`。
 *
 * @param rs 源寄存器
 * @param base 基址寄存器（访问全局符号时不需要）
 * @param offset 偏移量或全局符号
 */
function store(rs: string, base: string, offset: number | string = 0): string {
    return typeof offset == "string" ? `sw ${rs}, ${offset}, t1` : `sw ${rs}, ${offset}(${base})`;
}

/** 调整栈指针，即 `sp += offset` */
function adjustStack(offset: number): string {
    if (offset) {
        return `addi sp, sp, ${offset}`;
    } else {
        return "";
    }
}

/** IR 到 RV32 的代码生成器 */
export class Riscv32CodeGen extends IrVisitor<string> {
    private asm: string = "";
    private currentFunc: IrFunc;

    /** 生成一个标签 */
    private emitLabel(label: string | Label) {
        this.asm += `${label}:\n`;
    }
    /** 生成一个指示符，如 .globl, .data, .word 等 */
    private emitDirective(directive: string) {
        this.asm += `    ${directive}\n`;
    }
    /** 生成一条或多条机器指令 */
    private emitInstr(instr: string | string[]) {
        if (instr.length > 0) {
            if (instr instanceof Array) {
                this.asm += `    ${instr.filter((i) => i.length > 0).join("\n    ")}\n`;
            } else {
                this.asm += `    ${instr}\n`;
            }
        }
    }

    /** 函数序言 */
    private emitPrologue(func: IrFunc) {
        // 栈帧大小为局部变量大小加 ra、fp 的大小加保存参数寄存器所需的大小
        let fsize = func.localVarSize + (Math.min(func.paramCount, ARG_REGS_COUNT) + 2) * WORD_SIZE;
        this.emitDirective(`.globl ${func.name}`);
        this.emitLabel(func.name);
        this.emitInstr(adjustStack(-fsize));
        this.emitInstr(store("ra", "sp", fsize - WORD_SIZE));
        this.emitInstr(store("fp", "sp", fsize - WORD_SIZE * 2));
        this.emitInstr(`addi fp, sp, ${fsize}`);
        // 将传参寄存器保存到栈上
        for (let i = 0; i < func.paramCount && i < ARG_REGS_COUNT; i++) {
            let offset = -(3 + i) * WORD_SIZE - this.currentFunc.localVarSize;
            this.emitInstr(store(`a${i}`, "fp", offset));
        }
    }

    /** 函数收尾 */
    private emitEpilogue(func: IrFunc) {
        let fsize = func.localVarSize + (Math.min(func.paramCount, ARG_REGS_COUNT) + 2) * WORD_SIZE;
        this.emitLabel(`${func.name}_exit`);
        this.emitInstr(load("ra", "sp", fsize - WORD_SIZE));
        this.emitInstr(load("fp", "sp", fsize - WORD_SIZE * 2));
        this.emitInstr(adjustStack(fsize));
        this.emitInstr("ret");
    }

    /** 生成对变量的操作指令，操作类型见 {@link VariableOp} */
    private emitVariableOp(op: VariableOp, instr: IrInstr): string {
        let base: string; // 基址寄存器（访问全局符号是不需要）
        let offset: number | string; // 相对于 `base` 的偏移量或全局符号
        switch (instr.op2) {
            case "g": // 全局变量
                offset = instr.op;
                break;
            case "p": // 参数
                base = "fp";
                offset =
                    instr.op < ARG_REGS_COUNT
                        ? // 前 8 个参数用寄存器传递，从当前函数栈帧中获取
                          -(3 + instr.op) * WORD_SIZE - this.currentFunc.localVarSize
                        : // 多余 8 个的参数，从上一函数栈帧中获取
                          (instr.op - ARG_REGS_COUNT) * WORD_SIZE;
                break;
            case "l": // 局部变量
                base = "fp";
                offset = -instr.op - 3 * WORD_SIZE;
                break;
            default:
                throw new OtherError(`invalid operand '${instr.op2}' of IR insruction '${instr}'`);
        }
        switch (op) {
            case VariableOp.Load:
                return load("t0", base, offset);
            case VariableOp.Store:
                return store("t0", base, offset);
            case VariableOp.AddrOf:
                return instr.op2 == "g" ? `la t0, ${offset}` : `addi t0, ${base}, ${offset}`;
            default:
                throw new OtherError(
                    `invalid variable operation '${op}' of IR insruction '${instr}'`,
                );
        }
    }

    visitLabel(instr: IrInstr) {
        this.emitLabel(instr.op);
    }

    visitImmediate(instr: IrInstr) {
        this.emitInstr(`li t0, ${instr.op}`);
    }

    visitUnary(instr: IrInstr) {
        this.emitInstr(unaryOp(instr.op, "t0", "t0"));
    }

    visitBinary(instr: IrInstr) {
        this.emitInstr(binaryOp(instr.op, "t0", "t1", "t0"));
    }

    visitLoad(instr: IrInstr) {
        this.emitInstr(load(irReg2rvReg(instr.op), irReg2rvReg(instr.op2)));
    }

    visitStore(instr: IrInstr) {
        this.emitInstr(store(irReg2rvReg(instr.op), irReg2rvReg(instr.op2)));
    }

    visitLoadVar(instr: IrInstr) {
        this.emitInstr(this.emitVariableOp(VariableOp.Load, instr));
    }

    visitStoreVar(instr: IrInstr) {
        this.emitInstr(this.emitVariableOp(VariableOp.Store, instr));
    }

    visitAddrVar(instr: IrInstr) {
        this.emitInstr(this.emitVariableOp(VariableOp.AddrOf, instr));
    }

    visitPush(instr: IrInstr) {
        this.emitInstr([adjustStack(-WORD_SIZE), store(irReg2rvReg(instr.op), "sp")]);
    }

    visitPop(instr: IrInstr) {
        this.emitInstr([load(irReg2rvReg(instr.op), "sp"), adjustStack(WORD_SIZE)]);
    }

    visitJump(instr: IrInstr) {
        this.emitInstr(`j ${instr.op}`);
    }

    visitBeqz(instr: IrInstr) {
        this.emitInstr(`beqz t0, ${instr.op}`);
    }

    visitBnez(instr: IrInstr) {
        this.emitInstr(`bnez t0, ${instr.op}`);
    }

    visitCall(instr: IrInstr) {
        let argCount = instr.op2; // 参数总数
        let regArgCount = Math.min(argCount, ARG_REGS_COUNT); // 使用寄存器传参的参数个数
        let stackArgCount = argCount - regArgCount; // 使用栈传参的参数个数
        for (let i = regArgCount - 1; i >= 0; i--) {
            // 将前 `regArgCount` 个参数依次存到 `a0`, `a1`, ... 寄存器
            this.emitInstr(load(`a${i}`, "sp", i * WORD_SIZE));
        }
        this.emitInstr(adjustStack(regArgCount * WORD_SIZE)); // 弹出已存到寄存器中的参数的空间
        this.emitInstr(`call ${instr.op}`);
        this.emitInstr("mv t0, a0");
        this.emitInstr(adjustStack(stackArgCount * WORD_SIZE)); // 释放参数空间
    }

    visitReturn(_instr: IrInstr) {
        this.emitInstr("mv a0, t0");
        this.emitInstr(`j ${this.currentFunc.name}_exit`); // `RET` 指令修改为跳到函数收尾
    }

    visitAll(): string {
        if (this.ir.globals.size) this.emitDirective(".data");
        this.ir.globals.forEach((data, name) => {
            this.emitDirective(`.globl ${name}`);
            this.emitDirective(".align 2");
            this.emitLabel(name);
            this.emitDirective(data.init ? `.word ${data.init}` : `.zero ${data.size}`);
            this.asm += "\n";
        });
        this.emitDirective(".text");
        this.ir.funcs.forEach((func) => {
            this.currentFunc = func;
            this.emitPrologue(func);
            for (let i of func.instrs) {
                this.visitInstr(i);
            }
            this.emitEpilogue(func);
            this.asm += "\n";
        });
        return this.asm;
    }
}
