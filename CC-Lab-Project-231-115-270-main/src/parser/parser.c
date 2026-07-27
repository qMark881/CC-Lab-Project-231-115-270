#include "parser.h"
#include "../util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

static void parser_advance(Parser *parser) {
    token_free(&parser->current);
    parser->current = lexer_next(&parser->lexer);
}

static void parser_error(Parser *parser, int line, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "Syntax Error: ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, " at line %d\n", line);
    va_end(args);
    parser->error_count++;
}

static bool parser_match(Parser *parser, TokenType type) {
    if (parser->current.type == type) {
        parser_advance(parser);
        return true;
    }
    return false;
}

static bool parser_expect(Parser *parser, TokenType type, const char *message) {
    if (parser_match(parser, type)) {
        return true;
    }
    parser_error(parser, parser->current.line, "%s", message);
    return false;
}


static DataType token_to_type(TokenType type) {
    switch (type) {
        case TOK_INT: return TYPE_INT;
        case TOK_FLOAT: return TYPE_FLOAT;
        case TOK_BOOL: return TYPE_BOOL;
        default: return TYPE_ERROR;
    }
}

static ASTNode *parse_expression(Parser *parser);

static char *dup_lexeme(const char *s) {
    return xstrdup(s ? s : "");
}

static ASTNode *make_binary(const char *op, ASTNode *left, ASTNode *right, int line) {
    ASTNode *node = ast_create(NODE_BINARY, op, line);
    ast_add_child(node, left);
    ast_add_child(node, right);
    return node;
}

static ASTNode *make_unary(const char *op, ASTNode *child, int line) {
    ASTNode *node = ast_create(NODE_UNARY, op, line);
    ast_add_child(node, child);
    return node;
}

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
                parser_error(parser, parser->current.line, "Missing ')'");
            }
            return expr;
        }
        case TOK_INVALID:
            fprintf(stderr, "Lexical Error: Invalid token '%s' at line %d\n", tok.lexeme, tok.line);
            parser->error_count++;
            parser_advance(parser);
            return ast_make_literal("0", TYPE_INT, tok.line);
        default:
            parser_error(parser, tok.line, "Unexpected token '%s'", tok.lexeme);
            parser_advance(parser);
            return ast_make_literal("0", TYPE_INT, tok.line);
    }
}

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

static ASTNode *parse_expression(Parser *parser) {
    return parse_logical_or(parser);
}

static ASTNode *parse_block(Parser *parser);

static ASTNode *parse_statement(Parser *parser);

static ASTNode *parse_decl(Parser *parser) {
    Token type_tok = parser->current;
    DataType decl_type = token_to_type(type_tok.type);
    parser_advance(parser);

    if (parser->current.type != TOK_ID) {
        parser_error(parser, parser->current.line, "Expected identifier after type");
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

    if (!parser_expect(parser, TOK_SEMI, "Missing ';'")) {
        /* Recover by leaving current token untouched if it's already the next statement. */
    }

    return decl;
}

static ASTNode *parse_assignment(Parser *parser) {
    Token id_tok = parser->current;
    char *id_name = dup_lexeme(id_tok.lexeme);
    parser_advance(parser);

    ASTNode *assign = ast_create(NODE_ASSIGN, "=", id_tok.line);
    ASTNode *id = ast_make_identifier(id_name, id_tok.line);
    free(id_name);
    ast_add_child(assign, id);

    if (!parser_match(parser, TOK_ASSIGN)) {
        parser_error(parser, parser->current.line, "Missing '=' in assignment");
        return assign;
    }

    ASTNode *rhs = parse_expression(parser);
    ast_add_child(assign, rhs);

    if (!parser_expect(parser, TOK_SEMI, "Missing ';'")) {
        /* recovery */
    }

    return assign;
}

static ASTNode *parse_print(Parser *parser) {
    Token kw = parser->current;
    parser_advance(parser);

    ASTNode *node = ast_create(NODE_PRINT, "print", kw.line);
    ASTNode *expr = parse_expression(parser);
    ast_add_child(node, expr);

    if (!parser_expect(parser, TOK_SEMI, "Missing ';'")) {
        /* recovery */
    }

    return node;
}

static ASTNode *parse_if(Parser *parser) {
    Token kw = parser->current;
    parser_advance(parser);

    ASTNode *node = ast_create(NODE_IF, "if", kw.line);

    if (!parser_expect(parser, TOK_LPAREN, "Missing '('")) {
        /* continue */
    }
    ASTNode *cond = parse_expression(parser);
    ast_add_child(node, cond);

    if (!parser_match(parser, TOK_RPAREN)) {
        parser_error(parser, parser->current.line, "Missing ')'");
    }

    ASTNode *then_block = parse_block(parser);
    ast_add_child(node, then_block);

    if (parser_match(parser, TOK_ELSE)) {
        ASTNode *else_block = parse_block(parser);
        ast_add_child(node, else_block);
    }

    return node;
}

static ASTNode *parse_while(Parser *parser) {
    Token kw = parser->current;
    parser_advance(parser);

    ASTNode *node = ast_create(NODE_WHILE, "while", kw.line);

    if (!parser_expect(parser, TOK_LPAREN, "Missing '('")) {
        /* continue */
    }
    ASTNode *cond = parse_expression(parser);
    ast_add_child(node, cond);

    if (!parser_match(parser, TOK_RPAREN)) {
        parser_error(parser, parser->current.line, "Missing ')'");
    }

    ASTNode *body = parse_block(parser);
    ast_add_child(node, body);
    return node;
}

static ASTNode *parse_block(Parser *parser) {
    int line = parser->current.line;
    ASTNode *block = ast_create(NODE_BLOCK, "block", line);
    if (!parser_match(parser, TOK_LBRACE)) {
        parser_error(parser, line, "Missing '{'");
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
                parser_error(parser, parser->current.line, "Unexpected token '%s'", parser->current.lexeme);
                parser_advance(parser);
            }
        }
    }

    if (!parser_match(parser, TOK_RBRACE)) {
        parser_error(parser, parser->current.line, "Missing '}'");
    }

    return block;
}

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
            fprintf(stderr, "Lexical Error: Invalid token '%s' at line %d\n", parser->current.lexeme, parser->current.line);
            parser->error_count++;
            parser_advance(parser);
            return NULL;

        case TOK_SEMI:
            parser_advance(parser);
            return NULL;

        default:
            parser_error(parser, parser->current.line, "Unexpected token '%s'", parser->current.lexeme);
            parser_advance(parser);
            return NULL;
    }
}

void parser_init(Parser *parser, const char *source) {
    lexer_init(&parser->lexer, source);
    parser->current = lexer_next(&parser->lexer);
    parser->error_count = 0;
}

void parser_destroy(Parser *parser) {
    token_free(&parser->current);
}

ASTNode *parse_program(Parser *parser) {
    ASTNode *program = ast_create(NODE_PROGRAM, "program", 1);
    while (parser->current.type != TOK_EOF) {
        ASTNode *stmt = parse_statement(parser);
        if (stmt) {
            ast_add_child(program, stmt);
        }
    }
    return program;
}
