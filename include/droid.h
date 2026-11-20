#ifndef WIPEOUT64_DROID_H
#define WIPEOUT64_DROID_H

#include "common.h"
#include "radscn.h"
#include "skeleton.h"
#include <ultra64.h>

/*
 * Temporary US/EU symbol mappings until config/{us,eu}/symbol_addrs.txt
 * registers canonical names for droid.c functions and external symbols.
 */
#ifdef VERSION_EU
#define UpdateRescueDroid           func_8002F1C0
#define UpdateDroidStart            func_8002F6A4
#define UpdateDroidCircle           func_8002F950
#define UpdateDroidRescue           func_8002FAF0
#define SetDroidVectors             func_8002FDD4
#define InitRescueDroid             func_80030048

#define ratan2                      func_8001BBE0
#define UpdateCameraRaceExternal    func_80021BEC
#define UpdateCameraRaceInternal    func_80021F3C
#define UpdateCameraDroidView       func_80022968
#define GetMagnitude                func_80023EE4
#define ang                         func_80023F8C
#define UpdatePlayerShip            func_800275E4
#define ResetRenderList             func_800513F0
#define FlushRenderList             func_80051520
#define AddRenderGroup              func_800517C0
#define StopSfx                     func_8007200C
#define PlaySfx                     func_80072048

#define droidFlyin                  D_80091260
#define g_droidVoice                D_800968B0
#define g_mtxPool                   D_800AE4C0
#define g_mtxIndex                  D_800AE514
#define CameraUpdate                D_800AE624
#define CameraUpdate2               D_800B33F8
#define CameraUpdate3               D_800B33FC
#define CameraUpdate4               D_800B3400
#define ownShip                     D_800B3414
#define g_pauseState                D_800B7550
#define ownShip2                    D_800B756C
#define ownShip3                    D_800B7570
#define ownShip4                    D_800B7574
#define ships                       D_800B767C
#define g_polyCount                 D_800B83D0
#define droidTarget                 D_800B84F0
#define droidTarget2                D_800B8500
#define droidTarget3                D_800B8510
#define droidTarget4                D_800B8520
#define g_droidRenderList           D_800C4800
#define camera                      D_800CBE30
#define camera2                     D_800CBE4C
#define camera3                     D_800CBE68
#define camera4                     D_800CBE84
#else
#define UpdateRescueDroid           func_80027B20
#define UpdateDroidStart            func_80027FA0
#define UpdateDroidCircle           func_8002824C
#define UpdateDroidRescue           func_800283EC
#define SetDroidVectors             func_800286D0
#define InitRescueDroid             func_80028944

#define ratan2                      func_80014C30
#define UpdateCameraRaceExternal    func_8001ACAC
#define UpdateCameraRaceInternal    func_8001AF88
#define UpdateCameraDroidView       func_8001B9B4
#define GetMagnitude                func_8001CEC4
#define ang                         func_8001CF6C
#define UpdatePlayerShip            func_8002033C
#define ResetRenderList             func_8004F660
#define FlushRenderList             func_8004F790
#define AddRenderGroup              func_8004FA30
#define StopSfx                     func_8007006C
#define PlaySfx                     func_800700A8

#define droidFlyin                  D_80091C30
#define g_droidVoice                D_80097300
#define g_mtxPool                   D_800A0EE0
#define g_mtxIndex                  D_800A0F34
#define CameraUpdate                D_800A1044
#define CameraUpdate2               D_800A5E18
#define CameraUpdate3               D_800A5E1C
#define CameraUpdate4               D_800A5E20
#define ownShip                     D_800A5E2C
#define g_pauseState                D_800A9F70
#define ownShip2                    D_800A9F8C
#define ownShip3                    D_800A9F90
#define ownShip4                    D_800A9F94
#define ships                       D_800AA0A4
#define g_polyCount                 D_800AAE00
#define droidTarget                 D_800AAF20
#define droidTarget2                D_800AAF30
#define droidTarget3                D_800AAF40
#define droidTarget4                D_800AAF50
#define g_droidRenderList           D_800C44C0
#define camera                      D_800CBAF0
#define camera2                     D_800CBB0C
#define camera3                     D_800CBB28
#define camera4                     D_800CBB44
#endif

/* Shape index of the rescue droid model in shipShapes[]. */
#define RescueDroidModel    45

/* ShipData.attr bit flags used by rescue droid logic. */
#define IN_TOW              0x0001
#define REMOTE_VIEW         0x0002
#define VIEW_IN             0x0004
#define FLYING              0x0010
#define RESCUE              0x0400

/* PadInfo.pad bit flags. */
#define PAD_ANALOG_THRUST   0x0002
#define PAD_THRUST          0x4000

/* Sound effect ID for the rescue droid loop. */
#define SFX_DROID           17

typedef u32 musHandle;

typedef struct VECTOR {
    /* 0x00 */ s32 vx;
    /* 0x04 */ s32 vy;
    /* 0x08 */ s32 vz;
    /* 0x0C */ s32 pad;
} VECTOR;

typedef struct PadInfo {
    /* 0x00 */ u16 unk00;
    /* 0x02 */ u16 pad;
    /* 0x04 */ u16 xpad;
    /* 0x06 */ u16 opad;
    /* 0x08 */ u8 thrust;
    /* 0x09 */ s8 stickX;
    /* 0x0A */ s8 stickY;
    /* 0x0B */ u8 unk0B;
    /* 0x0C */ u16 stickPad;
    /* 0x0E */ u16 xstickPad;
    /* 0x10 */ u16 ostickPad;
} PadInfo;

