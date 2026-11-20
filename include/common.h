#ifndef WIPEOUT64_COMMON_H
#define WIPEOUT64_COMMON_H

#include <PR/ultratypes.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/*
 * INCLUDE_ASM lets a C translation unit mix matched C functions with
 * disassembled assembly stubs from asm/<version>/<folder>/<name>.s.
 * tools/cc.py splices the compiled C chunks and assembled stubs in order.
 */
#define INCLUDE_ASM(FOLDER, NAME)

#endif /* WIPEOUT64_COMMON_H */
