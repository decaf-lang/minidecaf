import { IrInstrName, IrInstr, IrVisitor } from "../ir";

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
