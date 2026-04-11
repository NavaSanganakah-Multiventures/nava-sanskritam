#pragma once

#include "AST.hpp"
#include <map>
#include <set>
#include <string>
#include <vector>

class Interpreter {
public:
    std::string evaluate(Program* program);

private:
    std::map<std::string, std::string> environment;
    std::map<std::string, FunctionDeclaration*> functions;
    std::set<std::string> constants;
    std::string outputBuffer;
    bool hasReturned = false;
    std::string returnValue = "";

    void visit(ASTNode* node);
    std::string evaluateExpression(Expression* expr);

    // Darshanam JSON Serialization for Web Playground
    std::string serializeDarshanam(DarshanamBlock* block);
    std::string serializeUI(DrishyamElement* element);

#ifndef EMSCRIPTEN
    llvm::Value* generateCallExpression(CallExpression* node); // Not used in Interpreter but part of structure
#endif
};
