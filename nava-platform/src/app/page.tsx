'use client';

import React, { useState, useEffect } from 'react';
import { motion, AnimatePresence } from 'framer-motion';
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
  const [code, setCode] = useState('नित्य संख्याः = १०;\nअस्ति ख = ५;\n\nविधिः योग(क, ख) {\n    फलम् क + ख;\n}\n\nवद(योग(संख्याः, ख));');
  const [output, setOutput] = useState('');
  const [isCompiling, setIsCompiling] = useState(false);
  const [activeTab, setActiveTab] = useState('editor');

  const handleCompile = async () => {
    setIsCompiling(true);
    // Simulate WASM compilation
    setTimeout(() => {
      setIsCompiling(false);
      setOutput('NVC (Nava LLVM Compiler) v4.0.0\n--------------------------------\nTarget: x86_64-native-binary\nOptimization: -O3\n\n[Output]: 15.000000\n\nCompilation successful in 42ms.');
    }, 800);
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
              {isCompiling ? 'Compiling...' : 'Run Native'}
            </button>
          </div>
        </div>
      </nav>

      <main className="max-w-screen-2xl mx-auto p-6 grid grid-cols-12 gap-6 h-[calc(100vh-80px)]">
        {/* Editor Section */}
        <section className="col-span-12 lg:col-span-8 flex flex-col gap-4">
          <div className="flex-1 flex flex-col rounded-2xl border border-white/10 bg-white/[0.02] backdrop-blur-lg overflow-hidden group hover:border-white/20 transition-all">
            <div className="h-12 px-5 border-b border-white/10 flex items-center justify-between bg-white/[0.02]">
              <div className="flex items-center gap-2">
                <Code2 className="w-4 h-4 text-orange-500" />
                <span className="text-sm font-semibold text-white/70">playground.ns</span>
              </div>
              <div className="flex items-center gap-4">
                 <div className="flex items-center gap-2 text-[10px] font-mono text-white/30 uppercase">
                    <div className="w-1.5 h-1.5 rounded-full bg-green-500 animate-pulse" />
                    Native Linker Active
                 </div>
              </div>
            </div>
            <textarea
              value={code}
              onChange={(e) => setCode(e.target.value)}
              spellCheck={false}
              className="flex-1 w-full bg-transparent p-6 font-mono text-base leading-relaxed focus:outline-none resize-none placeholder-white/20"
            />
          </div>
        </section>

        {/* Output Section */}
        <section className="col-span-12 lg:col-span-4 flex flex-col gap-4">
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
               <motion.div 
                 initial={{ scale: 0.8, opacity: 0 }}
                 animate={{ scale: 1, opacity: 1 }}
                 className="w-24 h-24 rounded-full bg-gradient-to-tr from-orange-500/20 to-orange-500/0 border-2 border-orange-500/50 flex items-center justify-center relative"
               >
                  <div className="w-12 h-12 rounded-full border border-orange-500/80 animate-ping absolute" />
                  <div className="w-4 h-4 bg-orange-500 rounded-full shadow-[0_0_20px_rgba(249,115,22,0.6)]" />
               </motion.div>
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
              {output || 'Ready for native compilation...'}
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
