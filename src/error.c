/*
 * Error.c — Fatal error trap and string utility routines.
 *
 * Error() executes the SN64 debugger trap (`break 0x407`); StringCopy and
 * small string helpers live in the same translation unit.
 */
#include "error.h"

/* Sets the global error-status flag (first function in Error.c). */
void SetErrorFlag(void) {
    g_errorFlag = 1;
}

/* Original Psy-Q N64 Error() invokes an inline SN64 debugger breakpoint. */
void Error(const char *msg, s16 errorLevel) {
    __asm__("break 0x407");
}

char *StringCopy(char *dst, const char *src) {
    while ((*dst++ = *src++) != '\0') {
    }
    return dst;
}

char *StringCat(char *dst, const char *src) {
    while (*dst != '\0') {
        dst++;
    }
    StringCopy(dst, src);
    return dst;
}

char *StringAlloc(const char *str) {
    char *copy;

    copy = (char *)DAlloc(g_heap, strlen(str) + 1, "Error.c", 78);
    StringCopy(copy, str);
    return copy;
}

void StringFree(void *ptr) {
    DFree(g_heap, ptr);
}
