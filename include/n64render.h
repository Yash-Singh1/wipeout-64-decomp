#ifndef WIPEOUT64_N64RENDER_H
#define WIPEOUT64_N64RENDER_H

#include "common.h"
#include "dheap.h"
#include <ultra64.h>

/*
 * Temporary US/EU symbol-name mappings for symbols not yet named in
 * config/{us,eu}/symbol_addrs.txt. Remove once canonical names are added there.
 */
#ifdef VERSION_EU
#define g_textureTable      D_800B2778
#define g_renderNodePool    D_800C3780
#define g_renderNodePtr     D_800C4968
#define ResetRenderList     func_800513F0
#define InitRenderList      func_80051474
#define FlushRenderList     func_80051520
#define AddRenderGroup      func_800517C0
#define AddRenderDisplayList func_80051894
#else
#define g_textureTable      D_800A5198
#define g_renderNodePool    D_800C3440
#define g_renderNodePtr     D_800C4628
#define ResetRenderList     func_8004F660
#define InitRenderList      func_8004F6E4
#define FlushRenderList     func_8004F790
#define AddRenderGroup      func_8004FA30
#define AddRenderDisplayList func_8004FB04
#endif

#define RENDER_NODE_POOL_SIZE   350
#define RENDER_UNTEXT_DL_LEN    100
#define RENDER_TEX_SKIP_FLAG    0x10000

typedef struct N64Texture {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 clutSize;
    /* 0x04 */ s16 bpp;
    /* 0x06 */ s16 width;
    /* 0x08 */ s16 height;
    /* 0x0A */ s16 unk0A;
    /* 0x0C */ void *texels;
    /* 0x10 */ void *palette;
    /* 0x14 */ u16 uScale;
    /* 0x16 */ u16 vScale;
    /* 0x18 */ s32 flags;
    /* 0x1C */ s32 index;
    /* 0x20 */ Gfx dl[17];
} N64Texture;

typedef struct RenderNode {
    /* 0x00 */ struct RenderNode *next;
    /* 0x04 */ Mtx *mtx;
    /* 0x08 */ Gfx *dl;
} RenderNode;

typedef struct RenderList {
    /* 0x00 */ u32 bucketCount;
    /* 0x04 */ u32 textureStart;
    /* 0x08 */ RenderNode *buckets;
    /* 0x0C */ RenderNode untextured;
} RenderList;

typedef struct RenderGroupEntry {
    /* 0x00 */ s32 textureIndex;
    /* 0x04 */ Gfx *dl;
} RenderGroupEntry;

typedef struct RenderGroup {
    /* 0x00 */ u32 count;
    /* 0x04 */ RenderGroupEntry *entries;
} RenderGroup;

extern N64Texture *g_textureTable[];
extern RenderNode g_renderNodePool[RENDER_NODE_POOL_SIZE];
extern RenderNode *g_renderNodePtr;

void ResetRenderList(RenderList *list);
void InitRenderList(RenderList *list, u32 textureStart, u32 bucketCount);
Gfx *FlushRenderList(RenderList *list, Gfx *glistp);
void AddRenderGroup(RenderList *list, RenderGroup *group, Mtx *mtx);
void AddRenderDisplayList(RenderList *list, Gfx **dlPtr, Mtx *mtx);

#endif /* WIPEOUT64_N64RENDER_H */
