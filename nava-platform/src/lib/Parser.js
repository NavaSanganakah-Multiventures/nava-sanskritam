class Parser {
    constructor(tokens) {
        this.tokens = tokens;
        this.pos = 0;
    }

    parse() {
        const program = { type: 'Program', body: [] };
        while (!this.isAtEnd()) {
            program.body.push(this.parseStatement());
        }
        return program;
    }

    stripVibhakti(id) {
        // Dummy implementation since actual is complex logic
        return id;
    }

    parseStatement() {
        if (this.match('KEYWORD', 'विधिः') ||
            this.match('KEYWORD', 'क्रिया') ||
            this.match('KEYWORD', 'सूत्रम्') ||
            this.match('KEYWORD', 'योगः')) return this.parseFunctionDeclaration();
        if (this.match('KEYWORD', 'फलम्')) return this.parseReturnStatement();
        if (this.match('KEYWORD', 'अस्ति')) return this.parseVariableDeclaration();
        if (this.match('KEYWORD', 'नित्य')) return this.parseConstantDeclaration();
        if (this.match('KEYWORD', 'दर्शनम्')) return this.parseDarshanamBlock();
        if (this.match('KEYWORD', 'मंजूषा')) return this.parseManjushaElement();
        if (this.match('KEYWORD', 'सूची')) return this.parseSuchiElement();
        if (this.match('KEYWORD', 'चित्त्रम्')) return this.parseChittramElement();
        if (this.match('KEYWORD', 'प्रविष्टिः')) return this.parsePrashtihElement();
        if (this.match('KEYWORD', 'वद')) return this.parsePrintStatement();
        if (this.match('KEYWORD', 'यदि')) return this.parseIfStatement();
        if (this.match('KEYWORD', 'चक्र')) return this.parseLoopStatement();
        return this.parseExpressionStatement();
    }

    parseFunctionDeclaration() {
        const idToken = this.consume('IDENTIFIER', "Expected function name.");
        this.consume('PUNCTUATION', "Expected '('", "(");
        const params = [];
        if (!this.check('PUNCTUATION', ")")) {
            do {
                if (params.length > 0) this.consume('PUNCTUATION', "Expected ','", ",");
                params.push(this.consume('IDENTIFIER', "Expected param.").value);
            } while (this.check('PUNCTUATION', ","));
        }
        this.consume('PUNCTUATION', "Expected ')'", ")");
        const body = this.parseBlock();
        return { type: 'FunctionDeclaration', id: idToken.value, params, body };
    }

    parseReturnStatement() {
        const argument = this.parseExpression();
        if (this.check('PUNCTUATION', ";")) this.advance();
        return { type: 'ReturnStatement', argument };
    }

    parseVariableDeclaration() {
        const idToken = this.consume('IDENTIFIER', "Expected var name.");
        this.consume('OPERATOR', "Expected '='", "=");
        const init = this.parseExpression();
        if (this.check('PUNCTUATION', ";")) this.advance();
        return { type: 'VariableDeclaration', id: idToken.value, init };
    }

    parseConstantDeclaration() {
        const idToken = this.consume('IDENTIFIER', "Expected var name.");
        this.consume('OPERATOR', "Expected '='", "=");
        const init = this.parseExpression();
        if (this.check('PUNCTUATION', ";")) this.advance();
        return { type: 'ConstantDeclaration', id: idToken.value, init };
    }

    parseDarshanamBlock() {
        const idToken = this.consume('IDENTIFIER', "Expected name.");
        this.consume('PUNCTUATION', "Expected '{'", "{");
        const elements = [];
        while (!this.check('PUNCTUATION', "}")) {
            if (this.match('KEYWORD', 'दृश्यम्')) elements.push(this.parseDrishyamElement());
            else if (this.match('KEYWORD', 'मंजूषा')) elements.push(this.parseManjushaElement());
            else if (this.match('KEYWORD', 'सूची')) elements.push(this.parseSuchiElement());
            else if (this.match('KEYWORD', 'चित्त्रम्')) elements.push(this.parseChittramElement());
            else if (this.match('KEYWORD', 'प्रविष्टिः')) elements.push(this.parsePrashtihElement());
            else if (this.check('IDENTIFIER')) { elements.push(this.parseDrishyamElement()); }
            else this.advance();
        }
        this.consume('PUNCTUATION', "Expected '}'", "}");
        return { type: 'DarshanamBlock', id: idToken.value, elements };
    }

    parseDrishyamElement() {
        const typeToken = this.advance();
        if (typeToken.type !== 'IDENTIFIER' && typeToken.type !== 'KEYWORD') {
            throw new Error(`Parser Error: Expected widget type. Found '${typeToken.value}'`);
        }
        const element = { type: typeToken.value, children: [] };

        if (this.match('PUNCTUATION', "(")) {
            if (!this.check('PUNCTUATION', ")")) {
                element.pos = [this.parseExpression()];
                if (this.match('PUNCTUATION', ",")) element.pos.push(this.parseExpression());
            }
            this.consume('PUNCTUATION', "Expected ')'", ")");
        }

        if (this.match('PUNCTUATION', "{")) {
            while (!this.check('PUNCTUATION', "}")) {
                if (this.match('KEYWORD', 'दृश्यम्') || this.match('KEYWORD', 'मंजूषा') || this.match('KEYWORD', 'सूची')) {
                    element.children.push(this.parseDrishyamElement());
                } else {
                    const key = this.consume('IDENTIFIER', "Expected attribute.").value;
                    this.consume('PUNCTUATION', "Expected ':'", ":");
                    if (key === 'नाम') {
                        if (this.check('STRING')) element.label = this.consume('STRING', "").value;
                        else element.label = this.consume('IDENTIFIER', "").value;
                    } else if (key === 'रङ्गः') element.color = this.consume('IDENTIFIER', "").value;
                    else if (key === 'स्रोतस') element.source = this.consume('STRING', "").value;
                    if (this.check('PUNCTUATION', ";")) this.advance();
                }
            }
            this.consume('PUNCTUATION', "Expected '}'", "}");
        }
        return element;
    }

    parseManjushaElement() {
        const element = { type: "Box", children: [] };
        if (this.check('IDENTIFIER')) element.label = this.advance().value;
        if (this.match('PUNCTUATION', "{")) {
            while (!this.check('PUNCTUATION', "}")) {
                if (this.match('KEYWORD', 'दृश्यम्')) element.children.push(this.parseDrishyamElement());
                else if (this.match('KEYWORD', 'मंजूषा')) element.children.push(this.parseManjushaElement());
                else if (this.match('KEYWORD', 'सूची')) element.children.push(this.parseSuchiElement());
                else if (this.match('KEYWORD', 'चित्त्रम्')) element.children.push(this.parseChittramElement());
                else if (this.match('KEYWORD', 'प्रविष्टिः')) element.children.push(this.parsePrashtihElement());
                else if (this.check('IDENTIFIER')) { this.pos--; element.children.push(this.parseDrishyamElement()); }
                else this.advance();
            }
            this.consume('PUNCTUATION', "Expected '}'", "}");
        }
        return element;
    }

    parseSuchiElement() {
        const element = this.parseManjushaElement();
        element.type = "List";
        return element;
    }

    parseChittramElement() {
        const element = this.parseDrishyamElement();
        element.type = "Image";
        return element;
    }

    parsePrashtihElement() {
        const element = this.parseDrishyamElement();
        element.type = "Input";
        return element;
    }

    parsePrintStatement() {
        this.consume('PUNCTUATION', "Expected '('", "(");
        const expr = this.parseExpression();
        this.consume('PUNCTUATION', "Expected ')'", ")");
        if (this.check('PUNCTUATION', ";")) this.advance();
        return { type: 'PrintStatement', expression: expr };
    }

    parseIfStatement() {
        this.consume('PUNCTUATION', "Expected '('", "(");
        const condition = this.parseExpression();
        this.consume('PUNCTUATION', "Expected ')'", ")");
        this.consume('KEYWORD', "Expected 'तर्हि'", "तर्हि");
        const consequence = this.check('PUNCTUATION', "{") ? this.parseBlock() : this.parseStatement();
        let alternate = null;
        if (this.match('KEYWORD', 'अन्यथा')) {
            alternate = this.check('PUNCTUATION', "{") ? this.parseBlock() : this.parseStatement();
        }
        return { type: 'IfStatement', condition, consequence, alternate };
    }

    parseLoopStatement() {
        this.consume('PUNCTUATION', "Expected '('", "(");
        let init = null;
        if (!this.check('PUNCTUATION', ";")) {
            const idToken = this.consume('IDENTIFIER', "");
            this.consume('OPERATOR', "Expected '='", "=");
            const value = this.parseExpression();
            init = { type: 'Assignment', left: idToken.value, right: value };
        }
        this.consume('PUNCTUATION', "Expected ';'", ";");
        let test = null;
        if (!this.check('PUNCTUATION', ";")) test = this.parseExpression();
        this.consume('PUNCTUATION', "Expected ';'", ";");
        let update = null;
        if (!this.check('PUNCTUATION', ")")) {
            const idToken = this.consume('IDENTIFIER', "");
            this.consume('OPERATOR', "Expected '='", "=");
            const value = this.parseExpression();
            update = { type: 'Assignment', left: idToken.value, right: value };
        }
        this.consume('PUNCTUATION', "Expected ')'", ")");
        const body = this.check('PUNCTUATION', "{") ? this.parseBlock() : { type: 'BlockStatement', body: [this.parseStatement()] };
        return { type: 'LoopStatement', init, test, update, body };
    }

    parseExpressionStatement() {
        if (this.check('IDENTIFIER') && this.pos + 1 < this.tokens.length && this.tokens[this.pos + 1].type === 'OPERATOR' && this.tokens[this.pos + 1].value === '=') {
            const idToken = this.consume('IDENTIFIER', "");
            this.consume('OPERATOR', "Expected '='", "=");
            const right = this.parseExpression();
            if (this.check('PUNCTUATION', ";")) this.advance();
            return { type: 'Assignment', left: idToken.value, right };
        }
        const expr = this.parseExpression();
        if (this.check('PUNCTUATION', ";")) this.advance();
        return { type: 'ExpressionStatement', expression: expr };
    }

    parseBlock() {
        this.consume('PUNCTUATION', "Expected '{'", "{");
        const body = [];
        while (!this.check('PUNCTUATION', "}")) {
            body.push(this.parseStatement());
        }
        this.consume('PUNCTUATION', "Expected '}'", "}");
        return { type: 'BlockStatement', body };
    }

    parseExpression() {
        return this.parseEquality();
    }

    parseEquality() {
        let expr = this.parseRelational();
        while (this.match('OPERATOR', "==") || this.match('OPERATOR', "!=")) {
            const op = this.previous().value;
            const right = this.parseRelational();
            expr = { type: 'BinaryExpression', op, left: expr, right };
        }
        return expr;
    }

    parseRelational() {
        let expr = this.parseAdditive();
        while (this.match('OPERATOR', "<") || this.match('OPERATOR', "<=") || this.match('OPERATOR', ">") || this.match('OPERATOR', ">=")) {
            const op = this.previous().value;
            const right = this.parseAdditive();
            expr = { type: 'BinaryExpression', op, left: expr, right };
        }
        return expr;
    }

    parseAdditive() {
        let expr = this.parseMultiplicative();
        while (this.match('OPERATOR', "+") || this.match('OPERATOR', "-")) {
            const op = this.previous().value;
            const right = this.parseMultiplicative();
            expr = { type: 'BinaryExpression', op, left: expr, right };
        }
        return expr;
    }

    parseMultiplicative() {
        let expr = this.parsePrimary();
        while (this.match('OPERATOR', "*") || this.match('OPERATOR', "/")) {
            const op = this.previous().value;
            const right = this.parsePrimary();
            expr = { type: 'BinaryExpression', op, left: expr, right };
        }
        return expr;
    }

    parsePrimary() {
        if (this.match('PUNCTUATION', "{")) {
            const obj = { type: 'ObjectLiteral', properties: [] };
            while (!this.check('PUNCTUATION', "}")) {
                const keyToken = this.consume('IDENTIFIER', "Expected property key.");
                this.consume('PUNCTUATION', "Expected ':'", ":");
                const value = this.parseExpression();
                obj.properties.push({ key: keyToken.value, value });
                if (this.check('PUNCTUATION', ",")) this.advance();
                else break;
            }
            this.consume('PUNCTUATION', "Expected '}'", "}");
            return obj;
        }

        if (this.match('NUMBER')) return { type: 'Literal', value: this.previous().value, isString: false };
        if (this.match('STRING')) return { type: 'Literal', value: this.previous().value, isString: true };

        if (this.match('IDENTIFIER')) {
            const idToken = this.previous();
            const expr = { type: 'Identifier', name: idToken.value };

            if (this.match('PUNCTUATION', "(")) {
                const call = { type: 'CallExpression', callee: expr, arguments: [] };
                if (!this.check('PUNCTUATION', ")")) {
                    do {
                        if (call.arguments.length > 0) this.consume('PUNCTUATION', "Expected ','", ",");
                        call.arguments.push(this.parseExpression());
                    } while (this.check('PUNCTUATION', ","));
                }
                this.consume('PUNCTUATION', "Expected ')'", ")");
                return call;
            }
            return expr;
        }
        if (this.match('PUNCTUATION', "(")) {
            const expr = this.parseExpression();
            this.consume('PUNCTUATION', "Expected ')'", ")");
            return expr;
        }
        throw new Error(`Parser Error: Unexpected token '${this.peek().value}' at line ${this.peek().line}, col ${this.peek().col}`);
    }

    isAtEnd() { return this.pos >= this.tokens.length || this.tokens[this.pos].type === 'EOF'; }
    peek() { return this.pos >= this.tokens.length ? this.tokens[this.tokens.length - 1] : this.tokens[this.pos]; }
    previous() { return this.tokens[this.pos - 1]; }

    check(type, value) {
        if (this.isAtEnd()) return false;
        const token = this.peek();
        if (token.type !== type) return false;
        if (value !== undefined && value !== "" && token.value !== value) return false;
        return true;
    }

    match(type, value) {
        if (this.check(type, value)) {
            this.advance();
            return true;
        }
        return false;
    }

    advance() {
        if (!this.isAtEnd()) this.pos++;
        return this.previous();
    }

    consume(type, message, value) {
        if (this.check(type, value)) return this.advance();
        throw new Error(`Parser Error: ${message} Found '${this.peek().value}' at line ${this.peek().line}, col ${this.peek().col}`);
    }
}

export default Parser;
