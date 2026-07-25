#include "util.h"
#include "parser/parser.h"
#include "semantic/semantic.h"
#include "tac/tac.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

static void print_usage(const char *prog) {
    fprintf(stderr, "Usage: %s <source-file>\n", prog);
    fprintf(stderr, "The compiler also accepts Markdown files with a fenced C code block.\n");
}

int main(int argc, char **argv) {
    if (argc < 2) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    char *raw = read_entire_file(argv[1]);
    if (!raw) {
        fprintf(stderr, "Error: could not open input file '%s'\n", argv[1]);
        return EXIT_FAILURE;
    }

    char *source = extract_code_block(raw);
    free(raw);

    Parser parser;
    parser_init(&parser, source);
    ASTNode *root = parse_program(&parser);
    parser_destroy(&parser);

    if (parser.error_count > 0) {
        fprintf(stderr, "\nCompilation stopped due to syntax/lexical errors.\n");
        ast_free(root);
        free(source);
        return EXIT_FAILURE;
    }

    SemanticContext sem;
    semantic_init(&sem);
    semantic_analyze(root, &sem);

    if (sem.error_count > 0) {
        fprintf(stderr, "\nCompilation stopped due to semantic errors.\n");
        semantic_destroy(&sem);
        ast_free(root);
        free(source);
        return EXIT_FAILURE;
    }

    printf("Abstract Syntax Tree:\n");
    ast_print(root, 0);

    TacProgram tac;
    tac_init(&tac);
    tac_generate(root, &tac);

    printf("\nThree Address Code:\n");
    tac_print(&tac);

    tac_free(&tac);
    semantic_destroy(&sem);
    ast_free(root);
    free(source);
    return EXIT_SUCCESS;
}
