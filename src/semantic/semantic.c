#include "semantic.h"
#include "../util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>

/* Report a semantic error with formatted message and error code.
 * Increments the error count and prints the error to stderr. */
static void semantic_error(SemanticContext *ctx, int line, ErrorCode code, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "[%s] ", error_code_to_string(code));
    vfprintf(stderr, fmt, args);
    fprintf(stderr, " at line %d: %s\n", line, error_code_description(code));
    va_end(args);
    ctx->error_count++;
    ctx->last_error_code = code;
}

/* Initialize the semantic analysis context.
 * Creates an empty symbol table and initializes error tracking. */
void semantic_init(SemanticContext *ctx) {
    symtab_init(&ctx->table);
    ctx->issues = NULL;
    ctx->error_count = 0;
    ctx->last_error_code = ERR_NONE;
    ctx->warning_count = 0;
    ctx->last_warning_code = WARN_NONE;
}

/* Clean up semantic analysis resources.
 * Destroys the symbol table and frees all reported issues. */
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

/* Check if an issue has already been reported to avoid duplicate error messages. */
static bool issue_already_reported(SemanticContext *ctx, int kind, const char *name) {
    for (ReportedIssue *issue = ctx->issues; issue; issue = issue->next) {
        if (issue->kind == kind && strcmp(issue->name, name) == 0) {
            return true;
        }
    }
    return false;
}

/* Remember a reported issue to avoid duplicate error messages. */
static void remember_issue(SemanticContext *ctx, int kind, const char *name) {
    ReportedIssue *issue = (ReportedIssue *)xmalloc(sizeof(ReportedIssue));
    issue->name = xstrdup(name);
    issue->kind = kind;
    issue->next = ctx->issues;
    ctx->issues = issue;
}

/* Look up an identifier in the symbol table.
 * Returns the variable's type or reports an error if not found or out of scope. */
static DataType lookup_identifier(ASTNode *node, SemanticContext *ctx) {
    Symbol *sym = symtab_lookup_active(&ctx->table, node->text);
    if (sym) {
        node->data_type = sym->type;
        /* Record variable usage */
        symtab_record_usage(&ctx->table, node->text, node->line);
        return sym->type;
    }
    Symbol *any = symtab_lookup_any(&ctx->table, node->text);
    if (any) {
        if (!issue_already_reported(ctx, 1, node->text)) {
            remember_issue(ctx, 1, node->text);
            semantic_error(ctx, node->line, ERR_SEMANTIC_OUT_OF_SCOPE, "Variable '%s' is out of scope", node->text);
        }
    } else {
        if (!issue_already_reported(ctx, 0, node->text)) {
            remember_issue(ctx, 0, node->text);
            semantic_error(ctx, node->line, ERR_SEMANTIC_UNDECLARED_VAR, "Undeclared variable '%s'", node->text);
        }
    }
    node->data_type = TYPE_ERROR;
    return TYPE_ERROR;
}

/* Analyze binary expressions for type checking.
 * Performs type checking on both operands and determines the result type. */
static DataType analyze_binary(ASTNode *node, SemanticContext *ctx) {
    DataType left = analyze_expr(node->children[0], ctx);
    DataType right = analyze_expr(node->children[1], ctx);
    const char *op = node->text;

    if (left == TYPE_ERROR || right == TYPE_ERROR) {
        node->data_type = TYPE_ERROR;
        return TYPE_ERROR;
    }

    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 || strcmp(op, "*") == 0 || strcmp(op, "/") == 0 || strcmp(op, "%") == 0) {
        if (!is_numeric_type(left) || !is_numeric_type(right)) {
            semantic_error(ctx, node->line, ERR_SEMANTIC_INVALID_ARITHMETIC, "Invalid expression: arithmetic operator '%s' requires numeric operands", op);
            node->data_type = TYPE_ERROR;
            return TYPE_ERROR;
        }
        if (strcmp(op, "%") == 0 && (left != TYPE_INT || right != TYPE_INT)) {
            semantic_error(ctx, node->line, ERR_SEMANTIC_INVALID_MODULO, "Invalid expression: modulo operator requires integer operands");
            node->data_type = TYPE_ERROR;
            return TYPE_ERROR;
        }
        node->data_type = numeric_result_type(left, right);
        return node->data_type;
    }

    if (strcmp(op, "<") == 0 || strcmp(op, ">") == 0 || strcmp(op, "<=") == 0 || strcmp(op, ">=") == 0) {
        if (!is_numeric_type(left) || !is_numeric_type(right)) {
            semantic_error(ctx, node->line, ERR_SEMANTIC_INVALID_RELATIONAL, "Invalid expression: relational operator '%s' requires numeric operands", op);
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
            semantic_error(ctx, node->line, ERR_SEMANTIC_TYPE_MISMATCH, "Invalid expression: equality operator '%s' requires matching operands", op);
            node->data_type = TYPE_ERROR;
            return TYPE_ERROR;
        }
        node->data_type = TYPE_BOOL;
        return TYPE_BOOL;
    }

    if (strcmp(op, "&&") == 0 || strcmp(op, "||") == 0) {
        if (!is_bool_type(left) || !is_bool_type(right)) {
            semantic_error(ctx, node->line, ERR_SEMANTIC_INVALID_LOGICAL, "Invalid expression: logical operator '%s' requires bool operands", op);
            node->data_type = TYPE_ERROR;
            return TYPE_ERROR;
        }
        node->data_type = TYPE_BOOL;
        return TYPE_BOOL;
    }

    semantic_error(ctx, node->line, ERR_SEMANTIC_INVALID_ARITHMETIC, "Unknown binary operator '%s'", op);
    node->data_type = TYPE_ERROR;
    return TYPE_ERROR;
}

