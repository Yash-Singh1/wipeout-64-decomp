#ifndef WIPEOUT64_SHIPS_H
#define WIPEOUT64_SHIPS_H

#include "common.h"

/*
 * TEMPORARY: radscn.h only declares FrontendOptions up to numPlayers.
 * Rename its declaration so ships.h can expose the additional fields.
 */
#define FrontendOptions FrontendOptionsHead
#define frontendOptions_g frontendOptionsHead_g
#include "radscn.h"
#undef FrontendOptions
#undef frontendOptions_g

#include "dheap.h"
#include "wadfile.h"
#include "skeleton.h"
#include "n64render.h"
#include <ultra64.h>

/*
 * Temporary US/EU symbol mappings until config/{us,eu}/symbol_addrs.txt
 * registers canonical names for ships.c functions and external symbols.
 */
#ifdef VERSION_EU
#define DrawShips                   func_80017EC0
#define DrawStartDroidAndLights     func_80018B00
#define loadShipData                func_80019194
#define initShipData                func_80019580
#define SetSpread                   func_8001AFE4
#define SetGroupPrimColour          func_8001B52C
#define DisplayListLength           func_8001B5CC
#define CopyDisplayList             func_8001B5F8
#define InitGhostShipGroup          func_8001B680
#define InitPlayerShipGroups        func_8001B838

#define lightCount                  D_8008953E
#define shakeAmount                 D_8008957C
#define g_demoShip                  D_80089FA4
#define sillyShips                  D_80089FC0
#define g_ghostActive               D_8008A004
#define g_ghostShipGroup            D_8008A0F0
#define g_ghostShipNo               D_8008A0F4
#define g_spreadScaleVector         D_8008A0F8
#define g_spreadScaleVenom          D_8008A118
#define g_spreadScaleRapier         D_8008A138
#define g_spreadScalePhantom        D_8008A158
#define shipMassClass0              D_8008A178
#define shipMaxThrustClass0         D_8008A184
#define shipResistanceClass0        D_8008A190
#define shipHeadingIncClass0        D_8008A19C
#define shipMaxHeadingClass0        D_8008A1A8
#define shipSkidClass0              D_8008A1B4
#define shipMassClass1              D_8008A1C0
#define shipMaxThrustClass1         D_8008A1CC
#define shipResistanceClass1        D_8008A1D8
#define shipHeadingIncClass1        D_8008A1E4
#define shipMaxHeadingClass1        D_8008A1F0
#define shipSkidClass1              D_8008A1FC
#define shipMassClass2              D_8008A208
#define shipMaxThrustClass2         D_8008A214
#define shipResistanceClass2        D_8008A220
#define shipHeadingIncClass2        D_8008A22C
#define shipMaxHeadingClass2        D_8008A238
#define shipSkidClass2              D_8008A244
#define shipMassClass3              D_8008A250
#define shipMaxThrustClass3         D_8008A25C
#define shipResistanceClass3        D_8008A268
#define shipHeadingIncClass3        D_8008A274
#define shipMaxHeadingClass3        D_8008A280
#define shipSkidClass3              D_8008A28C
#define g_aiMaxThrustVector         D_8008A298
#define g_aiThrustMagVector         D_8008A2D4
#define g_aiMaxThrustVenom          D_8008A310
#define g_aiThrustMagVenom          D_8008A34C
#define g_aiMaxThrustRapier         D_8008A388
#define g_aiThrustMagRapier         D_8008A3C4
#define g_aiMaxThrustPhantom        D_8008A400
#define g_aiThrustMagPhantom        D_8008A43C
#define startLinePos                D_8008A478
#define highResTable                D_8008A4A8
#define shipTex                     D_8008A4C8
#define bigGrid                     D_8008A4CA
#define g_shipUpdatePaused          D_8008F11C
#define g_colorGammaLut             D_80091630
#define g_unk800923F9               D_800919A9
#define lightSections               D_80098E60
#define g_startDroidState           D_80098E70
#define lightState                  D_80098F10
#define g_shipShapes                D_800A9048
#define startDroidObjects           D_800A9200
#define g_finishPosition            D_800A9248
#define activeShipCount             D_800A9250
#define g_mtxPool                   D_800AE4C0
#define lightGroups                 D_800AE4C8
#define g_mtxIndex                  D_800AE514
#define numTrackSections            D_800AE6A2
#define ownShip                     D_800B3414
#define ownShip2                    D_800B756C
#define ownShip3                    D_800B7570
#define ownShip4                    D_800B7574
#define pauseRequests               D_800B75F0
#define attractMode                 D_800B7600
#define raceType                    D_800B7604
#define ships                       D_800B767C
#define g_playerShipGroup           D_800B8390
#define g_playerShipXluGroup        D_800B83A0
#define startGridOrder              D_800B83B0
#define g_polyCount                 D_800B83D0
#define gridPosition                D_800B83D8
#define proximity                   D_800B83F8
#define g_weaponCharge              D_800B84A8
#define g_droidRenderList           D_800C4800
#define g_lightRenderList           D_800C4878
#define g_startDroidRenderList      D_800C4890
#define g_shipRenderList            D_800C4938
#define LoadCmpFiles                func_80016994
#define LoadCompressedTextureSequence func_800172FC
#define WADLoadCompressedTextureSequence func_80017534
#define WADLoadPrm                  func_80011278
#define LoadPrm                     func_8001129C
#define NewObjectInstance           func_800116A4
#define ratan2                      func_8001BBE0
#define RotTrans                    func_8001BF94
#define GetFaceVertex               func_8001F06C
#define GetMagnitude                func_80023EE4
#define UpdatePlayerShipStart       func_800271A4
#define UpdateDroneShipStart        func_80027F30
#else
#define DrawShips                   func_800110A0
#define DrawStartDroidAndLights     func_80011CE0
#define loadShipData                func_80012374
#define initShipData                func_80012760
#define SetSpread                   func_80014054
#define SetGroupPrimColour          func_80014578
#define DisplayListLength           func_80014618
#define CopyDisplayList             func_80014644
#define InitGhostShipGroup          func_800146CC
#define InitPlayerShipGroups        func_80014884

