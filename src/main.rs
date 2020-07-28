use std::env;

fn main() {
    let args : Vec<String> = env::args().collect();

    let input : String = args[1..].join(" ");
    let input : String = [&input, " "].join("");

    let mut tokens: Vec<&str> = Vec::new();

    let mut begin_idx = 0;
    let mut is_in_the_middle_of_number = false;
    for (i, s) in input.chars().enumerate() {
        match s {
            ' ' => {
                if is_in_the_middle_of_number {
                    tokens.push(&input[begin_idx..i]);
                    begin_idx = i + 1;
                    is_in_the_middle_of_number = false;
                } else {
                    begin_idx = i + 1;
                }
            },
            '+' => {
                if is_in_the_middle_of_number {
                    tokens.push(&input[begin_idx..i]);
                    tokens.push("+");
                    begin_idx = i + i;
                    is_in_the_middle_of_number = false;
                } else{
                    tokens.push("+");
                    begin_idx = i + i;
                }
            },
            '0' ..= '9' => {
                if is_in_the_middle_of_number {
                } else{
                    begin_idx = i;
                    is_in_the_middle_of_number = true;
                }
            },
            _ => {
                panic!("Cannot tokenize {}", s);
            },
        }
    }

    println!(".global main");
    println!("main:");

    let mut stack: Vec<&str> = Vec::new();
    for (i, token) in tokens.iter().enumerate() {
        if i == 0 {
            let num: u32 = match token.parse() {
                Ok(num) => num,
                Err(_) => panic!("Expect a number in the head"),
            };
            println!("\tli a0, {}", num);
            continue;
        }

        if token == &"+" {
            if stack.is_empty() {
                stack.push("+");
            } else{
                panic!("Expect a number after an operator");
            }
            continue
        }

        let num: u32 = match token.parse() {
            Ok(num) => num,
            Err(_) => panic!("Expect a number after an operator"),
        };

        let top = match stack.pop() {
            Some(top) => top,
            None => panic!("Expect an operator after a number"),
        };

        if top != "+" {
            panic!("Expect an operator after a number")
        }

        println!("\tadd a0, a0, {}", num);
    }

    println!("\tret");
}