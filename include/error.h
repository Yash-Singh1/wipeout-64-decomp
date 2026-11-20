#ifndef WIPEOUT64_ERROR_H
#define WIPEOUT64_ERROR_H

#include "common.h"

extern void *g_heap;
void *DAlloc(void *heap, s32 size, const char *file, s32 line);
void DFree(void *heap, void *ptr);
u32 strlen(const char *s);

void Error(const char *msg, s16 errorLevel);
char *StringCopy(char *dst, const char *src);
char *StringCat(char *dst, const char *src);
char *StringAlloc(const char *str);
void StringFree(void *ptr);

#endif /* WIPEOUT64_ERROR_H */
