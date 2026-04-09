const SanjnaEngine = require('./SanjnaEngine');

class FixedSanjnaEngine extends SanjnaEngine {
    constructor() {
        super();
        this.gunaMap = {
            'ि': 'े', 'ी': 'े',
            'ु': 'ो', 'ू': 'ो',
            'ृ': 'र्', 'ॄ': 'र्',
            'इ': 'ए', 'ई': 'ए',
            'उ': 'ओ', 'ऊ': 'ओ',
            'ऋ': 'अर्', 'ॠ': 'अर्',
            'ऌ': 'अल्'
        };

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
}
const eng = new FixedSanjnaEngine();
console.log(eng.applyGuna('भानु'));
console.log(eng.applyVriddhi('देव'));
