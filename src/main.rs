pub mod ast;
pub mod codegen;
pub mod ir;
pub mod lexer;
pub mod parser;

fn main() -> std::io::Result<()> {
  let path = std::env::args()
    .nth(1)
    .expect("usage: minidecaf <input path>");
  minidecaf::run(path, &mut std::io::stdout())
}
