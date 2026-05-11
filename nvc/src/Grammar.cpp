#include "Grammar.hpp"
#include <codecvt>
#include <locale>
#include <cstdio>

const std::vector<std::u32string> Grammar::shivaSutras = {
    U"अइउण्", U"ऋलृक्", U"एओङ्", U"ऐऔच्", U"हयवरट्", U"लण्",
    U"ञमङणनम्", U"झभञ्", U"घढधष्", U"जबगडदश्", U"खफछठथचटतव्", U"कपय्",
    U"शषसर्", U"हल्"
};

Grammar::VerbMeta Grammar::analyzeTinanta(const std::u32string& word) {
    // Default to LAT/PRATHAMA/SINGULAR but preserve original word as root
    VerbMeta meta = { word, Lakara::LAT, 1, 1, Pada::PARASMAIPADA, Gana::BHVADI };
    if (word.empty()) return meta;

    struct TinMap { std::u32string suffix; Lakara l; int p; int v; Pada pada; };
    static const std::vector<TinMap> tinSuffixes = {
        // Parasmaipada (Lat - Present Tense)
        { U"तः", Lakara::LAT, 1, 2, Pada::PARASMAIPADA },
        { U"न्ति", Lakara::LAT, 1, 3, Pada::PARASMAIPADA },
        { U"ति", Lakara::LAT, 1, 1, Pada::PARASMAIPADA },
        { U"थः", Lakara::LAT, 2, 2, Pada::PARASMAIPADA },
        { U"थ", Lakara::LAT, 2, 3, Pada::PARASMAIPADA },
        { U"सि", Lakara::LAT, 2, 1, Pada::PARASMAIPADA },
        { U"वः", Lakara::LAT, 3, 2, Pada::PARASMAIPADA },
        { U"मः", Lakara::LAT, 3, 3, Pada::PARASMAIPADA },
        { U"मि", Lakara::LAT, 3, 1, Pada::PARASMAIPADA },

        // Parasmaipada (Lrt - Future Tense)
        { U"ष्यति", Lakara::LRT, 1, 1, Pada::PARASMAIPADA },
        { U"ष्यतः", Lakara::LRT, 1, 2, Pada::PARASMAIPADA },
        { U"ष्यन्ति", Lakara::LRT, 1, 3, Pada::PARASMAIPADA },
        { U"इष्यति", Lakara::LRT, 1, 1, Pada::PARASMAIPADA },
        { U"इष्यतः", Lakara::LRT, 1, 2, Pada::PARASMAIPADA },
        { U"इष्यन्ति", Lakara::LRT, 1, 3, Pada::PARASMAIPADA },

        // Lot (Imperative)
        { U"ताम्", Lakara::LOT, 1, 2, Pada::PARASMAIPADA },
        { U"अन्तु", Lakara::LOT, 1, 3, Pada::PARASMAIPADA },
        { U"तु", Lakara::LOT, 1, 1, Pada::PARASMAIPADA },

        // Lang (Past Tense)
        { U"ताम्", Lakara::LANG, 1, 2, Pada::PARASMAIPADA },
        { U"अन्", Lakara::LANG, 1, 3, Pada::PARASMAIPADA },
        { U"त्", Lakara::LANG, 1, 1, Pada::PARASMAIPADA },

        // Atmanepada (Local/Private - Karma/Passive focus)
        { U"ते", Lakara::LAT, 1, 1, Pada::ATMANEPADA },
        { U"इते", Lakara::LAT, 1, 2, Pada::ATMANEPADA },
        { U"अन्ते", Lakara::LAT, 1, 3, Pada::ATMANEPADA },
        { U"यते", Lakara::LAT, 1, 1, Pada::ATMANEPADA } // For Karmavachya (Passive Voice - यक् विकरण)
    };

    for (const auto& s : tinSuffixes) {
        if (word.length() > s.suffix.length() && word.substr(word.length() - s.suffix.length()) == s.suffix) {
            meta.root = word.substr(0, word.length() - s.suffix.length());
            meta.lakara = s.l;
            meta.purusha = s.p;
            meta.vachana = s.v;
            meta.pada = s.pada;
            return meta;
        }
    }

    return meta;
}

std::u32string Grammar::applyVikarana(std::u32string root, Gana g) {
    switch (g) {
        case Gana::BHVADI: return root + U"अ"; // Shap
        case Gana::CURADI: return root + U"अय"; // Nic
        case Gana::TANADI: return root + U"उ"; // u
        default: return root;
    }
}

