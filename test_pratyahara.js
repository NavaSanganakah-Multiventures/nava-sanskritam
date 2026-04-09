const shivaSutras = [
    { letters: ['अ', 'इ', 'उ'], it: 'ण्' },
    { letters: ['ऋ', 'ऌ'], it: 'क्' },
    { letters: ['ए', 'ओ'], it: 'ङ्' },
    { letters: ['ऐ', 'औ'], it: 'च्' },
    { letters: ['ह', 'य', 'व', 'र'], it: 'ट्' },
    { letters: ['ल'], it: 'ण्' },
    { letters: ['ञ', 'म', 'ङ', 'ण', 'न'], it: 'म्' },
    { letters: ['झ', 'भ'], it: 'ञ्' },
    { letters: ['घ', 'ढ', 'ध'], it: 'ष्' },
    { letters: ['ज', 'ब', 'ग', 'ड', 'द'], it: 'श्' },
    { letters: ['ख', 'फ', 'छ', 'ठ', 'थ', 'च', 'ट', 'त'], it: 'व्' },
    { letters: ['क', 'प'], it: 'य्' },
    { letters: ['श', 'ष', 'स'], it: 'र्' },
    { letters: ['ह'], it: 'ल्' }
];

function getPratyahara(name) {
    let startChar = name.charAt(0);
    let itChar = name.slice(1);

    let result = [];
    let collecting = false;

    for (let sutra of shivaSutras) {
        for (let letter of sutra.letters) {
            if (letter === startChar) {
                collecting = true;
            }
            if (collecting) {
                result.push(letter);
            }
        }
        if (collecting && sutra.it === itChar) {
            return result;
        }
    }
    return [];
}

console.log(getPratyahara('अक्')); // Should be ['अ', 'इ', 'उ', 'ऋ', 'ऌ']
console.log(getPratyahara('हल्')); // all consonants
