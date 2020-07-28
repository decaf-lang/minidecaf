// use crate::compiler::*;
use std::collections::HashMap;

pub fn generate_native_code(mid_commands: &Vec<String>) -> Vec<String> {
    let mut native_commands = Vec::new();
    native_commands.push(String::from(".global main"));
    native_commands.push(String::from("main:"));
    native_commands.push(format!("\tmv s0, sp"));

    let mut variable_map = HashMap::new();

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
            "STORE" => {
                // pop t0
                native_commands.push(format!("\tld t0, 0(sp)"));
                native_commands.push(format!("\taddi sp, sp, 8"));

                match variable_map.get(v[1]) {
                    Some(offset) => {
                        // store t0 to s0-offset
                        native_commands.push(format!("\tsd t0, -{}(s0)", offset));
                    }
                    _ => {
                        // if the v[1] is new variable, create a new entry in variable_map and move sp
                        // (assume that no operand is in the stack when storing)
                        let new_offset = 8 * (variable_map.len() + 1);
                        variable_map.insert(v[1], new_offset);
                        native_commands.push(format!("\tsd t0, -{}(s0)", new_offset));
                        native_commands.push(format!("\taddi sp, sp, -8"));
                    }
                }
            }
            "LOAD" => {
                let offset = variable_map.get(v[1]);
                match offset {
                    Some(offset_) => {
                        // load s0-offset to t0
                        native_commands.push(format!("\tld t0, -{}(s0)", offset_));

                        // push t0
                        native_commands.push(format!("\tsd t0, -8(sp)"));
                        native_commands.push(format!("\taddi sp, sp, -8"));
                    }
                    _ => {
                        panic!("Cannot load from {}", v[1]);
                    }
                }
            }
            "ADD" | "SUB" | "MUL" | "DIV" | "EQUAL" | "NONEQUAL" | "LT" | "ELT" => {
                // pop t0
                native_commands.push(format!("\tld t0, 0(sp)"));
                native_commands.push(format!("\taddi sp, sp, 8"));

                // pop t1
                native_commands.push(format!("\tld t1, 0(sp)"));
                native_commands.push(format!("\taddi sp, sp, 8"));

                // t0 = t1 + t0
                match v[0] {
                    "ADD" => {
                        native_commands.push(format!("\tadd t0, t1, t0"));
                    }
                    "SUB" => {
                        native_commands.push(format!("\tsub t0, t1, t0"));
                    }
                    "MUL" => {
                        native_commands.push(format!("\tmul t0, t1, t0"));
                    }
                    "DIV" => {
                        native_commands.push(format!("\tdiv t0, t1, t0"));
                    }
                    "EQUAL" => {
                        native_commands.push(format!("\tsub t0, t0, t1"));
                        native_commands.push(format!("\tseqz t0, t0"));
                    }
                    "NONEQUAL" => {
                        native_commands.push(format!("\tsub t0, t1, t0"));
                        native_commands.push(format!("\tsnez t0, t0"));
                    }
                    "LT" => {
                        native_commands.push(format!("\tslt t0, t1, t0"));
                    }
                    "ELT" => {
                        native_commands.push(format!("\tsgt t0, t1, t0"));
                        native_commands.push(format!("\txori t0, t0, 1"));
                    }
                    _ => {}
                }

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
