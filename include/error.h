#ifndef WIPEOUT64_ERROR_H
#define WIPEOUT64_ERROR_H

#include "common.h"
#include "dheap.h"

void Error(const char *msg, s16 errorLevel);
char *StringCopy(char *dst, const char *src);
char *StringCat(char *dst, const char *src);
char *StringAlloc(const char *str);
void StringFree(void *ptr);

#endif /* WIPEOUT64_ERROR_H */
