# Nava Sanskritam — Terminology Review

Status: Initial review register; no candidate below is approved by this document.

## Review principles

The goal is not to replace every English programming term mechanically. The goal is to select a Sanskrit expression whose form, grammar and meaning are defensible and whose technical definition is unambiguous.

For each concept, compare attested Classical Sanskrit terms, possible compounds and derivations, traditional technical usage in grammar/logic/mathematics/shastra, and clarity for programmers.

## Initial review queue

| Existing token / concept | Current technical role (to verify in source) | Review question | Initial status |
|---|---|---|---|
| अस्ति | variable declaration in current guide | Is this suitable as an invariant declaration keyword, or should declaration and existence be distinguished? | under_review |
| नित्य | constant declaration | Is the intended sense “permanent/constant” precise in this context? | under_review |
| विधिः | function declaration | Compare विधि, क्रिया, प्रक्रिया and other candidates; check grammatical form and semantics. | under_review |
| फलम् | return statement | Does “result/fruit” accurately describe returning a value, or is a more precise term preferable? | under_review |
| वद | output/print builtin | Verify imperative form and whether “speak/say” fits the intended usage. | under_review |
| यदि / तर्हि / अन्यथा | conditional syntax | Review syntax consistency and the grammatical/semantic role of each particle. | under_review |
| चक्र | loop construct | Verify whether this is an appropriate technical metaphor and whether a more precise term exists. | under_review |
| गणनम् / गणन | arithmetic builtin | Distinguish noun forms and ensure canonical spelling matches the runtime API. | under_review |
| समय | time builtin | Verify exact API meaning (clock time, timestamp, duration, etc.). | under_review |
| नियमः | assertion/check builtin | Confirm whether “rule” matches assertion semantics or a more precise term is needed. | under_review |
| ज्या / कोज्या | sine / cosine | Verify mathematical terminology against established Sanskrit mathematical literature. | under_review |
| वर्गः / वर्गमूलम् | square / square root | Verify intended mathematical meanings and naming consistency. | under_review |
| त्रैराशिकम् | rule-of-three builtin | Verify traditional mathematical meaning and actual implementation. | under_review |
| दर्शनम्, मंजूषा, दृश्यम् | UI / framework vocabulary | Review whether terms are attested and semantically apt for view, container and component. | under_review |

This queue is an audit starting point, not a final glossary. Source code, documentation and runtime must be checked before any token is classified as a keyword or builtin.

## Required record fields

Each concept record in TERMINOLOGY_DATABASE.json should include: concept; technical_definition; candidate; transliteration; part_of_speech; gender; stem_or_form; derivation; dhatu; pratyaya_or_formation; attestation_status (attested, semantic_extension, coined, unknown); language_profile (laukika, vaidika, both, undetermined); sources with exact bibliographic locations; rationale; alternatives_considered; review_status; reviewer; compiler_impact; compatibility_aliases.

Do not fill grammatical or etymological fields by guesswork. Use null or unknown until verified.