use crate::compiler::*;

pub fn lexing(input: &String) -> Vec<Token> {
    let mut tokens: Vec<Token> = Vec::new();

    let mut begin_idx = 0;
    let mut condition = LexerCondition::CondCompletion;
    for (i, s) in input.chars().enumerate() {
        match return_letter_kind(s) {
            LetterKind::LtNum => match condition {
                LexerCondition::CondMiddleOfNumber => {}
                LexerCondition::CondMiddleOfComparisonOperator => {
                    let new_token = Token {
                        kind: TokenKind::TkComparisonOperator,
                        val: check_valid_token(&input[begin_idx..i]).to_string(),
                    };
                    tokens.push(new_token);
                    begin_idx = i;
                    condition = LexerCondition::CondMiddleOfNumber;
                }
                _ => {
                    begin_idx = i;
                    condition = LexerCondition::CondMiddleOfNumber;
                }
            },
            LetterKind::LtSpace => match condition {
                LexerCondition::CondMiddleOfNumber => {
                    let new_token = Token {
                        kind: TokenKind::TkNum,
                        val: input[begin_idx..i].to_string(),
                    };
                    tokens.push(new_token);
                    begin_idx = i + 1;
                    condition = LexerCondition::CondCompletion;
                }
                LexerCondition::CondMiddleOfComparisonOperator => {
                    let new_token = Token {
                        kind: TokenKind::TkComparisonOperator,
                        val: input[begin_idx..i].to_string(),
                    };
                    tokens.push(new_token);
                    begin_idx = i + 1;
                    condition = LexerCondition::CondCompletion;
                }
                _ => {
                    begin_idx = i + 1;
                }
            },
            LetterKind::LtOperator | LetterKind::LtParenthesis => {
                let new_tokenkind;
                match return_letter_kind(s) {
                    LetterKind::LtParenthesis => {
                        new_tokenkind = TokenKind::TkParenthesis;
                    }
                    _ => {
                        new_tokenkind = TokenKind::TkOperator;
                    }
                }
                match condition {
                    LexerCondition::CondMiddleOfNumber => {
                        let new_token = Token {
                            kind: TokenKind::TkNum,
                            val: input[begin_idx..i].to_string(),
                        };
                        tokens.push(new_token);
                        let new_token = Token {
                            kind: new_tokenkind,
                            val: String::from(s.to_string()),
                        };
                        tokens.push(new_token);
                        begin_idx = i + 1;
                        condition = LexerCondition::CondCompletion;
                    }
                    LexerCondition::CondMiddleOfComparisonOperator => {
                        let new_token = Token {
                            kind: TokenKind::TkComparisonOperator,
                            val: check_valid_token(&input[begin_idx..i]).to_string(),
                        };
                        tokens.push(new_token);
                        let new_token = Token {
                            kind: new_tokenkind,
                            val: String::from(s.to_string()),
                        };
                        tokens.push(new_token);
                        begin_idx = i + 1;
                        condition = LexerCondition::CondCompletion;
                    }
                    _ => {
                        let new_token = Token {
                            kind: new_tokenkind,
                            val: String::from(s.to_string()),
                        };
                        tokens.push(new_token);
                        begin_idx = i + 1;
                    }
                }
            }
            LetterKind::LtComparisonOperator => match condition {
                LexerCondition::CondMiddleOfNumber => {
                    let new_token = Token {
                        kind: TokenKind::TkNum,
                        val: input[begin_idx..i].to_string(),
                    };
                    tokens.push(new_token);
                    begin_idx = i;
                    condition = LexerCondition::CondMiddleOfComparisonOperator;
                }
                LexerCondition::CondMiddleOfComparisonOperator => {
                    let new_token = Token {
                        kind: TokenKind::TkComparisonOperator,
                        val: check_valid_token(&input[begin_idx..i + 1]).to_string(),
                    };
                    tokens.push(new_token);
                    begin_idx = i + 1;
                    condition = LexerCondition::CondCompletion;
                }
                _ => {
                    begin_idx = i;
                    condition = LexerCondition::CondMiddleOfComparisonOperator;
                }
            },
        }
    }
    tokens
}

fn return_letter_kind(s: char) -> LetterKind {
    match s {
        '0'..='9' => LetterKind::LtNum,
        ' ' => LetterKind::LtSpace,
        '+' | '-' | '*' | '/' => LetterKind::LtOperator,
        '(' | ')' => LetterKind::LtParenthesis,
        '<' | '>' | '=' | '!' => LetterKind::LtComparisonOperator,
        _ => {
            panic!("Cannot recognize {}", s);
        }
    }
}

fn check_valid_token(s: &str) -> &str {
    match s {
        "<" | ">" | "<=" | ">=" | "==" | "!=" => s,
        _ => {
            panic!("Not a valid token :{}", s);
        }
    }
}
