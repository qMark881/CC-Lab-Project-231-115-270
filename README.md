# Compiler Construction Lab Project

This repository contains a modular mini compiler front-end for the language defined in the **Compiler Construction Lab Project Manual.pdf**.

It includes:

- Lexical analysis
- Syntax analysis
- Abstract Syntax Tree (AST) construction
- Visual AST printing and readable summaries
- Nested-scope symbol table
- Semantic analysis
- Three Address Code (TAC) generation

## Repository Structure

- `src/` — compiler source code
- `tests/` — valid and invalid test programs
- `examples/` — sample programs
- `docs/` — project report and supporting documents

## Build

```bash
make
```

This produces the `compiler` executable in the repository root.

## Run

```bash
./compiler tests/valid/arithmetic.md
```

The compiler accepts plain source files and also Markdown files that contain the program inside a fenced code block.

## Output Style

For every valid program, the compiler prints:

1. The standard AST
2. A visual tree view of the AST
3. A readable summary of the program structure
4. Three Address Code (TAC)

This makes the compiler easier to demonstrate during lab evaluation and viva.

## Supported Language Features

- `int`, `float`, `bool`
- variable declarations
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

A more complete demonstration program is available at `examples/valid/full_demo.md` and `tests/valid/full_demo.md`.

## Notes

- Valid programs produce AST and TAC output.
- Lexical, syntax, and semantic errors are reported with line numbers where possible.
- Generated files and executables are excluded by `.gitignore`.

## Contributing

Please keep changes modular, readable, and well documented.
