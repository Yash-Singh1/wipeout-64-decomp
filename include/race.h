/*
 * Race initialization and heap lifecycle (Race.c).
 *
 * Allocates per-race resources (ships, weapons, HUD sprites, render lists,
 * track geometry, checkpoints, weapon grids, cameras, and challenge medals)
 * and manages the temporary race heap reset between races.
 */
#ifndef WIPEOUT64_RACE_H
#define WIPEOUT64_RACE_H

#include "common.h"

/*
 * TEMPORARY: radscn.h only declares FrontendOptions up to numPlayers. Race.c needs the
 * fields below as well; fold them into radscn.h and drop this rename.
 */
#define FrontendOptions FrontendOptionsHead
#define frontendOptions_g frontendOptionsHead_g
#include "radscn.h"
#undef FrontendOptions
#undef frontendOptions_g

#include "dheap.h"
#include "n64init.h"
#include "wadfile.h"

typedef struct FrontendOptions {
    /* 0x00 */ s32 team;
    /* 0x04 */ u8 teamClass;
    /* 0x08 */ u32 trackFile;  /* ROM file offset of the track WAD */
    /* 0x0C */ s32 trackNo;
    /* 0x10 */ u8 numPlayers;
    /* 0x11 */ u8 unk11[4];
    /* 0x15 */ u8 challengeMedal;
    /* 0x16 */ u8 unk16;
    /* 0x18 */ s32 ownShips[4];
} FrontendOptions;

extern FrontendOptions frontendOptions_g;

#define NO_OF_SHIPS 15
#define NO_OF_HUD_SPRITES 25
#define NO_OF_CHECKPOINTS 6
#define NO_OF_DAMAGE_SLOTS 15

/* Race configuration (derived from Wipeout XL ConfigData). */
typedef struct ConfigData {
    /* 0x00 */ s32 initMagic;
    /* 0x04 */ u8 trackNum;
    /* 0x05 */ u8 actTrackNum;
    /* 0x06 */ s8 shipNum;
    /* 0x07 */ s8 ship2Num;
    /* 0x08 */ s8 shipClass;
    /* 0x09 */ u8 unk09[0x2E - 0x09];
    /* 0x2E */ u8 unk2E;
} ConfigData;

typedef struct ShipData {
    /* 0x000 */ s16 highResTableIndex;
    /* 0x002 */ s16 lowResTableIndex;
    /* 0x004 */ s32 unk04;
    /* 0x008 */ TrackSection *nearTrkSect;
    /* 0x00C */ u8 unk0C[0x108 - 0x0C];
} ShipData;

/* Track camera state (0x1C bytes); one per active player. */
typedef struct TrackCamera {
    /* 0x00 */ Skeleton *camPos;
    /* 0x04 */ TrackSection *section;
    /* 0x08 */ s16 hdg, pitch, roll;
    /* 0x0E */ s16 vhdg, vpitch, vroll;
    /* 0x14 */ s16 vel[4];
} TrackCamera;

typedef struct CheckPoint {
    /* 0x00 */ s16 section;
    /* 0x02 */ s16 bonusSeconds;
    /* 0x04 */ s8 done;
} CheckPoint;

/* HUD sprite image entry as stored in the texture table. */
typedef struct SpriteImage {
    /* 0x00 */ s16 is8Bit;
    /* 0x02 */ s16 unk02[2];
    /* 0x06 */ s16 width;
    /* 0x08 */ s16 height;
    /* 0x0C */ void *pixels;
    /* 0x10 */ void *palette;
} SpriteImage;

typedef struct RescueData {
    /* 0x00 */ u8 data[0x4C];
} RescueData;

typedef struct PauseData {
    /* 0x00 */ u8 unk00[0x18];
    /* 0x18 */ u8 menu;
    /* 0x19 */ u8 selection;
} PauseData;

typedef void (*CameraUpdateFunc)();

