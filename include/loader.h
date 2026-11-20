#ifndef WIPEOUT64_LOADER_H
#define WIPEOUT64_LOADER_H

#include "n64init.h"
#include <PR/gs2dex.h>

#ifdef VERSION_EU
#define GLIST_LEN 0x900
#else
#define GLIST_LEN 0x880
#endif

#define STATIC_SEGMENT 1
#define CFB_SEGMENT    2

#define RDP_OUTPUT_LEN 0x400
#define MTX_POOL_LEN   200

/*
 * Temporary US/EU symbol aliases for unnamed symbols not yet in
 * config/{us,eu}/symbol_addrs.txt.
 */
#ifdef VERSION_EU
#define CreateLoader2DTask    func_8003B980
#define CreateLoaderGfxTask   func_8003BD48

#define DrawHudText           func_800330E8
#define DrawHudTextCentered   func_80034244
#define DrawFontText          func_8003D388
#define DrawFontTextYellow    func_8003D60C
#define DrawFontTextButton    func_8003CE4C
#define SetupGfxTask          func_8004F288

#define g_s2dexTaskTemplate   D_80091450
#define g_staticSegment       D_800A69C4
#define g_dl2DInit            D_800CBC50
#define g_loaderSprite        D_800CE1E0
#define g_gameState           D_80091730
#define g_rumblePakFound      D_800B7660
#define g_loadDone            D_8008A008
#define g_rdpOutputBuf        D_800A6FB0
#define g_gfxYieldBuf         D_800A5DC0
#define g_dramStack           D_800F7FF0
#define g_gfxInfo             D_800CEA08
#define g_mtxCount            D_800AE514
#define g_mtxPool             D_800AE4C0
#define g_frameDelayFlag      D_80091818
#else
#define CreateLoader2DTask    func_80034290
#define CreateLoaderGfxTask   func_80034658

#define DrawHudText           func_8002B9E4
#define DrawHudTextCentered   func_8002CB40
#define DrawFontText          func_80035C98
#define DrawFontTextYellow    func_80035F1C
#define DrawFontTextButton    func_8003575C
#define SetupGfxTask          func_8004D470

#define g_s2dexTaskTemplate   D_80091E20
#define g_staticSegment       D_800993E4
#define g_dl2DInit            D_800CB910
#define g_loaderSprite        D_800CDEA0
#define g_gameState           D_80092180
#define g_rumblePakFound      D_800AA088
#define g_loadDone            D_8008A9D8
#define g_rdpOutputBuf        D_800999D0
#define g_gfxYieldBuf         D_800987E0
#define g_dramStack           D_800FF0E0
#define g_gfxInfo             D_800CE6C8
#define g_mtxCount            D_800A0F34
#define g_mtxPool             D_800A0EE0
#define g_frameDelayFlag      D_80092268
#endif

/* Static display list segment (segment 1) symbols */
#define g_vpFullScreen        D_1000000
#define g_rspInitDL           D_1000090
#define g_rdpInitDL           D_10000B0
#define g_clearZBufferDL      D_1000130
#define g_clearCfbDL          D_1000168
#define g_rspInit2DDL         D_10001B0
#define g_rdpInit2DDL         D_10001D8

#define rspbootTextEnd        n_aspMainTextStart

typedef struct {
    Mtx projection;
    Mtx ortho;
    Mtx modeling[2];
    Gfx glist[GLIST_LEN];
    Mtx mtxPool[MTX_POOL_LEN];
} Dynamic;

typedef struct {
    OSScTask task[7];
    OSScTask task2D;
    OSScTask taskExtra;
    Dynamic dp;
    OSScMsg doneMsg;
    OSScMsg taskMsgs[7];
    void *cfb;
    Gfx *glist2D;
    s32 active;
    u32 flags;
} GFXInfo;

extern u64 n_aspMainTextStart[];

extern OSTask g_s2dexTaskTemplate;
extern void *g_staticSegment;
extern Gfx g_dl2DInit[];
extern uSprite g_loaderSprite;
extern s32 g_gameState;
extern s32 g_rumblePakFound;
extern s32 g_loadDone;
extern u64 g_rdpOutputBuf[];
extern u64 g_gfxYieldBuf[];
extern u64 g_dramStack[];
extern GFXInfo g_gfxInfo[2];
extern s32 g_mtxCount;
extern Mtx *g_mtxPool;
extern s32 g_frameDelayFlag;

extern Vp g_vpFullScreen;
extern Gfx g_rspInitDL[];
extern Gfx g_rdpInitDL[];
extern Gfx g_clearZBufferDL[];
extern Gfx g_clearCfbDL[];
extern Gfx g_rspInit2DDL[];
extern Gfx g_rdpInit2DDL[];

Gfx *DrawHudText(const char *str, s32 x, s32 y, s32 size, s32 arg4, Gfx *glistp);
Gfx *DrawHudTextCentered(const char *str, s32 x, s32 y, s32 size, s32 arg4, Gfx *glistp);
Gfx *DrawFontText(const char *str, s32 x, s32 y, s32 size, Gfx *glistp);
Gfx *DrawFontTextYellow(const char *str, s32 x, s32 y, s32 size, Gfx *glistp);
Gfx *DrawFontTextButton(s32 x, s32 y, s16 size, const char *str, Gfx *glistp);
void SetupGfxTask(GFXInfo *info, s32 taskIdx, Gfx *glistStart, Gfx *glistEnd);

void CreateLoader2DTask(Gfx *glistStart, GFXInfo *i);
void CreateLoaderGfxTask(s32 frameIdx);

#endif /* WIPEOUT64_LOADER_H */
