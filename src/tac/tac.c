#include "tac.h"
#include "../util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

static void tac_emit(TacProgram *program, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buffer[512];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    if (program->count >= program->cap) {
        program->cap = program->cap ? program->cap * 2 : 32;
        program->lines = (char **)xrealloc(program->lines, program->cap * sizeof(char *));
    }
    program->lines[program->count++] = xstrdup(buffer);
}

static char *new_temp(TacProgram *program) {
    char buf[32];
    snprintf(buf, sizeof(buf), "t%d", ++program->temp_counter);
    return xstrdup(buf);
}

static char *new_label(TacProgram *program) {
    char buf[32];
    snprintf(buf, sizeof(buf), "L%d", ++program->label_counter);
    return xstrdup(buf);
}

void tac_init(TacProgram *program) {
    program->lines = NULL;
    program->count = 0;
    program->cap = 0;
    program->temp_counter = 0;
    program->label_counter = 0;
}

void tac_free(TacProgram *program) {
    for (size_t i = 0; i < program->count; ++i) {
        free(program->lines[i]);
    }
    free(program->lines);
    program->lines = NULL;
    program->count = 0;
    program->cap = 0;
}

void tac_print(const TacProgram *program) {
    for (size_t i = 0; i < program->count; ++i) {
        puts(program->lines[i]);
    }
}

static char *gen_expr(ASTNode *node, TacProgram *program);

static char *clone_text(const char *text) {
    return xstrdup(text ? text : "");
}

static char *gen_expr(ASTNode *node, TacProgram *program) {
    if (!node) return xstrdup("");
    switch (node->kind) {
        case NODE_LITERAL:
        case NODE_IDENTIFIER:
            return clone_text(node->text);

        case NODE_UNARY: {
            char *operand = gen_expr(node->children[0], program);
            char *temp = new_temp(program);
            tac_emit(program, "%s = %s%s", temp, node->text, operand);
            free(operand);
            return temp;
        }

        case NODE_BINARY: {
            char *left = gen_expr(node->children[0], program);
            char *right = gen_expr(node->children[1], program);
            char *temp = new_temp(program);
            tac_emit(program, "%s = %s %s %s", temp, left, node->text, right);
            free(left);
            free(right);
            return temp;
        }

        default:
            return xstrdup("");
    }
}

static void gen_stmt(ASTNode *node, TacProgram *program);

static void gen_block(ASTNode *node, TacProgram *program) {
    if (!node) return;
    for (int i = 0; i < node->child_count; ++i) {
        gen_stmt(node->children[i], program);
    }
}

static void gen_stmt(ASTNode *node, TacProgram *program) {
    if (!node) return;

    switch (node->kind) {
        case NODE_PROGRAM:
        case NODE_BLOCK:
            gen_block(node, program);
            break;

        case NODE_DECL:
            if (node->child_count > 1) {
                ASTNode *id = node->children[0];
                ASTNode *init = node->children[1];
                char *rhs = gen_expr(init, program);
                tac_emit(program, "%s = %s", id->text, rhs);
                free(rhs);
            }
            break;

        case NODE_ASSIGN: {
            ASTNode *id = node->children[0];
            ASTNode *rhs_node = node->children[1];
            char *rhs = gen_expr(rhs_node, program);
            tac_emit(program, "%s = %s", id->text, rhs);
            free(rhs);
            break;
        }

        case NODE_PRINT: {
            char *value = gen_expr(node->children[0], program);
            tac_emit(program, "print %s", value);
            free(value);
            break;
        }

        case NODE_IF: {
            char *cond = gen_expr(node->children[0], program);
            char *else_label = new_label(program);
            char *end_label = new_label(program);
            if (node->child_count > 2) {
                tac_emit(program, "ifFalse %s goto %s", cond, else_label);
                gen_stmt(node->children[1], program);
                tac_emit(program, "goto %s", end_label);
                tac_emit(program, "%s:", else_label);
                gen_stmt(node->children[2], program);
                tac_emit(program, "%s:", end_label);
            } else {
                tac_emit(program, "ifFalse %s goto %s", cond, end_label);
                gen_stmt(node->children[1], program);
                tac_emit(program, "%s:", end_label);
            }
            free(cond);
            free(else_label);
            free(end_label);
            break;
        }

        case NODE_WHILE: {
            char *start_label = new_label(program);
            char *end_label = new_label(program);
            tac_emit(program, "%s:", start_label);
            char *cond = gen_expr(node->children[0], program);
            tac_emit(program, "ifFalse %s goto %s", cond, end_label);
            gen_stmt(node->children[1], program);
            tac_emit(program, "goto %s", start_label);
            tac_emit(program, "%s:", end_label);
            free(cond);
            free(start_label);
            free(end_label);
            break;
        }

        default:
            break;
    }
}

void tac_generate(ASTNode *root, TacProgram *program) {
    gen_stmt(root, program);
}
