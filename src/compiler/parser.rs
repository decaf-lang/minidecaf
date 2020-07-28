use crate::compiler::*;

pub fn parsing(tokens: &Vec<Token>) -> Option<Node> {
    let mut idx: usize = 0;
    let node = expr(&tokens, &mut idx);
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

fn expr(tokens: &Vec<Token>, idx: &mut usize) -> Option<Node> {
    let mut node = addsub(&tokens, idx);

    loop {
        if consume(&tokens, idx, &"==") {
            node = create_node(
                &"==",
                NodeKind::NdComparisonOperator,
                node,
                addsub(&tokens, idx),
            );
        } else if consume(&tokens, idx, &"!=") {
            node = create_node(
                &"!=",
                NodeKind::NdComparisonOperator,
                node,
                addsub(&tokens, idx),
            );
        } else if consume(&tokens, idx, &"<") {
            node = create_node(
                &"<",
                NodeKind::NdComparisonOperator,
                node,
                addsub(&tokens, idx),
            );
        } else if consume(&tokens, idx, &"<=") {
            node = create_node(
                &"<=",
                NodeKind::NdComparisonOperator,
                node,
                addsub(&tokens, idx),
            );
        } else if consume(&tokens, idx, &">") {
            node = create_node(
                &"<",
                NodeKind::NdComparisonOperator,
                addsub(&tokens, idx),
                node,
            );
        } else if consume(&tokens, idx, &">=") {
            node = create_node(
                &"<=",
                NodeKind::NdComparisonOperator,
                addsub(&tokens, idx),
                node,
            );
        } else {
            return node;
        }
    }
}

fn addsub(tokens: &Vec<Token>, idx: &mut usize) -> Option<Node> {
    let mut node = term(&tokens, idx);

    loop {
        if consume(&tokens, idx, &"+") {
            node = create_node(&"+", NodeKind::NdOperator, node, term(&tokens, idx));
        } else if consume(&tokens, idx, &"-") {
            node = create_node(&"-", NodeKind::NdOperator, node, term(&tokens, idx));
        } else {
            return node;
        }
    }
}

fn term(tokens: &Vec<Token>, idx: &mut usize) -> Option<Node> {
    let mut node = unary(&tokens, idx);

    loop {
        if consume(&tokens, idx, &"*") {
            node = create_node(&"*", NodeKind::NdOperator, node, unary(&tokens, idx));
        } else if consume(&tokens, idx, &"/") {
            node = create_node(&"/", NodeKind::NdOperator, node, unary(&tokens, idx));
        } else {
            return node;
        }
    }
}

fn unary(tokens: &Vec<Token>, idx: &mut usize) -> Option<Node> {
    if consume(&tokens, idx, &"+") {
        let node = create_node("0", NodeKind::NdNum, None, None);
        return create_node(&"+", NodeKind::NdOperator, node, factor(&tokens, idx));
    } else if consume(&tokens, idx, &"-") {
        let node = create_node("0", NodeKind::NdNum, None, None);
        return create_node(&"-", NodeKind::NdOperator, node, factor(&tokens, idx));
    } else {
        return factor(&tokens, idx);
    }
}

fn factor(tokens: &Vec<Token>, idx: &mut usize) -> Option<Node> {
    let idx_: usize = *idx;
    match tokens[idx_].kind {
        TokenKind::TkNum => {
            *idx += 1;
            return create_node(&tokens[idx_].val, NodeKind::NdNum, None, None);
        }
        TokenKind::TkParenthesis => {
            if &tokens[idx_].val[..] == "(" {
                *idx += 1;
                let node = expr(&tokens, idx);
                if consume(&tokens, idx, &")") {
                    return node;
                } else {
                    panic!("Expected )");
                }
            } else {
                return None;
            }
        }
        _ => {
            return None;
        }
    }
}
