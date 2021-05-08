#ifndef INCLUDED_CAT
#define INCLUDED_CAT

#include <stdio.h>
void do_cat_file(FILE* fp, const char* path);
void do_cat(const char *path[], size_t n);

#endif
