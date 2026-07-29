#include "util.h"
#include "parser/parser.h"
#include "semantic/semantic.h"
#include "tac/tac.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define COMPILER_VERSION "ZorvLabs Mini Compiler 2.0"

static void print_usage(const char *prog) {
    fprintf(stderr, "Usage: %s <source-file | ->\n", prog);
    fprintf(stderr, "       %s --help\n", prog);
    fprintf(stderr, "       %s --version\n", prog);
    fprintf(stderr, "\n");
    fprintf(stderr, "The compiler accepts plain source files and Markdown files with a fenced C code block.\n");
    fprintf(stderr, "Use '-' to read source from standard input.\n");
}

static void print_compilation_stage_header(const char *title) {
    printf("\n%s\n", title);
    for (size_t i = 0; i < strlen(title); ++i) {
        putchar('-');
    }
    putchar('\n');
}

static void print_success_footer(void) {
    print_compilation_stage_header("Compilation Result");
    puts("Compilation succeeded without lexical, syntax, or semantic errors.");
}

static char *load_source_text(const char *input_path) {
    if (strcmp(input_path, "-") == 0) {
        return read_entire_stream(stdin);
    }

    char *raw = read_entire_file(input_path);
    if (!raw) {
        return NULL;
    }

    char *source = extract_code_block(raw);
    free(raw);
    return source;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        print_usage(argv[0]);
        return EXIT_SUCCESS;
    }

    if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
        puts(COMPILER_VERSION);
        return EXIT_SUCCESS;
    }

    char *source = load_source_text(argv[1]);
    if (!source) {
        fprintf(stderr, "Error: could not open input source '%s'\n", argv[1]);
        return EXIT_FAILURE;
    }

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

    print_compilation_stage_header("Abstract Syntax Tree");
    ast_print(root, 0);

    print_compilation_stage_header("Visual AST");
    ast_print_visual(root, "", true);

    print_compilation_stage_header("Readable Summary");
    ast_print_summary(root, 0);

    TacProgram tac;
    tac_init(&tac);
    tac_generate(root, &tac);

    print_compilation_stage_header("Three Address Code");
    tac_print(&tac);

    print_success_footer();

    tac_free(&tac);
    semantic_destroy(&sem);
    ast_free(root);
    free(source);
    return EXIT_SUCCESS;
}
