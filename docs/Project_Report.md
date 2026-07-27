# Project Report

## 1. Introduction
This repository implements a mini compiler front-end for the language defined in the Compiler Construction Lab Project Manual.

## 2. Lexical Analyzer
The lexer recognizes keywords, identifiers, numeric literals, boolean literals, operators, delimiters, comments, whitespace, and invalid tokens. It also keeps track of line numbers so that diagnostics can point to the correct location.

## 3. Syntax Analyzer
The parser implements a recursive-descent grammar for declarations, assignments, if, if-else, while, print, nested blocks, and expressions with correct precedence and associativity. It handles basic error recovery so that the compiler can report more than one issue in a single run when possible.

## 4. Abstract Syntax Tree
The parser builds an AST that removes syntactic noise and preserves only meaningful language constructs. In addition to the regular indented tree output, the project now includes a visual tree view and a readable summary mode, which make the structure easier to understand during demonstration.

## 5. Symbol Table
The semantic pass uses a nested-scope symbol table to detect redeclaration, undeclared variables, and scope violations. Each identifier is tracked with its declared type, scope level, and line number.

## 6. Semantic Analysis
Type checking validates arithmetic, relational, logical, assignment, and conditional expressions. The semantic phase rejects invalid programs such as mixing incompatible types, using undeclared variables, or referencing variables outside their active scope.

## 7. Intermediate Code Generation
The backend emits human-readable Three Address Code (TAC) for expressions, assignment, print, if, if-else, and while statements. Temporary variables and labels are generated automatically to keep the output readable and easy to trace.

## 8. Testing
Valid and invalid test programs are stored in `tests/` and `examples/`. The repository is structured to make it easy to demonstrate successful compilation as well as lexical, syntax, and semantic failures.

## 9. Build Instructions
Run `make` from the repository root to build the compiler executable.

## 10. Run Instructions
Example:

```bash
./compiler tests/valid/arithmetic.md
```

Markdown files are supported when the source code is inside a fenced code block.

## 11. Conclusion
The repository provides a readable, modular compiler front-end suitable for lab demonstration and viva preparation. The added visual AST and readable summary output improve clarity for students reviewing compiler construction concepts.
