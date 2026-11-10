# Examples — नवसंस्कृतम्

Every program in this folder compiles and runs with the current NVC interpreter:

```bash
nvc --run examples/sankalpa.ns
```

`run_examples.py` runs all of them and fails if any one exits non-zero. It is wired
into the `NVC Interpreter Tests` workflow, so an example that stops working is a
CI failure rather than a stale file nobody notices.

## Core language

| File | What it shows |
| --- | --- |
| `sankalpa.ns` | Constants, variables, a `विधिः` function, string concatenation |
| `test_interpreter.ns` | Functions, `यदि / तर्हि / अन्यथा`, `फलम्` return values |
| `vibhakti_test.ns` | Same logic written with declined (vibhakti) identifiers |
| `audityam.ns` | `नित्य` immutability, `वर्गः` intrinsic, mixed printing |
| `verified_logic_test.ns` | Shashti (possessive) identifier forms in expressions |
| `check_v3.ns` | Latin digits mixed with Devanagari digits, conditionals |
| `custom_test.ns` | `क्रिया` as a synonym for `विधिः`, ASCII function names |

## Standard library

| File | What it shows |
| --- | --- |
| `math_master_test.ns` | `^`, `वर्गः`, `वर्गमूलम्`, `त्रैराशिकम्` and the rest of the math intrinsics |
| `sandhi_test.ns` | `सन्धिः` string joining |
| `Rupa.ns` | The Rupa colour tokens (`रक्तवर्णः`, `नीलवर्णः`, …) used by the UI layer |

## Darshanam (declarative UI)

| File | What it shows |
| --- | --- |
| `website.ns` | Nested `मंजूषा` containers with `दृश्यम् पाठः / बटनम्` and attributes |
| `complete_ui_test.ns` | A full page: header, list, form field, buttons |
| `ui_test.ns` | Positional form — `दृश्यम् बटनम्(५०, १००)` |
| `Samhita.ns` | Showcase program that also exercises block comments |

Darshanam programs print `darshanam.json` on stdout when run with the interpreter,
and can be exported as a static site with `nvc --target web <file>.ns`.

## legacy/

These are pre-v18 demo scripts kept for reference. They use syntax the current
parser does not accept, and `run_examples.py` skips them:

| File | Blocked on |
| --- | --- |
| `shashti_test.ns`, `file.ns`, `my.ns` | Object literals (`{ रत्नम्: ९९९ }`) and postfix `वद` |
| `shloka.ns`, `subanta_test.ns` | Optional semicolons and postfix `वद` (v7 / v9 style) |
| `syntax_check.ns` | Invalid call — two arguments with no comma |
| `math_test.ns` | Deliberately triggers `नियमः` assertion failure (a TRAP) |

Turning `legacy/` back into runnable examples is tracked as language work, not as
a documentation fix.
