#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <memory>

enum class TokenType {
    KEYWORD,
    IDENTIFIER,
    NUMBER,
    STRING,
    OPERATOR,
    PUNCTUATION,
    END_OF_FILE,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int col;
};

class Lexer {
public:
    Lexer(const std::string& input);
    std::vector<Token> tokenize();

private:
    std::string input;
    size_t pos;
    int line;
    int col;
    std::vector<Token> tokens;

    std::unordered_set<std::string> keywords;
    std::unordered_map<std::string, std::string> devanagariDigits;

    std::string currentChar();
    void advance(size_t len = 1);

    bool isWhitespace(const std::string& charStr);
    bool isPunctuation(const std::string& charStr);
    bool isOperatorChar(const std::string& charStr);
    bool isDigit(const std::string& charStr);
    bool isDevanagariDigit(const std::string& charStr);
    bool isIdentifierChar(const std::string& charStr);

    std::string readOperator();
    std::string readString(const std::string& quoteChar);
    std::string readNumber();
    std::string readIdentifier();

    std::string getNextUtf8Char();
};
