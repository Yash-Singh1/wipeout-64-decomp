#ifndef WIPEOUT64_SCNANIM_H
#define WIPEOUT64_SCNANIM_H

/*
 * Temporary US/EU symbol-name mappings for symbols not yet named in
 * config/{us,eu}/symbol_addrs.txt. Remove once canonical names are added there.
 */
#ifdef VERSION_EU
#define TrackDirection          func_80030120
#define DrawLandscape           func_800301F4
#define CompareObjectName       func_80011BB4
#define ratan2                  func_8001BBE0
#define RotTrans                func_8001BF94
#define g_waveIndex             D_80091270
#define g_sceneryNearClip       D_80091274
#define g_sceneryFarClip        D_80091294
#define g_drawnObjects          D_800A9100
#define g_mtxPool               D_800AE4C0
#define g_mtxIndex              D_800AE514
#define g_trackIndex            D_800B340C
#define g_sceneryPrimCount      D_800B8530
#define g_sceneryRenderList1    D_800C4860
#define g_sceneryRenderList2    D_800C48F0
#else
#define TrackDirection          func_80028A10
#define DrawLandscape           func_80028AE4
#define CompareObjectName       func_8000AD94
#define ratan2                  func_80014C30
#define RotTrans                func_80014FE4
#define g_waveIndex             D_80091C40
#define g_sceneryNearClip       D_80091C44
#define g_sceneryFarClip        D_80091C64
#define g_drawnObjects          D_8009BB20
#define g_mtxPool               D_800A0EE0
#define g_mtxIndex              D_800A0F34
#define g_trackIndex            D_800A5E30
#define g_sceneryPrimCount      D_800AAF60
#define g_sceneryRenderList1    D_800C4520
#define g_sceneryRenderList2    D_800C45B0
#endif

#include "common.h"
#include "utils.h"
#include "n64render.h"

#define ShipForwards    1
#define ShipBackwards   2
#define ShipRight       4
#define ShipLeft        8

#define OBJ_FLAG_RENDERLIST2    0x0004
#define OBJ_FLAG_TRANSLUCENT    0x0008

typedef struct TrackCamera {
    /* 0x00 */ Skeleton *camPos;
    /* 0x04 */ TrackSection *section;
    /* 0x08 */ s16 hdg;
    /* 0x0A */ s16 pitch;
    /* 0x0C */ s16 roll;
    /* 0x0E */ s16 vhdg;
    /* 0x10 */ s16 vpitch;
    /* 0x12 */ s16 vroll;
    /* 0x14 */ SVECTOR vel;
} TrackCamera;

extern s32 g_waveIndex;
extern s32 g_sceneryNearClip[8];
extern s32 g_sceneryFarClip[8];
extern s16 g_drawnObjects[128];
extern N64Mtx *g_mtxPool;
extern s32 g_mtxIndex;
extern u16 g_trackIndex;
extern s32 g_sceneryPrimCount;
extern RenderList g_sceneryRenderList1;
extern RenderList g_sceneryRenderList2;

s32 CompareObjectName(const char *s1, const char *s2, s32 len);

u8 TrackDirection(TrackCamera *camera, s32 *angleOut);
Gfx *DrawLandscape(ShipData *playerShip, TrackCamera *camera, Gfx *glistp);

#endif /* WIPEOUT64_SCNANIM_H */
