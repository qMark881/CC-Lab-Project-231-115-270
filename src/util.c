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

/* Get error context around a specific line */
char *get_error_context(const char *source, int error_line, int context_lines) {
    if (!source || error_line < 1) return xstrdup("");
    
    /* Count lines in source */
    int total_lines = 1;
    for (size_t i = 0; i < strlen(source); i++) {
        if (source[i] == '\n') total_lines++;
    }
    
    /* Calculate context range */
    int start_line = error_line - context_lines;
    if (start_line < 1) start_line = 1;
    int end_line = error_line + context_lines;
    if (end_line > total_lines) end_line = total_lines;
    
    /* Extract context lines */
    size_t buffer_size = 1024;
    char *context = (char *)xmalloc(buffer_size);
    size_t context_len = 0;
    
    int current_line = 1;
    size_t i = 0;
    
    while (source[i] != '\0' && current_line <= end_line) {
        if (current_line >= start_line) {
            /* Add line number prefix */
            char line_prefix[32];
            snprintf(line_prefix, sizeof(line_prefix), "%3d | ", current_line);
            size_t prefix_len = strlen(line_prefix);
            
            /* Ensure buffer has space */
            if (context_len + prefix_len + 256 >= buffer_size) {
                buffer_size *= 2;
                context = (char *)xrealloc(context, buffer_size);
            }
            
            /* Add line prefix */
            memcpy(context + context_len, line_prefix, prefix_len);
            context_len += prefix_len;
            
            /* Add line content */
            while (source[i] != '\0' && source[i] != '\n') {
                if (context_len + 1 >= buffer_size) {
                    buffer_size *= 2;
                    context = (char *)xrealloc(context, buffer_size);
                }
                context[context_len++] = source[i++];
            }
            
            /* Add newline */
            if (context_len + 1 >= buffer_size) {
                buffer_size *= 2;
                context = (char *)xrealloc(context, buffer_size);
            }
            context[context_len++] = '\n';
            
            /* Add error marker for error line */
            if (current_line == error_line) {
                char marker[64];
                snprintf(marker, sizeof(marker), "    ^");
                size_t marker_len = strlen(marker);
                
                if (context_len + marker_len >= buffer_size) {
                    buffer_size *= 2;
                    context = (char *)xrealloc(context, buffer_size);
                }
                memcpy(context + context_len, marker, marker_len);
                context_len += marker_len;
                
                if (context_len + 1 >= buffer_size) {
                    buffer_size *= 2;
                    context = (char *)xrealloc(context, buffer_size);
                }
                context[context_len++] = '\n';
            }
        }
        
        if (source[i] == '\n') {
            current_line++;
            i++;
        }
    }
    
    context[context_len] = '\0';
    return context;
}

/* Print error context to stderr */
void print_error_context(const char *source, int error_line, int context_lines) {
    char *context = get_error_context(source, error_line, context_lines);
    fprintf(stderr, "Error context (showing %d lines around line %d):\n", 
            context_lines * 2 + 1, error_line);
    fprintf(stderr, "%s", context);
    free(context);
}
