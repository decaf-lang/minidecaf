import { Token } from "antlr4ts";

function errorMessage(line: number, charPositionInLine: number, msg: string): string {
    return `line ${line}, col ${charPositionInLine + 1}: ${msg}`;
}

export class SyntaxError extends Error {
    constructor(line: number, charPositionInLine: number, msg: string) {
        super(errorMessage(line, charPositionInLine, msg));
    }
}

export class SemanticError extends Error {
    constructor(token: Token, msg: string) {
        super(errorMessage(token.line, token.charPositionInLine, msg));
    }
}

export class RuntimeError extends Error {
    constructor(msg: string) {
        super(`RuntimeError: ${msg}`);
    }
}

export class OtherError extends Error {
    constructor(msg: string) {
        super(`OtherError: ${msg}`);
    }
}