#define lightCount                  D_80091FFE
#define shakeAmount                 D_8009203C
#define g_demoShip                  D_8008A974
#define sillyShips                  D_8008A990
#define g_ghostActive               D_8008A9D4
#define g_ghostShipGroup            D_8008AAC0
#define g_ghostShipNo               D_8008AAC4
#define g_spreadScaleVector         D_8008AAC8
#define g_spreadScaleVenom          D_8008AAE8
#define g_spreadScaleRapier         D_8008AB08
#define g_spreadScalePhantom        D_8008AB28
#define shipMassClass0              D_8008AB48
#define shipMaxThrustClass0         D_8008AB54
#define shipResistanceClass0        D_8008AB60
#define shipHeadingIncClass0        D_8008AB6C
#define shipMaxHeadingClass0        D_8008AB78
#define shipSkidClass0              D_8008AB84
#define shipMassClass1              D_8008AB90
#define shipMaxThrustClass1         D_8008AB9C
#define shipResistanceClass1        D_8008ABA8
#define shipHeadingIncClass1        D_8008ABB4
#define shipMaxHeadingClass1        D_8008ABC0
#define shipSkidClass1              D_8008ABCC
#define shipMassClass2              D_8008ABD8
#define shipMaxThrustClass2         D_8008ABE4
#define shipResistanceClass2        D_8008ABF0
#define shipHeadingIncClass2        D_8008ABFC
#define shipMaxHeadingClass2        D_8008AC08
#define shipSkidClass2              D_8008AC14
#define shipMassClass3              D_8008AC20
#define shipMaxThrustClass3         D_8008AC2C
#define shipResistanceClass3        D_8008AC38
#define shipHeadingIncClass3        D_8008AC44
#define shipMaxHeadingClass3        D_8008AC50
#define shipSkidClass3              D_8008AC5C
#define g_aiMaxThrustVector         D_8008AC68
#define g_aiThrustMagVector         D_8008ACA4
#define g_aiMaxThrustVenom          D_8008ACE0
#define g_aiThrustMagVenom          D_8008AD1C
#define g_aiMaxThrustRapier         D_8008AD58
#define g_aiThrustMagRapier         D_8008AD94
#define g_aiMaxThrustPhantom        D_8008ADD0
#define g_aiThrustMagPhantom        D_8008AE0C
#define startLinePos                D_8008AE48
#define highResTable                D_8008AE78
#define shipTex                     D_8008AE98
#define bigGrid                     D_8008AE9A
#define g_shipUpdatePaused          D_8008FAEC
#define g_colorGammaLut             D_80092080
#define g_unk800923F9               D_800923F9
#define lightSections               D_800AE970
#define g_startDroidState           D_800AE980
#define lightState                  D_800BAA20
#define g_shipShapes                D_8009BA68
#define startDroidObjects           D_8009BC20
#define g_finishPosition            D_8009BC68
#define activeShipCount             D_8009BC70
#define g_mtxPool                   D_800A0EE0
#define lightGroups                 D_800A0EE8
#define g_mtxIndex                  D_800A0F34
#define numTrackSections            D_800A10C2
#define ownShip                     D_800A5E2C
#define ownShip2                    D_800A9F8C
#define ownShip3                    D_800A9F90
#define ownShip4                    D_800A9F94
#define pauseRequests               D_800AA010
#define attractMode                 D_800AA020
#define raceType                    D_800AA024
#define ships                       D_800AA0A4
#define g_playerShipGroup           D_800AADC0
#define g_playerShipXluGroup        D_800AADD0
#define startGridOrder              D_800AADE0
#define g_polyCount                 D_800AAE00
#define gridPosition                D_800AAE08
#define proximity                   D_800AAE28
#define g_weaponCharge              D_800AAED8
#define g_droidRenderList           D_800C44C0
#define g_lightRenderList           D_800C4538
#define g_startDroidRenderList      D_800C4550
#define g_shipRenderList            D_800C45F8
#define LoadCmpFiles                func_8000FB74
#define LoadCompressedTextureSequence func_800104DC
#define WADLoadCompressedTextureSequence func_80010714
#define WADLoadPrm                  func_8000A458
#define LoadPrm                     func_8000A47C
#define NewObjectInstance           func_8000A884
#define ratan2                      func_80014C30
#define RotTrans                    func_80014FE4
#define GetFaceVertex               func_8001812C
#define GetMagnitude                func_8001CEC4
#define UpdatePlayerShipStart       func_8001FF04
#define UpdateDroneShipStart        func_80020C40
#endif

