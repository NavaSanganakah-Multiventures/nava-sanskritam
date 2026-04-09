const fs = require('fs');
const Lexer = require('./Lexer');
const Parser = require('./Parser');
const Interpreter = require('./Interpreter');

const file = process.argv[2];
const code = fs.readFileSync(file, 'utf8');

const lexer = new Lexer(code);
const parser = new Parser(lexer.tokenize());
const ast = parser.parse();
const interpreter = new Interpreter();
interpreter.interpret(ast);
