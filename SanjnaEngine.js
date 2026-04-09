class SanjnaEngine {
    constructor() {
        // गुण (Guna) - 1.1.2: अदेङ्गुणः (अ, ए, ओ)
        this.gunaMap = {
            'ि': 'े', 'ी': 'े',
            'ु': 'ो', 'ू': 'ो',
            'ृ': 'र्', 'ॄ': 'र्',
            'इ': 'ए', 'ई': 'ए',
            'उ': 'ओ', 'ऊ': 'ओ',
            'ऋ': 'अर्', 'ॠ': 'अर्',
            'ऌ': 'अल्'
        };

        // वृद्धि (Vriddhi) - 1.1.1: वृद्धिरादैच् (आ, ऐ, औ)
        this.vriddhiMap = {
            'अ': 'आ', 'आ': 'आ',
            'इ': 'ऐ', 'ई': 'ऐ', 'ए': 'ऐ', 'ऐ': 'ऐ',
            'उ': 'औ', 'ऊ': 'औ', 'ओ': 'औ', 'औ': 'औ',
            'ऋ': 'आर्', 'ॠ': 'आर्',
            'ऌ': 'आल्',
            'ि': 'ै', 'ी': 'ै', 'े': 'ै', 'ै': 'ै',
            'ु': 'ौ', 'ू': 'ौ', 'ो': 'ौ', 'ौ': 'ौ'
        };
    }

    /**
     * Applies Guna transformation to the last vowel if applicable.
     */
    applyGuna(word) {
        if (!word) return word;
        // Find the last character. If it's in the gunaMap, transform it.
        const lastChar = word.charAt(word.length - 1);
        if (this.gunaMap[lastChar]) {
            return word.slice(0, -1) + this.gunaMap[lastChar];
        }
        return word;
    }

    /**
     * Applies Vriddhi transformation to the first vowel (or simplistic last vowel for now).
     * Traditionally Vriddhi applies to the first vowel of a word in many taddhita affixes,
     * but we'll implement a basic phonetic transformation.
     */
    applyVriddhi(word) {
        if (!word) return word;
        const lastChar = word.charAt(word.length - 1);
        if (this.vriddhiMap[lastChar]) {
            return word.slice(0, -1) + this.vriddhiMap[lastChar];
        }
        // Also check if the first character is a vowel that can take Vriddhi
        const firstChar = word.charAt(0);
        if (this.vriddhiMap[firstChar]) {
             return this.vriddhiMap[firstChar] + word.slice(1);
        }
        return word;
    }

    /**
     * Rule conflict resolution mechanism:
     * विप्रतिषेधे परं कार्यम् (1.4.2) - In case of a conflict, the later rule prevails.
     * rule1 and rule2 are assumed to be objects with an `id` (e.g., "1.1.1") and `apply` function.
     */
    resolveConflict(rule1, rule2, word) {
        // Simple comparison of rule IDs (assuming standard formatting like "1.1.1" vs "1.1.2")
        // But the rule is "Para" (later). So "1.4.2" beats "1.1.1".
        const parseRuleId = (id) => id.split('.').map(Number);
        const r1 = parseRuleId(rule1.id);
        const r2 = parseRuleId(rule2.id);

        let rule1IsLater = false;
        for (let i = 0; i < Math.max(r1.length, r2.length); i++) {
            const v1 = r1[i] || 0;
            const v2 = r2[i] || 0;
            if (v1 > v2) {
                rule1IsLater = true;
                break;
            } else if (v2 > v1) {
                rule1IsLater = false;
                break;
            }
        }

        const selectedRule = rule1IsLater ? rule1 : rule2;
        return selectedRule.apply(word);
    }
}

module.exports = SanjnaEngine;
