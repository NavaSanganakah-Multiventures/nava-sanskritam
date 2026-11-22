# Nava Sanskritam — Sanskrit Language Policy

Status: Proposed foundation (v0.1)
Primary profile: लौकिक-संस्कृतम् (Classical Sanskrit)
Future profile: वैदिक-संस्कृतम् (Vedic Sanskrit), a separately specified extension

## 1. Purpose

Nava Sanskritam is intended to be a programming language whose user-facing vocabulary and language design are grounded in attested, grammatically defensible Sanskrit. Devanagari spelling alone does not make a token Sanskrit. Every official keyword and standard-library name must have a documented linguistic and technical justification.

This document establishes a policy; it does not certify the current vocabulary. Existing tokens remain provisional until reviewed.

## 2. Primary linguistic basis

The initial language standard shall use Classical/Laukika Sanskrit as its normative basis. Paninian grammar is the principal grammatical framework, interpreted with relevant traditional grammatical literature and lexicographic evidence.

The Ashtadhyayi must not be described as exclusively a grammar of post-Vedic Classical Sanskrit. Panini's system also records Vedic usage, including rules explicitly scoped by expressions such as chandasi. For the first implementation, Nava Sanskritam will deliberately select the Classical profile and will not silently mix in Vedic-only forms.

### 2.1 What the Laukika profile means
- Prefer forms accepted in the Classical Sanskrit grammatical tradition.
- Record the grammatical form and intended technical meaning of each official term.
- Distinguish attested words from newly coined technical terms.
- Do not invent a derivation or cite a rule unless it has been checked.
- Do not force every programming construct into a Sanskrit inflection if a clear, consistent lexical keyword is more suitable.

## 3. Future Vedic profile

Vedic Sanskrit is in scope as a future language profile, not as an abandoned or inferior form. It requires separate research and specification. A future profile may address Vedic accent notation (उदात्त, अनुदात्त, स्वरित), Unicode handling, Vedic-specific forms and usages, and source attribution for meanings.

Until that profile is specified, Vedic-only forms must not be introduced into the default Classical profile without an explicit compatibility decision.

## 4. Terminology acceptance criteria

Every proposed keyword or standard-library name must have a record containing:
1. Programming concept and precise technical definition.
2. Proposed Devanagari form and, where useful, transliteration.
3. Part of speech, gender (where applicable), stem and inflectional form.
4. Derivation: root (धातु), suffix/formation (प्रत्यय/प्रक्रिया), only when verified.
5. Lexical and grammatical evidence with exact source references.
6. Whether the form is attested, a semantic extension, or a coined technical term.
7. Rationale for why its Sanskrit meaning fits the programming concept.
8. Approved aliases, deprecated forms and migration notes.
9. Review status and reviewer.

### Status vocabulary
- proposed — suggested; not an official language token.
- under_review — linguistic or technical review is in progress.
- approved — accepted for the Classical profile.
- deprecated — retained temporarily for compatibility.
- rejected — reviewed and not accepted.

No term becomes an official keyword merely because it already exists in the compiler.

## 5. Evidence hierarchy

Reviewers should consult, as relevant: Ashtadhyayi with reliable traditional or scholarly commentary; Dhatupatha and relevant grammatical sources; established Sanskrit dictionaries and koshas (for example, Monier-Williams, Vachaspatyam, Shabdakalpadruma and Amarakosha); and attested Classical Sanskrit literature and technical shastra usage.

A dictionary gloss alone does not prove that a proposed technical meaning is appropriate. Cite edition, volume/section/page or stable digital entry wherever possible. Conflicting evidence must be recorded.

## 6. Technical design principles
- Separate surface spelling from the compiler's stable internal token ID.
- The compiler must not depend on English transliteration as a token's identity.
- Aliases must be explicit, documented and test-covered.
- Unicode normalization and Devanagari handling must be tested.
- Avoid using inflectional endings as hidden execution operators unless a complete, documented grammar justifies that behavior.
- Grammatical case, number and tense must not be assigned pointer, memory, concurrency or asynchronous semantics merely by analogy. Such semantics require a separately justified language design.
- Keep syntax, semantics and terminology decisions versioned.

## 7. Governance

Changes to official terminology require a proposal, linguistic evidence and technical rationale, review by a Sanskrit grammar/lexicon reviewer where available, compiler and documentation impact analysis, tests and migration notes, and a recorded approval.

## 8. Scope

This policy does not claim that all existing Nava Sanskritam words are incorrect, nor does it certify any individual token. It creates the review process by which each term will be assessed. The current keyword inventory must be audited before a vocabulary migration is proposed.