typedef struct TrackCamera {
    /* 0x00 */ Skeleton *camPos;
    /* 0x04 */ TrackSection *section;
    /* 0x08 */ u8 unk08[0x14];
} TrackCamera; /* size = 0x1C */

struct ShipData;
struct RescueData;

typedef void (*CameraUpdateFunc)(TrackCamera *camera, struct ShipData *camShip,
                                 struct RescueData *rescueDroid, s32 shipNo);

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
    /* 0x0A0 */ u8 unkA0[0x44];
    /* 0x0E4 */ s32 updateCount;
    /* 0x0E8 */ u8 unkE8[0x1C];
    /* 0x104 */ void (*update)(struct ShipData *shipIndex, Object **shipShapes,
                               s32 shipNo, void *weaponIndex, PadInfo *padInfo);
} ShipData; /* size = 0x108 */

typedef struct RescueData {
    /* 0x00 */ TrackSection *section;
    /* 0x04 */ s16 modelID;
    /* 0x06 */ s16 pad06;
    /* 0x08 */ VECTOR pos;
    /* 0x18 */ VECTOR vel;
    /* 0x28 */ VECTOR acc;
    /* 0x38 */ s16 hdg;
    /* 0x3A */ s16 pitch;
    /* 0x3C */ s16 roll;
    /* 0x3E */ s16 vhdg;
    /* 0x40 */ s16 vpitch;
    /* 0x42 */ s16 vroll;
    /* 0x44 */ s16 updateCount;
    /* 0x46 */ s16 pad46;
    /* 0x48 */ Gfx *(*droidUpdate)(struct RescueData *rescueDroid, ShipData *shipIndex,
                                   Object **shipShapes, s32 shipNo, Gfx *glistp);
} RescueData; /* size = 0x4C */

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
} RenderList; /* size = 0x18 */

typedef struct RenderGroupEntry {
    /* 0x00 */ s32 textureIndex;
    /* 0x04 */ Gfx *dl;
} RenderGroupEntry;

typedef struct RenderGroup {
    /* 0x00 */ u32 count;
    /* 0x04 */ RenderGroupEntry *entries;
} RenderGroup;

extern s32 droidFlyin;
extern musHandle g_droidVoice;
extern N64Mtx *g_mtxPool;
extern s32 g_mtxIndex;
extern CameraUpdateFunc CameraUpdate;
extern CameraUpdateFunc CameraUpdate2;
extern CameraUpdateFunc CameraUpdate3;
extern CameraUpdateFunc CameraUpdate4;
extern s32 ownShip;
extern s32 ownShip2;
extern s32 ownShip3;
extern s32 ownShip4;
extern s32 g_pauseState;
extern ShipData *ships;
extern s32 g_polyCount;
extern VECTOR droidTarget;
extern VECTOR droidTarget2;
extern VECTOR droidTarget3;
extern VECTOR droidTarget4;
extern RenderList g_droidRenderList;
extern TrackCamera camera;
extern TrackCamera camera2;
extern TrackCamera camera3;
extern TrackCamera camera4;

/* External helpers called by droid.c */
s32 ratan2(s32 y, s32 x);
s16 rsin(s16 a);
s16 rcos(s16 a);
s32 GetMagnitude(VECTOR vec);
s32 ang(s32 angle);
void UpdatePlayerShip(ShipData *shipIndex, Object **shipShapes, s32 shipNo,
                      void *weaponIndex, PadInfo *padInfo);
void UpdateCameraDroidView(TrackCamera *cam, ShipData *camShip, RescueData *rescueDroid, s32 shipNo);
void UpdateCameraRaceInternal(TrackCamera *cam, ShipData *camShip, RescueData *rescueDroid, s32 shipNo);
void UpdateCameraRaceExternal(TrackCamera *cam, ShipData *camShip, RescueData *rescueDroid, s32 shipNo);
void ResetRenderList(RenderList *list);
Gfx *FlushRenderList(RenderList *list, Gfx *glistp);
void AddRenderGroup(RenderList *list, RenderGroup *group, N64Mtx *mtx);
void StopSfx(musHandle *handle);
musHandle PlaySfx(s32 number, s32 volume, s32 pan, s32 restartflag, s32 priority);

/* droid.c functions */
void UpdateRescueDroid(ShipData *shipIndex, Object **shipShapes, s32 shipNo,
                       void *weaponIndex, PadInfo *padInfo);
Gfx *UpdateDroidStart(RescueData *rescueDroid, ShipData *shipIndex, Object **shipShapes,
                      s32 shipNo, Gfx *glistp);
Gfx *UpdateDroidCircle(RescueData *rescueDroid, ShipData *shipIndex, Object **shipShapes,
                       s32 shipNo, Gfx *glistp);
Gfx *UpdateDroidRescue(RescueData *rescueDroid, ShipData *shipIndex, Object **shipShapes,
                       s32 shipNo, Gfx *glistp);
Gfx *SetDroidVectors(RescueData *rescueDroid, Object **shipShapes, s32 shipNo,
                     TrackCamera *cam, Gfx *glistp);
void InitRescueDroid(RescueData *rescueDroid, Object **shipShapes, ShipData *shipIndex);

#endif /* WIPEOUT64_DROID_H */
