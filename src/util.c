#include "util.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *xmalloc(size_t size) {
    void *ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr, "Fatal: out of memory\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void *xrealloc(void *ptr, size_t size) {
    void *out = realloc(ptr, size);
    if (!out) {
        fprintf(stderr, "Fatal: out of memory\n");
        exit(EXIT_FAILURE);
    }
    return out;
}

char *xstrdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = (char *)xmalloc(len + 1);
    memcpy(copy, s, len + 1);
    return copy;
}

char *read_entire_file(const char *path) {
    FILE *fp = fopen(path, "rb");
    if (!fp) return NULL;
    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return NULL;
    }
    long size = ftell(fp);
    if (size < 0) {
        fclose(fp);
        return NULL;
    }
    rewind(fp);
    char *buffer = (char *)xmalloc((size_t)size + 1);
    size_t read_bytes = fread(buffer, 1, (size_t)size, fp);
    buffer[read_bytes] = '\0';
    fclose(fp);
    return buffer;
}

static char *trim_copy(const char *start, const char *end) {
    while (start < end && (*start == '\n' || *start == '\r' || isspace((unsigned char)*start))) {
        start++;
    }
    while (end > start && (end[-1] == '\n' || end[-1] == '\r' || isspace((unsigned char)end[-1]))) {
        end--;
    }
    size_t len = (size_t)(end - start);
    char *out = (char *)xmalloc(len + 1);
    memcpy(out, start, len);
    out[len] = '\0';
    return out;
}

char *extract_code_block(const char *text) {
    if (!text) return xstrdup("");
    const char *f1 = strstr(text, "```");
    if (!f1) return xstrdup(text);
    const char *after_f1 = strchr(f1 + 3, '\n');
    if (!after_f1) return xstrdup(text);
    after_f1++;

    const char *f2 = strstr(after_f1, "```");
    if (!f2) return xstrdup(text);
    return trim_copy(after_f1, f2);
}
