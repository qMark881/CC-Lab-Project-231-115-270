# Project Report

## 5. Introduction
This repository implements a mini compiler front-end for the custom language described in the project manual. The implementation demonstrates how the major phases of a compiler communicate with one another: lexical analysis, syntax analysis, AST construction, semantic analysis, symbol-table management, and TAC generation.

## 6. Objectives
The project is intended to demonstrate a complete compiler front-end pipeline in working code. The compiler accepts source programs, detects invalid input with clear diagnostics, builds a structured AST for valid input, validates semantic constraints, and emits Three Address Code for execution-independent representation.

## 7. Language Specification
The supported language includes:
- data types: `int`, `float`, `bool`
- declarations
- assignments
- arithmetic, relational, and logical expressions
- `if`, `if-else`, and `while`
- `print`
- nested blocks with scope-sensitive declarations

A formal grammar reference is documented in `docs/LanguageSpecification.md`.

## 8. Compiler Architecture
The pipeline is organized as follows:

1. Read a source file or Markdown file containing a fenced code block.
2. Lexical analysis converts the input into tokens.
3. The recursive-descent parser constructs the AST.
4. The semantic pass validates types, scope rules, and declaration rules.
5. The TAC generator emits readable three-address code.

The output is intentionally text-based so it can be shown clearly during lab demonstration and viva.

## 9. Lexer Design
The lexer recognizes:
- keywords (`int`, `float`, `bool`, `if`, `else`, `while`, `print`, `true`, `false`)
- identifiers
- integer and floating-point literals
- arithmetic, relational, and logical operators
- braces, parentheses, and semicolons
- comments and whitespace, which are discarded
- invalid tokens, which are reported with line numbers

The lexer keeps track of line numbers for accurate error reporting.

## 10. Parser Design
The parser implements a recursive-descent grammar with precedence-aware expression parsing. It supports declarations, assignments, blocks, `if`, `if-else`, `while`, and `print` statements. Basic recovery is used so the compiler can continue reporting issues where possible.

## 11. Abstract Syntax Tree
The AST removes syntactic noise and retains only meaningful language constructs. Each node stores its kind, label/text, source line, inferred type, and child nodes. The project prints the AST in three ways:
- standard indented tree
- visual branch tree
- readable summary

## 12. Semantic Analysis
The semantic pass walks the AST and validates rules that syntax alone cannot enforce:
- undeclared variable use
- redeclaration in the same scope
- scope violations
- type mismatches
- invalid assignment
- invalid expressions for arithmetic, logical, or relational operators

The analyzer reports clear messages with line numbers whenever possible.

## 13. Symbol Table
The symbol table records each identifier’s:
- name
- type
- scope level
- declaration line
- active/inactive status

Nested blocks are supported, so identifiers declared inside a block are not visible outside that block.

## 14. Intermediate Code
The TAC generator emits a linear, human-readable intermediate form. It handles:
- arithmetic expressions
- relational expressions
- logical expressions
- assignments
- print statements
- `if`
- `if-else`
- `while`

Temporaries and labels are generated automatically to keep the output easy to trace.

## 15. Challenges
The main implementation challenges were:
- keeping the lexer and parser in sync with the language specification
- tracking nested scopes correctly
- making error messages readable
- generating TAC that stays faithful to the AST structure

## 16. Testing
The repository includes:
- valid programs that compile successfully
- invalid programs that demonstrate lexical, syntax, and semantic error handling
- sample programs for presentation and viva preparation

Each test file is small and focused so that a specific language feature or error case can be demonstrated quickly.

## 17. Conclusion
The project provides a clear, modular compiler front-end that demonstrates the relationships between language design, parsing, semantic validation, and intermediate code generation. The repository is organized for demonstration, review, and viva discussion.

## 18. References
- Compiler Construction Lab Project Manual
- Course lecture notes and lab materials
- Standard C library documentation
