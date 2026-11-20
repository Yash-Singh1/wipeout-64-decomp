#ifndef WIPEOUT64_EFFECTS_H
#define WIPEOUT64_EFFECTS_H

#include "common.h"

/*
 * Temporary US/EU symbol mappings until config/{us,eu}/symbol_addrs.txt
 * registers canonical names for effects.c functions and external symbols.
 */
#ifdef VERSION_EU
#define SetDisplayListPrimColour        func_80007C30
#define LoadEffectTextures              func_80007CC0
#define DrawEffects                     func_80007DE0
#define DrawMedalEffects                func_80008864
#define UpdateEffectAnims               func_80008974
#define DrawTrailEffects                func_800089F8
#define InitPlasmaWall                  func_80008CAC
#define DrawPlasmaWall                  func_80008FB0
#define ResetStartBoom                  func_800094DC
#define ColourDroidLights               func_80009568
#define UpdateStartDroid                func_800098A0
#define LoadStartDroid                  func_80009F1C
#define LoadShadowTextures              func_8000BFD8
#define DrawShadow                      func_8000C03C
#define InitWeaponIcons                 func_8000C9E8
#define IsSuperComboWeapon              func_8000CF0C
#define DrawWeaponIcons                 func_8000CF48
#define SetShake                        func_8000D5D4
#define SetRumble                       func_8000D5EC
#define ShakeScreen                     func_8000D61C
#define UpdateRumble                    func_8000D728

#define InitDroidThings                 func_8000A8CC
#define ResetLightStart                 func_8000AB84
#define UpdateLightStart                func_8000ACFC
#define ratan2                          func_8001BBE0
#define PlaneLineCollision              func_8002E2A4
#define DrawFinishMedal                 func_80051174
#define UpdateInGamePlasma              func_80053394
#define AddEboltParticles               func_80068530
#define DrawShipTrails                  func_8000F604
#define UpdateTarget                    func_8000E0D0
#define DrawTarget                      func_8000E1B4
#define Set3PlayerViewport              func_8004DA04
#define UpdateMissile                   func_800401C8
#define UpdateRocket                    func_800498B4
#define UpdateElecBolt                  func_80044B20
#define DrawExplode                     func_80068A58
#define DrawSmoke                       func_80068E18
#define DrawExplosions                  func_80069D00
#define AddSmallExplosion               func_8006A20C
#define AddLargeExplosion               func_8006A274
#define AddExplosionFlash               func_8006A438
#define DrawMissileTrail                func_8006A59C
#define DrawRocketTrail                 func_8006AC50
#define DrawSuperTrail                  func_8006B30C
#define AddShieldSparks                 func_8006BB34
#define DrawShieldSparks                func_8006BDB4
#define DrawParticleSystem              func_8006C308
#define DrawTargetOverlay               func_8006C78C
#define DrawParticles                   func_800691E0

#define g_glistp                        D_800B842C
#define g_dlistUsed                     D_800B8438
#define g_dlistPct                      D_800B843C
#define g_mtxPool                       D_800AE4C0
#define g_mtxIndex                      D_800AE514
#define g_gameMode                      D_800B7604
#define g_demoMode                      D_800B7660
#define g_playerFinished                D_800A8FF8
#define particleSlots                   D_800B7698
#define g_superWeaponActive1            D_800914A8
#define g_inGamePlasmaTexture           D_800C49A8
#define g_inGamePlasmaPalette           D_800C51A8
#define g_medalRenderList               D_800C47E8
#define g_trailRenderList               D_800C4830
#define g_lightRenderList               D_800C4878
#define g_sroidRenderList               D_800C4890
#define g_effectsRenderList             D_800C48A8
#define shipShapes                      D_800A9048
#define targetPos                       D_8009ED60
#define targetActive                    D_8009EEE0
#define targetVtx                       D_800A4CC8
#define g_plasmaWall                    D_80097CB0
#define g_multiplayerFlag               D_80095F44
#define numTrackSections                D_800AE6A2
#define classLaps                       D_80091410
#define droidSideTable                  D_8008952E
#define droidBlinkTick                  D_8008955C
#define lightUpdateTick                 D_80089560
#define lightBlinkPhase                 D_80089564
#define lightAnimFrame                  D_80089568
#define lightGoFlag                     D_8008956C
#define lightGoTimer                    D_80089570
#define lightTex0                       D_80089544
#define lightTex1                       D_80089548
#define lightTex2                       D_8008954C
#define lightTex3                       D_80089550
#define startDroidState                 D_80098E70
#define startCountdown                  D_80091140
#define cheatSuperCombo                 D_80091258
#define g_superComboUnlocked            D_8009452E
#define g_superComboOption              D_800C5EBE
#define pauseData                       D_800B7550
#define goSpriteTimer                   D_80089574
#define superWeaponBlinkAlpha           D_80089578
#define weaponIconsMP                   D_800B8808
#define weaponBackingSprite             D_800B8C98
#define startLightSprites               D_800B8CB0
#define weaponTexturesMP                D_800B8D10

