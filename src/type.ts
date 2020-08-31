export enum Type {
    Int = "int",
}

/** 函数的类型 */
export class FuncType {
    /** 各参数的类型 */
    params: Type[];
    /** 返回值的类型 */
    ret: Type;

    constructor(params: Type[], ret: Type) {
        this.params = params;
        this.ret = ret;
    }

    equal(rhs: FuncType): boolean {
        if (this.ret !== rhs.ret || this.params.length != rhs.params.length) {
            return false;
        }
        for (let i = 0; i < this.params.length; i++) {
            if (this.params[i] !== rhs.params[i]) {
                return false;
            }
        }
        return true;
    }

    paramCount(): number {
        return this.params.length;
    }
}

export class Variable {
    /** 变量名 */
    name: string;
    /** 变量类型，目前只有整数类型 */
    type: Type;
    /** 如果是局部变量，表示变量在栈帧中的偏移量；如果是参数，表示第几个参数 */
    offset: number;
    /** 是否是参数 */
    isParam: boolean = false;

    constructor(name: string, type: Type, offset: number, isParam?: boolean) {
        this.name = name;
        this.type = type;
        this.offset = offset;
        if (isParam) {
            this.isParam = true;
        }
    }
}

export class Function {
    /** 函数名 */
    name: string;
    /** 函数类型 */
    type: FuncType;
    /** 是否已定义。如果一个函数只声明而未定义，该值为 false */
    defined: boolean = false;

    constructor(name: string, type: FuncType) {
        this.name = name;
        this.type = type;
    }
}
