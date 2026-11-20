#ifndef WIPEOUT64_TRAIL_H
#define WIPEOUT64_TRAIL_H

#include "common.h"

/*
 * Temporary US/EU symbol mappings until config/{us,eu}/symbol_addrs.txt
 * registers canonical names for trail.c functions and external symbols.
 */
#ifdef VERSION_EU
#define DrawFlare           func_8000D810
#define UpdateTarget        func_8000E0D0
#define DrawTarget          func_8000E1B4
#define InitTrails          func_8000E630
#define NewTrailSegment     func_8000EB90
#define BuildOwnShipTrail   func_8000EDE4
#define DrawTrail           func_8000F0EC
#define DrawShipTrails      func_8000F604
#define UpdateShipTrails    func_8000F92C

#define ApplyMatrix         func_8001BDA4
#define newRand             func_80023FCC

#define ships               D_800B767C
#define shipShapes          D_800A9048
#define cameras             D_800CBE30
#define ownShip             D_800B3414
#define ownShip2            D_800B756C
#define ownShip3            D_800B7570
#define ownShip4            D_800B7574
#define g_glistp            D_800B842C
#define g_mtxPool           D_800AE4C0
#define g_mtxIndex          D_800AE514
#define g_gameMode          D_800B7604
#define g_demoMode          D_800B7660
#define g_demoShip          D_80089FA4
#define g_ghostActive       D_8008A004
#define missileLockTimer    D_800914BC
#define targetSpin          D_800895B0
#define targetBank          D_800895B8
#define g_flareCount        D_800BA618
#define g_trailCounter      D_800BA624
#define g_ownShipPosX       D_800BA628
#define g_ownShipPosY       D_800BA62C
#define g_ownShipPosZ       D_800BA630
#define targetAngles        D_8009ECE0
#define targetPos           D_8009ED60
#define targetActive        D_8009EEE0
#define shipTrails          D_8009EF00
#define targetVtx           D_800A4CC8
#else
#define DrawFlare           func_800700F0
#define UpdateTarget        func_800709B0
#define DrawTarget          func_80070AB8
#define InitTrails          func_80070F1C
#define NewTrailSegment     func_8007147C
#define BuildOwnShipTrail   func_800716D0
#define DrawTrail           func_800719D8
#define DrawShipTrails      func_80071E9C
#define UpdateShipTrails    func_800721A0

#define ApplyMatrix         func_80014DF4
#define newRand             func_8001CFAC

#define ships               D_800AA0A4
#define shipShapes          D_8009BA68
#define cameras             D_800CBAF0
#define ownShip             D_800A5E2C
#define ownShip2            D_800A9F8C
#define ownShip3            D_800A9F90
#define ownShip4            D_800A9F94
#define g_glistp            D_800AAE5C
#define g_mtxPool           D_800A0EE0
#define g_mtxIndex          D_800A0F34
#define g_gameMode          D_800AA024
#define g_demoMode          D_800AA088
#define g_demoShip          D_8008A974
#define g_ghostActive       D_8008A9D4
#define missileLockTimer    D_80091E8C
#define targetSpin          D_80097320
#define targetBank          D_80097328
#define g_flareCount        D_800AD048
#define g_trailCounter      D_800AD054
#define g_ownShipPosX       D_800AD058
#define g_ownShipPosY       D_800AD05C
#define g_ownShipPosZ       D_800AD060
#define targetAngles        D_800F4640
#define targetPos           D_800F46C0
#define targetActive        D_800F4840
#define shipTrails          D_800F4860
#define targetVtx           D_800F8868
#endif

/*
 * TEMPORARY: radscn.h only declares FrontendOptions up to numPlayers. Trail.c
 * also reads challengeMedal at +0x15; fold it into radscn.h and drop this rename.
 */
#define FrontendOptions FrontendOptionsHead
#define frontendOptions_g frontendOptionsHead_g
#include "radscn.h"
#undef FrontendOptions
#undef frontendOptions_g

#include "dheap.h"
#include "gte.h"
#include "quat.h"
#include "skeleton.h"
#include <ultra64.h>

#define NO_OF_SHIPS         15
#define TRAIL_SEGMENTS      9
#define TARGET_BANKS        8
#define MAX_PLAYERS         4

#ifdef VERSION_EU
#define TRAIL_FADE          14
#define TARGET_VTX_COUNT    8
#else
#define TRAIL_FADE          16
#define TARGET_VTX_COUNT    5
#endif

/* ShipData.attr bit flags. */
#define SHIP_ATTR_VIEW_IN   0x0004
#define SHIP_ATTR_RACING    0x0040
#define SHIP_ATTR_VISIBLE   0x0200

/* ShipData.combatAttr bit flags. */
#define SHIP_COMBAT_DESTROYED 0x8000

typedef struct FrontendOptions {
    /* 0x00 */ s32 team;
    /* 0x04 */ u8 teamClass;
    /* 0x08 */ u32 trackFile;
    /* 0x0C */ s32 trackNo;
    /* 0x10 */ u8 numPlayers;
    /* 0x11 */ u8 unk11[4];
    /* 0x15 */ u8 challengeMedal;
    /* 0x16 */ u8 unk16;
    /* 0x18 */ s32 ownShips[4];
} FrontendOptions;

