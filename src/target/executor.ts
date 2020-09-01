import { VariableOp, IrInstr, IrFunc, IrVisitor, Ir } from "../ir";
import { RuntimeError, OtherError } from "../error";
import { WORD_SIZE } from "../type";

/** 为了模拟 32 位计算机，所有运算结果都要按位与上该数，以截取低 32 位 */
const MAX_UINT = 0xffff_ffff;
/** 模拟的内存大小 */
const MEMORY_SIZE = 0x8000_0000 / WORD_SIZE;
/** 此地址以上为全局数据段，此地址以下为栈空间 */
const STACK_OFFSET = MEMORY_SIZE / 2;

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
    /** 模拟栈指针 */
    private sp: number = 0;
    /** 模拟栈帧指针，即刚进入函数时的栈顶位置，用于计算局部变量的地址 */
    private fp: number = 0;
    /** 模拟内存，同时用作栈和全局数据。每个整数占据一个位置 */
    private memory: any[] = [];
    /** 全局变量名到地址的映射表 */
    private symbolAddr: Map<string, number> = new Map();
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
        this.memory.length = MEMORY_SIZE;
        this.sp = STACK_OFFSET;
    }

    /** 函数调用开始 */
    private callBegin(func: IrFunc) {
        this.push(this.currentFunc?.name); // 保存各种状态到栈
        this.push(this.pc);
        this.push(this.fp);
        this.currentFunc = func; // 更新当前函数
        this.pc = 0; // 函数的第一条指令位置是 0
        this.fp = this.sp * WORD_SIZE; // 保存当前栈顶位置
        this.sp -= func.localVarSize / WORD_SIZE; // 分配局部变量空间
    }

    /** 函数调用结束 */
    private callEnd() {
        this.sp = this.fp / WORD_SIZE; // 恢复栈大小，释放栈空间
        this.fp = this.pop(); // 从栈顶依次弹出各种保存的状态
        this.pc = this.pop() + 1;
        let oldFn = this.pop();
        this.sp += this.currentFunc.paramCount; // 释放参数空间
        if (this.currentFunc.name == "main") {
            this.halt = true;
        }
        this.currentFunc = this.ir.funcs.get(oldFn);
    }

    private loadData(addr: number): number {
        return this.memory[addr / WORD_SIZE] ?? 0;
    }

    private storeData(addr: number, value: number) {
        this.memory[addr / WORD_SIZE] = value;
    }

    private push(value: any) {
        this.memory[--this.sp] = value;
    }

    private pop(): any {
        return this.memory[this.sp++];
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
        let addr: number; // 变量在模拟内存中的绝对地址
        let isGlobal = false;
        switch (instr.op2) {
            case "g": // 全局变量
                isGlobal = true;
                addr = this.symbolAddr.get(instr.op);
                break;
            case "p": // 参数
                addr = this.fp + (instr.op + 3) * WORD_SIZE;
                break;
            case "l": // 局部变量
                addr = this.fp - this.currentFunc.localVarSize + instr.op;
                break;
            default:
                throw new OtherError(`invalid operand '${instr.op2}' of IR insruction '${instr}'`);
        }
        switch (op) {
            case VariableOp.Load:
                this.r0 = this.loadData(addr);
                break;
            case VariableOp.Store:
                this.storeData(addr, this.r0);
                break;
            case VariableOp.AddrOf:
                this.r0 = addr;
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

    visitLoad(instr: IrInstr) {
        this.pc++;
        this[instr.op] = this.loadData(this[instr.op2]);
    }

    visitStore(instr: IrInstr) {
        this.pc++;
        this.storeData(this[instr.op2], this[instr.op]);
    }

    visitLoadVar(instr: IrInstr) {
        this.variableOp(VariableOp.Load, instr);
    }

    visitStoreVar(instr: IrInstr) {
        this.variableOp(VariableOp.Store, instr);
    }

    visitAddrVar(instr: IrInstr) {
        this.variableOp(VariableOp.AddrOf, instr);
    }

    visitPush(instr: IrInstr) {
        this.pc++;
        this.push(this[instr.op]);
    }

    visitPop(instr: IrInstr) {
        this.pc++;
        this[instr.op] = this.pop();
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
        let addr = STACK_OFFSET * WORD_SIZE;
        this.ir.globals.forEach((data, name) => {
            this.symbolAddr.set(name, addr);
            this.storeData(addr, data.init ?? 0);
            addr += data.size;
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
