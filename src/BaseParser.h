#ifndef BASE_PARSER_H_
#define BASE_PARSER_H_

#include <string>
#include <iostream>
#include "antlr4-runtime.h"

class BaseParser : public antlr4::Parser
{
protected:
    BaseParser(antlr4::TokenStream *input)
        : antlr4::Parser(input) {}

    std::string push() {
        return "addi sp, sp, -8\n"
               "sd a0, (sp)\n";
    }

    std::string pop() {
        return "ld t0, (sp)\n"
               "addi sp, sp, 8\n";
    }

    std::string pop2() {
        return "ld t0, 8(sp)\n"
               "ld t1, (sp)\n"
               "addi sp, sp, 16\n";
    }
};

#endif // BASE_PARSER_H_
