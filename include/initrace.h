#ifndef WIPEOUT64_INITRACE_H
#define WIPEOUT64_INITRACE_H

#include "common.h"
#include "dheap.h"
#include "error.h"
#include "files.h"
#include "radscn.h"

/*
 * Temporary symbol aliases until these functions and globals are added to
 * config/{us,eu}/symbol_addrs.txt.
 */
#ifdef VERSION_EU
#define InitCheckPoints         func_8001F2F0
#define InitWeaponGrids         func_8001F5FC
#define InitAttractMode         func_8001F7C0
#define InitRaceData            func_8001F9D0
#define InitRaceResetData       func_8001FA30
#define InitRaceLine            func_8001FA60
#define GetFaceVertex           func_8001F06C
#define InitCamera              func_80021B50
#define UpdateCameraAttractMode func_80023158
#define SetTrackAttributes      func_800265D8
#define UpdateDroneShipStart    func_80027F30
#define noCheckPoints           D_800B7601
#define checkPointReached       D_800A9228
#define roadSectionCount        D_800A9268
#define weaponGridSections      D_800B3410
#define ownShip                 D_800B3414
#define ownShip2                D_800B756C
#define ownShip3                D_800B7570
#define ownShip4                D_800B7574
#define cameras                 D_800CBE30
#define CameraUpdate            D_800AE624
#define CameraUpdate2           D_800B33F8
#define CameraUpdate3           D_800B33FC
#define CameraUpdate4           D_800B3400
#define attractDelayFrames      D_800B3408
#define pauseRequests           D_800B75F0
#define raceLines               D_800B757C
#define numTrackSections        D_800AE6A2
#ifndef errorString
#define errorString             D_800AE520
#endif
#else
#define InitCheckPoints         func_800183B0
#define InitWeaponGrids         func_800186BC
#define InitAttractMode         func_80018880
#define InitRaceData            func_80018A90
#define InitRaceResetData       func_80018AF0
#define InitRaceLine            func_80018B20
#define GetFaceVertex           func_8001812C
#define InitCamera              func_8001AC10
#define UpdateCameraAttractMode func_8001C130
#define SetTrackAttributes      func_8001F33C
#define UpdateDroneShipStart    func_80020C40
#define noCheckPoints           D_800AA028
#define checkPointReached       D_8009BC48
#define roadSectionCount        D_8009BC88
#define weaponGridSections      D_800A5E34
#define ownShip                 D_800A5E2C
#define ownShip2                D_800A9F8C
#define ownShip3                D_800A9F90
#define ownShip4                D_800A9F94
#define cameras                 D_800CBAF0
#define CameraUpdate            D_800A1044
#define CameraUpdate2           D_800A5E18
#define CameraUpdate3           D_800A5E1C
#define CameraUpdate4           D_800A5E20
#define attractDelayFrames      D_800A5E28
#define pauseRequests           D_800AA010
#define raceLines               D_800A9F9C
#define numTrackSections        D_800A10C2
#ifndef errorString
#define errorString             D_800A0F40
#endif
#endif

#ifdef VERSION_EU
#define ATTRACT_FRAMES 1500
#else
#define ATTRACT_FRAMES 1800
#endif

#define MAX_CHECKPOINTS 6
#define MAX_PLAYERS     4

/* TrackSection.flags */
#define JunctionStart 0x10

/* Face.flags */
#define TRACK_BASE 0x01

/* ShipData.attr */
#define VIEW_IN 0x04
#define RACING  0x40

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

typedef struct ConfigData {
    /* 0x00 */ s32 initMagic;
    /* 0x04 */ u8 trackNum;
    /* 0x05 */ u8 actTrackNum;
    /* 0x06 */ s8 shipNum;
    /* 0x07 */ s8 ship2Num;
    /* 0x08 */ s8 shipClass;
} ConfigData;

typedef struct CheckPoints {
    /* 0x00 */ s16 section;
    /* 0x02 */ s16 time;
    /* 0x04 */ s8 reached;
    /* 0x05 */ s8 padding;
} CheckPoints;

typedef struct ShipData ShipData;

typedef void (*ShipUpdate)();

