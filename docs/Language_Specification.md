# Language Specification

This document summarizes the source language implemented by the compiler and provides a formal grammar that matches the project manual.

## 1. Lexical Categories

### Keywords
`int`, `float`, `bool`, `if`, `else`, `while`, `print`, `true`, `false`

### Identifiers
Identifiers must begin with a letter or underscore, followed by letters, digits, or underscores.

### Literals
- integer literals: `42`
- floating-point literals: `3.14`
- boolean literals: `true`, `false`

### Operators
- Arithmetic: `+ - * / %`
- Relational: `< > <= >= == !=`
- Logical: `&& || !`
- Assignment: `=`

### Delimiters
`{ } ( ) ;`

Comments and whitespace are ignored by the lexer.

## 2. EBNF Grammar

```ebnf
program         ::= { statement } EOF

statement       ::= declaration
                  | assignment
                  | if_statement
                  | while_statement
                  | print_statement
                  | block
                  | ";"

declaration     ::= type identifier [ "=" expression ] ";"
assignment      ::= identifier "=" expression ";"
if_statement    ::= "if" "(" expression ")" block [ "else" block ]
while_statement ::= "while" "(" expression ")" block
print_statement ::= "print" expression ";"
block           ::= "{" { statement } "}"

type            ::= "int" | "float" | "bool"

expression      ::= logical_or

logical_or      ::= logical_and { "||" logical_and }
logical_and     ::= equality { "&&" equality }
equality        ::= relational { ("==" | "!=") relational }
relational      ::= additive { ("<" | ">" | "<=" | ">=") additive }
additive        ::= multiplicative { ("+" | "-") multiplicative }
multiplicative  ::= unary { ("*" | "/" | "%") unary }
unary           ::= ("!" | "-") unary
                  | primary
primary         ::= identifier
                  | literal
                  | "(" expression ")"
literal         ::= integer_literal | float_literal | boolean_literal
```

## 3. Operator Precedence

From highest to lowest:
1. parenthesized expressions
2. unary operators `!` and `-`
3. multiplicative operators `* / %`
4. additive operators `+ -`
5. relational operators `< > <= >=`
6. equality operators `== !=`
7. logical `&&`
8. logical `||`

## 4. Scope Rules

- The program starts in the global scope.
- Every block introduces a nested scope.
- Identifiers declared in a block are active only inside that block and its nested child blocks.
- Redeclaration in the same scope is rejected.

## 5. Semantic Rules

- Arithmetic operators require numeric operands.
- `%` requires integer operands.
- Relational operators require numeric operands.
- `&&` and `||` require boolean operands.
- `==` and `!=` require operands of compatible types.
- `if` and `while` conditions must be boolean.
- Assignments must respect the declared type of the target variable.

## 6. Example Program

```c
int x;
int y;
bool done;

x = 10;
y = 0;
done = false;

while (x > 0) {
    y = y + x;
    x = x - 1;
}

if (done == false) {
    print y;
}
```