#define lightUpdate                     D_800912D4
#define lightCount                      D_8008953E
#define lightState                      D_80098F10
#define startDroidSection               D_80097E38
#define droidStartSections              D_80098E40
#define lightSections                   D_80098E60
#define startLinePos                    D_8008A478
#define lightGroups                     D_800AE4C8

#define LoadCmpFiles                    func_80016994
#define WADLoadPrm                      func_80011278
#define NewInstance                     func_800116A4
#define LoadCompressedTextureSequence   func_800172FC
#define WADLoadCompressedTextureSequence func_80017534
#define newRand                         func_80023FCC
#define StartRumble                     func_8004B988
#define StopRumble                      func_8004B9EC

#define startBoomCount                  D_80089FB8
#define droidLightPhase                 D_80089558
#define missileLockTimer                D_800914BC
#define shakeAmount                     D_8008957C
#define rumbleTime                      D_8008958C
#define rumbleActive                    D_8008959C
#define g_effectTexScroll0              D_80095FA4
#define g_effectTexScroll1              D_80095FAC
#define g_effectTexScroll2              D_80095FB8
#define targetSpin                      D_800895B0
#define g_targetFlash                   D_800895B4
#define targetBank                      D_800895B8
#define cameras                         D_800CBE30
#define startBoomObjs                   D_800B7688
#define effectTex0Start                 D_80097C90
#define effectTex0Count                 D_80097C94
#define effectTex1Start                 D_80097C98
#define effectTex1Count                 D_80097C9C
#define effectTex2Start                 D_80097CA0
#define effectTex2Count                 D_80097CA4
#define effectTex3Start                 D_80097CA8
#define effectTex3Count                 D_80097CAC
#define shadowTextureStart              D_80097E3C
#define shadowVtx                       D_80097E40
#define shadow2TextureStart             D_80098E5C
#define ownShip                         D_800B3414
#define ownShip2                        D_800B756C
#define ownShip3                        D_800B7570
#define ownShip4                        D_800B7574
#else
#define SetDisplayListPrimColour        func_80043B10
#define LoadEffectTextures              func_80043BA0
#define DrawEffects                     func_80043CC0
#define DrawEffects2                    func_800443F4
#define DrawMedalEffects                func_800449C4
#define UpdateEffectAnims               func_80044AD4
#define DrawTrailEffects                func_80044B58
#define InitPlasmaWall                  func_80044DF8
#define DrawPlasmaWall                  func_800450FC
#define ResetStartBoom                  func_80045628
#define ColourDroidLights               func_800456B4
#define UpdateStartDroid                func_800459EC
#define LoadStartDroid                  func_80046068
#define LoadShadowTextures              func_80048124
#define DrawShadow                      func_80048188
#define InitWeaponIcons                 func_80048B34
#define IsSuperComboWeapon              func_80049058
#define DrawWeaponIcons                 func_80049094
#define SetShake                        func_80049720
#define SetRumble                       func_80049738
#define ShakeScreen                     func_80049760
#define UpdateRumble                    func_8004986C

