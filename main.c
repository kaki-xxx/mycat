#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

const char* prog_name;

void die(const char* fmt, ...) {
    fprintf(stderr, "%s: ", prog_name);
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    exit(EXIT_FAILURE);
}

void do_cat(const char *path[], size_t n) {
    for (int i = 0; i < n; i++) {
        errno = 0;
        FILE *fp = fopen(path[i], "r");
        if (fp == NULL) {
            die("%s: %s\n", path[i], strerror(errno));
        }
        char buf[256];
        do {
            errno = 0;
            size_t fwb = fread(buf, sizeof(*buf), ARRAY_SIZE(buf), fp);
            if (fwb < ARRAY_SIZE(buf) && !feof(fp)) {
                die("%s: %s\n", path[i], strerror(errno));
            }
            errno = 0;
            size_t ret = fwrite(buf, sizeof(*buf), fwb, stdout);
            if (fwb < ret) {
                die("%s: %s\n", path[i], strerror(errno));
            }
        } while (!feof(fp));
        errno = 0;
        if (fclose(fp) == EOF) {
            die("%s: %s\n", path[i], strerror(errno));
        }
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
int optind = 1, optopt;
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
    int prev_opt = argv[optind][strind];
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
        optopt = prev_opt;
        return '?';
    }
    return *optstring;
}

void show_usage() {
    printf("Usage: cat [OPTION]... [FILE]...\n"
           "Concatenate FILE(s) to standard output.\n"
           "\n"
           "With no FILE, or when FILE is -, read standard input.\n");
}

void find_long_opt(int argc, char* const argv[]) {
    int i = 1;
    while (i < argc && !starts_with(argv[i], "--")) {
        i++;
    }
    if (i >= argc) {
        return;
    }
    if (!strcmp(&argv[i][2], "help")) {
        show_usage();
        exit(0);
    } else {
        die("invalid option -- '%s'\n"
            "Try '%s --help' for more information.\n", argv[i], prog_name);
    }
}

int main(int argc, char *argv[]) {
    prog_name = argv[0];
    find_long_opt(argc, argv);
    char c;
    while ((c = getopt(argc, argv, "abc:")) != -1) {
        if (c == 'c') {
            printf("%c %s\n", c, optarg);
        } else if (c == '?') {
            die("invalid option -- '%c'\n"
                "Try '%s --help' for more information.\n", optopt, prog_name);
        }
    }
    while (optind < argc) {
        printf("%s\n", argv[optind]);
        optind++;
    }
    return 0;
}
