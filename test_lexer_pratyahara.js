const Lexer = require('./Lexer.js');
const lexer = new Lexer("रामः पुस्तकम् पठति");
const tokens = lexer.tokenize();
console.log(tokens);
console.log("अक् pratyahara:", lexer.getPratyahara("अक्"));
