#include "symbol_table.h"
#include "../util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void symtab_init(SymbolTable *table) {
    table->symbols = NULL;
    table->current_scope = 0;
    table->scopes_entered = 1;
    table->max_scope_depth = 0;
    table->symbol_count = 0;
}

void symtab_destroy(SymbolTable *table) {
    Symbol *sym = table->symbols;
    while (sym) {
        Symbol *next = sym->next;
        free(sym->name);
        free(sym->usage_lines);
        free(sym);
        sym = next;
    }
    table->symbols = NULL;
    table->current_scope = 0;
    table->scopes_entered = 0;
    table->max_scope_depth = 0;
    table->symbol_count = 0;
}

void symtab_enter_scope(SymbolTable *table) {
    table->current_scope++;
    table->scopes_entered++;
    if (table->current_scope > table->max_scope_depth) {
        table->max_scope_depth = table->current_scope;
    }
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
        return false;
    }

    Symbol *sym = (Symbol *)xmalloc(sizeof(Symbol));
    sym->name = xstrdup(name);
    sym->type = type;
    sym->declared_line = line;
    sym->scope_level = table->current_scope;
    sym->active = true;
    sym->usage_count = 0;
    sym->usage_lines = NULL;
    sym->usage_capacity = 0;
    sym->next = table->symbols;
    table->symbols = sym;
    table->symbol_count++;
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

/* Find symbol conflicts (variable shadowing) in the symbol table */
int symtab_find_conflicts(const SymbolTable *table, SymbolConflict *conflicts, int max_conflicts) {
    int conflict_count = 0;
    
    for (Symbol *inner = table->symbols; inner && conflict_count < max_conflicts; inner = inner->next) {
        if (!inner->active) continue;
        
        /* Look for symbols with the same name in outer scopes */
        for (Symbol *outer = table->symbols; outer && conflict_count < max_conflicts; outer = outer->next) {
            if (outer == inner) continue;
            if (!outer->active) continue;
            
            /* Check if same name and different scope levels (shadowing) */
            if (strcmp(inner->name, outer->name) == 0 && 
                inner->scope_level != outer->scope_level) {
                
                /* Determine which is outer and which is inner */
                Symbol *outer_sym = (outer->scope_level < inner->scope_level) ? outer : inner;
                Symbol *inner_sym = (outer->scope_level < inner->scope_level) ? inner : outer;
                
                conflicts[conflict_count].symbol_name = inner_sym->name;
                conflicts[conflict_count].outer_scope_line = outer_sym->declared_line;
                conflicts[conflict_count].inner_scope_line = inner_sym->declared_line;
                conflicts[conflict_count].outer_type = outer_sym->type;
                conflicts[conflict_count].inner_type = inner_sym->type;
                
                conflict_count++;
            }
        }
    }
    
    return conflict_count;
}

/* Print symbol conflict information */
void symtab_print_conflicts(const SymbolConflict *conflicts, int count) {
    if (count == 0) {
        printf("No symbol conflicts detected.\n");
        return;
    }
    
    printf("=== Symbol Conflicts Detected ===\n");
    printf("Total conflicts: %d\n", count);
    printf("\n");
    
    for (int i = 0; i < count; i++) {
        printf("Conflict %d:\n", i + 1);
        printf("  Symbol: '%s'\n", conflicts[i].symbol_name);
        printf("  Outer declaration: line %d (type: %s)\n", 
               conflicts[i].outer_scope_line, type_to_string(conflicts[i].outer_type));
        printf("  Inner declaration: line %d (type: %s)\n", 
               conflicts[i].inner_scope_line, type_to_string(conflicts[i].inner_type));
        printf("  Issue: Variable shadowing detected\n");
        printf("\n");
    }
    
    printf("=================================\n");
}

/* Record variable usage at a specific line */
void symtab_record_usage(SymbolTable *table, const char *name, int line) {
    Symbol *sym = symtab_lookup_active(table, name);
    if (!sym) return;
    
    /* Expand usage_lines array if needed */
    if (sym->usage_count >= sym->usage_capacity) {
        int new_capacity = sym->usage_capacity == 0 ? 4 : sym->usage_capacity * 2;
        sym->usage_lines = (int *)xrealloc(sym->usage_lines, new_capacity * sizeof(int));
        sym->usage_capacity = new_capacity;
    }
    
    /* Add usage line if not already recorded */
    bool already_recorded = false;
    for (int i = 0; i < sym->usage_count; i++) {
        if (sym->usage_lines[i] == line) {
            already_recorded = true;
            break;
        }
    }
    
    if (!already_recorded) {
        sym->usage_lines[sym->usage_count++] = line;
    }
}

/* Print cross-reference information for all symbols */
void symtab_print_cross_references(const SymbolTable *table) {
    printf("=== Symbol Cross-Reference Information ===\n");
    printf("Scope Level: %d\n", table->current_scope);
    printf("\n");
    
    bool found = false;
    for (Symbol *sym = table->symbols; sym; sym = sym->next) {
        if (sym->active) {
            printf("Symbol: %s\n", sym->name);
            printf("  Type: %s\n", type_to_string(sym->type));
            printf("  Declared at line: %d\n", sym->declared_line);
            printf("  Scope level: %d\n", sym->scope_level);
            printf("  Usage count: %d\n", sym->usage_count);
            
            if (sym->usage_count > 0) {
                printf("  Used at lines: ");
                for (int i = 0; i < sym->usage_count; i++) {
                    printf("%d", sym->usage_lines[i]);
                    if (i < sym->usage_count - 1) printf(", ");
                }
                printf("\n");
            } else {
                printf("  Warning: Variable declared but never used\n");
            }
            
            printf("\n");
            found = true;
        }
    }
    
    if (!found) {
        printf("No active symbols found.\n");
    }
    
    printf("==========================================\n");
}
