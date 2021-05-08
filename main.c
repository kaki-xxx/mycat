#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "config.h"
#include "die.h"
#include "cat.h"
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

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

void show_version() {
    printf("cat %d.%d\n", CAT_VERSION_MAJOR, CAT_VERSION_MINOR);
}

const char* prog_name;

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
    } else if (!strcmp(&argv[i][2], "version")) {
        show_version();
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
    if (argc - optind == 0) {
        do_cat_file(stdin, "stdin");
    } else {
        do_cat((const char **)&argv[optind], argc - optind);
    }
    return 0;
}