#define TextureTableCount           g_unk8008A970
#define raceLines                   g_unkAlloc9600
#define shipRank                    g_unkAlloc60
#define _droidTexturesFile          D_25FB0
#define _droidModelFile             D_26CB0

#define NO_OF_SHIPS                 15
#define NO_OF_TEAMS                 5
#define MAX_LIGHT_GROUPS            3
#define LIGHT_GROUP_OBJECTS         6
#define LIGHT_GROUP_SKELETON        3
#define START_DROID_HIDDEN          10

/* shipShapes[] layout: three model sets of NO_OF_SHIPS, then the rescue droid. */
#define SHIP_MODEL_SETS             3
#define RescueDroidModel            (NO_OF_SHIPS * SHIP_MODEL_SETS)

/* WAD entry in the startup image that holds the ship models and textures. */
#define STARTUP_SHIP_ENTRY          6
#define MAX_PLAYERS                 4

/* Ship classes (ConfigData.shipClass) */
#define CLASS_VECTOR                0
#define CLASS_VENOM                 1
#define CLASS_RAPIER                2
#define CLASS_PHANTOM               3

/* Race types */
#define RACE_CHAMPIONSHIP           0
#define RACE_TIME_TRIAL             1
#define RACE_MULTIPLAYER            3
#define RACE_CHALLENGE              6

/* Challenge subtypes (FrontendOptions.challengeType) */
#define CHALLENGE_RACE              1
#define CHALLENGE_TIME_TRIAL_2      2
#define CHALLENGE_TIME_TRIAL        4

/* Face.flags */
#define TRACK_BASE                  0x01

/* ShipData.attr */
#define REMOTE_VIEW                 0x0002
#define VIEW_IN                     0x0004
#define DIRECTION                   0x0008
#define LSIDE                       0x0020
#define RACING                      0x0040
#define VISIBLE                     0x0200

/* ShipData.combatAttr */
#define SHIP_CLIPPED                0x0080
#define SHIP_CLOAKED                0x0100
#define SHIP_DESTROYED              0x8000

