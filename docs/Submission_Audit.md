# Final Submission Audit

## Executive Result

The submitted repository was inspected against the seven requested evaluation areas. All seven areas are present and connected in a working pipeline. The original archive did not build cleanly; build-breaking defects were repaired before the final verification.

## Rubric Matrix

| Evaluation area | Status | Evidence |
|---|---:|---|
| Lexical Analysis | PASS | Keywords, identifiers, literals, operators, delimiters, comments, invalid characters, line tracking |
| Syntax Analysis | PASS | Complete statement grammar, precedence-aware expressions, nested blocks, panic-mode recovery |
| Abstract Syntax Tree | PASS | Typed node model, constructors, tree/visual/summary printers, recursive cleanup |
| Symbol Table | PASS | Name, type, scope, declaration line, active-state tracking, nested-scope visibility |
| Semantic Analysis | PASS | Undeclared use, redeclaration, out-of-scope access, assignment/type errors, operator rules, boolean conditions |
| Three-Address Code | PASS | Expressions, assignments, print, if/else, while, temporaries, labels, jumps, validation |
| Project Report | PASS | Manual-aligned PDF and Markdown report under `docs/` |

## Defects Corrected During Finalization

- Added missing standard includes that prevented compilation.
- Repaired parser and semantic diagnostic calls with incorrect parameters.
- Corrected TAC label validation and unsafe label parsing.
- Added lexer destruction and removed detected memory leaks.
- Added parser progress guarantees and panic-mode recovery to prevent malformed input from causing an infinite loop.
- Separated lexical and syntax error counts.
- Removed false-positive warnings for legal declarations without initializers.
- Prevented duplicate cascading semantic diagnostics after an earlier expression error.
- Corrected two incorrectly classified valid tests.
- Replaced the fragile one-command build with object-based dependency-aware Make rules.
- Added regression automation and deterministic expected-output snapshots.
- Added terminal evidence images for build, valid compilation, lexical failure, and semantic failure.

## Verification Performed

```text
Clean strict build: PASS
Regression suite: 29/29 PASS
Valid programs accepted: 13/13
Invalid programs rejected: 16/16
AddressSanitizer/UndefinedBehaviorSanitizer suite: PASS
TAC validation on valid control-flow programs: PASS
```

## Important Submission Note

The mandatory compiler stages are complete. Flex and Bison specification files are included for the formal course toolchain. The tested default build is intentionally portable and does not require generator binaries to be installed on the grading machine.
