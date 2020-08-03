parser grammar MDParser;

options {
    tokenVocab = MDLexer;
}

@parser::postinclude {

#include <string>

#include "ASTNode.h"

}

program returns [std::shared_ptr<ProgramNode> node]
        : stmtSeq EOF
          {
            $node = ProgramNode::make({FunctionNode::make("main", $stmtSeq.node)});
          }
        | funcs stmtSeq EOF
          {
            $node = $funcs.node;
            $node->funcs_.push_back(FunctionNode::make("main", $stmtSeq.node));
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
        : Identifier '(' ')' '{' stmtSeq '}'
          {
            $node = FunctionNode::make($Identifier.text, $stmtSeq.node);
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
        | Identifier '=' expr ';'
          {
            $node = AssignNode::make(VarNode::make($Identifier.text), $expr.node);
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
        | Identifier
          {
            $node = VarNode::make($Identifier.text);
          }
        | Identifier '(' ')'
          {
            $node = CallNode::make($Identifier.text);
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

