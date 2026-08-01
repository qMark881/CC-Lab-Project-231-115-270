#ifndef PARSER_H
#define PARSER_H

#include "../ast/ast.h"
#include "../lexer/lexer.h"

/* Parser structure that maintains state during syntax analysis.
 * Contains the lexer for token generation, current token being processed,
 * and error count for tracking syntax errors. */
typedef struct Parser {
    Lexer lexer;          /* Lexer instance for token generation */
    Token current;        /* Current token being processed */
    int error_count;      /* Total lexical and syntax errors */
    int lexical_error_count;
    int syntax_error_count;
    int token_count;
    double lexical_time;
    ErrorCode last_error_code; /* Last error code encountered */
} Parser;

/* Initialize the parser with source code */
void parser_init(Parser *parser, const char *source);

/* Clean up parser resources */
void parser_destroy(Parser *parser);

/* Parse the entire program and return the AST root node */
ASTNode *parse_program(Parser *parser);

#endif
