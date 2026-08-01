#include "parser.h"
#include "../util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>

/* Advance to the next token by consuming the current token.
 * Frees the current token and gets the next one from the lexer. */
static Token parser_next_token(Parser *parser) {
    clock_t start = clock();
    Token token = lexer_next(&parser->lexer);
    parser->lexical_time += (double)(clock() - start) / CLOCKS_PER_SEC;
    return token;
}

static void parser_record_token(Parser *parser) {
    if (parser->current.type != TOK_EOF) {
        parser->token_count++;
    }
}

static void parser_advance(Parser *parser) {
    token_free(&parser->current);
    parser->current = parser_next_token(parser);
    parser_record_token(parser);
}

/* Report a syntax error with formatted message and error code.
 * Increments the error count and prints the error to stderr. */
static void parser_error(Parser *parser, int line, ErrorCode code, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "[%s] ", error_code_to_string(code));
    vfprintf(stderr, fmt, args);
    fprintf(stderr, " at line %d: %s\n", line, error_code_description(code));
    va_end(args);
    parser->error_count++;
    parser->syntax_error_count++;
    parser->last_error_code = code;
}

static void parser_lexical_error(Parser *parser) {
    fprintf(stderr, "%s\n", parser->current.lexeme ? parser->current.lexeme : "Lexical error");
    parser->error_count++;
    parser->lexical_error_count++;
    parser->last_error_code = parser->lexer.pending_error_code != ERR_NONE
                                  ? parser->lexer.pending_error_code
                                  : ERR_LEXICAL_INVALID_CHAR;
}

static bool token_starts_statement(TokenType type) {
    return type == TOK_INT || type == TOK_FLOAT || type == TOK_BOOL ||
           type == TOK_ID || type == TOK_IF || type == TOK_WHILE ||
           type == TOK_PRINT || type == TOK_LBRACE;
}

/* Panic-mode recovery: stop at a statement boundary without looping forever. */
static void parser_synchronize(Parser *parser) {
    if (parser->current.type == TOK_EOF || parser->current.type == TOK_RBRACE) return;
    parser_advance(parser);
    while (parser->current.type != TOK_EOF && parser->current.type != TOK_RBRACE) {
        if (parser->current.type == TOK_SEMI) {
            parser_advance(parser);
            return;
        }
        if (token_starts_statement(parser->current.type)) return;
        parser_advance(parser);
    }
}

/* Check if the current token matches the expected type.
 * If it matches, consume the token and return true.
 * Otherwise, return false without consuming. */
static bool parser_match(Parser *parser, TokenType type) {
    if (parser->current.type == type) {
        parser_advance(parser);
        return true;
    }
    return false;
}

/* Expect the current token to be of a specific type.
 * If it matches, consume the token and return true.
 * Otherwise, report an error with the given message and return false. */
static bool parser_expect(Parser *parser, TokenType type, const char *message, ErrorCode code) {
    if (parser_match(parser, type)) {
        return true;
    }
    parser_error(parser, parser->current.line, code, "%s", message);
    return false;
}


/* Convert a token type to the corresponding data type.
 * Used for type checking and semantic analysis. */
static DataType token_to_type(TokenType type) {
    switch (type) {
        case TOK_INT: return TYPE_INT;
        case TOK_FLOAT: return TYPE_FLOAT;
        case TOK_BOOL: return TYPE_BOOL;
        default: return TYPE_ERROR;
    }
}

static ASTNode *parse_expression(Parser *parser);

/* Duplicate a lexeme string for safe storage in AST nodes. */
static char *dup_lexeme(const char *s) {
    return xstrdup(s ? s : "");
}

/* Create a binary expression node with left and right operands. */
static ASTNode *make_binary(const char *op, ASTNode *left, ASTNode *right, int line) {
    ASTNode *node = ast_create(NODE_BINARY, op, line);
    ast_add_child(node, left);
    ast_add_child(node, right);
    return node;
}

/* Create a unary expression node with a single operand. */
static ASTNode *make_unary(const char *op, ASTNode *child, int line) {
    ASTNode *node = ast_create(NODE_UNARY, op, line);
    ast_add_child(node, child);
    return node;
}

/* Parse primary expressions: literals, identifiers, and parenthesized expressions.
 * These are the basic building blocks of the language grammar. */
