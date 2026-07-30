#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <stdio.h>

void *xmalloc(size_t size);
void *xrealloc(void *ptr, size_t size);
char *xstrdup(const char *s);
char *read_entire_file(const char *path);
char *read_entire_stream(FILE *fp);
char *extract_code_block(const char *text);

/* Memory tracking functions */
size_t get_total_memory_allocated(void);
size_t get_peak_memory_usage(void);
void reset_memory_tracking(void);

/* Error context display functions */
char *get_error_context(const char *source, int error_line, int context_lines);
void print_error_context(const char *source, int error_line, int context_lines);

#endif
