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

let nvcModule = null;

// WASM Compiler Integration
if (typeof NavaCompiler === 'function') {
    NavaCompiler().then(instance => {
        nvcModule = instance;
        console.log("🚩 नव्या-सङ्कलकः (WASM) सिद्धः!");
    });
}

// UI Rendering Logic (Darshanam)
const colorMap = { "रक्तवर्णः": "bg-red-500 text-white", "नीलवर्णः": "bg-blue-600 text-white", "श्वेतवर्णः": "bg-white text-black border border-gray-200", "हरितवर्णः": "bg-green-500 text-white", "पीतवर्णः": "bg-yellow-400 text-black", "कृष्णवर्णः": "bg-black text-white", "None": "" };

function createDrishyam(node) {
    const type = node.type.toLowerCase(); 
    let el;
    if (type === "box" || type === "मंजूषा") { 
        el = document.createElement("div"); 
        el.className = `p-6 rounded-2xl shadow-xl flex flex-col gap-4 m-2 transition-all duration-500 ${colorMap[node.color] || "bg-white/10 backdrop-blur-md border border-white/20"}`; 
    }
    else if (type === "button" || type === "बटनम्") { 
        el = document.createElement("button"); 
        el.innerText = node.label; 
        el.className = `px-6 py-3 rounded-xl font-bold transition-all shadow-lg hover:shadow-2xl hover:scale-105 active:scale-95 ${colorMap[node.color] || "bg-indigo-600 text-white"}`; 
    }
    else if (type === "text" || type === "पाठः") { 
        el = document.createElement("p"); 
        el.innerText = node.label || "नमस्ते"; 
        el.className = `text-lg font-medium text-gray-200`; 
    }
    else if (type === "image" || type === "चित्त्रम्") { 
        el = document.createElement("img"); 
        el.src = node.source || "https://api.placeholder.com/150"; 
        el.className = "rounded-xl object-cover shadow-lg w-full max-w-md h-auto"; 
    }
    else if (type === "input" || type === "प्रविष्टिः") { 
        el = document.createElement("input"); 
        el.placeholder = node.label || "लिखतु..."; 
        el.className = "px-4 py-3 rounded-lg bg-white/5 border border-white/20 text-white focus:outline-none focus:ring-2 focus:ring-blue-500 transition-all"; 
    }
    else { 
        el = document.createElement("div"); 
        el.innerText = "Unknown: " + node.type; 
        el.className = "text-red-500"; 
    }
    if (node.children) { 
        node.children.forEach(child => { 
            el.appendChild(createDrishyam(child)); 
        }); 
    }
    return el;
}

document.getElementById('run-btn').addEventListener('click', async () => {
    const code = window.editor.getValue();
    const out = document.getElementById('output-box');
    const load = document.getElementById('loading');
    
    out.innerText = "[NVC Engine] सङ्कलनं भवति (Compiling)...";
    load.classList.remove('hidden');
    
    if (nvcModule) {
        try {
            const compileSanskrit = nvcModule.cwrap('compileSanskrit', 'string', ['string']);
            const result = compileSanskrit(code);
            
            load.classList.add('hidden');
            out.innerText = "[LLVM WebAssembly Engine] Target: wasm32-unknown-unknown\nExecution Result:\n" + result;

            // Handle UI (Darshanam) rendering
            const root = document.getElementById('sul-root');
            root.innerHTML = ""; // Clear existing

            // Detect JSON Darshanam Block in result (Simulated for this script)
            if (result.startsWith('[') || result.startsWith('{')) {
                const data = JSON.parse(result);
                data.forEach(d => {
                    const c = document.createElement("div");
                    c.id = d.id;
                    c.className = "w-full max-w-4xl mx-auto py-10";
                    d.elements.forEach(e => { c.appendChild(createDrishyam(e)); });
                    root.appendChild(c);
                });
            }
        } catch (err) {
            load.classList.add('hidden');
            out.innerText = "त्रुटिः (Error): " + err.message;
        }
    } else {
        await new Promise(r => setTimeout(r, 1000));
        load.classList.add('hidden');
        out.innerText = "त्रुटिः: सङ्कलकः अद्यापि न सिद्धः! (Compiler not ready)";
    }
});
