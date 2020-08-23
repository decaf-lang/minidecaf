use std::env;

fn main() {
    let args : Vec<String> = env::args().collect();
    let value = &args[1];

    println!(".global main");
    println!("main:");
    println!("\tli a0, {}", value);
    println!("\tret");
}
