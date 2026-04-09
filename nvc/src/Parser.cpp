#include "Parser.hpp"
#include <stdexcept>
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), pos(0) {}

std::unique_ptr<Program> Parser::parse() {
    auto program = std::make_unique<Program>();
    while (!isAtEnd()) {
        program->body.push_back(parseStatement());
    }
    return program;
}

std::string Parser::stripVibhakti(std::string id, std::string* role) {
    std::u32string u32id = grammar.toUtf32(id);
    Grammar::WordMeta meta = grammar.analyzeSubanta(u32id);
    
    if (role) {
        switch (meta.vibhakti) {
            case Grammar::Vibhakti::PRATHAMA: *role = "Karta"; break;
            case Grammar::Vibhakti::DWITIYA: *role = "Karma"; break;
            case Grammar::Vibhakti::TRITIYA: *role = "Karana"; break;
            case Grammar::Vibhakti::CHATURTHI: *role = "Sampradana"; break;
            case Grammar::Vibhakti::PANCHAMI: *role = "Apadaana"; break;
            case Grammar::Vibhakti::SHASHTI: *role = "Shashti"; break;
            case Grammar::Vibhakti::SAPTAMI: *role = "Adhikarana"; break;
            default: *role = "None"; break;
        }
    }
    
    return grammar.toUtf8(meta.root);
}

std::unique_ptr<Statement> Parser::parseStatement() {
    if (match(TokenType::KEYWORD, "विधिः") || 
        match(TokenType::KEYWORD, "क्रिया") || 
        match(TokenType::KEYWORD, "सूत्रम्") || 
        match(TokenType::KEYWORD, "योगः")) return parseFunctionDeclaration();
    if (match(TokenType::KEYWORD, "फलम्")) return parseReturnStatement();
    if (match(TokenType::KEYWORD, "अस्ति")) return parseVariableDeclaration();
    if (match(TokenType::KEYWORD, "नित्य")) return parseConstantDeclaration();
    if (match(TokenType::KEYWORD, "दर्शनम्")) return parseDarshanamBlock();
    if (match(TokenType::KEYWORD, "मंजूषा")) return parseManjushaElement();
    if (match(TokenType::KEYWORD, "सूची")) return parseSuchiElement();
    if (match(TokenType::KEYWORD, "चित्त्रम्")) return parseChittramElement();
    if (match(TokenType::KEYWORD, "प्रविष्टिः")) return parsePrashtihElement();
    if (match(TokenType::KEYWORD, "वद")) return parsePrintStatement();
    if (match(TokenType::KEYWORD, "यदि")) return parseIfStatement();
    if (match(TokenType::KEYWORD, "चक्र")) return parseLoopStatement();
    return parseExpressionStatement();
}

std::unique_ptr<FunctionDeclaration> Parser::parseFunctionDeclaration() {
    Token idToken = consume(TokenType::IDENTIFIER, "Expected function name.");
    consume(TokenType::PUNCTUATION, "Expected '(' after function name.", "(");
    std::vector<std::string> params;
    if (!check(TokenType::PUNCTUATION, ")")) {
        do {
            if (!params.empty()) {
                consume(TokenType::PUNCTUATION, "Expected ',' between parameters.", ",");
            }
            params.push_back(grammar.toUtf8(grammar.analyzeSubanta(grammar.toUtf32(consume(TokenType::IDENTIFIER, "Expected parameter name.").value)).root));
        } while (check(TokenType::PUNCTUATION, ","));
    }
    consume(TokenType::PUNCTUATION, "Expected ')' after parameters.", ")");

    auto meta = grammar.analyzeTinanta(grammar.toUtf32(idToken.value));
    auto decl = std::make_unique<FunctionDeclaration>();
    decl->id = grammar.toUtf8(meta.root);
    decl->params = std::move(params);
    
    // SUL v13.0: Tag Lakara/Pada/Gana
    if (meta.lakara == Grammar::Lakara::LAT) decl->lakara = "Lat";
    else if (meta.lakara == Grammar::Lakara::LRT) decl->lakara = "Lrt";
    else if (meta.lakara == Grammar::Lakara::VIDHI_LIN) decl->lakara = "Vidhi-Lin";
    else decl->lakara = "Unknown";
    
    decl->ipada = (meta.pada == Grammar::Pada::PARASMAIPADA) ? "Parasmaipada" : "Atmanepada";
    decl->body = parseBlock();
    return decl;
}

