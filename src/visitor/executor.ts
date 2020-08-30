import { AbstractParseTreeVisitor } from "antlr4ts/tree";
import { MiniDecafVisitor } from "../gen/MiniDecafVisitor";

/** 解释执行器，直接在语法树上执行 */
export class Executor extends AbstractParseTreeVisitor<number> implements MiniDecafVisitor<number> {
    defaultResult(): number {
        return 0;
    }
}
