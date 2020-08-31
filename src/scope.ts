import { Type, Variable } from "./type";

/** 作用域 */
export class Scope {
    /** 符号表 */
    private symbols: Map<string, Variable> = new Map();
    /** 局部变量所占的内存大小 */
    localVarSize: number = 0;

    /** 在该作用域中定义一个变量 */
    declareVar(name: string, type: Type): Variable {
        let v = new Variable(name, type, this.localVarSize);
        this.localVarSize += 4;
        this.symbols.set(name, v);
        return v;
    }

    /** 在该作用域中查找名为 `name` 的变量 */
    find(name: string): Variable {
        return this.symbols.get(name);
    }
}
