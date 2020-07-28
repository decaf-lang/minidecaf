use std::env;

pub mod compiler;

fn main() {
    let args: Vec<String> = env::args().collect();
    let input: String = args[1..].join(" ");
    let input: String = [&input, " "].join("");

    let tokens = crate::compiler::lexing(&input);
    let node = crate::compiler::parsing(&tokens);
    let mid_commands = crate::compiler::generate_intermediate_code(&node);
    let native_commands = crate::compiler::generate_native_code(&mid_commands);

    for command in native_commands.iter() {
        println!("{}", command);
    }
}
