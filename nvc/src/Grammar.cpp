#include "Grammar.hpp"
#include <codecvt>
#include <locale>

const std::vector<std::u32string> Grammar::shivaSutras = {
    U"अइउण्", U"ऋलृक्", U"एओङ्", U"ऐऔच्", U"हयवरट्", U"लण्",
    U"ञमङणनम्", U"झभञ्", U"घढधष्", U"जबगडदश्", U"खफछठथचटतव्", U"कपय्",
    U"शषसर्", U"हल्"
};

Grammar::VerbMeta Grammar::analyzeTinanta(const std::u32string& word) {
    VerbMeta meta = { word, Lakara::LAT, 1, 1, Pada::PARASMAIPADA, Gana::BHVADI };
    if (word.empty()) return meta;

    struct TinMap { std::u32string suffix; Lakara l; int p; int v; Pada pada; };
    static const std::vector<TinMap> tinSuffixes = {
        // Parasmaipada (Lat - Standard)
        { U"तः", Lakara::LAT, 1, 2, Pada::PARASMAIPADA },
        { U"न्ति", Lakara::LAT, 1, 3, Pada::PARASMAIPADA },
        { U"ति", Lakara::LAT, 1, 1, Pada::PARASMAIPADA },
        // Parasmaipada (Lrt - Async)
        { U"इष्यति", Lakara::LRT, 1, 1, Pada::PARASMAIPADA },
        { U"इष्यतः", Lakara::LRT, 1, 2, Pada::PARASMAIPADA },
        { U"इष्यन्ति", Lakara::LRT, 1, 3, Pada::PARASMAIPADA },
        // Lot (Imperative)
        { U"तु", Lakara::LOT, 1, 1, Pada::PARASMAIPADA },
        // Lang (Past)
        { U"त्", Lakara::LANG, 1, 1, Pada::PARASMAIPADA },
        // Atmanepada (Local/Private)
        { U"ते", Lakara::LAT, 1, 1, Pada::ATMANEPADA }
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

    // 4. Guna
    if ((last == 0x0905 || last == 0x0906) && gunaMap.count(first)) {
        std::u32string res = w1.substr(0, w1.length() - 1);
        res += gunaMap[first];
        res += w2.substr(1);
        return res;
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

std::u32string Grammar::toUtf32(const std::string& utf8) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
    return convert.from_bytes(utf8);
}

std::string Grammar::toUtf8(const std::u32string& utf32) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> convert;
    return convert.to_bytes(utf32);
}

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

Grammar::WordMeta Grammar::analyzeSubanta(const std::u32string& word) {
    WordMeta meta = { word, Vibhakti::PRATHAMA, SupPratyaya::SU, 1 };
    if (word.empty()) return meta;

    // Ordered list of Pada-Siddhi transformations (v12.0 Table)
    struct SuffixMap { std::u32string transformed; Vibhakti v; SupPratyaya p; int n; };
    static const std::vector<SuffixMap> suffixes = {
        // Prathama (Karta)
        { U"ः", Vibhakti::PRATHAMA, SupPratyaya::SU, 1 },
        { U"औ", Vibhakti::PRATHAMA, SupPratyaya::AU, 2 },
        { U"आः", Vibhakti::PRATHAMA, SupPratyaya::JAS, 3 },
        // Dwitiya (Karma)
        { U"म्", Vibhakti::DWITIYA, SupPratyaya::AM, 1 },
        { U"औ", Vibhakti::DWITIYA, SupPratyaya::AUT, 2 },
        { U"आन्", Vibhakti::DWITIYA, SupPratyaya::SHAS, 3 },
        // Tritiya (Karana)
        { U"एण", Vibhakti::TRITIYA, SupPratyaya::TA, 1 },
        { U"आभ्याम्", Vibhakti::TRITIYA, SupPratyaya::BHYAM, 2 },
        { U"ऐः", Vibhakti::TRITIYA, SupPratyaya::BHIS, 3 },
        // Chaturthi (Sampradana)
        { U"आय", Vibhakti::CHATURTHI, SupPratyaya::NGE, 1 },
        { U"आभ्याम्", Vibhakti::CHATURTHI, SupPratyaya::BHYAM2, 2 },
        { U"एभ्यः", Vibhakti::CHATURTHI, SupPratyaya::BHYAS, 3 },
        // Panchami (Apadana)
        { U"आत्", Vibhakti::PANCHAMI, SupPratyaya::NGASI, 1 },
        { U"आभ्याम्", Vibhakti::PANCHAMI, SupPratyaya::BHYAM3, 2 },
        { U"एभ्यः", Vibhakti::PANCHAMI, SupPratyaya::BHYAS2, 3 },
        // Shashti (Pointer Access)
        { U"स्य", Vibhakti::SHASHTI, SupPratyaya::NGAS, 1 },
        { U"योः", Vibhakti::SHASHTI, SupPratyaya::OS, 2 },
        { U"आणाम्", Vibhakti::SHASHTI, SupPratyaya::AM2, 3 },
        // Saptami (Base Allocator)
        { U"ए", Vibhakti::SAPTAMI, SupPratyaya::NGI, 1 },
        { U"योः", Vibhakti::SAPTAMI, SupPratyaya::OS2, 2 },
        { U"एषु", Vibhakti::SAPTAMI, SupPratyaya::SUP, 3 }
    };

    for (const auto& s : suffixes) {
        if (word.length() > s.transformed.length() && word.substr(word.length() - s.transformed.length()) == s.transformed) {
            meta.root = word.substr(0, word.length() - s.transformed.length());
            meta.vibhakti = s.v;
            meta.pratyaya = s.p;
            meta.vachana = s.n;
            return meta;
        }
    }

    return meta;
}
