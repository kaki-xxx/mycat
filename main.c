/**
 * @file main.c
 * @brief プログラムのエントリポイント.
 * @details コマンドライン引数のパース, 使い方の表示, バージョン情報の表示など.
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "config.h"
#include "die.h"
#include "cat.h"

/**
 * @brief 文字列sが文字列tで始まるかどうか.
 */
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

/**
 * @brief コマンドライン引数のパース.
 * @details POSIXのgetoptのエミュレーション. opterrは未実装.
 */
int getopt(int argc, char* const argv[], const char* optstring) {
    if (optind >= argc) {
        return -1;
    }
    int i = optind;
    while (i < argc && (!starts_with(argv[i], "-") || strlen(argv[i]) <= 1)) {
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

/**
 * @brief コマンドの使い方を表示.
 */
void show_usage() {
    printf("Usage: cat [OPTION]... [FILE]...\n"
           "Concatenate FILE(s) to standard output.\n"
           "\n"
           "With no FILE, or when FILE is -, read standard input.\n"
           "\n"
           "  -n         number all output lines\n"
           "  --help     display this help and exit\n"
           "  --version  output version information and exit\n");
}

/**
 * @brief バージョン情報を表示.
 */
void show_version() {
    printf("cat %d.%d\n", CAT_VERSION_MAJOR, CAT_VERSION_MINOR);
}

//! プログラムの名前(エラー出力用)
const char* prog_name;

/**
 * @brief ロングオプション(--で始まるオプション)の簡易的な実装.
 * @details --helpか--versionが指定された場合は対応した関数を呼び出して終了.
 *          それ以外のロングオプションはエラーメッセージを表示して終了.
 */
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

//! 行番号を表示するかどうかのオプション.
bool line_num = false;

int main(int argc, char *argv[]) {
    prog_name = argv[0];
    find_long_opt(argc, argv);
    char c;
    while ((c = getopt(argc, argv, "n")) != -1) {
        if (c == 'n') {
            line_num = true;
        } else if (c == '?') {
            die("invalid option -- '%c'\n"
                "Try '%s --help' for more information.\n", optopt, prog_name);
        }
    }
    int n = argc - optind;
    if (n == 0 || n == 1 && !strcmp(argv[optind], "-")) {
        do_cat_file(stdin, "stdin");
    } else {
        do_cat((const char **)&argv[optind], argc - optind);
    }
    return 0;
}
