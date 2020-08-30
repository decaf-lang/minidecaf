/** IR 指令名称 */
export enum IrInstrName {
    /** 加载一个立即数到 `r0` */
    IMM = "IMM",
    /** 返回指令，返回值为 `r0` */
    RET = "RET",
}

/** 一条 IR 指令。可拥有 1 个操作数 */
export class IrInstr {
    /** 指令名称 */
    readonly name: IrInstrName;
    /** 第一个操作数 */
    readonly op: any;

    constructor(name: IrInstrName, op: any = undefined) {
        this.name = name;
        this.op = op;
    }

    toString = (): string => {
        return this.name + (this.op !== undefined ? ` ${this.op}` : "");
    };
}

/** 中间表示。该 IR 拥有一个栈和两个寄存器 `r0`, `r1` */
export class Ir {
    private _instrs: IrInstr[] = [];

    /** 获取所有 IR 指令 */
    get instrs(): IrInstr[] {
        return this._instrs;
    }

    /** 生成一条 IR 指令 */
    private emit(instr: IrInstr) {
        this._instrs.push(instr);
    }

    /** 格式化输出 */
    toString = (): string => {
        return this._instrs.join("\n");
    };

    /** 加载一个立即数 `value` 到 `r0` */
    emitImmediate(value: number) {
        this.emit(new IrInstr(IrInstrName.IMM, value));
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

    /** 处理立即数指令 `IMM` */
    abstract visitImmediate(instr: IrInstr): any;
    /** 处理返回指令 `RET` */
    abstract visitReturn(instr: IrInstr): any;

    /** 自定义的遍历过程 */
    abstract visitAll(): Result;

    /** 根据指令名称自动调用对应的 visit 函数 */
    protected visitInstr(i: IrInstr): any {
        switch (i.name) {
            case IrInstrName.IMM:
                return this.visitImmediate(i);
            case IrInstrName.RET:
                return this.visitReturn(i);
        }
    }
}
