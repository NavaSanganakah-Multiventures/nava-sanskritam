#include "Lexer.hpp"
#include "Parser.hpp"
#include "CodeGen.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdio>

const char* SUL_WEB_TEMPLATE = R"HTML(
<!DOCTYPE html>
<html lang="sa">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Nava Sanskritam Darshanam</title>
    <script src="https://cdn.tailwindcss.com"></script>
    <script>
        tailwind.config = { theme: { extend: { fontFamily: { sans: ['Inter', 'Outfit', 'sans-serif'] } } } }
    </script>
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@400;600;700&family=Outfit:wght@400;600;800&display=swap" rel="stylesheet">
    <style>
        body { background-color: #050505; color: #e0e0e0; min-height: 100vh; }
        .bg-glow { position: fixed; top: -10%; left: -10%; width: 40%; height: 40%; background: rgba(249,115,22,0.05); filter: blur(120px); border-radius: 50%; pointer-events: none; }
    </style>
</head>
<body class="font-sans antialiased">
    <div class="bg-glow"></div>
    <div id="sul-root" class="relative z-10 px-6"></div>
    <script src="SanskritRuntime.js"></script>
</body>
</html>
)HTML";

const char* SUL_RUNTIME_JS = R"JS(
const colorMap = { "रक्तवर्णः": "bg-red-500 text-white", "नीलवर्णः": "bg-blue-600 text-white", "श्वेतवर्णः": "bg-white text-black border border-gray-200", "हरितवर्णः": "bg-green-500 text-white", "पीतवर्णः": "bg-yellow-400 text-black", "कृष्णवर्णः": "bg-black text-white", "None": "" };
const textColorMap = { "रक्तवर्णः": "text-red-500", "नीलवर्णः": "text-blue-600", "श्वेतवर्णः": "text-white", "कृष्णवर्णः": "text-black" };
function createDrishyam(node) {
    const type = node.type.toLowerCase(); let el;
    if (type === "box" || type === "मंजूषा") { el = document.createElement("div"); el.className = `p-6 rounded-2xl shadow-xl flex flex-col gap-4 m-2 transition-all duration-500 ${colorMap[node.color] || "bg-white/10 backdrop-blur-md border border-white/20"}`; }
    else if (type === "button" || type === "बटनम्") { el = document.createElement("button"); el.innerText = node.label; el.className = `px-6 py-3 rounded-xl font-bold transition-all shadow-lg hover:shadow-2xl hover:scale-105 active:scale-95 ${colorMap[node.color] || "bg-indigo-600 text-white"}`; }
    else if (type === "text" || type === "पाठः") { el = document.createElement("p"); el.innerText = node.label; el.className = `text-lg font-medium ${textColorMap[node.color] || "text-gray-200"}`; }
    else if (type === "image" || type === "चित्त्रम्") { el = document.createElement("img"); el.src = node.source || "https://api.placeholder.com/150"; el.className = "rounded-xl object-cover shadow-lg w-full max-w-md h-auto"; }
    else if (type === "input" || type === "प्रविष्टिः") { el = document.createElement("input"); el.placeholder = node.label || "लिखतु..."; el.className = "px-4 py-3 rounded-lg bg-white/5 border border-white/20 text-white focus:outline-none focus:ring-2 focus:ring-blue-500 transition-all"; }
    else if (type === "list" || type === "सूची") { el = document.createElement("div"); el.className = "flex flex-col gap-2 w-full"; }
    else { el = document.createElement("div"); el.innerText = "Unknown: " + node.type; el.className = "text-red-500"; }
    if (node.children) { node.children.forEach(child => { if (type === "list" || type === "सूची") { const w = document.createElement("div"); w.className = "p-4 bg-white/5 rounded-lg border border-white/10 hover:bg-white/10 transition-all cursor-pointer"; w.appendChild(createDrishyam(child)); el.appendChild(w); } else { el.appendChild(createDrishyam(child)); } }); }
    return el;
}
async function initSUL() {
    try { const response = await fetch('darshanam.json'); const data = await response.json(); const root = document.getElementById('sul-root');
        data.forEach(d => { const c = document.createElement("div"); c.id = d.id; c.className = "w-full max-w-4xl mx-auto py-10"; d.elements.forEach(e => { c.appendChild(createDrishyam(e)); }); root.appendChild(c); });
    } catch (err) { console.error("SUL Runtime Error:", err); }
}
window.onload = initSUL;
)JS";

