const { ipcRenderer } = window.nodeRequire ? window.nodeRequire('electron') : require('electron');
const fs = window.nodeRequire ? window.nodeRequire('fs') : require('fs');
const path = window.nodeRequire ? window.nodeRequire('path') : require('path');

let editor;
let currentFilePath = null;

// Real Sanskrit Transliteration Engine
const consonants = {
    'k': 'क', 'kh': 'ख', 'g': 'ग', 'gh': 'घ', 'ng': 'ङ',
    'c': 'च', 'ch': 'छ', 'j': 'ज', 'jh': 'झ', 'ny': 'ञ',
    'T': 'ट', 'Th': 'ठ', 'D': 'ड', 'Dh': 'ढ', 'N': 'ण',
    't': 'त', 'th': 'थ', 'd': 'द', 'dh': 'ध', 'n': 'न',
    'p': 'प', 'ph': 'फ', 'b': 'ब', 'bh': 'भ', 'm': 'म',
    'y': 'य', 'r': 'र', 'l': 'ल', 'v': 'व', 'sh': 'श', 'shh': 'ष', 's': 'स', 'h': 'ह', 'L': 'ळ'
};

const vowels = {
    'a': '', 'aa': 'ा', 'i': 'ि', 'ii': 'ी', 'u': 'ु', 'uu': 'ू', 'e': 'े', 'ai': 'ै', 'o': 'ो', 'au': 'ौ', 'an': 'ं', 'ah': 'ः'
};

const independentVowels = {
    'a': 'अ', 'aa': 'आ', 'i': 'इ', 'ii': 'ई', 'u': 'उ', 'uu': 'ऊ', 'e': 'ए', 'ai': 'ऐ', 'o': 'ओ', 'au': 'औ', 'an': 'अं', 'ah': 'अः'
};

const virama = '्';

function transliterate(str) {
    let result = '';
    let i = 0;
    while (i < str.length) {
        let chunk2 = str.substring(i, i + 2);
        let chunk1 = str.substring(i, i + 1);
        let c = null; let cLen = 0;
        if (consonants[chunk2]) { c = consonants[chunk2]; cLen = 2; }
        else if (consonants[chunk1]) { c = consonants[chunk1]; cLen = 1; }
        
        if (c) {
            i += cLen;
            let v = null; let vLen = 0;
            let next2 = str.substring(i, i + 2);
            let next1 = str.substring(i, i + 1);
            if (vowels[next2] !== undefined) { v = vowels[next2]; vLen = 2; }
            else if (vowels[next1] !== undefined) { v = vowels[next1]; vLen = 1; }
            
            if (v !== null) { result += c + v; i += vLen; }
            else { result += c + virama; }
        } else {
            let iv = null; let ivLen = 0;
            if (independentVowels[chunk2]) { iv = independentVowels[chunk2]; ivLen = 2; }
            else if (independentVowels[chunk1]) { iv = independentVowels[chunk1]; ivLen = 1; }
            if (iv) { result += iv; i += ivLen; }
            else { result += chunk1; i++; }
        }
    }
    return result.replace(/्ा/g, 'ा').replace(/्ि/g, 'ि').replace(/्ी/g, 'ी').replace(/्ु/g, 'ु').replace(/्ू/g, 'ू').replace(/्े/g, 'े').replace(/्ै/g, 'ै').replace(/्ो/g, 'ो').replace(/्ौ/g, 'ौ').replace(/्ं/g, 'ं').replace(/्ः/g, 'ः').replace(/् /g, ' ');
}

// Load Monaco Editor
require.config({ paths: { 'vs': 'https://cdnjs.cloudflare.com/ajax/libs/monaco-editor/0.34.0/min/vs' } });