static ASTNode *parse_primary(Parser *parser) {
    Token tok = parser->current;
    switch (tok.type) {
        case TOK_INT_LITERAL: {
            char *copy = dup_lexeme(tok.lexeme);
            parser_advance(parser);
            ASTNode *node = ast_make_literal(copy, TYPE_INT, tok.line);
            free(copy);
            return node;
        }
        case TOK_FLOAT_LITERAL: {
            char *copy = dup_lexeme(tok.lexeme);
            parser_advance(parser);
            ASTNode *node = ast_make_literal(copy, TYPE_FLOAT, tok.line);
            free(copy);
            return node;
        }
        case TOK_TRUE:
            parser_advance(parser);
            return ast_make_literal("true", TYPE_BOOL, tok.line);
        case TOK_FALSE:
            parser_advance(parser);
            return ast_make_literal("false", TYPE_BOOL, tok.line);
        case TOK_ID: {
            char *copy = dup_lexeme(tok.lexeme);
            parser_advance(parser);
            ASTNode *node = ast_make_identifier(copy, tok.line);
            free(copy);
            return node;
        }
        case TOK_LPAREN: {
            parser_advance(parser);
            ASTNode *expr = parse_expression(parser);
            if (!parser_match(parser, TOK_RPAREN)) {
                parser_error(parser, parser->current.line, ERR_SYNTAX_MISSING_RPAREN, "Missing ')'");
            }
            return expr;
        }
        case TOK_INVALID:
            parser_lexical_error(parser);
            parser_advance(parser);
            return ast_make_literal("0", TYPE_INT, tok.line);
        default:
            parser_error(parser, tok.line, ERR_SYNTAX_UNEXPECTED_TOKEN, "Unexpected token '%s'", tok.lexeme);
            parser_advance(parser);
            return ast_make_literal("0", TYPE_INT, tok.line);
    }
}

/* Parse unary expressions: operators with single operands (- and !).
 * Handles right-associative unary operators recursively. */
static ASTNode *parse_unary(Parser *parser) {
    Token tok = parser->current;
    if (tok.type == TOK_MINUS || tok.type == TOK_NOT) {
        char *op = dup_lexeme(tok.lexeme);
        parser_advance(parser);
        ASTNode *child = parse_unary(parser);
        ASTNode *node = make_unary(op, child, tok.line);
        free(op);
        return node;
    }
    return parse_primary(parser);
}

/* Parse multiplicative expressions: *, /, and % operators.
 * These operators have higher precedence than additive operators. */
static ASTNode *parse_multiplicative(Parser *parser) {
    ASTNode *node = parse_unary(parser);
    while (parser->current.type == TOK_STAR || parser->current.type == TOK_SLASH || parser->current.type == TOK_PERCENT) {
        Token tok = parser->current;
        char *op = dup_lexeme(tok.lexeme);
        parser_advance(parser);
        ASTNode *rhs = parse_unary(parser);
        node = make_binary(op, node, rhs, tok.line);
        free(op);
    }
    return node;
}

/* Parse additive expressions: + and - operators.
 * These operators have lower precedence than multiplicative operators. */
static ASTNode *parse_additive(Parser *parser) {
    ASTNode *node = parse_multiplicative(parser);
    while (parser->current.type == TOK_PLUS || parser->current.type == TOK_MINUS) {
        Token tok = parser->current;
        char *op = dup_lexeme(tok.lexeme);
        parser_advance(parser);
        ASTNode *rhs = parse_multiplicative(parser);
        node = make_binary(op, node, rhs, tok.line);
        free(op);
    }
    return node;
}

/* Parse relational expressions: <, >, <=, and >= operators.
 * These operators have lower precedence than additive operators. */
static ASTNode *parse_relational(Parser *parser) {
    ASTNode *node = parse_additive(parser);
    while (parser->current.type == TOK_LT || parser->current.type == TOK_GT ||
           parser->current.type == TOK_LE || parser->current.type == TOK_GE) {
        Token tok = parser->current;
        char *op = dup_lexeme(tok.lexeme);
        parser_advance(parser);
        ASTNode *rhs = parse_additive(parser);
        node = make_binary(op, node, rhs, tok.line);
        free(op);
    }
    return node;
}

/* Parse equality expressions: == and != operators.
 * These operators have lower precedence than relational operators. */
static ASTNode *parse_equality(Parser *parser) {
    ASTNode *node = parse_relational(parser);
    while (parser->current.type == TOK_EQ || parser->current.type == TOK_NEQ) {
        Token tok = parser->current;
        char *op = dup_lexeme(tok.lexeme);
        parser_advance(parser);
        ASTNode *rhs = parse_relational(parser);
        node = make_binary(op, node, rhs, tok.line);
        free(op);
    }
    return node;
}

/* Parse logical AND expressions: && operator.
 * This operator has lower precedence than equality operators. */
static ASTNode *parse_logical_and(Parser *parser) {
    ASTNode *node = parse_equality(parser);
    while (parser->current.type == TOK_AND) {
        Token tok = parser->current;
        char *op = dup_lexeme(tok.lexeme);
        parser_advance(parser);
        ASTNode *rhs = parse_equality(parser);
        node = make_binary(op, node, rhs, tok.line);
        free(op);
    }
    return node;
}

