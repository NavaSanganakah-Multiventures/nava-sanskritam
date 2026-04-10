#include "Interpreter.hpp"
#include <iostream>

std::string Interpreter::evaluate(Program* program) {
    outputBuffer = "";
    for (const auto& stmt : program->body) {
        visit(stmt.get());
    }
    return outputBuffer;
}

void Interpreter::visit(ASTNode* node) {
    if (!node) return;

    if (auto varDecl = dynamic_cast<VariableDeclaration*>(node)) {
        if (varDecl->init) {
            environment[varDecl->id] = evaluateExpression(varDecl->init.get());
        }
    } else if (auto constDecl = dynamic_cast<ConstantDeclaration*>(node)) {
        if (constDecl->init) {
            environment[constDecl->id] = evaluateExpression(constDecl->init.get());
        }
    } else if (auto printStmt = dynamic_cast<PrintStatement*>(node)) {
        std::string val = evaluateExpression(printStmt->expression.get());
        outputBuffer += val + "\n";
    } else if (auto exprStmt = dynamic_cast<ExpressionStatement*>(node)) {
        evaluateExpression(exprStmt->expression.get());
    } else if (auto blockStmt = dynamic_cast<BlockStatement*>(node)) {
        for (const auto& stmt : blockStmt->body) {
            visit(stmt.get());
        }
    } else if (auto funcDecl = dynamic_cast<FunctionDeclaration*>(node)) {
        // Functions mock
    }
}

std::string Interpreter::evaluateExpression(Expression* expr) {
    if (!expr) return "";

    if (auto lit = dynamic_cast<Literal*>(expr)) {
        return lit->value;
    } else if (auto id = dynamic_cast<Identifier*>(expr)) {
        if (environment.find(id->name) != environment.end()) {
            return environment[id->name];
        }
        return id->name; // fallback
    } else if (auto bin = dynamic_cast<BinaryExpression*>(expr)) {
        std::string left = evaluateExpression(bin->left.get());
        std::string right = evaluateExpression(bin->right.get());
        // Simple numeric interpretation
        try {
            double l = std::stod(left);
            double r = std::stod(right);
            if (bin->op == "+") return std::to_string(l + r);
            if (bin->op == "-") return std::to_string(l - r);
            if (bin->op == "*") return std::to_string(l * r);
            if (bin->op == "/") return std::to_string(l / r);
        } catch (...) {
            if (bin->op == "+") return left + right; // string concat
        }
        return left + bin->op + right;
    } else if (auto call = dynamic_cast<CallExpression*>(expr)) {
        if (call->callee->name == "वद") {
            std::string res = "";
            for (const auto& arg : call->arguments) {
                res += evaluateExpression(arg.get()) + " ";
            }
            outputBuffer += res + "\n";
            return res;
        } else {
            // Simulated function call evaluating arguments
            return "Call to " + call->callee->name;
        }
    }
    return "";
}