std::unique_ptr<Statement> Parser::parseReturnStatement() {
    auto value = parseExpression();
    consume(TokenType::PUNCTUATION, "Expected ';' after return value.", ";");
    auto stmt = std::make_unique<ReturnStatement>();
    stmt->argument = std::move(value);
    return stmt;
}

std::unique_ptr<VariableDeclaration> Parser::parseVariableDeclaration() {
    Token idToken = consume(TokenType::IDENTIFIER, "Expected variable name after 'अस्ति'.");
    consume(TokenType::OPERATOR, "Expected '=' after variable name.", "=");
    auto init = parseExpression();
    consume(TokenType::PUNCTUATION, "Expected ';' after variable declaration.", ";");

    auto decl = std::make_unique<VariableDeclaration>();
    decl->id = stripVibhakti(idToken.value, nullptr);
    decl->init = std::move(init);
    return decl;
}

std::unique_ptr<ConstantDeclaration> Parser::parseConstantDeclaration() {
    Token idToken = consume(TokenType::IDENTIFIER, "Expected variable name after 'नित्य'.");
    consume(TokenType::OPERATOR, "Expected '=' after variable name.", "=");
    auto init = parseExpression();
    consume(TokenType::PUNCTUATION, "Expected ';' after constant declaration.", ";");

    auto decl = std::make_unique<ConstantDeclaration>();
    decl->id = stripVibhakti(idToken.value, nullptr);
    decl->init = std::move(init);
    return decl;
}

std::unique_ptr<PrintStatement> Parser::parsePrintStatement() {
    consume(TokenType::PUNCTUATION, "Expected '(' after 'वद'.", "(");
    auto expr = parseExpression();
    consume(TokenType::PUNCTUATION, "Expected ')' after print expression.", ")");
    consume(TokenType::PUNCTUATION, "Expected ';' after print statement.", ";");

    auto stmt = std::make_unique<PrintStatement>();
    stmt->expression = std::move(expr);
    return stmt;
}

std::unique_ptr<IfStatement> Parser::parseIfStatement() {
    consume(TokenType::PUNCTUATION, "Expected '(' after 'यदि'.", "(");
    auto cond = parseExpression();
    consume(TokenType::PUNCTUATION, "Expected ')' after condition.", ")");

    consume(TokenType::KEYWORD, "Expected 'तर्हि' after condition in if statement.", "तर्हि");

    std::unique_ptr<Statement> cons;
    if (check(TokenType::PUNCTUATION, "{")) {
        cons = parseBlock();
    } else {
        cons = parseStatement();
    }

    std::unique_ptr<Statement> alt = nullptr;
    if (match(TokenType::KEYWORD, "अन्यथा")) {
        if (check(TokenType::PUNCTUATION, "{")) {
            alt = parseBlock();
        } else {
            alt = parseStatement();
        }
    }

    auto stmt = std::make_unique<IfStatement>();
    stmt->condition = std::move(cond);
    stmt->consequence = std::move(cons);
    stmt->alternate = std::move(alt);
    return stmt;
}

