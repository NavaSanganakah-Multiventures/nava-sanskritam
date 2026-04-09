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

        // Shiva Sutrani for Pratyahara definitions
        this.shivaSutras = [
            { letters: ['अ', 'इ', 'उ'], it: 'ण्' },
            { letters: ['ऋ', 'ऌ'], it: 'क्' },
            { letters: ['ए', 'ओ'], it: 'ङ्' },
            { letters: ['ऐ', 'औ'], it: 'च्' },
            { letters: ['ह', 'य', 'व', 'र'], it: 'ट्' },
            { letters: ['ल'], it: 'ण्' },
            { letters: ['ञ', 'म', 'ङ', 'ण', 'न'], it: 'म्' },
            { letters: ['झ', 'भ'], it: 'ञ्' },
            { letters: ['घ', 'ढ', 'ध'], it: 'ष्' },
            { letters: ['ज', 'ब', 'ग', 'ड', 'द'], it: 'श्' },
            { letters: ['ख', 'फ', 'छ', 'ठ', 'थ', 'च', 'ट', 'त'], it: 'व्' },
            { letters: ['क', 'प'], it: 'य्' },
            { letters: ['श', 'ष', 'स'], it: 'र्' },
            { letters: ['ह'], it: 'ल्' }
        ];
    }

    /**
     * Look up a Pratyahara (e.g., 'अक्') and return its array of letters.
     */
    getPratyahara(name) {
        if (!name || name.length < 2) return [];
        let startChar = name.charAt(0);
        let itChar = name.slice(1);

        let result = [];
        let collecting = false;

        for (let sutra of this.shivaSutras) {
            for (let letter of sutra.letters) {
                if (letter === startChar) {
                    collecting = true;
                }
                if (collecting) {
                    result.push(letter);
                }
            }
            if (collecting && sutra.it === itChar) {
                return result;
            }
        }
        return [];
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
                    // Implicit Vibhakti suffix stripping is removed per Adhyaya 1 integration.
                    // The raw variable name with its suffix is sent to the Parser.
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