/*
 * Retail USA / Europe symbol address aliases until promoted to config/{us,eu}/symbol_addrs.txt.
 */
#ifdef VERSION_EU
#define ships                    D_800B767C
#define shipShapes               D_800A9048
#define cameras                  D_800CBE30
#define ownShip                  D_800B3414
#define ownShip2                 D_800B756C
#define ownShip3                 D_800B7570
#define ownShip4                 D_800B7574
#define gameData                 D_800B74E0
#define tempGameData             D_800AE620
#define checkPoints              D_800A8FC8
#define rescueData               D_800A9310
#define resetData                D_800B7640
#define pauseData                D_800B7550
#define startDroidObjects        D_800A9200
#define lightGroups              D_800AE4C8
#define prmObjects               D_800B8B20
#define weaponGrid               D_800A8FF4
#define noWeaponGrids            D_80089FC4
#define noSpecialGrids           D_80089FCC
#define RaceCompleteDList        D_80095FF8
#define PauseDList               D_80095FF0
#define challengeMedalObject     D_8008A010
#define CameraUpdate             D_800AE624
#define CameraUpdate2            D_800B33F8
#define CameraUpdate3            D_800B33FC
#define CameraUpdate4            D_800B3400
#define spriteTextures           D_800B2778
#define spriteTextureBase        D_800B3404
#define hudSprites               D_800BA3C0
#define weaponShapes             D_800B7498
#define weaponData               D_800A94BC
#define particleSlots            D_800B7698
#define g_raceSectionResetFlag   D_80089FC8
#define g_raceTimer0             D_800912C8
#define g_raceTimer1             D_800912CC
#define g_effectTexStart         D_800912DC
#define g_ghostLapTime0          D_800912F0
#define g_challengeMedalLevel    D_80091434
#define g_multiplayerFlag        D_80095F44
#define g_scenePrmIndex          D_80095F48
#define g_scene2PrmIndex         D_80095F4C
#define g_trackTextureCount      D_80095F50
#define g_prevTrackNo            D_800A8FEC
#define g_prevTeamClass          D_800A8FF0
#define shipDamage               D_800A92C0
#define g_hudTimers              D_800A9440
#define g_hudLaps                D_800AE628
#define g_shipClassIndex         D_800AE6A0
#define g_trackIndex             D_800B340C
#define g_ghostResetFlag         D_800B3426
#define g_gameType               D_800B7604
#define g_prevTeam               D_800B765C
#define g_attractMode            D_800B7660
#define g_shipEffectA            D_800B8540
#define g_weaponTargetData       D_800B8AC8
#define g_shipEffectB            D_800B8C20
#define g_miscRenderListA        D_800C47E8
#define g_trackRenderList        D_800C4818
#define g_miscRenderListB        D_800C4830
#define g_miscRenderListC        D_800C48A8
#define g_trailRenderList        D_800C4908
#define g_shipRenderList         D_800C4938
#define g_raceLineFlags          D_800C5C50
#define g_sceneObjectCount       D_800C5EE0
#define WADInitScene             func_80016600
#define WADLoadCompressedTextureSequence func_80017534
#define LoadCmpFiles             func_80016994
#define WADInitVertices          func_8001CBB4
#define WADInitFaces             func_8001D058
#define WADInitSections          func_8001D254
#define WADInitViewList          func_8001D49C
#define InitCheckPoints          func_8001F2F0
#define InitWeaponGrids          func_8001F5FC
#define InitRaceResetData        func_8001FA30
#define InitRaceLine             func_8001FA60
#define initShipData             func_80019580
#define InitCamera               func_80021B50
#define UpdateCameraRaceStart    func_80022038
#define InitRescueDroid          func_80030048
#define LoadStartDroid           func_80009F1C
#define InitDroidThings          func_8000A8CC
#define InitParticle             func_80068230
#define InitExplode              func_800689E4
#define InitRing                 func_80069850
#define WADLoadPrm               func_80011278
#define LoadShipShapes           func_80019194
#define StartMusic               func_8000FC8C
#define InitShipRenderLists      func_8001B838
#define InitShipShadows          func_8001B680
#define InitTrackDisplayLists    func_8001D3CC
#define SetUnitVectors           func_80026314
#define InitSpeedoSprites        func_80031BA0
#define InitWeapons              func_8003F0C8
#define ResetWeapons             func_8003F78C
#define ResetEffects             func_80007CC0
#define InitEffects              func_8000BFD8
#define InitShipEffects          func_8000C9E8
#define SetupTrack3Anim          func_8004F538
#define SetupTrack4Anim          func_8004FDD4
#define SetupTrack6Anim          func_8004FF54
#define SetupTrack2Anim          func_80050804
#define SetupTrack5Anim          func_80050C44
#define InitRenderList           func_80051474
#define InitSky                  func_80066660
#define LoadExplodeGFX           func_80068968
#define ResetRace                func_80070820
#define InspectLoadAiDamage      func_80071518
#define InspectLoadAiShips       func_800717A4
#define InitTrails               func_8000E630
#define InitRaceResources        func_80030EA0
#define ResetRaceHeaps           func_80031128
#define InitRace                 func_80031180
#else
#define ships                    D_800AA0A4
#define shipShapes               D_8009BA68
#define cameras                  D_800CBAF0
#define ownShip                  D_800A5E2C
#define ownShip2                 D_800A9F8C
#define ownShip3                 D_800A9F90
#define ownShip4                 D_800A9F94
#define gameData                 D_800A9F00
#define tempGameData             D_800A1040
#define checkPoints              D_8009B9E8
#define rescueData               D_8009BD30
#define resetData                D_800AA068
#define pauseData                D_800A9F70
#define startDroidObjects        D_8009BC20
#define lightGroups              D_800A0EE8
#define prmObjects               D_800AB550
#define weaponGrid               D_8009BA14
#define noWeaponGrids            D_8008A994
#define noSpecialGrids           D_8008A99C
#define RaceCompleteDList        D_80096A48
#define PauseDList               D_80096A40
#define challengeMedalObject     D_8008A9E0
#define CameraUpdate             D_800A1044
#define CameraUpdate2            D_800A5E18
#define CameraUpdate3            D_800A5E1C
#define CameraUpdate4            D_800A5E20
#define spriteTextures           D_800A5198
#define spriteTextureBase        D_800A5E24
#define hudSprites               D_800ACDF0
#define weaponShapes             D_800A9EB8
#define weaponData               D_8009BEDC
#define particleSlots            D_800AA0C4
#define g_raceSectionResetFlag   D_8008A998
#define g_raceTimer0             D_80091C98
#define g_raceTimer1             D_80091C9C
#define g_effectTexStart         D_80091CAC
#define g_ghostLapTime0          D_80091CC0
#define g_challengeMedalLevel    D_80091E04
#define g_multiplayerFlag        D_80096994
#define g_scenePrmIndex          D_80096998
#define g_scene2PrmIndex         D_8009699C
#define g_trackTextureCount      D_800969A0
#define g_prevTrackNo            D_8009BA0C
#define g_prevTeamClass          D_8009BA10
#define shipDamage               D_8009BCE0
#define g_hudTimers              D_8009BE60
#define g_hudLaps                D_800A1048
#define g_shipClassIndex         D_800A10C0
#define g_trackIndex             D_800A5E30
#define g_ghostResetFlag         D_800A5E46
#define g_gameType               D_800AA024
#define g_prevTeam               D_800AA084
#define g_attractMode            D_800AA088
#define g_shipEffectA            D_800AAF70
#define g_weaponTargetData       D_800AB4F8
#define g_shipEffectB            D_800AB650
#define g_miscRenderListA        D_800C44A8
#define g_trackRenderList        D_800C44D8
#define g_miscRenderListB        D_800C44F0
#define g_miscRenderListC        D_800C4568
#define g_trailRenderList        D_800C45C8
#define g_shipRenderList         D_800C45F8
#define g_raceLineFlags          D_800C5910
#define g_sceneObjectCount       D_800C5BA0
#define WADInitScene             func_8000F7E0
#define WADLoadCompressedTextureSequence func_80010714
#define LoadCmpFiles             func_8000FB74
#define WADInitVertices          func_80015CA0
#define WADInitFaces             func_80016144
#define WADInitSections          func_80016340
#define WADInitViewList          func_80016590
#define InitCheckPoints          func_800183B0
#define InitWeaponGrids          func_800186BC
#define InitRaceResetData        func_80018AF0
#define InitRaceLine             func_80018B20
#define initShipData             func_80012760
#define InitCamera               func_8001AC10
#define UpdateCameraRaceStart    func_8001B084
#define InitRescueDroid          func_80028944
#define LoadStartDroid           func_80046068
#define InitDroidThings          func_80046A18
#define InitParticle             func_800664A0
#define InitExplode              func_80066C10
#define InitRing                 func_80067A7C
#define WADLoadPrm               func_8000A458
#define LoadShipShapes           func_80012374
#define StartMusic               func_80008E5C
#define InitShipRenderLists      func_80014884
#define InitShipShadows          func_800146CC
#define InitTrackDisplayLists    func_800164C0
#define SetUnitVectors           func_8001F078
#define InitSpeedoSprites        func_8002A490
#define InitWeapons              func_800379D8
#define ResetWeapons             func_8003809C
#define ResetEffects             func_80043BA0
#define InitEffects              func_80048124
#define InitShipEffects          func_80048B34
#define SetupTrack3Anim          func_8004D76C
#define SetupTrack4Anim          func_8004E008
#define SetupTrack6Anim          func_8004E188
#define SetupTrack2Anim          func_8004EA38
#define SetupTrack5Anim          func_8004EE78
#define InitRenderList           func_8004F6E4
#define InitSky                  func_800648F0
#define LoadExplodeGFX           func_80066B94
#define ResetRace                func_8006E880
#define InspectLoadAiDamage      func_8006F578
#define InspectLoadAiShips       func_8006F804
#define InitTrails               func_80070F1C
#define InitRaceResources        func_80029790
#define ResetRaceHeaps           func_80029A18
#define InitRace                 func_80029A70
#endif