require(['vs/editor/editor.main'], function () {
    monaco.languages.register({ id: 'nava' });
    monaco.languages.setMonarchTokensProvider('nava', {
        tokenizer: {
            root: [
                [/\b(अस्ति|वद|यदि|तर्हि|चक्र|अन्यथा|फलम्|विधिः|नित्य|दर्शनम्|दृश्यम्|मंजूषा|सूची|चित्त्रम्|प्रविष्टिः|क्रिया|सूत्रम्|योगः)\b/, 'keyword'],
                [/"[^"]*"/, 'string'],
                [/\b\d+\b/, 'number'],
                [/\/\/.*/, 'comment'],
            ]
        }
    });

    editor = monaco.editor.create(document.getElementById('editor-container'), {
        value: '// नमस्ते! नव-सङ्कृतम् कोडं लिखतु...\n\nवद("🚩 नमनम् नव-सङ्गणक-शालातः!");\n',
        language: 'nava', theme: 'vs-dark', fontSize: 16, automaticLayout: true, minimap: { enabled: false }
    });

    // Editor Transliteration Fix (Regex-based)
    editor.onKeyUp((e) => {
        if (!document.getElementById('keyboard-toggle').checked) return;
        if (e.keyCode === monaco.KeyCode.Space || e.browserEvent.key === ' ') {
            const model = editor.getModel();
            const pos = editor.getPosition();
            const lineContent = model.getLineContent(pos.lineNumber);
            const textBefore = lineContent.substring(0, pos.column - 1);
            
            // Find last word ending with Space
            const match = textBefore.match(/([a-zA-Z]+)\s$/);
            if (match) {
                const word = match[1];
                const devanagari = transliterate(word.toLowerCase());
                const range = new monaco.Range(pos.lineNumber, pos.column - word.length - 1, pos.lineNumber, pos.column - 1);
                editor.executeEdits("transliteration", [{ range: range, text: devanagari }]);
            }
        }
    });

    // Sanskrit IntelliSense (Autocomplete)
    monaco.languages.registerCompletionItemProvider('nava', {
        provideCompletionItems: (model, position) => {
            const keywords = ['अस्ति', 'वद', 'यदि', 'तर्हि', 'चक्र', 'अन्यथा', 'फलम्', 'विधिः', 'नित्य', 'दर्शनम्', 'दृश्यम्', 'मंजूषा', 'सूची', 'चित्त्रम्', 'प्रविष्टिः', 'क्रिया', 'सूत्रम्', 'योगः'];
            const libraryFunctions = ['त्रैराशिकम्', 'नियमः', 'गणनम्', 'वर्गः'];
            
            const suggestions = keywords.map(kw => ({
                label: kw,
                kind: monaco.languages.CompletionItemKind.Keyword,
                insertText: kw
            }));

            libraryFunctions.forEach(fn => {
                suggestions.push({
                    label: fn,
                    kind: monaco.languages.CompletionItemKind.Function,
                    insertText: fn + '()',
                    detail: 'मानक-पुस्तकालय-विधिः (Standard Library Function)'
                });
            });

            // Dynamic Variable Suggestions
            const text = model.getValue();
            const varRegex = /(?:अस्ति|नित्य)\s+([^\s;=]+)/g;
            let match;
            const foundVars = new Set();
            while ((match = varRegex.exec(text)) !== null) {
                foundVars.add(match[1]);
            }

            foundVars.forEach(v => {
                suggestions.push({
                    label: v,
                    kind: monaco.languages.CompletionItemKind.Variable,
                    insertText: v,
                    detail: 'नव्या-सञ्चिका-चरः (SUL Variable)'
                });
            });

            return { suggestions: suggestions };
        }
    });

    updateFileExplorer();

    // Real-time Diagnostics (साक्षात्-दोष-दर्शनम्)
    let lintTimeout;
    editor.onDidChangeModelContent(() => {
        clearTimeout(lintTimeout);
        lintTimeout = setTimeout(() => {
            if (!nvcModule) return;
            const code = editor.getValue();
            const compileSanskrit = nvcModule.cwrap('compileSanskrit', 'string', ['string']);
            const result = compileSanskrit(code);
            
            const markers = [];
            if (result.includes('त्रुटि')) {
                // Parse coordinates for Red Underline
                // Format: ... - पङ्क्तिः 6, स्तम्भः 15
                const lineMatch = result.match(/पङ्क्तिः\s+(\d+)/);
                const colMatch = result.match(/स्तम्भः\s+(\d+)/);
                
                if (lineMatch) {
                    const line = parseInt(lineMatch[1]);
                    const col = colMatch ? parseInt(colMatch[1]) : 1;
                    markers.push({
                        startLineNumber: line,
                        startColumn: col,
                        endLineNumber: line,
                        endColumn: col + 5, // Approximate
                        message: result,
                        severity: monaco.MarkerSeverity.Error
                    });
                }
            }
            monaco.editor.setModelMarkers(editor.getModel(), 'nava', markers);
        }, 500);
    });
});

