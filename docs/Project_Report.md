# Project Report

## 5. Introduction
This project implements a mini compiler front-end for the language described in the Compiler Construction Lab Project Manual. The implementation integrates lexical analysis, parsing, AST construction, symbol-table management, semantic checking, and three-address code generation into one working pipeline.

## 6. Objectives
The goal of the project is to demonstrate how a compiler front-end is built from cooperating phases. The repository is intended to show:
- how source text is turned into tokens,
- how tokens are organized into an AST,
- how scope and type rules are enforced,
- and how TAC is produced from the verified tree.

## 7. Language Specification
The supported language includes:
- data types: `int`, `float`, `bool`
- declarations, assignments, `if`, `if-else`, `while`, `print`, and nested blocks
- arithmetic, relational, and logical expressions

A formal grammar and operator hierarchy are documented in `docs/Language_Specification.md`.

## 8. Compiler Architecture
The compilation pipeline is:
1. read the source file
2. optionally extract a fenced code block from Markdown
3. tokenize the input
4. parse the token stream into an AST
5. perform semantic analysis with scoped symbol tracking
6. generate TAC
7. print the tree and TAC output for demonstration

Each compiler phase is implemented in a separate source module under `src/`.

## 9. Lexer Design
The lexer recognizes:
- keywords: `int`, `float`, `bool`, `if`, `else`, `while`, `print`, `true`, `false`
- identifiers
- integer and floating-point literals
- arithmetic, relational, logical, assignment, and delimiter tokens
- comments and whitespace, which are discarded
- invalid tokens, which are reported as lexical errors

Line numbers are tracked so diagnostics can be reported precisely.

## 10. Parser Design
The parser is implemented with recursive descent and uses precedence-aware expression parsing. It handles:
- declarations
- assignments
- block statements
- `if`
- `if-else`
- `while`
- `print`
- nested expressions with correct precedence and associativity

Basic error recovery is used so the compiler can continue scanning after some syntax errors.

## 11. Abstract Syntax Tree
The AST removes syntactic noise and keeps only meaningful language constructs. Nodes are created for:
- program and block structure
- declarations and assignments
- conditionals and loops
- unary and binary expressions
- literals and identifiers

The project prints the AST in three ways:
- standard tree form
- visual indented tree form
- readable summary form

## 12. Semantic Analysis
The semantic phase validates rules that the grammar alone cannot enforce:
- undeclared variable use
- redeclaration in the same scope
- scope violations
- invalid assignments
- invalid arithmetic, relational, or logical expressions
- incorrect condition types in `if` and `while`

The analyzer uses the AST and symbol table together to produce clear human-readable diagnostics.

## 13. Symbol Table
The symbol table stores each identifier together with:
- its name
- its type
- the scope in which it was declared
- the line where it was declared

Nested scopes are supported so that variables declared inside a block are not visible outside that block.

## 14. Intermediate Code
The compiler generates TAC as the final required output. TAC is emitted for:
- arithmetic expressions
- relational expressions
- logical expressions
- assignments
- `print`
- `if`
- `if-else`
- `while`

Temporaries and labels are generated automatically to keep the output readable.

## 15. Challenges
The main implementation challenges were:
- preserving precedence across multiple expression kinds
- enforcing scope rules cleanly
- keeping the AST and TAC generation readable
- reporting helpful errors without halting after the first issue
- organizing a modular repository structure for future extension

## 16. Testing
The `tests/` directory contains valid and invalid programs that demonstrate:
- successful compilation
- lexical errors
- syntax errors
- semantic errors

The test set covers the manual's required failure cases and also includes examples for floating-point arithmetic, logical expressions, and nested blocks.

## 17. Conclusion
This repository demonstrates a complete and readable compiler front-end suitable for the Compiler Construction Lab project. The implementation is intentionally modular so that future engines, extensions, or bonus features can be added without rewriting the core compiler pipeline.

## 18. References
- Compiler Construction Lab Project Manual
- Flex documentation
- Bison documentation
- GCC documentation
- Course lecture material
