use std::env;

pub mod compiler;

fn main() {
    let args: Vec<String> = env::args().collect();
    let input: String = args[1..].join(" ");
    let input: String = [&input, " "].join("");

    let tokens = compiler::lexing(&input);
    let ast = compiler::parsing(&tokens);
    let mid_commands = compiler::generate_intermediate_code(&ast);
    let native_commands = compiler::generate_native_code(&mid_commands);

    for command in native_commands.iter() {
        println!("{}", command);
    }
}