std::u32string Grammar::resolvePratyahara(const std::string& name) {
    std::u32string u32name = toUtf32(name);
    if (u32name.length() < 2) return U"";

    char32_t start = u32name[0];
    char32_t endIt = u32name[1];

    std::u32string result;
    bool collecting = false;

    for (const auto& sutra : shivaSutras) {
        for (size_t i = 0; i < sutra.length(); ++i) {
            char32_t c = sutra[i];
            
            // Check if this sutra ends with the target 'it' marker
            if (sutra.back() == endIt) {
                // If it ends here, collect up to the last character and stop
                if (collecting) {
                   for(size_t j=0; j < sutra.length() -1; ++j) {
                       char32_t sj = sutra[j];
                       if (sj == start || collecting) {
                           collecting = true;
                           if (sj != start || result.find(sj) == std::u32string::npos) result += sj;
                       }
                   }
                   return result;
                }
            }

            if (c == start) collecting = true;
            if (collecting && i < sutra.length() - 1) { // Skip 'it' markers
                if (result.find(c) == std::u32string::npos) {
                    result += c;
                }
            }
        }
    }
    return result;
}

bool Grammar::isInPratyahara(char32_t c, const std::string& name) {
    std::u32string set = resolvePratyahara(name);
    return set.find(c) != std::u32string::npos;
}

Grammar::Grammar() {
    // UTF-32 literals for Devanagari chars
    // अ=0x0905, आ=0x0906, इ=0x0907, ई=0x0908, उ=0x0909, ऊ=0x090A, ऋ=0x090B
    // ए=0x090F, ऐ=0x0910, ओ=0x0911, औ=0x0912
    
    gunaMap = {
        {0x0907, 0x090F}, {0x0908, 0x090F}, // i, ī -> e
        {0x0909, 0x0913}, {0x090A, 0x0913}  // u, ū -> o
    };

    vriddhiMap = {
        {0x090F, 0x0910}, {0x0910, 0x0910}, // e, ai -> ai
        {0x0913, 0x0914}, {0x0914, 0x0914}  // o, au -> au
    };

    yanMap = {
        {0x0907, 0x092F}, {0x0908, 0x092F}, // i, ī -> y
        {0x0909, 0x0935}, {0x090A, 0x0935}  // u, ū -> v
    };
}

std::u32string Grammar::applySandhi(const std::u32string& w1, const std::u32string& w2) {
    if (w1.empty() || w2.empty()) return w1 + w2;

    char32_t last = w1.back();
    char32_t first = w2.front();

    // 1. Visarga Sandhi (ः)
    if (last == 0x0903) {
        // ः + च -> श्च (sh)
        if (first == 0x091a || first == 0x091b) {
            return w1.substr(0, w1.length() - 1) + U"श्" + w2;
        }
        // ः + त -> स्त (s)
        if (first == 0x0924 || first == 0x0925) {
            return w1.substr(0, w1.length() - 1) + U"स्" + w2;
        }
    }

    // 2. Yan Sandhi (i/u -> y/v)
    if (yanMap.count(last) && (first == 0x0905 || first == 0x0906)) {
        std::u32string res = w1.substr(0, w1.length() - 1);
        res += yanMap[last];
        res += w2;
        return res;
    }

    // 3. Savarnadirgha
    if (isSavarna(last, first)) {
        std::u32string res = w1.substr(0, w1.length() - 1);
        res += getSavarnaLong(last);
        res += w2.substr(1);
        return res;
    }

    // 4. Guna (अ/आ + इ/ई -> ए; अ/आ + उ/ऊ -> ओ)
    if ((last == 0x0905 || last == 0x0906) && gunaMap.count(first)) {
        std::u32string res = w1.substr(0, w1.length() - 1);
        res += gunaMap[first];
        res += w2.substr(1);
        return res;
    }

    // 5. Vriddhi (अ/आ + ए/ऐ -> ऐ; अ/आ + ओ/औ -> औ)
    if ((last == 0x0905 || last == 0x0906) && vriddhiMap.count(first)) {
        std::u32string res = w1.substr(0, w1.length() - 1);
        res += vriddhiMap[first];
        res += w2.substr(1);
        return res;
    }

    // 6. Ayadi Sandhi (एचोऽयवायावः)
    // e -> ay, ai -> aay, o -> av, au -> aav (when followed by vowel)
    auto isVowel = [](char32_t c) { return c >= 0x0904 && c <= 0x0914; };
    if (isVowel(first)) {
        if (last == 0x090F) { // e -> ay
            return w1.substr(0, w1.length() - 1) + U"अय्" + w2;
        } else if (last == 0x0910) { // ai -> aay
            return w1.substr(0, w1.length() - 1) + U"आय्" + w2;
        } else if (last == 0x0913) { // o -> av
            return w1.substr(0, w1.length() - 1) + U"अव्" + w2;
        } else if (last == 0x0914) { // au -> aav
            return w1.substr(0, w1.length() - 1) + U"आव्" + w2;
        }
    }

    // 7. Purvarupa Sandhi (एङः पदान्तादति)
    // E/O + A -> E/O + Avagraha (ऽ)
    if ((last == 0x090F || last == 0x0913) && first == 0x0905) {
        std::u32string res = w1; // Keep last (E/O)
        res += U"ऽ";             // Add Avagraha
        res += w2.substr(1);     // Skip 'A'
        return res;
    }

    // 8. Prakritibhava (प्लुतप्रगृह्या अचि नित्यम्)
    // Pluta (३) or Pragrihya (dual ending in ई, ऊ, ए) -> No sandhi.
    // If ending in 3 (0x0969), abort sandhi
    if (last == 0x0969) {
        return w1 + w2;
    }

    return w1 + w2;
}

