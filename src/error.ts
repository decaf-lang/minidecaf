function errorMessage(line: number, charPositionInLine: number, msg: string): string {
    return `line ${line}, col ${charPositionInLine + 1}: ${msg}`;
}

export class SyntaxError extends Error {
    constructor(line: number, charPositionInLine: number, msg: string) {
        super(errorMessage(line, charPositionInLine, msg));
    }
}
