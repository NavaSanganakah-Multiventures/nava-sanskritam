const Lexer = require('./Lexer');
const Parser = require('./Parser');
const Interpreter = require('./Interpreter');

const code = `
वद(गुण("भानु"));
वद(वृद्धि("देव"));
`;

const lexer = new Lexer(code);
const parser = new Parser(lexer.tokenize());
const ast = parser.parse();
const interpreter = new Interpreter();
interpreter.interpret(ast);
