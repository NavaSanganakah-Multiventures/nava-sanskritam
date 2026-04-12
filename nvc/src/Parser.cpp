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

std::string Parser::stripVibhakti(std::string id, std::string* role, int* vachana, std::string* ling) {
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
    
    if (vachana) *vachana = meta.vachana;
    if (ling) {
        switch (meta.ling) {
            case Grammar::Ling::PULLINGA: *ling = "Pullinga"; break;
            case Grammar::Ling::STRILINGA: *ling = "Strilinga"; break;
            case Grammar::Ling::NAPUNSAKA: *ling = "Napunsaka"; break;
            default: *ling = "Unknown"; break;
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
    Token idToken = consume(TokenType::IDENTIFIER, "व्याकरण-त्रुटिः: क्रिया-नाम अपेक्षितम् ");
    consume(TokenType::PUNCTUATION, "व्याकरण-त्रुटिः: '(' अपेक्षितम् ", "(");
    std::vector<std::string> params;
    if (!check(TokenType::PUNCTUATION, ")")) {
        do {
            if (!params.empty()) {
                consume(TokenType::PUNCTUATION, "व्याकरण-त्रुटिः: मापदण्डानां मध्ये ',' अपेक्षितम् ", ",");
            }
            params.push_back(grammar.toUtf8(grammar.analyzeSubanta(grammar.toUtf32(consume(TokenType::IDENTIFIER, "व्याकरण-त्रुटिः: मापदण्ड-नाम अपेक्षितम् ").value)).root));
        } while (check(TokenType::PUNCTUATION, ","));
    }
    consume(TokenType::PUNCTUATION, "व्याकरण-त्रुटिः: मापदण्डानन्तरं ')' अपेक्षितम् ", ")");

    auto meta = grammar.analyzeTinanta(grammar.toUtf32(idToken.value));
    auto decl = std::make_unique<FunctionDeclaration>();
    decl->id = grammar.toUtf8(meta.root);
    decl->params = std::move(params);
    
    // SUL v13.0: Tag Lakara/Pada/Gana
    if (meta.lakara == Grammar::Lakara::LAT) decl->lakara = "Lat";
    else if (meta.lakara == Grammar::Lakara::LRT) decl->lakara = "Lrt";
    else if (meta.lakara == Grammar::Lakara::VIDHI_LIN) decl->lakara = "Vidhi-Lin";
    else decl->lakara = "Unknown";
    
    decl->pada = (meta.pada == Grammar::Pada::PARASMAIPADA) ? "Parasmaipada" : "Atmanepada";
    decl->body = parseBlock();
    return decl;
}

std::unique_ptr<Statement> Parser::parseReturnStatement() {
    auto value = parseExpression();
    consume(TokenType::PUNCTUATION, "व्याकरण-त्रुटिः: फलात् परं ';' अपेक्षितम् ", ";");
    auto stmt = std::make_unique<ReturnStatement>();
    stmt->argument = std::move(value);
    return stmt;
}

std::unique_ptr<VariableDeclaration> Parser::parseVariableDeclaration() {
    Token idToken;
    if (check(TokenType::IDENTIFIER) || check(TokenType::KEYWORD)) {
        idToken = advance();
    } else {
        throw std::runtime_error("व्याकरण-त्रुटिः: 'अस्ति' पदानन्तरं चर-नाम अपेक्षितम् ");
    }
    consume(TokenType::OPERATOR, "व्याकरण-त्रुटिः: '=' अपेक्षितम् ", "=");
    auto init = parseExpression();
    consume(TokenType::PUNCTUATION, "व्याकरण-त्रुटिः: ';' अपेक्षितम् ", ";");

    auto decl = std::make_unique<VariableDeclaration>();
    decl->id = stripVibhakti(idToken.value, nullptr, &decl->vachana, &decl->ling);
    decl->init = std::move(init);
    return decl;
}

std::unique_ptr<ConstantDeclaration> Parser::parseConstantDeclaration() {
    Token idToken = consume(TokenType::IDENTIFIER, "व्याकरण-त्रुटिः: 'नित्य' पदानन्तरं स्थिर-नाम अपेक्षितम् ");
    consume(TokenType::OPERATOR, "व्याकरण-त्रुटिः: '=' अपेक्षितम् ", "=");
    auto init = parseExpression();
    consume(TokenType::PUNCTUATION, "व्याकरण-त्रुटिः: ';' अपेक्षितम् ", ";");

    auto decl = std::make_unique<ConstantDeclaration>();
    decl->id = stripVibhakti(idToken.value, nullptr, &decl->vachana, &decl->ling);
    decl->init = std::move(init);
    return decl;
}

std::unique_ptr<PrintStatement> Parser::parsePrintStatement() {
    consume(TokenType::PUNCTUATION, "व्याकरण-त्रुटिः: '(' अपेक्षितम् ", "(");
    auto stmt = std::make_unique<PrintStatement>();
    
    if (!check(TokenType::PUNCTUATION, ")")) {
        do {
            if (!stmt->expressions.empty()) {
                consume(TokenType::PUNCTUATION, "व्याकरण-त्रुटिः: ',' अपेक्षितम् ", ",");
            }
            stmt->expressions.push_back(parseExpression());
        } while (check(TokenType::PUNCTUATION, ","));
    }
    
    consume(TokenType::PUNCTUATION, "व्याकरण-त्रुटिः: ')' अपेक्षितम् ", ")");
    consume(TokenType::PUNCTUATION, "व्याकरण-त्रुटिः: ';' अपेक्षितम् ", ";");

    return stmt;
}

std::unique_ptr<IfStatement> Parser::parseIfStatement() {
    consume(TokenType::PUNCTUATION, "व्याकरण-त्रुटिः: '(' अपेक्षितम् ", "(");
    auto cond = parseExpression();
    consume(TokenType::PUNCTUATION, "व्याकरण-त्रुटिः: ')' अपेक्षितम् ", ")");

    consume(TokenType::KEYWORD, "व्याकरण-त्रुटिः: 'तर्हि' अपेक्षितम् ", "तर्हि");

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
    consume(TokenType::PUNCTUATION, "व्याकरण-त्रुटिः: '(' अपेक्षितम् ", "(");

    std::unique_ptr<ASTNode> init = nullptr;
    if (!check(TokenType::PUNCTUATION, ";")) {
        if (check(TokenType::IDENTIFIER)) {
            Token idToken = consume(TokenType::IDENTIFIER, "");
            consume(TokenType::OPERATOR, "व्याकरण-त्रुटिः: '=' अपेक्षितम् ", "=");
            auto value = parseExpression();
            auto assign = std::make_unique<Assignment>();
            assign->left = stripVibhakti(idToken.value, nullptr);
            assign->right = std::move(value);
            init = std::move(assign);
        } else {
            throw std::runtime_error("व्याकरण-त्रुटिः: अमान्यः आरम्भः ");
        }
    }
    consume(TokenType::PUNCTUATION, "व्याकरण-त्रुटिः: ';' अपेक्षितम् ", ";");

    std::unique_ptr<Expression> test = nullptr;
    if (!check(TokenType::PUNCTUATION, ";")) {
        test = parseExpression();
    }
    consume(TokenType::PUNCTUATION, "व्याकरण-त्रुटिः: ';' अपेक्षितम् ", ";");

    std::unique_ptr<ASTNode> update = nullptr;
    if (!check(TokenType::PUNCTUATION, ")")) {
        if (check(TokenType::IDENTIFIER)) {
            Token idToken = consume(TokenType::IDENTIFIER, "");
            consume(TokenType::OPERATOR, "व्याकरण-त्रुटिः: '=' अपेक्षितम् ", "=");
            auto value = parseExpression();
            auto assign = std::make_unique<Assignment>();
            assign->left = stripVibhakti(idToken.value, nullptr);
            assign->right = std::move(value);
            update = std::move(assign);
        }
    }
    consume(TokenType::PUNCTUATION, "व्याकरण-त्रुटिः: ')' अपेक्षितम् ", ")");

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
    auto expr = parseExpression();
    
    if (match(TokenType::OPERATOR, "=")) {
        auto right = parseExpression();
        consume(TokenType::PUNCTUATION, "व्याकरण-त्रुटिः: ';' अपेक्षितम् ", ";");

        auto assign = std::make_unique<Assignment>();
        
        // Handle Identifier or MemberAccess on the left
        if (expr->getType() == ASTNodeType::Identifier) {
            assign->left = static_cast<Identifier*>(expr.get())->name;
        } else if (expr->getType() == ASTNodeType::MemberAccess) {
            auto member = static_cast<MemberAccess*>(expr.get());
            assign->left = static_cast<Identifier*>(member->object.get())->name + "." + member->property;
        } else {
            throw std::runtime_error("व्याकरण-त्रुटिः: अमान्यः निर्देशः ");
        }
        
        assign->right = std::move(right);
        auto stmt = std::make_unique<ExpressionStatement>();
        stmt->expression = std::move(assign);
        return stmt;
    }

    consume(TokenType::PUNCTUATION, "व्याकरण-त्रुटिः: ';' अपेक्षितम् ", ";");
    auto stmt = std::make_unique<ExpressionStatement>();
    stmt->expression = std::move(expr);
    return stmt;
}

std::unique_ptr<BlockStatement> Parser::parseBlock() {
    consume(TokenType::PUNCTUATION, "व्याकरण-त्रुटिः: '{' अपेक्षितम् ", "{");
    auto block = std::make_unique<BlockStatement>();
    while (!check(TokenType::PUNCTUATION, "}") && !isAtEnd()) {
        block->body.push_back(parseStatement());
    }
    consume(TokenType::PUNCTUATION, "व्याकरण-त्रुटिः: '}' अपेक्षितम् ", "}");
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
    if (match(TokenType::IDENTIFIER) || match(TokenType::KEYWORD, "नियमः") || match(TokenType::KEYWORD, "फलम्")) {
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
        
        switch (meta.ling) {
            case Grammar::Ling::PULLINGA: expr->ling = "Pullinga"; break;
            case Grammar::Ling::STRILINGA: expr->ling = "Strilinga"; break;
            case Grammar::Ling::NAPUNSAKA: expr->ling = "Napunsaka"; break;
            default: expr->ling = "Unknown"; break;
        }

        // SUL v12.0: Native Shashti (Genitive) Property Access
        if (expr->role == "Shashti") {
            if (check(TokenType::IDENTIFIER)) {
                auto propToken = consume(TokenType::IDENTIFIER, "व्याकरण-त्रुटिः: षष्ठीविभक्तेः पश्चात् गुण-नाम अपेक्षितम् ");
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
                        consume(TokenType::PUNCTUATION, "व्याकरण-त्रुटिः: ',' अपेक्षितम् ", ",");
                    }
                    call->arguments.push_back(parseExpression());
                } while (check(TokenType::PUNCTUATION, ","));
            }
            consume(TokenType::PUNCTUATION, "व्याकरण-त्रुटिः: ')' अपेक्षितम् ", ")");
            return call;
        }
        return expr;
    }
    if (match(TokenType::PUNCTUATION, "(")) {
        auto expr = parseExpression();
        consume(TokenType::PUNCTUATION, "व्याकरण-त्रुटिः: ')' अपेक्षितम् ", ")");
        return expr;
    }
    throw std::runtime_error("व्याकरण-त्रुटिः: अनुचितः शब्दः '" + peek().value + "'  - पङ्क्तिः " + std::to_string(peek().line) + ", स्तम्भः " + std::to_string(peek().col));
}