/* Parse logical OR expressions: || operator.
 * This operator has lower precedence than logical AND operator. */
static ASTNode *parse_logical_or(Parser *parser) {
    ASTNode *node = parse_logical_and(parser);
    while (parser->current.type == TOK_OR) {
        Token tok = parser->current;
        char *op = dup_lexeme(tok.lexeme);
        parser_advance(parser);
        ASTNode *rhs = parse_logical_and(parser);
        node = make_binary(op, node, rhs, tok.line);
        free(op);
    }
    return node;
}

/* Parse a complete expression.
 * This is the entry point for expression parsing, handling all operator precedence levels. */
static ASTNode *parse_expression(Parser *parser) {
    return parse_logical_or(parser);
}

static ASTNode *parse_block(Parser *parser);

static ASTNode *parse_statement(Parser *parser);

/* Parse variable declarations with optional initialization.
 * Handles declarations like: int x; or float y = 3.14; */
static ASTNode *parse_decl(Parser *parser) {
    Token type_tok = parser->current;
    DataType decl_type = token_to_type(type_tok.type);
    parser_advance(parser);

    if (parser->current.type != TOK_ID) {
        parser_error(parser, parser->current.line, ERR_SYNTAX_EXPECTED_IDENTIFIER, "Expected identifier after type");
        parser_synchronize(parser);
        return NULL;
    }

    Token id_tok = parser->current;
    char *id_name = dup_lexeme(id_tok.lexeme);
    parser_advance(parser);

    ASTNode *decl = ast_create(NODE_DECL, type_to_string(decl_type), type_tok.line);
    decl->data_type = decl_type;
    ASTNode *id = ast_make_identifier(id_name, id_tok.line);
    free(id_name);
    ast_add_child(decl, id);

    if (parser_match(parser, TOK_ASSIGN)) {
        ASTNode *init = parse_expression(parser);
        ast_add_child(decl, init);
    }

    if (!parser_expect(parser, TOK_SEMI, "Missing ';'", ERR_SYNTAX_MISSING_SEMICOLON)) {
        /* Recover by leaving current token untouched if it's already the next statement. */
    }

    return decl;
}

/* Parse assignment statements: identifier = expression;
 * Handles variable assignments like: x = 10; */
static ASTNode *parse_assignment(Parser *parser) {
    Token id_tok = parser->current;
    char *id_name = dup_lexeme(id_tok.lexeme);
    parser_advance(parser);

    ASTNode *assign = ast_create(NODE_ASSIGN, "=", id_tok.line);
    ASTNode *id = ast_make_identifier(id_name, id_tok.line);
    free(id_name);
    ast_add_child(assign, id);

    if (!parser_match(parser, TOK_ASSIGN)) {
        parser_error(parser, parser->current.line, ERR_SYNTAX_MISSING_ASSIGN, "Missing '=' in assignment");
        return assign;
    }

    ASTNode *rhs = parse_expression(parser);
    ast_add_child(assign, rhs);

    if (!parser_expect(parser, TOK_SEMI, "Missing ';'", ERR_SYNTAX_MISSING_SEMICOLON)) {
        /* recovery */
    }

    return assign;
}

/* Parse print statements: print expression;
 * Handles output statements like: print x; */
static ASTNode *parse_print(Parser *parser) {
    Token kw = parser->current;
    parser_advance(parser);

    ASTNode *node = ast_create(NODE_PRINT, "print", kw.line);
    ASTNode *expr = parse_expression(parser);
    ast_add_child(node, expr);

    if (!parser_expect(parser, TOK_SEMI, "Missing ';'", ERR_SYNTAX_MISSING_SEMICOLON)) {
        /* recovery */
    }

    return node;
}

/* Parse if and if-else statements.
 * Handles conditional statements with optional else branch. */
static ASTNode *parse_if(Parser *parser) {
    Token kw = parser->current;
    parser_advance(parser);

    ASTNode *node = ast_create(NODE_IF, "if", kw.line);

    if (!parser_expect(parser, TOK_LPAREN, "Missing '('", ERR_SYNTAX_MISSING_LPAREN)) {
        /* continue */
    }
    ASTNode *cond = parse_expression(parser);
    ast_add_child(node, cond);

    if (!parser_match(parser, TOK_RPAREN)) {
        parser_error(parser, parser->current.line, ERR_SYNTAX_MISSING_RPAREN, "Missing ')'");
    }

    ASTNode *then_block = parse_block(parser);
    ast_add_child(node, then_block);

    if (parser_match(parser, TOK_ELSE)) {
        ASTNode *else_block = parse_block(parser);
        ast_add_child(node, else_block);
    }

    return node;
}