#define InitDroidThings                 func_80046A18
#define ResetLightStart                 func_80046CD0
#define UpdateLightStart                func_80046E48
#define ratan2                          func_80014C30
#define PlaneLineCollision              func_80026C00
#define DrawFinishMedal                 func_8004F3A8
#define UpdateInGamePlasma              func_80051604
#define AddEboltParticles               func_8006675C
#define DrawShipTrails                  func_80071E9C
#define UpdateTarget                    func_800709B0
#define DrawTarget                      func_80070AB8
#define Set3PlayerViewport              func_8004BB10
#define UpdateMissile                   func_80038AD8
#define UpdateRocket                    func_80041B90
#define UpdateElecBolt                  func_8003CF94
#define DrawExplode                     func_80066C84
#define DrawSmoke                       func_80067044
#define DrawExplosions                  func_80067F2C
#define AddSmallExplosion               func_800683D8
#define AddLargeExplosion               func_80068440
#define AddExplosionFlash               func_80068604
#define DrawMissileTrail                func_80068768
#define DrawRocketTrail                 func_80068E1C
#define DrawSuperTrail                  func_800694D8
#define AddShieldSparks                 func_80069D00
#define DrawShieldSparks                func_80069F60
#define DrawParticleSystem              func_8006A3E4
#define DrawTargetOverlay               func_8006A748
#define DrawParticles                   func_8006740C

#define g_glistp                        D_800AAE5C
#define g_dlistUsed                     D_800AAE68
#define g_dlistPct                      D_800AAE6C
#define g_mtxPool                       D_800A0EE0
#define g_mtxIndex                      D_800A0F34
#define g_gameMode                      D_800AA024
#define g_demoMode                      D_800AA088
#define g_playerFinished                D_8009BA18
#define particleSlots                   D_800AA0C4
#define g_superWeaponActive1            D_80091E78
#define g_inGamePlasmaTexture           D_800C4668
#define g_inGamePlasmaPalette           D_800C4E68
#define g_medalRenderList               D_800C44A8
#define g_trailRenderList               D_800C44F0
#define g_lightRenderList               D_800C4538
#define g_sroidRenderList               D_800C4550
#define g_effectsRenderList             D_800C4568
#define g_effects2RenderList            D_800C45C8
#define shipShapes                      D_8009BA68
#define targetPos                       D_800F46C0
#define targetActive                    D_800F4840
#define targetVtx                       D_800F8868
#define g_plasmaWall                    D_800AD7C0
#define g_multiplayerFlag               D_80096994
#define numTrackSections                D_800A10C2
#define classLaps                       D_80091DE0
#define droidSideTable                  D_80091FEE
#define droidBlinkTick                  D_8009201C
#define lightUpdateTick                 D_80092020
#define lightBlinkPhase                 D_80092024
#define lightAnimFrame                  D_80092028
#define lightGoFlag                     D_8009202C
#define lightGoTimer                    D_80092030
#define lightTex0                       D_80092004
#define lightTex1                       D_80092008
#define lightTex2                       D_8009200C
#define lightTex3                       D_80092010
#define startDroidState                 D_800AE980
#define startCountdown                  D_80091B10
#define cheatSuperCombo                 D_80091C28
#define g_superComboUnlocked            D_80094F7E
#define g_superComboOption              D_800C5B7E
#define pauseData                       D_800A9F70
#define goSpriteTimer                   D_80092034
#define superWeaponBlinkAlpha           D_80092038
#define weaponIconsMP                   D_800AB238
#define weaponBackingSprite             D_800AB6C8
#define startLightSprites               D_800AB6E0
#define weaponTexturesMP                D_800AB740

