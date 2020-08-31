import { OtherError } from "./error";

/** 标签 */
export class Label {
    /** 标签编号 */
    readonly id: number;
    private constructor(id: number) {
        this.id = id;
    }
    toString = (): string => {
        return `.L${this.id}`;
    };

    private static labelCount: number = 1;
    /** 初始化标签分配器 */
    static initAllocation() {
        this.labelCount = 1;
    }
    /** 新分配一个标签 */
    static alloc(): Label {
        return new Label(this.labelCount++);
    }
}

/** 对变量的操作 */
export enum VariableOp {
    /** 读变量的值到寄存器 */
    Load = "load",
    /** 将寄存器的值保存到变量 */
    Store = "store",
}

/** IR 指令名称 */
export enum IrInstrName {
    /** 标签 */
    LABEL = "LABEL",
    /** 加载一个立即数到 `r0` */
    IMM = "IMM",
    /** 对 `r0` 进行一元运算，结果存到 `r0` */
    UNARY = "UNARY",
    /** 计算二元运算，左右操作数分别是 `r1` 和 `r0`，结果存到 `r0` */
    BINARY = "BINARY",
    /** 读出变量的值，保存到 `r0` */
    LOAD_VAR = "LOAD_VAR",
    /** 将 `r0` 保存到变量 */
    STORE_VAR = "STORE_VAR",
    /** 将给定的寄存器压入栈顶 */
    PUSH = "PUSH",
    /** 从栈顶弹出一个数，存到给定的寄存器 */
    POP = "POP",
    /** 无条件跳转到给定的标签 */
    JUMP = "JUMP",
    /** 如果 `r0` 为 0，跳转到给定的标签 */
    BEQZ = "BEQZ",
    /** 如果 `r0` 不为 0，跳转到给定的标签 */
    BNEZ = "BNEZ",
    /** 函数调用指令 */
    CALL = "CALL",
    /** 返回指令，返回值为 `r0` */
    RET = "RET",
}

/** 一条 IR 指令。可拥有最多 2 个操作数 */
export class IrInstr {
    /** 指令名称 */
    readonly name: IrInstrName;
    /** 第一个操作数 */
    readonly op: any;
    /** 第二个操作数 */
    readonly op2: any;

    constructor(name: IrInstrName, op: any = undefined, op2: any = undefined) {
        this.name = name;
        this.op = op;
        this.op2 = op2;
    }

    toString = (): string => {
        return (
            this.name +
            (this.op !== undefined ? ` ${this.op}` : "") +
            (this.op2 !== undefined ? `, ${this.op2}` : "")
        );
    };
}

/** IR 函数，保存了函数的基本信息 */
export class IrFunc {
    /** 函数名 */
    name: string;
    /** 参数个数 */
    paramCount: number;
    /** 局部变量所占的内存大小，用于计算栈帧大小 */
    localVarSize: number;
    /** 指令列表 */
    instrs: IrInstr[];
    /** 标签号到指令位置的映射表 */
    labelIndices: Map<number, number>;

    constructor(name: string, paramCount: number, localVarSize: number) {
        this.name = name;
        this.paramCount = paramCount;
        this.localVarSize = localVarSize;
        this.instrs = [];
        this.labelIndices = new Map();
    }

    toString = (): string => {
        let str = `FUNC ${this.name}(paramCount=${this.paramCount}, localVarSize=${this.localVarSize}):\n`;
        this.instrs.forEach((i) => {
            str += i.name == IrInstrName.LABEL ? `${i}:\n` : `    ${i}\n`;
        });
        return str;
    };
}

/** 中间表示。该 IR 拥有一个栈和两个寄存器 `r0`, `r1` */
export class Ir {
    /** 函数名到 {@link IrFunc} 的映射表 */
    private _funcs: Map<string, IrFunc> = new Map();
    /** 当前所在的函数 */
    private currentFunc: IrFunc;

    /** 获取所有 IR 函数 */
    get funcs(): Map<string, IrFunc> {
        return this._funcs;
    }

    /** 给当前函数生成一条 IR 指令 */
    private emit(instr: IrInstr) {
        this.currentFunc.instrs.push(instr);
    }

    /** 格式化输出 */
    toString = (): string => {
        let str = "";
        this._funcs.forEach((func) => {
            str += func.toString() + "\n";
        });
        return str;
    };

    /**
     * 新建一个函数。
     *
     * @param name 函数名
     * @param paramCount 参数个数
     * @param localVarSize 局部变量所占的内存大小
     */
    newFunc(name: string, paramCount: number, localVarSize: number) {
        let f = new IrFunc(name, paramCount, localVarSize);
        this._funcs.set(name, f);
        this.currentFunc = f;
    }

    /** 当前函数是否缺少 `RET` 指令，即最后一条指令是不是 `RET` */
    missReturn(): boolean {
        return (
            this.currentFunc &&
            this.currentFunc.instrs[this.currentFunc.instrs.length - 1]?.name !== IrInstrName.RET
        );
    }

    /** 新建一个标签 */
    emitLabel(label: Label) {
        this.currentFunc.labelIndices.set(label.id, this.currentFunc.instrs.length);
        this.emit(new IrInstr(IrInstrName.LABEL, label));
    }

    /** 加载一个立即数 `value` 到 `r0` */
    emitImmediate(value: number) {
        this.emit(new IrInstr(IrInstrName.IMM, value));
    }

