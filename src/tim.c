/*
 * Tim.c — Standalone and WAD-backed TIM texture file loaders.
 *
 * Reads TIM texture blobs from cartridge ROM or open WAD archives into the
 * dynamic heap and hands them off to NewLoadTexture().
 */
#include "tim.h"

Texture *LoadTexture(u32 fileOffset, s16 translucent) {
    Tim *tim;
    s32 length;

    length = FileLength(fileOffset);
    if (length <= 0) {
        /*
         * Retail loads a word from offset 0 of the file handle on these error
         * paths (same port quirk as LoadPrm and LoadCompressedTextureSequence).
         */
        sprintf(errorString, "Tim.c:LoadTexture: File %s not found\n", *(const char **)fileOffset);
        Error(errorString, Warning);
        return NULL;
    }

    tim = (Tim *)DAlloc(g_heap, length + 32, "Tim.c", 181);
    if (!tim) {
        sprintf(errorString, "Tim.c:LoadTexture(): Failed to allocate memory for file %s\n",
                *(const char **)fileOffset);
        Error(errorString, Fatal);
    }

    if (LoadFile(fileOffset, tim) != length) {
        sprintf(errorString, "Tim.c:LoadTexture(): Failed to load file %s\n",
                *(const char **)fileOffset);
        Error(errorString, Fatal);
    }

    return NewLoadTexture(tim, translucent, 0);
}

Texture *WADLoadTexture(WADFILE *wad, s16 translucent) {
    Tim *tim;

    tim = (Tim *)DAlloc(g_heap, wad->sizes[wad->current] + 32, "Tim.c", 210);
    ReadNextWadFile(wad, tim);
    return NewLoadTexture(tim, translucent, 0);
}
