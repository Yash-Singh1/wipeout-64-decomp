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

/* XL ConfigData, as far as Race.c touches it. */
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

/* XL TrackCamera (0x1C bytes); one per player. */
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

/* HUD sprite image as stored in the texture table. */
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
 * TEMPORARY: retail USA / Europe addresses of externals that symbol_addrs.txt does not
 * name yet. Once they are named in config/{us,eu}/symbol_addrs.txt this block goes away.
 */
#ifdef VERSION_EU
#define ships               D_800B767C
#define shipShapes          D_800A9048
#define cameras             D_800CBE30
#define ownShip             D_800B3414
#define ownShip2            D_800B756C
#define ownShip3            D_800B7570
#define ownShip4            D_800B7574
#define gameData            D_800B74E0
#define tempGameData        D_800AE620
#define checkPoints         D_800A8FC8
#define rescueData          D_800A9310
#define resetData           D_800B7640
#define pauseData           D_800B7550
#define startDroidObjects   D_800A9200
#define lightGroups         D_800AE4C8
#define prmObjects          D_800B8B20
#define weaponGrid          D_800A8FF4
#define noWeaponGrids       D_80089FC4
#define noSpecialGrids      D_80089FCC
#define RaceCompleteDList   D_80095FF8
#define PauseDList          D_80095FF0
#define challengeMedalObject D_8008A010
#define CameraUpdate        D_800AE624
#define CameraUpdate2       D_800B33F8
#define CameraUpdate3       D_800B33FC
#define CameraUpdate4       D_800B3400
#define spriteTextures      D_800B2778
#define spriteTextureBase   D_800B3404
#define hudSprites          D_800BA3C0
#define weaponShapes        D_800B7498
#define weaponData          D_800A94BC
#define particleSlots       D_800B7698
#define D_8008A998 D_80089FC8
#define D_80091C98 D_800912C8
#define D_80091C9C D_800912CC
#define D_80091CAC D_800912DC
#define D_80091CC0 D_800912F0
#define D_80091E04 D_80091434
#define D_80096994 D_80095F44
#define D_80096998 D_80095F48
#define D_8009699C D_80095F4C
#define D_800969A0 D_80095F50
#define D_8009BA0C D_800A8FEC
#define D_8009BA10 D_800A8FF0
#define D_8009BCE0 D_800A92C0
#define D_8009BE60 D_800A9440
#define D_800A1048 D_800AE628
#define D_800A10C0 D_800AE6A0
#define D_800A5E30 D_800B340C
#define D_800A5E46 D_800B3426
#define D_800AA024 D_800B7604
#define D_800AA084 D_800B765C
#define D_800AA088 D_800B7660
#define D_800AAF70 D_800B8540
#define D_800AB4F8 D_800B8AC8
#define D_800AB650 D_800B8C20
#define D_800C44A8 D_800C47E8
#define D_800C44D8 D_800C4818
#define D_800C44F0 D_800C4830
#define D_800C4568 D_800C48A8
#define D_800C45C8 D_800C4908
#define D_800C45F8 D_800C4938
#define D_800C5910 D_800C5C50
#define D_800C5BA0 D_800C5EE0
#define WADInitScene        func_80016600
#define WADLoadCompressedTextureSequence func_80017534
#define LoadCmpFiles        func_80016994
#define WADInitVertices     func_8001CBB4
#define WADInitFaces        func_8001D058
#define WADInitSections     func_8001D254
#define WADInitViewList     func_8001D49C
#define InitCheckPoints     func_8001F2F0
#define InitWeaponGrids     func_8001F5FC
#define InitRaceResetData   func_8001FA30
#define InitRaceLine        func_8001FA60
#define initShipData        func_80019580
#define InitCamera          func_80021B50
#define UpdateCameraRaceStart func_80022038
#define InitRescueDroid     func_80030048
#define LoadStartDroid      func_80009F1C
#define InitDroidThings     func_8000A8CC
#define InitParticle        func_80068230
#define InitExplode         func_800689E4
#define InitRing            func_80069850
#define WADLoadPrm          func_80011278
#define LoadShipShapes      func_80019194
#define func_80008E5C func_8000FC8C
#define func_80014884 func_8001B838
#define func_800146CC func_8001B680
#define func_800164C0 func_8001D3CC
#define func_8001F078 func_80026314
#define func_8002A490 func_80031BA0
#define func_800379D8 func_8003F0C8
#define func_8003809C func_8003F78C
#define func_80043BA0 func_80007CC0
#define func_80048124 func_8000BFD8
#define func_80048B34 func_8000C9E8
#define func_8004D76C func_8004F538
#define func_8004E008 func_8004FDD4
#define func_8004E188 func_8004FF54
#define func_8004EA38 func_80050804
#define func_8004EE78 func_80050C44
#define func_8004F6E4 func_80051474
#define func_800648F0 func_80066660
#define func_80066B94 func_80068968
#define func_8006E880 func_80070820
#define func_8006F578 func_80071518
#define func_8006F804 func_800717A4
#define func_80070F1C func_8000E630
#define InitRaceResources   func_80030EA0
#define ResetRaceHeaps      func_80031128
#define InitRace            func_80031180
#else
#define ships               D_800AA0A4
#define shipShapes          D_8009BA68
#define cameras             D_800CBAF0
#define ownShip             D_800A5E2C
#define ownShip2            D_800A9F8C
#define ownShip3            D_800A9F90
#define ownShip4            D_800A9F94
#define gameData            D_800A9F00
#define tempGameData        D_800A1040
#define checkPoints         D_8009B9E8
#define rescueData          D_8009BD30
#define resetData           D_800AA068
#define pauseData           D_800A9F70
#define startDroidObjects   D_8009BC20
#define lightGroups         D_800A0EE8
#define prmObjects          D_800AB550
#define weaponGrid          D_8009BA14
#define noWeaponGrids       D_8008A994
#define noSpecialGrids      D_8008A99C
#define RaceCompleteDList   D_80096A48
#define PauseDList          D_80096A40
#define challengeMedalObject D_8008A9E0
#define CameraUpdate        D_800A1044
#define CameraUpdate2       D_800A5E18
#define CameraUpdate3       D_800A5E1C
#define CameraUpdate4       D_800A5E20
#define spriteTextures      D_800A5198
#define spriteTextureBase   D_800A5E24
#define hudSprites          D_800ACDF0
#define weaponShapes        D_800A9EB8
#define weaponData          D_8009BEDC
#define particleSlots       D_800AA0C4
#define WADInitScene        func_8000F7E0
#define WADLoadCompressedTextureSequence func_80010714
#define LoadCmpFiles        func_8000FB74
#define WADInitVertices     func_80015CA0
#define WADInitFaces        func_80016144
#define WADInitSections     func_80016340
#define WADInitViewList     func_80016590
#define InitCheckPoints     func_800183B0
#define InitWeaponGrids     func_800186BC
#define InitRaceResetData   func_80018AF0
#define InitRaceLine        func_80018B20
#define initShipData        func_80012760
#define InitCamera          func_8001AC10
#define UpdateCameraRaceStart func_8001B084
#define InitRescueDroid     func_80028944
#define LoadStartDroid      func_80046068
#define InitDroidThings     func_80046A18
#define InitParticle        func_800664A0
#define InitExplode         func_80066C10
#define InitRing            func_80067A7C
#define WADLoadPrm          func_8000A458
#define LoadShipShapes      func_80012374
#define InitRaceResources   func_80029790
#define ResetRaceHeaps      func_80029A18
#define InitRace            func_80029A70
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

