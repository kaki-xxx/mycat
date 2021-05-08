#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

extern const char* prog_name;

void die(const char* fmt, ...) {
    fprintf(stderr, "%s: ", prog_name);
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    exit(EXIT_FAILURE);
}
