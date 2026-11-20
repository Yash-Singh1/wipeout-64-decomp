#ifndef WIPEOUT64_COMMON_H
#define WIPEOUT64_COMMON_H

typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef signed long long s64;
typedef unsigned long long u64;

typedef volatile s8 vs8;
typedef volatile u8 vu8;
typedef volatile s16 vs16;
typedef volatile u16 vu16;
typedef volatile s32 vs32;
typedef volatile u32 vu32;
typedef volatile s64 vs64;
typedef volatile u64 vu64;

typedef float f32;
typedef double f64;

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
