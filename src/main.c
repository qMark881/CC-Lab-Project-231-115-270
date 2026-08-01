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

#define COMPILER_VERSION "CC Lab Mini Compiler 2.0"
#define COMPILER_NAME "CC Lab Mini Compiler"

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

static int count_source_lines(const char *source) {
    if (!source || source[0] == '\0') return 0;
    int lines = 1;
    for (const char *cursor = source; *cursor; ++cursor) {
        if (*cursor == '\n') lines++;
    }
    return lines;
}

static void print_profile_summary(const CompilationStats *stats) {
    printf("\nPerformance Profile\n");
    printf("-------------------\n");
    printf("Mode: %s\n", profile_mode_to_string(profile_mode));
    printf("Total: %.6f s\n", stats->total_time);
    if (profile_mode >= PROFILE_DETAILED) {
        printf("Lexer: %.6f s\n", stats->lexical_time);
        printf("Parser/AST: %.6f s\n", stats->syntax_time);
        printf("Semantic: %.6f s\n", stats->semantic_time);
        printf("TAC generation: %.6f s\n", stats->codegen_time);
    }
    if (profile_mode >= PROFILE_FULL) {
        printf("Tokens: %d\n", stats->total_tokens);
        printf("AST nodes: %d\n", stats->total_ast_nodes);
        printf("Symbols: %d\n", stats->total_symbols);
        printf("TAC instructions: %d\n", stats->total_tac_instructions);
        printf("Tracked allocation volume: %zu bytes\n", stats->total_memory_allocated);
    }
}

