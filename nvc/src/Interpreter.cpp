#include "Interpreter.hpp"
#include <iostream>

std::string Interpreter::evaluate(Program* program) {
    outputBuffer = "";
    hasReturned = false;
    returnValue = "";
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
            std::string val = evaluateExpression(constDecl->init.get());
            environment[constDecl->id] = val;
            constants.insert(constDecl->id);
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
            if (hasReturned) break;
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
    } else if (type == ASTNodeType::ReturnStatement) {
        auto retStmt = static_cast<ReturnStatement*>(node);
        if (retStmt->argument) {
            returnValue = evaluateExpression(retStmt->argument.get());
        } else {
            returnValue = "";
        }
        hasReturned = true;
    } else if (type == ASTNodeType::FunctionDeclaration) {
        auto funcDecl = static_cast<FunctionDeclaration*>(node);
        functions[funcDecl->id] = funcDecl;
    } else if (type == ASTNodeType::DarshanamBlock) {
        auto darshanam = static_cast<DarshanamBlock*>(node);
        outputBuffer += "[" + serializeDarshanam(darshanam) + "]";
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
        if (constants.count(assign->left)) {
            throw std::runtime_error("त्रुटिः: नित्य-चरस्य मूल्यं अपरिवर्तनीयम् (Constant is immutable: " + assign->left + ")");
        }
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
        return "न-प्राप्तम् (Property not found: " + key + ")";
    } else if (type == ASTNodeType::CallExpression) {
        auto call = static_cast<CallExpression*>(expr);
        if (call->callee->name == "वद") {
            std::string res = "";
            for (const auto& arg : call->arguments) {
                res += evaluateExpression(arg.get()) + " ";
            }
            outputBuffer += res + "\n";
            return res;
        } else if (call->callee->name == "त्रैराशिकम्") {
            if (call->arguments.size() == 3) {
                std::string aStr = evaluateExpression(call->arguments[0].get());
                std::string bStr = evaluateExpression(call->arguments[1].get());
                std::string cStr = evaluateExpression(call->arguments[2].get());
                try {
                    double a = std::stod(aStr);
                    double b = std::stod(bStr);
                    double c = std::stod(cStr);
                    return std::to_string((b * c) / a);
                } catch (...) { }
            }
            return "0";
        } else if (call->callee->name == "नियमः") {
            if (call->arguments.size() >= 1) {
                std::string condStr = evaluateExpression(call->arguments[0].get());
                bool condition = false;
                try {
                    condition = std::stod(condStr) != 0;
                } catch (...) {
                    condition = !condStr.empty() && condStr != "0";
                }
                if (!condition) {
                    std::string msg = "नियम-भङ्गः (Assertion Failed)";
                    if (call->arguments.size() > 1) {
                        msg = evaluateExpression(call->arguments[1].get());
                    }
                    std::cerr << "TRAP: " << msg << std::endl;
                    exit(1);
                }
            }
            return "1";
        } else if (call->callee->name == "गणनम्") {
            if (call->arguments.size() == 2) {
                std::string lStr = evaluateExpression(call->arguments[0].get());
                std::string rStr = evaluateExpression(call->arguments[1].get());
                try {
                    double l = std::stod(lStr);
                    double r = std::stod(rStr);
                    return std::to_string(l + r);
                } catch (...) {
                    return lStr + rStr;
                }
            }
            return "0";
        } else if (call->callee->name == "वर्गः") {
            if (call->arguments.size() == 1) {
                std::string aStr = evaluateExpression(call->arguments[0].get());
                try {
                    double a = std::stod(aStr);
                    return std::to_string(a * a);
                } catch (...) { }
            }
            return "0";
        } else {
            if (functions.find(call->callee->name) != functions.end()) {
                auto funcDecl = functions[call->callee->name];
                std::map<std::string, std::string> oldEnv = environment;
                for (size_t i = 0; i < funcDecl->params.size() && i < call->arguments.size(); ++i) {
                    environment[funcDecl->params[i]] = evaluateExpression(call->arguments[i].get());
                }

                bool oldHasReturned = hasReturned;
                std::string oldReturnValue = returnValue;
                hasReturned = false;
                returnValue = "";

                visit(funcDecl->body.get());

                std::string result = returnValue;
                hasReturned = oldHasReturned;
                returnValue = oldReturnValue;
                environment = oldEnv;
                return result;
            }
            return "Call to " + call->callee->name;
        }
    }
    return "";
}

std::string Interpreter::serializeDarshanam(DarshanamBlock* block) {
    if (!block) return "{}";
    std::string json = "{\n";
    json += "  \"id\": \"" + block->id + "\",\n";
    json += "  \"elements\": [\n";
    for (size_t i = 0; i < block->elements.size(); ++i) {
        if (block->elements[i]) {
            json += "    " + serializeUI(block->elements[i].get());
            if (i < block->elements.size() - 1) json += ",\n";
        }
    }
    json += "\n  ]\n}";
    return json;
}

std::string Interpreter::serializeUI(DrishyamElement* element) {
    if (!element) return "{}";
    std::string json = "{\n";
    json += "  \"type\": \"" + element->type + "\",\n";
    json += "  \"label\": \"" + element->label + "\",\n";
    json += "  \"color\": \"" + element->color + "\",\n";
    json += "  \"source\": \"" + element->source + "\",\n";
    json += "  \"children\": [\n";
    for (size_t i = 0; i < element->children.size(); ++i) {
        if (element->children[i]) {
            json += "    " + serializeUI(element->children[i].get());
            if (i < element->children.size() - 1) json += ",\n";
        }
    }
    json += "\n  ]\n}";
    return json;
}
