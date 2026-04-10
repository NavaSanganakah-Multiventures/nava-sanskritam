const { ipcRenderer } = window.nodeRequire ? window.nodeRequire('electron') : require('electron');
const fs = window.nodeRequire ? window.nodeRequire('fs') : require('fs');
const path = window.nodeRequire ? window.nodeRequire('path') : require('path');

let editor;
let currentFilePath = null;

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
        value: '// नमस्ते! Nava Sanskritam Code यहाँ लिखें...\n\nवद("🚩 नमनम् Nava Studio!");\n',
        language: 'nava',
        theme: 'vs-dark',
        fontSize: 16,
        automaticLayout: true,
        minimap: { enabled: false }
    });

    // Initial Scan
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
            div.onclick = () => loadFile(path.join(cwd, file));
            fileListMsg.appendChild(div);
        });
    });
}

function loadFile(filePath) {
    const content = fs.readFileSync(filePath, 'utf8');
    editor.setValue(content);
    currentFilePath = filePath;
    
    // UI Feedback
    document.querySelectorAll('.file-item').forEach(el => el.classList.remove('active'));
    event.target.classList.add('active');
}

// Run Button Event
document.getElementById('run-btn').addEventListener('click', () => {
    const code = editor.getValue();
    const output = document.getElementById('terminal-output');
    output.innerText = 'संकलनं भवति... (Compiling...)';
    
    ipcRenderer.send('run-code', code);
});

// Save Button Event
document.getElementById('save-btn').addEventListener('click', () => {
    const code = editor.getValue();
    if (currentFilePath) {
        fs.writeFileSync(currentFilePath, code);
        const output = document.getElementById('terminal-output');
        output.innerText += '\nसञ्चिका सुरक्षिता! (File Saved!)';
    } else {
        ipcRenderer.send('save-file', code);
    }
});

// Receive Result
ipcRenderer.on('run-result', (event, result) => {
    const output = document.getElementById('terminal-output');
    if (result.success) {
        output.style.color = '#10b981';
        output.innerText = 'सफलता (Result):\n' + result.output;
    } else {
        output.style.color = '#ef4444';
        output.innerText = 'त्रुटि (Error):\n' + result.output;
    }
});
