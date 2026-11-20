#ifndef WIPEOUT64_SHIPCTRL_H
#define WIPEOUT64_SHIPCTRL_H

#include "common.h"

/*
 * TEMPORARY: radscn.h only declares FrontendOptions up to numPlayers.
 * Rename its declaration so shipctrl.h can expose the additional fields.
 */
#define FrontendOptions FrontendOptionsHead
#define frontendOptions_g frontendOptionsHead_g
#include "radscn.h"
#undef FrontendOptions
#undef frontendOptions_g

#include "pad.h"

/*
 * Temporary US/EU symbol mappings until config/{us,eu}/symbol_addrs.txt
 * registers canonical names for shipctrl.c functions and external symbols.
 */
#ifdef VERSION_EU
#define UpdateShips                 func_800269B0
#define SortAllShips                func_80026E00
#define UpdatePlayerShipStart       func_800271A4
#define SetUpFirstCheckPoint        func_80027400
#define UpdatePlayerShip            func_800275E4
#define GeneralStartProcs           func_80027DFC

#define StartMusic                  func_8000FC8C
#define PlayNote                    func_8001BCE8
#define UpdateCameraRaceExternal    func_80021BEC
#define GetAng                      func_80023CB4
#define GetMagnitude                func_80023EE4
#define newRand                     func_80023FCC
#define ShipTrkReaction             func_80024BF0
#define SetUnitVectors              func_80026314
#define UpdateDroneShip             func_80028220
#define doShipShipCollision         func_8002AC50
#define SetBrake                    func_8002E6EC
#define SetView                     func_8002E794
#define SetFire                     func_8002E8C8
#define SetDitch                    func_8002EBB4
#define SetShake                    func_8000D5D4
#define buildGhostReplayData        func_80070CF4

#define g_demoShip                  D_80089FA4
#define turnDivisor                 D_80089FB0
#define startLinePos                D_8008A478
#define g_shipUpdatePaused          D_8008F11C
#define classCheckTime              D_80091130
#define startCountdown              D_80091140
#define g_steeringActive               D_80091490
#define countdownActive             D_80089540
#define g_pauseDebounceTimer               D_800917E4
#define checkPoints                 D_800A8FC8
#define checkPointReached           D_800A9228
#define activeShipCount             D_800A9250
#define tempGameData                D_800AE620
#define CameraUpdate                D_800AE624
#define CameraUpdate2               D_800B33F8
#define CameraUpdate3               D_800B33FC
#define CameraUpdate4               D_800B3400
#define ownShip                     D_800B3414
#define shipDeathCount              D_800B74A0
#define ownShip2                    D_800B756C
#define ownShip3                    D_800B7570
#define ownShip4                    D_800B7574
#define raceType                    D_800B7604
#define g_attractMode               D_800B7660
#define ghostControl                D_800B7668
#define ships                       D_800B767C
#define playerPosition              D_800B7682
#define player2Position             D_800B76AA
#define player3Position             D_800B76AC
#define player4Position             D_800B76B4
#define checkTimer                  D_800CE1D0
#define checkTimer2                 D_800CE9F8
#define checkTimer3                 D_800CE9FC
#define checkTimer4                 D_800CEA00
#else
#define UpdateShips                 func_8001F710
#define SortAllShips                func_8001FB60
#define UpdatePlayerShipStart       func_8001FF04
#define SetUpFirstCheckPoint        func_80020160
#define UpdatePlayerShip            func_8002033C
#define GeneralStartProcs           func_80020B0C

#define StartMusic                  func_80008E5C
#define PlayNote                    func_80014D38
#define UpdateCameraRaceExternal    func_8001ACAC
#define GetAng                      func_8001CC94
#define GetMagnitude                func_8001CEC4
#define newRand                     func_8001CFAC
#define ShipTrkReaction             func_8001DB1C
#define SetUnitVectors              func_8001F078
#define UpdateDroneShip             func_80020F30
#define doShipShipCollision         func_800237B0
#define SetBrake                    func_8002704C
#define SetView                     func_800270F4
#define SetFire                     func_80027228
#define SetDitch                    func_80027514
#define SetShake                    func_80049720
#define buildGhostReplayData        func_8006ED54