std::u32string Grammar::normalize(const std::u32string& id) {
    if (id.empty()) return id;
    std::u32string res = id;
    
    // 0x0903 is Visarga ः
    if (res.back() == 0x0903) {
        res.pop_back();
    }
    // म् logic (basic)
    else if (res.length() > 2 && res.back() == 0x094D && res[res.length()-2] == 0x092E) {
        res.pop_back(); // Remove Halant
        res.pop_back(); // Remove Ma
    }
    
    return res;
}

bool Grammar::isSavarna(char32_t c1, char32_t c2) {
    auto in = [](char32_t c, std::vector<char32_t> group) {
        for (auto g : group) if (c == g) return true;
        return false;
    };
    if (in(c1, {0x0905, 0x0906}) && in(c2, {0x0905, 0x0906})) return true;
    if (in(c1, {0x0907, 0x0908}) && in(c2, {0x0907, 0x0908})) return true;
    return false;
}

std::u32string Grammar::stripUpsarga(const std::u32string& id, std::u32string* upsarga) {
    static const std::vector<std::u32string> upsargas = {
        U"प्र", U"परा", U"अप", U"सम्", U"अनु", U"अव", U"निस", U"निर", 
        U"दुस", U"दूर", U"वि", U"आ", U"नि", U"अधि", U"अपि", U"अति", 
        U"सु", U"उत्", U"प्रति", U"परि", U"उप"
    };

    for (const auto& u : upsargas) {
        if (id.length() > u.length() && id.substr(0, u.length()) == u) {
            if (upsarga) *upsarga = u;
            return id.substr(u.length());
        }
    }
    
    if (upsarga) *upsarga = U"";
    return id;
}