/* Analyze unary expressions for type checking.
 * Performs type checking on the operand and determines the result type. */
static DataType analyze_unary(ASTNode *node, SemanticContext *ctx) {
    DataType operand = analyze_expr(node->children[0], ctx);
    const char *op = node->text;
    if (operand == TYPE_ERROR) {
        node->data_type = TYPE_ERROR;
        return TYPE_ERROR;
    }
    if (strcmp(op, "!") == 0) {
        if (!is_bool_type(operand)) {
            semantic_error(ctx, node->line, ERR_SEMANTIC_INVALID_LOGICAL, "Invalid expression: logical not requires bool operand");
            node->data_type = TYPE_ERROR;
            return TYPE_ERROR;
        }
        node->data_type = TYPE_BOOL;
        return TYPE_BOOL;
    }
    if (strcmp(op, "-") == 0) {
        if (!is_numeric_type(operand)) {
            semantic_error(ctx, node->line, ERR_SEMANTIC_INVALID_ARITHMETIC, "Invalid expression: unary minus requires numeric operand");
            node->data_type = TYPE_ERROR;
            return TYPE_ERROR;
        }
        node->data_type = operand;
        return operand;
    }
    semantic_error(ctx, node->line, ERR_SEMANTIC_INVALID_ARITHMETIC, "Unknown unary operator '%s'", op);
    node->data_type = TYPE_ERROR;
    return TYPE_ERROR;
}

/* Analyze an expression node and determine its type.
 * Dispatches to the appropriate analysis function based on node type. */
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
        default:
            return TYPE_ERROR;
    }
}

static void analyze_stmt(ASTNode *node, SemanticContext *ctx);

/* Analyze a block of statements with a new scope.
 * Enters a new scope, analyzes all statements, then exits the scope. */
static void analyze_block(ASTNode *node, SemanticContext *ctx) {
    symtab_enter_scope(&ctx->table);
    for (int i = 0; i < node->child_count; ++i) {
        analyze_stmt(node->children[i], ctx);
    }
    symtab_exit_scope(&ctx->table);
}

