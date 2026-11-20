#ifndef WIPEOUT64_CAMERA_H
#define WIPEOUT64_CAMERA_H

#include "common.h"

/*
 * TEMPORARY: radscn.h only declares FrontendOptions up to numPlayers. camera.c also
 * reads gameMode (0x15) and cameraView (0x59) from frontendOptions_g.
 */
#define FrontendOptions FrontendOptionsHead
#define frontendOptions_g frontendOptionsHead_g
#include "radscn.h"
#undef FrontendOptions
#undef frontendOptions_g

#include "skeleton.h"

/*
 * Temporary US/EU symbol mappings until config/{us,eu}/symbol_addrs.txt
 * registers canonical names for camera.c functions and external symbols.
 */
#ifdef VERSION_EU
#define InitCamera                   func_80021B50
#define UpdateCameraRaceExternal     func_80021BEC
#define UpdateCameraRaceInternal     func_80021F3C
#define UpdateCameraRaceStart        func_80022038
#define UpdateCameraAttractCircle    func_8002242C
#define UpdateCameraAttractMedal     func_80022778
#define UpdateCameraAttractDestroyed func_8002291C
#define UpdateCameraDroidView        func_80022968
#define UpdateCameraAttractInternal  func_80022CBC
#define UpdateCameraStaticFollow     func_80022EC4
#define UpdateCameraAttractMode      func_80023158
#define SetUnitFinVector             func_80023700

#define ratan2                       func_8001BBE0
#define FindNearestSection           func_80023A98
#define GetMagnitude                 func_80023EE4
#define sar                          func_80023F70
#define newRand                      func_80023FCC
#define GetTargetPos                 func_800260A4

#define cameraTarget                 D_8008F100
#define cameraTarget2                D_8008F102
#define cameraTarget3                D_8008F104
#define cameraTarget4                D_8008F106
#define g_raceStartCamera            D_8008F108
#define raceExternalVel              D_8008F10C
#define raceExternalVel2             D_8008F110
#define raceExternalVel3             D_8008F114
#define raceExternalVel4             D_8008F118
#define droidViewVel                 D_8008F120
#define droidViewVel2                D_8008F124
#define droidViewVel3                D_8008F128
#define droidViewVel4                D_8008F12C
#define g_cameraSwitchFlag           D_80095F40
#define g_playerFinished             D_800A8FF8
#define CameraUpdate                 D_800AE624
#define CameraUpdate2                D_800B33F8
#define CameraUpdate3                D_800B33FC
#define CameraUpdate4                D_800B3400
#define ownShip                      D_800B3414
#define ownShip2                     D_800B756C
#define ownShip3                     D_800B7570
#define ownShip4                     D_800B7574
#define g_gameType                   D_800B7604
#define ships                        D_800B767C
#define cameraStaticPos              D_800B8450
#define cameraStaticPos2             D_800B8460
#define cameraStaticPos3             D_800B8470
#define cameraStaticPos4             D_800B8480
#else
#define InitCamera                   func_8001AC10
#define UpdateCameraRaceExternal     func_8001ACAC
#define UpdateCameraRaceInternal     func_8001AF88
#define UpdateCameraRaceStart        func_8001B084
#define UpdateCameraAttractCircle    func_8001B478
#define UpdateCameraAttractMedal     func_8001B7C4
#define UpdateCameraAttractDestroyed func_8001B968
#define UpdateCameraDroidView        func_8001B9B4
#define UpdateCameraAttractInternal  func_8001BC94
#define UpdateCameraStaticFollow     func_8001BE9C
#define UpdateCameraAttractMode      func_8001C130
#define SetUnitFinVector             func_8001C6D8

#define ratan2                       func_80014C30
#define FindNearestSection           func_8001CA78
#define GetMagnitude                 func_8001CEC4
#define sar                          func_8001CF50
#define newRand                      func_8001CFAC
#define GetTargetPos                 func_8001EE08

#define cameraTarget                 D_8008FAD0
#define cameraTarget2                D_8008FAD2
#define cameraTarget3                D_8008FAD4
#define cameraTarget4                D_8008FAD6
#define g_raceStartCamera            D_8008FAD8
#define raceExternalVel              D_8008FADC
#define raceExternalVel2             D_8008FAE0
#define raceExternalVel3             D_8008FAE4
#define raceExternalVel4             D_8008FAE8
#define droidViewVel                 D_8008FAF0
#define droidViewVel2                D_8008FAF4
#define droidViewVel3                D_8008FAF8
#define droidViewVel4                D_8008FAFC
#define g_cameraSwitchFlag           D_80096990
#define g_playerFinished             D_8009BA18
#define CameraUpdate                 D_800A1044
#define CameraUpdate2                D_800A5E18
#define CameraUpdate3                D_800A5E1C
#define CameraUpdate4                D_800A5E20
#define ownShip                      D_800A5E2C
#define ownShip2                     D_800A9F8C
#define ownShip3                     D_800A9F90
#define ownShip4                     D_800A9F94
#define g_gameType                   D_800AA024
#define ships                        D_800AA0A4
#define cameraStaticPos              D_800AAE80
#define cameraStaticPos2             D_800AAE90
#define cameraStaticPos3             D_800AAEA0
#define cameraStaticPos4             D_800AAEB0
#endif