#define lightUpdate                     D_80091CA4
#define lightCount                      D_80091FFE
#define lightState                      D_800BAA20
#define startDroidSection               D_800AD948
#define droidStartSections              D_800AE950
#define lightSections                   D_800AE970
#define startLinePos                    D_8008AE48
#define lightGroups                     D_800A0EE8

#define LoadCmpFiles                    func_8000FB74
#define WADLoadPrm                      func_8000A458
#define NewInstance                     func_8000A884
#define LoadCompressedTextureSequence   func_800104DC
#define WADLoadCompressedTextureSequence func_80010714
#define newRand                         func_8001CFAC
#define StartRumble                     func_80049AA8
#define StopRumble                      func_80049B0C

#define startBoomCount                  D_8008A988
#define droidLightPhase                 D_80092018
#define missileLockTimer                D_80091E8C
#define shakeAmount                     D_8009203C
#define rumbleTime                      D_8009204C
#define rumbleActive                    D_8009205C
#define g_effectTexScroll0              D_800969EC
#define g_effectTexScroll1              D_800969F4
#define g_effectTexScroll2              D_80096A00
#define targetSpin                      D_80097320
#define g_targetFlash                   D_80097324
#define targetBank                      D_80097328
#define cameras                         D_800CBAF0
#define startBoomObjs                   D_800AA0B0
#define effectTex0Start                 D_800AD7A0
#define effectTex0Count                 D_800AD7A4
#define effectTex1Start                 D_800AD7A8
#define effectTex1Count                 D_800AD7AC
#define effectTex2Start                 D_800AD7B0
#define effectTex2Count                 D_800AD7B4
#define effectTex3Start                 D_800AD7B8
#define effectTex3Count                 D_800AD7BC
#define shadowTextureStart              D_800AD94C
#define shadowVtx                       D_800AD950
#define shadow2TextureStart             D_800AE96C
#define ownShip                         D_800A5E2C
#define ownShip2                        D_800A9F8C
#define ownShip3                        D_800A9F90
#define ownShip4                        D_800A9F94
#endif

#define TextureTableCount               g_unk8008A970
#define raceLines                       g_unkAlloc9600
#define _effectTexturesFile             D_375E50
#define _shadowTexturesFile             D_3E59A0
#define _shadow2TexturesFile            D_3E5DC0
#define g_vpFullScreen                  D_1000000

#define FrontendOptions FrontendOptionsHead
#define frontendOptions_g frontendOptionsHead_g
#include "radscn.h"
#undef FrontendOptions
#undef frontendOptions_g

#include "n64render.h"
#include "wadfile.h"
#include <ultra64.h>

#define MAX_PLAYERS         4
#define NO_OF_SHIPS         15
#define NO_OF_WEAPONS       52
#define NO_OF_CHECKPOINTS   6
#define LIGHT_GROUP_OBJECTS 6
#define LIGHT_GROUP_SKELETON 3
#define MAX_LIGHT_GROUPS    3
#define START_BOOM_POLYS    3
#define NO_OF_WEAPON_ICONS  29
#define TARGET_BANKS        8

#ifdef VERSION_EU
#define TARGET_VTX_COUNT    8
#else
#define TARGET_VTX_COUNT    5
#endif

#define G_CC_DECALRGB_PRIMA 0, 0, 0, TEXEL0, TEXEL0, 0, PRIMITIVE, 0

typedef struct FrontendOptions {
    /* 0x00 */ s32 team;
    /* 0x04 */ u8 teamClass;
    /* 0x08 */ u32 trackFile;
    /* 0x0C */ s32 trackNo;
    /* 0x10 */ u8 numPlayers;
    /* 0x11 */ u8 unk11[3];
    /* 0x14 */ u8 unk14;
    /* 0x15 */ u8 challengeMedal;
    /* 0x16 */ u8 challengeSub;
    /* 0x18 */ s32 ownShips[MAX_PLAYERS];
    /* 0x28 */ s32 splitMode;
    /* 0x2C */ u8 unk2C[0x0C];
    /* 0x38 */ s32 controller[MAX_PLAYERS];
    /* 0x48 */ u8 unk48[0x13];
    /* 0x5B */ u8 unk5B;
} FrontendOptions;

