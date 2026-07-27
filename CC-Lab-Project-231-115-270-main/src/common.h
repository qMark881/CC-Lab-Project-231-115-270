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

#endif
