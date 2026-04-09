#pragma once

#include "Lexer.hpp"
#include "AST.hpp"
#include "Grammar.hpp"
#include <memory>

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::unique_ptr<Program> parse();

private:
    std::vector<Token> tokens;
    size_t pos;
    Grammar grammar;

    bool isAtEnd() const;
    Token peek() const;
    Token previous() const;
    bool check(TokenType type, const std::string& value = "") const;
    bool match(TokenType type, const std::string& value = "");
    Token advance();
    Token consume(TokenType type, const std::string& message, const std::string& value = "");

    std::string stripVibhakti(std::string id, std::string* role);

    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<FunctionDeclaration> parseFunctionDeclaration();
    std::unique_ptr<Statement> parseReturnStatement();
    std::unique_ptr<VariableDeclaration> parseVariableDeclaration();
    std::unique_ptr<ConstantDeclaration> parseConstantDeclaration();
    std::unique_ptr<PrintStatement> parsePrintStatement();
    std::unique_ptr<IfStatement> parseIfStatement();
    std::unique_ptr<LoopStatement> parseLoopStatement();
    std::unique_ptr<Statement> parseExpressionStatement();
    std::unique_ptr<BlockStatement> parseBlock();

    std::unique_ptr<Expression> parseExpression();
    std::unique_ptr<Expression> parseEquality();
    std::unique_ptr<Expression> parseRelational();
    std::unique_ptr<Expression> parseAdditive();
    std::unique_ptr<Expression> parseMultiplicative();
    std::unique_ptr<Expression> parseExponential();
    std::unique_ptr<Expression> parsePrimary();
};
