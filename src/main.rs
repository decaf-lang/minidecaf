use std::env;

enum TokenKind {
    TkNum,
    TkOperator,
}

struct Token {
    kind: TokenKind,
    val: String,
}

enum LetterKind {
    LtNum,
    LtSpace,
    LtOperator,
}

fn return_letter_kind(s: char) -> LetterKind {
    match s {
        '0'..='9' => LetterKind::LtNum,
        ' ' => LetterKind::LtSpace,
        '+' | '-' => LetterKind::LtOperator,
        _ => {
            panic!("Cannot recognize {}", s);
        }
    }
}

fn lexing(input: &String) -> Vec<Token> {
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
            LetterKind::LtOperator => {
                if is_in_the_middle_of_number {
                    let new_token = Token {
                        kind: TokenKind::TkNum,
                        val: input[begin_idx..i].to_string(),
                    };
                    tokens.push(new_token);
                    let new_token = Token {
                        kind: TokenKind::TkOperator,
                        val: String::from(s.to_string()),
                    };
                    tokens.push(new_token);
                    begin_idx = i + 1;
                    is_in_the_middle_of_number = false;
                } else {
                    let new_token = Token {
                        kind: TokenKind::TkOperator,
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

fn parsing(tokens: &Vec<Token>) -> Vec<String> {
    let mut commands = Vec::new();
    commands.push(String::from(".global main"));
    commands.push(String::from("main:"));

    let mut stack: Vec<&Token> = Vec::new();
    for (i, token) in tokens.iter().enumerate() {
        if i == 0 {
            match token.kind {
                TokenKind::TkNum => {
                    commands.push(format!("\tli a0, {}", token.val));
                }
                TokenKind::TkOperator => panic!("Expect a number in the head"),
            }
            continue;
        }

        match token.kind {
            TokenKind::TkNum => {
                let top = match stack.pop() {
                    Some(top) => top,
                    None => panic!("Expect an operator after a number"),
                };
                match top.kind {
                    TokenKind::TkNum => panic!("Expect an operator after a number"),
                    TokenKind::TkOperator => match &top.val[..] {
                        "+" => {
                            commands.push(format!("\taddiw a0, a0, {}", token.val));
                        }
                        "-" => {
                            commands.push(format!("\taddiw a0, a0, -{}", token.val));
                        }
                        _ => {
                            panic!("Unexpected operator: {}", token.val);
                        }
                    },
                }
            }
            TokenKind::TkOperator => {
                if stack.is_empty() {
                    stack.push(&token);
                } else {
                    panic!("Expect a number after an operator");
                }
                continue;
            }
        }
    }

    commands.push(format!("\tret"));
    commands
}

fn main() {
    let args: Vec<String> = env::args().collect();
    let input: String = args[1..].join(" ");
    let input: String = [&input, " "].join("");

    let tokens = lexing(&input);
    let commands = parsing(&tokens);

    for command in commands.iter() {
        println!("{}", command);
    }
}