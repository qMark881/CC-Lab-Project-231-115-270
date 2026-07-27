#include "semantic.h"
#include "../util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

static void semantic_error(SemanticContext *ctx, int line, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "Semantic Error: ");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, " at line %d\n", line);
    va_end(args);
    ctx->error_count++;
}

void semantic_init(SemanticContext *ctx) {
    symtab_init(&ctx->table);
    ctx->issues = NULL;
    ctx->error_count = 0;
}

void semantic_destroy(SemanticContext *ctx) {
    symtab_destroy(&ctx->table);
    ReportedIssue *issue = ctx->issues;
    while (issue) {
        ReportedIssue *next = issue->next;
        free(issue->name);
        free(issue);
        issue = next;
    }
    ctx->issues = NULL;
}


static DataType analyze_expr(ASTNode *node, SemanticContext *ctx);

static bool issue_already_reported(SemanticContext *ctx, int kind, const char *name) {
    for (ReportedIssue *issue = ctx->issues; issue; issue = issue->next) {
        if (issue->kind == kind && strcmp(issue->name, name) == 0) {
            return true;
        }
    }
    return false;
}

static void remember_issue(SemanticContext *ctx, int kind, const char *name) {
    ReportedIssue *issue = (ReportedIssue *)xmalloc(sizeof(ReportedIssue));
    issue->name = xstrdup(name);
    issue->kind = kind;
    issue->next = ctx->issues;
    ctx->issues = issue;
}

static DataType lookup_identifier(ASTNode *node, SemanticContext *ctx) {
    Symbol *sym = symtab_lookup_active(&ctx->table, node->text);
    if (sym) {
        node->data_type = sym->type;
        return sym->type;
    }
    Symbol *any = symtab_lookup_any(&ctx->table, node->text);
    if (any) {
        if (!issue_already_reported(ctx, 1, node->text)) {
            remember_issue(ctx, 1, node->text);
            semantic_error(ctx, node->line, "Variable '%s' is out of scope", node->text);
        }
    } else {
        if (!issue_already_reported(ctx, 0, node->text)) {
            remember_issue(ctx, 0, node->text);
            semantic_error(ctx, node->line, "Undeclared variable '%s'", node->text);
        }
    }
    node->data_type = TYPE_ERROR;
    return TYPE_ERROR;
}

static DataType analyze_binary(ASTNode *node, SemanticContext *ctx) {
    DataType left = analyze_expr(node->children[0], ctx);
    DataType right = analyze_expr(node->children[1], ctx);
    const char *op = node->text;

    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 || strcmp(op, "*") == 0 || strcmp(op, "/") == 0 || strcmp(op, "%") == 0) {
        if (!is_numeric_type(left) || !is_numeric_type(right)) {
            semantic_error(ctx, node->line, "Invalid expression: arithmetic operator '%s' requires numeric operands", op);
            node->data_type = TYPE_ERROR;
            return TYPE_ERROR;
        }
        if (strcmp(op, "%") == 0 && (left != TYPE_INT || right != TYPE_INT)) {
            semantic_error(ctx, node->line, "Invalid expression: modulo operator requires integer operands");
            node->data_type = TYPE_ERROR;
            return TYPE_ERROR;
        }
        node->data_type = numeric_result_type(left, right);
        return node->data_type;
    }

    if (strcmp(op, "<") == 0 || strcmp(op, ">") == 0 || strcmp(op, "<=") == 0 || strcmp(op, ">=") == 0) {
        if (!is_numeric_type(left) || !is_numeric_type(right)) {
            semantic_error(ctx, node->line, "Invalid expression: relational operator '%s' requires numeric operands", op);
            node->data_type = TYPE_ERROR;
            return TYPE_ERROR;
        }
        node->data_type = TYPE_BOOL;
        return TYPE_BOOL;
    }

    if (strcmp(op, "==") == 0 || strcmp(op, "!=") == 0) {
        bool both_numeric = is_numeric_type(left) && is_numeric_type(right);
        bool both_bool = left == TYPE_BOOL && right == TYPE_BOOL;
        if (!both_numeric && !both_bool) {
            semantic_error(ctx, node->line, "Invalid expression: equality operator '%s' requires matching operands", op);
            node->data_type = TYPE_ERROR;
            return TYPE_ERROR;
        }
        node->data_type = TYPE_BOOL;
        return TYPE_BOOL;
    }

    if (strcmp(op, "&&") == 0 || strcmp(op, "||") == 0) {
        if (!is_bool_type(left) || !is_bool_type(right)) {
            semantic_error(ctx, node->line, "Invalid expression: logical operator '%s' requires bool operands", op);
            node->data_type = TYPE_ERROR;
            return TYPE_ERROR;
        }
        node->data_type = TYPE_BOOL;
        return TYPE_BOOL;
    }

    semantic_error(ctx, node->line, "Unknown binary operator '%s'", op);
    node->data_type = TYPE_ERROR;
    return TYPE_ERROR;
}

static DataType analyze_unary(ASTNode *node, SemanticContext *ctx) {
    DataType operand = analyze_expr(node->children[0], ctx);
    const char *op = node->text;
    if (strcmp(op, "!") == 0) {
        if (!is_bool_type(operand)) {
            semantic_error(ctx, node->line, "Invalid expression: logical not requires bool operand");
            node->data_type = TYPE_ERROR;
            return TYPE_ERROR;
        }
        node->data_type = TYPE_BOOL;
        return TYPE_BOOL;
    }
    if (strcmp(op, "-") == 0) {
        if (!is_numeric_type(operand)) {
            semantic_error(ctx, node->line, "Invalid expression: unary minus requires numeric operand");
            node->data_type = TYPE_ERROR;
            return TYPE_ERROR;
        }
        node->data_type = operand;
        return operand;
    }
    semantic_error(ctx, node->line, "Unknown unary operator '%s'", op);
    node->data_type = TYPE_ERROR;
    return TYPE_ERROR;
}

