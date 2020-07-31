parser grammar MDParser;

options {
    tokenVocab = MDLexer;
}

@parser::postinclude {

#include <iostream>

}

program : Integer
          {
            std::cout << "ori a0, x0, " << $Integer.text << std::endl;
            std::cout << "ret" << std::endl;
          }
        ;

