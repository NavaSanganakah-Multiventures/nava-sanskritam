const Lexer = require('./Lexer.js');
const Parser = require('./Parser.js');
const lexer = new Lexer("अस्ति रामः = १०; वद(रामम्);");
const tokens = lexer.tokenize();
const parser = new Parser(tokens);
const ast = parser.parse();
console.log(JSON.stringify(ast, null, 2));
