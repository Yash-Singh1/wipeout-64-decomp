#ifndef WIPEOUT64_DYNAM_H
#define WIPEOUT64_DYNAM_H

#include "common.h"
#include "skeleton.h"

/*
 * TEMPORARY: radscn.h only declares FrontendOptions up to numPlayers. Dynam.c needs the
 * challenge and weapon fields as well; fold them into radscn.h and drop this rename.
 */
#define FrontendOptions FrontendOptionsHead
#define frontendOptions_g frontendOptionsHead_g
#include "radscn.h"
#undef FrontendOptions
#undef frontendOptions_g

/* Temporary symbol aliases until config/{us,eu}/symbol_addrs.txt are updated. */
#ifdef VERSION_EU
#define ShipFollowTrack         func_80023FF0
#define UpdateSkyVisibility     func_80024614
#define ShipTrkReaction         func_80024BF0
#define getWorldPoint           func_80025F58
#define GetTargetPos            func_800260A4
#define SetUnitVectors          func_80026314
#define SetTrackAttributes      func_800265D8

#define ratan2                  func_8001BBE0
#define OuterProduct12          func_8001BC54
#define RotTrans                func_8001BF94
#define GetFaceVertex           func_8001F06C
#define SectionSearch           func_80023840
#define GetMagnitude            func_80023EE4
#define arccos                  func_80023F38
#define sar                     func_80023F70
#define ang                     func_80023F8C
#define moveDownCentre          func_8002A3CC
#define followRaceLine          func_8002A3DC
#define Ship2TrackBase          func_8002BD78
#define Ship2Track              func_8002C298
#define PlaneLineCollision      func_8002E2A4
#define CheckPolygonCollision   func_8002E374
#define UpdateRescueDroid       func_8002F1C0

#define droidFlyin              D_80091260
#define challenges              D_80094194
#define skyVisible              D_80095CE0
#define checkPointReached       D_800A9228
#define recordDisabled          D_800AE706
#define ownShip                 D_800B3414
#define ownShip2                D_800B756C
#define ownShip3                D_800B7570
#define ownShip4                D_800B7574
#define g_gameType              D_800B7604
#define ships                   D_800B767C
#define upperSkeletons          D_800B87F8
#define cameras                 D_800CBE30
#else
#define ShipFollowTrack         func_8001CFD0
#define UpdateSkyVisibility     func_8001D540
#define ShipTrkReaction         func_8001DB1C
#define getWorldPoint           func_8001ECBC
#define GetTargetPos            func_8001EE08
#define SetUnitVectors          func_8001F078
#define SetTrackAttributes      func_8001F33C

#define ratan2                  func_80014C30
#define OuterProduct12          func_80014CA4
#define RotTrans                func_80014FE4
#define GetFaceVertex           func_8001812C
#define SectionSearch           func_8001C820
#define GetMagnitude            func_8001CEC4
#define arccos                  func_8001CF18
#define sar                     func_8001CF50
#define ang                     func_8001CF6C
#define moveDownCentre          func_80022F28
#define followRaceLine          func_80022F38
#define Ship2TrackBase          func_800248D8
#define Ship2Track              func_80024BF4
#define PlaneLineCollision      func_80026C00
#define CheckPolygonCollision   func_80026CD0
#define UpdateRescueDroid       func_80027B20

#define droidFlyin              D_80091C30
#define challenges              D_80094BE4
#define skyVisible              D_80096730
#define checkPointReached       D_8009BC48
#define recordDisabled          D_800A1126
#define ownShip                 D_800A5E2C
#define ownShip2                D_800A9F8C
#define ownShip3                D_800A9F90
#define ownShip4                D_800A9F94
#define g_gameType              D_800AA024
#define ships                   D_800AA0A4
#define upperSkeletons          D_800AB228
#define cameras                 D_800CBAF0
#endif

/*
 * PAL_SPEED(x): a per-frame quantity tuned for 60 Hz, raised on PAL so the same amount
 * happens per second. PAL_SPEED_VECTOR and PAL_SPEED_UP scale in place as a separate step,
 * which the US code does not have at all.
 */
#ifdef VERSION_EU
#define PAL_SPEED(x) (((x) * 60) / 50)
#define PAL_SPEED_VECTOR(v) \
    ((v).vx = PAL_SPEED((v).vx), (v).vy = PAL_SPEED((v).vy), (v).vz = PAL_SPEED((v).vz))
#define PAL_SPEED_UP(lvalue) ((lvalue) = PAL_SPEED(lvalue))
#else
#define PAL_SPEED(x) (x)
#define PAL_SPEED_VECTOR(v) ((void)0)
#define PAL_SPEED_UP(lvalue) ((void)0)
#endif

#define MAX_CHECKPOINTS 6
#define MAX_PLAYERS     4

/* g_gameType */
#define GAME_SINGLE_RACE  0
#define GAME_TIME_TRIAL   1
#define GAME_CHAMPIONSHIP 3
#define GAME_CHALLENGE    6

/* FrontendOptions.challengeWeapons */
#define CHALLENGE_WEAPONS_TABLE 1
#define CHALLENGE_WEAPONS_OFF   2

/* TrackSection.flags */
#define TRACK_JUMP 0x0001

/* Face.flags */
#define TRACK_BASE        0x01
#define WEAPON_GRID_LEFT  0x02
#define WEAPON_GRID_RIGHT 0x08
#define SPEED_UP          0x20
#define PIT_LANE          0x40
#define CHECKPOINT        0x80

