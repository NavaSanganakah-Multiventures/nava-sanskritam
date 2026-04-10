const { ipcRenderer } = window.nodeRequire ? window.nodeRequire('electron') : require('electron');

let editor;

// Load Monaco Editor
require.config({ paths: { 'vs': 'https://cdnjs.cloudflare.com/ajax/libs/monaco-editor/0.34.0/min/vs' } });

require(['vs/editor/editor.main'], function () {
    // Define Nava Sanskritam Language
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
});

// Run Button Event
document.getElementById('run-btn').addEventListener('click', () => {
    const code = editor.getValue();
    const outputContainer = document.getElementById('output-container');
    
    outputContainer.innerText = 'Compiling... (संकलनं भवति...)';
    
    ipcRenderer.send('run-code', code);
});

// Save Button Event
document.getElementById('save-btn').addEventListener('click', () => {
    const code = editor.getValue();
    ipcRenderer.send('save-file', code);
});

// Receive Result
ipcRenderer.on('run-result', (event, result) => {
    const outputContainer = document.getElementById('output-container');
    if (result.success) {
        outputContainer.style.color = '#10b981';
        outputContainer.innerText = result.output;
    } else {
        outputContainer.style.color = '#ef4444';
        outputContainer.innerText = 'त्रुटि (Error):\n' + result.output;
    }
});
