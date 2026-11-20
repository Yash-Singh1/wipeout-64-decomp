#ifndef WIPEOUT64_CONTROL_H
#define WIPEOUT64_CONTROL_H

#include "common.h"

#define FrontendOptions FrontendOptionsHead
#define frontendOptions_g frontendOptionsHead_g
#include "radscn.h"
#undef FrontendOptions
#undef frontendOptions_g

/* Temporary symbol aliases until config/{us,eu}/symbol_addrs.txt registers these names. */
#ifdef VERSION_EU
#define UpdateDroneShipStart        func_80027F30
#define GetDroneShield              func_80027FE8
#define UpdateDroneShip             func_80028220
#define moveDownCentre              func_8002A3CC
#define followRaceLine              func_8002A3DC
#define lookForWayPast              func_8002A474
#define calcMyXPos                  func_8002A68C
#define goRight                     func_8002A8D4
#define goLeft                      func_8002A980
#define getShipInfrontLAP           func_8002AA2C
#define getShipBehindLAP            func_8002AB18
#define humanToDroneSpeed           func_8002AC0C

#define ratan2                      func_8001BBE0
#define PlayNote                    func_8001BCE8
#define GetFaceVertex               func_8001F06C
#define SectionSearch               func_80023840
#define GetMagnitude                func_80023EE4
#define sar                         func_80023F70
#define ang                         func_80023F8C
#define newRand                     func_80023FCC
#define GetTargetPos                func_800260A4
#define UpdatePlayerShip            func_800275E4
#define DroneShip2TrackBase         func_8002C0FC
#define PlaneLineCollision          func_8002E2A4
#define FireWeapon                  func_8003E9A8

#define g_shipMaxShield             D_80089FD0
#define g_aiShipMaxShield           D_80089FE4
#define classSpeedLimit             D_80091150
#define g_aiCatchupMaxThrust        D_80091158
#define g_aiCatchupThrustMag        D_80091168
#define g_aiBoostMaxThrust          D_80091178
#define g_aiBoostThrustMag          D_80091188
#define rivalMarginScale1           D_80091198
#define rivalAccelScale1            D_800911A8
#define rivalMarginScale2           D_800911B8
#define rivalAccelScale2            D_800911C8
#define rivalMarginScale3           D_800911D8
#define rivalAccelScale3            D_800911E8
#define rivalMarginScale4           D_800911F8
#define rivalAccelScale4            D_80091208
#define pitLaneSection1             D_80091218
#define pitLaneSection2             D_80091228
#define g_classLaps                 D_80091410
#define g_unk800923F9               D_800919A9
#define activeShipCount             D_800A9250
#define shipDamage                  D_800A92C0
#define shipClassIndex              D_800AE6A0
#define numTrackSections            D_800AE6A2
#define trackIndex                  D_800B340C
#define ownShip                     D_800B3414
#define ownShip2                    D_800B756C
#define ownShip3                    D_800B7570
#define ownShip4                    D_800B7574
#define attractMode                 D_800B7600
#define g_gameType                  D_800B7604
#define playerPosition              D_800B7682
#define player2Position             D_800B76AA
#define player3Position             D_800B76AC
#define player4Position             D_800B76B4
#define startGridOrder              D_800B83B0
#else
#define UpdateDroneShipStart        func_80020C40
#define GetDroneShield              func_80020CF8
#define UpdateDroneShip             func_80020F30
#define moveDownCentre              func_80022F28
#define followRaceLine              func_80022F38
#define lookForWayPast              func_80022FD0
#define calcMyXPos                  func_800231E8
#define goRight                     func_80023430
#define goLeft                      func_800234DC
#define getShipInfrontLAP           func_80023588
#define getShipBehindLAP            func_80023674
#define humanToDroneSpeed           func_80023768

#define ratan2                      func_80014C30
#define PlayNote                    func_80014D38
#define GetFaceVertex               func_8001812C
#define SectionSearch               func_8001C820
#define GetMagnitude                func_8001CEC4
#define sar                         func_8001CF50
#define ang                         func_8001CF6C
#define newRand                     func_8001CFAC
#define GetTargetPos                func_8001EE08
#define UpdatePlayerShip            func_8002033C
#define DroneShip2TrackBase         func_80024B2C
#define PlaneLineCollision          func_80026C00
#define FireWeapon                  func_800372B8

