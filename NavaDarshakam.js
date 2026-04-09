const fs = require('fs');
const path = require('path');

class NavaDarshakam {
    constructor() {
        this.htmlElements = [];
        this.styles = {};
        this.cssClasses = new Map();
        this.used = false;

        // Maps style properties
        this.styleMap = {
            'वर्णः': 'color',
            'आकारः': 'font-size',
            'सीमा': 'border',
            'अन्तरालम्': 'padding'
        };
    }

    addTag(tag, content, styleObj = null, role = 'None') {
        this.used = true;
        let mappedTag = tag;
        switch (tag) {
            case 'अङ्गम्': mappedTag = 'div'; break;
            case 'शीर्षकम्': mappedTag = 'h1'; break;
            case 'अनुच्छेदः': mappedTag = 'p'; break;
            case 'सूची': mappedTag = 'ul'; break;
            case 'बटनम्': mappedTag = 'button'; break;
        }

        let cssStr = '';
        if (styleObj) {
            cssStr += this.parseStyles(styleObj);
        }

        // Apply automatic dynamic styling based on grammatical roles
        if (role === 'Subject') {
            cssStr += 'color: #1E3A8A; font-weight: bold; '; // Distinct color + bold
        } else if (role === 'Object') {
            cssStr += 'color: #065F46; font-style: italic; '; // Distinct color + italic
        } else if (role === 'Locative/Dative') {
            cssStr += 'color: #92400E; text-decoration: underline; ';
        }

        let classStr = '';
        if (cssStr !== '') {
            let className = 'ns-style-' + Math.random().toString(36).substring(2, 9);
            this.cssClasses.set(className, cssStr);
            classStr = ` class="${className}"`;
        }

        this.htmlElements.push(`<${mappedTag}${classStr}>${content}</${mappedTag}>`);
    }

    parseStyles(styleObj) {
        let cssStr = '';
        if (typeof styleObj === 'string') {
            // For layout like 'रचना: श्रेणी'
            if (styleObj === 'रचना: श्रेणी') {
                cssStr += 'display: flex; flex-direction: row; ';
            } else if (styleObj === 'रचना: स्तम्भ') {
                 cssStr += 'display: flex; flex-direction: column; ';
            } else if (styleObj.startsWith('रचना: जाल')) {
                 cssStr += 'display: grid; ';
            }
        } else if (typeof styleObj === 'object') {
            for (let [key, val] of Object.entries(styleObj)) {
                if (this.styleMap[key]) {
                    cssStr += `${this.styleMap[key]}: ${val}; `;
                }
            }
        }
        return cssStr;
    }

    // Additional generic style string appending
    addStyleClass(className, styleProps) {
        let css = '';
        for (let [k, v] of Object.entries(styleProps)) {
            if (this.styleMap[k]) {
                css += `${this.styleMap[k]}: ${v}; `;
            } else if (k === 'रचना') {
                if (v === 'श्रेणी') css += 'display: flex; flex-direction: row; ';
                else if (v === 'स्तम्भ') css += 'display: flex; flex-direction: column; ';
                else if (v === 'जाल') css += 'display: grid; ';
            }
        }
        this.cssClasses.set(className, css);
    }

    generate() {
        if (!this.used) return;

        const distDir = path.resolve(process.cwd(), 'dist');
        if (!fs.existsSync(distDir)) {
            fs.mkdirSync(distDir);
        }

        let cssContent = '';
        for (let [className, css] of this.cssClasses.entries()) {
            cssContent += `.${className} { ${css} }\n`;
        }

        let htmlContent = `<!DOCTYPE html>
<html lang="sa">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>NavaSanganakah Web</title>
    <link rel="stylesheet" href="style.css">
</head>
<body>
    ${this.htmlElements.join('\n    ')}
</body>
</html>`;

        fs.writeFileSync(path.join(distDir, 'index.html'), htmlContent);
        fs.writeFileSync(path.join(distDir, 'style.css'), cssContent);
        console.log("Nava-Darshakam: Web files generated in 'dist' directory. (index.html, style.css)");
    }
}

module.exports = NavaDarshakam;
