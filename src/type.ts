export enum Type {
    Int = "int",
}

export class Variable {
    /** 变量名 */
    name: string;
    /** 变量类型，目前只有整数类型 */
    type: Type;
    /** 变量在栈帧中的偏移量 */
    offset: number;

    constructor(name: string, type: Type, offset: number) {
        this.name = name;
        this.type = type;
        this.offset = offset;
    }
}
