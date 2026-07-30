#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_BOOL,
    TYPE_VOID,
    TYPE_ERROR
} DataType;

/* Error codes for standardized error reporting */
typedef enum {
    ERR_NONE = 0,
    ERR_LEXICAL_INVALID_CHAR = 1000,
    ERR_LEXICAL_UNTERMINATED_COMMENT = 1001,
    ERR_SYNTAX_UNEXPECTED_TOKEN = 2000,
    ERR_SYNTAX_MISSING_SEMICOLON = 2001,
    ERR_SYNTAX_MISSING_LPAREN = 2002,
    ERR_SYNTAX_MISSING_RPAREN = 2003,
    ERR_SYNTAX_MISSING_LBRACE = 2004,
    ERR_SYNTAX_MISSING_RBRACE = 2005,
    ERR_SYNTAX_MISSING_ASSIGN = 2006,
    ERR_SYNTAX_EXPECTED_IDENTIFIER = 2007,
    ERR_SEMANTIC_UNDECLARED_VAR = 3000,
    ERR_SEMANTIC_REDECLARATION = 3001,
    ERR_SEMANTIC_TYPE_MISMATCH = 3002,
    ERR_SEMANTIC_INVALID_ARITHMETIC = 3003,
    ERR_SEMANTIC_INVALID_MODULO = 3004,
    ERR_SEMANTIC_INVALID_RELATIONAL = 3005,
    ERR_SEMANTIC_INVALID_LOGICAL = 3006,
    ERR_SEMANTIC_CONDITION_NOT_BOOL = 3007,
    ERR_SEMANTIC_OUT_OF_SCOPE = 3008,
    ERR_SEMANTIC_ASSIGNMENT_INCOMPATIBLE = 3009
} ErrorCode;

/* Warning codes for non-critical issues */
typedef enum {
    WARN_NONE = 0,
    WARN_UNUSED_VARIABLE = 4000,
    WARN_UNUSED_DECLARATION = 4001,
    WARN_IMPLICIT_CONVERSION = 4002,
    WARN_STYLE_ISSUE = 4003,
    WARN_DEAD_CODE = 4004
} WarningCode;

/* Convert warning code to string representation */
static inline const char *warning_code_to_string(WarningCode code) {
    switch (code) {
        case WARN_NONE: return "WARN_NONE";
        case WARN_UNUSED_VARIABLE: return "WARN_UNUSED_VARIABLE";
        case WARN_UNUSED_DECLARATION: return "WARN_UNUSED_DECLARATION";
        case WARN_IMPLICIT_CONVERSION: return "WARN_IMPLICIT_CONVERSION";
        case WARN_STYLE_ISSUE: return "WARN_STYLE_ISSUE";
        case WARN_DEAD_CODE: return "WARN_DEAD_CODE";
        default: return "WARN_UNKNOWN";
    }
}

/* Get human-readable description for warning code */
static inline const char *warning_code_description(WarningCode code) {
    switch (code) {
        case WARN_NONE: return "No warning";
        case WARN_UNUSED_VARIABLE: return "Variable declared but never used";
        case WARN_UNUSED_DECLARATION: return "Declaration without initialization";
        case WARN_IMPLICIT_CONVERSION: return "Implicit type conversion";
        case WARN_STYLE_ISSUE: return "Code style suggestion";
        case WARN_DEAD_CODE: return "Code that will never be executed";
        default: return "Unknown warning";
    }
}

/* Compilation statistics structure */
typedef struct {
    int total_tokens;              /* Total number of tokens generated */
    int total_ast_nodes;           /* Total number of AST nodes created */
    int total_symbols;             /* Total number of symbols in symbol table */
    int total_scopes;              /* Total number of scopes entered */
    int total_tac_instructions;   /* Total number of TAC instructions */
    int constant_folds;           /* Number of constant folding optimizations */
    int source_lines;             /* Number of lines in source code */
    int source_characters;        /* Number of characters in source code */
    int lexical_errors;           /* Number of lexical errors */
    int syntax_errors;            /* Number of syntax errors */
    int semantic_errors;          /* Number of semantic errors */
    int warning_count;             /* Number of warnings issued */
    double total_time;            /* Total compilation time in seconds */
    double lexical_time;          /* Lexical analysis time */
    double syntax_time;           /* Syntax analysis time */
    double semantic_time;         /* Semantic analysis time */
    double optimization_time;     /* Optimization time */
    double codegen_time;          /* Code generation time */
} CompilationStats;

