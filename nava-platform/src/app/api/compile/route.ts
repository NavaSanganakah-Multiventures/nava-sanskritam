import { NextRequest, NextResponse } from "next/server";

// Cloudflare Workers with Assets (Edge Support)
export const runtime = 'edge';

export async function POST(req: NextRequest) {
    try {
        const { code } = await req.json();

        if (!code || typeof code !== 'string') {
            return NextResponse.json({ error: "No target Sanskrit code provided" }, { status: 400 });
        }

        // Native WASM instantiation on Cloudflare Edge Worker
        // In production, the NVC compiler provides `nvc.wasm` which will be available in the public assets directory
        const url = new URL('/nvc.wasm', req.url);
        
        try {
            // Emscripten runtime logic loads its module by fetching the wasm from our own Cloudflare Asset Storage
            const wasmResponse = await fetch(url.toString());
            if (!wasmResponse.ok) {
                // If .wasm doesn't exist yet, we simulate the AST execution for early testing of the Web engine.
                // This will be replaced by the actual Emscripten module initialization as soon as the build finishes.
                return NextResponse.json({
                    success: true,
                    output: "Simulation Mode: API Ready! (Waiting for nvc.wasm bundle compilation via GitHub Actions.)\nInput Code Registered -> " + code.substring(0, 50) + "..."
                });
            }

            // Real compiler integration happens here (Once emcc is ran via GitHub workflow)
            const wasmBuffer = await wasmResponse.arrayBuffer();
            // ... (WASM instantiation logic will be hydrated by nvc.js emscripten glue code)

            return NextResponse.json({
                success: true,
                output: "Compilation API Successfully Hit!"
            });

        } catch (wasmError) {
             console.error(wasmError);
             return NextResponse.json({ error: String(wasmError) }, { status: 500 });
        }

    } catch (err: any) {
        return NextResponse.json({ error: err.message }, { status: 500 });
    }
}
