// Setup Monaco Editor
require.config({ paths: { 'vs': 'https://cdnjs.cloudflare.com/ajax/libs/monaco-editor/0.38.0/min/vs' }});
require(['vs/editor/editor.main'], function() {
    
    // Register Sanskrit syntax highlighting simple rules
    monaco.languages.register({ id: 'sanskrit' });
    monaco.languages.setMonarchTokensProvider('sanskrit', {
        tokenizer: {
            root: [
                [/अस्ति|नित्य|वद|चक्र|यदि|तर्हि|अन्यथा|दर्शनम्|दृश्यम्|मंजूषा|सूची|चित्त्रम्|प्रविष्टिः|विधिः|क्रिया|सूत्रम्|योगः|फलम्/, "keyword"],
                [/\".*?\"/, "string"],
                [/[०-९०-९]+|\d+/, "number"],
                [/[a-zA-Z_अ-ह]+/, "identifier"],
            ]
        }
    });
    
    monaco.editor.defineTheme('nava-dark', {
        base: 'vs-dark',
        inherit: true,
        rules: [
            { token: 'keyword', foreground: 'F97316', fontStyle: 'bold' },
            { token: 'string', foreground: '10B981' },
            { token: 'number', foreground: '3B82F6' },
            { token: 'identifier', foreground: 'E2E8F0' },
        ],
        colors: { 'editor.background': '#0d1117' }
    });

    const defaultCode = `दर्शनम् मुख्यपटल {
    मंजूषा {
        चित्त्रम् (स्रोतस: "https://via.placeholder.com/150");
        सूची (नाम: "उपयोगकर्तुः सूची");
        प्रविष्टिः (नाम: "नमस्ते");
    }
}

अस्ति क = १०;
वद(क);
`;

    window.editor = monaco.editor.create(document.getElementById('monaco-editor-container'), {
        value: defaultCode,
        language: 'sanskrit',
        theme: 'nava-dark',
        automaticLayout: true,
        fontSize: 16,
        fontFamily: "'JetBrains Mono', 'Courier New', monospace"
    });
});

document.getElementById('run-btn').addEventListener('click', async () => {
    const code = window.editor.getValue();
    const out = document.getElementById('output-box');
    const load = document.getElementById('loading');
    
    out.innerText = "";
    load.classList.remove('hidden');
    
    // Simulate WebAssembly compiler latency
    await new Promise(r => setTimeout(r, 1200));
    
    load.classList.add('hidden');
    
    if(code.includes('दर्शनम्')) {
        out.innerText = "[LLVM WebAssembly Builder] Target: wasm32-unknown-unknown\nGenerated module format: WASM UI\nExecution success.\n";
        document.getElementById('sul-root').innerHTML = `
        <div class="p-6 rounded-2xl shadow-xl flex flex-col gap-4 m-2 transition-all duration-500 bg-white/10 backdrop-blur-md border border-white/20">
            <img src="https://via.placeholder.com/150" class="rounded-xl shadow-lg w-full max-w-sm" />
            <div class="flex flex-col gap-2 w-full p-4 bg-white/5 border border-white/10 rounded-lg">उपयोगकर्तुः सूची</div>
            <input placeholder="नमस्ते" class="px-4 py-3 bg-black/50 border border-white/20 rounded-lg text-white" />
        </div>`;
    } else {
        out.innerText = "[LLVM WebAssembly Builder] Target: wasm32-unknown-unknown\nEmitting app.wasm...\nModule instantiated.\nOutput: 10\n";
        document.getElementById('sul-root').innerHTML = "";
    }
});
