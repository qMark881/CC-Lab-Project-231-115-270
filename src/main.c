#include "util.h"
#include "parser/parser.h"
#include "semantic/semantic.h"
#include "tac/tac.h"
#include "symbol_table/symbol_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>
#include <math.h>

#define COMPILER_VERSION "ZorvLabs Mini Compiler 2.0"
#define COMPILER_NAME "ZorvLabs Compiler"

static bool verbose_mode = false;
static char *output_file = NULL;
static bool show_stats = false;
static bool format_source = false;
static bool json_output = false;
static bool csv_output = false;
static bool show_logo = false;
static ProfileMode profile_mode = PROFILE_NONE;
static CompilationStats compilation_stats;

static void print_usage(const char *prog) {
    fprintf(stderr, "Usage: %s [options] <source-file | ->\n", prog);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  --help, -h         Show this help message\n");
    fprintf(stderr, "  --version, -v      Show compiler version\n");
    fprintf(stderr, "  --logo, -L         Display compiler logo and branding\n");
    fprintf(stderr, "  --verbose, -V       Enable verbose output with detailed compilation information\n");
    fprintf(stderr, "  --output, -o <file> Specify output file for compilation results\n");
    fprintf(stderr, "  --stats, -S        Print compilation statistics\n");
    fprintf(stderr, "  --format, -F       Format and pretty-print source code\n");
    fprintf(stderr, "  --json, -J         Output statistics in JSON format\n");
    fprintf(stderr, "  --csv, -C          Output statistics in CSV format\n");
    fprintf(stderr, "  --profile, -P <mode> Enable performance profiling (none|basic|detailed|full)\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "The compiler accepts plain source files and Markdown files with a fenced C code block.\n");
    fprintf(stderr, "Use '-' to read source from standard input.\n");
}

/* Display compiler logo and branding information */
static void print_logo(void) {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════════════╗\n");
    printf("║  ____  _   _ ____  _    _ _   _    ____ ____  ___  _   _ ___   ║\n");
    printf("║ |  _ \\| | | |  _ \\| |  | | | | |  / ___|  _ \\/ _ \\| | | | __|  ║\n");
    printf("║ | | | | | | | | | | |  | | | | | | |   | |_) | | | | | | | |_   ║\n");
    printf("║ | |_| | |_| | |_| | |__| | |_| | | |___|  _ <| |_| | |_| |  _|  ║\n");
    printf("║ |____/ \\___/|____/|____/ \\___/   \\____|_| \\_\\\\___/ \\___/|_|    ║\n");
    printf("║                                                                ║\n");
    printf("║                    ____  ____  ___    _   _ _                  ║\n");
    printf("║                   / ___||  _ \\|_ _|  | | | | |                 ║\n");
    printf("║                   \\___ \\| |_) || |   | | | | |                 ║\n");
    printf("║                    ___) |  _ < | |   | |_| | |___              ║\n");
    printf("║                   |____/|_| \\_\\___|   \\___/|_____|             ║\n");
    printf("║                                                                ║\n");
    printf("║                       MINI COMPILER v2.0                       ║\n");
    printf("║                   Advanced Compiler Technology                  ║\n");
    printf("╚════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
    printf("Features:\n");
    printf("  • Lexical Analysis with Token Generation\n");
    printf("  • Syntax Analysis with AST Construction\n");
    printf("  • Semantic Analysis with Type Checking\n");
    printf("  • Symbol Table with Nested Scope Support\n");
    printf("  • Three Address Code (TAC) Generation\n");
    printf("  • Constant Folding Optimization\n");
    printf("  • Dead Code Elimination\n");
    printf("  • Error Reporting with Error Codes\n");
    printf("  • Warning System for Code Quality\n");
    printf("  • Source Code Formatting\n");
    printf("  • IR Validation and Consistency Checks\n");
    printf("  • Comprehensive Statistics Collection\n");
    printf("  • Symbol Conflict Detection\n");
    printf("\n");
    printf("Version: %s\n", COMPILER_VERSION);
    printf("Built: " __DATE__ " " __TIME__ "\n");
    printf("\n");
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

/* Enhanced source code formatter with better formatting rules */
static void format_source_code(const char *source) {
    if (!source) return;
    
    printf("=== Formatted Source Code ===\n");
    
    int indent_level = 0;
    bool in_block = false;
    bool after_operator = false;
    bool after_keyword = false;
    
    for (size_t i = 0; i < strlen(source); i++) {
        char c = source[i];
        char prev = (i > 0) ? source[i-1] : '\0';
        char next = (i < strlen(source)-1) ? source[i+1] : '\0';
        
        /* Handle braces for indentation */
        if (c == '{') {
            putchar(' ');
            putchar('{');
            putchar('\n');
            indent_level++;
            in_block = true;
            /* Add indentation for next line */
            for (int j = 0; j < indent_level; j++) {
                putchar(' ');
                putchar(' ');
            }
            continue;
        }
        
        if (c == '}') {
            indent_level--;
            if (indent_level < 0) indent_level = 0;
            putchar('\n');
            for (int j = 0; j < indent_level; j++) {
                putchar(' ');
                putchar(' ');
            }
            putchar('}');
            putchar('\n');
            in_block = false;
            continue;
        }
        
        /* Handle semicolons */
        if (c == ';') {
            putchar(';');
            putchar('\n');
            if (in_block) {
                for (int j = 0; j < indent_level; j++) {
                    putchar(' ');
                    putchar(' ');
                }
            }
            after_operator = false;
            after_keyword = false;
            continue;
        }
        
        /* Handle operators with spacing */
        if (c == '=' || c == '+' || c == '-' || c == '*' || c == '/' || c == '%') {
            /* Add space before operator if needed */
            if (prev != ' ' && prev != '\t' && prev != '\n' && prev != '(' && !after_operator) {
                putchar(' ');
            }
            putchar(c);
            /* Add space after operator if needed */
            if (next != ' ' && next != '\t' && next != '\n' && next != ';' && next != ')') {
                putchar(' ');
            }
            after_operator = true;
            continue;
        }
        
        /* Handle comparison operators */
        if (c == '<' || c == '>' || c == '!') {
            if (next == '=' || (c == '<' && next == '<') || (c == '>' && next == '>')) {
                /* Two-character operator */
                if (prev != ' ' && prev != '\t' && prev != '\n') {
                    putchar(' ');
                }
                putchar(c);
                /* Next character will be handled in next iteration */
                after_operator = true;
                continue;
            }
            if (prev != ' ' && prev != '\t' && prev != '\n' && !after_operator) {
                putchar(' ');
            }
            putchar(c);
            if (next != ' ' && next != '\t' && next != '\n' && next != ';') {
                putchar(' ');
            }
            after_operator = true;
            continue;
        }
        
        /* Handle logical operators */
        if (c == '&' || c == '|') {
            if ((c == '&' && next == '&') || (c == '|' && next == '|')) {
                /* Two-character logical operator */
                if (prev != ' ' && prev != '\t' && prev != '\n' && !after_operator) {
                    putchar(' ');
                }
                putchar(c);
                after_operator = true;
                continue;
            }
        }
        
        /* Handle parentheses */
        if (c == '(') {
            if (prev != ' ' && prev != '\t' && prev != '\n' && prev != '(' && !after_keyword) {
                putchar(' ');
            }
            putchar('(');
            after_operator = false;
            continue;
        }
        
        if (c == ')') {
            putchar(')');
            after_operator = false;
            continue;
        }
        
        /* Skip excessive whitespace */
        if (c == ' ' || c == '\t') {
            /* Check if this is meaningful whitespace */
            if (i > 0 && source[i-1] != ' ' && source[i-1] != '\t' && 
                i < strlen(source)-1 && source[i+1] != ' ' && source[i+1] != '\t' &&
                source[i+1] != '\n' && source[i+1] != ';' && source[i+1] != '{' && source[i+1] != '}') {
                putchar(' ');
            }
            continue;
        }
        
        /* Handle newlines */
        if (c == '\n') {
            if (in_block) {
                for (int j = 0; j < indent_level; j++) {
                    putchar(' ');
                    putchar(' ');
                }
            }
            after_operator = false;
            after_keyword = false;
            continue;
        }
        
        /* Check for keywords */
        if (isalpha((unsigned char)c)) {
            /* Check if this starts a keyword */
            const char *keywords[] = {"int", "float", "bool", "if", "else", "while", "print", "true", "false"};
            for (size_t k = 0; k < sizeof(keywords)/sizeof(keywords[0]); k++) {
                if (strncmp(&source[i], keywords[k], strlen(keywords[k])) == 0) {
                    if (i > 0 && source[i-1] != ' ' && source[i-1] != '\t' && source[i-1] != '\n') {
                        putchar(' ');
                    }
                    after_keyword = true;
                    break;
                }
            }
        }
        
        putchar(c);
        after_operator = false;
        if (!isalpha((unsigned char)c)) {
            after_keyword = false;
        }
    }
    
    printf("\n=== End Formatted Source ===\n");
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
        if (strcmp(argv[i], "--stats") == 0 || strcmp(argv[i], "-S") == 0) {
            show_stats = true;
            source_arg = i + 1;
            continue;
        }
        if (strcmp(argv[i], "--format") == 0 || strcmp(argv[i], "-F") == 0) {
            format_source = true;
            source_arg = i + 1;
            continue;
        }
        if (strcmp(argv[i], "--json") == 0 || strcmp(argv[i], "-J") == 0) {
            json_output = true;
            source_arg = i + 1;
            continue;
        }
        if (strcmp(argv[i], "--csv") == 0 || strcmp(argv[i], "-C") == 0) {
            csv_output = true;
            source_arg = i + 1;
            continue;
        }
        if (strcmp(argv[i], "--logo") == 0 || strcmp(argv[i], "-L") == 0) {
            show_logo = true;
            source_arg = i + 1;
            continue;
        }
        if (strcmp(argv[i], "--profile") == 0 || strcmp(argv[i], "-P") == 0) {
            if (i + 1 < argc) {
                const char *mode_str = argv[i + 1];
                if (strcmp(mode_str, "none") == 0) {
                    profile_mode = PROFILE_NONE;
                } else if (strcmp(mode_str, "basic") == 0) {
                    profile_mode = PROFILE_BASIC;
                } else if (strcmp(mode_str, "detailed") == 0) {
                    profile_mode = PROFILE_DETAILED;
                } else if (strcmp(mode_str, "full") == 0) {
                    profile_mode = PROFILE_FULL;
                } else {
                    fprintf(stderr, "Error: invalid profile mode '%s'\n", mode_str);
                    return EXIT_FAILURE;
                }
                i++; // Skip the next argument
                source_arg = i + 1;
                continue;
            } else {
                fprintf(stderr, "Error: --profile requires a mode argument\n");
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

    if (show_logo) {
        print_logo();
        return EXIT_SUCCESS;
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
        printf("Profile mode: %s\n", profile_mode_to_string(profile_mode));
        printf("==============================\n\n");
    }
    
    /* Print profiling header if profiling is enabled */
    if (profile_mode != PROFILE_NONE) {
        printf("=== Performance Profiling Mode: %s ===\n", profile_mode_to_string(profile_mode));
        printf("Profiling started...\n\n");
    }

    /* Initialize compilation statistics */
    stats_init(&compilation_stats);
    
    /* Start total compilation timer */
    clock_t start_time = clock();

    char *source = load_source_text(argv[source_arg]);
    if (!source) {
        fprintf(stderr, "Error: could not open input source '%s'\n", argv[source_arg]);
        return EXIT_FAILURE;
    }

    /* Collect source code statistics */
    compilation_stats.source_characters = (int)strlen(source);
    int line_count = 0;
    for (size_t i = 0; i < strlen(source); i++) {
        if (source[i] == '\n') line_count++;
    }
    compilation_stats.source_lines = line_count;
    
    /* Track source file dependency */
    compilation_stats.dependency_count = 1; /* Main source file */

    if (format_source) {
        printf("Formatting source code...\n");
        format_source_code(source);
        printf("Source code formatting completed.\n");
        return EXIT_SUCCESS;
    }

    if (verbose_mode) {
        printf("Source code loaded (%zu bytes, %d lines)\n", strlen(source), line_count);
        printf("=== Source Code ===\n%s\n=== End Source ===\n\n", source);
    }

    Parser parser;
    parser_init(&parser, source);
    
    if (verbose_mode) {
        printf("=== Starting Lexical and Syntax Analysis ===\n");
    }
    
    clock_t syntax_start = clock();
    ASTNode *root = parse_program(&parser);
    clock_t syntax_end = clock();
    compilation_stats.syntax_time = (double)(syntax_end - syntax_start) / CLOCKS_PER_SEC;
    
    int parser_errors = parser.error_count;
    compilation_stats.syntax_errors = parser_errors;
    compilation_stats.lexical_errors = parser.error_count; /* Combined for now */
    parser_destroy(&parser);

    if (verbose_mode) {
        printf("Syntax analysis completed. Parser errors: %d\n", parser_errors);
        printf("Syntax analysis time: %.3f seconds\n", compilation_stats.syntax_time);
    }

    if (parser_errors > 0) {
        fprintf(stderr, "\nCompilation stopped due to syntax/lexical errors.\n");
        
        /* Show error context for first error if verbose */
        if (verbose_mode && source) {
            fprintf(stderr, "\nError context:\n");
            print_error_context(source, parser.current.line, 2);
        }
        
        ast_free(root);
        free(source);
        return EXIT_FAILURE;
    }

    SemanticContext sem;
    semantic_init(&sem);
    
    if (verbose_mode) {
        printf("=== Starting Semantic Analysis ===\n");
    }
    
    clock_t semantic_start = clock();
    semantic_analyze(root, &sem);
    clock_t semantic_end = clock();
    compilation_stats.semantic_time = (double)(semantic_end - semantic_start) / CLOCKS_PER_SEC;
    
    int semantic_errors = sem.error_count;
    compilation_stats.semantic_errors = semantic_errors;
    compilation_stats.warning_count = sem.warning_count;

    if (verbose_mode) {
        printf("Semantic analysis completed. Semantic errors: %d\n", semantic_errors);
        printf("Semantic analysis time: %.3f seconds\n", compilation_stats.semantic_time);
        
        /* Check for symbol conflicts */
        printf("Checking for symbol conflicts...\n");
        SymbolConflict conflicts[100];
        int conflict_count = symtab_find_conflicts(&sem.table, conflicts, 100);
        compilation_stats.symbol_conflicts = conflict_count;
        
        if (conflict_count > 0) {
            printf("Symbol conflicts detected: %d\n", conflict_count);
            symtab_print_conflicts(conflicts, conflict_count);
        } else {
            printf("No symbol conflicts detected.\n");
        }
        
        /* Print warnings if any */
        if (sem.warning_count > 0) {
            printf("Warnings generated: %d\n", sem.warning_count);
            semantic_print_warnings(&sem);
        }
        
        /* Print cross-reference information in verbose mode */
        if (verbose_mode) {
            printf("=== Symbol Cross-Reference Information ===\n");
            symtab_print_cross_references(&sem.table);
        }
    }

    /* Apply constant folding optimization if no semantic errors */
    if (semantic_errors == 0) {
        if (verbose_mode) {
            printf("=== Starting Constant Folding Optimization ===\n");
        }
        clock_t opt_start = clock();
        semantic_optimize_constant_folding(root);
        clock_t opt_end = clock();
        compilation_stats.optimization_time = (double)(opt_end - opt_start) / CLOCKS_PER_SEC;
        compilation_stats.constant_folds = 1; /* Placeholder for actual count */
        if (verbose_mode) {
            printf("Constant folding optimization completed.\n");
            printf("Optimization time: %.3f seconds\n", compilation_stats.optimization_time);
        }
        
        /* Apply dead code elimination optimization */
        if (verbose_mode) {
            printf("=== Starting Dead Code Elimination Optimization ===\n");
        }
        clock_t dce_start = clock();
        semantic_optimize_dead_code_elimination(root);
        clock_t dce_end = clock();
        compilation_stats.optimization_time += (double)(dce_end - dce_start) / CLOCKS_PER_SEC;
        if (verbose_mode) {
            printf("Dead code elimination optimization completed.\n");
            printf("Total optimization time: %.3f seconds\n", compilation_stats.optimization_time);
        }
    }

    if (semantic_errors > 0) {
        fprintf(stderr, "\nCompilation stopped due to semantic errors.\n");
        
        /* Show error context if verbose */
        if (verbose_mode && source) {
            fprintf(stderr, "\nError context:\n");
            print_error_context(source, 1, 2);
        }
        
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

    if (verbose_mode) {
        print_compilation_stage_header("Detailed AST Node Information");
        ast_print_detailed(root);
    }

    TacProgram tac;
    tac_init(&tac);
    
    if (verbose_mode) {
        printf("=== Generating Three Address Code ===\n");
    }
    
    clock_t codegen_start = clock();
    tac_generate(root, &tac);
    clock_t codegen_end = clock();
    compilation_stats.codegen_time = (double)(codegen_end - codegen_start) / CLOCKS_PER_SEC;
    compilation_stats.total_tac_instructions = (int)tac.count;

    if (verbose_mode) {
        printf("TAC generation completed. Instructions generated: %zu\n", tac.count);
        printf("Code generation time: %.3f seconds\n", compilation_stats.codegen_time);
        
        /* Validate TAC if verbose */
        printf("Validating TAC...\n");
        bool tac_valid = tac_validate(&tac);
        if (tac_valid) {
            printf("TAC validation: PASSED\n");
        } else {
            printf("TAC validation: FAILED (%d errors)\n", tac.validation_errors);
            tac_print_validation_errors(&tac);
        }
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
        
        /* Print detailed compilation statistics */
        stats_print(&compilation_stats);
    }
    
    /* Print statistics if requested */
    if (show_stats) {
        if (json_output) {
            stats_print_json(&compilation_stats);
        } else if (csv_output) {
            stats_print_csv(&compilation_stats);
        } else {
            stats_print(&compilation_stats);
        }
    }
    
    /* Print statistics if requested */
    if (show_stats) {
        if (json_output) {
            stats_print_json(&compilation_stats);
        } else if (csv_output) {
            stats_print_csv(&compilation_stats);
        } else {
            stats_print(&compilation_stats);
        }
    }
    
    /* Print profiling summary if profiling is enabled */
    if (profile_mode != PROFILE_NONE) {
        printf("\n=== Performance Profiling Summary ===\n");
        printf("Profile mode: %s\n", profile_mode_to_string(profile_mode));
        printf("Total compilation time: %.3f seconds\n", compilation_stats.total_time);
        
        if (profile_mode >= PROFILE_DETAILED) {
            printf("\nPhase breakdown:\n");
            printf("  Lexical analysis: %.3f seconds (%.1f%%)\n", 
                   compilation_stats.lexical_time,
                   compilation_stats.total_time > 0 ? (compilation_stats.lexical_time / compilation_stats.total_time * 100) : 0);
            printf("  Syntax analysis: %.3f seconds (%.1f%%)\n",
                   compilation_stats.syntax_time,
                   compilation_stats.total_time > 0 ? (compilation_stats.syntax_time / compilation_stats.total_time * 100) : 0);
            printf("  Semantic analysis: %.3f seconds (%.1f%%)\n",
                   compilation_stats.semantic_time,
                   compilation_stats.total_time > 0 ? (compilation_stats.semantic_time / compilation_stats.total_time * 100) : 0);
            printf("  Optimization: %.3f seconds (%.1f%%)\n",
                   compilation_stats.optimization_time,
                   compilation_stats.total_time > 0 ? (compilation_stats.optimization_time / compilation_stats.total_time * 100) : 0);
            printf("  Code generation: %.3f seconds (%.1f%%)\n",
                   compilation_stats.codegen_time,
                   compilation_stats.total_time > 0 ? (compilation_stats.codegen_time / compilation_stats.total_time * 100) : 0);
        }
        
        if (profile_mode >= PROFILE_FULL) {
            printf("\nPerformance metrics:\n");
            printf("  Parsing speed: %.2f chars/sec\n", compilation_stats.parsing_speed);
            printf("  Codegen speed: %.2f instr/sec\n", compilation_stats.codegen_speed);
            printf("  Memory allocated: %zu bytes\n", compilation_stats.total_memory_allocated);
            printf("  Peak memory: %zu bytes\n", compilation_stats.peak_memory_usage);
        }
        
        printf("=====================================\n");
    }

    tac_free(&tac);
    semantic_destroy(&sem);
    ast_free(root);
    free(source);
    
    /* Calculate total compilation time */
    clock_t end_time = clock();
    compilation_stats.total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    
    /* Calculate performance metrics */
    if (compilation_stats.source_characters > 0 && compilation_stats.total_time > 0) {
        compilation_stats.parsing_speed = (double)compilation_stats.source_characters / compilation_stats.total_time;
    }
    if (compilation_stats.total_tac_instructions > 0 && compilation_stats.codegen_time > 0) {
        compilation_stats.codegen_speed = (double)compilation_stats.total_tac_instructions / compilation_stats.codegen_time;
    }
    
    /* Collect memory usage statistics */
    compilation_stats.total_memory_allocated = get_total_memory_allocated();
    compilation_stats.peak_memory_usage = get_peak_memory_usage();
    
    return EXIT_SUCCESS;
}
