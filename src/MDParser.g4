parser grammar MDParser;

options {
    tokenVocab = MDLexer;
    superClass = BaseParser;
}

@parser::postinclude {

#include "BaseParser.h"

}

program : stmt
          {
            std::cout << "addi sp, sp, 8" << std::endl;
            std::cout << "ret" << std::endl;
          }
        ;

stmt    : expr ';'
          {
            std::cout << $expr.code;
          }
        ;

expr    returns [std::string code]
        : Integer
          {
            $code = "ori a0, x0, " + $Integer.text + "\n" + push();
          }
        | '(' expr ')'
          {
            $code = $expr.code;
          }
        | '-' expr
          {
            $code = $expr.code;
            $code += pop() + "sub a0, x0, t0\n" + push();
          }
        | '+' expr
          {
            $code = $expr.code;
          }
        | lhs=expr op=('*' | '/') rhs=expr
          {
            $code = $lhs.code + $rhs.code;
            if ($op.text == "*") {
                $code += pop2() + "mul a0, t0, t1\n" + push();
            } else {
                $code += pop2() + "div a0, t0, t1\n" + push();
            }
          }
        | lhs=expr op=('+' | '-') rhs=expr
          {
            $code = $lhs.code + $rhs.code;
            if ($op.text == "+") {
                $code += pop2() + "add a0, t0, t1\n" + push();
            } else {
                $code += pop2() + "sub a0, t0, t1\n" + push();
            }
          }
        ;

