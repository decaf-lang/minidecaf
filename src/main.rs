use std::env;

enum LetterKind {
    LtNum,
    LtSpace,
    LtOperator,
}

enum TokenKind {
    TkNum,
    TkOperator,
}

enum NodeKind {
    NdNum,
    NdOperator,
}

struct Token {
    kind: TokenKind,
    val: String,
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
    kind: NodeKind,
    left: Option<Box<Node>>,
    right: Option<Box<Node>>,
}

fn traverse(node: &Node, mid_commands: &mut Vec<String>){
    match &node.left {
        Some(node_left) => {
            traverse(&node_left, mid_commands);
        },
        _ => {},
    }

    match &node.right {
        Some(node_right) => {
            traverse(&node_right, mid_commands);
        },
        _ => {},
    }

    match node.kind {
        NodeKind::NdNum => {
            mid_commands.push(format!("PUSH {}", node.val));
        },
        NodeKind::NdOperator => {
            match &node.val[..] {
                "+" => {
                    mid_commands.push(String::from("ADD"));
                },
                "-" => {
                    mid_commands.push(String::from("SUB"));
                },
                _ => {
                    panic!("Unexpected operator: {}", node.val);

                }
            }
        },
    }
}

fn generate_intermidiate_code(node: &Option<Node>) -> Vec<String> {
    let mut mid_commands : Vec<String>= Vec::new();
    match node {
        Some(node) => {
            traverse(&node, &mut mid_commands);
        }
        _ => {},
    }
    mid_commands
}

fn create_node(val: &str, kind: NodeKind, left: Option<Node>, right: Option<Node>) -> Option<Node> {
    match (left, right) {
        (Some(node_left), Some(node_right)) => {
            Some(Node {
                val: String::from(val),
                kind: kind,
                left: Some(Box::new(node_left)),
                right: Some(Box::new(node_right)),
            }
            )
        },
        (Some(node_left), None) => {
            Some(Node {
                val: String::from(val),
                kind: kind,
                left: Some(Box::new(node_left)),
                right: None,
            })
        },
        (None, Some(node_right)) => {
            Some( Node {
                val: String::from(val),
                kind: kind,
                left: None,
                right: Some(Box::new(node_right))
            })
        },
        (None, None) => {
            Some( Node {
                val: String::from(val),
                kind: kind,
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
            node = create_node(&"+", NodeKind::NdOperator, node, primary(&tokens, idx));
        } else if consume(&tokens, idx, &"-") {
            node = create_node(&"-", NodeKind::NdOperator, node, primary(&tokens, idx));
        } else{
            return node;
        }
    }
}

fn primary(tokens: &Vec<Token>, idx: &mut usize) -> Option<Node> {
    let idx_ : usize = *idx;
    if let TokenKind::TkNum = tokens[idx_].kind {
        *idx += 1;
        return create_node(&tokens[idx_].val, NodeKind::NdNum, None, None);
    };
    None
}

fn parsing(tokens: &Vec<Token>) -> Option<Node> {
    let mut idx : usize = 0;
    let node = exp(&tokens, &mut idx);
    node
}

fn generate_native_code(mid_commands: &Vec<String>) -> Vec<String> {
    let mut native_commands = Vec::new();
    native_commands.push(String::from(".global main"));
    native_commands.push(String::from("main:"));

    for mid_command in mid_commands.iter() {
        println!("// {}", mid_command);
    }

    for command in mid_commands.iter() {
        let v: Vec<&str> = command[..].split(' ').collect();
        match v[0] {
            "PUSH" => {
                // push v[1]
                native_commands.push(format!("\tadd t0, x0, {}", v[1]));
                native_commands.push(format!("\tsd t0, -8(sp)"));
                native_commands.push(format!("\taddi sp, sp, -8"));
            }
            "ADD" => {
                // pop t0
                native_commands.push(format!("\tld t0, 0(sp)"));
                native_commands.push(format!("\taddi sp, sp, 8"));

                // pop t1
                native_commands.push(format!("\tld t1, 0(sp)"));
                native_commands.push(format!("\taddi sp, sp, 8"));

                // t0 = t1 + t0
                native_commands.push(format!("\tadd t0, t1, t0"));

                // push t0
                native_commands.push(format!("\tsd t0, -8(sp)"));
                native_commands.push(format!("\taddi sp, sp, -8"));
            }
            "SUB" => {
                // pop t0
                native_commands.push(format!("\tld t0, 0(sp)"));
                native_commands.push(format!("\taddi sp, sp, 8"));
                // pop t1
                native_commands.push(format!("\tld t1, 0(sp)"));
                native_commands.push(format!("\taddi sp, sp, 8"));

                // t0 = t1 - t0
                native_commands.push(format!("\tsub t0, t1, t0"));

                // push t0
                native_commands.push(format!("\tsd t0, -8(sp)"));
                native_commands.push(format!("\taddi sp, sp, -8"));
            }
            _ => {
            },
        }
    }

    // pop a0
    native_commands.push(format!("\tld a0, 0(sp)"));
    native_commands.push(format!("\taddi sp, sp, -8"));
    native_commands.push(format!("\tret"));

    native_commands
}

fn main() {
    let args: Vec<String> = env::args().collect();
    let input: String = args[1..].join(" ");
    let input: String = [&input, " "].join("");

    let tokens = lexing(&input);
    let node = parsing(&tokens);
    let mid_commands = generate_intermidiate_code(&node);
    let native_commands = generate_native_code(&mid_commands);

    for command in native_commands.iter() {
        println!("{}", command);
    }
}
