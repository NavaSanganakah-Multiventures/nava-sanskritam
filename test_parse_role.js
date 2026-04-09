const Lexer = require('./Lexer');
const Parser = require('./Parser');
const lexer = new Lexer("अङ्गम्(रामः);");
const parser = new Parser(lexer.tokenize());
const ast = parser.parse();
console.log(JSON.stringify(ast, null, 2));
