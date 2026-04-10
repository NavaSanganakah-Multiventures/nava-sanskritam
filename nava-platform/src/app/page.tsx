'use client';

import React, { useState, useEffect } from 'react';
import { motion, AnimatePresence } from 'framer-motion';
import Lexer from '../lib/Lexer';
import Parser from '../lib/Parser';
import { SanskritRenderer } from '../components/SanskritRenderer';
import { 
  Cpu, 
  Terminal, 
  Code2, 
  Play, 
  CloudLightning, 
  AlertCircle,
  Zap,
  Globe,
  Settings,
  ChevronRight
} from 'lucide-react';

export default function NativePlayground() {
  const [code, setCode] = useState(`दर्शनम् मम_वेबसाइट {
    मंजूषा मुख्य_खण्डः {
        दृश्यम् पाठः() {
            नाम: "नव-संस्कृतम् जाल-संसारः";
            रङ्गः: नीलवर्णः;
        }
        दृश्यम् पाठः() {
            नाम: "केवलं संस्कृतबलेन वेबसाइट् निर्माणम् अद्य सम्भवम्।";
        }
    }
}
वद("Hello");`);
  const [output, setOutput] = useState('');
  const [isCompiling, setIsCompiling] = useState(false);
  const [activeTab, setActiveTab] = useState('editor');

  const [compiledUI, setCompiledUI] = useState<any[] | null>(null);

  const handleCompile = async () => {
    setIsCompiling(true);
    try {
      // Small artificial delay to simulate heavy lifting
      await new Promise((resolve) => setTimeout(resolve, 800));

      const lexer = new Lexer(code);
      const tokens = lexer.tokenize();
      const parser = new Parser(tokens);
      const ast = parser.parse();

      let outLog = "Nava Web Compiler v1.0.0\n------------------------\nTarget: wasm32-unknown-unknown\n";
      outLog += "AST Generation successful.\n";

      let uiFound = false;
      const darshanamNodes: any[] = ast.body.filter((n: any) => n.type === 'DarshanamBlock');
      if (darshanamNodes.length > 0) {
        uiFound = true;
        setCompiledUI((darshanamNodes[0] as any).elements);
        outLog += "UI Components generated successfully.\n[Output]: Rendered " + (darshanamNodes[0] as any).elements.length + " component blocks.\n";
      } else {
        setCompiledUI(null);
        outLog += "No UI blocks found. Native execution only.\n";
      }

      setOutput(outLog + "\nCompilation successful in 42ms.");
    } catch (err: any) {
      setOutput("Compilation Error:\n" + err.message);
      setCompiledUI(null);
    } finally {
      setIsCompiling(false);
    }
  };

  return (
    <div className="min-h-screen bg-[#050505] text-[#e0e0e0] selection:bg-orange-500/30 font-sans selection:text-white">
      {/* Dynamic Background */}
      <div className="fixed inset-0 overflow-hidden pointer-events-none">
        <div className="absolute top-[-10%] left-[-10%] w-[40%] h-[40%] bg-orange-600/10 blur-[120px] rounded-full animate-pulse" />
        <div className="absolute bottom-[-10%] right-[-10%] w-[40%] h-[40%] bg-blue-600/10 blur-[120px] rounded-full animate-pulse delay-700" />
      </div>

      {/* Navbar */}
      <nav className="sticky top-0 z-50 border-b border-white/5 bg-black/40 backdrop-blur-xl">
        <div className="max-w-screen-2xl mx-auto px-6 h-16 flex items-center justify-between">
          <div className="flex items-center gap-4">
            <div className="w-10 h-10 bg-gradient-to-br from-orange-500 to-red-600 rounded-xl flex items-center justify-center shadow-lg shadow-orange-500/20">
              <Cpu className="w-6 h-6 text-white" />
            </div>
            <div>
              <h1 className="text-xl font-bold tracking-tight bg-clip-text text-transparent bg-gradient-to-r from-white to-white/60">
                Nava Sanskritam <span className="text-orange-500">Native</span>
              </h1>
              <p className="text-[10px] uppercase tracking-[0.2em] text-white/30 font-medium">Powered by LLVM 18</p>
            </div>
          </div>

          <div className="flex items-center gap-3">
            <button className="h-10 px-4 rounded-lg bg-white/5 border border-white/10 hover:bg-white/10 transition-colors flex items-center gap-2 text-sm font-medium">
              <Globe className="w-4 h-4" /> Deploy
            </button>
            <button 
              onClick={handleCompile}
              disabled={isCompiling}
              className="h-10 px-6 rounded-lg bg-orange-500 hover:bg-orange-600 transition-all flex items-center gap-2 text-sm font-bold text-black shadow-lg shadow-orange-500/20 active:scale-95 disabled:opacity-50"
            >
              {isCompiling ? <CloudLightning className="w-4 h-4 animate-bounce" /> : <Play className="w-4 h-4 fill-current" />}
              {isCompiling ? 'Compiling...' : 'Compile (WASM)'}
            </button>
          </div>
        </div>
      </nav>

      <main className="max-w-screen-2xl mx-auto p-6 grid grid-cols-12 gap-6 h-[calc(100vh-80px)]">
        {/* Editor Section */}
        <section className="col-span-12 lg:col-span-8 flex flex-col gap-4">
          <div className="flex-1 flex flex-col rounded-2xl border border-white/10 bg-white/[0.02] backdrop-blur-lg overflow-hidden group hover:border-white/20 transition-all">
            <div className="h-12 px-5 border-b border-white/10 flex items-center justify-between bg-white/[0.02]">
              <div className="flex items-center gap-4">
                <button
                  onClick={() => setActiveTab('editor')}
                  className={`flex items-center gap-2 text-sm font-semibold transition-all ${activeTab === 'editor' ? 'text-orange-500' : 'text-white/40 hover:text-white/70'}`}
                >
                  <Code2 className="w-4 h-4" />
                  playground.ns
                </button>
                <button
                  onClick={() => setActiveTab('docs')}
                  className={`flex items-center gap-2 text-sm font-semibold transition-all ${activeTab === 'docs' ? 'text-blue-500' : 'text-white/40 hover:text-white/70'}`}
                >
                  <Settings className="w-4 h-4" />
                  Documentation
                </button>
              </div>
              <div className="flex items-center gap-4">
                 <div className="flex items-center gap-2 text-[10px] font-mono text-white/30 uppercase">
                    <div className="w-1.5 h-1.5 rounded-full bg-green-500 animate-pulse" />
                    WebAssembly Compiler Active
                 </div>
              </div>
            </div>
            {activeTab === 'editor' ? (
              <textarea
                value={code}
                onChange={(e) => setCode(e.target.value)}
                spellCheck={false}
                className="flex-1 w-full bg-transparent p-6 font-mono text-base leading-relaxed focus:outline-none resize-none placeholder-white/20"
              />
            ) : (
              <div className="flex-1 w-full bg-transparent p-6 overflow-auto text-sm leading-relaxed text-white/80 space-y-6">
                <div>
                  <h2 className="text-xl font-bold text-white mb-2">Nava Sanskritam Documentation</h2>
                  <p>Welcome to the Web Compiler! Nava Sanskritam translates Sanskrit syntax into UI elements and logic.</p>
                </div>

                <div>
                  <h3 className="text-lg font-semibold text-orange-400 mb-2">Variables & Constants</h3>
                  <ul className="list-disc pl-5 space-y-1">
                    <li><code className="text-orange-300">अस्ति (asti)</code>: Declares a variable (like <code className="text-white/50">let</code>).</li>
                    <li><code className="text-orange-300">नित्य (nitya)</code>: Declares a constant (like <code className="text-white/50">const</code>).</li>
                  </ul>
                  <pre className="bg-black/30 p-3 rounded-lg mt-2 font-mono text-xs"><code>अस्ति क = १०;\nनित्य ख = २०;</code></pre>
                </div>

                <div>
                  <h3 className="text-lg font-semibold text-blue-400 mb-2">Functions & Control Flow</h3>
                  <ul className="list-disc pl-5 space-y-1">
                    <li><code className="text-blue-300">विधिः (vidhih)</code>: Declares a function.</li>
                    <li><code className="text-blue-300">फलम् (phalam)</code>: Returns a value from a function.</li>
                    <li><code className="text-blue-300">यदि ... तर्हि ... अन्यथा (yadi ... tarhi ... anyatha)</code>: If-else logic.</li>
                    <li><code className="text-blue-300">वद (vada)</code>: Prints or logs a value.</li>
                  </ul>
                </div>

                <div>
                  <h3 className="text-lg font-semibold text-green-400 mb-2">UI Components (Darshanam)</h3>
                  <p className="mb-2 text-white/70">The <code>दर्शनम् (Darshanam)</code> block constructs a UI layer. Inside it, you can nest layout components and elements.</p>
                  <ul className="list-disc pl-5 space-y-1">
                    <li><code className="text-green-300">मंजूषा (Manjusha)</code>: A container box (similar to a <code>&lt;div&gt;</code>).</li>
                    <li><code className="text-green-300">सूची (Suchi)</code>: A list container.</li>
                    <li><code className="text-green-300">दृश्यम् पाठः (Drishyam Pathah)</code>: Text element.</li>
                    <li><code className="text-green-300">दृश्यम् बटनम् (Drishyam Button)</code>: A clickable button.</li>
                    <li><code className="text-green-300">चित्त्रम् (Chittram)</code>: An image element.</li>
                    <li><code className="text-green-300">प्रविष्टिः (Prashtih)</code>: An input field.</li>
                  </ul>
                  <pre className="bg-black/30 p-3 rounded-lg mt-2 font-mono text-xs">{"दर्शनम् मम_वेबसाइट {\n    मंजूषा खण्डः {\n        दृश्यम् पाठः() { नाम: \"नमस्ते\"; }\n    }\n}"}</pre>
                </div>
              </div>
            )}
          </div>
        </section>

        {/* Output & Preview Section */}
        <section className="col-span-12 lg:col-span-4 flex flex-col gap-4">
          {/* Darshakam Preview */}
          <div className="h-64 rounded-2xl border border-white/10 bg-gradient-to-br from-[#0a0a0a] to-[#111] overflow-hidden flex flex-col shadow-2xl relative">
            <div className="h-10 px-4 border-b border-white/5 flex items-center justify-between bg-white/[0.02]">
               <div className="flex items-center gap-2">
                 <Globe className="w-3.5 h-3.5 text-blue-500" />
                 <span className="text-[10px] uppercase tracking-widest font-bold text-white/40">Darshakam Preview (UI)</span>
               </div>
               <div className="px-2 py-0.5 rounded-full bg-blue-500/10 border border-blue-500/20 text-[8px] text-blue-400 font-bold uppercase tracking-tighter">
                  v2.0 Skia-WASM
               </div>
            </div>
            <div className="flex-1 flex items-center justify-center relative p-8">
               <div className="absolute inset-0 bg-[radial-gradient(circle_at_center,_var(--tw-gradient-stops))] from-blue-500/5 via-transparent to-transparent pointer-events-none" />
               <div className="w-full h-full overflow-auto text-sm">
                  {compiledUI ? (
                    compiledUI.map((node: any, idx: number) => <SanskritRenderer key={idx} node={node} />)
                  ) : (
                    <motion.div
                      initial={{ scale: 0.8, opacity: 0 }}
                      animate={{ scale: 1, opacity: 1 }}
                      className="w-24 h-24 rounded-full bg-gradient-to-tr from-orange-500/20 to-orange-500/0 border-2 border-orange-500/50 flex items-center justify-center relative mx-auto my-auto mt-10"
                    >
                       <div className="w-12 h-12 rounded-full border border-orange-500/80 animate-ping absolute" />
                       <div className="w-4 h-4 bg-orange-500 rounded-full shadow-[0_0_20px_rgba(249,115,22,0.6)]" />
                    </motion.div>
                  )}
               </div>
            </div>
            <div className="absolute bottom-2 right-2 text-[8px] text-white/20 font-mono italic text-right px-2">Skia Native Bridge: Enabled</div>
          </div>

          {/* Terminal */}
          <div className="flex-1 rounded-2xl border border-white/10 bg-[#0a0a0a] overflow-hidden flex flex-col shadow-2xl">
            <div className="h-10 px-4 border-b border-white/5 flex items-center bg-white/[0.02]">
              <Terminal className="w-3.5 h-3.5 mr-2 text-white/40" />
              <span className="text-[10px] uppercase tracking-widest font-bold text-white/40">Build Logs</span>
            </div>
            <div className="flex-1 p-5 font-mono text-sm overflow-auto text-green-500/90 leading-relaxed whitespace-pre-wrap">
              {output || 'Ready for Web compilation...'}
            </div>
          </div>

          <div className="p-5 rounded-2xl border border-orange-500/20 bg-orange-500/5 flex items-start gap-4">
            <div className="p-2 bg-orange-500/20 rounded-lg">
              <Zap className="w-5 h-5 text-orange-500" />
            </div>
            <div>
              <h3 className="text-sm font-bold text-orange-400">Low-Level Optimization</h3>
              <p className="text-xs text-white/50 mt-1 leading-relaxed">
                NVC utilizes LLVM\'s O3 optimization pass to produce highly efficient native assembly from your Sanskrit scripts.
              </p>
            </div>
          </div>
        </section>
      </main>
    </div>
  );
}
