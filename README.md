<div align="center">
  <h1>🕉️ Nava Sanskritam (नवसंस्कृतम्)</h1>
  <p><b>A highly performant, parallel-execution programming language powered by Nava Engine and Paninian Grammar rules.</b></p>
  <img src="https://img.shields.io/badge/Architecture-NavaEngine-blue.svg" alt="NavaEngine"/>
  <img src="https://img.shields.io/badge/IDE-NavaStudio%20(Electron)-blueviolet.svg" alt="NavaStudio"/>
  <img src="https://img.shields.io/badge/Build-GitHub%20Actions%20CI%2FCD-success.svg" alt="GitHub Actions"/>
</div>

---

## 🪷 Vedic Vision
At **NavaSanganakah Multiventures**, we blend the ancient intelligence of Vedic sciences with modern high-performance computation. Panini's grammatical rules in the *Ashtadhyayi* lay out an algorithmic framework that predates modern computer science by millennia. 

> [!TIP]
> **🚩 विस्तृत मार्गदर्शिका (Detailed Guide):** [NAV_SANSKRITAM_GUIDE.md](file:///c:/Users/DHEERENDRA/Desktop/ns-language/NAV_SANSKRITAM_GUIDE.md) पर भाषा के सभी नियमों और घटकों (Components) की पूरी जानकारी प्राप्त करें।

Nava Sanskritam (SUL) replaces standard sequential paradigms with concurrent, lakara-driven (tense-driven) async execution, vibhakti-based memory models, and native hardware compilation via **Nava Engine**.

## 🚀 The Nava Ecosystem

The modern Nava Ecosystem consists of two major components:
1. **NVC (Nava Compiler)**: A robust Nava Engine compiler leveraging Nava Engine infrastructure for hyper-fast native binary generation.
2. **Nava Studio**: A premium, Electron-based desktop IDE providing an immersive coding environment, integrated Monaco editor, and one-click compilation.

## 📦 Installation & Setup

We use an automated CI/CD pipeline via GitHub Actions to deliver zero-configuration installers directly to developers.

### Windows (Setup.exe)
1. Navigate to the **Actions** tab in this repository.
2. Select the latest successful build under `Build Nava Studio (Desktop Installer)`.
3. Download the `nava-studio-installer` artifact (`.exe`).
4. Run the installer to enjoy a fully managed Nava environment!

### Compiling from Source (Developers)
NVC requires `CMake` and `Nava Engine 18+`. On Windows, we highly recommend using **MSYS2**.

```bash
# Clone repository
git clone https://github.com/navasanganakah/ns-language-own.git
cd ns-language-own/nvc

# Configure & Build Nava Engine Compiler
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
```

## ✨ Syntax & Features (v18.0)

Nava Sanskritam introduces groundbreaking programmatic concepts using संस्कृत शब्दावली (Sanskrit terminology).

### 📝 Core Keywords (मुख्य शब्दावली)
- **`अस्ति` (Asti)**: Used to define a variable or assign data to memory. (चर निर्माण)
- **`नित्य` (Nitya)**: Used to define a constant variable whose value never changes. (स्थिर चर)
- **`वद` (Vad)**: Print or output to the console. (प्रिंट कमाण्ड)
- **`यदि / तर्हि / अन्यथा` (Yadi / Tarhi / Anyatha)**: If / Then / Else condition logic. (शर्त)
- **`विधिः` (Vidhih)**: Function declaration. (फंक्शन)
- **`फलम्` (Phalam)**: Return statement. (परिणाम)

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

#### 🎨 UI Components (दृश्यम् शब्दावली)
- **`दर्शनम्` (Darshanam)**: The main view or screen container.
- **`मंजूषा` (Manjusha)**: A structural box or container (like a div).
- **`दृश्यम् पाठः` (Pathah)**: Text component.
- **`दृश्यम् बटनम्` (Buttonam)**: Interactive button.
- **`प्रविष्टिः` (Pravishtih)**: User input field.
- **`चित्त्रम्` (Chittram)**: Image element.
- **`सूची` (Suchi)**: List or array representation.

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
NVC comes with mathematically pure Vedic functions integrated straight into Nava Engine intrinsics.
```sanskrit
अस्ति a = त्रैराशिकम्(१०, ५, २); # 10 * 5 / 2
अस्ति b = वर्गमूलम्(१६);       # 4.0
```

## 🛠 Architecture
*   `Lexer.cpp`: Converts raw Devanagari and Latin UTF-8 down to tokens, actively replacing keywords with Paninian properties.
*   `Parser.cpp`: Generates the AST alongside UI Nodes (`DarshanamBlock`).
*   `Grammar.cpp`: The bridge that aligns Panini’s Rules (Sandhi, Tiṅ Pratyayas, Sup Pratyayas) into programmatic data models.
*   `CodeGen.cpp`: Directly emits optimized machine code (Object Files) mapping our AST to Nava Engine IR using `IRBuilder`.

## 📄 License
Proprietary software of **NavaSanganakah Multiventures**. All Rights Reserved.

---

## 🙏 रचयिता (The Creator)

नव संस्कृतम् भाषा का निर्माण **आचार्य पण्डित धीरेन्द्र त्रिपाठी** जी द्वारा किया गया है, जो **NavaSanganakah Multiventures Company** के संस्थापक हैं। उनका उद्देश्य प्राचीन वैदिक ज्ञान और आधुनिक तकनीकी क्षमताओं का समन्वय कर एक विश्वस्तरीय प्लेटफार्म प्रदान करना है।