std::unique_ptr<DarshanamBlock> Parser::parseDarshanamBlock() {
    Token idToken = consume(TokenType::IDENTIFIER, "व्याकरण-त्रुटिः: दर्शन-नाम अपेक्षितम् ");
    consume(TokenType::PUNCTUATION, "व्याकरण-त्रुटिः: दर्शन-नामनः पश्चात् '{' अपेक्षितम् ", "{");
    
    auto block = std::make_unique<DarshanamBlock>();
    block->id = idToken.value;

    while (!check(TokenType::PUNCTUATION, "}")) {
        if (match(TokenType::KEYWORD, "दृश्यम्")) {
            block->elements.push_back(parseDrishyamElement());
        } else if (match(TokenType::KEYWORD, "मंजूषा")) {
            block->elements.push_back(parseManjushaElement());
        } else if (match(TokenType::KEYWORD, "सूची")) {
            block->elements.push_back(parseSuchiElement());
        } else if (match(TokenType::KEYWORD, "चित्त्रम्")) {
            block->elements.push_back(parseChittramElement());
        } else if (match(TokenType::KEYWORD, "प्रविष्टिः")) {
            block->elements.push_back(parsePrashtihElement());
        } else if (match(TokenType::IDENTIFIER, "पाठः")) {
            block->elements.push_back(parseDrishyamElement("Text"));
        } else if (match(TokenType::IDENTIFIER, "बटनम्")) {
            block->elements.push_back(parseDrishyamElement("Button"));
        } else if (match(TokenType::PUNCTUATION, ";")) {
            continue; // Skip extra semicolons
        } else {
            advance(); // Skip unknown
        }
    }
    consume(TokenType::PUNCTUATION, "व्याकरण-त्रुटिः: '}' अपेक्षितम् ", "}");
    return block;
}

