#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "../common.h"
#include <stdbool.h>

typedef struct Symbol {
    char *name;
    DataType type;
    int declared_line;
    int scope_level;
    bool active;
    struct Symbol *next;
} Symbol;

typedef struct SymbolTable {
    Symbol *symbols;
    int current_scope;
} SymbolTable;

void symtab_init(SymbolTable *table);
void symtab_destroy(SymbolTable *table);
void symtab_enter_scope(SymbolTable *table);
void symtab_exit_scope(SymbolTable *table);
bool symtab_insert(SymbolTable *table, const char *name, DataType type, int line);
Symbol *symtab_lookup_active(const SymbolTable *table, const char *name);
Symbol *symtab_lookup_any(const SymbolTable *table, const char *name);
Symbol *symtab_lookup_current_scope(const SymbolTable *table, const char *name);
void symtab_print(const SymbolTable *table);
void symtab_print_detailed(const SymbolTable *table);

/* Detect symbol conflicts (shadowing, name collisions) */
typedef struct {
    char *symbol_name;
    int outer_scope_line;
    int inner_scope_line;
    DataType outer_type;
    DataType inner_type;
} SymbolConflict;

/* Find symbol conflicts in the symbol table */
int symtab_find_conflicts(const SymbolTable *table, SymbolConflict *conflicts, int max_conflicts);

/* Print symbol conflict information */
void symtab_print_conflicts(const SymbolConflict *conflicts, int count);

#endif