typedef struct SVector {
    /* 0x00 */ s16 vx;
    /* 0x02 */ s16 vy;
    /* 0x04 */ s16 vz;
} SVector;

typedef struct Face {
    /* 0x00 */ s16 vertex[4];
    /* 0x08 */ SVector normal;
    /* 0x0E */ u8 texture;
    /* 0x0F */ u8 flags;
    /* 0x10 */ u8 red;
    /* 0x11 */ u8 green;
    /* 0x12 */ u8 blue;
    /* 0x13 */ u8 pad;
} Face; /* size = 0x14 */

typedef struct TrackCamera {
    /* 0x00 */ Skeleton *camPos;
    /* 0x04 */ TrackSection *section;
    /* 0x08 */ s16 hdg, pitch, roll;
    /* 0x0E */ s16 vhdg, vpitch, vroll;
    /* 0x14 */ s16 vel[4];
} TrackCamera; /* size = 0x1C */

typedef struct {
    s32 vx;
    s32 vy;
    s32 vz;
    s32 pad;
} VECTOR;

typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} SVECTOR;

typedef struct Quat {
    /* 0x00 */ s32 w;
    /* 0x04 */ s32 x;
    /* 0x08 */ s32 y;
    /* 0x0C */ s32 z;
} Quat;

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
    /* 0x0B2 */ u8 unkB2[0x0C];
    /* 0x0BE */ u8 weaponType;
    /* 0x0BF */ u8 unkBF[2];
    /* 0x0C1 */ s8 targetShip;
    /* 0x0C2 */ u8 unkC2[8];
    /* 0x0CA */ s8 lapNo;
    /* 0x0CB */ u8 unkCB;
    /* 0x0CC */ s16 nearSecNo;
    /* 0x0CE */ u8 unkCE[0x26];
    /* 0x0F4 */ u8 active;
    /* 0x0F5 */ u8 unkF5[0x09];
    /* 0x0FE */ u8 visible[MAX_PLAYERS];
    /* 0x102 */ u8 unk102[0x06];
} ShipData; /* size = 0x108 */

typedef struct WeaponData {
    /* 0x00 */ s16 count;
    /* 0x02 */ s16 bumpStep;
    /* 0x04 */ s16 shapeNo;
    /* 0x06 */ s16 pad06;
    /* 0x08 */ TrackSection *section;
    /* 0x0C */ s32 accelX;
    /* 0x10 */ s32 accelY;
    /* 0x14 */ s32 accelZ;
    /* 0x18 */ s32 faceIndex;
    /* 0x1C */ s32 velX;
    /* 0x20 */ s32 velY;
    /* 0x24 */ s32 velZ;
    /* 0x28 */ s32 sideFlag;
    /* 0x2C */ VECTOR pos;
    /* 0x3C */ s16 hdg;
    /* 0x3E */ s16 pitch;
    /* 0x40 */ s8 target;
    /* 0x41 */ u8 owner;
    /* 0x42 */ u8 type;
    /* 0x43 */ u8 pad43;
    /* 0x44 */ void *update;
} WeaponData; /* size = 0x48 */

typedef struct PlasmaWall {
    /* 0x000 */ VECTOR corners[4];
    /* 0x040 */ VECTOR dir;
    /* 0x050 */ VECTOR shipPos;
    /* 0x060 */ s32 timer;
    /* 0x064 */ TrackSection *section;
    /* 0x068 */ s32 pad68[2];
    /* 0x070 */ Vtx vtx[MAX_PLAYERS][4];
    /* 0x170 */ WeaponData *weapon;
} PlasmaWall; /* size = 0x174 */

typedef struct ConfigData {
    /* 0x00 */ s32 initMagic;
    /* 0x04 */ u8 trackNum;
    /* 0x05 */ u8 actTrackNum;
    /* 0x06 */ s8 shipNum;
    /* 0x07 */ s8 ship2Num;
    /* 0x08 */ s8 shipClass;
} ConfigData;

