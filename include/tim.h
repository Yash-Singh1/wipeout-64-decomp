#ifndef WIPEOUT64_TIM_H
#define WIPEOUT64_TIM_H

#include "common.h"
#include "dheap.h"
#include "error.h"
#include "files.h"
#include "wadfile.h"

#ifndef Warning
#define Warning 0
#endif

#ifndef NonFatal
#define NonFatal 1
#endif

#ifndef Fatal
#define Fatal 2
#endif

/*
 * Temporary symbol aliases until LoadTexture, WADLoadTexture,
 * NewLoadTexture, and errorString are added to config/{us,eu}/symbol_addrs.txt.
 */
#ifdef VERSION_EU
#define LoadTexture    func_8003C0B0
#define WADLoadTexture func_8003C1D0
#define NewLoadTexture func_80016C48
#define errorString    D_800AE520
#else
#define LoadTexture    func_800349C0
#define WADLoadTexture func_80034AE0
#define NewLoadTexture func_8000FE28
#define errorString    D_800A0F40
#endif

typedef struct Tim {
    /* 0x00 */ s32 id;
    /* 0x04 */ s32 flags;
} Tim;

typedef struct Texture Texture;

extern char errorString[256];

int sprintf(char *dst, const char *fmt, ...);

Texture *NewLoadTexture(Tim *timPtr, s16 translucent, s32 scene);

Texture *LoadTexture(u32 fileOffset, s16 translucent);
Texture *WADLoadTexture(WADFILE *wad, s16 translucent);

#endif /* WIPEOUT64_TIM_H */
