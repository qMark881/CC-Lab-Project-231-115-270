#ifndef TAC_H
#define TAC_H

#include "../ast/ast.h"
#include <stddef.h>

typedef struct TacProgram {
    char **lines;
    size_t count;
    size_t cap;
    int temp_counter;
    int label_counter;
    int validation_errors;  /* Number of validation errors */
} TacProgram;

/* Convenience alias for instruction count */
#define instruction_count count

void tac_init(TacProgram *program);
void tac_free(TacProgram *program);
void tac_print(const TacProgram *program);
void tac_generate(ASTNode *root, TacProgram *program);
bool tac_validate(const TacProgram *program);
void tac_print_validation_errors(const TacProgram *program);

#endif
