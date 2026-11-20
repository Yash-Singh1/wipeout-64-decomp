#ifndef WIPEOUT64_ERROR_H
#define WIPEOUT64_ERROR_H

#include "common.h"
#include "dheap.h"

#ifdef VERSION_EU
#define SetErrorFlag func_80015F30
#define g_errorFlag  D_80089F90
#else
#define SetErrorFlag func_8000F110
#define g_errorFlag  D_8008A960
#endif

extern s32 g_errorFlag;

void SetErrorFlag(void);
void Error(const char *msg, s16 errorLevel);
char *StringCopy(char *dst, const char *src);
char *StringCat(char *dst, const char *src);
char *StringAlloc(const char *str);
void StringFree(void *ptr);

#endif /* WIPEOUT64_ERROR_H */
