#pragma once

#include <string>
#include <vector>
#include <memory>

enum class ASTNodeType {
    Program,
    BlockStatement,
    VariableDeclaration,
    ConstantDeclaration,
    PrintStatement,
    IfStatement,
    LoopStatement,
    ReturnStatement,
    ExpressionStatement,
    BinaryExpression,
    Literal,
    Identifier,
    CallExpression,
    Assignment,
    FunctionDeclaration,
    ObjectLiteral,
    MemberAccess,
    DarshanamBlock,
    DrishyamElement
};

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual ASTNodeType getType() const = 0;
};

class Expression : public ASTNode {};
class Statement : public ASTNode {};

class Program : public ASTNode {
public:
    std::vector<std::unique_ptr<Statement>> body;
    ASTNodeType getType() const override { return ASTNodeType::Program; }
};

class BlockStatement : public Statement {
public:
    std::vector<std::unique_ptr<Statement>> body;
    ASTNodeType getType() const override { return ASTNodeType::BlockStatement; }
};

class VariableDeclaration : public Statement {
public:
    std::string id;
    std::unique_ptr<Expression> init;
    ASTNodeType getType() const override { return ASTNodeType::VariableDeclaration; }
};

class ConstantDeclaration : public Statement {
public:
    std::string id;
    std::unique_ptr<Expression> init;
    ASTNodeType getType() const override { return ASTNodeType::ConstantDeclaration; }
};

class PrintStatement : public Statement {
public:
    std::unique_ptr<Expression> expression;
    ASTNodeType getType() const override { return ASTNodeType::PrintStatement; }
};

class IfStatement : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> consequence;
    std::unique_ptr<Statement> alternate;
    ASTNodeType getType() const override { return ASTNodeType::IfStatement; }
};

class LoopStatement : public Statement {
public:
    std::unique_ptr<ASTNode> init;
    std::unique_ptr<Expression> test;
    std::unique_ptr<ASTNode> update;
    std::unique_ptr<Statement> body;
    ASTNodeType getType() const override { return ASTNodeType::LoopStatement; }
};

class ReturnStatement : public Statement {
public:
    std::unique_ptr<Expression> argument;
    ASTNodeType getType() const override { return ASTNodeType::ReturnStatement; }
};

class ExpressionStatement : public Statement {
public:
    std::unique_ptr<Expression> expression;
    ASTNodeType getType() const override { return ASTNodeType::ExpressionStatement; }
};

class BinaryExpression : public Expression {
public:
    std::string op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    ASTNodeType getType() const override { return ASTNodeType::BinaryExpression; }
};

class Assignment : public Expression {
public:
    std::string left;
    std::unique_ptr<Expression> right;
    ASTNodeType getType() const override { return ASTNodeType::Assignment; }
};

class Literal : public Expression {
public:
    std::string value;
    bool isString;
    ASTNodeType getType() const override { return ASTNodeType::Literal; }
};

class Identifier : public Expression {
public:
    std::string name;
    std::string role; // "Karta", "Karma", "Shashti", etc.
    int vachana;      // 1: single, 2: dual, 3: plural
    ASTNodeType getType() const override { return ASTNodeType::Identifier; }
};

class CallExpression : public Expression {
public:
    std::unique_ptr<Identifier> callee;
    std::vector<std::unique_ptr<Expression>> arguments;
    std::string lakara; // "Lat", "Lrt" (Async), etc.
    int vachana;        // 3 for parallel execution
    ASTNodeType getType() const override { return ASTNodeType::CallExpression; }
};

class FunctionDeclaration : public Statement {
public:
    std::string id;
    std::vector<std::string> params;
    std::unique_ptr<BlockStatement> body;
    std::string lakara; // "Lat", "Lrt", etc.
    std::string pada;   // "Parasmaipada", "Atmanepada"
    std::string gana;   // "Bhvadi", etc.
    ASTNodeType getType() const override { return ASTNodeType::FunctionDeclaration; }
};

class DrishyamElement : public ASTNode {
public:
    std::string type; // "Button", "Text", etc.
    std::vector<std::unique_ptr<Expression>> pos; // x, y, w, h
    std::string label;
    std::string color;
    ASTNodeType getType() const override { return ASTNodeType::DrishyamElement; }
};

class DarshanamBlock : public Statement {
public:
    std::string id;
    std::vector<std::unique_ptr<DrishyamElement>> elements;
    ASTNodeType getType() const override { return ASTNodeType::DarshanamBlock; }
};

class ObjectLiteral : public Expression {
public:
    struct Property {
        std::string key;
        std::unique_ptr<Expression> value;
    };
    std::vector<Property> properties;
    ASTNodeType getType() const override { return ASTNodeType::ObjectLiteral; }
};

class MemberAccess : public Expression {
public:
    std::unique_ptr<Expression> object;
    std::string property;
    ASTNodeType getType() const override { return ASTNodeType::MemberAccess; }
};