// File Explorer Logic
function updateFileExplorer() {
    const cwd = process.cwd();
    const list = document.getElementById('file-list');
    list.innerHTML = '';
    fs.readdir(cwd, (err, files) => {
        if (err) return;
        files.filter(f => f.endsWith('.ns')).forEach(file => {
            const div = document.createElement('div');
            div.className = 'file-item';
            div.innerText = '📄 ' + file;
            div.onclick = (e) => loadFile(path.join(cwd, file), e.target);
            list.appendChild(div);
        });
    });
}
function loadFile(filePath, element) {
    const content = fs.readFileSync(filePath, 'utf8');
    editor.setValue(content);
    currentFilePath = filePath;
    document.querySelectorAll('.file-item').forEach(el => el.classList.remove('active'));
    element.classList.add('active');
}

// WASM Compiler Integration
let nvcModule = null;
if (typeof NavaCompiler === 'function') {
    NavaCompiler().then(instance => {
        nvcModule = instance;
        const output = document.getElementById('terminal-output');
        if (output) output.innerText += '\n🚩 नव्या-सङ्कलकः (WASM) सिद्धः!';
    });
}

document.getElementById('run-btn').addEventListener('click', () => {
    const code = editor.getValue();
    const output = document.getElementById('terminal-output');
    output.innerText = 'सङ्कलनं भवति (Compiling)...';
    
    if (nvcModule) {
        try {
            const compileSanskrit = nvcModule.cwrap('compileSanskrit', 'string', ['string']);
            const result = compileSanskrit(code);
            output.style.color = '#10b981';
            output.innerText = 'सफलता (Success):\n' + result;
        } catch (err) {
            output.style.color = '#ef4444';
            output.innerText = 'त्रुटि (Error):\n' + err.message;
        }
    } else {
        ipcRenderer.send('run-code', code);
    }
});
document.getElementById('save-btn').addEventListener('click', () => {
    if (currentFilePath) {
        fs.writeFileSync(currentFilePath, editor.getValue());
        document.getElementById('terminal-output').innerText += '\nसञ्चिका सुरक्षिता!';
    } else { ipcRenderer.send('save-file', editor.getValue()); }
});
ipcRenderer.on('run-result', (event, result) => {
    const output = document.getElementById('terminal-output');
    if (result.success) { output.style.color = '#10b981'; output.innerText = 'सफलता:\n' + result.output; }
    else { output.style.color = '#ef4444'; output.innerText = 'त्रुटिः:\n' + result.output; }
});

// Terminal Input + Transliteration
const termInput = document.getElementById('terminal-input');
termInput.addEventListener('keyup', (e) => {
    if (!document.getElementById('keyboard-toggle').checked) return;
    if (e.key === ' ' || e.code === 'Space') {
        const val = termInput.value;
        const parts = val.split(' ');
        const last = parts[parts.length - 2]; // Get word before current space
        if (last && /^[a-zA-Z]+$/.test(last)) {
            parts[parts.length - 2] = transliterate(last.toLowerCase());
            termInput.value = parts.join(' ');
        }
    }
});
termInput.addEventListener('keydown', (e) => {
    if (e.key === 'Enter') {
        const cmd = termInput.value.trim().toLowerCase();
        const output = document.getElementById('terminal-output');
        output.innerText += '\n🚩 > ' + termInput.value;
        if (cmd === 'चालय' || cmd === 'run') { document.getElementById('run-btn').click(); }
        else if (cmd === 'संग्रहः' || cmd === 'save') { document.getElementById('save-btn').click(); }
        else if (cmd === 'साफ' || cmd === 'clear') { output.innerText = 'नव-सङ्गणक-शालायां स्वागतम्...'; }
        else { output.innerText += '\nत्रुटिः: अमान्या आज्ञा'; }
        termInput.value = '';
        output.scrollTop = output.scrollHeight;
    }
});