/* Initialize compilation statistics structure */
static inline void stats_init(CompilationStats *stats) {
    stats->total_tokens = 0;
    stats->total_ast_nodes = 0;
    stats->total_symbols = 0;
    stats->total_scopes = 0;
    stats->total_tac_instructions = 0;
    stats->constant_folds = 0;
    stats->source_lines = 0;
    stats->source_characters = 0;
    stats->lexical_errors = 0;
    stats->syntax_errors = 0;
    stats->semantic_errors = 0;
    stats->warning_count = 0;
    stats->total_time = 0.0;
    stats->lexical_time = 0.0;
    stats->syntax_time = 0.0;
    stats->semantic_time = 0.0;
    stats->optimization_time = 0.0;
    stats->codegen_time = 0.0;
}

/* Print compilation statistics */
static inline void stats_print(const CompilationStats *stats) {
    printf("\n=== Compilation Statistics ===\n");
    printf("Source Code:\n");
    printf("  Lines: %d\n", stats->source_lines);
    printf("  Characters: %d\n", stats->source_characters);
    printf("\nLexical Analysis:\n");
    printf("  Tokens generated: %d\n", stats->total_tokens);
    printf("  Lexical errors: %d\n", stats->lexical_errors);
    printf("  Time: %.3f seconds\n", stats->lexical_time);
    printf("\nSyntax Analysis:\n");
    printf("  AST nodes created: %d\n", stats->total_ast_nodes);
    printf("  Syntax errors: %d\n", stats->syntax_errors);
    printf("  Time: %.3f seconds\n", stats->syntax_time);
    printf("\nSemantic Analysis:\n");
    printf("  Symbols declared: %d\n", stats->total_symbols);
    printf("  Scopes entered: %d\n", stats->total_scopes);
    printf("  Semantic errors: %d\n", stats->semantic_errors);
    printf("  Warnings: %d\n", stats->warning_count);
    printf("  Time: %.3f seconds\n", stats->semantic_time);
    printf("\nOptimization:\n");
    printf("  Constant folds: %d\n", stats->constant_folds);
    printf("  Time: %.3f seconds\n", stats->optimization_time);
    printf("\nCode Generation:\n");
    printf("  TAC instructions: %d\n", stats->total_tac_instructions);
    printf("  Time: %.3f seconds\n", stats->codegen_time);
    printf("  Validation: %s\n", stats->total_tac_instructions > 0 ? "N/A" : "N/A");
    printf("\nTotal Compilation:\n");
    printf("  Total time: %.3f seconds\n", stats->total_time);
    printf("=============================\n");
}

static inline const char *type_to_string(DataType type) {
    switch (type) {
        case TYPE_INT: return "int";
        case TYPE_FLOAT: return "float";
        case TYPE_BOOL: return "bool";
        case TYPE_VOID: return "void";
        default: return "error";
    }
}

static inline bool is_numeric_type(DataType type) {
    return type == TYPE_INT || type == TYPE_FLOAT;
}

static inline bool is_bool_type(DataType type) {
    return type == TYPE_BOOL;
}

static inline DataType numeric_result_type(DataType left, DataType right) {
    if (left == TYPE_FLOAT || right == TYPE_FLOAT) {
        return TYPE_FLOAT;
    }
    return TYPE_INT;
}

static inline bool assignment_compatible(DataType target, DataType source) {
    if (target == TYPE_ERROR || source == TYPE_ERROR) {
        return true;
    }
    if (target == source) {
        return true;
    }
    if (target == TYPE_FLOAT && source == TYPE_INT) {
        return true;
    }
    return false;
}