std::unique_ptr<LoopStatement> Parser::parseLoopStatement() {
    consume(TokenType::PUNCTUATION, "Expected '(' after 'चक्र'.", "(");

    std::unique_ptr<ASTNode> init = nullptr;
    if (!check(TokenType::PUNCTUATION, ";")) {
        if (check(TokenType::IDENTIFIER)) {
            Token idToken = consume(TokenType::IDENTIFIER, "");
            consume(TokenType::OPERATOR, "Expected '=' in loop initialization.", "=");
            auto value = parseExpression();
            auto assign = std::make_unique<Assignment>();
            assign->left = stripVibhakti(idToken.value, nullptr);
            assign->right = std::move(value);
            init = std::move(assign);
        } else {
            throw std::runtime_error("Invalid initialization in loop.");
        }
    }
    consume(TokenType::PUNCTUATION, "Expected ';' after loop initialization.", ";");

    std::unique_ptr<Expression> test = nullptr;
    if (!check(TokenType::PUNCTUATION, ";")) {
        test = parseExpression();
    }
    consume(TokenType::PUNCTUATION, "Expected ';' after loop condition.", ";");

    std::unique_ptr<ASTNode> update = nullptr;
    if (!check(TokenType::PUNCTUATION, ")")) {
        if (check(TokenType::IDENTIFIER)) {
            Token idToken = consume(TokenType::IDENTIFIER, "");
            consume(TokenType::OPERATOR, "Expected '=' in loop update.", "=");
            auto value = parseExpression();
            auto assign = std::make_unique<Assignment>();
            assign->left = stripVibhakti(idToken.value, nullptr);
            assign->right = std::move(value);
            update = std::move(assign);
        }
    }
    consume(TokenType::PUNCTUATION, "Expected ')' after loop update.", ")");

    std::unique_ptr<Statement> body;
    if (check(TokenType::PUNCTUATION, "{")) {
        body = parseBlock();
    } else {
        auto block = std::make_unique<BlockStatement>();
        block->body.push_back(parseStatement());
        body = std::move(block);
    }

    auto stmt = std::make_unique<LoopStatement>();
    stmt->init = std::move(init);
    stmt->test = std::move(test);
    stmt->update = std::move(update);
    stmt->body = std::move(body);
    return stmt;
}

std::unique_ptr<Statement> Parser::parseExpressionStatement() {
    if (check(TokenType::IDENTIFIER)) {
        if (pos + 1 < tokens.size() && tokens[pos + 1].type == TokenType::OPERATOR && tokens[pos + 1].value == "=") {
            Token idToken = consume(TokenType::IDENTIFIER, "");
            consume(TokenType::OPERATOR, "Expected '='", "=");
            auto right = parseExpression();
            consume(TokenType::PUNCTUATION, "Expected ';'", ";");

            auto assign = std::make_unique<Assignment>();
            assign->left = stripVibhakti(idToken.value, nullptr);
            assign->right = std::move(right);

            auto stmt = std::make_unique<ExpressionStatement>();
            stmt->expression = std::move(assign);
            return stmt;
        }
    }

    auto expr = parseExpression();
    consume(TokenType::PUNCTUATION, "Expected ';' after expression.", ";");
    auto stmt = std::make_unique<ExpressionStatement>();
    stmt->expression = std::move(expr);
    return stmt;
}

std::unique_ptr<BlockStatement> Parser::parseBlock() {
    consume(TokenType::PUNCTUATION, "Expected '{'.", "{");
    auto block = std::make_unique<BlockStatement>();
    while (!check(TokenType::PUNCTUATION, "}") && !isAtEnd()) {
        block->body.push_back(parseStatement());
    }
    consume(TokenType::PUNCTUATION, "Expected '}'.", "}");
    return block;
}

std::unique_ptr<Expression> Parser::parseExpression() {
    return parseEquality();
}

std::unique_ptr<Expression> Parser::parseEquality() {
    auto expr = parseRelational();
    while (match(TokenType::OPERATOR, "==") || match(TokenType::OPERATOR, "!=")) {
        std::string op = previous().value;
        auto right = parseRelational();
        auto bin = std::make_unique<BinaryExpression>();
        bin->op = op;
        bin->left = std::move(expr);
        bin->right = std::move(right);
        expr = std::move(bin);
    }
    return expr;
}

std::unique_ptr<Expression> Parser::parseRelational() {
    auto expr = parseAdditive();
    while (match(TokenType::OPERATOR, "<") || match(TokenType::OPERATOR, "<=") ||
           match(TokenType::OPERATOR, ">") || match(TokenType::OPERATOR, ">=")) {
        std::string op = previous().value;
        auto right = parseAdditive();
        auto bin = std::make_unique<BinaryExpression>();
        bin->op = op;
        bin->left = std::move(expr);
        bin->right = std::move(right);
        expr = std::move(bin);
    }
    return expr;
}

std::unique_ptr<Expression> Parser::parseAdditive() {
    auto expr = parseMultiplicative();
    while (match(TokenType::OPERATOR, "+") || match(TokenType::OPERATOR, "-")) {
        std::string op = previous().value;
        auto right = parseMultiplicative();
        auto bin = std::make_unique<BinaryExpression>();
        bin->op = op;
        bin->left = std::move(expr);
        bin->right = std::move(right);
        expr = std::move(bin);
    }
    return expr;
}

