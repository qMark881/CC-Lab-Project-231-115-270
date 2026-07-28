# Compiler Architecture

This repository is organized as a front-end compiler pipeline.

## Pipeline

1. **File Reader**
   - Reads plain source files or Markdown files containing a fenced code block.
2. **Lexer**
   - Converts the source text into a stream of tokens.
3. **Parser**
   - Builds an Abstract Syntax Tree from the token stream.
4. **Semantic Analyzer**
   - Builds and checks scope information through the symbol table.
   - Validates type rules and scope rules.
5. **TAC Generator**
   - Produces Three Address Code for valid programs.
6. **Printer**
   - Displays the AST, a visual tree, a readable summary, and TAC.

## Module Responsibilities

| Module | Responsibility |
|---|---|
| `src/lexer/` | tokenization, comments, literals, operators, invalid-token detection |
| `src/parser/` | recursive-descent parsing and AST construction |
| `src/ast/` | AST node allocation, printing, visualization, and cleanup |
| `src/symbol_table/` | scope tracking and identifier lookup |
| `src/semantic/` | type checking, scope checking, and semantic diagnostics |
| `src/tac/` | TAC generation with temporaries and labels |
| `src/util.*` | memory helpers and file loading |
| `src/main.c` | command-line entry point and phase orchestration |

## Design Principles

- Keep parsing and semantic analysis separate.
- Keep AST nodes independent of TAC generation.
- Keep scope handling inside the symbol table.
- Keep error messages human-readable and line-aware.
- Keep the source tree modular so additional compiler phases can be added later without rewriting the whole project.

## Output Order

A successful compilation prints:
1. AST
2. visual AST
3. readable summary
4. TAC

If a lexical, syntax, or semantic error occurs, the compiler stops before printing final output and reports the issue clearly.
