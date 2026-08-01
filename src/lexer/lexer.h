#ifndef LEXER_H
#define LEXER_H

#include "../common.h"
#include <stddef.h>

/* Enumeration of all token types recognized by the lexer.
 * These represent the fundamental lexical units of the language. */
typedef enum {
    TOK_EOF = 0,          /* End of file marker */
    TOK_INT,              /* 'int' keyword */
    TOK_FLOAT,            /* 'float' keyword */
    TOK_BOOL,             /* 'bool' keyword */
    TOK_IF,               /* 'if' keyword */
    TOK_ELSE,             /* 'else' keyword */
    TOK_WHILE,            /* 'while' keyword */
    TOK_PRINT,            /* 'print' keyword */
    TOK_TRUE,             /* 'true' boolean literal */
    TOK_FALSE,            /* 'false' boolean literal */
    TOK_ID,               /* Identifier (variable name) */
    TOK_INT_LITERAL,      /* Integer literal */
    TOK_FLOAT_LITERAL,    /* Floating-point literal */
    TOK_PLUS,             /* '+' operator */
    TOK_MINUS,            /* '-' operator */
    TOK_STAR,             /* '*' operator */
    TOK_SLASH,            /* '/' operator */
    TOK_PERCENT,          /* '%' operator */
    TOK_LT,               /* '<' operator */
    TOK_GT,               /* '>' operator */
    TOK_LE,               /* '<=' operator */
    TOK_GE,               /* '>=' operator */
    TOK_EQ,               /* '==' operator */
    TOK_NEQ,              /* '!=' operator */
    TOK_AND,              /* '&&' operator */
    TOK_OR,               /* '||' operator */
    TOK_NOT,              /* '!' operator */
    TOK_ASSIGN,           /* '=' assignment operator */
    TOK_LPAREN,           /* '(' left parenthesis */
    TOK_RPAREN,           /* ')' right parenthesis */
    TOK_LBRACE,           /* '{' left brace */
    TOK_RBRACE,           /* '}' right brace */
    TOK_SEMI,             /* ';' semicolon */
    TOK_INVALID           /* Invalid token/error */
} TokenType;

/* Token structure representing a single lexical unit.
 * Contains the token type, its text representation, literal type (if applicable),
 * and the line number where it appears in the source code. */
typedef struct Token {
    TokenType type;           /* The type of this token */
    char *lexeme;             /* The actual text of the token */
    DataType literal_type;    /* Type of literal if this is a literal token */
    int line;                 /* Line number where token appears */
} Token;

/* Lexer structure that maintains the state during lexical analysis.
 * Tracks position in source code, line/column numbers for error reporting,
 * and pending error state for deferred error reporting. */
typedef struct Lexer {
    const char *source;           /* Source code being lexed */
    size_t length;                /* Length of source code */
    size_t pos;                   /* Current position in source */
    int line;                     /* Current line number */
    int column;                   /* Current column number */
    bool has_pending_error;       /* Flag for pending error */
    char *pending_error;          /* Pending error message */
    int pending_error_line;       /* Line number of pending error */
    int pending_error_column;     /* Column number of pending error */
    ErrorCode pending_error_code;  /* Error code for pending error */
    char *last_comment;           /* Last comment encountered */
    int last_comment_line;         /* Line number of last comment */
} Lexer;

/* Initialize the lexer with source code */
void lexer_init(Lexer *lexer, const char *source);

/* Release any internal lexer-owned buffers. */
void lexer_destroy(Lexer *lexer);

/* Get the next token from the source code */
Token lexer_next(Lexer *lexer);

/* Free memory allocated for a token */
void token_free(Token *token);

/* Get human-readable name for a token type */
const char *token_type_name(TokenType type);

/* Get the last comment encountered during lexing */
const char *lexer_get_last_comment(Lexer *lexer);

/* Get the line number of the last comment */
int lexer_get_last_comment_line(Lexer *lexer);

/* Clear the last comment (after it's been used) */
void lexer_clear_last_comment(Lexer *lexer);

#endif
