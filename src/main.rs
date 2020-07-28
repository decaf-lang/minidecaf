use std::env;

fn main() {
    let args : Vec<String> = env::args().collect();
    //println!("{:#?}",args);
    let input : String = args[1..].join("");
    //println!("{:#?}",input);

    println!(".global main");
    println!("main:");

    let mut idx = 0;

    for s in input.chars() {
        match s {
            '+' => {
                idx=idx+1;
            },
            '0' ..= '9' => {

                if idx == 0 {
                    let num: u32 = match s.to_digit(10) {
                        Some(num) => num,
                        None => panic!("Expect a number in the head"),
                    };
                    println!("\tli a0, {}", num);
                } else {
                    if idx %2 == 0 {
                        let num: u32 = match s.to_digit(10) {
                            Some(num) => num,
                            None => panic!("Expect a number in the head"),
                        };
                        println!("\tadd a0, a0, {}", num);
                    }else {
                        panic!("Expect a number in the middle");
                    }
                }
                idx=idx+1;
            },
            _ => {
                panic!("Cannot tokenize {}", s);
            },
        }
    }

    println!("\tret");
}
