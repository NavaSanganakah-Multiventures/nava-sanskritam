const { ipcRenderer } = window.nodeRequire ? window.nodeRequire('electron') : require('electron');
const fs = window.nodeRequire ? window.nodeRequire('fs') : require('fs');
const path = window.nodeRequire ? window.nodeRequire('path') : require('path');

let editor;
let currentFilePath = null;

// Real Sanskrit Transliteration Engine (Handling Matras & Conjuncts)
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
        
        // 1. Check for Consonant
        let c = null;
        let cLen = 0;
        if (consonants[chunk2]) { c = consonants[chunk2]; cLen = 2; }
        else if (consonants[chunk1]) { c = consonants[chunk1]; cLen = 1; }
        
        if (c) {
            i += cLen;
            let v = null;
            let vLen = 0;
            let nextChunk2 = str.substring(i, i + 2);
            let nextChunk1 = str.substring(i, i + 1);
            
            if (vowels[nextChunk2] !== undefined) { v = vowels[nextChunk2]; vLen = 2; }
            else if (vowels[nextChunk1] !== undefined) { v = vowels[nextChunk1]; vLen = 1; }
            
            if (v !== null) {
                result += c + v;
                i += vLen;
            } else {
                // No vowel following - add Virama (half-letter) unless it's the end of word and we want schwa?
                // For Sanskrit, usually it's better to add Virama for lone consonants.
                result += c + virama;
            }
        } 
        // 2. Check for Independent Vowel
        else {
            let iv = null;
            let ivLen = 0;
            if (independentVowels[chunk2]) { iv = independentVowels[chunk2]; ivLen = 2; }
            else if (independentVowels[chunk1]) { iv = independentVowels[chunk1]; ivLen = 1; }
            
            if (iv) {
                result += iv;
                i += ivLen;
            } else {
                result += chunk1;
                i++;
            }
        }
    }
    
    // Cleanup double Viramas or Virama at end if user typed 'a'
    return result.replace(/्ा/g, 'ा').replace(/्ि/g, 'ि').replace(/्ी/g, 'ी').replace(/्ु/g, 'ु').replace(/्ू/g, 'ू').replace(/्े/g, 'े').replace(/्ै/g, 'ै').replace(/्ो/g, 'ो').replace(/्ौ/g, 'ौ').replace(/्ं/g, 'ं').replace(/्ः/g, 'ः').replace(/् /g, ' ');
}

// Load Monaco Editor
require.config({ paths: { 'vs': 'https://cdnjs.cloudflare.com/ajax/libs/monaco-editor/0.34.0/min/vs' } });

require(['vs/editor/editor.main'], function () {
    monaco.languages.register({ id: 'nava' });
    monaco.languages.setMonarchTokensProvider('nava', {
        tokenizer: {
            root: [
                [/\b(अस्ति|वद|यदि|तर्हि|चक्र|अन्यथा|फलम्|विधिः|नित्य)\b/, 'keyword'],
                [/"[^"]*"/, 'string'],
                [/\b\d+\b/, 'number'],
                [/\/\/.*/, 'comment'],
            ]
        }
    });

    editor = monaco.editor.create(document.getElementById('editor-container'), {
        value: '// नमस्ते! नव-सङ्स्कृतम् कोडं लिखतु...\n\nवद("🚩 नमनम् नव-सङ्गणक-शालातः!");\n',
        language: 'nava',
        theme: 'vs-dark',
        fontSize: 16,
        automaticLayout: true,
        minimap: { enabled: false }
    });

    // Transliteration Listener
    editor.onKeyUp((e) => {
        if (!document.getElementById('keyboard-toggle').checked) return;
        
        if (e.keyCode === monaco.KeyCode.Space) {
            const model = editor.getModel();
            const pos = editor.getPosition();
            const lineContent = model.getLineContent(pos.lineNumber);
            
            // Get text before cursor
            const beforeCursor = lineContent.substring(0, pos.column - 1);
            const words = beforeCursor.split(' ');
            const lastWord = words[words.length - 1];
            
            if (lastWord && /^[a-zA-Z]+$/.test(lastWord)) {
                const devanagari = transliterate(lastWord.toLowerCase());
                const range = new monaco.Range(pos.lineNumber, pos.column - lastWord.length - 1, pos.lineNumber, pos.column - 1);
                editor.executeEdits("transliteration", [{ range: range, text: devanagari }]);
            }
        }
    });

    updateFileExplorer();
});

// File Explorer Logic
function updateFileExplorer() {
    const cwd = process.cwd();
    const fileListMsg = document.getElementById('file-list');
    fileListMsg.innerHTML = '';

    fs.readdir(cwd, (err, files) => {
        if (err) return;
        
        files.filter(f => f.endsWith('.ns')).forEach(file => {
            const div = document.createElement('div');
            div.className = 'file-item';
            div.innerText = '📄 ' + file;
            div.onclick = (e) => loadFile(path.join(cwd, file), e.target);
            fileListMsg.appendChild(div);
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

document.getElementById('run-btn').addEventListener('click', () => {
    const code = editor.getValue();
    const output = document.getElementById('terminal-output');
    output.innerText = 'सङ्कलनं भवति...';
    ipcRenderer.send('run-code', code);
});

document.getElementById('save-btn').addEventListener('click', () => {
    const code = editor.getValue();
    if (currentFilePath) {
        fs.writeFileSync(currentFilePath, code);
        const output = document.getElementById('terminal-output');
        output.innerText += '\nसञ्चिका सुरक्षिता!';
    } else {
        ipcRenderer.send('save-file', code);
    }
});

ipcRenderer.on('run-result', (event, result) => {
    const output = document.getElementById('terminal-output');
    if (result.success) {
        output.style.color = '#10b981';
        output.innerText = 'सफलता:\n' + result.output;
    } else {
        output.style.color = '#ef4444';
        output.innerText = 'त्रुटिः:\n' + result.output;
    }
});

document.getElementById('terminal-input').addEventListener('keydown', (e) => {
    if (e.key === 'Enter') {
        const cmd = e.target.value.trim().toLowerCase();
        const output = document.getElementById('terminal-output');
        output.innerText += '\n🚩 > ' + cmd;
        
        if (cmd === 'चलाओ' || cmd === 'run') {
            document.getElementById('run-btn').click();
        } else if (cmd === 'संग्रहः' || cmd === 'save') {
            document.getElementById('save-btn').click();
        } else if (cmd === 'साफ' || cmd === 'clear') {
            output.innerText = 'नव-सङ्गणक-शालायां स्वागतम्...';
        } else {
            output.innerText += '\nत्रुटिः: अमान्या आज्ञा';
        }
        
        e.target.value = '';
        output.scrollTop = output.scrollHeight;
    }
});