/* ShipData.attr bit flags */
#define VIEW_IN     0x0004

/* ShipData.combatAttr bit flags */
#define DESTROYED   0x8000

typedef struct VECTOR {
    /* 0x00 */ s32 vx;
    /* 0x04 */ s32 vy;
    /* 0x08 */ s32 vz;
    /* 0x0C */ s32 pad;
} VECTOR;

typedef struct SVECTOR {
    /* 0x00 */ s16 vx;
    /* 0x02 */ s16 vy;
    /* 0x04 */ s16 vz;
    /* 0x06 */ s16 pad;
} SVECTOR;

typedef struct FrontendOptions {
    /* 0x00 */ s32 team;
    /* 0x04 */ u8 teamClass;
    /* 0x05 */ u8 unk05[7];
    /* 0x0C */ s32 trackNo;
    /* 0x10 */ u8 numPlayers;
    /* 0x11 */ u8 unk11[4];
    /* 0x15 */ u8 gameMode;
    /* 0x16 */ u8 unk16[0x43];
    /* 0x59 */ u8 cameraView;
} FrontendOptions;

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
    /* 0x0B2 */ u8 unkB2[0x32];
    /* 0x0E4 */ s32 updateCount;
    /* 0x0E8 */ u8 unkE8[0x20];
} ShipData; /* size = 0x108 */

typedef struct RescueData {
    /* 0x00 */ u8 data[0x4C];
} RescueData; /* size = 0x4C */

typedef void (*CameraUpdateFunc)(TrackCamera *camera, ShipData *camShip,
                                 RescueData *rescueDroid, s32 shipNo);

extern FrontendOptions frontendOptions_g;

extern s16 cameraTarget;
extern s16 cameraTarget2;
extern s16 cameraTarget3;
extern s16 cameraTarget4;
extern s32 g_raceStartCamera;
extern s32 raceExternalVel;
extern s32 raceExternalVel2;
extern s32 raceExternalVel3;
extern s32 raceExternalVel4;
extern s32 droidViewVel;
extern s32 droidViewVel2;
extern s32 droidViewVel3;
extern s32 droidViewVel4;
extern u8 g_cameraSwitchFlag;
extern u8 g_playerFinished[4];
extern CameraUpdateFunc CameraUpdate;
extern CameraUpdateFunc CameraUpdate2;
extern CameraUpdateFunc CameraUpdate3;
extern CameraUpdateFunc CameraUpdate4;
extern s32 ownShip;
extern s32 ownShip2;
extern s32 ownShip3;
extern s32 ownShip4;
extern s32 g_gameType;
extern ShipData *ships;
extern VECTOR cameraStaticPos;
extern VECTOR cameraStaticPos2;
extern VECTOR cameraStaticPos3;
extern VECTOR cameraStaticPos4;

/* External helpers called by camera.c */
s32 ratan2(s32 y, s32 x);
s16 rsin(s16 a);
s16 rcos(s16 a);
TrackSection *FindNearestSection(TrackSection *trk, VECTOR pos);
s32 GetMagnitude(VECTOR vec);
s32 sar(s32 a, s32 n);
s32 newRand(void);
void GetTargetPos(VECTOR *bestPath, VECTOR pos, Vector nextCentre, Vector curCentre);

/* camera.c functions */
void InitCamera(TrackCamera *camera, TrackSection *section);
void UpdateCameraRaceExternal(TrackCamera *camera, ShipData *camShip,
                              RescueData *rescueDroid, s32 shipNo);
void UpdateCameraRaceInternal(TrackCamera *camera, ShipData *camShip,
                              RescueData *rescueDroid, s32 shipNo);
void UpdateCameraRaceStart(TrackCamera *camera, ShipData *camShip,
                           RescueData *rescueDroid, s32 shipNo);
void UpdateCameraAttractCircle(TrackCamera *camera, ShipData *camShip,
                               RescueData *rescueDroid, s32 shipNo);
void UpdateCameraAttractMedal(TrackCamera *camera, ShipData *camShip,
                              RescueData *rescueDroid, s32 shipNo);
void UpdateCameraAttractDestroyed(TrackCamera *camera, ShipData *camShip,
                                  RescueData *rescueDroid, s32 shipNo);
void UpdateCameraDroidView(TrackCamera *camera, ShipData *camShip,
                           RescueData *rescueDroid, s32 shipNo);
void UpdateCameraAttractInternal(TrackCamera *camera, ShipData *camShip,
                                 RescueData *rescueDroid, s32 shipNo);
void UpdateCameraStaticFollow(TrackCamera *camera, ShipData *camShip,
                              RescueData *rescueDroid, s32 shipNo);
void UpdateCameraAttractMode(TrackCamera *camera, ShipData *camShip,
                             RescueData *rescueDroid, s32 shipNo);
void SetUnitFinVector(VECTOR *fin, ShipData *ship);

#endif /* WIPEOUT64_CAMERA_H */
