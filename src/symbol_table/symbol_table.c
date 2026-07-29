#include "symbol_table.h"
#include "../util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void symtab_init(SymbolTable *table) {
    table->symbols = NULL;
    table->current_scope = 0;
}

void symtab_destroy(SymbolTable *table) {
    Symbol *sym = table->symbols;
    while (sym) {
        Symbol *next = sym->next;
        free(sym->name);
        free(sym);
        sym = next;
    }
    table->symbols = NULL;
    table->current_scope = 0;
}

void symtab_enter_scope(SymbolTable *table) {
    table->current_scope++;
}

void symtab_exit_scope(SymbolTable *table) {
    for (Symbol *sym = table->symbols; sym; sym = sym->next) {
        if (sym->active && sym->scope_level == table->current_scope) {
            sym->active = false;
        }
    }
    if (table->current_scope > 0) {
        table->current_scope--;
    }
}


Symbol *symtab_lookup_current_scope(const SymbolTable *table, const char *name) {
    for (Symbol *sym = table->symbols; sym; sym = sym->next) {
        if (sym->active && sym->scope_level == table->current_scope && strcmp(sym->name, name) == 0) {
            return sym;
        }
    }
    return NULL;
}

bool symtab_insert(SymbolTable *table, const char *name, DataType type, int line) {
    if (symtab_lookup_current_scope(table, name)) {
        fprintf(stderr, "Semantic Error: Redeclaration of variable '%s' at line %d\n", name, line);
        return false;
    }

    Symbol *sym = (Symbol *)xmalloc(sizeof(Symbol));
    sym->name = xstrdup(name);
    sym->type = type;
    sym->declared_line = line;
    sym->scope_level = table->current_scope;
    sym->active = true;
    sym->next = table->symbols;
    table->symbols = sym;
    return true;
}

Symbol *symtab_lookup_active(const SymbolTable *table, const char *name) {
    for (Symbol *sym = table->symbols; sym; sym = sym->next) {
        if (sym->active && strcmp(sym->name, name) == 0) {
            return sym;
        }
    }
    return NULL;
}

Symbol *symtab_lookup_any(const SymbolTable *table, const char *name) {
    for (Symbol *sym = table->symbols; sym; sym = sym->next) {
        if (strcmp(sym->name, name) == 0) {
            return sym;
        }
    }
    return NULL;
}

/* Print the current symbol table showing only active symbols */
void symtab_print(const SymbolTable *table) {
    printf("=== Symbol Table (Scope Level: %d) ===\n", table->current_scope);
    printf("Active Symbols:\n");
    
    bool found = false;
    for (Symbol *sym = table->symbols; sym; sym = sym->next) {
        if (sym->active) {
            printf("  - %s : %s (declared at line %d, scope %d)\n", 
                   sym->name, type_to_string(sym->type), sym->declared_line, sym->scope_level);
            found = true;
        }
    }
    
    if (!found) {
        printf("  (no active symbols)\n");
    }
    printf("========================================\n");
}

/* Print detailed symbol table showing all symbols including inactive ones */
void symtab_print_detailed(const SymbolTable *table) {
    printf("=== Detailed Symbol Table (Scope Level: %d) ===\n", table->current_scope);
    printf("All Symbols:\n");
    
    bool found = false;
    for (Symbol *sym = table->symbols; sym; sym = sym->next) {
        printf("  - %s : %s (declared at line %d, scope %d) [%s]\n", 
               sym->name, type_to_string(sym->type), sym->declared_line, sym->scope_level,
               sym->active ? "ACTIVE" : "INACTIVE");
        found = true;
    }
    
    if (!found) {
        printf("  (no symbols)\n");
    }
    printf("================================================\n");
}
