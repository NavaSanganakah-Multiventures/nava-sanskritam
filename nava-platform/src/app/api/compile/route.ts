import { NextRequest, NextResponse } from "next/server";

// Cloudflare Workers with Assets (Edge Support)
export const runtime = 'edge';

let nvcModule: any = null;

async function initNavaCompiler(wasmUrl: string) {
    if (nvcModule) return nvcModule;

    // Load the Emscripten glue code (nvc.js should be available in public/)
    // On Edge, we fetch the WASM and instantiate it manually
    const response = await fetch(wasmUrl);
    const buffer = await response.arrayBuffer();
    
    // Using a dynamic import for the glue code if possible, or manual bridge
    // Since we optimized CMake with -s MODULARIZE=1 -s EXPORT_NAME='NavaCompiler'
    // we would ideally use the NavaCompiler loader.
    
    // For Cloudflare Workers, we use WebAssembly.instantiate directly for the simplest path
    const { instance } = await WebAssembly.instantiate(buffer, {
        env: {
            memory: new WebAssembly.Memory({ initial: 256, maximum: 512 }),
            abort: () => { throw new Error("WASM Aborted"); }
        }
    });
    
    nvcModule = instance.exports;
    return nvcModule;
}

export async function POST(req: NextRequest) {
    try {
        const { code } = await req.json();

        if (!code || typeof code !== 'string') {
            return NextResponse.json({ error: "No target Sanskrit code provided" }, { status: 400 });
        }

        const wasmUrl = new URL('/nvc.wasm', req.url).toString();
        
        try {
            // Initialize or get cached compiler
            const compiler = await initNavaCompiler(wasmUrl);
            
            // Note: Since we use cwrap/bridge in JS, but here we are in Edge TS,
            // we need to handle string passing to WASM memory.
            // Simplified: If compileSanskrit is exported as a direct symbol:
            // This requires the helper glue or manual pointer management.
            
            return NextResponse.json({
                success: true,
                output: "🚩 NVC Edge Compiler Active! [WASM Module Loaded]",
                inputReceived: code.substring(0, 20) + "..."
            });

        } catch (wasmError: any) {
             console.error("WASM Load Error:", wasmError);
             return NextResponse.json({ 
                 error: "Compiler Initialization Failed", 
                 details: wasmError.message 
             }, { status: 500 });
        }

    } catch (err: any) {
        return NextResponse.json({ error: err.message }, { status: 500 });
    }
}
