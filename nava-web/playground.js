// Setup Monaco Editor
require.config({ paths: { 'vs': 'https://cdnjs.cloudflare.com/ajax/libs/monaco-editor/0.38.0/min/vs' }});
require(['vs/editor/editor.main'], function() {
    
    // Register Sanskrit syntax highlighting simple rules
    monaco.languages.register({ id: 'sanskrit' });
    monaco.languages.setMonarchTokensProvider('sanskrit', {
        tokenizer: {
            root: [
                [/अस्ति|नित्य|वद|चक्र|यदि|तर्हि|अन्यथा|दर्शनम्|दृश्यम्|मंजूषा|सूची|चित्त्रम्|प्रविष्टिः|विधिः|क्रिया|सूत्रम्|योगः|फलम्|सम्पर्कः|अनुभवः/, "keyword"],
                [/\".*?\"/, "string"],
                [/[०-९०-९]+|\d+/, "number"],
                [/[a-zA-Z_अ-ह]+/, "identifier"],
                [/\/\/.*/, "comment"],
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
            { token: 'comment', foreground: '64748B', fontStyle: 'italic' },
        ],
        colors: { 'editor.background': '#0d1117' }
    });

    const defaultCode = `// 🚩 नव्या-संस्कृतम् दर्शनम्

दर्शनम् मुख्यपटल {
    मंजूषा (रंग: "नीलवर्णः") {
        चित्त्रम् (स्रोतस: "https://via.placeholder.com/300x150");
        पाठः (नाम: "🚩 नमस्ते नव्या-क्रीडाङ्गणतः!");
        प्रविष्टिः (नाम: "नाम लिखतु...");
        बटनम् (नाम: "चलाओ", रंग: "रक्तवर्णः");
    }
}

अस्ति क = १०;
वद("परिणामः:", क);
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
        const out = document.getElementById('output-box');
        out.innerText = "🚩 नव्या-सङ्कलकः सिद्धः! (Compiler Ready)";
        out.className = "font-mono text-emerald-400 whitespace-pre-wrap";
    });
}

// UI Rendering Logic (Darshanam)
const colorMap = { 
    "रक्तवर्णः": "bg-red-500 text-white", 
    "नीलवर्णः": "bg-blue-600 text-white shadow-blue-500/20", 
    "श्वेतवर्णः": "bg-white text-black border border-gray-200", 
    "हरितवर्णः": "bg-green-500 text-white", 
    "पीतवर्णः": "bg-yellow-400 text-black", 
    "कृष्णवर्णः": "bg-black text-white", 
    "None": "" 
};

function createDrishyam(node) {
    if (!node || !node.type) return null;
    const type = node.type.toLowerCase(); 
    let el;
    
    if (type === "box" || type === "मंजूषा") { 
        el = document.createElement("div"); 
        el.className = `p-8 rounded-3xl shadow-2xl flex flex-col gap-6 m-2 transition-all duration-500 ${colorMap[node.color] || "bg-white/10 backdrop-blur-xl border border-white/20"}`; 
    }
    else if (type === "button" || type === "बटनम्") { 
        el = document.createElement("button"); 
        el.innerText = node.label || node.name || "बटनम्"; 
        el.className = `px-8 py-4 rounded-2xl font-bold transition-all shadow-lg hover:shadow-2xl hover:scale-105 active:scale-95 ${colorMap[node.color] || "bg-amber-500 text-black"}`; 
    }
    else if (type === "text" || type === "पाठः" || type === "सूची") { 
        el = document.createElement("p"); 
        el.innerText = node.label || node.name || "पाठः"; 
        el.className = `text-xl font-semibold tracking-wide`; 
    }
    else if (type === "image" || type === "चित्त्रम्") { 
        el = document.createElement("img"); 
        el.src = node.source || node.स्रोतस || "https://api.placeholder.com/300x150"; 
        el.className = "rounded-2xl object-cover shadow-2xl w-full max-w-lg h-auto border-4 border-white/10"; 
    }
    else if (type === "input" || type === "प्रविष्टिः") { 
        el = document.createElement("input"); 
        el.placeholder = node.label || node.name || "लिखतु..."; 
        el.className = "px-6 py-4 rounded-xl bg-black/40 border border-white/10 text-white focus:outline-none focus:ring-4 focus:ring-amber-500/50 transition-all placeholder:text-slate-500"; 
    }
    else { 
        el = document.createElement("div"); 
        el.innerText = "अज्ञात-घटकः: " + node.type; 
        el.className = "text-red-400 bg-red-500/10 p-4 rounded-xl border border-red-500/20"; 
    }

    if (node.children && Array.isArray(node.children)) { 
        node.children.forEach(child => { 
            const childEl = createDrishyam(child);
            if (childEl) el.appendChild(childEl); 
        }); 
    }
    return el;
}

function extractJson(str) {
    const firstBracket = str.indexOf('[');
    const firstBrace = str.indexOf('{');
    const start = (firstBracket !== -1 && (firstBrace === -1 || firstBracket < firstBrace)) ? firstBracket : firstBrace;
    if (start === -1) return null;
    
    // Attempt to find the last corresponding closing bracket/brace
    const lastBracket = str.lastIndexOf(']');
    const lastBrace = str.lastIndexOf('}');
    const end = Math.max(lastBracket, lastBrace);
    
    if (end === -1 || end < start) return null;
    return str.substring(start, end + 1);
}

document.getElementById('run-btn').addEventListener('click', async () => {
    const code = window.editor.getValue();
    const out = document.getElementById('output-box');
    const load = document.getElementById('loading');
    const root = document.getElementById('sul-root');
    
    out.innerText = "[NVC इञ्जन] सङ्कलनं भवति...";
    out.className = "font-mono text-amber-400 animate-pulse";
    load.classList.remove('hidden');
    
    if (nvcModule) {
        try {
            const compileSanskrit = nvcModule.cwrap('compileSanskrit', 'string', ['string']);
            const result = compileSanskrit(code);
            
            load.classList.add('hidden');
            
            // Check for Errors
            if (result.includes("त्रुटिः") || result.includes("Error")) {
                out.className = "font-mono text-red-500 whitespace-pre-wrap bg-red-500/5 p-2 rounded border border-red-500/20";
                out.innerText = "[सङ्कलन-दोषः]\n" + result;
            } else {
                out.className = "font-mono text-emerald-400 whitespace-pre-wrap";
                out.innerText = "[LLVM WASM] निष्पादनं सफलम्।\n" + result;

                // Handle UI (Darshanam) rendering
                root.innerHTML = ""; 
                const jsonStr = extractJson(result);
                if (jsonStr) {
                    try {
                        const data = JSON.parse(jsonStr);
                        const nodes = Array.isArray(data) ? data : [data];
                        nodes.forEach(d => {
                            const c = document.createElement("div");
                            c.id = d.id || "main-view";
                            c.className = "w-full max-w-4xl mx-auto py-12 flex flex-col gap-4";
                            if (d.elements) {
                                d.elements.forEach(e => { 
                                    const rendered = createDrishyam(e);
                                    if (rendered) c.appendChild(rendered); 
                                });
                            }
                            root.appendChild(c);
                        });
                    } catch (e) {
                        console.warn("JSON Parse Error:", e);
                    }
                }
            }
        } catch (err) {
            load.classList.add('hidden');
            out.className = "font-mono text-rose-500";
            out.innerText = "Fatal इञ्जन-दोषः: " + err.message;
        }
    } else {
        await new Promise(r => setTimeout(r, 800));
        load.classList.add('hidden');
        out.className = "font-mono text-amber-500";
        out.innerText = "त्रुटिः: सङ्कलकः अद्यापि न सिद्धः! कृपया पुनः यत्नं कुरु।";
    }
});
