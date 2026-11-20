#ifndef WIPEOUT64_UTILS_H
#define WIPEOUT64_UTILS_H

#include "common.h"
#include "gte.h"
#include "radscn.h"

/* Temporary symbol aliases until utils.c functions and globals are added to symbol_addrs.txt */
#ifdef VERSION_EU
#define SectionSearch       func_80023840
#define FindNearestSection  func_80023A98
#define abs                 func_80023CA0
#define GetAng              func_80023CB4
#define ScalarProduct       func_80023DC0
#define CrossProduct        func_80023E08
#define MinusVector         func_80023EA0
#define GetMagnitude        func_80023EE4
#define arccos              func_80023F38
#define sar                 func_80023F70
#define ang                 func_80023F8C
#define assign              func_80023FA8
#define newRand             func_80023FCC
#define g_arccosTable       D_8008F130
#define g_randSeed          D_80089FA8
#else
#define SectionSearch       func_8001C820
#define FindNearestSection  func_8001CA78
#define abs                 func_8001CC80
#define GetAng              func_8001CC94
#define ScalarProduct       func_8001CDA0
#define CrossProduct        func_8001CDE8
#define MinusVector         func_8001CE80
#define GetMagnitude        func_8001CEC4
#define arccos              func_8001CF18
#define sar                 func_8001CF50
#define ang                 func_8001CF6C
#define assign              func_8001CF88
#define newRand             func_8001CFAC
#define g_arccosTable       D_8008FB00
#define g_randSeed          D_8008A978
#endif

/* TrackSection.flags */
#define JunctionEnd         0x08
#define JunctionStart       0x10
#define JunctionSection     0x20

/* ShipData.attr */
#define FLYING              0x10

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
    /* 0x09A */ s16 shipInitializationValue;
    /* 0x09C */ s16 thrust;
    /* 0x09E */ s16 maxThrust;
    /* 0x0A0 */ s16 thrustCount;
    /* 0x0A2 */ s16 rotationCorrectionDisabled;
    /* 0x0A4 */ s16 headingInc;
    /* 0x0A6 */ s16 maxHeading;
    /* 0x0A8 */ s16 resistance;
    /* 0x0AA */ s16 skid;
    /* 0x0AC */ s16 lateralOffset;
    /* 0x0AE */ s16 moveDist;
    /* 0x0B0 */ u16 combatAttr;
    /* 0x0B2 */ s16 numToSpeed;
    /* 0x0B4 */ s32 distance;
    /* 0x0B8 */ s16 trackDistance;
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
    /* 0x0F4 */ u8 unkF4;
    /* 0x0F5 */ u8 unkF5[7];
    /* 0x0FC */ s16 unkFC;
    /* 0x0FE */ u8 unkFE;
    /* 0x0FF */ u8 unkFF[5];
    /* 0x104 */ void (*update)(struct ShipData *ships, Object **shipShapes, s32 shipNo,
                               void *weapons, void *control);
} ShipData;

extern s16 g_arccosTable[];
extern s32 g_randSeed;

s32 SectionSearch(ShipData *playerShip);
TrackSection *FindNearestSection(TrackSection *trk, VECTOR pos);
s32 abs(s32 value);
s32 GetAng(VECTOR vec1, VECTOR vec2);
s32 ScalarProduct(VECTOR v, VECTOR w);
void CrossProduct(VECTOR *out, VECTOR a, VECTOR b);
void MinusVector(VECTOR start, VECTOR end, VECTOR *result);
s32 GetMagnitude(VECTOR vec1);
s32 arccos(s32 value);
s32 sar(s32 a, s32 n);
s32 ang(s32 angle);
void assign(VECTOR *out, VECTOR v);
s32 newRand(void);

#endif /* WIPEOUT64_UTILS_H */
