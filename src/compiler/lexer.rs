use crate::compiler::*;

pub fn lexing(input: &String) -> Vec<Token> {
    let mut tokens: Vec<Token> = Vec::new();

    let mut begin_idx = 0;
    let mut is_in_the_middle_of_number = false;
    for (i, s) in input.chars().enumerate() {
        match return_letter_kind(s) {
            LetterKind::LtNum => {
                if is_in_the_middle_of_number {
                } else {
                    begin_idx = i;
                    is_in_the_middle_of_number = true;
                }
            }
            LetterKind::LtSpace => {
                if is_in_the_middle_of_number {
                    let new_token = Token {
                        kind: TokenKind::TkNum,
                        val: input[begin_idx..i].to_string(),
                    };
                    tokens.push(new_token);
                    begin_idx = i + 1;
                    is_in_the_middle_of_number = false;
                } else {
                    begin_idx = i + 1;
                }
            }
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
                if is_in_the_middle_of_number {
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
                    is_in_the_middle_of_number = false;
                } else {
                    let new_token = Token {
                        kind: new_tokenkind,
                        val: String::from(s.to_string()),
                    };
                    tokens.push(new_token);
                    begin_idx = i + 1;
                }
            }
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
        _ => {
            panic!("Cannot recognize {}", s);
        }
    }
}
