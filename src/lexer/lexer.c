#include "lexer.h"
#include "../util.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Token make_token(TokenType type, const char *lexeme, DataType literal_type, int line) {
    Token tok;
    tok.type = type;
    tok.lexeme = xstrdup(lexeme ? lexeme : "");
    tok.literal_type = literal_type;
    tok.line = line;
    return tok;
}

static char peek(const Lexer *lexer) {
    if (lexer->pos >= lexer->length) return '\0';
    return lexer->source[lexer->pos];
}

static char peek_next(const Lexer *lexer) {
    if (lexer->pos + 1 >= lexer->length) return '\0';
    return lexer->source[lexer->pos + 1];
}

static char advance(Lexer *lexer) {
    char c = peek(lexer);
    if (c == '\0') return c;
    lexer->pos++;
    if (c == '\n') lexer->line++;
    return c;
}

static void skip_whitespace_and_comments(Lexer *lexer) {
    for (;;) {
        char c = peek(lexer);
        if (c == '\0') return;
        if (isspace((unsigned char)c)) {
            advance(lexer);
            continue;
        }
        if (c == '/' && peek_next(lexer) == '/') {
            while (peek(lexer) != '\0' && peek(lexer) != '\n') {
                advance(lexer);
            }
            continue;
        }
        if (c == '/' && peek_next(lexer) == '*') {
            advance(lexer);
            advance(lexer);
            while (peek(lexer) != '\0') {
                if (peek(lexer) == '*' && peek_next(lexer) == '/') {
                    advance(lexer);
                    advance(lexer);
                    break;
                }
                advance(lexer);
            }
            continue;
        }
        break;
    }
}

void lexer_init(Lexer *lexer, const char *source) {
    lexer->source = source ? source : "";
    lexer->length = strlen(lexer->source);
    lexer->pos = 0;
    lexer->line = 1;
}

static TokenType keyword_type(const char *text) {
    if (strcmp(text, "int") == 0) return TOK_INT;
    if (strcmp(text, "float") == 0) return TOK_FLOAT;
    if (strcmp(text, "bool") == 0) return TOK_BOOL;
    if (strcmp(text, "if") == 0) return TOK_IF;
    if (strcmp(text, "else") == 0) return TOK_ELSE;
    if (strcmp(text, "while") == 0) return TOK_WHILE;
    if (strcmp(text, "print") == 0) return TOK_PRINT;
    if (strcmp(text, "true") == 0) return TOK_TRUE;
    if (strcmp(text, "false") == 0) return TOK_FALSE;
    return TOK_ID;
}

