class Parser {
    constructor(tokens) {
        this.tokens = tokens;
        this.pos = 0;
    }

    parse() {
        let ast = { type: 'Program', body: [] };
        while (!this.isAtEnd()) {
            ast.body.push(this.parseStatement());
        }
        return ast;
    }

    parseStatement() {
        if (this.match('KEYWORD', 'अस्ति')) return this.parseVariableDeclaration();
        if (this.match('KEYWORD', 'वद')) return this.parsePrintStatement();
        if (this.match('KEYWORD', 'यदि')) return this.parseIfStatement();
        if (this.match('KEYWORD', 'चक्र')) return this.parseLoopStatement();

        return this.parseExpressionStatement();
    }

    parseVariableDeclaration() {
        // अस्ति नाम = "आचार्य";
        let idToken = this.consume('IDENTIFIER', "Expected variable name after 'अस्ति'.");
        this.consume('OPERATOR', "Expected '=' after variable name.", '=');
        let init = this.parseExpression();
        this.consume('PUNCTUATION', "Expected ';' after variable declaration.", ';');
        return {
            type: 'VariableDeclaration',
            id: idToken.value,
            init: init
        };
    }

    parsePrintStatement() {
        // वद("सत्यम्");
        this.consume('PUNCTUATION', "Expected '(' after 'वद'.", '(');
        let expression = this.parseExpression();
        this.consume('PUNCTUATION', "Expected ')' after print expression.", ')');
        this.consume('PUNCTUATION', "Expected ';' after print statement.", ';');
        return {
            type: 'PrintStatement',
            expression: expression
        };
    }

    parseIfStatement() {
        // यदि (क > १०) तर्हि वद("सत्यम्");
        this.consume('PUNCTUATION', "Expected '(' after 'यदि'.", '(');
        let condition = this.parseExpression();
        this.consume('PUNCTUATION', "Expected ')' after condition.", ')');

        this.consume('KEYWORD', "Expected 'तर्हि' after condition in if statement.", 'तर्हि');

        // Single statement or block
        let consequence;
        if (this.check('PUNCTUATION', '{')) {
            consequence = this.parseBlock();
        } else {
            consequence = this.parseStatement();
        }

        return {
            type: 'IfStatement',
            condition: condition,
            consequence: consequence
        };
    }

    parseLoopStatement() {
        // चक्र (i = १; i < ५) { ... }
        this.consume('PUNCTUATION', "Expected '(' after 'चक्र'.", '(');

        let init = null;
        if (!this.check('PUNCTUATION', ';')) {
            if (this.check('IDENTIFIER')) {
                let idToken = this.consume('IDENTIFIER');
                this.consume('OPERATOR', "Expected '=' in loop initialization.", '=');
                let value = this.parseExpression();
                init = { type: 'Assignment', left: idToken.value, right: value };
            } else {
                throw new Error("Invalid initialization in loop.");
            }
        }
        this.consume('PUNCTUATION', "Expected ';' after loop initialization.", ';');

        let test = null;
        if (!this.check('PUNCTUATION', ';')) {
            test = this.parseExpression();
        }
        this.consume('PUNCTUATION', "Expected ';' after loop condition.", ';');

        // Handling post-increment (e.g. i = i + 1 as currently parsing assignment might need special parsing)
        // For simplicity based on specs, let's say the update might be missing or parsed as assignment
        let update = null;
        if (!this.check('PUNCTUATION', ')')) {
             if (this.check('IDENTIFIER')) {
                let idToken = this.consume('IDENTIFIER');
                this.consume('OPERATOR', "Expected '=' in loop update.", '=');
                let value = this.parseExpression();
                update = { type: 'Assignment', left: idToken.value, right: value };
            }
        }
        this.consume('PUNCTUATION', "Expected ')' after loop update.", ')');

        let body;
        if (this.check('PUNCTUATION', '{')) {
            body = this.parseBlock();
        } else {
            body = { type: 'BlockStatement', body: [this.parseStatement()] };
        }

        return {
            type: 'LoopStatement',
            init: init,
            test: test,
            update: update,
            body: body
        };
    }

