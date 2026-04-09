#!/usr/bin/env node

const { program } = require('commander');
const fs = require('fs');
const path = require('path');

const Lexer = require('../Lexer');
const Parser = require('../Parser');
const Interpreter = require('../Interpreter');

program
    .name('nava')
    .description('Nava Sanskritam (Panini\'s Ashtadhyayi logic engine CLI)')
    .version('1.0.0');

program
    .command('run <file>')
    .description('Execute a Nava Sanskritam (.ns) script')
    .action((file) => {
        try {
            if (!file.endsWith('.ns')) {
                console.error("दोषः (Error): कृपया '.ns' इति विस्तारयुक्तं सञ्चिकां प्रयच्छतु। (Please provide a file with a '.ns' extension.)");
                process.exit(1);
            }

            const filePath = path.resolve(process.cwd(), file);

            if (!fs.existsSync(filePath)) {
                console.error(`दोषः (Error): सञ्चिका न प्राप्ता। (File not found: ${file})`);
                process.exit(1);
            }

            const input = fs.readFileSync(filePath, 'utf-8');

            const lexer = new Lexer(input);
            const tokens = lexer.tokenize();

            const parser = new Parser(tokens);
            const ast = parser.parse();

            const interpreter = new Interpreter();
            interpreter.interpret(ast);

        } catch (error) {
            console.error(`दोषः (Error during execution):\n${error.message}`);
            process.exit(1);
        }
    });

program.parse(process.argv);
