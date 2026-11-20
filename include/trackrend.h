#ifndef WIPEOUT64_TRACKREND_H
#define WIPEOUT64_TRACKREND_H

#include "common.h"
#include "n64render.h"
#include "radscn.h"
#include "skeleton.h"
#include <ultra64.h>

/*
 * Temporary US/EU symbol mappings until config/{us,eu}/symbol_addrs.txt
 * registers canonical names for trackrend.c functions and external symbols.
 */
#ifdef VERSION_EU
#define InitSky                 func_80066660
#define GetSkyFacing            func_80066870
#define DrawSkyHighRes          func_800668B8
#define DrawSky                 func_80066F5C
#define DrawSkyHorizon          func_80067600

#define ModifyVertexST          func_8001187C
#define FlushSkyRenderList      func_8001EF64

#define g_skyTopColours         D_80095CE8
#define g_skyMidTopColours      D_80095D04
#define g_skyMidBotColours      D_80095D20
#define g_skyBotColours         D_80095D3C
#define g_skyBaseVerts          D_80095D58
#define g_skyOffsetY            D_80095F38
#define g_mtxPool               D_800AE4C0
#define g_mtxIndex              D_800AE514
#define g_skyGfxPtr             D_800B842C
#define g_skyRenderList         D_800C48D8
#define g_skyUpperBandVtx       D_800C6260
#define g_skyLowerBandVtx       D_800C6400
#define g_skyDlCount            D_800C65A0
#define g_skyDlPool             D_800C65A8
#define g_skyHorizonVtx         D_800C9AD0
#define g_skyCloudVtx           D_800C9BB0
#else
#define InitSky                 func_800648F0
#define GetSkyFacing            func_80064B00
#define DrawSkyHighRes          func_80064B48
#define DrawSky                 func_800651EC
#define DrawSkyHorizon          func_80065890

#define ModifyVertexST          func_8000AA5C
#define FlushSkyRenderList      func_80018024

#define g_skyTopColours         D_80096738
#define g_skyMidTopColours      D_80096754
#define g_skyMidBotColours      D_80096770
#define g_skyBotColours         D_8009678C
#define g_skyBaseVerts          D_800967A8
#define g_skyOffsetY            D_80096988
#define g_mtxPool               D_800A0EE0
#define g_mtxIndex              D_800A0F34
#define g_skyGfxPtr             D_800AAE5C
#define g_skyRenderList         D_800C4598
#define g_skyUpperBandVtx       D_800C5F20
#define g_skyLowerBandVtx       D_800C60C0
#define g_skyDlCount            D_800C6260
#define g_skyDlPool             D_800C6268
#define g_skyHorizonVtx         D_800C9790
#define g_skyCloudVtx           D_800C9870
#endif

#define SKY_DIR_NORTH   1
#define SKY_DIR_SOUTH   2
#define SKY_DIR_WEST    4
#define SKY_DIR_EAST    8

typedef struct SkyColour {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} SkyColour;

typedef struct SkySVector {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} SkySVector;

typedef struct TrackCamera {
    /* 0x00 */ Skeleton *camPos;
    /* 0x04 */ TrackSection *section;
    /* 0x08 */ s16 hdg;
    /* 0x0A */ s16 pitch;
    /* 0x0C */ s16 roll;
    /* 0x0E */ s16 vhdg;
    /* 0x10 */ s16 vpitch;
    /* 0x12 */ s16 vroll;
    /* 0x14 */ SkySVector vel;
} TrackCamera; /* size = 0x1C */

extern SkyColour g_skyTopColours[7];
extern SkyColour g_skyMidTopColours[7];
extern SkyColour g_skyMidBotColours[7];
extern SkyColour g_skyBotColours[7];
extern SkySVector g_skyBaseVerts[60];
extern s32 g_skyOffsetY;

extern Mtx *g_mtxPool;
extern s32 g_mtxIndex;
extern Gfx *g_skyGfxPtr;
extern RenderList g_skyRenderList;
extern Vtx g_skyUpperBandVtx[26];
extern Vtx g_skyLowerBandVtx[26];
extern s32 g_skyDlCount;
extern Gfx g_skyDlPool[];
extern Vtx g_skyHorizonVtx[14];
extern Vtx g_skyCloudVtx[38];

Gfx *ModifyVertexST(Gfx *dl, s32 vtx, s32 u, s32 v, s32 texIdx);
Gfx *FlushSkyRenderList(RenderList *list, Gfx *glistp);

void InitSky(void);
s16 GetSkyFacing(s16 angle);
Gfx *DrawSkyHighRes(TrackCamera *cam, Gfx *glistp);
Gfx *DrawSky(TrackCamera *cam, Gfx *glistp);
Gfx *DrawSkyHorizon(TrackCamera *cam, Gfx *glistp);

#endif /* WIPEOUT64_TRACKREND_H */
