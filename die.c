/**
 * @file die.c
 * @brief エラー処理のヘルパ関数.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

extern const char* prog_name;

/**
 * @brief エラーメッセージを標準エラー出力に出力して終了する.
 * @details 引数はprintfと同様. プログラムの終了ステータスとして異常値を返す.
 */
void die(const char* fmt, ...) {
    fprintf(stderr, "%s: ", prog_name);
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    exit(EXIT_FAILURE);
}
