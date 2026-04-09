#include "Grammar.hpp"
#include <codecvt>
#include <locale>

const std::vector<std::u32string> Grammar::shivaSutras = {
    U"अइउण्", U"ऋलृक्", U"एओङ्", U"ऐऔच्", U"हयवरट्", U"लण्",
    U"ञमङणनम्", U"झभञ्", U"घढधष्", U"जबगडदश्", U"खफछठथचटतव्", U"कपय्",
    U"शषसर्", U"हल्"
};

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

    // Savarnadirgha
    if (isSavarna(last, first)) {
        std::u32string res = w1.substr(0, w1.length() - 1);
        res += getSavarnaLong(last);
        res += w2.substr(1);
        return res;
    }

    // Guna
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