typedef struct ConfigData {
    /* 0x00 */ s32 initMagic;
    /* 0x04 */ u8 trackNum;
    /* 0x05 */ u8 actTrackNum;
    /* 0x06 */ s8 shipNum;
    /* 0x07 */ s8 ship2Num;
    /* 0x08 */ s8 shipClass;
} ConfigData;

typedef struct TIMlist TIMlist;

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

typedef struct TrackCamera {
    /* 0x00 */ Skeleton *camPos;
    /* 0x04 */ TrackSection *section;
    /* 0x08 */ u8 unk08[0x14];
} TrackCamera; /* size = 0x1C */

typedef struct StartDroidState {
    /* 0x00 */ s32 leftState;
    /* 0x04 */ s32 rightState;
    /* 0x08 */ Vector leftPos;
    /* 0x14 */ Vector rightPos;
    /* 0x20 */ s16 unk20;
    /* 0x22 */ s16 leftHdg;
    /* 0x24 */ s16 unk24;
    /* 0x26 */ s16 rightHdg;
} StartDroidState; /* size = 0x28 */

typedef struct FrontendOptions {
    /* 0x00 */ s32 team;
    /* 0x04 */ u8 teamClass;
    /* 0x05 */ u8 unk05[3];
    /* 0x08 */ u32 trackFile;
    /* 0x0C */ s32 trackNo;
    /* 0x10 */ u8 numPlayers;
    /* 0x11 */ u8 unk11[2];
    /* 0x13 */ u8 mode;
    /* 0x14 */ u8 aiShipsEnabled;
    /* 0x15 */ u8 challengeType;
    /* 0x16 */ u8 unk16[2];
    /* 0x18 */ s32 ownShips[4];
    /* 0x28 */ s32 screenLayout[4];
    /* 0x38 */ s32 playerPad[4];
} FrontendOptions;

typedef struct ShipData {
    /* 0x000 */ s16 highResTableIndex;
    /* 0x002 */ s16 lowResTableIndex;
    /* 0x004 */ s16 farResTableIndex;
    /* 0x006 */ s16 pad06;
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
    /* 0x0F5 */ u8 unkF5;
    /* 0x0F6 */ u8 padF6[2];
    /* 0x0F8 */ s32 unkF8;
    /* 0x0FC */ s16 unkFC;
    /* 0x0FE */ u8 visible[MAX_PLAYERS];
    /* 0x102 */ u8 pad102[2];
    /* 0x104 */ void (*update)();
} ShipData; /* size = 0x108 */

extern u8 _droidTexturesFile[];
extern u8 _droidModelFile[];

