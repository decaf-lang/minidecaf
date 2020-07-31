parser grammar MDParser;

options {
    tokenVocab = MDLexer;
}

@parser::postinclude {

#include <iostream>

}

program : expr
          {
            std::cout << "ret" << std::endl;
          }
        ;

expr    : (integer | add | sub) ';'
        ;

integer : Integer
          {
            std::cout << "ori a0, x0, " << $Integer.text << std::endl;
          }
        ;

add     : lhs=Integer '+' rhs=Integer
          {
            std::cout << "ori t0, x0, " << $lhs.text << std::endl;
            std::cout << "ori t1, x0, " << $rhs.text << std::endl;
            std::cout << "add a0, t0, t1" << std::endl;
          }
        ;

sub     : lhs=Integer '-' rhs=Integer
          {
            std::cout << "ori t0, x0, " << $lhs.text << std::endl;
            std::cout << "ori t1, x0, " << $rhs.text << std::endl;
            std::cout << "sub a0, t0, t1" << std::endl;
          }
        ;

