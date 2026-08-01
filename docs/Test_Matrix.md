# Test Matrix

Final verified result: **29/29 tests passed** (`13` valid, `16` invalid).

## Valid Programs

| File | Purpose |
|---|---|
| `tests/valid/arithmetic.md` | arithmetic precedence and print output |
| `tests/valid/assignment.md` | assignment handling |
| `tests/valid/boundary_conditions.md` | boundary values and multiple declarations |
| `tests/valid/complete_program.md` | integrated control flow, scope, AST, and TAC |
| `tests/valid/complex_expressions.md` | mixed numeric and nested expression precedence |
| `tests/valid/declaration.md` | type declarations and initialization |
| `tests/valid/deep_nesting.md` | deeply nested lexical scopes |
| `tests/valid/float_arithmetic.md` | floating-point arithmetic and safe widening |
| `tests/valid/full_demo.md` | large end-to-end demonstration |
| `tests/valid/if_else.md` | conditional branching and labels |
| `tests/valid/logical_expressions.md` | boolean expressions and logical operators |
| `tests/valid/nested_blocks.md` | nested scope visibility |
| `tests/valid/while.md` | loop analysis and backward TAC jump |

## Invalid Programs

| File | Purpose |
|---|---|
| `tests/invalid/condition_type_error.md` | non-boolean if/while condition |
| `tests/invalid/invalid_assignment.md` | incompatible assignment |
| `tests/invalid/invalid_logical_token.md` | invalid single logical token |
| `tests/invalid/lexical_error.md` | invalid character detection |
| `tests/invalid/logical_type_error.md` | logical operator applied to numeric values |
| `tests/invalid/missing_identifier.md` | declaration missing identifier and parser recovery |
| `tests/invalid/missing_semicolon.md` | statement terminator syntax error |
| `tests/invalid/modulo_type_error.md` | modulo applied to non-integer operands |
| `tests/invalid/operator_precedence_error.md` | expression type failure after precedence evaluation |
| `tests/invalid/redeclaration.md` | same-scope redeclaration |
| `tests/invalid/scope_violation.md` | identifier used after declaring scope exits |
| `tests/invalid/syntax_error.md` | general grammar violation |
| `tests/invalid/type_mismatch.md` | initializer or assignment type mismatch |
| `tests/invalid/undeclared_variable.md` | identifier used without declaration |
| `tests/invalid/unterminated_comment.md` | unfinished block comment |
| `tests/invalid/unterminated_string.md` | unsupported/unterminated quote input |

## Verification Commands

```bash
make clean && make
make test
make sanitize FILE=tests/valid/complete_program.md
```

Expected/actual snapshots are stored under `tests/expected/valid/` and `tests/expected/invalid/`.
