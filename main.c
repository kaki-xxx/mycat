#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

void die(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    exit(EXIT_FAILURE);
}

void do_cat(const char *path) {
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        die("Can't open: %s\n", path);
    }
    char buf[256];
    do {
        size_t ret = fread(buf, sizeof(*buf), ARRAY_SIZE(buf), fp);
        fwrite(buf, sizeof(*buf), ret, stdout);
    } while (!feof(fp));
    fclose(fp);
}

int main(int argc, char *argv[]) {
    do_cat(argv[1]);
    return 0;
}
