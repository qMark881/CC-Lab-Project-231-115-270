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
} TacProgram;

void tac_init(TacProgram *program);
void tac_free(TacProgram *program);
void tac_print(const TacProgram *program);
void tac_generate(ASTNode *root, TacProgram *program);

#endif