extern ShipData *ships;
extern Object *shipShapes[];
extern TrackCamera cameras[4];
extern s32 ownShip;
extern s32 ownShip2;
extern s32 ownShip3;
extern s32 ownShip4;
extern ConfigData gameData;
extern ConfigData *tempGameData;
extern CheckPoint checkPoints[NO_OF_CHECKPOINTS];
extern RescueData rescueData[4];
extern u8 resetData[];
extern PauseData pauseData;
extern Object *startDroidObjects[];
extern Object *lightGroups[];
extern Object *prmObjects[];
extern void *weaponGrid;
extern s32 noWeaponGrids;
extern s16 noSpecialGrids;
extern Gfx *RaceCompleteDList;
extern Gfx *PauseDList;
extern Object *challengeMedalObject;
extern CameraUpdateFunc CameraUpdate;
extern CameraUpdateFunc CameraUpdate2;
extern CameraUpdateFunc CameraUpdate3;
extern CameraUpdateFunc CameraUpdate4;
extern SpriteImage *spriteTextures[];
extern s16 spriteTextureBase;
extern uSprite hudSprites[NO_OF_HUD_SPRITES];
extern void *weaponShapes;
extern void *weaponData;
extern s32 particleSlots;

extern s16 g_raceSectionResetFlag;
extern s32 g_raceTimer0;
extern s32 g_raceTimer1;
extern s16 g_effectTexStart;
extern s32 g_ghostLapTime0;
extern s32 g_challengeMedalLevel;
extern s32 g_multiplayerFlag;
extern s32 g_scenePrmIndex;
extern s32 g_scene2PrmIndex;
extern s32 g_trackTextureCount;
extern s32 g_prevTrackNo;
extern u8 g_prevTeamClass;
extern s32 shipDamage[NO_OF_DAMAGE_SLOTS];
extern u8 g_hudTimers[];
extern u8 g_hudLaps[];
extern s16 g_shipClassIndex;
extern s16 g_trackIndex;
extern s16 g_ghostResetFlag;
extern s32 g_gameType;
extern s32 g_prevTeam;
extern s32 g_attractMode;
extern u8 g_shipEffectA[];
extern u8 g_weaponTargetData[];
extern u8 g_shipEffectB[];
extern u8 g_miscRenderListA[];
extern u8 g_trackRenderList[];
extern u8 g_miscRenderListB[];
extern u8 g_miscRenderListC[];
extern u8 g_trailRenderList[];
extern u8 g_shipRenderList[];
extern s16 g_raceLineFlags;
extern s16 g_sceneObjectCount;

