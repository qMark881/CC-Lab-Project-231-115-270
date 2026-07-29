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

#endif
