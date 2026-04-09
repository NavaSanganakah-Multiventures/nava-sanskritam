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
    static std::u32string toUtf32(const std::string& utf8);
    static std::string toUtf8(const std::u32string& utf32);

    // Advanced Sandhi
    std::u32string applyVisargaSandhi(char32_t visarga, char32_t next);

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
