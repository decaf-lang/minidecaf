import { Type, FuncType, Variable, Function } from "./type";

/** 作用域种类 */
enum ScopeType {
    /** 全局作用域 */
    Global,
    /** 函数作用域 */
    Func,
    /** 局部作用域 */
    Local,
}

/** 作用域 */
export class Scope {
    /** 作用域名 */
    readonly name: string;
    /** 作用域种类，见 {@link ScopeType} */
    private readonly type: ScopeType;
    /** 符号表 */
    private symbols: Map<string, Variable | Function>;
    /** 下一个变量在栈帧中的偏移量 */
    nextOffset: number = 0;
    /** 局部变量所占的内存大小 */
    localVarSize: number = 0;

    constructor(name: string, type: ScopeType) {
        this.name = name;
        this.type = type;
        this.symbols = new Map();
    }

    /** 在该作用域中定义一个变量 */
    declareVar(name: string, type: Type): Variable {
        let v = new Variable(name, type, this.nextOffset);
        this.nextOffset += 4;
        this.symbols.set(name, v);
        return v;
    }

    /** 在该作用域中定义一个参数 */
    declareParam(name: string, type: Type, index: number): Variable {
        let v = new Variable(name, type, index, true);
        this.symbols.set(name, v);
        return v;
    }

    /** 在该作用域中定义一个函数 */
    declareFunc(name: string, type: FuncType): Function {
        let f = new Function(name, type);
        this.symbols.set(name, f);
        return f;
    }

    /** 在该作用域中查找名为 `name` 的符号 */
    find(name: string): Variable | Function {
        return this.symbols.get(name);
    }

    isGlobal(): boolean {
        return this.type === ScopeType.Global;
    }

    isFunc(): boolean {
        return this.type == ScopeType.Func;
    }

    isLocal(): boolean {
        return this.type === ScopeType.Local;
    }

    static newGlobal(): Scope {
        return new Scope("GLOBAL", ScopeType.Global);
    }

    static newFunc(name: string): Scope {
        return new Scope(name, ScopeType.Func);
    }

    static newLocal(): Scope {
        return new Scope("LOCAL", ScopeType.Local);
    }
}

/** 多个嵌套的作用域构成的栈 */
export class ScopeStack {
    private scopes: Scope[] = [];

    /** 当前作用域，即最里层作用域 */
    private current(): Scope {
        return this.scopes[this.scopes.length - 1];
    }

    /** 当前的全局作用域 */
    currentGlobal(): Scope {
        return this.scopes[0];
    }

    /** 当前的函数作用域 */
    currentFunc(): Scope {
        return this.scopes[1];
    }

    /** 打开一个作用域 `scope`，会将其压入栈顶 */
    open(scope: Scope) {
        if (scope.isLocal()) {
            scope.nextOffset = this.current().nextOffset;
        }
        this.scopes.push(scope);
    }

    /** 关闭最里层的作用域，即从栈顶弹出 */
    close() {
        let scope = this.scopes.pop();
        if (scope.isLocal()) {
            let func = this.currentFunc();
            func.localVarSize = Math.max(func.localVarSize, scope.nextOffset);
        }
    }

    /** 当前作用域是否可以定义名为 `name` 的符号 */
    canDeclare(name: string): boolean {
        if (this.current().find(name)) {
            return false;
        }
        // 如果上层作用域是函数作用域，且与其中的符号同名（参数），不允许定义
        if (this.scopes.length > 1) {
            let funcScope = this.scopes[this.scopes.length - 2];
            if (funcScope.isFunc() && funcScope.find(name)) {
                return false;
            }
        }
        return true;
    }

    /** 在当前作用域中定义一个变量 */
    declareVar(name: string, type: Type): Variable {
        return this.current().declareVar(name, type);
    }

    /** 在当前作用域中定义一个参数。
     *
     * @param name 参数名
     * @param type 参数类型
     * @param index 第几个参数
     */
    declareParam(name: string, type: Type, index: number): Variable {
        return this.current().declareParam(name, type, index);
    }

    /** 在当前作用域中定义一个函数 */
    declareFunc(name: string, type: FuncType, defined?: boolean): Function {
        let f = this.current().declareFunc(name, type);
        if (defined) f.defined = true;
        return f;
    }

    /** 从里到外依次在各作用域中查找名为 `name` 的变量 */
    find(name: string): Variable | Function {
        for (let i = this.scopes.length - 1; i >= 0; i--) {
            let v = this.scopes[i].find(name);
            if (v) return v;
        }
        return null;
    }
}
