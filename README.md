# Nava Sanskritam (नव संस्कृतम्)

A professional, high-performance programming language built on Panini's Ashtadhyayi logic, created by **NavaSanganakah Multiventures**.

## Vedic Vision

At NavaSanganakah Multiventures, we blend the ancient intelligence of Vedic sciences with modern computation. Panini's grammatical rules in Ashtadhyayi lay out a complete, unambiguous algorithmic framework that predates modern computer science by millennia. Nava Sanskritam is our proprietary, production-ready execution engine designed to bring this profound logic directly into modern software development, providing developers with a robust, modular, and performant AST-based interpreter.

## Installation

You can install the Nava CLI globally on your local machine using `npm link` to map the `nava` command to your environment:

```bash
# Clone the repository
git clone <repository-url>
cd nava

# Install dependencies
npm install

# Link the package globally
npm link
```

After installation, you can use the command globally:

```bash
nava run yourfile.ns
```

## Syntax Specs

Nava Sanskritam supports a robust core of programmatic concepts using Devanagari numerals and Sanskrit keywords.

### Variable Declaration (अस्ति)
Use `अस्ति` to declare variables.

```sanskrit
अस्ति नाम = "आचार्य";
अस्ति क = १०;
```

### Output (वद)
Use `वद` to print expressions to the console.

```sanskrit
वद("सत्यम्");
वद(नाम);
```

### Conditionals (यदि-तर्हि)
Use `यदि` for conditions and `तर्हि` for execution blocks if the condition is met.

```sanskrit
यदि (क > ५) तर्हि वद("सत्यम्");

यदि (क > ५) तर्हि {
    वद("सत्यम्");
}
```

### Loops (चक्र)
Use `चक्र` to create iterative loops using standard or Devanagari numerals.

```sanskrit
चक्र (i = १; i < ५; i = i + १) {
    वद(i);
}
```

## Development and Architecture
- `Lexer.js`: Robust tokenizer supporting UTF-8 identifiers, Paninian keywords, and Devanagari Numerals.
- `Parser.js`: Builds an Abstract Syntax Tree (AST) utilizing standard logical bindings and structures.
- `Interpreter.js`: AST execution engine complete with lexical environment scoping.

## License
Proprietary software of NavaSanganakah Multiventures. All Rights Reserved.
