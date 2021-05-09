/**
 * @file cat.c
 * @brief catのメイン処理.
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include "die.h"

/**
 * @brief ファイルから一行読み込む.
 * @param[in] fp 対象となるファイル.
 * @param[in] buf 書き込まれるバッファ.
 * @param[in] n 書き込まれるバイト数の上限.
 * @param[out] reached_newline 改行文字が見つかったかどうか.
 * @return int 読み込んだバイト数. ファイルに読むデータが残っていない場合はEOF.
 * @details fpからn - 1バイト以下のデータを読み込んでbufに書き込む.
 *          改行文字が見つかるとそこで読むのをやめる.
 *          bufに書き込まれる文字列は改行文字を含み, null文字で終端される.
 */
int until_newline(FILE *fp, char *buf, size_t n, bool *reached_newline) {
    if (feof(fp)) {
        return EOF;
    }
    size_t i = 0;
    char c;
    *reached_newline = false;
    while (i < n - 1 && (c = fgetc(fp)) != EOF) {
        buf[i] = c;
        i++;
        if (c == '\n') {
            *reached_newline = true;
            break;
        }
    }
    if (c == EOF && ferror(stdin)) {
        die("%s\n", strerror(errno));
    }
    buf[i] = '\0';
    return i;
}

extern bool line_num;

/**
 * @brief ファイルを標準出力に出力する.
 * @param fp 出力するファイルを表す構造体のポインタ.
 * @param path 出力するファイルのパス(エラー出力用)
 */
#define BUF_SIZE 256
void do_cat_file(FILE* fp, const char* path) {
    char buf[BUF_SIZE];
    static size_t line = 1;
    bool reached_newline = false;
    bool begin_line = true;
    int written;
    while ((written = until_newline(fp, buf, BUF_SIZE, &reached_newline)) != EOF) {
        if (line_num && begin_line && written > 0) {
            printf("%6zd  ", line);
            line++;
            begin_line = false;
        }
        printf("%s", buf);
        if (reached_newline) {
            begin_line = true;
        }
    }
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
