const Lexer = require('./Lexer');
const Parser = require('./Parser');
const Interpreter = require('./Interpreter');

const code = `
अस्ति रामः = "रामः";
अङ्गम्(रामः);
`;

const lexer = new Lexer(code);
const parser = new Parser(lexer.tokenize());
const ast = parser.parse();
console.log(JSON.stringify(ast, null, 2));

const interpreter = new Interpreter();
interpreter.interpret(ast);
console.log(interpreter.darshakam.htmlElements);
