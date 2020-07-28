pub mod intermediate_code_generator;
pub mod lexer;
pub mod native_code_generator;
pub mod parser;

pub use intermediate_code_generator::generate_intermediate_code;
pub use lexer::lexing;
pub use native_code_generator::generate_native_code;
pub use parser::parsing;

pub enum LetterKind {
    LtNum,
    LtSpace,
    LtOperator,
    LtComparisonOperator,
    LtParenthesis,
    LtSymbol,
    LtAlphabet,
}

pub enum LexerCondition {
    CondCompletion,
    CondMiddleOfNumber,
    CondMiddleOfComparisonOperator,
    CondMiddleOfVariable,
}

pub enum TokenKind {
    TkNum,
    TkOperator,
    TkComparisonOperator,
    TkParenthesis,
    TkSymbol,
    TkVariable,
}

pub struct Token {
    pub kind: TokenKind,
    pub val: String,
}

pub enum NodeKind {
    NdNum,
    NdOperator,
    NdComparisonOperator,
    NdAssignOperator,
    NdVariable,
}

pub struct Node {
    pub val: String,
    pub kind: NodeKind,
    pub left: Option<Box<Node>>,
    pub right: Option<Box<Node>>,
}
