# Debugging Guide

This guide provides comprehensive information for debugging the compiler implementation.

## Table of Contents

1. [Compiler Architecture](#compiler-architecture)
2. [Debugging Tools](#debugging-tools)
3. [Common Issues and Solutions](#common-issues-and-solutions)
4. [Debugging by Phase](#debugging-by-phase)
5. [Testing Strategies](#testing-strategies)

## Compiler Architecture

The compiler is organized into several phases:

1. **Lexical Analysis** (`src/lexer/`) - Tokenizes source code
2. **Syntax Analysis** (`src/parser/`) - Builds Abstract Syntax Tree (AST)
3. **Semantic Analysis** (`src/semantic/`) - Type checking and scope validation
4. **Symbol Table** (`src/symbol_table/`) - Manages variable scopes
5. **AST** (`src/ast/`) - Represents program structure
6. **Three Address Code** (`src/tac/`) - Generates intermediate code

## Debugging Tools

### AST Visualization

The compiler provides multiple ways to visualize the AST:

```bash
# Standard AST printing
./compiler tests/valid/complete_program.md

# Visual tree representation (automatically included)
./compiler tests/valid/complete_program.md

# Readable summary format (automatically included)
./compiler tests/valid/complete_program.md
```

### Symbol Table Debugging

Use the symbol table debugging functions to inspect variable scopes:

```c
#include "symbol_table/symbol_table.h"

// Print active symbols only
symtab_print(&table);

// Print all symbols including inactive ones
symtab_print_detailed(&table);
```

### Enhanced AST Information

Use the detailed AST printing function for comprehensive node information:

```c
#include "ast/ast.h"

// Print detailed information about a specific node
ast_print_detailed(node);
```

## Common Issues and Solutions

### Lexical Analysis Issues

**Problem**: Invalid character errors
- **Solution**: Check source code for unsupported characters
- **Debug**: The lexer now reports line and column numbers for precise location

**Problem**: Unterminated comments
- **Solution**: Ensure all block comments are properly closed with `*/`
- **Debug**: Lexer reports the exact line where the comment started

### Syntax Analysis Issues

**Problem**: Unexpected token errors
- **Solution**: Check for missing semicolons, braces, or parentheses
- **Debug**: Parser reports the expected token vs. actual token

**Problem**: Missing operators
- **Solution**: Verify all expressions have proper operators
- **Debug**: Check operator precedence in the language specification

### Semantic Analysis Issues

**Problem**: Undeclared variable errors
- **Solution**: Ensure all variables are declared before use
- **Debug**: Use `symtab_print_detailed()` to see all symbols in scope

**Problem**: Type mismatch errors
- **Solution**: Check that operands match operator requirements
- **Debug**: AST printing now shows type information for expressions

**Problem**: Out of scope errors
- **Solution**: Verify variable scope and lifetime
- **Debug**: Use symbol table debugging to track scope levels

### Scope Issues

**Problem**: Variable not accessible in nested block
- **Solution**: Ensure proper scope nesting and variable declaration
- **Debug**: Check scope level in symbol table output

**Problem**: Redeclaration errors
- **Solution**: Avoid declaring variables with the same name in the same scope
- **Debug**: Symbol table shows current scope level for each variable

## Debugging by Phase

### Lexical Analysis Debugging

1. **Enable lexer debugging**: Add print statements in `lexer_next()` function
2. **Check token types**: Verify each token matches expected TokenType
3. **Line/column tracking**: Use enhanced error reporting for precise locations

### Syntax Analysis Debugging

1. **Parser state**: Monitor `parser->current` token and `parser->error_count`
2. **AST construction**: Use `ast_print_detailed()` to inspect created nodes
3. **Error recovery**: Parser continues after errors to find additional issues

### Semantic Analysis Debugging

1. **Symbol table state**: Use `symtab_print()` and `symtab_print_detailed()`
2. **Type information**: AST nodes now contain resolved type information
3. **Error tracking**: Check `ctx->error_count` and `ctx->issues` for problems

### AST Debugging

1. **Node inspection**: Use `ast_print_detailed()` for comprehensive node information
2. **Type checking**: Verify `node->data_type` is correctly set
3. **Structure validation**: Ensure parent-child relationships are correct

## Testing Strategies

### Unit Testing

Test individual components:

```bash
# Test lexer with simple expressions
echo "int x = 10;" | ./compiler -

# Test parser with complex expressions
echo "x = (10 + 20) * 3;" | ./compiler -

# Test semantic analysis with scope
echo "{ int x = 10; print x; }" | ./compiler -
```

### Integration Testing

Test complete programs:

```bash
# Test valid programs
./compiler tests/valid/complete_program.md
./compiler tests/valid/full_demo.md

# Test invalid programs
./compiler tests/invalid/undeclared_variable.md
./compiler tests/invalid/type_mismatch.md
```

### Edge Case Testing

Test boundary conditions:

```bash
# Test complex nesting
./compiler tests/valid/deep_nesting.md

# Test boundary conditions
./compiler tests/valid/boundary_conditions.md

# Test complex expressions
./compiler tests/valid/complex_expressions.md
```

## Debugging Checklist

Before reporting issues, verify:

- [ ] Source code syntax is correct
- [ ] All variables are properly declared
- [ ] Types match operator requirements
- [ ] Scope rules are followed
- [ ] No semantic errors in the program
- [ ] Compiler is built with latest changes
- [ ] Test cases are up to date

## Advanced Debugging

### Adding Custom Debug Output

Add debug prints to track execution:

```c
// In lexer.c
printf("Lexer: Processing token %s at line %d\n", 
       token_type_name(tok.type), tok.line);

// In parser.c
printf("Parser: Parsing %s statement\n", ast_kind_name(node->kind));

// In semantic.c
printf("Semantic: Analyzing expression with type %s\n", 
       type_to_string(node->data_type));
```

### Performance Profiling

Identify bottlenecks:

```c
#include <time.h>

clock_t start = clock();
// Code to profile
clock_t end = clock();
double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
printf("Time: %f seconds\n", elapsed);
```

## Getting Help

If issues persist:

1. Check this documentation
2. Review the language specification
3. Examine test cases for similar patterns
4. Use the debugging tools provided
5. Check the project architecture documentation

## Additional Resources

- [Language Specification](Language_Specification.md)
- [Compiler Architecture](Compiler_Architecture.md)
- [Project Report](Project_Report.md)
- [Test Matrix](Test_Matrix.md)