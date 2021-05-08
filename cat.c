#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "die.h"
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

void do_cat_file(FILE* fp, const char* path) {
    char buf[256];
    do {
        errno = 0;
        size_t read = fread(buf, sizeof(*buf), ARRAY_SIZE(buf), fp);
        if (read < ARRAY_SIZE(buf) && !feof(fp)) {
            die("%s: %s\n", path, strerror(errno));
        }
        errno = 0;
        size_t written = fwrite(buf, sizeof(*buf), read, stdout);
        if (written < read) {
            die("%s: %s\n", path, strerror(errno));
        }
    } while (!feof(fp));
}

void do_cat(const char *path[], size_t n) {
    for (int i = 0; i < n; i++) {
        errno = 0;
        FILE *fp = fopen(path[i], "r");
        if (fp == NULL) {
            die("%s: %s\n", path[i], strerror(errno));
        }
        do_cat_file(fp, path[i]);
        errno = 0;
        if (fclose(fp) == EOF) {
            die("%s: %s\n", path[i], strerror(errno));
        }
    }
}