/* Parse while loop statements.
 * Handles iteration statements with condition checking. */
static ASTNode *parse_while(Parser *parser) {
    Token kw = parser->current;
    parser_advance(parser);

    ASTNode *node = ast_create(NODE_WHILE, "while", kw.line);

    if (!parser_expect(parser, TOK_LPAREN, "Missing '('", ERR_SYNTAX_MISSING_LPAREN)) {
        /* continue */
    }
    ASTNode *cond = parse_expression(parser);
    ast_add_child(node, cond);

    if (!parser_match(parser, TOK_RPAREN)) {
        parser_error(parser, parser->current.line, ERR_SYNTAX_MISSING_RPAREN, "Missing ')'");
    }

    ASTNode *body = parse_block(parser);
    ast_add_child(node, body);
    return node;
}

/* Parse a block of statements enclosed in braces.
 * Creates a block node containing multiple statements. */
static ASTNode *parse_block(Parser *parser) {
    int line = parser->current.line;
    ASTNode *block = ast_create(NODE_BLOCK, "block", line);
    if (!parser_match(parser, TOK_LBRACE)) {
        parser_error(parser, line, ERR_SYNTAX_MISSING_LBRACE, "Missing '{'");
        return block;
    }

    while (parser->current.type != TOK_RBRACE && parser->current.type != TOK_EOF) {
        ASTNode *stmt = parse_statement(parser);
        if (stmt) {
            ast_add_child(block, stmt);
        } else if (parser->current.type == TOK_EOF) {
            break;
        } else if (parser->current.type == TOK_RBRACE) {
            break;
        } else {
            /* make sure progress is possible */
            if (parser->current.type != TOK_INVALID) {
                /* current token was not consumed by the failed statement */
                parser_error(parser, parser->current.line, ERR_SYNTAX_UNEXPECTED_TOKEN, "Unexpected token '%s'", parser->current.lexeme);
                parser_advance(parser);
            }
        }
    }

    if (!parser_match(parser, TOK_RBRACE)) {
        parser_error(parser, parser->current.line, ERR_SYNTAX_MISSING_RBRACE, "Missing '}'");
    }

    return block;
}

/* Parse a single statement based on the current token type.
 * Dispatches to the appropriate parsing function for each statement type. */
static ASTNode *parse_statement(Parser *parser) {
    switch (parser->current.type) {
        case TOK_INT:
        case TOK_FLOAT:
        case TOK_BOOL:
            return parse_decl(parser);

        case TOK_ID:
            return parse_assignment(parser);

        case TOK_IF:
            return parse_if(parser);

        case TOK_WHILE:
            return parse_while(parser);

        case TOK_PRINT:
            return parse_print(parser);

        case TOK_LBRACE:
            return parse_block(parser);

        case TOK_INVALID:
            parser_lexical_error(parser);
            parser_advance(parser);
            return NULL;

        case TOK_SEMI:
            parser_advance(parser);
            return NULL;

        default:
            parser_error(parser, parser->current.line, ERR_SYNTAX_UNEXPECTED_TOKEN, "Unexpected token '%s'", parser->current.lexeme);
            parser_advance(parser);
            return NULL;
    }
}

/* Initialize the parser with source code.
 * Sets up the lexer and gets the first token. */
void parser_init(Parser *parser, const char *source) {
    lexer_init(&parser->lexer, source);
    parser->error_count = 0;
    parser->lexical_error_count = 0;
    parser->syntax_error_count = 0;
    parser->token_count = 0;
    parser->last_error_code = ERR_NONE;
    parser->current = parser_next_token(parser);
    parser_record_token(parser);
}

/* Clean up parser resources by freeing the current token. */
void parser_destroy(Parser *parser) {
    token_free(&parser->current);
    lexer_destroy(&parser->lexer);
}

/* Parse the entire program by parsing statements until EOF.
 * Returns the root of the AST representing the complete program. */
ASTNode *parse_program(Parser *parser) {
    ASTNode *program = ast_create(NODE_PROGRAM, "program", 1);
    while (parser->current.type != TOK_EOF) {
        size_t position_before = parser->lexer.pos;
        TokenType token_before = parser->current.type;
        ASTNode *stmt = parse_statement(parser);
        if (stmt) {
            ast_add_child(program, stmt);
        }
        if (parser->current.type != TOK_EOF &&
            parser->lexer.pos == position_before && parser->current.type == token_before) {
            parser_error(parser, parser->current.line, ERR_SYNTAX_UNEXPECTED_TOKEN,
                         "Parser recovery skipped token '%s'", parser->current.lexeme);
            parser_advance(parser);
        }
    }
    return program;
}