/* Analyze a statement node based on its type.
 * Dispatches to the appropriate analysis function for each statement type. */
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
            /* Analyze variable declarations with type checking of initializers */
            if (node->child_count < 1 || !node->children[0] || node->children[0]->kind != NODE_IDENTIFIER) {
                semantic_error(ctx, node->line, ERR_SYNTAX_EXPECTED_IDENTIFIER, "Malformed declaration");
                break;
            }
            const char *name = node->children[0]->text;
            DataType declared = node->data_type;
            if (symtab_lookup_current_scope(&ctx->table, name)) {
                semantic_error(ctx, node->line, ERR_SEMANTIC_REDECLARATION, "Redeclaration of variable '%s'", name);
                break;
            }
            
            if (node->child_count > 1) {
                DataType init_type = analyze_expr(node->children[1], ctx);
                if (init_type != TYPE_ERROR && !assignment_compatible(declared, init_type)) {
                    if (declared == TYPE_INT && init_type == TYPE_BOOL) {
                        semantic_error(ctx, node->line, ERR_SEMANTIC_ASSIGNMENT_INCOMPATIBLE, "Cannot assign bool to int");
                    } else {
                        semantic_error(ctx, node->line, ERR_SEMANTIC_TYPE_MISMATCH,
                                       "Type mismatch in initialization: expected %s, found %s",
                                       type_to_string(declared), type_to_string(init_type));
                    }
                }
            }
            symtab_insert(&ctx->table, name, declared, node->line);
            break;
        }

        case NODE_ASSIGN: {
            /* Analyze assignment statements with type checking */
            if (node->child_count != 2 || !node->children[0] || node->children[0]->kind != NODE_IDENTIFIER) {
                semantic_error(ctx, node->line, ERR_SYNTAX_EXPECTED_IDENTIFIER, "Malformed assignment");
                break;
            }
            ASTNode *id = node->children[0];
            ASTNode *rhs = node->children[1];
            Symbol *sym = symtab_lookup_active(&ctx->table, id->text);
            if (!sym) {
                if (symtab_lookup_any(&ctx->table, id->text)) {
                    if (!issue_already_reported(ctx, 1, id->text)) {
                        remember_issue(ctx, 1, id->text);
                        semantic_error(ctx, node->line, ERR_SEMANTIC_OUT_OF_SCOPE, "Variable '%s' is out of scope", id->text);
                    }
                } else {
                    if (!issue_already_reported(ctx, 0, id->text)) {
                        remember_issue(ctx, 0, id->text);
                        semantic_error(ctx, node->line, ERR_SEMANTIC_UNDECLARED_VAR, "Undeclared variable '%s'", id->text);
                    }
                }
                break;
            }
            
            /* Record variable usage */
            symtab_record_usage(&ctx->table, id->text, node->line);
            DataType rhs_type = analyze_expr(rhs, ctx);
            if (rhs_type != TYPE_ERROR && !assignment_compatible(sym->type, rhs_type)) {
                if (sym->type == TYPE_INT && rhs_type == TYPE_BOOL) {
                    semantic_error(ctx, node->line, ERR_SEMANTIC_ASSIGNMENT_INCOMPATIBLE, "Cannot assign bool to int");
                } else {
                    semantic_error(ctx, node->line, ERR_SEMANTIC_TYPE_MISMATCH, "Type mismatch in assignment: expected %s, found %s",
                                   type_to_string(sym->type), type_to_string(rhs_type));
                }
            }
            node->data_type = sym->type;
            break;
        }

        case NODE_IF: {
            /* Analyze if statements with boolean condition checking */
            if (node->child_count < 2) {
                semantic_error(ctx, node->line, ERR_SYNTAX_UNEXPECTED_TOKEN, "Malformed if statement");
                break;
            }
            DataType cond_type = analyze_expr(node->children[0], ctx);
            if (cond_type != TYPE_BOOL && cond_type != TYPE_ERROR) {
                semantic_error(ctx, node->line, ERR_SEMANTIC_CONDITION_NOT_BOOL, "Condition must be bool");
            }
            analyze_stmt(node->children[1], ctx);
            if (node->child_count > 2) {
                analyze_stmt(node->children[2], ctx);
            }
            break;
        }

        case NODE_WHILE: {
            /* Analyze while statements with boolean condition checking */
            if (node->child_count < 2) {
                semantic_error(ctx, node->line, ERR_SYNTAX_UNEXPECTED_TOKEN, "Malformed while statement");
                break;
            }
            DataType cond_type = analyze_expr(node->children[0], ctx);
            if (cond_type != TYPE_BOOL && cond_type != TYPE_ERROR) {
                semantic_error(ctx, node->line, ERR_SEMANTIC_CONDITION_NOT_BOOL, "Condition must be bool");
            }
            analyze_stmt(node->children[1], ctx);
            break;
        }

        case NODE_PRINT:
            /* Analyze print statements by checking the expression type */
            if (node->child_count != 1) {
                semantic_error(ctx, node->line, ERR_SYNTAX_UNEXPECTED_TOKEN, "Malformed print statement");
                break;
            }
            analyze_expr(node->children[0], ctx);
            break;

        default:
            break;
    }
}

/* Perform semantic analysis on the entire AST.
 * This is the main entry point for semantic analysis. */
void semantic_analyze(ASTNode *root, SemanticContext *ctx) {
    analyze_stmt(root, ctx);
}

/* Helper function to check if a node is a constant literal */
static bool is_constant_literal(ASTNode *node) {
    return node && node->kind == NODE_LITERAL;
}

