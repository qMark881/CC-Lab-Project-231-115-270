# Compiler Construction Lab Project

This repository implements a modular mini compiler front-end for the language defined in the **Compiler Construction Lab Project Manual**.

It demonstrates the full front-end pipeline required by the project manual:

- Lexical analysis
- Syntax analysis
- Abstract Syntax Tree (AST) construction
- Nested-scope symbol table
- Semantic analysis
- Three Address Code (TAC) generation

## Repository Layout

- `src/` — all compiler source code, grouped by phase
- `tests/` — valid and invalid programs for grading and viva preparation
- `examples/` — sample programs for live demonstration
- `docs/` — project report, language specification, architecture notes, and test matrix
- `Makefile` — build automation

## Build

```bash
make
```

This produces the `compiler` executable in the repository root.

## Run

You can run the compiler directly:

```bash
./compiler tests/valid/complete_program.md
```

or use the `run` target:

```bash
make run FILE=tests/valid/complete_program.md
```

You can also read from standard input:

```bash
./compiler -
```

and then paste a program followed by EOF.

The compiler accepts plain source files and also Markdown files that contain the program inside a fenced code block.

## Output

For valid programs, the compiler prints:

1. the AST
2. a visual tree view
3. a readable summary
4. Three Address Code (TAC)
5. a final success message

You can also print the compiler version with:

```bash
./compiler --version
```

For invalid programs, the compiler reports lexical, syntax, and semantic errors with line numbers wherever possible.

## Language Features

The language supports:

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
- nested blocks with scope handling

## Documentation

Recommended reading order:

1. `docs/Compiler_Architecture.md`
2. `docs/Language_Specification.md`
3. `docs/Project_Report.md`
4. `docs/Test_Matrix.md`

## Demo Programs

A complete demonstration program is available at:

- `examples/valid/full_demo.md`
- `tests/valid/full_demo.md`

## Notes

- The project keeps the manual-required compiler front-end stages intact.
- Generated files and executables are excluded by `.gitignore`.
- Clear commit history and descriptive messages are recommended for submission.