int main(int argc, char **argv) {
    const char *source_path = NULL;

    for (int i = 1; i < argc; ++i) {
        const char *arg = argv[i];
        if (strcmp(arg, "--help") == 0 || strcmp(arg, "-h") == 0) {
            print_usage(argv[0]);
            return EXIT_SUCCESS;
        } else if (strcmp(arg, "--version") == 0 || strcmp(arg, "-v") == 0) {
            puts(COMPILER_VERSION);
            return EXIT_SUCCESS;
        } else if (strcmp(arg, "--logo") == 0 || strcmp(arg, "-L") == 0) {
            show_logo = true;
        } else if (strcmp(arg, "--verbose") == 0 || strcmp(arg, "-V") == 0) {
            verbose_mode = true;
        } else if (strcmp(arg, "--stats") == 0 || strcmp(arg, "-S") == 0) {
            show_stats = true;
        } else if (strcmp(arg, "--format") == 0 || strcmp(arg, "-F") == 0) {
            format_source = true;
        } else if (strcmp(arg, "--json") == 0 || strcmp(arg, "-J") == 0) {
            json_output = true;
            show_stats = true;
        } else if (strcmp(arg, "--csv") == 0 || strcmp(arg, "-C") == 0) {
            csv_output = true;
            show_stats = true;
        } else if (strcmp(arg, "--output") == 0 || strcmp(arg, "-o") == 0) {
            if (++i >= argc) {
                fprintf(stderr, "Error: %s requires a file path.\n", arg);
                return EXIT_FAILURE;
            }
            output_file = argv[i];
        } else if (strcmp(arg, "--profile") == 0 || strcmp(arg, "-P") == 0) {
            if (++i >= argc) {
                fprintf(stderr, "Error: %s requires none, basic, detailed, or full.\n", arg);
                return EXIT_FAILURE;
            }
            const char *mode = argv[i];
            if (strcmp(mode, "none") == 0) profile_mode = PROFILE_NONE;
            else if (strcmp(mode, "basic") == 0) profile_mode = PROFILE_BASIC;
            else if (strcmp(mode, "detailed") == 0) profile_mode = PROFILE_DETAILED;
            else if (strcmp(mode, "full") == 0) profile_mode = PROFILE_FULL;
            else {
                fprintf(stderr, "Error: invalid profile mode '%s'.\n", mode);
                return EXIT_FAILURE;
            }
        } else if (strcmp(arg, "-") == 0 || arg[0] != '-') {
            if (source_path) {
                fprintf(stderr, "Error: provide exactly one source file.\n");
                return EXIT_FAILURE;
            }
            source_path = arg;
        } else {
            fprintf(stderr, "Error: unknown option '%s'.\n", arg);
            print_usage(argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (show_logo) {
        print_logo();
        if (!source_path) return EXIT_SUCCESS;
    }
    if (!source_path) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }
    if (json_output && csv_output) {
        fprintf(stderr, "Error: --json and --csv are mutually exclusive.\n");
        return EXIT_FAILURE;
    }

    reset_memory_tracking();
    stats_init(&compilation_stats);
    clock_t total_start = clock();

    char *source = load_source_text(source_path);
    if (!source) {
        fprintf(stderr, "Error: could not read '%s'.\n", source_path);
        return EXIT_FAILURE;
    }
    compilation_stats.source_characters = (int)strlen(source);
    compilation_stats.source_lines = count_source_lines(source);
    compilation_stats.dependency_count = 1;

    if (format_source) {
        format_source_code(source);
        free(source);
        return EXIT_SUCCESS;
    }

    if (verbose_mode) {
        printf("Compiler: %s\n", COMPILER_VERSION);
        printf("Input: %s (%d lines, %d characters)\n", source_path,
               compilation_stats.source_lines, compilation_stats.source_characters);
    }

    Parser parser;
    parser_init(&parser, source);
    clock_t parse_start = clock();
    ASTNode *root = parse_program(&parser);
    double parse_total = (double)(clock() - parse_start) / CLOCKS_PER_SEC;

    int parser_errors = parser.error_count;
    int lexical_errors = parser.lexical_error_count;
    int syntax_errors = parser.syntax_error_count;
    int token_count = parser.token_count;
    double lexical_time = parser.lexical_time;
    parser_destroy(&parser);

    compilation_stats.total_tokens = token_count;
    compilation_stats.lexical_errors = lexical_errors;
    compilation_stats.syntax_errors = syntax_errors;
    compilation_stats.lexical_time = lexical_time;
    compilation_stats.syntax_time = parse_total > lexical_time ? parse_total - lexical_time : 0.0;
    compilation_stats.total_ast_nodes = (int)ast_count_nodes(root);

    if (parser_errors > 0) {
        fprintf(stderr, "\nCompilation failed: %d lexical error(s), %d syntax error(s).\n",
                lexical_errors, syntax_errors);
        ast_free(root);
        free(source);
        return EXIT_FAILURE;
    }

    SemanticContext sem;
    semantic_init(&sem);
    clock_t semantic_start = clock();
    semantic_analyze(root, &sem);
    compilation_stats.semantic_time = (double)(clock() - semantic_start) / CLOCKS_PER_SEC;
    compilation_stats.semantic_errors = sem.error_count;
    compilation_stats.warning_count = sem.warning_count;
    compilation_stats.total_symbols = sem.table.symbol_count;
    compilation_stats.total_scopes = sem.table.scopes_entered;

    if (sem.error_count > 0) {
        fprintf(stderr, "\nCompilation failed: %d semantic error(s).\n", sem.error_count);
        semantic_destroy(&sem);
        ast_free(root);
        free(source);
        return EXIT_FAILURE;
    }

    TacProgram tac;
    tac_init(&tac);
    clock_t codegen_start = clock();
    tac_generate(root, &tac);
    compilation_stats.codegen_time = (double)(clock() - codegen_start) / CLOCKS_PER_SEC;
    compilation_stats.total_tac_instructions = (int)tac.count;
    bool tac_valid = tac_validate(&tac);

    compilation_stats.total_time = (double)(clock() - total_start) / CLOCKS_PER_SEC;
    if (compilation_stats.total_time > 0.0) {
        compilation_stats.parsing_speed =
            (double)compilation_stats.source_characters / compilation_stats.total_time;
    }
    if (compilation_stats.codegen_time > 0.0) {
        compilation_stats.codegen_speed =
            (double)compilation_stats.total_tac_instructions / compilation_stats.codegen_time;
    }
    compilation_stats.total_memory_allocated = get_total_memory_allocated();
    compilation_stats.peak_memory_usage = get_peak_memory_usage();

    FILE *saved_stdout = stdout;
    FILE *redirected_output = NULL;
    if (output_file) {
        redirected_output = fopen(output_file, "w");
        if (!redirected_output) {
            fprintf(stderr, "Error: could not open output file '%s'.\n", output_file);
            tac_free(&tac);
            semantic_destroy(&sem);
            ast_free(root);
            free(source);
            return EXIT_FAILURE;
        }
        stdout = redirected_output;
    }

    print_compilation_stage_header("Abstract Syntax Tree");
    ast_print(root, 0);

    print_compilation_stage_header("Visual AST");
    ast_print_visual(root, "", true);

    print_compilation_stage_header("Symbol Table");
    symtab_print_detailed(&sem.table);

    print_compilation_stage_header("Three Address Code");
    tac_print(&tac);

    if (verbose_mode) {
        print_compilation_stage_header("Readable AST Summary");
        ast_print_summary(root, 0);
        printf("\nTAC validation: %s\n", tac_valid ? "PASSED" : "FAILED");
        if (!tac_valid) tac_print_validation_errors(&tac);
    }

    if (tac_valid) {
        print_success_footer();
    } else {
        print_compilation_stage_header("Compilation Result");
        puts("Compilation failed because generated TAC did not pass validation.");
    }

    if (show_stats) {
        if (json_output) stats_print_json(&compilation_stats);
        else if (csv_output) stats_print_csv(&compilation_stats);
        else stats_print(&compilation_stats);
    }
    if (profile_mode != PROFILE_NONE) {
        print_profile_summary(&compilation_stats);
    }

    fflush(stdout);
    if (redirected_output) {
        fclose(redirected_output);
        stdout = saved_stdout;
        if (verbose_mode) printf("Output written to: %s\n", output_file);
    }

    tac_free(&tac);
    semantic_destroy(&sem);
    ast_free(root);
    free(source);
    return tac_valid ? EXIT_SUCCESS : EXIT_FAILURE;
}