typedef struct CheckPoint {
    /* 0x00 */ s16 section;
    /* 0x02 */ s16 bonusSeconds;
    /* 0x04 */ s8 done;
} CheckPoint;

typedef struct CVECTOR {
    /* 0x00 */ u8 r, g, b, cd;
} CVECTOR;

typedef struct Primitive {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
} Primitive;

typedef struct FT4 {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
    /* 0x04 */ s16 coords[4];
    /* 0x0C */ s16 texture;
    /* 0x0E */ s16 clut;
    /* 0x10 */ s16 tpage;
    /* 0x12 */ u8 u0, v0, u1, v1, u2, v2, u3, v3;
    /* 0x1A */ s16 pad1;
    /* 0x1C */ CVECTOR colour;
} FT4; /* size = 0x20 */

typedef struct GT3 {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
    /* 0x04 */ s16 coords[3];
    /* 0x0A */ s16 texture;
    /* 0x0C */ s16 clut;
    /* 0x0E */ s16 tpage;
    /* 0x10 */ u8 u0, v0, u1, v1, u2, v2;
    /* 0x16 */ s16 pad1;
    /* 0x18 */ CVECTOR colour[3];
} GT3; /* size = 0x24 */

typedef struct GT4 {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
    /* 0x04 */ s16 coords[4];
    /* 0x0C */ s16 texture;
    /* 0x0E */ s16 clut;
    /* 0x10 */ s16 tpage;
    /* 0x12 */ u8 u0, v0, u1, v1, u2, v2, u3, v3;
    /* 0x1A */ s16 pad1;
    /* 0x1C */ CVECTOR colour[4];
} GT4; /* size = 0x2C */

typedef union Poly {
    Primitive *prim;
    FT4 *ft4;
    GT3 *gt3;
    GT4 *gt4;
} Poly;

#define TypeFT4 4
#define TypeGT3 6
#define TypeGT4 8

extern u8 _effectTexturesFile[];
extern u8 _shadowTexturesFile[];
extern u8 _shadow2TexturesFile[];
extern Vp g_vpFullScreen[];

extern Gfx *g_glistp;
extern s32 g_dlistUsed;
extern s32 g_dlistPct;
extern N64Mtx *g_mtxPool;
extern s32 g_mtxIndex;
extern s32 g_gameMode;
extern s32 g_demoMode;
extern u8 g_playerFinished[MAX_PLAYERS];
extern void *particleSlots;
extern s32 g_superWeaponActive1;
extern u8 g_inGamePlasmaTexture[];
extern u16 g_inGamePlasmaPalette[];
typedef struct PauseData {
    /* 0x00 */ s32 selection;
    /* 0x04 */ s32 menu;
} PauseData;

typedef struct RaceLine {
    /* 0x00 */ u16 distance;
    /* 0x02 */ u8 unk02[0x0E];
} RaceLine; /* size = 0x10 */

typedef struct StartDroidState {
    /* 0x00 */ s32 signType[2];
    /* 0x08 */ Vector pos[2];
    /* 0x20 */ s32 angle[2];
} StartDroidState; /* size = 0x28 */

