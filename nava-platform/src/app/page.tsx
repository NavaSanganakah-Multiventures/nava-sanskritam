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
  const [code, setCode] = useState('विधिः योगः(क, ख) {\n    फलम् क + ख;\n}\n\nवद(योगः(१०, ५));');
  const [output, setOutput] = useState('');
  const [isCompiling, setIsCompiling] = useState(false);

  const handleCompile = async () => {
    setIsCompiling(true);
    try {
      const response = await fetch('/api/compile', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ code })
      });
      const data = await response.json();
      if (data.success) {
        setOutput(data.output);
      } else {
        setOutput('त्रुटिः: ' + (data.error || 'अज्ञात-त्रुटिः'));
      }
    } catch (err) {
      setOutput('सञ्जाल-त्रुटिः: नेटवर्क-समस्या अस्ति।');
    } finally {
      setIsCompiling(false);
    }
  };

  return (
    <div className="min-h-screen bg-[#020617] text-[#f8fafc] selection:bg-orange-500/30 font-sans">
      {/* Background Glow */}
      <div className="fixed inset-0 overflow-hidden pointer-events-none">
        <div className="absolute top-[-10%] left-[-10%] w-[40%] h-[40%] bg-orange-500/10 blur-[120px] rounded-full" />
      </div>

      {/* Navbar */}
      <nav className="sticky top-0 z-50 border-b border-white/5 bg-slate-900/40 backdrop-blur-xl">
        <div className="max-w-screen-2xl mx-auto px-6 h-16 flex items-center justify-between">
          <div className="flex items-center gap-4">
            <div className="w-10 h-10 bg-amber-500 rounded-xl flex items-center justify-center shadow-lg shadow-amber-500/20">
              <span className="text-xl font-bold text-black">न</span>
            </div>
            <div>
              <h1 className="text-xl font-bold tracking-tight">
                नव्या-मञ्चः <span className="text-amber-500">व्यासः</span>
              </h1>
              <p className="text-[10px] uppercase tracking-[0.2em] text-slate-500 font-medium">Nava Platform v2.5</p>
            </div>
          </div>

          <div className="flex items-center gap-3">
            <a 
              href="/playground"
              className="h-10 px-4 rounded-full bg-white/5 border border-white/10 hover:bg-white/10 transition-colors flex items-center gap-2 text-sm font-medium"
            >
              <Globe className="w-4 h-4" /> क्रीडाङ्गणम्
            </a>
            <button
              onClick={handleCompile}
              disabled={isCompiling}
              className="h-10 px-6 rounded-full bg-amber-500 hover:bg-amber-600 transition-all flex items-center gap-2 text-sm font-bold text-black shadow-lg shadow-amber-500/20 active:scale-95 disabled:opacity-50"
            >
              {isCompiling ? <CloudLightning className="w-4 h-4 animate-bounce" /> : <Play className="w-4 h-4 fill-current" />}
              {isCompiling ? 'सङ्कलनम्...' : 'चालनम्'}
            </button>
          </div>
        </div>
      </nav>

      <main className="max-w-screen-2xl mx-auto p-6 grid grid-cols-12 gap-6 h-[calc(100vh-80px)]">
        {/* Editor */}
        <section className="col-span-12 lg:col-span-8 flex flex-col gap-4">
          <div className="flex-1 flex flex-col rounded-2xl border border-white/10 bg-slate-900/20 backdrop-blur-lg overflow-hidden group">
            <div className="h-12 px-5 border-b border-white/10 flex items-center justify-between bg-white/[0.02]">
              <div className="flex items-center gap-2">
                <Code2 className="w-4 h-4 text-amber-500" />
                <span className="text-sm font-semibold text-slate-400">सञ्चिका.ns</span>
              </div>
            </div>
            <textarea
              value={code}
              onChange={(e) => setCode(e.target.value)}
              spellCheck={false}
              className="flex-1 w-full bg-transparent p-6 font-mono text-base leading-relaxed focus:outline-none resize-none"
            />
          </div>
        </section>

        {/* Output */}
        <section className="col-span-12 lg:col-span-4 flex flex-col gap-6">
          <div className="flex-1 rounded-2xl border border-white/10 bg-slate-950/50 overflow-hidden flex flex-col shadow-2xl">
            <div className="h-10 px-4 border-b border-white/5 flex items-center bg-white/[0.02]">
              <Terminal className="w-3.5 h-3.5 mr-2 text-slate-500" />
              <span className="text-[10px] uppercase tracking-widest font-bold text-slate-500">निर्गम-पटलम्</span>
            </div>
            <div className="flex-1 p-5 font-mono text-sm overflow-auto text-emerald-400 leading-relaxed whitespace-pre-wrap">
              {output || 'सङ्कलनस्य प्रतीक्षायां...'}
            </div>
          </div>

          <div className="p-5 rounded-2xl border border-amber-500/20 bg-amber-500/5 flex items-start gap-4">
            <div className="p-2 bg-amber-500/20 rounded-lg">
              <Zap className="w-5 h-5 text-amber-500" />
            </div>
            <div>
              <h3 className="text-sm font-bold text-amber-400">मेघ-सङ्गणनम् (Cloud)</h3>
              <p className="text-xs text-slate-500 mt-1 leading-relaxed">
                नव्या-मन्चः अधुना क्लाउड-एज (Cloud Edge) इत्यत्र कार्यं करोति।
              </p>
            </div>
          </div>
        </section>
      </main>
    </div>
  );
}

