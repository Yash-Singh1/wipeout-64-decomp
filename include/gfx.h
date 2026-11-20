#ifndef WIPEOUT64_GFX_H
#define WIPEOUT64_GFX_H

#include "common.h"
#include "dheap.h"
#include "n64init.h"
#include <ultra64.h>
#include <PR/sched.h>

#ifdef VERSION_EU
#define GLIST_LEN 0x900
#else
#define GLIST_LEN 0x880
#endif

#define MTX_POOL_LEN    200
#define NUM_GFX_TASKS   9
#define NUM_TASK_MSGS   7
#define NUM_INIT_DLS    6
#define INIT_DL_LEN     20

typedef struct Dynamic {
    /* 0x0000 */ Mtx projection;
    /* 0x0040 */ Mtx ortho;
    /* 0x0080 */ Mtx modeling1;
    /* 0x00C0 */ Mtx modeling2;
    /* 0x0100 */ Gfx glist[GLIST_LEN];
    /* 0x4500 / 0x4900 */ Mtx mtxPool[MTX_POOL_LEN];
} Dynamic; /* size = 0x7700 (US) / 0x7B00 (EU) */

typedef struct GFXInfo {
    /* 0x0000 */ OSScTask task[NUM_GFX_TASKS];
    /* 0x03A8 */ Dynamic dp;
    /* 0x7AA8 / 0x7EA8 */ OSScMsg doneMsg;
    /* 0x7AC8 / 0x7EC8 */ OSScMsg taskMsg[NUM_TASK_MSGS];
    /* 0x7BA8 / 0x7FA8 */ u16 *cfb;
    /* 0x7BAC / 0x7FAC */ s32 unk_7BAC;
    /* 0x7BB0 / 0x7FB0 */ s32 mode;
    /* 0x7BB4 / 0x7FB4 */ s32 flags;
} GFXInfo; /* size = 0x7BB8 (US) / 0x7FB8 (EU) */

/*
 * Temporary symbol-name mappings for unnamed externals/functions until they
 * are added to config/{us,eu}/symbol_addrs.txt and config/{us,eu}/syms.ld.
 */
#ifdef VERSION_EU
#define SubmitGfxTasks              func_80010448
#define BuildInitDisplayList        func_80010564
#define BuildGfx2DInitDisplayList   func_8003CB88
#define _staticSegmentRomStart      D_B00988C0
#define _staticSegmentRomEnd        D_B0098B10
#define g_gfxSubmitCount            D_800895F8
#define g_gfxFrameCount             D_800A69C0
#define g_staticSegment             D_800A69C4
#define g_initDls                   D_800CB9D0
#define g_gfxInfo                   D_800CEA08
#define g_zBuffer                   D_800F8400
#else
#define SubmitGfxTasks              func_80009618
#define BuildInitDisplayList        func_8000973C
#define BuildGfx2DInitDisplayList   func_80035498
#define _staticSegmentRomStart      D_B0099300
#define _staticSegmentRomEnd        D_B0099550
#define g_gfxSubmitCount            D_80089FC8
#define g_gfxFrameCount             D_800993E0
#define g_staticSegment             D_800993E4
#define g_initDls                   D_800CB690
#define g_gfxInfo                   D_800CE6C8
#define g_zBuffer                   D_800FF500
#endif

#define g_cfb0                      D_803B5000
#define g_cfb1                      D_803DA800

extern u8 _staticSegmentRomStart[];
extern u8 _staticSegmentRomEnd[];

extern s32 g_gfxSubmitCount;
extern s32 g_gfxFrameCount;
extern void *g_staticSegment;
extern Gfx g_initDls[NUM_INIT_DLS][INIT_DL_LEN];
extern GFXInfo g_gfxInfo[2];
extern u16 g_zBuffer[];
extern u16 g_cfb0[];
extern u16 g_cfb1[];

void BuildGfx2DInitDisplayList(Gfx *dl, s32 type);

void InitGfx(void);
void SubmitGfxTasks(s32 bufferIdx);
void BuildInitDisplayList(Gfx *dl, s32 type);

#endif /* WIPEOUT64_GFX_H */
