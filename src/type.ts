import { FuncContext } from "./gen/MiniDecafParser";

/** 一个整数或指针所占的字节数 */
export const WORD_SIZE = 4;

export interface Type {
    /** 与 `rhs` 的类型是否相等 */
    equal(rhs: Type): boolean;
    /** 所占内存的大小 */
    sizeof(): number;
    /** 引用后的类型 */
    ref(): Type;
    /** 解引用后的类型 */
    deref(): Type;
}

/** 基本类型（只有整数类型） */
export class BaseType implements Type {
    private readonly name: string;
    private constructor(name: string) {
        this.name = name;
    }
    toString = (): string => {
        return this.name;
    };

    sizeof(): number {
        return WORD_SIZE;
    }
    equal(rhs: Type): boolean {
        if (rhs instanceof BaseType) {
            return this.name === rhs.name;
        } else {
            return false;
        }
    }
    ref(): Type {
        return new PointerType(this);
    }
    deref(): Type {
        return undefined;
    }

    /** 整数类型 */
    static Int = new BaseType("int");
}

/** 指针类型 */
export class PointerType implements Type {
    /** 指针基类型 */
    private readonly base: BaseType | PointerType;
    constructor(base: BaseType | PointerType) {
        this.base = base;
    }
    toString = (): string => {
        return this.base.toString() + "*";
    };

    sizeof(): number {
        return WORD_SIZE;
    }
    equal(rhs: Type): boolean {
        if (rhs instanceof PointerType) {
            return this.base.equal(rhs.base);
        } else {
            return false;
        }
    }
    ref(): Type {
        return new PointerType(this);
    }
    deref(): Type {
        return this.base;
    }
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
        if (!this.ret.equal(rhs.ret) || this.params.length != rhs.params.length) {
            return false;
        }
        for (let i = 0; i < this.params.length; i++) {
            if (!this.params[i].equal(rhs.params[i])) {
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
    /** 变量类型 */
    type: Type;
    /** 如果是局部变量，表示变量在栈帧中的偏移量；如果是参数，表示第几个参数 */
    offset: number;
    /** 是否是参数 */
    isParam: boolean = false;
    /** 是否是全局变量 */
    isGlobal: boolean = false;

    /** 变量的种类。"g" 表示全局变量，"l" 表示局部变量，"p" 表示参数 */
    get kind(): string {
        return this.isGlobal ? "g" : this.isParam ? "p" : "l";
    }

    constructor(name: string, type: Type, offset: number, isParam?: boolean, isGlobal?: boolean) {
        this.name = name;
        this.type = type;
        this.offset = offset;
        if (isParam) {
            this.isParam = true;
        }
        if (isGlobal) {
            this.isGlobal = true;
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
