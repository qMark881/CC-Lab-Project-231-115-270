#include "ast.h"
#include "../util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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
    node->comment = NULL;
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

/* Set a comment for an AST node */
void ast_set_comment(ASTNode *node, const char *comment) {
    if (!node) return;
    if (node->comment) {
        free(node->comment);
    }
    node->comment = xstrdup(comment);
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
            /* Show resolved type for identifiers */
            if (node->data_type != TYPE_VOID && node->data_type != TYPE_ERROR) {
                printf(" : %s", type_to_string(node->data_type));
            }
            break;
        default:
            printf("%s", ast_kind_name(node->kind));
            if (node->text && node->text[0]) {
                printf(" %s", node->text);
            }
            break;
    }
    if (node->data_type != TYPE_VOID && node->kind != NODE_LITERAL && node->kind != NODE_IDENTIFIER) {
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

static void print_visual_label(const ASTNode *node) {
    if (!node) return;

    printf("%s", ast_kind_name(node->kind));
    if (node->text && node->text[0]) {
        switch (node->kind) {
            case NODE_PROGRAM:
            case NODE_BLOCK:
                break;
            case NODE_DECL:
            case NODE_ASSIGN:
            case NODE_IF:
            case NODE_WHILE:
            case NODE_PRINT:
            case NODE_BINARY:
            case NODE_UNARY:
            case NODE_LITERAL:
            case NODE_IDENTIFIER:
                printf(" %s", node->text);
                break;
            default:
                if (node->text[0]) {
                    printf(" %s", node->text);
                }
                break;
        }
    }

    /* Enhanced type information display */
    if (node->data_type != TYPE_VOID && node->data_type != TYPE_ERROR && node->kind != NODE_PROGRAM && node->kind != NODE_BLOCK) {
        printf(" : %s", type_to_string(node->data_type));
    }
    
    /* Show line number for better debugging */
    if (node->kind != NODE_PROGRAM && node->kind != NODE_BLOCK) {
        printf(" (line %d)", node->line);
    }
}

void ast_print_visual(const ASTNode *node, const char *prefix, bool is_last) {
    if (!node) return;

    printf("%s", prefix ? prefix : "");
    printf(is_last ? "'-- " : "|-- ");
    print_visual_label(node);
    printf("\n");

    char new_prefix[256];
    snprintf(new_prefix, sizeof(new_prefix), "%s%s", prefix ? prefix : "", is_last ? "    " : "|   ");

    for (int i = 0; i < node->child_count; ++i) {
        ast_print_visual(node->children[i], new_prefix, i == node->child_count - 1);
    }
}

static bool ast_is_expression_node(const ASTNode *node) {
    return node &&
           (node->kind == NODE_LITERAL ||
            node->kind == NODE_IDENTIFIER ||
            node->kind == NODE_BINARY ||
            node->kind == NODE_UNARY);
}

static void ast_print_expression_internal(const ASTNode *node) {
    if (!node) {
        return;
    }

    if (!ast_is_expression_node(node)) {
        if (node->text && node->text[0]) {
            fputs(node->text, stdout);
        } else {
            fputs(ast_kind_name(node->kind), stdout);
        }
        return;
    }

    switch (node->kind) {
        case NODE_LITERAL:
        case NODE_IDENTIFIER:
            fputs(node->text, stdout);
            break;

        case NODE_UNARY:
            printf("(%s ", node->text);
            ast_print_expression_internal(node->child_count > 0 ? node->children[0] : NULL);
            printf(")");
            break;

        case NODE_BINARY:
            printf("(");
            ast_print_expression_internal(node->child_count > 0 ? node->children[0] : NULL);
            printf(" %s ", node->text);
            ast_print_expression_internal(node->child_count > 1 ? node->children[1] : NULL);
            printf(")");
            break;

        default:
            break;
    }
}

void ast_print_expression(const ASTNode *node) {
    ast_print_expression_internal(node);
}

static void ast_print_summary_node(const ASTNode *node, int indent) {
    if (!node) {
        return;
    }

    switch (node->kind) {
        case NODE_PROGRAM:
            print_indent(indent);
            puts("Program");
            for (int i = 0; i < node->child_count; ++i) {
                ast_print_summary_node(node->children[i], indent + 1);
            }
            break;

        case NODE_BLOCK:
            print_indent(indent);
            puts("Block");
            for (int i = 0; i < node->child_count; ++i) {
                ast_print_summary_node(node->children[i], indent + 1);
            }
            break;

        case NODE_DECL:
            print_indent(indent);
            printf("%s ", node->text && node->text[0] ? node->text : type_to_string(node->data_type));
            if (node->child_count > 0 && node->children[0] && node->children[0]->text) {
                printf("%s", node->children[0]->text);
            } else {
                printf("<missing-id>");
            }
            /* Show type information */
            if (node->data_type != TYPE_VOID) {
                printf(" : %s", type_to_string(node->data_type));
            }
            if (node->child_count > 1) {
                printf(" = ");
                ast_print_expression(node->children[1]);
            }
            printf("\n");
            break;

        case NODE_ASSIGN:
            print_indent(indent);
            if (node->child_count > 0 && node->children[0] && node->children[0]->text) {
                printf("%s = ", node->children[0]->text);
            } else {
                printf("<missing-id> = ");
            }
            ast_print_expression(node->child_count > 1 ? node->children[1] : NULL);
            /* Show assignment type */
            if (node->data_type != TYPE_VOID) {
                printf(" : %s", type_to_string(node->data_type));
            }
            printf("\n");
            break;

        case NODE_PRINT:
            print_indent(indent);
            fputs("print ", stdout);
            ast_print_expression(node->child_count > 0 ? node->children[0] : NULL);
            /* Show expression type */
            if (node->child_count > 0 && node->children[0] && node->children[0]->data_type != TYPE_VOID) {
                printf(" : %s", type_to_string(node->children[0]->data_type));
            }
            printf("\n");
            break;

        case NODE_IF:
            print_indent(indent);
            printf("if (");
            ast_print_expression(node->child_count > 0 ? node->children[0] : NULL);
            /* Show condition type */
            if (node->child_count > 0 && node->children[0] && node->children[0]->data_type != TYPE_VOID) {
                printf(" : %s", type_to_string(node->children[0]->data_type));
            }
            printf(")\n");
            if (node->child_count > 1) {
                print_indent(indent);
                puts("then:");
                ast_print_summary_node(node->children[1], indent + 1);
            }
            if (node->child_count > 2) {
                print_indent(indent);
                puts("else:");
                ast_print_summary_node(node->children[2], indent + 1);
            }
            break;

        case NODE_WHILE:
            print_indent(indent);
            printf("while (");
            ast_print_expression(node->child_count > 0 ? node->children[0] : NULL);
            /* Show condition type */
            if (node->child_count > 0 && node->children[0] && node->children[0]->data_type != TYPE_VOID) {
                printf(" : %s", type_to_string(node->children[0]->data_type));
            }
            printf(")\n");
            if (node->child_count > 1) {
                ast_print_summary_node(node->children[1], indent + 1);
            }
            break;

        default:
            print_indent(indent);
            printf("%s", ast_kind_name(node->kind));
            if (node->text && node->text[0]) {
                printf(" %s", node->text);
            }
            if (node->data_type != TYPE_VOID && node->data_type != TYPE_ERROR) {
                printf(" : %s", type_to_string(node->data_type));
            }
            printf("\n");
            for (int i = 0; i < node->child_count; ++i) {
                ast_print_summary_node(node->children[i], indent + 1);
            }
            break;
    }
}

void ast_print_summary(const ASTNode *node, int indent) {
    ast_print_summary_node(node, indent);
}

/* Enhanced detailed printing with additional type and scope information */
void ast_print_detailed(const ASTNode *node) {
    if (!node) {
        printf("NULL AST Node\n");
        return;
    }
    
    printf("=== Detailed AST Information ===\n");
    printf("Node Kind: %s\n", ast_kind_name(node->kind));
    printf("Text: %s\n", node->text ? node->text : "(null)");
    printf("Line: %d\n", node->line);
    printf("Data Type: %s\n", type_to_string(node->data_type));
    printf("Child Count: %d\n", node->child_count);
    printf("Child Capacity: %d\n", node->child_cap);
    
    /* Add detailed type information */
    printf("Type Information:\n");
    printf("  - Base Type: %s\n", type_to_string(node->data_type));
    printf("  - Is Numeric: %s\n", is_numeric_type(node->data_type) ? "Yes" : "No");
    printf("  - Is Boolean: %s\n", is_bool_type(node->data_type) ? "Yes" : "No");
    
    /* Add node-specific information */
    printf("Node Properties:\n");
    switch (node->kind) {
        case NODE_LITERAL:
            printf("  - Literal Value: %s\n", node->text ? node->text : "(null)");
            printf("  - Constant: Yes\n");
            break;
        case NODE_IDENTIFIER:
            printf("  - Variable Name: %s\n", node->text ? node->text : "(null)");
            printf("  - Variable: Yes\n");
            break;
        case NODE_BINARY:
            printf("  - Operator: %s\n", node->text ? node->text : "(null)");
            printf("  - Expression: Binary\n");
            break;
        case NODE_UNARY:
            printf("  - Operator: %s\n", node->text ? node->text : "(null)");
            printf("  - Expression: Unary\n");
            break;
        case NODE_DECL:
            printf("  - Declaration Type: %s\n", type_to_string(node->data_type));
            printf("  - Statement: Declaration\n");
            break;
        case NODE_ASSIGN:
            printf("  - Assignment: Yes\n");
            printf("  - Statement: Assignment\n");
            break;
        case NODE_IF:
            printf("  - Control Flow: Conditional\n");
            printf("  - Statement: If\n");
            break;
        case NODE_WHILE:
            printf("  - Control Flow: Loop\n");
            printf("  - Statement: While\n");
            break;
        case NODE_PRINT:
            printf("  - Output: Yes\n");
            printf("  - Statement: Print\n");
            break;
        default:
            printf("  - Generic Node\n");
            break;
    }
    
    if (node->child_count > 0) {
        printf("Children:\n");
        for (int i = 0; i < node->child_count; ++i) {
            printf("  [%d] %s", i, ast_kind_name(node->children[i]->kind));
            if (node->children[i]->text) {
                printf(" (%s)", node->children[i]->text);
            }
            printf(" [line %d, type: %s]\n", node->children[i]->line, 
                   type_to_string(node->children[i]->data_type));
        }
    }
    printf("===============================\n");
}

void ast_free(ASTNode *node) {
    if (!node) return;
    for (int i = 0; i < node->child_count; ++i) {
        ast_free(node->children[i]);
    }
    free(node->children);
    free(node->text);
    free(node->comment);
    free(node);
}

size_t ast_count_nodes(const ASTNode *node) {
    if (!node) return 0;
    size_t count = 1;
    for (int i = 0; i < node->child_count; ++i) {
        count += ast_count_nodes(node->children[i]);
    }
    return count;
}
