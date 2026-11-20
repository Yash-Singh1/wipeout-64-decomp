#ifndef WIPEOUT64_SFX_H
#define WIPEOUT64_SFX_H

#include "common.h"
#include "radscn.h"

typedef u32 musHandle;

/* Sound effect IDs passed to MusStartEffect2. */
#define SFX_SCRAPE          1
#define SFX_TURBULENCE      7
#define SFX_ENGINE          21

/* ShipData flags referenced by engine SFX updates. */
#define SHIP_ATTR_VIEW_IN       0x0004
#define SHIP_COMBAT_MUTE_ENGINE 0x0100
#define SHIP_COMBAT_DESTROYED   0x8000

typedef struct ShipData {
    /* 0x000 */ s16 highResTableIndex;
    /* 0x002 */ s16 lowResTableIndex;
    /* 0x004 */ s32 unk04;
    /* 0x008 */ TrackSection *nearTrkSect;
    /* 0x00C */ TrackSection *prevShipSect;
    /* 0x010 */ s32 attr;
    /* 0x014 */ u8 unk14[0x80];
    /* 0x094 */ s16 r_brake_rot;
    /* 0x096 */ s16 l_brake_rot;
    /* 0x098 */ u16 speed;
    /* 0x09A */ u8 unk9A[0x16];
    /* 0x0B0 */ u16 combatAttr;
    /* 0x0B2 */ u8 unkB2[0x56];
} ShipData; /* size = 0x108 */

/*
 * Temporary version-specific symbol bindings until these symbols are
 * registered in config/{us,eu}/symbol_addrs.txt.
 */
#ifdef VERSION_EU
#define AdjustEngineSound       func_80071BF0
#define StartEngineSfx          func_80071EC0
#define StopEngineSfx           func_80071F78
#define SetSfxVolume            func_80071FE8
#define StopSfx                 func_8007200C
#define PlaySfx                 func_80072048
#define SetSfxFreqOffset        func_8007206C

#define MusStartEffect2         func_800736B8
#define MusHandleStop           func_800738BC
#define MusHandleSetVolume      func_80073984
#define MusHandleSetFreqOffset  func_80073A54

#define g_scrapeHandle          D_800968A0
#define g_scrapeVol             D_800968A4
#define g_engineHandle          D_800968B4
#define g_turbHandle            D_800968B8
#define ownShip                 D_800B3414
#define ownShip2                D_800B756C
#define ownShip3                D_800B7570
#define ownShip4                D_800B7574
#define ships                   D_800B767C
#else
#define AdjustEngineSound       func_8006FC50
#define StartEngineSfx          func_8006FF20
#define StopEngineSfx           func_8006FFD8
#define SetSfxVolume            func_80070048
#define StopSfx                 func_8007006C
#define PlaySfx                 func_800700A8
#define SetSfxFreqOffset        func_800700CC

#define MusStartEffect2         func_80073918
#define MusHandleStop           func_80073B1C
#define MusHandleSetVolume      func_80073BE4
#define MusHandleSetFreqOffset  func_80073CB4

#define g_scrapeHandle          D_800972F0
#define g_scrapeVol             D_800972F4
#define g_engineHandle          D_80097304
#define g_turbHandle            D_80097308
#define ownShip                 D_800A5E2C
#define ownShip2                D_800A9F8C
#define ownShip3                D_800A9F90
#define ownShip4                D_800A9F94
#define ships                   D_800AA0A4
#endif

extern musHandle g_scrapeHandle;
extern s16 g_scrapeVol;
extern musHandle g_engineHandle;
extern musHandle g_turbHandle;
extern s32 ownShip;
extern s32 ownShip2;
extern s32 ownShip3;
extern s32 ownShip4;
extern ShipData *ships;

/* libmus player API */
musHandle MusStartEffect2(s32 number, s32 volume, s32 pan, s32 restartflag, s32 priority);
s32 MusHandleStop(musHandle handle, s32 speed);
s32 MusHandleSetVolume(musHandle handle, s32 volume);
s32 MusHandleSetFreqOffset(musHandle handle, f32 offset);

/* sfx.c API */
void AdjustEngineSound(ShipData *playerShip, Skeleton *camPos);
void StartEngineSfx(void);
void StopEngineSfx(void);
void SetSfxVolume(musHandle handle, s32 volume);
void StopSfx(musHandle *handle);
musHandle PlaySfx(s32 number, s32 volume, s32 pan, s32 restartflag, s32 priority);
s32 SetSfxFreqOffset(musHandle handle, f32 offset);

#endif /* WIPEOUT64_SFX_H */