std::unique_ptr<Expression> Parser::parseMultiplicative() {
    auto expr = parseExponential();
    while (match(TokenType::OPERATOR, "*") || match(TokenType::OPERATOR, "/")) {
        std::string op = previous().value;
        auto right = parseExponential();
        auto bin = std::make_unique<BinaryExpression>();
        bin->op = op;
        bin->left = std::move(expr);
        bin->right = std::move(right);
        expr = std::move(bin);
    }
    return expr;
}

std::unique_ptr<Expression> Parser::parseExponential() {
    auto expr = parsePrimary();
    while (match(TokenType::OPERATOR, "^")) {
        std::string op = previous().value;
        auto right = parsePrimary();
        auto bin = std::make_unique<BinaryExpression>();
        bin->op = op;
        bin->left = std::move(expr);
        bin->right = std::move(right);
        expr = std::move(bin);
    }
    return expr;
}

std::unique_ptr<Expression> Parser::parsePrimary() {
    if (match(TokenType::NUMBER)) {
        auto lit = std::make_unique<Literal>();
        lit->value = previous().value;
        lit->isString = false;
        return lit;
    }
    if (match(TokenType::STRING)) {
        auto lit = std::make_unique<Literal>();
        lit->value = previous().value;
        lit->isString = true;
        return lit;
    }
    if (match(TokenType::IDENTIFIER)) {
        std::string role;
        Token idToken = previous();
        Grammar::WordMeta meta = grammar.analyzeSubanta(grammar.toUtf32(idToken.value));
        
        auto expr = std::make_unique<Identifier>();
        expr->name = grammar.toUtf8(meta.root);
        
        switch (meta.vibhakti) {
            case Grammar::Vibhakti::PRATHAMA: expr->role = "Karta"; break;
            case Grammar::Vibhakti::DWITIYA: expr->role = "Karma"; break;
            case Grammar::Vibhakti::TRITIYA: expr->role = "Karana"; break;
            case Grammar::Vibhakti::CHATURTHI: expr->role = "Sampradana"; break;
            case Grammar::Vibhakti::PANCHAMI: expr->role = "Apadaana"; break;
            case Grammar::Vibhakti::SHASHTI: expr->role = "Shashti"; break;
            case Grammar::Vibhakti::SAPTAMI: expr->role = "Adhikarana"; break;
            default: expr->role = "None"; break;
        }
        expr->vachana = meta.vachana;

        // SUL v12.0: Native Shashti (Genitive) Property Access
        if (expr->role == "Shashti") {
            if (check(TokenType::IDENTIFIER)) {
                auto propToken = consume(TokenType::IDENTIFIER, "Expected property name after Shashti declension.");
                auto member = std::make_unique<MemberAccess>();
                member->object = std::move(expr);
                member->property = propToken.value;
                return member;
            }
        }

        if (match(TokenType::PUNCTUATION, "(")) {
            auto call = std::make_unique<CallExpression>();
            auto meta = grammar.analyzeTinanta(grammar.toUtf32(idToken.value));
            
            auto calleeId = std::make_unique<Identifier>();
            calleeId->name = grammar.toUtf8(meta.root);
            call->callee = std::move(calleeId);
            
            // SUL v13.0: Call-time Lakara/Vachana detection
            if (meta.lakara == Grammar::Lakara::LRT) call->lakara = "Lrt"; 
            call->vachana = meta.vachana;

            if (!check(TokenType::PUNCTUATION, ")")) {
                do {
                    if (!call->arguments.empty()) {
                        consume(TokenType::PUNCTUATION, "Expected ',' between arguments.", ",");
                    }
                    call->arguments.push_back(parseExpression());
                } while (check(TokenType::PUNCTUATION, ","));
            }
            consume(TokenType::PUNCTUATION, "Expected ')' after arguments.", ")");
            return call;
        }
        return expr;
    }
    if (match(TokenType::PUNCTUATION, "(")) {
        auto expr = parseExpression();
        consume(TokenType::PUNCTUATION, "Expected ')' after expression.", ")");
        return expr;
    }
    throw std::runtime_error("Parser Error: Unexpected token '" + peek().value + "' at line " + std::to_string(peek().line) + ", col " + std::to_string(peek().col));
}

