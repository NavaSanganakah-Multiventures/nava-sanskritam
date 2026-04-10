#pragma once
#include "AST.hpp"
#include <map>
#include <string>
#include <vector>

class Interpreter {
public:
    std::string evaluate(Program* program);

private:
    std::map<std::string, std::string> environment;
    std::string outputBuffer;

    void visit(ASTNode* node);
    std::string evaluateExpression(Expression* expr);
};
