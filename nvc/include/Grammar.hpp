#ifndef GRAMMAR_HPP
#define GRAMMAR_HPP

#include <string>
#include <unordered_map>
#include <vector>

/**
 * Grammar - Pāṇinian Grammatical Logic for Nava Sanskritam (C++ Native)
 * Implements Sandhi rules and Vibhakti processing using UTF-32 for robust Unicode handling.
 */
class Grammar {
public:
    Grammar();
    
    // Sandhi Rules
    std::u32string applySandhi(const std::u32string& w1, const std::u32string& w2);
    
    // Vibhakti Normalization
    std::u32string normalize(const std::u32string& id);
    std::u32string stripUpsarga(const std::u32string& id, std::u32string* upsarga);

    // Phonetic Categories (Pratyaharas)
    std::u32string resolvePratyahara(const std::string& name);
    bool isInPratyahara(char32_t c, const std::string& name);

    // Helpers to convert between UTF-8 and UTF-32
    enum class Vibhakti { 
        PRATHAMA, DWITIYA, TRITIYA, CHATURTHI, 
        PANCHAMI, SHASHTI, SAPTAMI, SAMBODHANA, UNKNOWN 
    };

    enum class Ling {
        PULLINGA,   // Masculine
        STRILINGA,  // Feminine
        NAPUNSAKA,  // Neuter
        UNKNOWN
    };

    enum class SupPratyaya {
        SU, AU, JAS, AM, AUT, SHAS, TA, BHYAM, BHIS, 
        NGE, BHYAM2, BHYAS, NGASI, BHYAM3, BHYAS2, 
        NGAS, OS, AM2, NGI, OS2, SUP
    };

    enum class Lakara {
        LAT, LRT, LOT, LANG, VIDHI_LIN, ASHIR_LIN, LIT, LUT, LUNG, LRUNG, UNKNOWN
    };

    enum class Pada { PARASMAIPADA, ATMANEPADA };

    enum class Gana { BHVADI, ADADI, JOTYADADI, DIVADI, SVADI, TUDADI, RUDHADI, TANADI, KRYADI, CURADI };

    struct WordMeta {
        std::u32string root;
        Vibhakti vibhakti;
        SupPratyaya pratyaya;
        int vachana; // 1: singular, 2: dual, 3: plural
        Ling ling;
    };

    struct VerbMeta {
        std::u32string root;
        Lakara lakara;
        int purusha; // 1: Prathama, 2: Madhyama, 3: Uttama
        int vachana;
        Pada pada;
        Gana gana;
    };

    static bool isPureSanskrit(const std::u32string& word);
    static bool isValidVyakaranName(const std::u32string& word);
    static WordMeta analyzeSubanta(const std::u32string& word);
    static VerbMeta analyzeTinanta(const std::u32string& word);
    static std::vector<std::u32string> analyzeSamas(const std::u32string& word);
    static std::u32string processAnubandha(SupPratyaya p);
    static std::u32string applyVikarana(std::u32string root, Gana g);
    static std::u32string toUtf32(const std::string& utf8);
    static std::string toUtf8(const std::u32string& utf32);

    // Advanced Sandhi
    std::u32string applyVisargaSandhi(char32_t visarga, char32_t next);

    // Numeral Conversion
    static std::string toDevanagariNumerals(double val);
    static std::string toDevanagariNumerals(const std::string& western);

private:
    static const std::vector<std::u32string> shivaSutras;
    std::u32string utf8ToUtf32(const std::string& str);
    std::string utf32ToUtf8(const std::u32string& str);

    bool isSavarna(char32_t c1, char32_t c2);
    char32_t getSavarnaLong(char32_t c);

    std::unordered_map<char32_t, char32_t> gunaMap;
    std::unordered_map<char32_t, char32_t> vriddhiMap;
    std::unordered_map<char32_t, char32_t> yanMap;
};

#endif // GRAMMAR_HPP
