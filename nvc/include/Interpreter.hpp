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
    std::map<std::string, FunctionDeclaration*> functions;
    std::string outputBuffer;
    bool hasReturned = false;
    std::string returnValue = "";

    void visit(ASTNode* node);
    std::string evaluateExpression(Expression* expr);
};
