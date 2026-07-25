#include "ast.h"
#include "../util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_indent(int indent) {
    for (int i = 0; i < indent; ++i) {
        fputs("  ", stdout);
    }
}

const char *ast_kind_name(NodeKind kind) {
    switch (kind) {
        case NODE_PROGRAM: return "Program";
        case NODE_BLOCK: return "Block";
        case NODE_DECL: return "Declaration";
        case NODE_ASSIGN: return "Assignment";
        case NODE_IF: return "If";
        case NODE_WHILE: return "While";
        case NODE_PRINT: return "Print";
        case NODE_BINARY: return "BinaryOp";
        case NODE_UNARY: return "UnaryOp";
        case NODE_LITERAL: return "Literal";
        case NODE_IDENTIFIER: return "Identifier";
        default: return "Unknown";
    }
}

ASTNode *ast_create(NodeKind kind, const char *text, int line) {
    ASTNode *node = (ASTNode *)xmalloc(sizeof(ASTNode));
    node->kind = kind;
    node->text = xstrdup(text ? text : "");
    node->line = line;
    node->data_type = TYPE_VOID;
    node->child_count = 0;
    node->child_cap = 0;
    node->children = NULL;
    return node;
}

ASTNode *ast_add_child(ASTNode *parent, ASTNode *child) {
    if (!parent || !child) {
        return parent;
    }
    if (parent->child_count >= parent->child_cap) {
        parent->child_cap = parent->child_cap ? parent->child_cap * 2 : 4;
        parent->children = (ASTNode **)xrealloc(parent->children, (size_t)parent->child_cap * sizeof(ASTNode *));
    }
    parent->children[parent->child_count++] = child;
    return parent;
}

ASTNode *ast_make_literal(const char *text, DataType type, int line) {
    ASTNode *node = ast_create(NODE_LITERAL, text, line);
    node->data_type = type;
    return node;
}

ASTNode *ast_make_identifier(const char *name, int line) {
    return ast_create(NODE_IDENTIFIER, name, line);
}

static void print_node_label(const ASTNode *node) {
    if (!node) {
        return;
    }

    switch (node->kind) {
        case NODE_DECL:
            printf("%s %s", ast_kind_name(node->kind), node->text);
            break;
        case NODE_BINARY:
        case NODE_UNARY:
            printf("%s %s", ast_kind_name(node->kind), node->text);
            break;
        case NODE_LITERAL:
            printf("%s %s", ast_kind_name(node->kind), node->text);
            if (node->data_type != TYPE_VOID && node->data_type != TYPE_ERROR) {
                printf(" : %s", type_to_string(node->data_type));
            }
            break;
        case NODE_IDENTIFIER:
            printf("%s %s", ast_kind_name(node->kind), node->text);
            break;
        default:
            printf("%s", ast_kind_name(node->kind));
            if (node->text && node->text[0]) {
                printf(" %s", node->text);
            }
            break;
    }
    if (node->data_type != TYPE_VOID && node->kind != NODE_LITERAL) {
        printf(" : %s", type_to_string(node->data_type));
    }
    printf(" [line %d]\n", node->line);
}

void ast_print(const ASTNode *node, int indent) {
    if (!node) return;
    print_indent(indent);
    print_node_label(node);
    for (int i = 0; i < node->child_count; ++i) {
        ast_print(node->children[i], indent + 1);
    }
}

void ast_free(ASTNode *node) {
    if (!node) return;
    for (int i = 0; i < node->child_count; ++i) {
        ast_free(node->children[i]);
    }
    free(node->children);
    free(node->text);
    free(node);
}