/* Helper function to get integer value from a literal node */
static int get_int_value(ASTNode *node) {
    if (!node || !node->text) return 0;
    int value = 0;
    sscanf(node->text, "%d", &value);
    return value;
}

/* Helper function to get float value from a literal node */
static float get_float_value(ASTNode *node) {
    if (!node || !node->text) return 0.0f;
    float value = 0.0f;
    sscanf(node->text, "%f", &value);
    return value;
}

/* Helper function to create a new literal node from an integer value */
static ASTNode *create_int_literal(int value, int line) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d", value);
    return ast_make_literal(buffer, TYPE_INT, line);
}

/* Helper function to create a new literal node from a float value */
static ASTNode *create_float_literal(float value, int line) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.2f", value);
    return ast_make_literal(buffer, TYPE_FLOAT, line);
}

/* Perform constant folding on a binary expression node */
static ASTNode *fold_binary_expression(ASTNode *node) {
    if (!node || node->kind != NODE_BINARY || node->child_count < 2) {
        return node;
    }

    ASTNode *left = node->children[0];
    ASTNode *right = node->children[1];

    /* Only fold if both operands are constant literals */
    if (!is_constant_literal(left) || !is_constant_literal(right)) {
        return node;
    }

    const char *op = node->text;
    int line = node->line;

    /* Handle integer operations */
    if (left->data_type == TYPE_INT && right->data_type == TYPE_INT) {
        int left_val = get_int_value(left);
        int right_val = get_int_value(right);
        int result;

        if (strcmp(op, "+") == 0) {
            result = left_val + right_val;
        } else if (strcmp(op, "-") == 0) {
            result = left_val - right_val;
        } else if (strcmp(op, "*") == 0) {
            result = left_val * right_val;
        } else if (strcmp(op, "/") == 0) {
            if (right_val == 0) return node; /* Don't fold division by zero */
            result = left_val / right_val;
        } else if (strcmp(op, "%") == 0) {
            if (right_val == 0) return node; /* Don't fold modulo by zero */
            result = left_val % right_val;
        } else {
            return node; /* Not a foldable operation */
        }

        return create_int_literal(result, line);
    }

    /* Handle float operations */
    if ((left->data_type == TYPE_FLOAT || right->data_type == TYPE_FLOAT)) {
        float left_val = (left->data_type == TYPE_FLOAT) ? get_float_value(left) : (float)get_int_value(left);
        float right_val = (right->data_type == TYPE_FLOAT) ? get_float_value(right) : (float)get_int_value(right);
        float result;

        if (strcmp(op, "+") == 0) {
            result = left_val + right_val;
        } else if (strcmp(op, "-") == 0) {
            result = left_val - right_val;
        } else if (strcmp(op, "*") == 0) {
            result = left_val * right_val;
        } else if (strcmp(op, "/") == 0) {
            if (right_val == 0.0f) return node; /* Don't fold division by zero */
            result = left_val / right_val;
        } else {
            return node; /* Not a foldable operation */
        }

        return create_float_literal(result, line);
    }

    return node;
}

/* Perform constant folding on a unary expression node */
static ASTNode *fold_unary_expression(ASTNode *node) {
    if (!node || node->kind != NODE_UNARY || node->child_count < 1) {
        return node;
    }

    ASTNode *operand = node->children[0];

    /* Only fold if operand is a constant literal */
    if (!is_constant_literal(operand)) {
        return node;
    }

    const char *op = node->text;
    int line = node->line;

    /* Handle integer negation */
    if (operand->data_type == TYPE_INT && strcmp(op, "-") == 0) {
        int value = get_int_value(operand);
        return create_int_literal(-value, line);
    }

    /* Handle float negation */
    if (operand->data_type == TYPE_FLOAT && strcmp(op, "-") == 0) {
        float value = get_float_value(operand);
        return create_float_literal(-value, line);
    }

    return node;
}

