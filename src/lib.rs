pub mod ast;
pub mod codegen;
pub mod ir;
pub mod lexer;
pub mod parser;

use lexer::tokenize;
use parser::parsing;
use std::io::{Result, Write};

pub fn run(path: String, output: &mut impl Write) -> Result<()> {
  let t = tokenize(path);
  //println!("Tokens: {:#?}", t);
  let p = parsing(&t);
  let p = ir::ast2ir(&p);
  codegen::write_asm(&p, output)
}
