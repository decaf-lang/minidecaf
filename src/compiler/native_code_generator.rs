// use crate::compiler::*;

pub fn generate_native_code(mid_commands: &Vec<String>) -> Vec<String> {
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
            _ => {}
        }
    }

    // pop a0
    native_commands.push(format!("\tld a0, 0(sp)"));
    native_commands.push(format!("\taddi sp, sp, -8"));
    native_commands.push(format!("\tret"));

    native_commands
}
