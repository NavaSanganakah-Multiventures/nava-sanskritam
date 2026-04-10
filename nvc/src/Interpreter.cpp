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

    ASTNodeType type = node->getType();

    if (type == ASTNodeType::VariableDeclaration) {
        auto varDecl = static_cast<VariableDeclaration*>(node);
        if (varDecl->init) {
            environment[varDecl->id] = evaluateExpression(varDecl->init.get());
        }
    } else if (type == ASTNodeType::ConstantDeclaration) {
        auto constDecl = static_cast<ConstantDeclaration*>(node);
        if (constDecl->init) {
            environment[constDecl->id] = evaluateExpression(constDecl->init.get());
        }
    } else if (type == ASTNodeType::PrintStatement) {
        auto printStmt = static_cast<PrintStatement*>(node);
        std::string result = "";
        for (size_t i = 0; i < printStmt->expressions.size(); ++i) {
            result += evaluateExpression(printStmt->expressions[i].get());
            if (i < printStmt->expressions.size() - 1) result += " ";
        }
        outputBuffer += result + "\n";
    } else if (type == ASTNodeType::ExpressionStatement) {
        auto exprStmt = static_cast<ExpressionStatement*>(node);
        evaluateExpression(exprStmt->expression.get());
    } else if (type == ASTNodeType::BlockStatement) {
        auto blockStmt = static_cast<BlockStatement*>(node);
        for (const auto& stmt : blockStmt->body) {
            visit(stmt.get());
        }
    } else if (type == ASTNodeType::IfStatement) {
        auto ifStmt = static_cast<IfStatement*>(node);
        std::string condStr = evaluateExpression(ifStmt->condition.get());
        bool condition = false;
        try {
            condition = std::stod(condStr) != 0;
        } catch (...) {
            condition = !condStr.empty();
        }

        if (condition) {
            visit(ifStmt->consequence.get());
        } else if (ifStmt->alternate) {
            visit(ifStmt->alternate.get());
        }
    } else if (type == ASTNodeType::LoopStatement) {
        auto loopStmt = static_cast<LoopStatement*>(node);
        // Initialization
        if (loopStmt->init) {
            if (loopStmt->init->getType() == ASTNodeType::Assignment) {
                auto assign = static_cast<Assignment*>(loopStmt->init.get());
                environment[assign->left] = evaluateExpression(assign->right.get());
            }
        }

        while (true) {
            // Condition Check
            if (loopStmt->test) {
                std::string condStr = evaluateExpression(loopStmt->test.get());
                bool condition = false;
                try {
                    condition = std::stod(condStr) != 0;
                } catch (...) {
                    condition = !condStr.empty();
                }
                if (!condition) break;
            }

            // Body
            visit(loopStmt->body.get());

            // Update
            if (loopStmt->update) {
                if (loopStmt->update->getType() == ASTNodeType::Assignment) {
                    auto assign = static_cast<Assignment*>(loopStmt->update.get());
                    environment[assign->left] = evaluateExpression(assign->right.get());
                }
            }
        }
    } else if (type == ASTNodeType::FunctionDeclaration) {
        // Functions mock
    }
}

std::string Interpreter::evaluateExpression(Expression* expr) {
    if (!expr) return "";

    ASTNodeType type = expr->getType();

    if (type == ASTNodeType::Literal) {
        auto lit = static_cast<Literal*>(expr);
        return lit->value;
    } else if (type == ASTNodeType::Identifier) {
        auto id = static_cast<Identifier*>(expr);
        if (environment.find(id->name) != environment.end()) {
            return environment[id->name];
        }
        return id->name; // fallback
    } else if (type == ASTNodeType::BinaryExpression) {
        auto bin = static_cast<BinaryExpression*>(expr);
        std::string leftStr = evaluateExpression(bin->left.get());
        std::string rightStr = evaluateExpression(bin->right.get());
        
        try {
            double l = std::stod(leftStr);
            double r = std::stod(rightStr);
            
            if (bin->op == "+") return std::to_string(l + r);
            if (bin->op == "-") return std::to_string(l - r);
            if (bin->op == "*") return std::to_string(l * r);
            if (bin->op == "/") return std::to_string(l / r);
            
            // Relational Operators
            if (bin->op == ">") return (l > r) ? "1" : "0";
            if (bin->op == "<") return (l < r) ? "1" : "0";
            if (bin->op == ">=") return (l >= r) ? "1" : "0";
            if (bin->op == "<=") return (l <= r) ? "1" : "0";
            if (bin->op == "==") return (l == r) ? "1" : "0";
            if (bin->op == "!=") return (l != r) ? "1" : "0";
            
        } catch (...) {
            if (bin->op == "+") return leftStr + rightStr; // string concat
            if (bin->op == "==") return (leftStr == rightStr) ? "1" : "0";
            if (bin->op == "!=") return (leftStr != rightStr) ? "1" : "0";
        }
        return leftStr + bin->op + rightStr;
    } else if (type == ASTNodeType::Assignment) {
        auto assign = static_cast<Assignment*>(expr);
        std::string val = evaluateExpression(assign->right.get());
        environment[assign->left] = val;
        return val;
    } else if (type == ASTNodeType::MemberAccess) {
        auto member = static_cast<MemberAccess*>(expr);
        std::string objName = static_cast<Identifier*>(member->object.get())->name;
        std::string key = objName + "." + member->property;
        if (environment.find(key) != environment.end()) {
            return environment[key];
        }
        return "न-प्राप्तम् (Not Found: " + key + ")";
    } else if (type == ASTNodeType::CallExpression) {
        auto call = static_cast<CallExpression*>(expr);
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