#define g_demoShip                  D_8008A974
#define turnDivisor                 D_8008A980
#define startLinePos                D_8008AE48
#define g_shipUpdatePaused          D_8008FAEC
#define classCheckTime              D_80091B00
#define startCountdown              D_80091B10
#define g_steeringActive               D_80091E60
#define countdownActive             D_80092000
#define g_pauseDebounceTimer               D_80092234
#define checkPoints                 D_8009B9E8
#define checkPointReached           D_8009BC48
#define activeShipCount             D_8009BC70
#define tempGameData                D_800A1040
#define CameraUpdate                D_800A1044
#define CameraUpdate2               D_800A5E18
#define CameraUpdate3               D_800A5E1C
#define CameraUpdate4               D_800A5E20
#define ownShip                     D_800A5E2C
#define shipDeathCount              D_800A9EC0
#define ownShip2                    D_800A9F8C
#define ownShip3                    D_800A9F90
#define ownShip4                    D_800A9F94
#define raceType                    D_800AA024
#define g_attractMode               D_800AA088
#define ghostControl                D_800AA090
#define ships                       D_800AA0A4
#define playerPosition              D_800AA0C0
#define player2Position             D_800AA0D6
#define player3Position             D_800AA0D8
#define player4Position             D_800AA0E0
#define checkTimer                  D_800CDE90
#define checkTimer2                 D_800CE6B8
#define checkTimer3                 D_800CE6BC
#define checkTimer4                 D_800CE6C0
#endif

#define NO_OF_SHIPS                 15
#define MAX_CHECKPOINTS             6
#define MAX_PLAYERS                 4

/* Face.flags */
#define TRACK_BASE                  0x01
#define CHECKPOINT                  0x80

/* ShipData.attr */
#define VIEW_IN                     0x0004
#define RACING                      0x0040
#define ELECTROED                   0x8000

/* ShipData.combatAttr */
#define SHIP_CRASHED                0x0004
#define SHIP_LOW_SHIELD             0x0008
#define SHIP_DESTROYED              0x8000

/* Race types */
#define RACE_TIME_TRIAL             1
#define RACE_MULTIPLAYER            3
#define RACE_CHALLENGE              6

/* Challenge subtypes */
#define CHALLENGE_TIME_TRIAL        2

/* Sound effect IDs */
#define SFX_START_STALL             0x04
#define SFX_COUNTDOWN_THREE         0x05
#define SFX_COUNTDOWN_TWO           0x08
#define SFX_COUNTDOWN_GO            0x17
#define SFX_COUNTDOWN_ONE           0x21
#define SFX_TURBO                   0x27

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
} Face; /* size = 0x14 */

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
} CheckPoints; /* size = 0x06 */

typedef struct RaceLine {
    /* 0x00 */ s16 distance;
    /* 0x02 */ u16 pad;
    /* 0x04 */ s32 x;
    /* 0x08 */ s32 y;
    /* 0x0C */ s32 z;
} RaceLine; /* size = 0x10 */

typedef struct ShipRank {
    /* 0x00 */ s16 position;
    /* 0x02 */ s16 shipNo;
} ShipRank; /* size = 0x04 */

typedef struct FrontendOptions {
    /* 0x00 */ s32 team;
    /* 0x04 */ u8 teamClass;
    /* 0x05 */ u8 unk05[3];
    /* 0x08 */ u32 trackFile;
    /* 0x0C */ s32 trackNo;
    /* 0x10 */ u8 numPlayers;
    /* 0x11 */ u8 unk11[3];
    /* 0x14 */ u8 aiShipsEnabled;
    /* 0x15 */ u8 challengeType;
    /* 0x16 */ u8 unk16[2];
    /* 0x18 */ s32 ownShips[4];
    /* 0x28 */ s32 screenLayout[4];
    /* 0x38 */ s32 playerPad[4];
    /* 0x48 */ u8 unk48[0x12];
    /* 0x5A */ u8 catchUp;
} FrontendOptions;

