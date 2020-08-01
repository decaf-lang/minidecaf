parser grammar MDParser;

options {
    tokenVocab = MDLexer;
    superClass = BaseParser;
}

@parser::postinclude {

#include <string>

#include "BaseParser.h"

}

program : stmt
        ;

stmt    : expr ';'
          {
            setExpr($expr.node);
          }
        ;

expr    returns [std::shared_ptr<ASTNode> node]
        : Integer
          {
            $node = IntegerNode::make(std::stoi($Integer.text));
          }
        | '(' expr ')'
          {
            $node = $expr.node;
          }
        | '-' expr
          {
            $node = SubNode::make(IntegerNode::make(0), $expr.node);
          }
        | '+' expr
          {
            $node = $expr.node;
          }
        | lhs=expr op=('*' | '/') rhs=expr
          {
            if ($op.text == "*") {
                $node = MulNode::make($lhs.node, $rhs.node);
            } else {
                $node = DivNode::make($lhs.node, $rhs.node);
            }
          }
        | lhs=expr op=('+' | '-') rhs=expr
          {
            if ($op.text == "+") {
                $node = AddNode::make($lhs.node, $rhs.node);
            } else {
                $node = SubNode::make($lhs.node, $rhs.node);
            }
          }
        | lhs=expr op=('<' | '>' | '<=' | '>=' | '==' | '!=') rhs=expr
          {
            if ($op.text == "<") {
                $node = LTNode::make($lhs.node, $rhs.node);
            } else if ($op.text == ">") {
                $node = GTNode::make($lhs.node, $rhs.node);
            } else if ($op.text == "<=") {
                $node = LENode::make($lhs.node, $rhs.node);
            } else if ($op.text == ">=") {
                $node = GENode::make($lhs.node, $rhs.node);
            } else if ($op.text == "==") {
                $node = EQNode::make($lhs.node, $rhs.node);
            } else if ($op.text == "!=") {
                $node = NENode::make($lhs.node, $rhs.node);
            }
          }
        ;

