const fs = require('fs');
const path = require('path');
const NavaCompiler = require('./wasm/nvc.js');

async function run() {
    const filePath = process.argv[2];
    if (!filePath) {
        console.error("Usage: node nvc_run.js <file.ns>");
        process.exit(1);
    }

    const code = fs.readFileSync(filePath, 'utf8');
    
    // Load WASM binary into memory for Node.js compatibility
    const wasmPath = path.join(__dirname, 'wasm', 'nvc.wasm');
    const wasmBuffer = fs.readFileSync(wasmPath);

    // Initialize WASM with direct binary
    const nvc = await NavaCompiler({
        wasmBinary: wasmBuffer
    });
    
    // Wrap C++ function - 'string' return type should handle conversion
    const compileSanskrit = nvc.cwrap('compileSanskrit', 'string', ['string']);
    
    console.log("🚩 --- सङ्कलनं प्रारभ्यते (Execution Starting) ---");
    const result = compileSanskrit(code);
    
    // In some cases, Emscripten might return a pointer if cwrap fails or isn't perfect in this environment. 
    // But usually 'string' works. If we got a number, something is wrong. 
    if (typeof result === 'number') {
        console.log("Memory Pointer detected:", result);
        // Fallback for manual string extraction if nvc provides it
        if (nvc.UTF8ToString) {
             console.log(nvc.UTF8ToString(result));
        }
    } else {
        console.log(result);
    }
    console.log("🚩 --- समाप्तम् (Execution Finished) ---");
}

run().catch(err => console.error(err));