extern RenderList g_medalRenderList;
extern RenderList g_trailRenderList;
extern RenderList g_lightRenderList;
extern RenderList g_sroidRenderList;
extern RenderList g_effectsRenderList;
#ifndef VERSION_EU
extern RenderList g_effects2RenderList;
#endif
extern Object *shipShapes[];
extern Vector targetPos[MAX_PLAYERS][TARGET_BANKS];
extern u8 targetActive[MAX_PLAYERS][TARGET_BANKS];
extern Vtx targetVtx[MAX_PLAYERS][TARGET_BANKS][TARGET_VTX_COUNT];
extern PlasmaWall g_plasmaWall;
extern s32 g_multiplayerFlag;
extern s16 numTrackSections;
extern u8 classLaps[];
extern s16 droidSideTable[];
extern s32 droidBlinkTick;
extern s32 lightUpdateTick;
extern s32 lightBlinkPhase;
extern s32 lightAnimFrame;
extern s32 lightGoFlag;
extern s32 lightGoTimer;
extern s32 lightTex0;
extern s32 lightTex1;
extern s32 lightTex2;
extern s32 lightTex3;
extern StartDroidState startDroidState[MAX_PLAYERS];
extern RaceLine *raceLines;
extern WADFILE *g_startupImage;
extern WADFILE *g_startupData;
extern s32 startCountdown;
extern u8 cheatSuperCombo;
extern u8 g_superComboUnlocked;
extern u8 g_superComboOption;
extern PauseData pauseData;
extern s32 goSpriteTimer;
extern s32 superWeaponBlinkAlpha;
extern uSprite weaponIconsMP[NO_OF_WEAPON_ICONS];
extern uSprite weaponBackingSprite;
extern uSprite startLightSprites[4];
extern N64Texture *weaponTexturesMP[NO_OF_WEAPON_ICONS];

extern FrontendOptions frontendOptions_g;
extern s16 TextureTableCount;
extern TrackCamera cameras[MAX_PLAYERS];
extern s16 startBoomCount;
extern Object *startBoomObjs[];
extern s16 missileLockTimer;
extern s32 lightUpdate;
extern s16 lightCount;
extern s32 lightState[MAX_LIGHT_GROUPS];
extern TrackSection *startDroidSection;
extern TrackSection *droidStartSections[NO_OF_CHECKPOINTS + 1];
extern TrackSection *lightSections[MAX_LIGHT_GROUPS];
extern s16 startLinePos[];
extern Object *lightGroups[MAX_LIGHT_GROUPS][LIGHT_GROUP_OBJECTS];
extern s16 droidLightPhase;
extern s32 shakeAmount[MAX_PLAYERS];
extern s32 rumbleTime[MAX_PLAYERS];
extern s32 rumbleActive[MAX_PLAYERS];
extern s32 g_effectTexScroll0;
extern s32 g_effectTexScroll1;
extern s32 g_effectTexScroll2;
extern s32 targetSpin;
extern u8 g_targetFlash;
extern s32 targetBank;
extern s32 effectTex0Start, effectTex0Count;
extern s32 effectTex1Start, effectTex1Count;
extern s32 effectTex2Start, effectTex2Count;
extern s32 effectTex3Start, effectTex3Count;
extern s32 shadowTextureStart;
extern Vtx shadowVtx[MAX_PLAYERS][16][4];
extern s32 shadow2TextureStart;
extern s32 ownShip;
extern s32 ownShip2;
extern s32 ownShip3;
extern s32 ownShip4;

