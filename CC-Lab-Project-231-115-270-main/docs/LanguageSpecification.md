# Language Specification

This document summarizes the compiler’s supported language features and a concise grammar reference.

## Lexical Elements

- Keywords: `int`, `float`, `bool`, `if`, `else`, `while`, `print`, `true`, `false`
- Identifiers: start with a letter or underscore, followed by letters, digits, or underscores
- Literals:
  - integer literals, e.g. `42`
  - floating-point literals, e.g. `3.14`
  - boolean literals: `true`, `false`
- Operators:
  - arithmetic: `+`, `-`, `*`, `/`, `%`
  - relational: `<`, `>`, `<=`, `>=`, `==`, `!=`
  - logical: `&&`, `||`, `!`
- Delimiters:
  - `{`, `}`, `(`, `)`, `;`
- Comments:
  - `//` line comments
  - `/* ... */` block comments

Whitespace is ignored.

## Grammar Overview

The compiler accepts programs composed of statements and nested blocks.

```ebnf
program     ::= statement*
statement   ::= declaration
              | assignment
              | if_statement
              | while_statement
              | print_statement
              | block
              | ";"

declaration ::= type ID ("=" expression)? ";"
assignment  ::= ID "=" expression ";"
if_statement ::= "if" "(" expression ")" block ("else" block)?
while_statement ::= "while" "(" expression ")" block
print_statement ::= "print" expression ";"
block       ::= "{" statement* "}"

type        ::= "int" | "float" | "bool"

expression  ::= logical_or
logical_or  ::= logical_and ("||" logical_and)*
logical_and ::= equality ("&&" equality)*
equality    ::= relational (("==" | "!=") relational)*
relational  ::= additive (("<" | ">" | "<=" | ">=") additive)*
additive    ::= multiplicative (("+" | "-") multiplicative)*
multiplicative ::= unary (("*" | "/" | "%") unary)*
unary       ::= ("-" | "!") unary | primary
primary     ::= literal | ID | "(" expression ")"
```

## Notes

- The grammar is left-associative for the binary operators shown with `*`.
- Parentheses control precedence in expressions.
- Nested blocks create nested scopes.
- The semantic pass rejects invalid type usage and scope violations.