Token lexer_next(Lexer *lexer) {
    skip_whitespace_and_comments(lexer);
    int line = lexer->line;
    char c = peek(lexer);
    if (c == '\0') {
        return make_token(TOK_EOF, "EOF", TYPE_VOID, line);
    }

    if (isalpha((unsigned char)c) || c == '_') {
        size_t start = lexer->pos;
        advance(lexer);
        while (isalnum((unsigned char)peek(lexer)) || peek(lexer) == '_') {
            advance(lexer);
        }
        size_t len = lexer->pos - start;
        char *text = (char *)xmalloc(len + 1);
        memcpy(text, lexer->source + start, len);
        text[len] = '\0';
        TokenType type = keyword_type(text);
        DataType lit_type = TYPE_VOID;
        if (type == TOK_TRUE || type == TOK_FALSE) {
            lit_type = TYPE_BOOL;
        }
        Token tok = make_token(type, text, lit_type, line);
        free(text);
        return tok;
    }

    if (isdigit((unsigned char)c)) {
        size_t start = lexer->pos;
        bool is_float = false;
        while (isdigit((unsigned char)peek(lexer))) {
            advance(lexer);
        }
        if (peek(lexer) == '.' && isdigit((unsigned char)peek_next(lexer))) {
            is_float = true;
            advance(lexer);
            while (isdigit((unsigned char)peek(lexer))) {
                advance(lexer);
            }
        }
        size_t len = lexer->pos - start;
        char *text = (char *)xmalloc(len + 1);
        memcpy(text, lexer->source + start, len);
        text[len] = '\0';
        Token tok = make_token(is_float ? TOK_FLOAT_LITERAL : TOK_INT_LITERAL, text, is_float ? TYPE_FLOAT : TYPE_INT, line);
        free(text);
        return tok;
    }

    advance(lexer);
    switch (c) {
        case '+': return make_token(TOK_PLUS, "+", TYPE_VOID, line);
        case '-': return make_token(TOK_MINUS, "-", TYPE_VOID, line);
        case '*': return make_token(TOK_STAR, "*", TYPE_VOID, line);
        case '/': return make_token(TOK_SLASH, "/", TYPE_VOID, line);
        case '%': return make_token(TOK_PERCENT, "%", TYPE_VOID, line);
        case '(': return make_token(TOK_LPAREN, "(", TYPE_VOID, line);
        case ')': return make_token(TOK_RPAREN, ")", TYPE_VOID, line);
        case '{': return make_token(TOK_LBRACE, "{", TYPE_VOID, line);
        case '}': return make_token(TOK_RBRACE, "}", TYPE_VOID, line);
        case ';': return make_token(TOK_SEMI, ";", TYPE_VOID, line);
        case '!':
            if (peek(lexer) == '=') {
                advance(lexer);
                return make_token(TOK_NEQ, "!=", TYPE_VOID, line);
            }
            return make_token(TOK_NOT, "!", TYPE_VOID, line);
        case '=':
            if (peek(lexer) == '=') {
                advance(lexer);
                return make_token(TOK_EQ, "==", TYPE_VOID, line);
            }
            return make_token(TOK_ASSIGN, "=", TYPE_VOID, line);
        case '<':
            if (peek(lexer) == '=') {
                advance(lexer);
                return make_token(TOK_LE, "<=", TYPE_VOID, line);
            }
            return make_token(TOK_LT, "<", TYPE_VOID, line);
        case '>':
            if (peek(lexer) == '=') {
                advance(lexer);
                return make_token(TOK_GE, ">=", TYPE_VOID, line);
            }
            return make_token(TOK_GT, ">", TYPE_VOID, line);
        case '&':
            if (peek(lexer) == '&') {
                advance(lexer);
                return make_token(TOK_AND, "&&", TYPE_VOID, line);
            }
            break;
        case '|':
            if (peek(lexer) == '|') {
                advance(lexer);
                return make_token(TOK_OR, "||", TYPE_VOID, line);
            }
            break;
        default:
            break;
    }

    char invalid[2] = { c, '\0' };
    return make_token(TOK_INVALID, invalid, TYPE_ERROR, line);
}

void token_free(Token *token) {
    if (!token) return;
    free(token->lexeme);
    token->lexeme = NULL;
}

const char *token_type_name(TokenType type) {
    switch (type) {
        case TOK_EOF: return "EOF";
        case TOK_INT: return "int";
        case TOK_FLOAT: return "float";
        case TOK_BOOL: return "bool";
        case TOK_IF: return "if";
        case TOK_ELSE: return "else";
        case TOK_WHILE: return "while";
        case TOK_PRINT: return "print";
        case TOK_TRUE: return "true";
        case TOK_FALSE: return "false";
        case TOK_ID: return "identifier";
        case TOK_INT_LITERAL: return "int literal";
        case TOK_FLOAT_LITERAL: return "float literal";
        case TOK_PLUS: return "+";
        case TOK_MINUS: return "-";
        case TOK_STAR: return "*";
        case TOK_SLASH: return "/";
        case TOK_PERCENT: return "%";
        case TOK_LT: return "<";
        case TOK_GT: return ">";
        case TOK_LE: return "<=";
        case TOK_GE: return ">=";
        case TOK_EQ: return "==";
        case TOK_NEQ: return "!=";
        case TOK_AND: return "&&";
        case TOK_OR: return "||";
        case TOK_NOT: return "!";
        case TOK_ASSIGN: return "=";
        case TOK_LPAREN: return "(";
        case TOK_RPAREN: return ")";
        case TOK_LBRACE: return "{";
        case TOK_RBRACE: return "}";
        case TOK_SEMI: return ";";
        case TOK_INVALID: return "invalid";
        default: return "unknown";
    }
}