/* Convert error code to string representation */
static inline const char *error_code_to_string(ErrorCode code) {
    switch (code) {
        case ERR_NONE: return "ERR_NONE";
        case ERR_LEXICAL_INVALID_CHAR: return "ERR_LEXICAL_INVALID_CHAR";
        case ERR_LEXICAL_UNTERMINATED_COMMENT: return "ERR_LEXICAL_UNTERMINATED_COMMENT";
        case ERR_SYNTAX_UNEXPECTED_TOKEN: return "ERR_SYNTAX_UNEXPECTED_TOKEN";
        case ERR_SYNTAX_MISSING_SEMICOLON: return "ERR_SYNTAX_MISSING_SEMICOLON";
        case ERR_SYNTAX_MISSING_LPAREN: return "ERR_SYNTAX_MISSING_LPAREN";
        case ERR_SYNTAX_MISSING_RPAREN: return "ERR_SYNTAX_MISSING_RPAREN";
        case ERR_SYNTAX_MISSING_LBRACE: return "ERR_SYNTAX_MISSING_LBRACE";
        case ERR_SYNTAX_MISSING_RBRACE: return "ERR_SYNTAX_MISSING_RBRACE";
        case ERR_SYNTAX_MISSING_ASSIGN: return "ERR_SYNTAX_MISSING_ASSIGN";
        case ERR_SYNTAX_EXPECTED_IDENTIFIER: return "ERR_SYNTAX_EXPECTED_IDENTIFIER";
        case ERR_SEMANTIC_UNDECLARED_VAR: return "ERR_SEMANTIC_UNDECLARED_VAR";
        case ERR_SEMANTIC_REDECLARATION: return "ERR_SEMANTIC_REDECLARATION";
        case ERR_SEMANTIC_TYPE_MISMATCH: return "ERR_SEMANTIC_TYPE_MISMATCH";
        case ERR_SEMANTIC_INVALID_ARITHMETIC: return "ERR_SEMANTIC_INVALID_ARITHMETIC";
        case ERR_SEMANTIC_INVALID_MODULO: return "ERR_SEMANTIC_INVALID_MODULO";
        case ERR_SEMANTIC_INVALID_RELATIONAL: return "ERR_SEMANTIC_INVALID_RELATIONAL";
        case ERR_SEMANTIC_INVALID_LOGICAL: return "ERR_SEMANTIC_INVALID_LOGICAL";
        case ERR_SEMANTIC_CONDITION_NOT_BOOL: return "ERR_SEMANTIC_CONDITION_NOT_BOOL";
        case ERR_SEMANTIC_OUT_OF_SCOPE: return "ERR_SEMANTIC_OUT_OF_SCOPE";
        case ERR_SEMANTIC_ASSIGNMENT_INCOMPATIBLE: return "ERR_SEMANTIC_ASSIGNMENT_INCOMPATIBLE";
        default: return "ERR_UNKNOWN";
    }
}

/* Get human-readable description for error code */
static inline const char *error_code_description(ErrorCode code) {
    switch (code) {
        case ERR_NONE: return "No error";
        case ERR_LEXICAL_INVALID_CHAR: return "Invalid character in source code";
        case ERR_LEXICAL_UNTERMINATED_COMMENT: return "Unterminated block comment";
        case ERR_SYNTAX_UNEXPECTED_TOKEN: return "Unexpected token encountered";
        case ERR_SYNTAX_MISSING_SEMICOLON: return "Missing semicolon";
        case ERR_SYNTAX_MISSING_LPAREN: return "Missing left parenthesis";
        case ERR_SYNTAX_MISSING_RPAREN: return "Missing right parenthesis";
        case ERR_SYNTAX_MISSING_LBRACE: return "Missing left brace";
        case ERR_SYNTAX_MISSING_RBRACE: return "Missing right brace";
        case ERR_SYNTAX_MISSING_ASSIGN: return "Missing assignment operator";
        case ERR_SYNTAX_EXPECTED_IDENTIFIER: return "Expected identifier";
        case ERR_SEMANTIC_UNDECLARED_VAR: return "Undeclared variable";
        case ERR_SEMANTIC_REDECLARATION: return "Variable redeclaration";
        case ERR_SEMANTIC_TYPE_MISMATCH: return "Type mismatch";
        case ERR_SEMANTIC_INVALID_ARITHMETIC: return "Invalid arithmetic operation";
        case ERR_SEMANTIC_INVALID_MODULO: return "Modulo requires integer operands";
        case ERR_SEMANTIC_INVALID_RELATIONAL: return "Invalid relational operation";
        case ERR_SEMANTIC_INVALID_LOGICAL: return "Invalid logical operation";
        case ERR_SEMANTIC_CONDITION_NOT_BOOL: return "Condition must be boolean";
        case ERR_SEMANTIC_OUT_OF_SCOPE: return "Variable out of scope";
        case ERR_SEMANTIC_ASSIGNMENT_INCOMPATIBLE: return "Assignment type incompatible";
        default: return "Unknown error";
    }
}

#endif
