import { IrInstrName, IrInstr, IrVisitor } from "../ir";
import { OtherError } from "../error";

/** 为了模拟 32 位计算机，所有运算结果都要按位与上该数，以截取低 32 位 */
const MAX_UINT = 0xffff_ffff;

/** 计算一元运算 */
function unaryOp(op: string, factor: number): number {
    switch (op) {
        case "-":
            return -factor & MAX_UINT;
        case "~":
            return ~factor & MAX_UINT;
        case "!":
            return factor === 0 ? 1 : 0;
        default:
            throw new OtherError(`unknown unary operator '${op}'`);
    }
}

/** IR 解释执行器 */
export class IrExecutor extends IrVisitor<number> {
    /** 当前的 IR 指令位置 */
    private pc: number = 0;
    /** 寄存器 `r0` */
    private r0: number = 0;
    /** 寄存器 `r1` */
    private r1: number = 0;
    /** 栈。每个整数占据栈的一个位置 */
    private stack: number[] = [];

    visitImmediate(instr: IrInstr) {
        this.pc++;
        this.r0 = instr.op;
    }

    visitUnary(instr: IrInstr) {
        this.pc++;
        this.r0 = unaryOp(instr.op, this.r0);
    }

    visitReturn(_instr: IrInstr) {}

    visitAll(): number {
        let steps = 0;
        while (this.pc < this.ir.instrs.length) {
            let i = this.ir.instrs[this.pc];
            this.visitInstr(i);
            if (i.name == IrInstrName.RET) {
                return this.r0;
            }
            steps++;
        }
        return -1;
    }
}
