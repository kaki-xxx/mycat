#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

void die(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    exit(EXIT_FAILURE);
}

void do_cat(const char *path[], size_t n) {
    for (int i = 0; i < n; i++) {
        FILE *fp = fopen(path[i], "r");
        if (fp == NULL) {
            die("%s: No such file or directory\n", path[i]);
        }
        char buf[256];
        do {
            size_t ret = fread(buf, sizeof(*buf), ARRAY_SIZE(buf), fp);
            fwrite(buf, sizeof(*buf), ret, stdout);
        } while (!feof(fp));
        fclose(fp);
    }
}

static bool starts_with(const char *s, const char *t) {
    if (strlen(s) < strlen(t)) {
        return false;
    }
    while (*t) {
        if (*s != *t) {
            return false;
        }
        s++;
        t++;
    }
    return true;
}

static inline void swap(char **a, char **b) {
    char *tmp = *a;
    *a = *b;
    *b = tmp;
}

char* optarg;
int optind = 1;
static int strind = 1;

int getopt(int argc, char* const argv[], const char* optstring) {
    if (optind >= argc) {
        return -1;
    }
    int i = optind;
    while (i < argc && !starts_with(argv[i], "-")) {
        i++;
    }
    if (i >= argc) {
        return -1;
    }
    swap((char **)&argv[optind], (char **)&argv[i]);
    while (*optstring) {
        if (argv[optind][strind] == *optstring) {
            break;
        }
        optstring++;
    }
    strind++;
    if (!argv[optind][strind]) {
        optind++;
        strind = 1;
    }
    if (*optstring && optstring[1] == ':') {
        optarg = &argv[optind][strind == 1 ? 0 : strind];
        optind++;
        strind = 1;
    }
    if (!*optstring) {
        return '?';
    }
    return *optstring;
}

int main(int argc, char *argv[]) {
    char c;
    while ((c = getopt(argc, argv, "abc:")) != -1) {
        if (c == 'c') {
            printf("%c %s\n", c, optarg);
        } else {
            printf("%c\n", c);
        }
    }
    while (optind < argc) {
        printf("%s\n", argv[optind]);
        optind++;
    }
    return 0;
}