extern FrontendOptions frontendOptions_g;
extern WADFILE *g_startupData;
extern s16 TextureTableCount;
extern RaceLine *raceLines;
extern ShipRank *shipRank;
extern s16 lightCount;
extern s32 shakeAmount[MAX_PLAYERS];
extern s32 g_demoShip;
extern u8 sillyShips;
extern s32 g_ghostActive;
extern s16 startLinePos[];
extern u16 highResTable[NO_OF_SHIPS];
extern s16 shipTex;
extern s16 bigGrid;
extern s32 g_shipUpdatePaused;
extern u8 g_colorGammaLut[256];
extern TrackSection *lightSections[MAX_LIGHT_GROUPS];
extern StartDroidState g_startDroidState[MAX_PLAYERS];
extern s32 lightState[MAX_LIGHT_GROUPS];
extern Object *startDroidObjects[];
extern s16 g_finishPosition[MAX_PLAYERS];
extern s16 activeShipCount;
extern N64Mtx *g_mtxPool;
extern Object *lightGroups[MAX_LIGHT_GROUPS][LIGHT_GROUP_OBJECTS];
extern s32 g_mtxIndex;
extern s16 numTrackSections;
extern s32 pauseRequests[MAX_PLAYERS];
extern u8 attractMode;
extern s16 startGridOrder[16];
extern s32 g_polyCount;
extern s16 gridPosition[NO_OF_SHIPS];
extern s32 proximity;
extern s16 g_weaponCharge[NO_OF_SHIPS];
extern RenderList g_droidRenderList;
extern RenderList g_lightRenderList;
extern RenderList g_startDroidRenderList;
extern RenderList g_shipRenderList;
extern RenderGroup *g_ghostShipGroup;
extern s32 g_spreadScaleVector[8];
extern s32 g_spreadScaleVenom[8];
extern s32 g_spreadScaleRapier[8];
extern s32 g_spreadScalePhantom[8];
extern s16 shipMassClass0[];
extern s16 shipMaxThrustClass0[];
extern s16 shipResistanceClass0[];
extern s16 shipHeadingIncClass0[];
extern s16 shipMaxHeadingClass0[];
extern s16 shipSkidClass0[];
extern s16 shipMassClass1[];
extern s16 shipMaxThrustClass1[];
extern s16 shipResistanceClass1[];
extern s16 shipHeadingIncClass1[];
extern s16 shipMaxHeadingClass1[];
extern s16 shipSkidClass1[];
extern s16 shipMassClass2[];
extern s16 shipMaxThrustClass2[];
extern s16 shipResistanceClass2[];
extern s16 shipHeadingIncClass2[];
extern s16 shipMaxHeadingClass2[];
extern s16 shipSkidClass2[];
extern s16 shipMassClass3[];
extern s16 shipMaxThrustClass3[];
extern s16 shipResistanceClass3[];
extern s16 shipHeadingIncClass3[];
extern s16 shipMaxHeadingClass3[];
extern s16 shipSkidClass3[];
extern s32 g_aiMaxThrustVector[NO_OF_SHIPS];
extern s32 g_aiThrustMagVector[NO_OF_SHIPS];
extern s32 g_aiMaxThrustVenom[NO_OF_SHIPS];
extern s32 g_aiThrustMagVenom[NO_OF_SHIPS];
extern s32 g_aiMaxThrustRapier[NO_OF_SHIPS];
extern s32 g_aiThrustMagRapier[NO_OF_SHIPS];
extern s32 g_aiMaxThrustPhantom[NO_OF_SHIPS];
extern s32 g_aiThrustMagPhantom[NO_OF_SHIPS];
extern s32 raceType;
extern s32 g_ghostShipNo;
extern Object *g_shipShapes[];
extern s32 ownShip;
extern s32 ownShip2;
extern s32 ownShip3;
extern s32 ownShip4;
extern ShipData *ships;
extern RenderGroup *g_playerShipGroup[MAX_PLAYERS];
extern RenderGroup *g_playerShipXluGroup[MAX_PLAYERS];
extern u8 g_unk800923F9;

/* External helpers called by ships.c */
int abs(int x);
s32 ratan2(s32 y, s32 x);
void RotTrans(SVECTOR *v0, VECTOR *v1, s32 *flag);
void GetFaceVertex(TrackSection *section, Face *face, s32 vertex, VECTOR *out);
s32 GetMagnitude(VECTOR vec);
void UpdatePlayerShipStart();
void UpdateDroneShipStart();
TIMlist *LoadCompressedTextureSequence(u32 fileOffset);
TIMlist *WADLoadCompressedTextureSequence(WADFILE *wad);
void LoadCmpFiles(TIMlist *timPtr);
Object *WADLoadPrm(WADFILE *wad, RenderList *renderList);
Object *LoadPrm(u32 fileOffset, RenderList *renderList);
Object *NewObjectInstance(Object *obj);

/* ships.c functions */
Gfx *DrawShips(ShipData *shipIndex, Object **shipShapes, s32 camShip, TrackCamera *camera,
               s32 player, Gfx *glistp);
Gfx *DrawStartDroidAndLights(Gfx *glistp, TrackCamera *camera, s32 player);
void loadShipData(TrackSection *section, ShipData *shipIndex, Object **shipShapes, Skeleton *camPos);
void initShipData(TrackSection *section, ShipData *shipIndex, ConfigData *options);
void SetSpread(ShipData *shipIndex, ConfigData *cfg, s16 pos, s16 num, s16 big);
void SetGroupPrimColour(RenderGroup *group, u8 red, u8 green, u8 blue);
s32 DisplayListLength(Gfx *dl);
void CopyDisplayList(Gfx *dst, Gfx *src, s32 restoreOpaque);
void InitGhostShipGroup(s32 shipNo);
void InitPlayerShipGroups(void);

#endif /* WIPEOUT64_SHIPS_H */
