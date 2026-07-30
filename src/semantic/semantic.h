#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "../ast/ast.h"
#include "../symbol_table/symbol_table.h"

/* Structure for tracking semantic issues found during analysis.
 * Maintains a linked list of reported issues for error reporting. */
typedef struct ReportedIssue {
    char *name;                       /* Name of the variable or expression involved */
    int kind;                         /* Type of issue (redeclaration, type mismatch, etc.) */
    struct ReportedIssue *next;       /* Next issue in the linked list */
} ReportedIssue;

/* Context structure for semantic analysis.
 * Contains the symbol table for scope management and error tracking. */
typedef struct SemanticContext {
    SymbolTable table;                /* Symbol table for variable declarations and scope tracking */
    ReportedIssue *issues;            /* Linked list of semantic issues found */
    int error_count;                  /* Total number of semantic errors encountered */
    ErrorCode last_error_code;        /* Last error code encountered */
    int warning_count;                /* Total number of warnings issued */
    ErrorCode last_warning_code;      /* Last warning code encountered */
} SemanticContext;

/* Initialize the semantic analysis context with an empty symbol table. */
void semantic_init(SemanticContext *ctx);

/* Clean up semantic analysis resources and free allocated memory. */
void semantic_destroy(SemanticContext *ctx);

/* Perform semantic analysis on the AST, checking for type errors,
 * undeclared variables, redeclarations, and other semantic issues. */
void semantic_analyze(ASTNode *root, SemanticContext *ctx);

/* Perform constant folding optimization on the AST.
 * Evaluates constant expressions at compile time. */
void semantic_optimize_constant_folding(ASTNode *root);

/* Issue a warning for non-critical issues */
void semantic_warning(SemanticContext *ctx, int line, WarningCode code, const char *fmt, ...);

/* Print all warnings accumulated during analysis */
void semantic_print_warnings(SemanticContext *ctx);

#endif
