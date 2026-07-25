
# Project Report

## 1. Introduction
This repository implements a mini compiler front-end for the language defined in the Compiler Construction Lab Project Manual.

## 2. Lexical Analyzer
The lexer recognizes keywords, identifiers, numeric literals, boolean literals, operators, delimiters, comments, whitespace, and invalid tokens.

## 3. Syntax Analyzer
The parser implements a recursive-descent grammar for declarations, assignments, if, if-else, while, print, nested blocks, and expressions with correct precedence.

## 4. Abstract Syntax Tree
The parser builds an AST that removes syntactic noise and preserves only meaningful language constructs.

## 5. Symbol Table
The semantic pass uses a nested-scope symbol table to detect redeclaration, undeclared variables, and scope violations.

## 6. Semantic Analysis
Type checking validates arithmetic, relational, logical, assignment, and conditional expressions.

## 7. Intermediate Code Generation
The backend emits human-readable Three Address Code (TAC) for expressions, assignment, print, if, if-else, and while statements.

## 8. Testing
Valid and invalid test programs are stored in `tests/` and `examples/`.

## 9. Build Instructions
Run `make` from the repository root to build the compiler executable.

## 10. Run Instructions
Example:

```bash
./compiler tests/valid/arithmetic.md
```

Markdown files are supported when the source code is inside a fenced code block.

## 11. Conclusion
The repository provides a complete, readable, modular compiler front-end suitable for lab demonstration and viva preparation.
