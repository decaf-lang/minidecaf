#ifndef BASE_PARSER_H_
#define BASE_PARSER_H_

#include "antlr4-runtime.h"
#include "ASTNode.h"

class BaseParser : public antlr4::Parser
{
public:
    const std::shared_ptr<ASTNode> &getExpr() const {
        return expr_;
    }

protected:
    BaseParser(antlr4::TokenStream *input)
        : antlr4::Parser(input) {}

    void setExpr(const std::shared_ptr<ASTNode> expr) {
        expr_ = expr;
    }

private:
    std::shared_ptr<ASTNode> expr_;
};

#endif  // BASE_PARSER_H_
