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

            if (char === '/' && this.input[this.pos + 1] === '/') {
                while (this.pos < this.input.length && this.currentChar() !== '\n') {
                    this.advance();
                }
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
                let samasaTokens = this.splitSamasa(id);
                
                for (let token of samasaTokens) {
                    token.line = this.line;
                    token.col = this.col;
                    this.tokens.push(token);
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
        return ['(', ')', '{', '}', ';', ',', '.' , ':'].includes(char);
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
    splitSamasa(id) {
        if (id.length === 0) return [];

        // 1. Normalization (Subanta / Suffix Analysis)
        const subantaSuffixes = [
            { s: 'आणाम्', v: 'SHASHTI', n: 3 }, { s: 'एभ्यः', v: 'CHATURTHI', n: 3 },
            { s: 'आभ्याम्', v: 'TRITIYA', n: 2 }, { s: 'एषु', v: 'SAPTAMI', n: 3 },
            { s: 'ईणाम्', v: 'SHASHTI', n: 3 }, { s: 'स्य', v: 'SHASHTI', n: 1 },
            { s: 'एण', v: 'TRITIYA', n: 1 }, { s: 'आय', v: 'CHATURTHI', n: 1 },
            { s: 'आत्', v: 'PANCHAMI', n: 1 }, { s: 'योः', v: 'SHASHTI', n: 2 },
            { s: 'आः', v: 'PRATHAMA', n: 3 }, { s: 'आन्', v: 'DWITIYA', n: 3 },
            { s: 'ऐः', v: 'TRITIYA', n: 3 }, { s: 'ए', v: 'SAPTAMI', n: 1 },
            { s: 'ः', v: 'PRATHAMA', n: 1 }, { s: 'म्', v: 'DWITIYA', n: 1 },
            { s: 'औ', v: 'PRATHAMA', n: 2 }
        ];

        let baseId = id;
        let vibhakti = 'UNKNOWN';
        let vachana = 1;

        for (const res of subantaSuffixes) {
            if (id.length > res.s.length && id.endsWith(res.s)) {
                baseId = id.slice(0, -res.s.length);
                vibhakti = res.v;
                vachana = res.n;
                break;
            }
        }

        if (this.keywords.has(baseId)) return [{ type: 'KEYWORD', value: baseId, vibhakti, vachana }];

        // 2. Greedy search for keywords within the compound
        for (let i = baseId.length - 1; i >= 2; i--) { // Min 2 chars for a keyword usually
            for (let start = 0; start <= baseId.length - i; start++) {
                let sub = baseId.substring(start, start + i);
                if (this.keywords.has(sub)) {
                    let left = baseId.substring(0, start);
                    let right = baseId.substring(start + i);
                    
                    let result = [];
                    if (left) result.push(...this.splitSamasa(left));
                    result.push({ type: 'KEYWORD', value: sub });
                    if (right) result.push(...this.splitSamasa(right));
                    return result;
                }
            }
        }

        // 3. Fallback: If no keywords found, it's a single identifier
        return [{ type: 'IDENTIFIER', value: baseId, vibhakti, vachana }];
    }
}

module.exports = Lexer;
