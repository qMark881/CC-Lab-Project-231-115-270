# Compiler Construction Lab Project

This repository contains a modular compiler front-end for the language specified in the **Compiler Construction Lab Project Manual**.

The project demonstrates the required compilation pipeline:

- lexical analysis
- syntax analysis
- abstract syntax tree (AST) construction
- nested-scope symbol table management
- semantic analysis
- three-address code (TAC) generation

## Repository Structure

- `src/` — compiler source code
- `tests/` — valid and invalid test programs
- `examples/` — sample programs
- `docs/` — project report, language specification, and supporting notes
- `Makefile` — build automation

## Build

```bash
make
```

This produces the `compiler` executable in the repository root.

## Run

```bash
make run INPUT=tests/valid/arithmetic.md
```

or run the executable directly:

```bash
./compiler tests/valid/arithmetic.md
```

The compiler accepts both:
- plain source files
- Markdown files containing the source inside a fenced code block

## Output

For valid programs, the compiler prints:

1. The AST
2. A visual AST tree
3. A readable summary
4. Three Address Code (TAC)

## Supported Language Features

- `int`, `float`, `bool`
- declarations
- assignments
- arithmetic expressions
- relational expressions
- logical expressions
- `if`
- `if-else`
- `while`
- `print`
- nested blocks

## Demo Programs

- `examples/valid/sample_program.md`
- `examples/valid/full_demo.md`
- `examples/valid/nested_scope.md`

## Testing

The `tests/` directory contains:

- valid programs that compile cleanly through TAC
- invalid programs that demonstrate lexical, syntax, and semantic error handling

## Notes

- Error messages include line numbers where possible.
- Generated files and executables are excluded by `.gitignore`.
- The codebase is organized to be easy to demonstrate during lab evaluation and viva.

## Documentation

- `docs/Project_Report.md`
- `docs/LanguageSpecification.md`