typedef struct WeaponData WeaponData;
typedef struct WeaponShape WeaponShape;
typedef void (*CameraUpdateFunc)();

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
    /* 0x0F0 */ s32 eliminations;
    /* 0x0F4 */ u8 activeInLeague;
    /* 0x0F5 */ u8 unkF5[7];
    /* 0x0FC */ s16 finishPosition;
    /* 0x0FE */ u8 unkFE;
    /* 0x0FF */ u8 unkFF[5];
    /* 0x104 */ void (*update)(struct ShipData *shipIndex, Object **shipShapes, s32 shipNo,
                               WeaponData *weaponIndex, PadInfo *controller);
} ShipData; /* size = 0x108 */

extern FrontendOptions frontendOptions_g;
extern s32 g_demoShip;
extern u8 turnDivisor;
extern s16 startLinePos[];
extern s32 g_shipUpdatePaused;
extern s32 classCheckTime[];
extern s32 startCountdown;
extern s32 g_steeringActive;
extern s8 countdownActive;
extern s32 g_pauseDebounceTimer;
extern CheckPoints checkPoints[MAX_CHECKPOINTS];
extern s8 checkPointReached[MAX_CHECKPOINTS][MAX_PLAYERS];
extern s16 activeShipCount;
extern ConfigData *tempGameData;
extern CameraUpdateFunc CameraUpdate;
extern CameraUpdateFunc CameraUpdate2;
extern CameraUpdateFunc CameraUpdate3;
extern CameraUpdateFunc CameraUpdate4;
extern s32 ownShip;
extern s32 shipDeathCount[NO_OF_SHIPS];
extern s32 ownShip2;
extern s32 ownShip3;
extern s32 ownShip4;
extern RaceLine *raceLines;
extern s32 raceType;
extern ShipRank *shipRank;
extern s32 g_attractMode;
extern PadInfo ghostControl;
extern ShipData *ships;
extern s16 playerPosition;
extern s16 player2Position;
extern s16 player3Position;
extern s16 player4Position;
extern s32 checkTimer;
extern s32 checkTimer2;
extern s32 checkTimer3;
extern s32 checkTimer4;

/* External helpers called by shipctrl.c */
int abs(int x);
void StartMusic(void);
s16 rsin(s16 a);
s16 rcos(s16 a);
void PlayNote(s32 id, s32 vol, s32 a2, s32 a3, s32 pitch);
void UpdateCameraRaceExternal();
s32 GetAng(VECTOR vec1, VECTOR vec2);
s32 GetMagnitude(VECTOR vec);
s32 newRand(void);
void ShipTrkReaction(ShipData *ship, Object **shipShapes, s32 shipNo);
void SetUnitVectors(ShipData *shipIndex);
void UpdateDroneShip(ShipData *shipIndex, Object **shipShapes, s32 shipNo,
                     WeaponData *weaponIndex, PadInfo *controller);
void doShipShipCollision(s32 shipNo, ShipData *shipIndex, Object **shipShapes);
void SetBrake(ShipData *ship, PadInfo *controller);
void SetView(ShipData *ship, PadInfo *controller, s32 shipNo);
void SetFire(ShipData *ship, WeaponData *weapons, s32 shipNo, PadInfo *controller);
void SetDitch(ShipData *ship, WeaponData *weapons, s32 shipNo, PadInfo *controller);
void SetShake(s32 strength, s32 player);
void buildGhostReplayData(ShipData *shipIndex);

/* shipctrl.c functions */
void UpdateShips(ShipData *shipIndex, WeaponShape *unused, Object **shipShapes,
                 WeaponData *weaponIndex);
void SortAllShips(ShipData *shipIndex);
void UpdatePlayerShipStart(ShipData *shipIndex, Object **shipShapes, s32 shipNo,
                           WeaponData *weaponIndex, PadInfo *controller);
void SetUpFirstCheckPoint(void);
void UpdatePlayerShip(ShipData *shipIndex, Object **shipShapes, s32 shipNo,
                      WeaponData *weaponIndex, PadInfo *controller);
void GeneralStartProcs(ShipData *ship);

#endif /* WIPEOUT64_SHIPCTRL_H */