s16 rsin(s16 angle);
void *memcpy(void *dst, const void *src, size_t len);
void *LoadCompressedTextureSequence(u32 fileOffset);
void *WADLoadCompressedTextureSequence(WADFILE *wad);
void LoadCmpFiles(void *timList);
Object *WADLoadPrm(WADFILE *wad, RenderList *renderList, WADFILE *texWad);
Object *NewInstance(Object *object);
s32 newRand(void);
s32 ratan2(s32 y, s32 x);
s32 PlaneLineCollision(VECTOR point, VECTOR linePoint, SVector normal);
void StartRumble(s32 channel);
void StopRumble(s32 channel);
void DrawFinishMedal(TrackCamera *camera, void *projMtx);
void UpdateInGamePlasma(s32 mode);
void AddEboltParticles(void *slots, s32 count, VECTOR *pos);
void DrawShipTrails(void *projMtx, void *perspNorm, s32 player);
void UpdateTarget(s32 x, s32 y, s32 z, s32 target, s32 player);
void DrawTarget(VECTOR *camPos, MATRIX *mat, s32 ship, s32 player, s32 bank);
Gfx *Set3PlayerViewport(s32 player, Mtx *projMtx, u16 *perspNorm, Gfx *glistp);
void AxisAngleToQuat(Quat *q, s32 angle, SVECTOR *axis);
void QuatToMatrix(Quat *q, MATRIX *m);
void UpdateMissile(void);
void UpdateRocket(void);
void UpdateElecBolt(void);
void DrawExplode(VECTOR *camPos, MATRIX *mat, s32 player);
void DrawSmoke(VECTOR *camPos, MATRIX *mat, s32 player);
void DrawExplosions(VECTOR *camPos, MATRIX *mat, s32 player);
void AddSmallExplosion(s32 x, s32 y, s32 z, s32 arg3);
void AddLargeExplosion(s32 x, s32 y, s32 z, s32 arg3);
void AddExplosionFlash(s32 x, s32 y, s32 z, s32 size, CVECTOR *colour);
void DrawMissileTrail(VECTOR *pos, VECTOR *camPos, MATRIX *mat, MATRIX *rot0, MATRIX *rot1, s32 player);
void DrawRocketTrail(VECTOR *pos, VECTOR *camPos, MATRIX *mat, MATRIX *rot0, MATRIX *rot1, s32 player);
void DrawSuperTrail(VECTOR *pos, VECTOR *camPos, MATRIX *mat, MATRIX *rot0, MATRIX *rot1, s32 player, s32 variant);
void AddShieldSparks(VECTOR *pos, s32 mode);
void DrawShieldSparks(VECTOR *camPos, s32 player);
void DrawParticleSystem(VECTOR *camPos, void *slots, s32 player);
void DrawTargetOverlay(void *projMtx, void *perspNorm, s32 player);
void DrawParticles(void *projMtx, void *perspNorm, s32 player);

void SetDisplayListPrimColour(Gfx *gfx, s32 r, s32 g, s32 b, s32 a, s32 count);
void LoadEffectTextures(void);
Gfx *DrawEffects(WeaponData *weapons, ShipData *ships, s32 player, Mtx *projMtx, u16 *perspNorm, Gfx *glistp);
#ifndef VERSION_EU
Gfx *DrawEffects2(WeaponData *weapons, ShipData *ships, s32 player, Mtx *projMtx, u16 *perspNorm, Gfx *glistp);
#endif
Gfx *DrawMedalEffects(void *projMtx, void *perspNorm, Gfx *glistp);
void UpdateEffectAnims(void);
void DrawTrailEffects(void *projMtx, void *perspNorm, s32 player);
void InitPlasmaWall(WeaponData *weapon, ShipData *ship);
void DrawPlasmaWall(VECTOR *camPos, s32 player);
void ResetStartBoom(void);
void ColourDroidLights(Object *droid);
void UpdateStartDroid(Object **shapes, ShipData *ships, CheckPoint *checkPoints, s16 textureBase,
                      ConfigData *game, s32 pause, s32 player);
void LoadStartDroid(Object **objects, char *name, Skeleton *parent);
void InitDroidThings(Object **objects, char *name, Skeleton *parent, CheckPoint *points, ConfigData *options);
void ResetLightStart(void);
void UpdateLightStart(Object **objects, s32 unused1, s32 unused2, s32 unused3, s32 unused4, s32 mode);
void LoadShadowTextures(void);
void DrawShadow(VECTOR *camPos, Object **shipShapes, ShipData *ships, TrackCamera *camera, s32 player);
void InitWeaponIcons(uSprite *weaponIcons, N64Texture **weaponTextures);
s32 IsSuperComboWeapon(s32 weaponType);
Gfx *DrawWeaponIcons(uSprite *weaponIcons, void *unused, ShipData *playerShip, s32 viewportMode, Gfx *glistp);
void SetShake(s32 amount, s32 player);
void SetRumble(s32 time, s32 player);
void ShakeScreen(s32 player);
void UpdateRumble(s32 player, u8 stop);

#endif /* WIPEOUT64_EFFECTS_H */
