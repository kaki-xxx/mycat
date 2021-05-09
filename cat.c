/**
 * @file cat.c
 * @brief catのメイン処理.
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "die.h"
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/**
 * @brief ファイルを標準出力に出力する.
 * @param fp 出力するファイルを表す構造体のポインタ.
 * @param path 出力するファイルのパス(エラー出力用)
 */
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

/**
 * @brief 指定されたパス文字列の配列が表すファイルを順に開いて標準出力へ出力.
 * @param path パス文字列の配列.
 * @param n パス文字列の個数.
 */
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