s16 WADInitScene(Object **objects, s16 *count, Skeleton *camPos, ConfigData *config, WADFILE *wad);
void *WADLoadCompressedTextureSequence(WADFILE *wad);
void LoadCmpFiles(void *sequence);
void WADInitVertices(WADFILE *wad);
void WADInitFaces(WADFILE *wad);
void WADInitSections(WADFILE *wad);
void WADInitViewList(WADFILE *wad);
void InitCheckPoints(CheckPoint *checkPoints, ConfigData *config);
void InitWeaponGrids(void *weaponGrid, TrackData *track, s32 *noWeaponGrids, s16 *noSpecialGrids,
                     CheckPoint *checkPoints, ConfigData *config);
void InitRaceResetData(void *reset, ShipData *ships, ConfigData *config, void *weapons,
                       void *weaponShapes, RescueData *rescue, Object **shipShapes,
                       CheckPoint *checkPoints);
void InitRaceLine(TrackData *track, ConfigData *config);
void initShipData(TrackSection *sections, ShipData *ships, ConfigData *config);
void InitCamera(TrackCamera *camera, TrackSection *section);
void UpdateCameraRaceStart();
void InitRescueDroid(RescueData *rescue, Object **shipShapes, ShipData *ship);
void LoadStartDroid(Object **objects, const char *name, Skeleton *camPos, CheckPoint *checkPoints,
                    ConfigData *config);
