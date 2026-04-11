#include "Lexer.hpp"
#include <stdexcept>
#include <iostream>

Lexer::Lexer(const std::string& input) : input(input), pos(0), line(1), col(1) {
    keywords = {"अस्ति", "वद", "यदि", "तर्हि", "चक्र", "अन्यथा", "फलम्", "विधिः", "नित्य", "दर्शनम्", "दृश्यम्", "मंजूषा", "सूची", "चित्त्रम्", "प्रविष्टिः", "क्रिया", "सूत्रम्", "योगः"};
    devanagariDigits = {
        {"०", "0"}, {"१", "1"}, {"२", "2"}, {"३", "3"}, {"४", "4"},
        {"५", "5"}, {"६", "6"}, {"७", "7"}, {"८", "8"}, {"९", "9"}
    };
}

std::string Lexer::getNextUtf8Char() {
    if (pos >= input.length()) return "";

    unsigned char c = input[pos];
    size_t len = 1;
    if ((c & 0xE0) == 0xC0) len = 2;
    else if ((c & 0xF0) == 0xE0) len = 3;
    else if ((c & 0xF8) == 0xF0) len = 4;

    if (pos + len > input.length()) {
        len = input.length() - pos; // fallback
    }

    return input.substr(pos, len);
}

void Lexer::advance(size_t len) {
    for (size_t i = 0; i < len; ++i) {
        if (pos < input.length()) {
            if (input[pos] == '\n') {
                line++;
                col = 1;
            } else {
                col++;
            }
            pos++;
        }
    }
}

std::string Lexer::currentChar() {
    return getNextUtf8Char();
}

bool Lexer::isWhitespace(const std::string& charStr) {
    if (charStr.length() == 1) {
        char c = charStr[0];
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }
    return false;
}

bool Lexer::isPunctuation(const std::string& charStr) {
    if (charStr.length() == 1) {
        char c = charStr[0];
        return c == '(' || c == ')' || c == '{' || c == '}' ||
               c == '[' || c == ']' || c == ',' || c == ';';
    }
    return false;
}

bool Lexer::isOperatorChar(const std::string& charStr) {
    if (charStr.length() == 1) {
        char c = charStr[0];
        return c == '=' || c == '+' || c == '-' || c == '*' ||
               c == '/' || c == '<' || c == '>' || c == '!' || c == '^';
    }
    return false;
}

bool Lexer::isDigit(const std::string& charStr) {
    if (charStr.length() == 1) {
        char c = charStr[0];
        return c >= '0' && c <= '9';
    }
    return false;
}

bool Lexer::isDevanagariDigit(const std::string& charStr) {
    return devanagariDigits.find(charStr) != devanagariDigits.end();
}

bool Lexer::isIdentifierChar(const std::string& charStr) {
    if (charStr.empty()) return false;
    if (isWhitespace(charStr) || isPunctuation(charStr) || isOperatorChar(charStr) || charStr == "\"" || charStr == "'") {
        return false;
    }
    return true;
}

std::string Lexer::readOperator() {
    std::string op = "";
    while (pos < input.length()) {
        std::string charStr = currentChar();
        if (isOperatorChar(charStr)) {
            op += charStr;
            advance(charStr.length());
        } else {
            break;
        }
    }
    return op;
}

std::string Lexer::readString(const std::string& quoteChar) {
    int startLine = line;
    int startCol = col;
    advance(quoteChar.length()); // skip opening quote
    std::string str = "";
    while (pos < input.length()) {
        std::string charStr = currentChar();
        if (charStr == quoteChar) {
            break;
        }
        str += charStr;
        advance(charStr.length());
    }
    if (pos >= input.length()) {
        throw std::runtime_error("पद-सङ्केत-त्रुटिः: अपूर्णः शब्दः  - पङ्क्तिः " + std::to_string(startLine) + ", स्तम्भः " + std::to_string(startCol));
    }
    advance(quoteChar.length()); // skip closing quote
    return str;
}

std::string Lexer::readNumber() {
    std::string numStr = "";
    while (pos < input.length()) {
        std::string charStr = currentChar();
        if (isDigit(charStr)) {
            numStr += charStr;
            advance(charStr.length());
        } else if (isDevanagariDigit(charStr)) {
            numStr += devanagariDigits[charStr];
            advance(charStr.length());
        } else {
            break;
        }
    }
    return numStr;
}

std::string Lexer::readIdentifier() {
    std::string id = "";
    while (pos < input.length()) {
        std::string charStr = currentChar();
        if (isIdentifierChar(charStr)) {
            id += charStr;
            advance(charStr.length());
        } else {
            break;
        }
    }
    return id;
}

std::vector<Token> Lexer::tokenize() {
    while (pos < input.length()) {
        std::string charStr = currentChar();

        if (isWhitespace(charStr)) {
            advance(charStr.length());
            continue;
        }

        if (isPunctuation(charStr)) {
            tokens.push_back({TokenType::PUNCTUATION, charStr, line, col});
            advance(charStr.length());
            continue;
        }

        if (isOperatorChar(charStr)) {
            std::string op = readOperator();
            if (op == "//") {
                while (pos < input.length() && currentChar() != "\n") {
                    advance(currentChar().length());
                }
                continue;
            }
            tokens.push_back({TokenType::OPERATOR, op, line, col});
            continue;
        }

        if (charStr == "\"" || charStr == "'") {
            std::string str = readString(charStr);
            tokens.push_back({TokenType::STRING, str, line, col});
            continue;
        }

        if (isDigit(charStr) || isDevanagariDigit(charStr)) {
            std::string num = readNumber();
            tokens.push_back({TokenType::NUMBER, num, line, col});
            continue;
        }

        if (isIdentifierChar(charStr)) {
            int startLine = line;
            int startCol = col;
            std::string id = readIdentifier();

            if (keywords.find(id) != keywords.end()) {
                tokens.push_back({TokenType::KEYWORD, id, startLine, startCol});
            } else {
                // Do not strip suffixes here anymore. Pass raw identifier to Parser.
                tokens.push_back({TokenType::IDENTIFIER, id, startLine, startCol});
            }
            continue;
        }

        throw std::runtime_error("पद-सङ्केत-त्रुटिः: अनुचितः वर्णः '" + charStr + "'  - पङ्क्तिः " + std::to_string(line) + ", स्तम्भः " + std::to_string(col));
    }

    tokens.push_back({TokenType::END_OF_FILE, "EOF", line, col});
    return tokens;
}
