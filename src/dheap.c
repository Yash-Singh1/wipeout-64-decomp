/*
 * dheap.c — Dynamic heap allocator (DInit / DAlloc / DFree / DCheckHeap).
 *
 * Doubly-linked free/allocated block arena ported from Wipeout XL. DAlloc
 * records __FILE__ and __LINE__ in retail calls (used for diagnostics and
 * proving original source TU boundaries).
 */
#include "dheap.h"

void DInit(DHeapBlock *heap, s32 size) {
    size -= sizeof(DHeapBlock);
    heap->used = 0;
    heap->size = size;
    heap->data = (void *)(((u32)heap + sizeof(DHeapBlock) + 15) & ~15);
    heap->next = NULL;
    heap->freeBytes = size;
    heap->magic = DHEAP_MAGIC;
}

void DCheckHeap(DHeapBlock *heap) {
    DHeapBlock *cur;

    cur = heap->next;
    while (cur != NULL) {
        if (cur->magic != DHEAP_MAGIC) {
            cur = ((DHeapBlock *)0)->next;
        }
        cur = cur->next;
    }
}

void *DAlloc(DHeapBlock *heap, s32 size, const char *file, s32 line) {
    DHeapBlock *cur;
    DHeapBlock *best;
    DHeapBlock *block;
    s32 bestSize;

    best = NULL;
    bestSize = 0x800000;
    size += 15;
    size &= ~15;

    cur = heap;
    while (cur != NULL) {
        if ((u32)cur->size >= (u32)(size + (s32)sizeof(DHeapBlock)) && cur->size < bestSize && !cur->used) {
            bestSize = cur->size;
            best = cur;
        }
        if (cur->size == size && !cur->used) {
            cur->used = 1;
            cur->magic = DHEAP_MAGIC;
            cur->line = line;
            cur->fileId = DRegisterFile(file);
            heap->freeBytes -= size;
            return cur->data;
        }
        cur = cur->next;
    }

    if (best == NULL) {
        return NULL;
    }
    cur = best;

    cur->size -= sizeof(DHeapBlock) + size;
    block = (DHeapBlock *)(((u32)cur->data + cur->size + 15) & ~15);
    block->used = 1;
    block->size = size;
    block->magic = DHEAP_MAGIC;
    block->data = (void *)((u32)block + sizeof(DHeapBlock));
    block->next = cur->next;
    block->line = line;
    block->fileId = DRegisterFile(file);
    cur->next = block;

    heap->freeBytes -= sizeof(DHeapBlock) + size;
    if (heap == g_mainHeap) {
        if (heap->freeBytes < g_heapMinFree) {
            g_heapMinFree = heap->freeBytes;
        }
    }

    return block->data;
}

s32 DMemCmp(const char *a, const char *b, s32 len) {
    s32 i;

    for (i = 0; i < len; i++) {
        if (b[i] != a[i]) {
            return 1;
        }
    }
    return 0;
}

s32 DRegisterFile(const char *file) {
    s32 i;
    char *dst;
    const char *p;

    if (g_allocFileCount == 0) {
        p = file;
        dst = g_allocFileNames[g_allocFileCount];
        while (*p != '.') {
            *dst++ = *p++;
        }
        *dst = '\0';
        return g_allocFileCount++;
    } else {
        p = file;
        for (i = 0; i < g_allocFileCount; i++) {
            if (DMemCmp(p, g_allocFileNames[i], strlen(g_allocFileNames[i])) == 0) {
                return i;
            }
        }
        dst = g_allocFileNames[g_allocFileCount];
        while (*p != '.') {
            *dst++ = *p++;
        }
        *dst = '\0';
        return g_allocFileCount++;
    }
}

void DFree(DHeapBlock *heap, void *ptr) {
    DHeapBlock *cur;
    DHeapBlock *prev;
    DHeapBlock *next;

    cur = heap->next;
    prev = heap;
    while (cur != NULL) {
        if (cur->data == ptr) {
            next = cur->next;
            heap->freeBytes += cur->size;
            if (next != NULL && !next->used) {
                cur->size += sizeof(DHeapBlock) + next->size;
                cur->next = next->next;
                heap->freeBytes += sizeof(DHeapBlock);
            }
            if (prev->used == 1) {
                cur->used = 0;
            } else {
                prev->size += sizeof(DHeapBlock) + cur->size;
                prev->next = cur->next;
                heap->freeBytes += sizeof(DHeapBlock);
            }
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}
