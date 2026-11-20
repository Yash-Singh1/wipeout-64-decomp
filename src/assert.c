/*
 * __assert — Runtime assertion trap.
 *
 * Retail builds halt in an infinite loop on failed assertions (preserving the
 * expression, filename, and line number in the argument registers for SN64).
 */
#include "common.h"

void __assert(const char *expr, const char *file, s32 line) {
    while (1) {
    }
}
