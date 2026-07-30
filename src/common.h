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
