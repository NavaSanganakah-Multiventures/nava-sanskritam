const Lexer = require('./Lexer');
const lexer = new Lexer('अस्ति संख्याः = १०; वद(संख्याम्);');
console.log(lexer.tokenize());