static DataType analyze_expr(ASTNode *node, SemanticContext *ctx) {
    if (!node) return TYPE_ERROR;
    switch (node->kind) {
        case NODE_LITERAL:
            return node->data_type;
        case NODE_IDENTIFIER:
            return lookup_identifier(node, ctx);
        case NODE_BINARY:
            return analyze_binary(node, ctx);
        case NODE_UNARY:
            return analyze_unary(node, ctx);
        case NODE_ASSIGN: {
            /* assignment expression is not produced by parser, but kept for robustness */
            return TYPE_ERROR;
        }
        case NODE_BLOCK:
        case NODE_PROGRAM:
        case NODE_IF:
        case NODE_WHILE:
        case NODE_PRINT:
        case NODE_DECL:
        default:
            return TYPE_ERROR;
    }
}

static void analyze_stmt(ASTNode *node, SemanticContext *ctx);

static void analyze_block(ASTNode *node, SemanticContext *ctx) {
    symtab_enter_scope(&ctx->table);
    for (int i = 0; i < node->child_count; ++i) {
        analyze_stmt(node->children[i], ctx);
    }
    symtab_exit_scope(&ctx->table);
}

static void analyze_stmt(ASTNode *node, SemanticContext *ctx) {
    if (!node) return;

    switch (node->kind) {
        case NODE_PROGRAM:
            for (int i = 0; i < node->child_count; ++i) {
                analyze_stmt(node->children[i], ctx);
            }
            break;

        case NODE_BLOCK:
            analyze_block(node, ctx);
            break;

        case NODE_DECL: {
            if (node->child_count < 1 || !node->children[0] || node->children[0]->kind != NODE_IDENTIFIER) {
                semantic_error(ctx, node->line, "Malformed declaration");
                break;
            }
            const char *name = node->children[0]->text;
            DataType declared = node->data_type;
            if (symtab_lookup_current_scope(&ctx->table, name)) {
                semantic_error(ctx, node->line, "Redeclaration of variable '%s'", name);
                break;
            }
            if (node->child_count > 1) {
                DataType init_type = analyze_expr(node->children[1], ctx);
                if (!assignment_compatible(declared, init_type)) {
                    if (declared == TYPE_INT && init_type == TYPE_BOOL) {
                        semantic_error(ctx, node->line, "Cannot assign bool to int");
                    } else {
                        semantic_error(ctx, node->line, "Type mismatch in assignment\nExpected %s\nFound %s",
                                       type_to_string(declared), type_to_string(init_type));
                    }
                }
            }
            symtab_insert(&ctx->table, name, declared, node->line);
            break;
        }

        case NODE_ASSIGN: {
            if (node->child_count != 2 || !node->children[0] || node->children[0]->kind != NODE_IDENTIFIER) {
                semantic_error(ctx, node->line, "Malformed assignment");
                break;
            }
            ASTNode *id = node->children[0];
            ASTNode *rhs = node->children[1];
            Symbol *sym = symtab_lookup_active(&ctx->table, id->text);
            if (!sym) {
                if (symtab_lookup_any(&ctx->table, id->text)) {
                    if (!issue_already_reported(ctx, 1, id->text)) {
                        remember_issue(ctx, 1, id->text);
                        semantic_error(ctx, node->line, "Variable '%s' is out of scope", id->text);
                    }
                } else {
                    if (!issue_already_reported(ctx, 0, id->text)) {
                        remember_issue(ctx, 0, id->text);
                        semantic_error(ctx, node->line, "Undeclared variable '%s'", id->text);
                    }
                }
                break;
            }
            DataType rhs_type = analyze_expr(rhs, ctx);
            if (!assignment_compatible(sym->type, rhs_type)) {
                if (sym->type == TYPE_INT && rhs_type == TYPE_BOOL) {
                    semantic_error(ctx, node->line, "Cannot assign bool to int");
                } else {
                    semantic_error(ctx, node->line, "Type mismatch in assignment\nExpected %s\nFound %s",
                                   type_to_string(sym->type), type_to_string(rhs_type));
                }
            }
            node->data_type = sym->type;
            break;
        }

        case NODE_IF: {
            if (node->child_count < 2) {
                semantic_error(ctx, node->line, "Malformed if statement");
                break;
            }
            DataType cond_type = analyze_expr(node->children[0], ctx);
            if (cond_type != TYPE_BOOL && cond_type != TYPE_ERROR) {
                semantic_error(ctx, node->line, "Condition must be bool");
            }
            analyze_stmt(node->children[1], ctx);
            if (node->child_count > 2) {
                analyze_stmt(node->children[2], ctx);
            }
            break;
        }

        case NODE_WHILE: {
            if (node->child_count < 2) {
                semantic_error(ctx, node->line, "Malformed while statement");
                break;
            }
            DataType cond_type = analyze_expr(node->children[0], ctx);
            if (cond_type != TYPE_BOOL && cond_type != TYPE_ERROR) {
                semantic_error(ctx, node->line, "Condition must be bool");
            }
            analyze_stmt(node->children[1], ctx);
            break;
        }

        case NODE_PRINT:
            if (node->child_count != 1) {
                semantic_error(ctx, node->line, "Malformed print statement");
                break;
            }
            analyze_expr(node->children[0], ctx);
            break;

        default:
            break;
    }
}

void semantic_analyze(ASTNode *root, SemanticContext *ctx) {
    analyze_stmt(root, ctx);
}