int main(int argc, char* argv[]) {
    bool targetWeb = false;
    bool targetWasm = false;
    std::string filename;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--target" && i + 1 < argc) {
            std::string target = argv[++i];
            if (target == "web") targetWeb = true;
            else if (target == "wasm") targetWasm = true;
        } else if (filename.empty()) {
            filename = arg;
        }
    }

    if (filename.empty()) {
        std::cerr << "Usage: nvc <file.ns> [--target web/wasm]\n";
        std::cerr << "(Press Enter to exit)\n";
        std::cin.get();
        return 1;
    }

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string sourceCode = buffer.str();

    std::string outputFilename;
    try {
        // 1. Lexical Analysis
        Lexer lexer(sourceCode);
        std::vector<Token> tokens = lexer.tokenize();

        // 2. Parsing
        Parser parser(tokens);
        std::unique_ptr<Program> ast = parser.parse();

        // 3. Code Generation
        CodeGen codegen("nava_module");
        
        if (targetWeb) {
            std::string json = codegen.exportAsJSON(ast.get());
            std::cout << "Exporting Darshakam Web Bundle...\n";
            
            // Create web_output directory
            #ifdef _WIN32
                std::system("mkdir web_output");
            #else
                std::system("mkdir -p web_output");
            #endif

            std::ofstream webOut("web_output/darshanam.json");
            webOut << json;
            webOut.close();

            std::cout << "Successfully generated web_output/darshanam.json\n";
            std::cout << "Copying SUL Runtime and Template...\n";
            
            std::ofstream htmlOut("web_output/index.html");
            htmlOut << SUL_WEB_TEMPLATE;
            htmlOut.close();

            std::ofstream jsOut("web_output/SanskritRuntime.js");
            jsOut << SUL_RUNTIME_JS;
            jsOut.close();

            std::cout << "NVC Web Suite Generated in 'web_output/'\n";
            std::cout << "Open web_output/index.html in a browser to view your Sanskrit Website.\n";
            return 0;
        }

        if (targetWasm) {
            codegen.targetWasm = true;
        }

        codegen.generate(ast.get());

        if (targetWasm) {
            outputFilename = filename.substr(0, filename.find_last_of('.')) + ".wasm.o";
            codegen.writeObject(outputFilename);
            std::cout << "Successfully compiled WebAssembly object to " << outputFilename << "\n";
            std::cout << "Use wasm-ld to link this into a final .wasm browser executable.\n";
            return 0; // Skip native linking
        }

        // 4. Output Object File
        outputFilename = filename.substr(0, filename.find_last_of('.')) + ".o";
        codegen.writeObject(outputFilename);

        std::cout << "Successfully compiled to " << outputFilename << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Compilation Error: " << e.what() << "\n";
        return 1;
    }

    // 5. Link Final Executable
    std::string binaryName = filename.substr(0, filename.find_last_of('.'));
    // We assume nvc is run from project root, and the static library is at nvc/build/libnvc_runtime.a
    std::string command = "clang++ -no-pie " + outputFilename + " nvc/build/libnvc_runtime.a -o " + binaryName;
    int linkResult = std::system(command.c_str());

    if (linkResult == 0) {
        std::cout << "Successfully linked executable to " << binaryName << "\n";
        // Remove intermediate .o
        std::remove(outputFilename.c_str());
    } else {
        std::cerr << "Linking failed.\n";
        return 1;
    }

    return 0;
}
