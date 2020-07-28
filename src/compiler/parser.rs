use crate::compiler::*;

pub fn parsing(tokens: &Vec<Token>) -> Option<Node> {
    let mut idx: usize = 0;
    let node = exp(&tokens, &mut idx);
    node
}

fn create_node(val: &str, kind: NodeKind, left: Option<Node>, right: Option<Node>) -> Option<Node> {
    match (left, right) {
        (Some(node_left), Some(node_right)) => Some(Node {
            val: String::from(val),
            kind: kind,
            left: Some(Box::new(node_left)),
            right: Some(Box::new(node_right)),
        }),
        (Some(node_left), None) => Some(Node {
            val: String::from(val),
            kind: kind,
            left: Some(Box::new(node_left)),
            right: None,
        }),
        (None, Some(node_right)) => Some(Node {
            val: String::from(val),
            kind: kind,
            left: None,
            right: Some(Box::new(node_right)),
        }),
        (None, None) => Some(Node {
            val: String::from(val),
            kind: kind,
            left: None,
            right: None,
        }),
    }
}

fn consume(tokens: &Vec<Token>, idx: &mut usize, target_val: &str) -> bool {
    let idx_: usize = *idx;

    if tokens.len() <= idx_ {
        return false;
    }

    if tokens[idx_].val == target_val {
        *idx += 1;
        true
    } else {
        false
    }
}

fn exp(tokens: &Vec<Token>, idx: &mut usize) -> Option<Node> {
    let mut node = primary(&tokens, idx);

    loop {
        if consume(&tokens, idx, &"+") {
            node = create_node(&"+", NodeKind::NdOperator, node, primary(&tokens, idx));
        } else if consume(&tokens, idx, &"-") {
            node = create_node(&"-", NodeKind::NdOperator, node, primary(&tokens, idx));
        } else {
            return node;
        }
    }
}

fn primary(tokens: &Vec<Token>, idx: &mut usize) -> Option<Node> {
    let idx_: usize = *idx;
    if let TokenKind::TkNum = tokens[idx_].kind {
        *idx += 1;
        return create_node(&tokens[idx_].val, NodeKind::NdNum, None, None);
    };
    None
}
