import { AbstractParseTreeVisitor } from "antlr4ts/tree";
import { MiniDecafVisitor } from "../gen/MiniDecafVisitor";

/** 语法树到 RV32 汇编码的生成器 */
export class Riscv32CodeGen
    extends AbstractParseTreeVisitor<string>
    implements MiniDecafVisitor<string> {
    defaultResult(): string {
        return "nop";
    }
}
