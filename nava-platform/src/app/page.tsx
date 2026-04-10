'use client';

import React, { useState } from 'react';
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
  ChevronRight,
  Package,
  BookOpen,
  Activity,
  Layers,
  ArrowUpRight,
  LayoutGrid
} from 'lucide-react';

export default function NavaDashboard() {
  const [activeTab, setActiveTab] = useState('dashboard');

  const libraries = [
    { name: 'Gaṇita-Saṃhitā', desc: 'Mathematical & Logical Subsystem', version: 'v1.4', type: 'Core' },
    { name: 'Vyakara-Nyaya', desc: 'Sanskrit Grammar Rule Engine', version: 'v2.1', type: 'Logic' },
    { name: 'Drishyam-UI', desc: 'Declarative Visual Engine', version: 'v0.5', type: 'UI' },
    { name: 'Siddhanta-Data', desc: 'Astronomical & Predictive Math', version: 'v0.8', type: 'Science' }
  ];

  return (
    <div className="min-h-screen bg-[#020617] text-[#f8fafc] selection:bg-amber-500/30 font-sans">
      {/* Background Atmosphere */}
      <div className="fixed inset-0 overflow-hidden pointer-events-none">
        <div className="absolute top-[-20%] left-[-10%] w-[60%] h-[60%] bg-indigo-500/10 blur-[150px] rounded-full" />
        <div className="absolute bottom-[-10%] right-[-5%] w-[40%] h-[40%] bg-amber-500/5 blur-[120px] rounded-full" />
      </div>

      {/* Navbar */}
      <nav className="sticky top-0 z-50 border-b border-white/5 bg-slate-900/40 backdrop-blur-xl">
        <div className="max-w-screen-2xl mx-auto px-8 h-16 flex items-center justify-between">
          <div className="flex items-center gap-4">
            <div className="w-10 h-10 bg-amber-500 rounded-xl flex items-center justify-center shadow-lg shadow-amber-500/20">
              <span className="text-xl font-bold text-black">न</span>
            </div>
            <div>
              <h1 className="text-xl font-bold tracking-tight">नव्या-मञ्चः <span className="text-amber-500">व्यासः</span></h1>
              <p className="text-[10px] uppercase tracking-[0.2em] text-slate-500 font-medium">Nava Platform v2.5</p>
            </div>
          </div>
          <div className="flex items-center gap-6">
            <div className="hidden md:flex gap-4 text-xs font-bold text-slate-400 uppercase tracking-widest">
              <button disabled className="hover:text-amber-500 transition-colors">सञ्चिका (Files)</button>
              <button disabled className="hover:text-amber-500 transition-colors">सेटिंग्स (Settings)</button>
            </div>
            <div className="w-px h-6 bg-white/10 mx-2" />
            <div className="flex items-center gap-3">
               <button className="px-4 py-1.5 rounded-lg bg-white/5 border border-white/10 text-[10px] font-bold uppercase tracking-widest text-slate-500 hover:text-white transition-colors">
                  प्रवेशः (Login)
               </button>
               <span className="text-[10px] items-center gap-1.5 px-2 py-1 rounded bg-emerald-500/10 text-emerald-400 border border-emerald-500/20 flex uppercase font-bold tracking-widest tracking-normal">
                  <div className="w-1 h-1 rounded-full bg-emerald-400 animate-pulse" /> Cloud Active
               </span>
               <div className="w-8 h-8 rounded-full bg-white/5 border border-white/10 flex items-center justify-center">
                  <Settings className="w-4 h-4 text-slate-500" />
               </div>
            </div>
          </div>
        </div>
      </nav>

      <main className="max-w-screen-2xl mx-auto p-8 flex flex-col gap-10">
        
        {/* Hero Section */}
        <section className="grid grid-cols-1 lg:grid-cols-12 gap-8">
            <div className="lg:col-span-8 flex flex-col gap-6 p-10 rounded-3xl border border-white/10 bg-gradient-to-br from-slate-900/50 to-transparent relative overflow-hidden group">
                <div className="flex flex-col gap-4 relative z-10">
                    <h2 className="text-4xl font-bold sanskrit-font leading-tight">नव्या सङ्कृतम् क्लाउड-मञ्चे <br /><span className="text-amber-500 underline decoration-amber-500/30 underline-offset-8">स्वागतम्!</span></h2>
                    <p className="text-slate-400 max-w-lg leading-relaxed">आधुनिक-प्रौद्योगिक्या सह सङ्स्कृत-भाषायाः सङ्गतिः। अत्र भवान् सङ्कलकस्य, पुस्तकालयानां च उपयोगं कर्तुं शक्नोति।</p>
                    <div className="flex gap-4 pt-4">
                        <a href="/playground" className="px-8 py-3 bg-white text-black rounded-full font-bold flex items-center gap-2 hover:bg-amber-500 transition-colors active:scale-95">
                            <Play className="w-4 h-4 fill-current" /> साक्षात् क्रीडाङ्गणम्
                        </a>
                        <button className="px-8 py-3 bg-white/5 border border-white/10 rounded-full font-bold flex items-center gap-2 hover:bg-white/10 transition-colors">
                            <BookOpen className="w-4 h-4" /> प्रलेखनम् (Docs)
                        </button>
                    </div>
                </div>
                <div className="absolute right-[-10%] bottom-[-20%] scale-150 rotate-[-12deg] opacity-[0.03] group-hover:opacity-[0.05] transition-opacity duration-1000">
                    <LayoutGrid size={400} />
                </div>
            </div>

            {/* Stats Panel */}
            <div className="lg:col-span-4 flex flex-col gap-6">
                <div className="p-8 rounded-3xl border border-white/10 bg-slate-900/40 backdrop-blur-xl flex flex-col gap-6 shadow-2xl">
                    <h3 className="text-xs font-bold text-slate-500 uppercase tracking-[0.2em] flex items-center justify-between">
                        एज-नेटवर्क-स्थितिः <Activity className="w-4 h-4 text-amber-500" />
                    </h3>
                    <div className="flex flex-col gap-6">
                        <div className="flex items-center justify-between">
                            <div className="flex items-center gap-3">
                                <div className="p-2 bg-indigo-500/10 rounded-lg"><Globe className="w-4 h-4 text-indigo-400" /></div>
                                <span className="text-sm font-medium">Global Nodes</span>
                            </div>
                            <span className="text-lg font-bold">८४</span>
                        </div>
                        <div className="flex items-center justify-between">
                            <div className="flex items-center gap-3">
                                <div className="p-2 bg-amber-500/10 rounded-lg"><Zap className="w-4 h-4 text-amber-400" /></div>
                                <span className="text-sm font-medium">Edge Latency</span>
                            </div>
                            <span className="text-lg font-bold">12ms</span>
                        </div>
                        <div className="flex items-center justify-between">
                            <div className="flex items-center gap-3">
                                <div className="p-2 bg-emerald-500/10 rounded-lg"><Cpu className="w-4 h-4 text-emerald-400" /></div>
                                <span className="text-sm font-medium">WASM Engine</span>
                            </div>
                            <span className="text-lg font-bold">Active</span>
                        </div>
                    </div>
                </div>
            </div>
        </section>

        {/* Library Registry */}
        <section className="flex flex-col gap-6">
            <div className="flex items-center justify-between">
                <div>
                     <h2 className="text-2xl font-bold flex items-center gap-3">
                        <Package className="text-amber-500" /> सङ्ग्रहः (Library Registry)
                    </h2>
                    <p className="text-slate-500 text-xs mt-1">नव्या-भाषा विकासार्थं उपयोगी सामाग्री</p>
                </div>
                <button className="text-amber-500 text-xs font-bold uppercase tracking-widest flex items-center gap-1 hover:underline">
                    All Libraries <ChevronRight className="w-4 h-4" />
                </button>
            </div>

            <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6">
                {libraries.map((lib, i) => (
                    <motion.div 
                        key={lib.name}
                        whileHover={{ y: -5 }}
                        className="p-6 rounded-2xl border border-white/5 bg-slate-900/20 hover:bg-slate-900/40 hover:border-amber-500/20 transition-all cursor-pointer group"
                    >
                        <div className="flex items-center justify-between mb-4">
                            <div className="p-2 bg-white/5 rounded-lg group-hover:bg-amber-500/10 transition-colors">
                                <Layers className="w-5 h-5 text-slate-400 group-hover:text-amber-500 transition-colors" />
                            </div>
                            <span className="text-[10px] font-bold text-slate-600 uppercase tracking-tighter">{lib.type}</span>
                        </div>
                        <h4 className="font-bold mb-1">{lib.name}</h4>
                        <p className="text-xs text-slate-500 leading-relaxed mb-4">{lib.desc}</p>
                        <div className="flex items-center justify-between pt-4 border-t border-white/5">
                            <span className="text-[10px] font-mono text-slate-600">{lib.version}</span>
                            <ArrowUpRight className="w-3 h-3 text-slate-700 group-hover:text-amber-500 transition-colors" />
                        </div>
                    </motion.div>
                ))}
            </div>
        </section>
        
        {/* Footer */}
        <footer className="mt-10 pt-10 border-t border-white/5 flex items-center justify-between text-slate-600 text-[10px] uppercase font-bold tracking-[.3em]">
            <span>Nava Platform (Vyasa Edition)</span>
            <span>© 2026 Navasanganakah</span>
        </footer>
      </main>
    </div>
  );
}