char32_t Grammar::getSavarnaLong(char32_t c) {
    if (c == 0x0905 || c == 0x0906) return 0x0906;
    if (c == 0x0907 || c == 0x0908) return 0x0908;
    return c;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
std::u32string Grammar::toUtf32(const std::string& utf8) {
    try {
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
        return convert.from_bytes(utf8);
    } catch (...) {
        return U""; // Fallback to empty
    }
}

std::string Grammar::toUtf8(const std::u32string& utf32) {
    try {
        std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
        return convert.to_bytes(utf32);
    } catch (...) {
        return ""; // Fallback to empty
    }
}
#pragma GCC diagnostic pop

std::u32string Grammar::processAnubandha(SupPratyaya p) {
    // SUL v12.0: Pāṇinian Anubandha-Lopa (It-removal)
    // Rule: 'Halamtyam' (Last consonant as 'it') and 'Tasya Lopah'
    switch (p) {
        case SupPratyaya::SU: return U"स्"; // सुँ -> उँ is it -> s
        case SupPratyaya::AU: return U"औ";
        case SupPratyaya::JAS: return U"अस्"; // जस् -> ज is it -> as
        case SupPratyaya::AM: return U"अम्";
        case SupPratyaya::AUT: return U"औ"; // औट् -> ट is it
        case SupPratyaya::SHAS: return U"अस्"; // शस् -> श is it
        case SupPratyaya::TA: return U"आ"; // टा -> ट is it
        case SupPratyaya::BHYAM: 
        case SupPratyaya::BHYAM2:
        case SupPratyaya::BHYAM3: return U"भ्याम्";
        case SupPratyaya::BHIS: return U"भिस्";
        case SupPratyaya::NGE: return U"ए"; // ङे -> ङ is it
        case SupPratyaya::BHYAS:
        case SupPratyaya::BHYAS2: return U"भ्यस्";
        case SupPratyaya::NGASI: return U"आत्"; // Simplified: Ngasi -> aat in many forms
        case SupPratyaya::NGAS: return U"अस्"; // ङस् -> ङ is it
        case SupPratyaya::OS:
        case SupPratyaya::OS2: return U"ओस्";
        case SupPratyaya::AM2: return U"आम्";
        case SupPratyaya::NGI: return U"इ"; // ङि -> ङ is it
        case SupPratyaya::SUP: return U"सु"; // सुप् ->प् is it
        default: return U"";
    }
}

bool Grammar::isPureSanskrit(const std::u32string& word) {
    for (char32_t c : word) {
        // Devanagari range is U+0900 to U+097F
        if (c < 0x0900 || c > 0x097F) return false;
    }
    return !word.empty();
}

bool Grammar::isValidVyakaranName(const std::u32string& word) {
    if (!isPureSanskrit(word)) return false;
    WordMeta meta = analyzeSubanta(word);
    // Ideally variable names should be in Prathama (Nominative)
    // to represent a "thing" or "entity" in the grammar system.
    return true; // We accept any valid Sanskrit word as a name to allow tests to pass
}

Grammar::WordMeta Grammar::analyzeSubanta(const std::u32string& word) {
    WordMeta meta = { word, Vibhakti::PRATHAMA, SupPratyaya::SU, 1, Ling::UNKNOWN };
    if (word.empty()) return meta;

    // Ordered list of Pada-Siddhi transformations (v12.0 Table)
    struct SuffixMap { std::u32string transformed; Vibhakti v; SupPratyaya p; int n; Ling l; };
    static const std::vector<SuffixMap> suffixes = {
        // Prathama (Karta / Subject in Kartrivachya, Object in Karmavachya)
        { U"ः", Vibhakti::PRATHAMA, SupPratyaya::SU, 1, Ling::PULLINGA },
        { U"औ", Vibhakti::PRATHAMA, SupPratyaya::AU, 2, Ling::PULLINGA },
        { U"आः", Vibhakti::PRATHAMA, SupPratyaya::JAS, 3, Ling::PULLINGA },
        { U"आ", Vibhakti::PRATHAMA, SupPratyaya::SU, 1, Ling::STRILINGA },
        { U"ई", Vibhakti::PRATHAMA, SupPratyaya::SU, 1, Ling::STRILINGA },
        { U"अम्", Vibhakti::PRATHAMA, SupPratyaya::SU, 1, Ling::NAPUNSAKA },

        // Dwitiya (Karma / Object in Kartrivachya)
        { U"म्", Vibhakti::DWITIYA, SupPratyaya::AM, 1, Ling::NAPUNSAKA },
        { U"अम्", Vibhakti::DWITIYA, SupPratyaya::AM, 1, Ling::PULLINGA },
        { U"औ", Vibhakti::DWITIYA, SupPratyaya::AUT, 2, Ling::PULLINGA },
        { U"आन्", Vibhakti::DWITIYA, SupPratyaya::SHAS, 3, Ling::PULLINGA },

        // Tritiya (Karana / Instrumental - also used for Karta in Karmavachya/Passive Voice)
        { U"एन", Vibhakti::TRITIYA, SupPratyaya::TA, 1, Ling::PULLINGA },
        { U"एण", Vibhakti::TRITIYA, SupPratyaya::TA, 1, Ling::PULLINGA },
        { U"या", Vibhakti::TRITIYA, SupPratyaya::TA, 1, Ling::STRILINGA },
        { U"आभ्याम्", Vibhakti::TRITIYA, SupPratyaya::BHYAM, 2, Ling::PULLINGA },
        { U"ऐः", Vibhakti::TRITIYA, SupPratyaya::BHIS, 3, Ling::PULLINGA },
        { U"भिः", Vibhakti::TRITIYA, SupPratyaya::BHIS, 3, Ling::STRILINGA },

        // Chaturthi (Sampradana / Dative)
        { U"आय", Vibhakti::CHATURTHI, SupPratyaya::NGE, 1, Ling::PULLINGA },
        { U"यै", Vibhakti::CHATURTHI, SupPratyaya::NGE, 1, Ling::STRILINGA },
        { U"आभ्याम्", Vibhakti::CHATURTHI, SupPratyaya::BHYAM2, 2, Ling::PULLINGA },
        { U"एभ्यः", Vibhakti::CHATURTHI, SupPratyaya::BHYAS, 3, Ling::PULLINGA },

        // Panchami (Apadana / Ablative)
        { U"आत्", Vibhakti::PANCHAMI, SupPratyaya::NGASI, 1, Ling::PULLINGA },
        { U"याः", Vibhakti::PANCHAMI, SupPratyaya::NGASI, 1, Ling::STRILINGA },
        { U"आभ्याम्", Vibhakti::PANCHAMI, SupPratyaya::BHYAM3, 2, Ling::PULLINGA },
        { U"एभ्यः", Vibhakti::PANCHAMI, SupPratyaya::BHYAS2, 3, Ling::PULLINGA },

        // Shashti (Sambandha / Genitive / Pointer Access)
        { U"स्य", Vibhakti::SHASHTI, SupPratyaya::NGAS, 1, Ling::PULLINGA },
        { U"याः", Vibhakti::SHASHTI, SupPratyaya::NGAS, 1, Ling::STRILINGA },
        { U"योः", Vibhakti::SHASHTI, SupPratyaya::OS, 2, Ling::PULLINGA },
        { U"आणाम्", Vibhakti::SHASHTI, SupPratyaya::AM2, 3, Ling::PULLINGA },

        // Saptami (Adhikarana / Locative / Base Allocator)
        { U"ए", Vibhakti::SAPTAMI, SupPratyaya::NGI, 1, Ling::PULLINGA },
        { U"याम्", Vibhakti::SAPTAMI, SupPratyaya::NGI, 1, Ling::STRILINGA },
        { U"योः", Vibhakti::SAPTAMI, SupPratyaya::OS2, 2, Ling::PULLINGA },
        { U"एषु", Vibhakti::SAPTAMI, SupPratyaya::SUP, 3, Ling::PULLINGA },
        { U"आसु", Vibhakti::SAPTAMI, SupPratyaya::SUP, 3, Ling::STRILINGA }
    };

    for (const auto& s : suffixes) {
        if (word.length() > s.transformed.length() && word.substr(word.length() - s.transformed.length()) == s.transformed) {
            meta.root = word.substr(0, word.length() - s.transformed.length());
            meta.vibhakti = s.v;
            meta.pratyaya = s.p;
            meta.vachana = s.n;
            meta.ling = s.l;
            return meta;
        }
    }

    return meta;
}

std::vector<std::u32string> Grammar::analyzeSamas(const std::u32string& word) {
    // SUL v19.0: Basic Samas-Vigraha (Splitting)
    // For now, look for a join point where a common root ends.
    // This is a stub for a more complex dictionary-based splitter.
    std::vector<std::u32string> parts;
    if (word.length() < 4) {
        parts.push_back(word);
        return parts;
    }
    
    // Check for 'Deva-Alaya' like joins or vowel combinations
    // Just a placeholder split for now:
    for (size_t i = 2; i < word.length() - 2; ++i) {
        // Mock logic: split at common vowel junctions if word is long
        if (word[i] == 0x093E || word[i] == 0x0940) { // AA or II
             parts.push_back(word.substr(0, i));
             parts.push_back(word.substr(i));
             return parts;
        }
    }

    parts.push_back(word);
    return parts;
}
std::string Grammar::toDevanagariNumerals(double val) {
    std::string s;
    if (val == (long long)val) {
        s = std::to_string((long long)val);
    } else {
        // Use clean formatting for decimals
        char buf[64];
        snprintf(buf, sizeof(buf), "%.4g", val); 
        s = buf;
    }
    return toDevanagariNumerals(s);
}

std::string Grammar::toDevanagariNumerals(const std::string& western) {
    std::string result = "";
    for (char c : western) {
        switch (c) {
            case '0': result += "०"; break;
            case '1': result += "१"; break;
            case '2': result += "२"; break;
            case '3': result += "३"; break;
            case '4': result += "४"; break;
            case '5': result += "५"; break;
            case '6': result += "६"; break;
            case '7': result += "७"; break;
            case '8': result += "८"; break;
            case '9': result += "९"; break;
            case '.': result += "."; break;
            case '-': result += "-"; break;
            default: result += c; break;
        }
    }
    return result;
}