/* ShipData.attr */
#define IN_TOW    0x0001
#define DIRECTION 0x0008
#define FLYING    0x0010
#define LSIDE     0x0020
#define RESCUE    0x0400

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
    /* 0x0AC */ u8 unkAC[0xC6 - 0xAC];
    /* 0x0C6 */ s16 autopilotCount;
    /* 0x0C8 */ u8 unkC8[0xCC - 0xC8];
    /* 0x0CC */ s16 nearSecNo;
    /* 0x0CE */ s16 prevSecNo;
    /* 0x0D0 */ u8 unkD0[0xE4 - 0xD0];
    /* 0x0E4 */ s32 updateCount;
    /* 0x0E8 */ u8 unkE8[0xF5 - 0xE8];
    /* 0x0F5 */ u8 pitchLocked;  /* set by the track response code; freezes pitch control */
    /* 0x0F6 */ u8 unkF6[0x104 - 0xF6];
    /* 0x104 */ void (*update)();
} ShipData; /* size = 0x108 */

typedef struct TrackCamera {
    /* 0x00 */ Skeleton *camPos;
    /* 0x04 */ TrackSection *section;
    /* 0x08 */ s16 hdg, pitch, roll;
    /* 0x0E */ s16 vhdg, vpitch, vroll;
    /* 0x14 */ SVector vel;
    /* 0x1A */ s16 pad;
} TrackCamera;

typedef struct WeaponGrid {
    /* 0x00 */ TrackSection *section;
    /* 0x04 */ u16 attr;
    /* 0x06 */ s16 angle;
    /* 0x08 */ s32 count;
    /* 0x0C */ s32 colour;
} WeaponGrid;

typedef struct SpecialGrid {
    /* 0x00 */ s16 section;
    /* 0x02 */ s16 count;
    /* 0x04 */ s8 attr;
    /* 0x05 */ s8 pad5;
} SpecialGrid;

typedef struct ChallengeData {
    /* 0x00 */ u8 unk00[0x0C];
    /* 0x0C */ s32 weaponsOn;
    /* 0x10 */ u8 unk10[0x2C - 0x10];
} ChallengeData; /* size = 0x2C */

typedef struct FrontendOptions {
    /* 0x00 */ s32 team;
    /* 0x04 */ u8 teamClass;
    /* 0x08 */ u32 trackFile;
    /* 0x0C */ s32 trackNo;
    /* 0x10 */ u8 numPlayers;
    /* 0x11 */ u8 unk11[4];
    /* 0x15 */ u8 challengeWeapons;
    /* 0x16 */ u8 challengeNo;
    /* 0x18 */ s32 ownShips[4];
    /* 0x28 */ u8 unk28[0x58 - 0x28];
    /* 0x58 */ u8 weaponsOn;
} FrontendOptions;

extern FrontendOptions frontendOptions_g;
extern ChallengeData challenges[];
extern s32 droidFlyin;
extern s16 skyVisible[MAX_PLAYERS];
extern s8 checkPointReached[MAX_CHECKPOINTS][MAX_PLAYERS];
extern s16 recordDisabled;
extern s32 ownShip;
extern s32 ownShip2;
extern s32 ownShip3;
extern s32 ownShip4;
extern s32 g_gameType;
extern ShipData *ships;
extern Skeleton *upperSkeletons[MAX_PLAYERS];
extern TrackCamera cameras[MAX_PLAYERS];

int abs(int x);
void __assert(const char *expr, const char *file, s32 line);
s32 ratan2(s32 y, s32 x);
s16 rsin(s16 angle);
s16 rcos(s16 angle);
void OuterProduct12(VECTOR *v0, VECTOR *v1, VECTOR *v2);
void RotTrans(SVECTOR *point, VECTOR *result, s32 *flag);
void GetFaceVertex(TrackSection *section, Face *face, s32 vertex, VECTOR *out);
void SectionSearch(ShipData *ship);
s32 GetMagnitude(VECTOR vec);
s32 arccos(s32 cosine);
s32 sar(s32 value, s32 shift);
s32 ang(s32 angle);
void moveDownCentre(VECTOR *offset, ShipData *ship, Face *face, s32 shipNo);
void followRaceLine(VECTOR *offset, ShipData *shipIndex, Face *face, s32 shipNo);
void Ship2TrackBase(ShipData *ship, Face *face, s32 clearance);
void Ship2Track(ShipData *ship, Face *face, Object **shipShapes);
s32 PlaneLineCollision(VECTOR point, VECTOR linePoint, SVector normal);
s32 CheckPolygonCollision(Face *face, VECTOR point, s32 alpha, s32 count, TrackSection *section);
void UpdateRescueDroid();

void ShipFollowTrack(ShipData *shipIndex, Object **shipShapes, s32 shipNo, void *weaponIndex,
                     void *padInfo, s32 scale);
void UpdateSkyVisibility(s32 section, s32 shipNo);
void ShipTrkReaction(ShipData *playerShip, Object **shipShapes);
void getWorldPoint(VECTOR *output, Object *object, Object *objectCollide, s32 pointNo);
void GetTargetPos(VECTOR *result, VECTOR pos, Vector cent1, Vector cent2);
void SetUnitVectors(ShipData *shipIndex);
s32 SetTrackAttributes(TrackSection *section, s16 sectionNo, WeaponGrid *weaponGrid, s32 noWeaponGrids,
                       s16 *checkPointCount, SpecialGrid *specialGrid);

#endif /* WIPEOUT64_DYNAM_H */
