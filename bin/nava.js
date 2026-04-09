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
    .version('3.0.0');

program
    .command('run <file>')
    .description('Execute a Nava Sanskritam (.ns) script')
    .action((file) => {
        console.log("Nava Sanskritam V3.0 - Powered by NavaSanganakah\n");
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
            if (interpreter.darshakam) { interpreter.darshakam.generate(); }

        } catch (error) {
            console.error(`दोषः (Error during execution):\n${error.message}`);
            process.exit(1);
        }
    });


program
    .command('prachaar')
    .description('Deploy the generated dist files to Cloudflare Pages automatically')
    .action(() => {
        console.log("Nava Sanskritam V3.0 - Cloudflare Deployment\n");
        const distPath = path.resolve(process.cwd(), 'dist');
        if (!fs.existsSync(distPath)) {
            console.error("दोषः (Error): 'dist' directory not found. Please compile a web project first using 'nava run'.");
            process.exit(1);
        }

        console.log("Starting deployment via Wrangler CLI...");
        const { execSync } = require('child_process');
        try {
            // Using npx to ensure we don't need wrangler installed globally if not available,
            // but assuming environment setup is appropriate or user can login
            execSync('npx wrangler pages deploy dist --project-name nava-web-project', { stdio: 'inherit' });
            console.log("\nसफलम् (Success): Project successfully deployed to Cloudflare!");
        } catch (error) {
            console.error("\nदोषः (Error during deployment):\n", error.message);
            process.exit(1);
        }
    });

program.parse(process.argv);