std::unique_ptr<DrishyamElement> Parser::parseDrishyamElement(std::string enforcedType) {
    auto element = std::make_unique<DrishyamElement>();
    
    if (enforcedType.empty()) {
        Token typeToken = consume(TokenType::IDENTIFIER, "व्याकरण-त्रुटिः: वस्तु-प्रकारः अपेक्षितम् ");
        element->type = typeToken.value;
    } else {
        element->type = enforcedType;
    }

    if (match(TokenType::PUNCTUATION, "(")) {
        while (!check(TokenType::PUNCTUATION, ")")) {
            Token key = consume(TokenType::IDENTIFIER, "व्याकरण-त्रुटिः: गुण-नाम अपेक्षितम् ");
            if (match(TokenType::PUNCTUATION, ":")) {
                if (key.value == "नाम" || key.value == "label") {
                    if (check(TokenType::STRING)) {
                        auto lit = std::make_unique<Literal>();
                        lit->value = consume(TokenType::STRING, "").value;
                        lit->isString = true;
                        element->label = std::move(lit);
                    } else {
                        element->label = parseExpression();
                    }
                } else if (key.value == "रङ्गः" || key.value == "रंग" || key.value == "color") {
                    if (check(TokenType::STRING)) {
                         element->color = consume(TokenType::STRING, "").value;
                    } else {
                         element->color = consume(TokenType::IDENTIFIER, "व्याकरण-त्रुटिः: रङ्ग-नाम अपेक्षितम् ").value;
                    }
                } else if (key.value == "स्रोतस" || key.value == "source") {
                    element->source = consume(TokenType::STRING, "व्याकरण-त्रुटिः: स्रोतस-लिङ्क अपेक्षितम् ").value;
                } else {
                    parseExpression(); // Handle unknown attributes generically
                }
            } else {
                // If it isn't a key:value pair, fallback for coordinates
            }
            if (match(TokenType::PUNCTUATION, ",")) continue;
        }
        consume(TokenType::PUNCTUATION, "व्याकरण-त्रुटिः: गुणानां पश्चात् ')' अपेक्षितम् ", ")");
    }

    if (match(TokenType::PUNCTUATION, "{")) {
        while (!check(TokenType::PUNCTUATION, "}")) {
            if (match(TokenType::KEYWORD, "दृश्यम्")) {
                element->children.push_back(parseDrishyamElement());
            } else if (match(TokenType::KEYWORD, "मंजूषा")) {
                element->children.push_back(parseManjushaElement());
            } else if (match(TokenType::KEYWORD, "सूची")) {
                element->children.push_back(parseSuchiElement());
            } else if (match(TokenType::KEYWORD, "चित्त्रम्")) {
                element->children.push_back(parseChittramElement());
            } else if (match(TokenType::KEYWORD, "प्रविष्टिः")) {
                element->children.push_back(parsePrashtihElement());
            } else if (match(TokenType::IDENTIFIER, "पाठः")) {
                element->children.push_back(parseDrishyamElement("Text"));
            } else if (match(TokenType::IDENTIFIER, "बटनम्")) {
                element->children.push_back(parseDrishyamElement("Button"));
            } else if (match(TokenType::PUNCTUATION, ";")) {
                continue; // Skip extra semicolons
            } else {
                advance(); // Skip unknown
            }
        }
        consume(TokenType::PUNCTUATION, "व्याकरण-त्रुटिः: बाल-वस्तूनां पश्चात् '}' अपेक्षितम् ", "}");
    } else {
        match(TokenType::PUNCTUATION, ";"); // End basic elements properly
    }

    return element;
}

std::unique_ptr<DrishyamElement> Parser::parseManjushaElement() {
    return parseDrishyamElement("Box"); 
}

std::unique_ptr<DrishyamElement> Parser::parseSuchiElement() {
    return parseDrishyamElement("List");
}

std::unique_ptr<DrishyamElement> Parser::parseChittramElement() {
    return parseDrishyamElement("Image");
}

std::unique_ptr<DrishyamElement> Parser::parsePrashtihElement() {
    return parseDrishyamElement("Input");
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
    throw std::runtime_error(message + " - प्राप्तम् '" + peek().value + "' - पङ्क्तिः " + std::to_string(peek().line) + ", स्तम्भः " + std::to_string(peek().col));
}
