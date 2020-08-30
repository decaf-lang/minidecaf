import { ANTLRInputStream, ANTLRErrorListener, CommonTokenStream } from "antlr4ts";
import { MiniDecafLexer } from "./gen/MiniDecafLexer";
import { MiniDecafParser } from "./gen/MiniDecafParser";
import { Riscv32CodeGen } from "./visitor/codegen";
import { Executor } from "./visitor/executor";
import { SyntaxError } from "./error";

export enum CompilerTarget {
    /** 编译到 RISC-V 32 汇编 */
    Riscv32Asm = "riscv32-asm",
    /** 作为解释器直接运行 */
    Executed = "executed",
}

export class CompilerOption {
    /** 编译目标 {@link CompilerTarget} */
    target: CompilerTarget;
}

/** 监听 ANTLR 的语法解析错误 */
class ErrorListener implements ANTLRErrorListener<any> {
    private errors: Error[] = [];
    syntaxError(
        _recognizer: any,
        _offendingSymbol: any,
        line: number,
        charPositionInLine: number,
        msg: string,
        _e: any,
    ) {
        this.errors.push(new SyntaxError(line, charPositionInLine, msg));
    }

    toString(): string {
        return this.errors.map((e) => e.message).join("\n");
    }
}

/** 编译器主入口。
 *
 * @param input 输入的源代码
 * @param option 编译选项
 * @returns 编译生成的汇编码
 */
export function compile(input: string, option: CompilerOption): string {
    let inputStream = new ANTLRInputStream(input);
    let lexer = new MiniDecafLexer(inputStream);
    let tokenStream = new CommonTokenStream(lexer);
    let parser = new MiniDecafParser(tokenStream);

    // 去掉默认的报错处理，使用自定义的
    let listener = new ErrorListener();
    lexer.removeErrorListeners();
    lexer.addErrorListener(listener);
    parser.removeErrorListeners();
    parser.addErrorListener(listener);
    parser.buildParseTree = true;

    let tree = parser.program();
    if (parser.numberOfSyntaxErrors > 0) {
        throw new Error(listener.toString());
    }

    if (option.target === CompilerTarget.Riscv32Asm) {
        return tree.accept(new Riscv32CodeGen());
    } else if (option.target === CompilerTarget.Executed) {
        return (tree.accept(new Executor()) & 0xff).toString(); // Shell 返回码只有 8 位
    }
}
