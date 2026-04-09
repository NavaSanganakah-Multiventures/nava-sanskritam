class Lexer {
    constructor(input) {
        this.input = input;
        this.pos = 0;
        this.line = 1;
        this.col = 1;
        this.tokens = [];

        this.keywords = new Set(['अस्ति', 'वद', 'यदि', 'तर्हि', 'चक्र', 'अन्यथा', 'फलम्', 'विधिः', 'नित्य']);
        this.devanagariDigits = {
            '०': '0', '१': '1', '२': '2', '३': '3', '४': '4',
            '५': '5', '६': '6', '७': '7', '८': '8', '९': '9'
        };
    }

    tokenize() {
        while (this.pos < this.input.length) {
            let char = this.currentChar();

            if (this.isWhitespace(char)) {
                this.advance();
                continue;
            }

            if (this.isPunctuation(char)) {
                this.tokens.push({ type: 'PUNCTUATION', value: char, line: this.line, col: this.col });
                this.advance();
                continue;
            }

            if (this.isOperatorChar(char)) {
                let op = this.readOperator();
                this.tokens.push({ type: 'OPERATOR', value: op, line: this.line, col: this.col });
                continue;
            }

            if (char === '"' || char === "'") {
                let str = this.readString(char);
                this.tokens.push({ type: 'STRING', value: str, line: this.line, col: this.col });
                continue;
            }

            if (this.isDigit(char) || this.isDevanagariDigit(char)) {
                let num = this.readNumber();
                this.tokens.push({ type: 'NUMBER', value: num, line: this.line, col: this.col });
                continue;
            }

            if (this.isIdentifierChar(char)) {
                let id = this.readIdentifier();
                if (this.keywords.has(id)) {
                    this.tokens.push({ type: 'KEYWORD', value: id, line: this.line, col: this.col });
                } else {
                    const keepSuffixes = new Set(['अङ्गम्', 'शीर्षकम्', 'अनुच्छेदः', 'सूची', 'बटनम्']);
                    if (!keepSuffixes.has(id)) {
                        if (id.length > 1 && id.endsWith('ः')) {
                            id = id.slice(0, -1);
                        } else if (id.length > 2 && id.endsWith('म्')) {
                            id = id.slice(0, -2);
                        } else if (id.length > 1 && id.endsWith('े')) {
                            id = id.slice(0, -1);
                        }
                    }
                    this.tokens.push({ type: 'IDENTIFIER', value: id, line: this.line, col: this.col });
                }
                continue;
            }

            throw new Error(`Lexer Error: Unexpected character '${char}' at line ${this.line}, col ${this.col}`);
        }

        this.tokens.push({ type: 'EOF', value: null, line: this.line, col: this.col });
        return this.tokens;
    }

    currentChar() {
        return this.input[this.pos];
    }

    advance() {
        if (this.currentChar() === '\n') {
            this.line++;
            this.col = 1;
        } else {
            this.col++;
        }
        this.pos++;
    }

    isWhitespace(char) {
        return /\s/.test(char);
    }

    isPunctuation(char) {
        return /[(){}\[\],;]/.test(char);
    }

    isOperatorChar(char) {
        return /[=+\-*/<>!]/.test(char);
    }

    readOperator() {
        let op = '';
        while (this.pos < this.input.length && this.isOperatorChar(this.currentChar())) {
            op += this.currentChar();
            this.advance();
        }
        return op;
    }

    readString(quoteChar) {
        let startLine = this.line;
        let startCol = this.col;
        this.advance(); // Skip opening quote
        let str = '';
        while (this.pos < this.input.length && this.currentChar() !== quoteChar) {
            str += this.currentChar();
            this.advance();
        }
        if (this.pos >= this.input.length) {
            throw new Error(`Lexer Error: Unterminated string starting at line ${startLine}, col ${startCol}`);
        }
        this.advance(); // Skip closing quote
        return str;
    }

    isDigit(char) {
        return /[0-9]/.test(char);
    }

    isDevanagariDigit(char) {
        return char in this.devanagariDigits;
    }

    readNumber() {
        let numStr = '';
        while (this.pos < this.input.length && (this.isDigit(this.currentChar()) || this.isDevanagariDigit(this.currentChar()))) {
            let char = this.currentChar();
            if (this.isDevanagariDigit(char)) {
                numStr += this.devanagariDigits[char];
            } else {
                numStr += char;
            }
            this.advance();
        }
        return parseFloat(numStr);
    }

    isIdentifierChar(char) {
        // Matches letters from any language, numbers, marks (like Devanagari matras/halants), or underscore
        return /[\p{L}\p{N}\p{M}_]/u.test(char) && !this.isPunctuation(char) && !this.isOperatorChar(char) && !this.isWhitespace(char);
    }

    readIdentifier() {
        let id = '';
        while (this.pos < this.input.length && this.isIdentifierChar(this.currentChar())) {
            id += this.currentChar();
            this.advance();
        }
        return id;
    }
}

module.exports = Lexer;
