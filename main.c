#include <stdio.h>
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

void do_cat(const char *path) {
    FILE *fp = fopen(path, "r");
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
