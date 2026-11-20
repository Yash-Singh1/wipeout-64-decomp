#ifndef WIPEOUT64_DHEAP_H
#define WIPEOUT64_DHEAP_H

#include "common.h"

#define DHEAP_MAGIC 0x12345678

typedef struct DHeapBlock {
    /* 0x00 */ s32 used;
    /* 0x04 */ s32 size;
    /* 0x08 */ s32 freeBytes;
    /* 0x0C */ void *data;
    /* 0x10 */ struct DHeapBlock *next;
    /* 0x14 */ u32 magic;
    /* 0x18 */ s32 line;
    /* 0x1C */ s32 fileId;
} DHeapBlock;

extern DHeapBlock *g_heap;
extern DHeapBlock *g_mainHeap;
extern s32 g_heapMinFree;
extern u8 g_allocFileCount;
extern char g_allocFileNames[][12];

u32 strlen(const char *s);

void DInit(DHeapBlock *heap, s32 size);
void DCheckHeap(DHeapBlock *heap);
void *DAlloc(DHeapBlock *heap, s32 size, const char *file, s32 line);
s32 DMemCmp(const char *a, const char *b, s32 len);
s32 DRegisterFile(const char *file);
void DFree(DHeapBlock *heap, void *ptr);

#endif /* WIPEOUT64_DHEAP_H */
