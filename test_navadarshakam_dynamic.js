const Lexer = require('./Lexer');
const Parser = require('./Parser');
const Interpreter = require('./Interpreter');

const code = `
अस्ति रामः = "रामः (कर्ता)";
अस्ति रावणम् = "रावणम् (कर्म)";
अङ्गम्(राम);
अङ्गम्(रावण);
`;

const lexer = new Lexer(code);
const parser = new Parser(lexer.tokenize());
const ast = parser.parse();
const interpreter = new Interpreter();
interpreter.interpret(ast);
interpreter.darshakam.generate();
