parser grammar MDParser;

options {
    tokenVocab = MDLexer;
}

@parser::postinclude {

#include <string>
#include <vector>

#include "ASTNode.h"

}

program returns [std::shared_ptr<ProgramNode> node]
        : funcs EOF
          {
            $node = $funcs.node;
          }
        ;

funcs   returns [std::shared_ptr<ProgramNode> node]
        : func
          {
            $node = ProgramNode::make({$func.node});
          }
        | part=funcs func
          {
            $node = $part.node;
            $node->funcs_.push_back($func.node);
          }
        ;

func    returns [std::shared_ptr<FunctionNode> node]
        : INT Identifier '(' vars ')' '{' stmtSeq '}'
          {
            $node = FunctionNode::make($Identifier.text, $vars.nodes, $stmtSeq.node);
          }
        ;

stmtSeq returns [std::shared_ptr<StmtSeqNode> node]
        : stmt
          {
            $node = StmtSeqNode::make({$stmt.node});
          }
        | part=stmtSeq stmt
          {
            $node = $part.node;
            $node->stmts_.push_back($stmt.node);
          }
        ;

stmt    returns [std::shared_ptr<StmtNode> node]
        : expr ';'
          {
            $node = InvokeNode::make($expr.node);
          }
        | var '=' expr ';'
          {
            $node = AssignNode::make($var.node, $expr.node);
          }
        | IF '(' expr ')' stmt
          {
            $node = IfThenElseNode::make($expr.node, $stmt.node);
          }
        | IF '(' expr ')' thenCase=stmt ELSE elseCase=stmt
          {
            $node = IfThenElseNode::make($expr.node, $thenCase.node, $elseCase.node);
          }
        | WHILE '(' expr ')' stmt
          {
            $node = WhileNode::make($expr.node, $stmt.node);
          }
        | RETURN expr ';'
          {
            $node = ReturnNode::make($expr.node);
          }
        | '{' stmtSeq '}'
          {
            $node = $stmtSeq.node;
          }
        ;

expr    returns [std::shared_ptr<ExprNode> node]
        : Integer
          {
            $node = IntegerNode::make(std::stoi($Integer.text));
          }
        | var
          {
            $node = $var.node;
          }
        | Identifier '(' exprs ')'
          {
            $node = CallNode::make($Identifier.text, $exprs.nodes);
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

exprs   returns [std::vector<std::shared_ptr<ExprNode>> nodes]
        : /* empty */
        | expr
          {
            $nodes = {$expr.node};
          }
        | part=exprs ',' expr
          {
            $nodes = $part.nodes;
            $nodes.push_back($expr.node);
          }
        ;

var     returns [std::shared_ptr<VarNode> node]
        : Identifier
          {
            $node = VarNode::make($Identifier.text);
          }
        ;

vars    returns [std::vector<std::shared_ptr<VarNode>> nodes]
        : /* empty */
        | var
          {
            $nodes = {$var.node};
          }
        | part=vars ',' var
          {
            $nodes = $part.nodes;
            $nodes.push_back($var.node);
          }
        ;