    parseExpressionStatement() {
        // e.g. a = 5;
        if (this.check('IDENTIFIER')) {
            let nextToken = this.tokens[this.pos + 1];
            if (nextToken && nextToken.type === 'OPERATOR' && nextToken.value === '=') {
                let idToken = this.consume('IDENTIFIER');
                this.consume('OPERATOR', "Expected '='", '=');
                let right = this.parseExpression();
                this.consume('PUNCTUATION', "Expected ';'", ';');
                return {
                    type: 'ExpressionStatement',
                    expression: {
                        type: 'Assignment',
                        left: idToken.value,
                        right: right
                    }
                };
            }
        }

        let expr = this.parseExpression();
        this.consume('PUNCTUATION', "Expected ';' after expression.", ';');
        return {
            type: 'ExpressionStatement',
            expression: expr
        };
    }

    parseBlock() {
        this.consume('PUNCTUATION', "Expected '{'.", '{');
        let statements = [];
        while (!this.check('PUNCTUATION', '}') && !this.isAtEnd()) {
            statements.push(this.parseStatement());
        }
        this.consume('PUNCTUATION', "Expected '}'.", '}');
        return {
            type: 'BlockStatement',
            body: statements
        };
    }

    parseExpression() {
        return this.parseEquality();
    }

    parseEquality() {
        let expr = this.parseRelational();
        while (this.match('OPERATOR', '==') || this.match('OPERATOR', '!=')) {
            let operator = this.previous().value;
            let right = this.parseRelational();
            expr = { type: 'BinaryExpression', operator, left: expr, right };
        }
        return expr;
    }

    parseRelational() {
        let expr = this.parseAdditive();
        while (this.match('OPERATOR', '<') || this.match('OPERATOR', '<=') ||
               this.match('OPERATOR', '>') || this.match('OPERATOR', '>=')) {
            let operator = this.previous().value;
            let right = this.parseAdditive();
            expr = { type: 'BinaryExpression', operator, left: expr, right };
        }
        return expr;
    }

    parseAdditive() {
        let expr = this.parseMultiplicative();
        while (this.match('OPERATOR', '+') || this.match('OPERATOR', '-')) {
            let operator = this.previous().value;
            let right = this.parseMultiplicative();
            expr = { type: 'BinaryExpression', operator, left: expr, right };
        }
        return expr;
    }

    parseMultiplicative() {
        let expr = this.parsePrimary();
        while (this.match('OPERATOR', '*') || this.match('OPERATOR', '/')) {
            let operator = this.previous().value;
            let right = this.parsePrimary();
            expr = { type: 'BinaryExpression', operator, left: expr, right };
        }
        return expr;
    }

    parsePrimary() {
        if (this.match('NUMBER')) {
            return { type: 'Literal', value: this.previous().value };
        }
        if (this.match('STRING')) {
            return { type: 'Literal', value: this.previous().value };
        }
        if (this.match('IDENTIFIER')) {
            return { type: 'Identifier', name: this.previous().value };
        }
        if (this.match('PUNCTUATION', '(')) {
            let expr = this.parseExpression();
            this.consume('PUNCTUATION', "Expected ')' after expression.", ')');
            return expr;
        }

        throw new Error(`Parser Error: Unexpected token '${this.peek().value}' at line ${this.peek().line}, col ${this.peek().col}`);
    }

    match(type, value = null) {
        if (this.check(type, value)) {
            this.advance();
            return true;
        }
        return false;
    }

    check(type, value = null) {
        if (this.isAtEnd()) return false;
        let token = this.peek();
        if (token.type !== type) return false;
        if (value !== null && token.value !== value) return false;
        return true;
    }

    advance() {
        if (!this.isAtEnd()) this.pos++;
        return this.previous();
    }

    isAtEnd() {
        return this.peek().type === 'EOF';
    }

    peek() {
        return this.tokens[this.pos];
    }

    previous() {
        return this.tokens[this.pos - 1];
    }

    consume(type, message, value = null) {
        if (this.check(type, value)) return this.advance();
        throw new Error(`Parser Error: ${message} Found '${this.peek().value}' at line ${this.peek().line}, col ${this.peek().col}`);
    }
}

module.exports = Parser;