typedef struct TrackCamera {
    /* 0x00 */ Skeleton *camPos;
    /* 0x04 */ TrackSection *section;
    /* 0x08 */ s16 hdg, pitch, roll;
    /* 0x0E */ s16 vhdg, vpitch, vroll;
    /* 0x14 */ s16 vel[4];
} TrackCamera; /* size = 0x1C */

typedef struct ShipData {
    /* 0x000 */ s16 highResTableIndex;
    /* 0x002 */ s16 lowResTableIndex;
    /* 0x004 */ s32 unk04;
    /* 0x008 */ TrackSection *nearTrkSect;
    /* 0x00C */ TrackSection *prevShipSect;
    /* 0x010 */ s32 attr;
    /* 0x014 */ VECTOR unitVecNose;
    /* 0x024 */ VECTOR unitVecWing;
    /* 0x034 */ VECTOR unitVecFin;
    /* 0x044 */ VECTOR ppivot;
    /* 0x054 */ VECTOR vpivot;
    /* 0x064 */ VECTOR apivot;
    /* 0x074 */ s16 hdg;
    /* 0x076 */ s16 pitch;
    /* 0x078 */ s16 roll;
    /* 0x07A */ s16 vhdg;
    /* 0x07C */ s16 vpitch;
    /* 0x07E */ s16 vroll;
    /* 0x080 */ s16 ahdg;
    /* 0x082 */ s16 mass;
    /* 0x084 */ VECTOR thrustVector;
    /* 0x094 */ s16 r_brake_rot;
    /* 0x096 */ s16 l_brake_rot;
    /* 0x098 */ u16 speed;
    /* 0x09A */ s16 unk9A;
    /* 0x09C */ s16 thrust;
    /* 0x09E */ s16 maxThrust;
    /* 0x0A0 */ u8 unkA0[0x10];
    /* 0x0B0 */ u16 combatAttr;
    /* 0x0B2 */ u8 unkB2[0x42];
    /* 0x0F4 */ u8 active;
    /* 0x0F5 */ u8 unkF5[0x09];
    /* 0x0FE */ u8 visible[MAX_PLAYERS];
    /* 0x102 */ u8 unk102[0x06];
} ShipData; /* size = 0x108 */

typedef struct TrailSegment {
    /* 0x00 */ u8 active;
    /* 0x01 */ u8 pad01;
    /* 0x02 */ s16 r;
    /* 0x04 */ s16 g;
    /* 0x06 */ s16 b;
    /* 0x08 */ s16 a;
    /* 0x0A */ s16 pad0A;
    /* 0x0C */ VECTOR left;
    /* 0x1C */ VECTOR right;
} TrailSegment; /* size = 0x2C */

typedef struct Trail {
    /* 0x000 */ Vtx trailVtx[MAX_PLAYERS][24];
    /* 0x600 */ Vtx flareVtx[MAX_PLAYERS][16];
    /* 0xA00 */ u8 colour[3][3];
    /* 0xA09 */ u8 padA09[3];
    /* 0xA0C */ s32 current;
    /* 0xA10 */ TrailSegment segment[TRAIL_SEGMENTS];
} Trail; /* size = 0xBA0 */

extern FrontendOptions frontendOptions_g;
extern ShipData *ships;
extern Object *shipShapes[];
extern TrackCamera cameras[MAX_PLAYERS];
extern s32 ownShip;
extern s32 ownShip2;
extern s32 ownShip3;
extern s32 ownShip4;
extern Gfx *g_glistp;
extern N64Mtx *g_mtxPool;
extern s32 g_mtxIndex;
extern s32 g_gameMode;
extern s32 g_demoMode;
extern s32 g_demoShip;
extern s32 g_ghostActive;
extern s16 missileLockTimer;
extern s32 targetSpin;
extern s32 targetBank;
extern s32 g_flareCount;
extern s32 g_trailCounter;
extern s32 g_ownShipPosX;
extern s32 g_ownShipPosY;
extern s32 g_ownShipPosZ;
extern s32 targetAngles[MAX_PLAYERS][TARGET_BANKS];
extern Vector targetPos[MAX_PLAYERS][TARGET_BANKS];
extern u8 targetActive[MAX_PLAYERS][TARGET_BANKS];
extern Trail *shipTrails;
extern Vtx targetVtx[MAX_PLAYERS][TARGET_BANKS][TARGET_VTX_COUNT];

s32 newRand(void);

void DrawFlare(VECTOR *pos, MATRIX *mat, Vtx *vtx, s32 ship, s32 player);
void UpdateTarget(s32 x, s32 y, s32 z, s32 target, s32 player);
void DrawTarget(VECTOR *pos, MATRIX *mat, s32 ship, s32 player, s32 bank);
void InitTrails(void);
void NewTrailSegment(s32 ship, VECTOR *left, VECTOR *right);
void BuildOwnShipTrail(s32 ship, VECTOR *left, VECTOR *right);
void DrawTrail(VECTOR *flarePos, MATRIX *mat, Trail *trail, s32 ship, s32 player);
void DrawShipTrails(TrackCamera *camera, void *arg1, s32 player);
void UpdateShipTrails(ShipData *shipIndex, s32 allShips);

#endif /* WIPEOUT64_TRAIL_H */