struct ShipData {
    /* 0x000 */ s16 highResTableIndex;
    /* 0x002 */ s16 lowResTableIndex;
    /* 0x004 */ s32 unk04;
    /* 0x008 */ TrackSection *nearTrkSect;
    /* 0x00C */ TrackSection *prevShipSect;
    /* 0x010 */ s32 attr;
    /* 0x014 */ u8 unk14[0x104 - 0x14];
    /* 0x104 */ ShipUpdate update;
};

typedef struct TrackCamera {
    /* 0x00 */ Skeleton *camPos;
    /* 0x04 */ TrackSection *section;
    /* 0x08 */ s16 hdg, pitch, roll;
    /* 0x0E */ s16 vhdg, vpitch, vroll;
    /* 0x14 */ SVector vel;
    /* 0x1A */ s16 pad;
} TrackCamera;

typedef void (*CameraUpdateFunc)();

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

typedef struct PauseMenu {
    /* 0x00 */ s32 selection;
    /* 0x04 */ s32 musicPaused;
    /* 0x08 */ s32 command;
    /* 0x0C */ s32 count;
    /* 0x10 */ s32 frameCount;
    /* 0x14 */ u8 pad14;
    /* 0x15 */ u8 active;
    /* 0x16 */ u8 showOptions;
    /* 0x17 */ u8 stage;
    /* 0x18 */ u8 confirmed;
    /* 0x19 */ u8 confirmChoice;
} PauseMenu;

typedef struct RaceLine {
    /* 0x00 */ s16 distance;
    /* 0x02 */ u16 pad;
    /* 0x04 */ s32 x, y, z;
} RaceLine;

typedef struct WeaponData WeaponData;
typedef struct WeaponShape WeaponShape;
typedef struct RescueData RescueData;

typedef struct ResetData {
    /* 0x00 */ ShipData *ships;
    /* 0x04 */ ConfigData *config;
    /* 0x08 */ WeaponData *weapons;
    /* 0x0C */ WeaponShape *weaponShapes;
    /* 0x10 */ RescueData *rescue;
    /* 0x14 */ Object **shipShapes;
    /* 0x18 */ CheckPoints *checkPoints;
} ResetData;

extern char errorString[256];
extern u8 noCheckPoints;
extern s8 checkPointReached[MAX_CHECKPOINTS][MAX_PLAYERS];
extern s32 roadSectionCount;
extern s32 weaponGridSections;
extern s32 ownShip;
extern s32 ownShip2;
extern s32 ownShip3;
extern s32 ownShip4;
extern TrackCamera cameras[MAX_PLAYERS];
extern CameraUpdateFunc CameraUpdate;
extern CameraUpdateFunc CameraUpdate2;
extern CameraUpdateFunc CameraUpdate3;
extern CameraUpdateFunc CameraUpdate4;
extern s32 attractDelayFrames;
extern s32 pauseRequests[MAX_PLAYERS];
extern RaceLine *raceLines;
extern s16 numTrackSections;

int sprintf(char *dst, const char *fmt, ...);

void GetFaceVertex(TrackSection *section, Face *face, s32 vertex, Vector *out);
void InitCamera(TrackCamera *camera, TrackSection *section);
void UpdateCameraAttractMode();
s32 SetTrackAttributes(TrackSection *section, s16 sectionNo, WeaponGrid *weaponGrids, s32 gridCount,
                       s16 *specialCount, SpecialGrid *specialGrids);
void UpdateDroneShipStart();

void InitCheckPoints(CheckPoints *checkPoints, ConfigData *config);
void InitWeaponGrids(WeaponGrid *weaponGrids, TrackData *trackData, s32 *gridCount, s16 *specialCount,
                     SpecialGrid *specialGrids);
void InitAttractMode(s32 mode, ShipData *ships);
void InitRaceData(PauseMenu *race);
void InitRaceResetData(ResetData *reset, ShipData *ships, ConfigData *config, WeaponData *weapons,
                       WeaponShape *weaponShapes, RescueData *rescue, Object **shipShapes,
                       CheckPoints *checkPoints);
void InitRaceLine(TrackData *trackData, ConfigData *config);

#endif /* WIPEOUT64_INITRACE_H */
