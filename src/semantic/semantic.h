#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "../ast/ast.h"
#include "../symbol_table/symbol_table.h"

typedef struct ReportedIssue {
    char *name;
    int kind;
    struct ReportedIssue *next;
} ReportedIssue;

typedef struct SemanticContext {
    SymbolTable table;
    ReportedIssue *issues;
    int error_count;
} SemanticContext;

void semantic_init(SemanticContext *ctx);
void semantic_destroy(SemanticContext *ctx);
void semantic_analyze(ASTNode *root, SemanticContext *ctx);

#endif
