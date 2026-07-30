#include "util.h"
#include "parser/parser.h"
#include "semantic/semantic.h"
#include "tac/tac.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define COMPILER_VERSION "ZorvLabs Mini Compiler 2.0"

static bool verbose_mode = false;
static char *output_file = NULL;

static void print_usage(const char *prog) {
    fprintf(stderr, "Usage: %s [options] <source-file | ->\n", prog);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  --help, -h         Show this help message\n");
    fprintf(stderr, "  --version, -v      Show compiler version\n");
    fprintf(stderr, "  --verbose, -V       Enable verbose output with detailed compilation information\n");
    fprintf(stderr, "  --output, -o <file> Specify output file for compilation results\n");
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

    /* Parse command line options */
    int source_arg = 1;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0]);
            return EXIT_SUCCESS;
        }
        if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "-v") == 0) {
            puts(COMPILER_VERSION);
            return EXIT_SUCCESS;
        }
        if (strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-V") == 0) {
            verbose_mode = true;
            source_arg = i + 1;
            continue;
        }
        if (strcmp(argv[i], "--output") == 0 || strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                output_file = argv[i + 1];
                i++; // Skip the next argument as it's the output file
                source_arg = i + 1;
                continue;
            } else {
                fprintf(stderr, "Error: --output requires a file argument\n");
                return EXIT_FAILURE;
            }
        }
        if (argv[i][0] != '-') {
            source_arg = i;
            break;
        }
    }

    if (source_arg >= argc) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (verbose_mode) {
        printf("=== Verbose Mode Enabled ===\n");
        printf("Compiler: %s\n", COMPILER_VERSION);
        printf("Source file: %s\n", argv[source_arg]);
        if (output_file) {
            printf("Output file: %s\n", output_file);
        } else {
            printf("Output: stdout\n");
        }
        printf("==============================\n\n");
    }

    char *source = load_source_text(argv[source_arg]);
    if (!source) {
        fprintf(stderr, "Error: could not open input source '%s'\n", argv[source_arg]);
        return EXIT_FAILURE;
    }

    if (verbose_mode) {
        printf("Source code loaded (%zu bytes)\n", strlen(source));
        printf("=== Source Code ===\n%s\n=== End Source ===\n\n", source);
    }

    Parser parser;
    parser_init(&parser, source);
    
    if (verbose_mode) {
        printf("=== Starting Lexical and Syntax Analysis ===\n");
    }
    
    ASTNode *root = parse_program(&parser);
    int parser_errors = parser.error_count;
    parser_destroy(&parser);

    if (verbose_mode) {
        printf("Syntax analysis completed. Parser errors: %d\n", parser_errors);
    }

    if (parser_errors > 0) {
        fprintf(stderr, "\nCompilation stopped due to syntax/lexical errors.\n");
        ast_free(root);
        free(source);
        return EXIT_FAILURE;
    }

    SemanticContext sem;
    semantic_init(&sem);
    
    if (verbose_mode) {
        printf("=== Starting Semantic Analysis ===\n");
    }
    
    semantic_analyze(root, &sem);
    int semantic_errors = sem.error_count;

    if (verbose_mode) {
        printf("Semantic analysis completed. Semantic errors: %d\n", semantic_errors);
    }

    if (semantic_errors > 0) {
        fprintf(stderr, "\nCompilation stopped due to semantic errors.\n");
        semantic_destroy(&sem);
        ast_free(root);
        free(source);
        return EXIT_FAILURE;
    }

    if (verbose_mode) {
        printf("=== Starting Code Generation ===\n");
    }

    /* Set up output file if specified */
    FILE *original_stdout = stdout;
    if (output_file) {
        stdout = fopen(output_file, "w");
        if (!stdout) {
            fprintf(stderr, "Error: could not open output file '%s'\n", output_file);
            ast_free(root);
            free(source);
            semantic_destroy(&sem);
            return EXIT_FAILURE;
        }
    }

    print_compilation_stage_header("Abstract Syntax Tree");
    ast_print(root, 0);

    print_compilation_stage_header("Visual AST");
    ast_print_visual(root, "", true);

    print_compilation_stage_header("Readable Summary");
    ast_print_summary(root, 0);

    TacProgram tac;
    tac_init(&tac);
    
    if (verbose_mode) {
        printf("=== Generating Three Address Code ===\n");
    }
    
    tac_generate(root, &tac);

    if (verbose_mode) {
        printf("TAC generation completed. Instructions generated.\n");
    }

    print_compilation_stage_header("Three Address Code");
    tac_print(&tac);

    print_success_footer();

    /* Flush and close output file if specified */
    fflush(stdout);
    if (output_file) {
        fclose(stdout);
        stdout = original_stdout;
    }

    if (verbose_mode) {
        printf("=== Compilation Summary ===\n");
        printf("Total source size: %zu bytes\n", strlen(source));
        printf("Parser errors: %d\n", parser_errors);
        printf("Semantic errors: %d\n", semantic_errors);
        printf("TAC instructions: %d\n", tac.instruction_count);
        if (output_file) {
            printf("Output written to: %s\n", output_file);
        }
        printf("========================\n");
    }

    tac_free(&tac);
    semantic_destroy(&sem);
    ast_free(root);
    free(source);
    return EXIT_SUCCESS;
}
