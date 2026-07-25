#ifndef AST_H
#define AST_H

#include "common.h"
#include <stddef.h>

typedef enum {
    NODE_PROGRAM,
    NODE_BLOCK,
    NODE_DECL,
    NODE_ASSIGN,
    NODE_IF,
    NODE_WHILE,
    NODE_PRINT,
    NODE_BINARY,
    NODE_UNARY,
    NODE_LITERAL,
    NODE_IDENTIFIER
} NodeKind;

typedef struct ASTNode {
    NodeKind kind;
    char *text;
    int line;
    DataType data_type;
    int child_count;
    int child_cap;
    struct ASTNode **children;
} ASTNode;

ASTNode *ast_create(NodeKind kind, const char *text, int line);
ASTNode *ast_add_child(ASTNode *parent, ASTNode *child);
ASTNode *ast_make_literal(const char *text, DataType type, int line);
ASTNode *ast_make_identifier(const char *name, int line);
void ast_print(const ASTNode *node, int indent);
void ast_free(ASTNode *node);
const char *ast_kind_name(NodeKind kind);

#endif
