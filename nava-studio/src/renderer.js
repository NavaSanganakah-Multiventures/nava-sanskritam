const { ipcRenderer } = window.nodeRequire ? window.nodeRequire('electron') : require('electron');
const fs = window.nodeRequire ? window.nodeRequire('fs') : require('fs');
const path = window.nodeRequire ? window.nodeRequire('path') : require('path');

let editor;
let currentFilePath = null;

// Sanskrit Transliteration Engine (Simple mapping)
const sansMapping = {
    'a': 'अ', 'aa': 'आ', 'i': 'इ', 'ii': 'ई', 'u': 'उ', 'uu': 'ऊ', 'e': 'ए', 'ai': 'ऐ', 'o': 'ओ', 'au': 'औ',
    'k': 'क', 'kh': 'ख', 'g': 'ग', 'gh': 'घ', 'ng': 'ङ',
    'c': 'च', 'ch': 'छ', 'j': 'ज', 'jh': 'झ', 'ny': 'ञ',
    'T': 'ट', 'Th': 'ठ', 'D': 'ड', 'Dh': 'ढ', 'N': 'ण',
    't': 'त', 'th': 'थ', 'd': 'द', 'dh': 'ध', 'n': 'न',
    'p': 'प', 'ph': 'फ', 'b': 'ब', 'bh': 'भ', 'm': 'म',
    'y': 'य', 'r': 'र', 'l': 'ल', 'v': 'व', 'sh': 'श', 'shh': 'ष', 's': 'स', 'h': 'ह',
    'om': 'ॐ', ' ': ' '
};

// Vowel Signs
const vowelSigns = {
    'aa': 'ा', 'i': 'ि', 'ii': 'ी', 'u': 'ु', 'uu': 'ू', 'e': 'े', 'ai': 'ै', 'o': 'ो', 'au': 'ौ'
};

// Phonetic conversion wrapper
function transliterate(str) {
    // This is a simplified "greedy" mapper for Devanagari
    let result = '';
    let i = 0;
    while (i < str.length) {
        let chunk3 = str.substring(i, i + 3);
        let chunk2 = str.substring(i, i + 2);
        let chunk1 = str.substring(i, i + 1);

        if (sansMapping[chunk3]) { result += sansMapping[chunk3]; i += 3; }
        else if (sansMapping[chunk2]) { result += sansMapping[chunk2]; i += 2; }
        else if (sansMapping[chunk1]) { result += sansMapping[chunk1]; i += 1; }
        else { result += chunk1; i++; }
    }
    return result;
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
        
        // Only trigger on space or word boundaries for simplicity
        if (e.keyCode === monaco.KeyCode.Space) {
            const model = editor.getModel();
            const pos = editor.getPosition();
            const lineContent = model.getLineContent(pos.lineNumber);
            const word = lineContent.trim().split(' ').pop();
            
            if (word && /^[a-zA-Z]+$/.test(word)) {
                const devanagari = transliterate(word.toLowerCase());
                const range = new monaco.Range(pos.lineNumber, pos.column - word.length - 1, pos.lineNumber, pos.column - 1);
                editor.executeEdits("transliteration", [{ range: range, text: devanagari }]);
            }
        }
    });

    updateFileExplorer();
});

// File Explorer Logic (Pure Sanskrit)
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

// Run Button Event (Pure Sanskrit)
document.getElementById('run-btn').addEventListener('click', () => {
    const code = editor.getValue();
    const output = document.getElementById('terminal-output');
    output.innerText = 'सङ्कलनं भवति... (Sankalanam...)';
    
    ipcRenderer.send('run-code', code);
});

// Save Button Event (Pure Sanskrit)
document.getElementById('save-btn').addEventListener('click', () => {
    const code = editor.getValue();
    if (currentFilePath) {
        fs.writeFileSync(currentFilePath, code);
        const output = document.getElementById('terminal-output');
        output.innerText += '\nसञ्चिका सुरक्षिता! (Pustakam Rakshitam!)';
    } else {
        ipcRenderer.send('save-file', code);
    }
});

// Receive Result (Pure Sanskrit)
ipcRenderer.on('run-result', (event, result) => {
    const output = document.getElementById('terminal-output');
    if (result.success) {
        output.style.color = '#10b981';
        output.innerText = 'सफलता (Saphala):\n' + result.output;
    } else {
        output.style.color = '#ef4444';
        output.innerText = 'त्रुटिः (Trutih):\n' + result.output;
    }
});

// Terminal Input Logic (Sanskrit Only)
document.getElementById('terminal-input').addEventListener('keydown', (e) => {
    if (e.key === 'Enter') {
        const cmd = e.target.value.trim().toLowerCase();
        const output = document.getElementById('terminal-output');
        
        output.innerText += '\n🚩 > ' + cmd;
        
        if (cmd === 'चलाओ' || cmd === 'run' || cmd === 'chalaya') {
            document.getElementById('run-btn').click();
        } else if (cmd === 'संग्रहः' || cmd === 'save' || cmd === 'rakshaya') {
            document.getElementById('save-btn').click();
        } else if (cmd === 'साफ' || cmd === 'clear' || cmd === 'shodhaya') {
            output.innerText = 'नव-सङ्गणक-शालायां स्वागतम्...';
        } else {
            output.innerText += '\nत्रुटिः: अमान्या आज्ञा (Unknown command)';
        }
        
        e.target.value = '';
        output.scrollTop = output.scrollHeight;
    }
});
