#ifndef LEXER_H
#define LEXER_H

#include "../common.h"
#include <stddef.h>

typedef enum {
    TOK_EOF = 0,
    TOK_INT,
    TOK_FLOAT,
    TOK_BOOL,
    TOK_IF,
    TOK_ELSE,
    TOK_WHILE,
    TOK_PRINT,
    TOK_TRUE,
    TOK_FALSE,
    TOK_ID,
    TOK_INT_LITERAL,
    TOK_FLOAT_LITERAL,
    TOK_PLUS,
    TOK_MINUS,
    TOK_STAR,
    TOK_SLASH,
    TOK_PERCENT,
    TOK_LT,
    TOK_GT,
    TOK_LE,
    TOK_GE,
    TOK_EQ,
    TOK_NEQ,
    TOK_AND,
    TOK_OR,
    TOK_NOT,
    TOK_ASSIGN,
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_SEMI,
    TOK_INVALID
} TokenType;

typedef struct Token {
    TokenType type;
    char *lexeme;
    DataType literal_type;
    int line;
} Token;

typedef struct Lexer {
    const char *source;
    size_t length;
    size_t pos;
    int line;
    int column;
    bool has_pending_error;
    char *pending_error;
    int pending_error_line;
    int pending_error_column;
} Lexer;

void lexer_init(Lexer *lexer, const char *source);
Token lexer_next(Lexer *lexer);
void token_free(Token *token);
const char *token_type_name(TokenType type);

#endif
