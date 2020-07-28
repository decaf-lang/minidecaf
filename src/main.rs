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

struct Node {
    val: String,
    left: Option<Box<Node>>,
    right: Option<Box<Node>>,
}

fn create_node(val: &str, left: Option<Node>, right: Option<Node>) -> Option<Node> {
    match (left, right) {
        (Some(node_left), Some(node_right)) => {
            Some(Node {
                val: String::from(val),
                left: Some(Box::new(node_left)),
                right: Some(Box::new(node_right)),
            }
            )
        },
        (Some(node_left), None) => {
            Some(Node {
                val: String::from(val),
                left: Some(Box::new(node_left)),
                right: None,
            })
        },
        (None, Some(node_right)) => {
            Some( Node {
                val: String::from(val),
                left: None,
                right: Some(Box::new(node_right))
            })
        },
        (None, None) => {
            Some( Node {
                val: String::from(val),
                left: None,
                right: None
            })
        },
    }
}

fn consume(tokens: &Vec<Token>, idx: &mut usize, target_val: &str) -> bool {
    let idx_ : usize = *idx;

    if tokens.len() <= idx_ {
        return false;
    }

    if tokens[idx_].val == target_val {
        *idx += 1;
        true
    } else {
        false
    }
}

fn exp(tokens: &Vec<Token>, idx: &mut usize) -> Option<Node> {
    let mut node = primary(&tokens, idx);

    loop {
        if consume(&tokens, idx, &"+") {
            node = create_node(&"+", node, exp(&tokens, idx));
        } else if consume(&tokens, idx, &"-") {
            node = create_node(&"-", node, exp(&tokens, idx));
        } else{
            return node;
        }
    }
}

fn primary(tokens: &Vec<Token>, idx: &mut usize) -> Option<Node> {
    let idx_ : usize = *idx;
    if let TokenKind::TkNum = tokens[idx_].kind {
        *idx += 1;
        return create_node(&tokens[idx_].val, None, None);
    };
    None
}

fn parsing(tokens: &Vec<Token>) -> Vec<String> {
    let mut commands = Vec::new();
    commands.push(String::from(".global main"));
    commands.push(String::from("main:"));

    let mut idx : usize = 0;
    let _root_node = exp(&tokens, &mut idx);

    //match root_node {
    //Some(node) =>  {
    //traverse(&node);
    //},
    //_ => {
    //},
    //}

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
