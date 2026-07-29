const Lexer = require('./Lexer');
const lexer = new Lexer('अस्ति रामः = १०; वद(रामम्); यदि (रामे > ५) तर्हि वद(सत्यम्);');
console.log(lexer.tokenize());
