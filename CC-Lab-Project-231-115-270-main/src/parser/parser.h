#ifndef PARSER_H
#define PARSER_H

#include "../ast/ast.h"
#include "../lexer/lexer.h"

typedef struct Parser {
    Lexer lexer;
    Token current;
    int error_count;
} Parser;

void parser_init(Parser *parser, const char *source);
void parser_destroy(Parser *parser);
ASTNode *parse_program(Parser *parser);

#endif
