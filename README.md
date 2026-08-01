# Compiler Construction Lab Project

**Course:** Compiler Construction Laboratory  
**Department:** Computer Science and Engineering, Metropolitan University, Bangladesh  
**Project:** Mini Programming Language Compiler Front-End with Three-Address Code

This repository implements the complete compiler pipeline required by the course project manual:

```text
Source Program
     |
     v
Lexical Analysis -> Syntax Analysis -> Abstract Syntax Tree
                                         |
                                         v
                                  Semantic Analysis
                                  + Symbol Table
                                         |
                                         v
                              Three-Address Code (TAC)
```

## Evaluation Rubric Coverage

| Rubric item | Implementation |
|---|---|
| Lexical Analysis | `src/lexer/lexer.c`, `src/lexer/lexer.h`, formal `src/lexer/lexer.l` |
| Syntax Analysis | `src/parser/parser.c`, `src/parser/parser.h`, formal `src/parser/parser.y` |
| Abstract Syntax Tree | `src/ast/ast.c`, `src/ast/ast.h` |
| Symbol Table | `src/symbol_table/symbol_table.c`, `src/symbol_table/symbol_table.h` |
| Semantic Analysis | `src/semantic/semantic.c`, `src/semantic/semantic.h` |
| Three-Address Code | `src/tac/tac.c`, `src/tac/tac.h` |
| Project Report | `docs/Project_Report.pdf` and `docs/Project_Report.md` |

## Implemented Language

The compiler supports the instructor-defined language without changing its mandatory core:

- data types: `int`, `float`, `bool`
- declarations with optional initialization
- assignments
- arithmetic operators: `+ - * / %`
- relational operators: `< > <= >= == !=`
- logical operators: `&& || !`
- `if`, `if-else`, `while`, and `print`
- nested blocks with lexical scope
- integer, floating-point, and boolean literals
- single-line and block comments

## Build

### Requirements

- GCC or another C11 compiler
- GNU Make
- Python 3 for the regression-test runner

On Ubuntu/Debian:

```bash
sudo apt install build-essential python3
```

### Compile

```bash
make
```

The build uses strict warnings:

```text
-std=c11 -Wall -Wextra -Wpedantic
```

The resulting executable is:

```text
./compiler
```

## Run

Compile a representative valid program:

```bash
./compiler tests/valid/complete_program.md
```

Run through Make:

```bash
make run FILE=tests/valid/full_demo.md
```

Read from standard input:

```bash
./compiler -
```

The compiler accepts plain source files and Markdown files containing a fenced code block.

## Output

For a valid program, the compiler prints:

1. Abstract Syntax Tree
2. Visual AST
3. Complete symbol table with scope information
4. Three-Address Code
5. Final compilation result

For invalid input, it returns a non-zero exit code and reports lexical, syntax, or semantic diagnostics with standardized error codes and source line numbers.

Useful options:

```text
--help, -h
--version, -v
--verbose, -V
--output, -o <file>
--stats, -S
--json, -J
--csv, -C
--profile, -P <none|basic|detailed|full>
--format, -F
```

## Automated Testing

Run the complete regression suite:

```bash
make test
```

The final verified suite contains:

- 13 valid end-to-end programs
- 16 invalid programs covering lexical, syntax, scope, and type errors
- 29/29 passing tests

Expected/actual grading snapshots are stored under:

```text
tests/expected/valid/
tests/expected/invalid/
```

Refresh snapshots only after verifying intended changes:

```bash
make expected
```

Memory and undefined-behavior checking:

```bash
make sanitize FILE=tests/valid/complete_program.md
```

## Flex and Bison Specifications

The repository includes formal generator specifications:

```text
src/lexer/lexer.l
src/parser/parser.y
```

The portable `make` build uses the reviewed C implementation so the project remains buildable even on systems where Flex and Bison are unavailable. On a machine with both tools installed, the formal specifications can be regenerated and checked with:

```bash
make flex-bison-check
```

Generated files are written to `build/generated/` and are not committed as primary source.

## Repository Structure

```text
.
├── docs/
│   ├── Project_Report.pdf
│   ├── Project_Report.md
│   ├── Compiler_Architecture.md
│   ├── Language_Specification.md
│   ├── Test_Matrix.md
│   └── screenshots/
├── examples/
├── scripts/
│   └── run_tests.py
├── src/
│   ├── ast/
│   ├── lexer/
│   ├── parser/
│   ├── semantic/
│   ├── symbol_table/
│   └── tac/
├── tests/
│   ├── valid/
│   ├── invalid/
│   └── expected/
├── Makefile
└── README.md
```

## Demonstration Sequence

For presentation or viva:

```bash
make clean && make
make test
./compiler tests/valid/full_demo.md
./compiler tests/invalid/lexical_error.md
./compiler tests/invalid/type_mismatch.md
./compiler tests/invalid/scope_violation.md
```

This sequence demonstrates successful compilation, AST construction, scoped symbol handling, TAC generation, and all three required error classes.

## Team

- **Mark Pranto Sarkar** - Student ID: **231-115-270**
- **Tanmoy Das** - Project contributor

## Documentation

Recommended reading order:

1. `docs/Project_Report.pdf`
2. `docs/Language_Specification.md`
3. `docs/Compiler_Architecture.md`
4. `docs/Test_Matrix.md`
5. `docs/Submission_Audit.md`
