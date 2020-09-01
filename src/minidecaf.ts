import { ANTLRInputStream, ANTLRErrorListener, CommonTokenStream } from "antlr4ts";
import { MiniDecafLexer } from "./gen/MiniDecafLexer";
import { MiniDecafParser } from "./gen/MiniDecafParser";
import { SemanticCheck } from "./visitor/semantic";
import { IrGen } from "./visitor/irgen";
import { Riscv32CodeGen } from "./target/riscv";
import { IrExecutor } from "./target/executor";
import { SyntaxError } from "./error";
import { Ir } from "./ir";

export enum CompilerTarget {
    /** 编译到 RISC-V 32 汇编 */
    Riscv32Asm = "riscv32-asm",
    /** 作为解释器直接运行 */
    Executed = "executed",
    /** 编译到中间表示(IR) */
    Ir = "ir",
}

export class CompilerOption {
    /** 编译目标 {@link CompilerTarget} */
    target: CompilerTarget;
    /** 是否将程序返回码截断为 8 位 */
    truncateReturnCode: boolean = false;
    /** 运行时间限制(秒) */
    timeoutSecond?: number;
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
 * @param input 输入，可以是源代码(`string`)或中间表示({@link Ir})
 * @param option 编译选项
 * @returns 编译结果，可以是汇编码(`string`)或中间表示({@link Ir})
 */
export function compile(input: string | Ir, option: CompilerOption): string | Ir {
    let ir: Ir;
    if (typeof input == "string") {
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

        let tree = parser.program(); // 语法解析
        if (parser.numberOfSyntaxErrors > 0) {
            throw new Error(listener.toString());
        }
        tree.accept(new SemanticCheck()); // 语义检查

        let irgen = new IrGen(); // 中间代码生成
        tree.accept(irgen);
        ir = irgen.ir;
    } else {
        ir = input;
    }

    if (option.target === CompilerTarget.Ir) {
        return ir;
    } else if (option.target === CompilerTarget.Riscv32Asm) {
        let codegen = new Riscv32CodeGen(ir); // 目标代码生成
        return codegen.visitAll();
    } else if (option.target === CompilerTarget.Executed) {
        let executor = new IrExecutor(ir, option.timeoutSecond); // 直接执行中间代码
        let res = executor.visitAll();
        if (option.truncateReturnCode) {
            res &= 0xff; // Shell 返回码只有 8 位
        }
        return res.toString();
    }
}
