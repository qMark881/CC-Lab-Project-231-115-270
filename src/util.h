#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

void *xmalloc(size_t size);
void *xrealloc(void *ptr, size_t size);
char *xstrdup(const char *s);
char *read_entire_file(const char *path);
char *extract_code_block(const char *text);

#endif