extern s16 D_8008A998;
extern s32 D_80091C98;
extern s32 D_80091C9C;
extern s16 D_80091CAC;
extern s32 D_80091CC0;
extern s32 D_80091E04;
extern s32 D_80096994;
extern s32 D_80096998;
extern s32 D_8009699C;
extern s32 D_800969A0;
extern s32 D_8009BA0C;
extern u8 D_8009BA10;
extern s32 D_8009BCE0[NO_OF_DAMAGE_SLOTS];
extern u8 D_8009BE60[];
extern u8 D_800A1048[];
extern s16 D_800A10C0;
extern s16 D_800A5E30;
extern s16 D_800A5E46;
extern s32 D_800AA024;
extern s32 D_800AA084;
extern s32 D_800AA088;
extern u8 D_800AAF70[];
extern u8 D_800AB4F8[];
extern u8 D_800AB650[];
extern u8 D_800C44A8[];
extern u8 D_800C44D8[];
extern u8 D_800C44F0[];
extern u8 D_800C4568[];
extern u8 D_800C45C8[];
extern u8 D_800C45F8[];
extern s16 D_800C5910;
extern s16 D_800C5BA0;

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

void func_80008E5C(void);
void func_80014884(void);
void func_800146CC(s32 ship);
void func_800164C0(void);
void func_8001F078(ShipData *ships);
void func_8002A490(void *a, void *b);
void func_800379D8(void *weaponShapes, Skeleton *camPos, void *weapons, void *d);
void func_8003809C(void);
void func_80043BA0(void);
void func_80048124(void);
void func_80048B34(void *a, void *b);
void func_8004D76C(void);
void func_8004E008(void);
void func_8004E188(void);
void func_8004EA38(void);
void func_8004EE78(void);
void func_8004F6E4(void *anim, s32 start, s32 count);
void func_800648F0(void);
void func_80066B94(void);
void func_8006E880(void *reset, PauseData *pause, s32 mode);
void func_8006F578(void);
void func_8006F804(void);
void func_80070F1C(void);

void InitRaceResources(void);
void ResetRaceHeaps(void);
void InitRace(void);

#endif /* WIPEOUT64_RACE_H */
