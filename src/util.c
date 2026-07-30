#include "util.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Memory tracking variables */
static size_t total_memory_allocated = 0;
static size_t peak_memory_usage = 0;

void *xmalloc(size_t size) {
    void *ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr, "Fatal: out of memory\n");
        exit(EXIT_FAILURE);
    }
    
    /* Update memory tracking */
    total_memory_allocated += size;
    if (total_memory_allocated > peak_memory_usage) {
        peak_memory_usage = total_memory_allocated;
    }
    
    return ptr;
}

void *xrealloc(void *ptr, size_t size) {
    void *out = realloc(ptr, size);
    if (!out) {
        fprintf(stderr, "Fatal: out of memory\n");
        exit(EXIT_FAILURE);
    }
    
    /* Update memory tracking (approximate) */
    total_memory_allocated += size;
    if (total_memory_allocated > peak_memory_usage) {
        peak_memory_usage = total_memory_allocated;
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

char *read_entire_stream(FILE *fp) {
    if (!fp) return NULL;

    size_t cap = 4096;
    size_t len = 0;
    char *buffer = (char *)xmalloc(cap);

    for (;;) {
        if (cap - len < 1024) {
            cap *= 2;
            buffer = (char *)xrealloc(buffer, cap);
        }

        size_t room = cap - len - 1;
        size_t read_bytes = fread(buffer + len, 1, room, fp);
        len += read_bytes;

        if (read_bytes < room) {
            if (ferror(fp)) {
                free(buffer);
                return NULL;
            }
            break;
        }
    }

    buffer[len] = '\0';
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

/* Memory tracking functions */
size_t get_total_memory_allocated(void) {
    return total_memory_allocated;
}

size_t get_peak_memory_usage(void) {
    return peak_memory_usage;
}

void reset_memory_tracking(void) {
    total_memory_allocated = 0;
    peak_memory_usage = 0;
}
