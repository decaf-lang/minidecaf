import { VariableOp, IrInstr, IrFunc, IrVisitor, Ir } from "../ir";
import { RuntimeError, OtherError } from "../error";
import { WORD_SIZE } from "../type";

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
    /** 模拟栈帧指针，即刚进入函数时的栈顶位置，用于计算局部变量的地址 */
    private fp: number = 0;
    /** 栈。每个整数占据栈的一个位置 */
    private stack: any[] = [];
    /** 全局变量名到值的映射表 */
    private globalData: Map<string, number> = new Map();
    /** 当前函数 */
    private currentFunc: IrFunc;
    /** 程序是否已终止，即 main 函数已返回 */
    private halt: boolean = false;

    /** 开始运行时的时间戳 */
    private startTime: number;
    /** 运行时间限制，单位秒 */
    private timeoutSecond: number;

    constructor(ir: Ir, timeoutSecond: number) {
        super(ir);
        this.startTime = Date.now();
        this.timeoutSecond = timeoutSecond;
    }

    /** 函数调用开始 */
    private callBegin(func: IrFunc) {
        this.stack.push(this.currentFunc?.name); // 保存各种状态到栈
        this.stack.push(this.pc);
        this.stack.push(this.fp);
        this.currentFunc = func; // 更新当前函数
        this.pc = 0; // 函数的第一条指令位置是 0
        this.fp = this.stack.length; // 保存当前栈顶位置
        this.stack.length += func.localVarSize / WORD_SIZE; // 分配局部变量空间
    }

    /** 函数调用结束 */
    private callEnd() {
        this.stack.length = this.fp; // 恢复栈大小，释放栈空间
        this.fp = this.stack.pop(); // 从栈顶依次弹出各种保存的状态
        this.pc = this.stack.pop() + 1;
        let oldFn = this.stack.pop();
        this.stack.length -= this.currentFunc.paramCount; // 释放参数空间
        if (this.currentFunc.name == "main") {
            this.halt = true;
        }
        this.currentFunc = this.ir.funcs.get(oldFn);
    }

    /** 检查是否超时 */
    private checkTimeout(): boolean {
        if (this.timeoutSecond) {
            return Date.now() - this.startTime > this.timeoutSecond * 1000;
        } else {
            return false;
        }
    }

    /** 处理对变量的操作，操作类型见 {@link VariableOp} */
    variableOp(op: VariableOp, instr: IrInstr) {
        this.pc++;
        let offset: number; // 对于非全局变量，相对于栈帧指针的偏移量
        let isGlobal = false;
        switch (instr.op2) {
            case "g": // 全局变量
                isGlobal = true;
                break;
            case "p": // 参数
                offset = -instr.op - 4;
                break;
            case "l": // 局部变量
                offset = instr.op / WORD_SIZE;
                break;
            default:
                throw new OtherError(`invalid operand '${instr.op2}' of IR insruction '${instr}'`);
        }
        switch (op) {
            case VariableOp.Load:
                // 全局变量直接从 `globalData` 而不是栈中获取，下同
                this.r0 = isGlobal ? this.globalData.get(instr.op) : this.stack[this.fp + offset];
                break;
            case VariableOp.Store:
                if (isGlobal) {
                    this.globalData.set(instr.op, this.r0);
                } else {
                    this.stack[this.fp + offset] = this.r0;
                }
                break;
            default:
                throw new OtherError(
                    `invalid variable operation '${op}' of IR insruction '${instr}'`,
                );
        }
    }

    visitLabel(_instr: IrInstr) {
        this.pc++;
    }

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

    visitLoadVar(instr: IrInstr) {
        this.variableOp(VariableOp.Load, instr);
    }

    visitStoreVar(instr: IrInstr) {
        this.variableOp(VariableOp.Store, instr);
    }

    visitPush(instr: IrInstr) {
        this.pc++;
        this.stack.push(this[instr.op]);
    }

    visitPop(instr: IrInstr) {
        this.pc++;
        this[instr.op] = this.stack.pop();
    }

    visitJump(instr: IrInstr) {
        this.pc = this.currentFunc.labelIndices.get(instr.op.id) + 1;
    }

    visitBeqz(instr: IrInstr) {
        if (this.r0 === 0) {
            this.pc = this.currentFunc.labelIndices.get(instr.op.id) + 1;
        } else {
            this.pc++;
        }
    }

    visitBnez(instr: IrInstr) {
        if (this.r0 !== 0) {
            this.pc = this.currentFunc.labelIndices.get(instr.op.id) + 1;
        } else {
            this.pc++;
        }
    }

    visitCall(instr: IrInstr) {
        let func = this.ir.funcs.get(instr.op);
        if (!func) {
            throw new RuntimeError(`call undefined function '${instr.op}'`);
        }
        this.callBegin(func);
    }

    visitReturn(_instr: IrInstr) {
        this.callEnd();
    }

    visitAll(): number {
        this.ir.globals.forEach((data, name) => {
            this.globalData.set(name, data.init ?? 0);
        });
        let steps = 0;
        let func = this.ir.funcs.get("main");
        if (!func) {
            throw new RuntimeError("no 'main' function");
        }
        this.callBegin(func);
        while (!this.checkTimeout()) {
            let i = this.currentFunc.instrs[this.pc];
            if (!i) {
                this.callEnd();
            } else {
                this.visitInstr(i);
            }
            if (this.halt) {
                return this.r0;
            }
            steps++;
        }
        throw new RuntimeError("time limit exceeded");
    }
}
