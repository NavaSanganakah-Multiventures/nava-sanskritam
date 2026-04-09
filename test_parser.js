const Lexer = require('./Lexer');
const Parser = require('./Parser');

const code = `
नित्य गुरुः = "आचार्य";
विधिः योग(क, ख) {
    यदि (क > ख) तर्हि फलम् क;
    अन्यथा फलम् ख;
}
वद(योग(१०, ५));
`;

const lexer = new Lexer(code);
const tokens = lexer.tokenize();
const parser = new Parser(tokens);
const ast = parser.parse();
console.log(JSON.stringify(ast, null, 2));