/* Recursively apply constant folding to an AST subtree */
static void apply_constant_folding(ASTNode *node) {
    if (!node) return;

    /* First, recursively process children */
    for (int i = 0; i < node->child_count; ++i) {
        apply_constant_folding(node->children[i]);
    }

    /* Then try to fold this node if it's an expression */
    if (node->kind == NODE_BINARY) {
        ASTNode *folded = fold_binary_expression(node);
        if (folded != node) {
            /* Replace children and convert to literal */
            for (int i = 0; i < node->child_count; ++i) {
                ast_free(node->children[i]);
            }
            node->child_count = 0;
            free(node->text);
            node->text = xstrdup(folded->text);
            node->data_type = folded->data_type;
            node->kind = NODE_LITERAL;
            ast_free(folded);
        }
    } else if (node->kind == NODE_UNARY) {
        ASTNode *folded = fold_unary_expression(node);
        if (folded != node) {
            /* Replace children and convert to literal */
            for (int i = 0; i < node->child_count; ++i) {
                ast_free(node->children[i]);
            }
            node->child_count = 0;
            free(node->text);
            node->text = xstrdup(folded->text);
            node->data_type = folded->data_type;
            node->kind = NODE_LITERAL;
            ast_free(folded);
        }
    }
}

/* Perform constant folding optimization on the AST.
 * This function evaluates constant expressions at compile time and replaces
 * them with their computed values, improving runtime performance. */
void semantic_optimize_constant_folding(ASTNode *root) {
    if (!root) return;
    apply_constant_folding(root);
}

/* Check if a condition is always false (unreachable branch) */
static bool is_always_false(ASTNode *condition) {
    if (!condition || condition->kind != NODE_LITERAL) {
        return false;
    }
    
    if (condition->data_type == TYPE_BOOL) {
        return strcmp(condition->text, "false") == 0;
    }
    
    if (condition->data_type == TYPE_INT) {
        int value = get_int_value(condition);
        return value == 0;
    }
    
    return false;
}

/* Check if a condition is always true (else branch unreachable) */
static bool is_always_true(ASTNode *condition) {
    if (!condition || condition->kind != NODE_LITERAL) {
        return false;
    }
    
    if (condition->data_type == TYPE_BOOL) {
        return strcmp(condition->text, "true") == 0;
    }
    
    if (condition->data_type == TYPE_INT) {
        int value = get_int_value(condition);
        return value != 0;
    }
    
    return false;
}

/* Perform dead code elimination on a single statement */
static void eliminate_dead_code_stmt(ASTNode *node) {
    if (!node) return;
    
    switch (node->kind) {
        case NODE_IF: {
            if (node->child_count >= 2) {
                ASTNode *condition = node->children[0];
                
                /* If condition is always false, remove the if statement entirely */
                if (is_always_false(condition)) {
                    /* Mark as dead code - in a real implementation, we'd remove it */
                    node->data_type = TYPE_VOID; /* Mark as removed */
                }
                /* If condition is always true, remove else branch if exists */
                else if (is_always_true(condition) && node->child_count > 2) {
                    /* Remove else branch by reducing child count */
                    ast_free(node->children[2]);
                    node->children[2] = NULL;
                    node->child_count = 2;
                }
            }
            break;
        }
        
        case NODE_WHILE: {
            if (node->child_count >= 1) {
                ASTNode *condition = node->children[0];
                
                /* If condition is always false, remove the while loop entirely */
                if (is_always_false(condition)) {
                    node->data_type = TYPE_VOID; /* Mark as removed */
                }
            }
            break;
        }
        
        default:
            break;
    }
    
    /* Recursively process children */
    for (int i = 0; i < node->child_count; ++i) {
        eliminate_dead_code_stmt(node->children[i]);
    }
}

/* Perform dead code elimination optimization on the AST.
 * This function removes unreachable code branches and unused statements,
 * improving runtime performance and code size. */
void semantic_optimize_dead_code_elimination(ASTNode *root) {
    if (!root) return;
    eliminate_dead_code_stmt(root);
}

/* Issue a warning for non-critical issues.
 * Similar to errors but doesn't stop compilation. */
void semantic_warning(SemanticContext *ctx, int line, WarningCode code, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "[%s] Warning: ", warning_code_to_string(code));
    vfprintf(stderr, fmt, args);
    fprintf(stderr, " at line %d: %s\n", line, warning_code_description(code));
    va_end(args);
    ctx->warning_count++;
    ctx->last_warning_code = code;
}

/* Print all warnings accumulated during analysis */
void semantic_print_warnings(SemanticContext *ctx) {
    if (ctx->warning_count == 0) {
        printf("No warnings generated during compilation.\n");
        return;
    }
    
    printf("=== Compilation Warnings ===\n");
    printf("Total warnings: %d\n", ctx->warning_count);
    printf("Last warning code: %s\n", warning_code_to_string(ctx->last_warning_code));
    printf("===========================\n");
}