void InitDroidThings(Object **objects, const char *name, Skeleton *camPos, CheckPoint *checkPoints,
                     ConfigData *config);
s32 InitParticle(void);
void InitExplode(void);
void InitRing(void);
Object *WADLoadPrm(WADFILE *wad, void *textures);
void LoadShipShapes(s32 index, ShipData *ships, Object **shipShapes, Skeleton *camPos);

void StartMusic(void);
void InitShipRenderLists(void);
void InitShipShadows(s32 ship);
void InitTrackDisplayLists(void);
void SetUnitVectors(ShipData *ships);
void InitSpeedoSprites(void *a, void *b);
void InitWeapons(void *weaponShapes, Skeleton *camPos, void *weapons, void *d);
void ResetWeapons(void);
void ResetEffects(void);
void InitEffects(void);
void InitShipEffects(void *a, void *b);
void SetupTrack3Anim(void);
void SetupTrack4Anim(void);
void SetupTrack6Anim(void);
void SetupTrack2Anim(void);
void SetupTrack5Anim(void);
void InitRenderList(void *anim, s32 start, s32 count);
void InitSky(void);
void LoadExplodeGFX(void);
void ResetRace(void *reset, PauseData *pause, s32 mode);
void InspectLoadAiDamage(void);
void InspectLoadAiShips(void);
void InitTrails(void);

void InitRaceResources(void);
void ResetRaceHeaps(void);
void InitRace(void);

#endif /* WIPEOUT64_RACE_H */
