'use client';

import React, { useState, useEffect } from 'react';
import { motion } from 'framer-motion';
import { loadNavaCompiler } from '@/lib/wasm-loader';
import { SanskritRenderer, DrishyamNode } from '@/components/SanskritRenderer';
import { 
  Play, 
  Terminal, 
  Code2, 
  Zap, 
  CloudLightning,
  ChevronRight,
  Info,
  Eye,
  Layout
} from 'lucide-react';

export default function SanskritPlayground() {
  const [code, setCode] = useState('दृश्यम्.मंजूषा({\n  प्रकारः: "मंजूषा",\n  वर्णः: "नीलवर्णः",\n  children: [\n    { प्रकारः: "पाठः", label: "🚩 नमस्ते नव्या-दृश्यम्!", वर्णः: "श्वेतवर्णः" },\n    { प्रकारः: "बटनम्", label: "अत्र क्लिकं कुर्वन्तु", वर्णः: "रक्तवर्णः" }\n  ]\n});');
  const [output, setOutput] = useState('सङ्कलनस्य प्रतीक्षायां...');
  const [visualData, setVisualData] = useState<DrishyamNode | null>(null);
  const [viewMode, setViewMode] = useState<'console' | 'preview'>('console');
  const [compiler, setCompiler] = useState<any>(null);
  const [isCompiling, setIsCompiling] = useState(false);

  useEffect(() => {
    loadNavaCompiler().then(instance => {
      setCompiler(instance);
      setOutput('🚩 नव्या-सङ्कलकः सिद्धः! (Compiler Ready)');
    }).catch(err => {
      setOutput('त्रुटिः: सङ्कलक-भारणम् असफलम्। (' + err.message + ')');
    });
  }, []);

  const runCode = async () => {
    if (!compiler) return;
    setIsCompiling(true);
    try {
      const result = compiler.compile(code);
      setOutput(result);
      
      // Try to detect if it's UI JSON
      if (result.trim().startsWith('{')) {
        try {
          const parsed = JSON.parse(result);
          setVisualData(parsed);
          setViewMode('preview'); // Auto-switch to preview if it's UI
        } catch (e) {
          setVisualData(null);
        }
      } else {
        setVisualData(null);
        setViewMode('console');
      }
    } catch (err: any) {
      setOutput('त्रुटिः: ' + err.message);
    } finally {
      setIsCompiling(false);
    }
  };

  return (
    <div className="min-h-screen bg-[#020617] text-[#f8fafc] p-6 selection:bg-amber-500/30">
        <div className="max-w-7xl mx-auto flex flex-col gap-8">
            {/* Header */}
            <header className="flex justify-between items-center">
                <div className="flex items-center gap-4">
                    <div className="w-12 h-12 bg-amber-500 rounded-xl flex items-center justify-center text-2xl font-bold text-black shadow-lg shadow-amber-500/20">न</div>
                    <div>
                        <h1 className="text-2xl font-bold tracking-wide">नव्या-क्रीडाङ्गणम्</h1>
                        <div className="flex items-center gap-2">
                             <p className="text-slate-500 text-sm">Nava Web IDE (Pure WASM)</p>
                             <span className="px-2 py-0.5 rounded-full bg-emerald-500/10 border border-emerald-500/20 text-[8px] text-emerald-400 font-bold uppercase tracking-widest">Global Edge Ready</span>
                        </div>
                    </div>
                </div>
                <div className="flex items-center gap-3">
                    <button 
                        onClick={runCode}
                        disabled={!compiler || isCompiling}
                        className="bg-amber-500 hover:bg-amber-600 disabled:opacity-50 text-black px-8 py-2.5 rounded-full font-bold transition-all transform hover:-translate-y-0.5 active:scale-95 shadow-xl shadow-amber-500/20 flex items-center gap-2"
                    >
                        {isCompiling ? <CloudLightning className="animate-bounce w-4 h-4" /> : <Play className="w-4 h-4 fill-current" />}
                        चालनम् (RUN)
                    </button>
                </div>
            </header>

            {/* Grid */}
            <div className="grid grid-cols-1 lg:grid-cols-12 gap-6">
                {/* Editor Container */}
                <div className="lg:col-span-7 flex flex-col gap-4">
                    <div className="flex-1 min-h-[500px] rounded-2xl border border-white/10 bg-slate-900/40 backdrop-blur-xl overflow-hidden flex flex-col">
                        <div className="h-12 px-5 border-b border-white/5 flex items-center justify-between bg-white/[0.02]">
                            <div className="flex items-center gap-2">
                                <Code2 className="w-4 h-4 text-amber-500" />
                                <span className="text-xs font-bold text-slate-500 uppercase tracking-widest leading-none">playground.ns</span>
                            </div>
                            <div className="flex gap-1.5">
                                <div className="w-2.5 h-2.5 rounded-full bg-red-500/20 border border-red-500/30" />
                                <div className="w-2.5 h-2.5 rounded-full bg-amber-500/20 border border-amber-500/30" />
                                <div className="w-2.5 h-2.5 rounded-full bg-emerald-500/20 border border-emerald-500/30" />
                            </div>
                        </div>
                        <textarea
                            value={code}
                            onChange={(e) => setCode(e.target.value)}
                            spellCheck={false}
                            className="flex-1 bg-transparent p-6 font-mono text-base leading-relaxed focus:outline-none resize-none placeholder-slate-700"
                        />
                    </div>
                </div>

                {/* Output & Sidebar */}
                <div className="lg:col-span-5 flex flex-col gap-6">
                    {/* Console / Preview Card */}
                    <div className="flex-1 min-h-[400px] rounded-2xl border border-white/10 bg-slate-950 flex flex-col shadow-2xl relative">
                        <div className="h-12 px-4 border-b border-white/5 flex items-center justify-between bg-white/[0.02]">
                            <div className="flex gap-4 h-full">
                                <button 
                                    onClick={() => setViewMode('console')}
                                    className={`flex items-center gap-2 px-3 border-b-2 transition-all ${viewMode === 'console' ? 'border-amber-500 text-amber-500' : 'border-transparent text-slate-500'}`}
                                >
                                    <Terminal className="w-3.5 h-3.5" />
                                    <span className="text-[10px] uppercase tracking-widest font-bold">निर्गम-पटलम्</span>
                                </button>
                                <button 
                                    onClick={() => setViewMode('preview')}
                                    disabled={!visualData}
                                    className={`flex items-center gap-2 px-3 border-b-2 transition-all ${viewMode === 'preview' ? 'border-amber-500 text-amber-500' : 'border-transparent text-slate-500'} disabled:opacity-30`}
                                >
                                    <Eye className="w-3.5 h-3.5" />
                                    <span className="text-[10px] uppercase tracking-widest font-bold">दृश्य-दर्शनम्</span>
                                </button>
                            </div>
                            {visualData && (
                                <div className="text-[8px] text-amber-500/50 font-bold uppercase tracking-widest flex items-center gap-1">
                                    <div className="w-1 h-1 rounded-full bg-amber-500 animate-pulse" /> UI Detected
                                </div>
                            )}
                        </div>
                        
                        <div className="flex-1 p-6 overflow-auto">
                            {viewMode === 'console' ? (
                                <pre className="font-mono text-emerald-400 text-sm whitespace-pre-wrap leading-relaxed">
                                    {output}
                                </pre>
                            ) : (
                                <div className="w-full h-full flex items-center justify-center bg-slate-900/10 rounded-xl border border-white/5 p-4">
                                    {visualData && <SanskritRenderer node={visualData} />}
                                </div>
                            )}
                        </div>
                    </div>

                    {/* Info Card */}
                    <div className="p-6 rounded-2xl border border-amber-500/10 bg-amber-500/[0.03] space-y-4">
                        <h3 className="text-amber-500 font-bold flex items-center gap-2 text-sm">
                            <Zap className="w-4 h-4" /> नव्या-भाषा-शक्तिः
                        </h3>
                        <p className="text-slate-400 text-xs leading-relaxed">
                            इयं वेबसाइट् रीयल-टाइम WebAssembly (WASM) सङ्कलकस्य उपयोगं करोति। अतः भवतः कोडः स्थानीयस्तरे एव सङ्कलितः भवति, न तु सर्वर्-मन्चे।
                        </p>
                        <div className="pt-2">
                             <button className="text-[10px] font-bold text-slate-500 uppercase tracking-widest flex items-center gap-1 hover:text-amber-500 transition-colors">
                                अधिकं पश्यतु (Read Docs) <ChevronRight className="w-3 h-3" />
                             </button>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </div>
  );
}