std::unique_ptr<DarshanamBlock> Parser::parseDarshanamBlock() {
    Token idToken = consume(TokenType::IDENTIFIER, "Expected Darshanam name.");
    consume(TokenType::PUNCTUATION, "Expected '{' after Darshanam name.", "{");
    
    auto block = std::make_unique<DarshanamBlock>();
    block->id = idToken.value;

    while (!check(TokenType::PUNCTUATION, "}")) {
        if (match(TokenType::KEYWORD, "दृश्यम्")) {
            block->elements.push_back(parseDrishyamElement());
        } else {
            advance(); // Skip unknown for now
        }
    }
    consume(TokenType::PUNCTUATION, "Expected '}' after Darshanam block.", "}");
    return block;
}

std::unique_ptr<DrishyamElement> Parser::parseDrishyamElement() {
    Token typeToken = consume(TokenType::IDENTIFIER, "Expected widget type (e.g. Button).");
    auto element = std::make_unique<DrishyamElement>();
    element->type = typeToken.value;

    if (match(TokenType::PUNCTUATION, "(")) {
        // Parse x, y coordinates
        element->pos.push_back(parseExpression());
        if (match(TokenType::PUNCTUATION, ",")) {
            element->pos.push_back(parseExpression());
        }
        consume(TokenType::PUNCTUATION, "Expected ')' after coordinates.", ")");
    }

    if (match(TokenType::PUNCTUATION, "{")) {
        while (!check(TokenType::PUNCTUATION, "}")) {
            if (match(TokenType::KEYWORD, "दृश्यम्") || 
                match(TokenType::KEYWORD, "मंजूषा") || 
                match(TokenType::KEYWORD, "सूची")) {
                // Recursive nesting of children
                element->children.push_back(parseDrishyamElement());
            } else {
                Token key = consume(TokenType::IDENTIFIER, "Expected attribute name.");
                consume(TokenType::PUNCTUATION, "Expected ':' after attribute.", ":");
                
                if (key.value == "नाम") {
                    element->label = consume(TokenType::LITERAL, "Expected label string.").value;
                } else if (key.value == "रङ्गः") {
                    element->color = consume(TokenType::IDENTIFIER, "Expected color name.").value;
                } else if (key.value == "स्रोतस") {
                    element->source = consume(TokenType::LITERAL, "Expected source URL.").value;
                }
                if (check(TokenType::PUNCTUATION, ";")) advance();
            }
        }
        consume(TokenType::PUNCTUATION, "Expected '}' after Drishyam attributes.", "}");
    }
    return element;
}

std::unique_ptr<DrishyamElement> Parser::parseManjushaElement() {
    auto element = std::make_unique<DrishyamElement>();
    element->type = "Box";
    return parseDrishyamElement(); 
}

std::unique_ptr<DrishyamElement> Parser::parseSuchiElement() {
    auto element = std::make_unique<DrishyamElement>();
    element->type = "List";
    return parseDrishyamElement();
}

std::unique_ptr<DrishyamElement> Parser::parseChittramElement() {
    auto element = std::make_unique<DrishyamElement>();
    element->type = "Image";
    return parseDrishyamElement();
}

std::unique_ptr<DrishyamElement> Parser::parsePrashtihElement() {
    auto element = std::make_unique<DrishyamElement>();
    element->type = "Input";
    return parseDrishyamElement();
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() const {
    if (pos >= tokens.size()) return tokens.back();
    return tokens[pos];
}

Token Parser::previous() const {
    return tokens[pos - 1];
}

bool Parser::check(TokenType type, const std::string& value) const {
    if (isAtEnd()) return false;
    Token token = peek();
    if (token.type != type) return false;
    if (value != "" && token.value != value) return false;
    return true;
}

bool Parser::match(TokenType type, const std::string& value) {
    if (check(type, value)) {
        advance();
        return true;
    }
    return false;
}

Token Parser::advance() {
    if (!isAtEnd()) pos++;
    return previous();
}

Token Parser::consume(TokenType type, const std::string& message, const std::string& value) {
    if (check(type, value)) return advance();
    throw std::runtime_error("Parser Error: " + message + " Found '" + peek().value + "' at line " + std::to_string(peek().line) + ", col " + std::to_string(peek().col));
}