#define g_shipMaxShield             D_8008A9A0
#define g_aiShipMaxShield           D_8008A9B4
#define classSpeedLimit             D_80091B20
#define g_aiCatchupMaxThrust        D_80091B28
#define g_aiCatchupThrustMag        D_80091B38
#define g_aiBoostMaxThrust          D_80091B48
#define g_aiBoostThrustMag          D_80091B58
#define rivalMarginScale1           D_80091B68
#define rivalAccelScale1            D_80091B78
#define rivalMarginScale2           D_80091B88
#define rivalAccelScale2            D_80091B98
#define rivalMarginScale3           D_80091BA8
#define rivalAccelScale3            D_80091BB8
#define rivalMarginScale4           D_80091BC8
#define rivalAccelScale4            D_80091BD8
#define pitLaneSection1             D_80091BE8
#define pitLaneSection2             D_80091BF8
#define g_classLaps                 D_80091DE0
#define g_unk800923F9               D_800923F9
#define activeShipCount             D_8009BC70
#define shipDamage                  D_8009BCE0
#define shipClassIndex              D_800A10C0
#define numTrackSections            D_800A10C2
#define trackIndex                  D_800A5E30
#define ownShip                     D_800A5E2C
#define ownShip2                    D_800A9F8C
#define ownShip3                    D_800A9F90
#define ownShip4                    D_800A9F94
#define attractMode                 D_800AA020
#define g_gameType                  D_800AA024
#define playerPosition              D_800AA0C0
#define player2Position             D_800AA0D6
#define player3Position             D_800AA0D8
#define player4Position             D_800AA0E0
#define startGridOrder              D_800AADE0
#endif

#define shipOrder                   g_unkAlloc60
#define raceLines                   g_unkAlloc9600

/* Face.flags */
#define TRACK_BASE                  0x01
#define SPEED_UP                    0x20

/* TrackSection.flags */
#define JunctionStart               0x10

/* ShipData.attr */
#define DIRECTION                   0x0008
#define FLYING                      0x0010
#define LSIDE                       0x0020
#define JUNC_LEFT                   0x2000
#define ELECTROED                   0x8000

/* ShipData.combatAttr */
#define SHIP_COMBAT_AI_LOW          0x0010
#define SHIP_COMBAT_AI_MID          0x0020
#define SHIP_COMBAT_AI_HIGH         0x0040
#define SHIP_COMBAT_NO_FIRE         0x0100
#define DESTROYED                   0x8000

/* Game types */
#define GAME_TYPE_ARCADE            0
#define GAME_TYPE_LEAGUE            3
#define GAME_TYPE_CHALLENGE         6

/* Weapon IDs */
#define WEAPON_ROCKET               1
#define WEAPON_MISSILE              2
#define WEAPON_MINE                 3
#define WEAPON_ELEC_BOLT            5
#define WEAPON_QUAKE                12

/* Sound effect IDs */
#define SFX_AUTOPILOT_OFF           16
#define SFX_ELEC_BOLT_WARN          18
#define SFX_MINE_WARN               30
#define SFX_MISSILE_WARN            31
#define SFX_ROCKET_WARN             38
#define SFX_QUAKE_WARN              62

/* PadInfo.xpad bit flags */
#define PAD_DISCARD                 0x8000

/* Sentinel when the leading human ship is outside the AI rubber-band range. */
#define NO_HUMAN_ZONE               100

typedef struct VECTOR {
    /* 0x00 */ s32 vx;
    /* 0x04 */ s32 vy;
    /* 0x08 */ s32 vz;
    /* 0x0C */ s32 pad;
} VECTOR;

typedef struct SVector {
    /* 0x00 */ s16 vx;
    /* 0x02 */ s16 vy;
    /* 0x04 */ s16 vz;
} SVector;

typedef struct Face {
    /* 0x00 */ s16 vertex[4];
    /* 0x08 */ SVector normal;
    /* 0x0E */ s8 texture;
    /* 0x0F */ u8 flags;
    /* 0x10 */ u8 red;
    /* 0x11 */ u8 green;
    /* 0x12 */ u8 blue;
} Face;

typedef struct RaceLine {
    /* 0x00 */ s16 distance;
    /* 0x02 */ u16 pad;
    /* 0x04 */ s32 x;
    /* 0x08 */ s32 y;
    /* 0x0C */ s32 z;
} RaceLine;

typedef struct ShipRank {
    /* 0x00 */ s16 position;
    /* 0x02 */ s16 shipNo;
} ShipRank;

typedef struct FrontendOptions {
    /* 0x00 */ s32 team;
    /* 0x04 */ u8 teamClass;
    /* 0x05 */ u8 unk05[7];
    /* 0x0C */ s32 trackNo;
    /* 0x10 */ u8 numPlayers;
    /* 0x11 */ u8 unk11[4];
    /* 0x15 */ u8 gameMode;
    /* 0x16 */ u8 gameSubMode;
} FrontendOptions;

