import { IrInstrName, IrInstr, IrVisitor } from "../ir";
import { RuntimeError, OtherError } from "../error";

/** 为了模拟 32 位计算机，所有运算结果都要按位与上该数，以截取低 32 位 */
const MAX_UINT = 0xffff_ffff;

/** 整数转布尔 */
function num2bool(val: number): boolean {
    return val !== 0;
}

/** 布尔转整数 */
function bool2num(val: boolean): number {
    return val ? 1 : 0;
}

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

/** 计算二元运算 */
function binaryOp(op: string, lhs: number, rhs: number): number {
    switch (op) {
        case "||":
            return bool2num(num2bool(lhs) || num2bool(rhs));
        case "&&":
            return bool2num(num2bool(lhs) && num2bool(rhs));
        case "==":
            return bool2num(lhs === rhs);
        case "!=":
            return bool2num(lhs !== rhs);
        case "<":
            return bool2num(lhs < rhs);
        case ">":
            return bool2num(lhs > rhs);
        case "<=":
            return bool2num(lhs <= rhs);
        case ">=":
            return bool2num(lhs >= rhs);
        case "+":
            return (lhs + rhs) & MAX_UINT;
        case "-":
            return (lhs - rhs) & MAX_UINT;
        case "*":
            return (lhs * rhs) & MAX_UINT;
        case "/":
            if (rhs === 0) {
                throw new RuntimeError("divide by zero");
            }
            return Math.trunc(lhs / rhs); // 截断取整
        case "%":
            if (rhs === 0) {
                throw new RuntimeError("divide by zero");
            }
            return lhs % rhs;
        default:
            throw new OtherError(`unknown binary operator '${op}'`);
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

    visitBinary(instr: IrInstr) {
        this.pc++;
        this.r0 = binaryOp(instr.op, this.r1, this.r0);
    }

    visitPush(instr: IrInstr) {
        this.pc++;
        this.stack.push(this[instr.op]);
    }

    visitPop(instr: IrInstr) {
        this.pc++;
        this[instr.op] = this.stack.pop();
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
