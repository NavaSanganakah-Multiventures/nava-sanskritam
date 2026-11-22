#pragma once

// Canonical spellings of every NVC builtin, in one place.
//
// Why this file exists: CodeGen.cpp used to compare call names against
// Devanagari string literals that had been corrupted by a bad
// decode/re-encode cycle (they were stored as mojibake such as "Ã Â¤Â¸..."),
// so no Devanagari builtin name ever matched and every such call fell through
// to "unknown function". Keeping the only copy of these strings here - as
// correct UTF-8, written once - removes that class of bug. Call sites compare
// enum values, not strings.
//
// Rule for contributors: never write a Devanagari literal anywhere else.
// Add the name here, add an id, and switch on the id.

#include <string>

namespace Keywords {

// Nitya builtins (standard library)
inline constexpr const char* SAMAYA = "समय";            // samaya  - wall clock
inline constexpr const char* GANANA = "गणन";            // ganana  - 2-arg arithmetic
inline constexpr const char* GANANAM = "गणनम्";          // gananam - alias of the above
inline constexpr const char* NIYAMAH = "नियमः";          // niyamah - assertion
inline constexpr const char* JYA = "ज्या";               // jya     - sine
inline constexpr const char* KOJYA = "कोज्या";            // kojya   - cosine
inline constexpr const char* VARGAMULAM = "वर्गमूलम्";     // vargamulam  - square root
inline constexpr const char* TRAIRASHIKAM = "त्रैराशिकम्";  // trairashikam - rule of three
inline constexpr const char* VARGAH = "वर्गः";            // vargah  - square

}  // namespace Keywords

enum class BuiltinId {
    Unknown,
    Samaya,
    Ganana,
    Niyama,
    Jya,
    Kojya,
    Vargamula,
    Trairashika,
    Varga,
};

// Resolves a call name to a builtin. Accepts the Devanagari spelling and, for
// the names that were already usable in ASCII, the Latin alias.
inline BuiltinId builtinId(const std::string& name) {
    if (name == Keywords::SAMAYA || name == "samaya") return BuiltinId::Samaya;
    if (name == Keywords::GANANA || name == Keywords::GANANAM ||
        name == "ganana" || name == "gananam") return BuiltinId::Ganana;
    if (name == Keywords::NIYAMAH || name == "niyamah") return BuiltinId::Niyama;
    if (name == Keywords::JYA || name == "jya") return BuiltinId::Jya;
    if (name == Keywords::KOJYA || name == "kojya") return BuiltinId::Kojya;
    if (name == Keywords::VARGAMULAM || name == "vargamulam") return BuiltinId::Vargamula;
    if (name == Keywords::TRAIRASHIKAM || name == "trairashikam") return BuiltinId::Trairashika;
    if (name == Keywords::VARGAH || name == "vargah") return BuiltinId::Varga;
    return BuiltinId::Unknown;
}

inline const char* builtinDisplayName(BuiltinId id) {
    switch (id) {
        case BuiltinId::Samaya: return Keywords::SAMAYA;
        case BuiltinId::Ganana: return Keywords::GANANA;
        case BuiltinId::Niyama: return Keywords::NIYAMAH;
        case BuiltinId::Jya: return Keywords::JYA;
        case BuiltinId::Kojya: return Keywords::KOJYA;
        case BuiltinId::Vargamula: return Keywords::VARGAMULAM;
        case BuiltinId::Trairashika: return Keywords::TRAIRASHIKAM;
        case BuiltinId::Varga: return Keywords::VARGAH;
        case BuiltinId::Unknown: break;
    }
    return "";
}