typedef struct WeaponData WeaponData;

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
    /* 0x0A0 */ s16 thrustCount;
    /* 0x0A2 */ s16 noAttitudeControl;
    /* 0x0A4 */ s16 headingInc;
    /* 0x0A6 */ s16 maxHeading;
    /* 0x0A8 */ s16 resistance;
    /* 0x0AA */ s16 skid;
    /* 0x0AC */ s16 lateralOffset;
    /* 0x0AE */ s16 moveDist;
    /* 0x0B0 */ u16 combatAttr;
    /* 0x0B2 */ s16 boostCount;
    /* 0x0B4 */ s16 distance;
    /* 0x0B6 */ s16 trackDistance;
    /* 0x0B8 */ s16 unkB8;
    /* 0x0BA */ u16 remoteMagnitude;
    /* 0x0BC */ u16 remoteMax;
    /* 0x0BE */ u8 weapon;
    /* 0x0BF */ u8 lastWeapon;
    /* 0x0C0 */ u8 rearFire;
    /* 0x0C1 */ s8 targetShip;
    /* 0x0C2 */ s8 rearTarget;
    /* 0x0C3 */ u8 padC3;
    /* 0x0C4 */ s16 electroCount;
    /* 0x0C6 */ s16 autopilotCount;
    /* 0x0C8 */ s16 stallCount;
    /* 0x0CA */ s8 lap;
    /* 0x0CB */ s8 reverseLap;
    /* 0x0CC */ s16 nearSecNo;
    /* 0x0CE */ s16 prevSecNo;
    /* 0x0D0 */ s32 lapTimes[5];
    /* 0x0E4 */ s32 updateCount;
    /* 0x0E8 */ s32 padE8;
    /* 0x0EC */ s32 startY;
    /* 0x0F0 */ s32 unkF0;
    /* 0x0F4 */ u8 unkF4[4];
    /* 0x0F8 */ s32 speedPadSection;
    /* 0x0FC */ u8 unkFC[8];
    /* 0x104 */ void (*update)();
} ShipData; /* size = 0x108 */

typedef struct PadInfo {
    /* 0x00 */ u16 unk00;
    /* 0x02 */ u16 pad;
    /* 0x04 */ u16 xpad;
    /* 0x06 */ u16 opad;
} PadInfo;

extern FrontendOptions frontendOptions_g;
extern TrackData *track;
extern RaceLine *raceLines;
extern ShipRank *shipOrder;
extern s16 activeShipCount;
extern s16 numTrackSections;
extern u16 shipClassIndex;
extern u16 trackIndex;
extern s32 shipDamage[];
extern s32 ownShip;
extern s32 ownShip2;
extern s32 ownShip3;
extern s32 ownShip4;
extern u8 attractMode;
extern s32 g_gameType;
extern s16 playerPosition;
extern s16 player2Position;
extern s16 player3Position;
extern s16 player4Position;
extern s16 startGridOrder[];

extern s32 g_shipMaxShield[5];
extern s32 g_aiShipMaxShield[5];
extern s16 classSpeedLimit[4];
extern s32 g_aiCatchupMaxThrust[4];
extern s32 g_aiCatchupThrustMag[4];
extern s32 g_aiBoostMaxThrust[4];
extern s32 g_aiBoostThrustMag[4];
extern f32 rivalMarginScale1[4];
extern f32 rivalAccelScale1[4];
extern f32 rivalMarginScale2[4];
extern f32 rivalAccelScale2[4];
extern f32 rivalMarginScale3[4];
extern f32 rivalAccelScale3[4];
extern f32 rivalMarginScale4[4];
extern f32 rivalAccelScale4[4];
extern s16 pitLaneSection1[8];
extern s16 pitLaneSection2[8];
extern u8 g_classLaps[];
extern u8 g_unk800923F9;

int abs(int value);
s32 ratan2(s32 y, s32 x);
s16 rsin(s16 a);
void PlayNote(s32 sound, s32 volume, s32 lrPan, s32 fbPan, s32 pitch);
void GetFaceVertex(TrackSection *section, Face *face, s32 vertex, Vector *out);
s32 SectionSearch(ShipData *playerShip);
s32 GetMagnitude(VECTOR vec);
s32 sar(s32 a, s32 n);
s32 ang(s32 angle);
s32 newRand(void);
void GetTargetPos(VECTOR *bestPath, VECTOR pos, Vector nextCentre, Vector curCentre);
void UpdatePlayerShip();
s32 PlaneLineCollision(VECTOR point, VECTOR pos, SVector normal);
void DroneShip2TrackBase(ShipData *ship, Face *face, s32 clearance);
void FireWeapon(ShipData *ship, WeaponData *weapons, s32 shipNo, s32 weaponType);

void UpdateDroneShipStart(ShipData *ships, Object **shipShapes, s32 shipNo);
s32 GetDroneShield(s32 shipNo);
void UpdateDroneShip(ShipData *ships, Object **shipShapes, s32 shipNo, WeaponData *weaponIndex,
                     PadInfo *padInfo);
void moveDownCentre(VECTOR *relative, ShipData *ships, Face *face, s32 shipNo);
void followRaceLine(VECTOR *relative, ShipData *ships, Face *face, s32 shipNo);
void lookForWayPast(VECTOR *relative, ShipData *ships, s16 shipNo, s16 otherShip);
void calcMyXPos(ShipData *ships, s16 shipNo, s16 *lateral, s16 *along);
void goRight(VECTOR *result, ShipData *ships, Face *face, s32 shipNo);
void goLeft(VECTOR *result, ShipData *ships, Face *face, s32 shipNo);
s16 getShipInfrontLAP(ShipData *ships, s16 shipNo);
s16 getShipBehindLAP(ShipData *ships, s16 shipNo);
void humanToDroneSpeed(ShipData *ships, s32 shipNo);

#endif /* WIPEOUT64_CONTROL_H */
