<div align="center">
  <h1>🕉️ Nava Sanskritam (नवसंस्कृतम्)</h1>
  <p><b>A highly performant, parallel-execution programming language powered by LLVM and Paninian Grammar rules.</b></p>
  <img src="https://img.shields.io/badge/Architecture-LLVM%2FC%2B%2B20-blue.svg" alt="LLVM/C++20"/>
  <img src="https://img.shields.io/badge/IDE-NavaStudio%20(Electron)-blueviolet.svg" alt="NavaStudio"/>
  <img src="https://img.shields.io/badge/Build-GitHub%20Actions%20CI%2FCD-success.svg" alt="GitHub Actions"/>
</div>

---

## 🪷 Vedic Vision
At **NavaSanganakah Multiventures**, we blend the ancient intelligence of Vedic sciences with modern high-performance computation. Panini's grammatical rules in the *Ashtadhyayi* lay out an algorithmic framework that predates modern computer science by millennia. 

Nava Sanskritam (SUL) replaces standard sequential paradigms with concurrent, lakara-driven (tense-driven) async execution, vibhakti-based memory models, and native hardware compilation via **LLVM**.

## 🚀 The Nava Ecosystem

The modern Nava Ecosystem consists of two major components:
1. **NVC (Nava Compiler)**: A robust C++20 compiler leveraging LLVM infrastructure for hyper-fast native binary generation.
2. **Nava Studio**: A premium, Electron-based desktop IDE providing an immersive coding environment, integrated Monaco editor, and one-click compilation.

## 📦 Installation & Setup

We use an automated CI/CD pipeline via GitHub Actions to deliver zero-configuration installers directly to developers.

### Windows (Setup.exe)
1. Navigate to the **Actions** tab in this repository.
2. Select the latest successful build under `Build Nava Studio (Desktop Installer)`.
3. Download the `nava-studio-installer` artifact (`.exe`).
4. Run the installer to enjoy a fully managed Nava environment!

### Compiling from Source (Developers)
NVC requires `CMake` and `LLVM 18+`. On Windows, we highly recommend using **MSYS2**.

```bash
# Clone repository
git clone https://github.com/navasanganakah/ns-language-own.git
cd ns-language-own/nvc

# Configure & Build LLVM Compiler
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
```

## ✨ Syntax & Features (v18.0)

Nava Sanskritam introduces groundbreaking programmatic concepts using Sanskrit terminology.

### Native UI Generation (Darshanam)
Build rich Cross-Platform Desktop & Web UIs inherently using the language.

```sanskrit
दर्शनम् मुख्यपटल {
    मंजूषा {
        चित्त्रम् (logo.png);
        सूची (["एकम्", "द्वे", "त्रीणि"]);
        प्रविष्टिः (placeholder="Enter Name");
    }
}
```

### Lakara-Based Synchronization (Lat / Lrt)
Use tenses for implicit memory synchronization.
```sanskrit
# Synchronous Parallel (Bhvadi Gana - Lat Lakara) -> Blocking
Lat Parasmaipada Bhvadi_Execute() {
    वद("Parallel Thread");
}

# Asynchronous Background (Lrt Lakara) -> Non-Blocking
Lrt Atmanepada Task_Background() {
    वद("Async Thread");
}
```

### Math Standard Library (Gaṇita-Saṃhitā)
NVC comes with mathematically pure Vedic functions integrated straight into LLVM intrinsics.
```sanskrit
अस्ति a = त्रैराशिकम्(१०, ५, २); # 10 * 5 / 2
अस्ति b = वर्गमूलम्(१६);       # 4.0
```

## 🛠 Architecture
*   `Lexer.cpp`: Converts raw Devanagari and Latin UTF-8 down to tokens, actively replacing keywords with Paninian properties.
*   `Parser.cpp`: Generates the AST alongside UI Nodes (`DarshanamBlock`).
*   `Grammar.cpp`: The bridge that aligns Panini’s Rules (Sandhi, Tiṅ Pratyayas, Sup Pratyayas) into programmatic data models.
*   `CodeGen.cpp`: Directly emits optimized machine code (Object Files) mapping our AST to LLVM IR using `IRBuilder`.

## 📄 License
Proprietary software of **NavaSanganakah Multiventures**. All Rights Reserved.