    /** 对 `r0` 进行一元运算，结果存到 `r0` */
    emitUnary(op: string) {
        this.emit(new IrInstr(IrInstrName.UNARY, op));
    }

    /** 计算二元运算，左右操作数分别是 `r1` 和 `r0`，结果存到 `r0` */
    emitBinary(op: string) {
        this.emit(new IrInstr(IrInstrName.BINARY, op));
    }

    /** 从 `offset` 处读出变量的值，保存到 `r0`。
     *
     * @param offset 变量在栈帧中的偏移量
     * @param varKind 变量种类，"l" 表示局部变量，"p" 表示参数
     */
    emitLoadVar(offset: number, varKind: string) {
        this.emit(new IrInstr(IrInstrName.LOAD_VAR, offset, varKind));
    }

    /** 将 `r0` 保存到位于 `offset` 的变量。
     *
     * @param offset 变量在栈帧中的偏移量
     * @param varKind 变量种类，"l" 表示局部变量，"p" 表示参数
     */
    emitStoreVar(offset: number, varKind: string) {
        this.emit(new IrInstr(IrInstrName.STORE_VAR, offset, varKind));
    }

    /** 将给定的寄存器 `reg` 压入栈顶 */
    emitPush(reg: string) {
        this.emit(new IrInstr(IrInstrName.PUSH, reg));
    }

    /** 从栈顶弹出一个数，存到给定的寄存器 `reg` */
    emitPop(reg: string) {
        this.emit(new IrInstr(IrInstrName.POP, reg));
    }

    /** 无条件跳转到 `label` */
    emitJump(label: Label) {
        this.emit(new IrInstr(IrInstrName.JUMP, label));
    }

    /** 如果 `r0` 为 0，跳转到 `label` */
    emitBeqz(label: Label) {
        this.emit(new IrInstr(IrInstrName.BEQZ, label));
    }

    /** 如果 `r0` 不为 0，跳转到 `label` */
    emitBnez(label: Label) {
        this.emit(new IrInstr(IrInstrName.BNEZ, label));
    }

    /** 函数调用指令。调用之前所有参数已被从右到左依次压入栈中。
     *
     * @param fnName 函数名
     * @param argCount 参数个数
     */
    emitCall(fnName: string, argCount: number) {
        this.emit(new IrInstr(IrInstrName.CALL, fnName, argCount));
    }

    /** 返回指令，返回值为 `r0` */
    emitReturn() {
        this.emit(new IrInstr(IrInstrName.RET));
    }
}

/** 遍历 IR 时所需的基类 */
export abstract class IrVisitor<Result> {
    protected readonly ir: Ir;
    constructor(ir: Ir) {
        this.ir = ir;
    }

    /** 处理标签指令 `LABEL` */
    abstract visitLabel(instr: IrInstr): any;
    /** 处理立即数指令 `IMM` */
    abstract visitImmediate(instr: IrInstr): any;
    /** 处理一元运算指令 `UNARY` */
    abstract visitUnary(instr: IrInstr): any;
    /** 处理二元运算指令 `BINARY` */
    abstract visitBinary(instr: IrInstr): any;
    /** 处理读取变量指令 `LOAD_VAR` */
    abstract visitLoadVar(instr: IrInstr): any;
    /** 处理保存变量指令 `STORE_VAR` */
    abstract visitStoreVar(instr: IrInstr): any;
    /** 处理压栈指令 `PUSH` */
    abstract visitPush(instr: IrInstr): any;
    /** 处理弹栈指令 `POP` */
    abstract visitPop(instr: IrInstr): any;
    /** 处理无条件跳转指令 `JUMP` */
    abstract visitJump(instr: IrInstr): any;
    /** 处理条件跳转指令 `BEQZ` */
    abstract visitBeqz(instr: IrInstr): any;
    /** 处理条件跳转指令 `BNEZ` */
    abstract visitBnez(instr: IrInstr): any;
    /** 处理函数调用指令 `CALL` */
    abstract visitCall(instr: IrInstr): any;
    /** 处理返回指令 `RET` */
    abstract visitReturn(instr: IrInstr): any;

    /** 自定义的遍历过程 */
    abstract visitAll(): Result;

    /** 根据指令名称自动调用对应的 visit 函数 */
    protected visitInstr(i: IrInstr): any {
        switch (i.name) {
            case IrInstrName.LABEL:
                return this.visitLabel(i);
            case IrInstrName.IMM:
                return this.visitImmediate(i);
            case IrInstrName.UNARY:
                return this.visitUnary(i);
            case IrInstrName.BINARY:
                return this.visitBinary(i);
            case IrInstrName.LOAD_VAR:
                return this.visitLoadVar(i);
            case IrInstrName.STORE_VAR:
                return this.visitStoreVar(i);
            case IrInstrName.PUSH:
                return this.visitPush(i);
            case IrInstrName.POP:
                return this.visitPop(i);
            case IrInstrName.JUMP:
                return this.visitJump(i);
            case IrInstrName.BEQZ:
                return this.visitBeqz(i);
            case IrInstrName.BNEZ:
                return this.visitBnez(i);
            case IrInstrName.CALL:
                return this.visitCall(i);
            case IrInstrName.RET:
                return this.visitReturn(i);
            default:
                throw new OtherError(`unknown IR instruction '${i}'`);
        }
    }
}
