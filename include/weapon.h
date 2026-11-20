#ifndef WIPEOUT64_WEAPON_H
#define WIPEOUT64_WEAPON_H

#include "common.h"
#include "dheap.h"
#include "skeleton.h"
#include "wadfile.h"
#include <ultra64.h>

/*
 * Temporary US/EU symbol mappings until config/{us,eu}/symbol_addrs.txt
 * registers canonical names for weapon.c functions and external symbols.
 */
/* BEGIN ALIASES */
#ifdef VERSION_EU
#define UpdateWeapons                    func_8003DFD0
#define InsertWeapon                     func_8003E130
#define FireWeapon                       func_8003E9A8
#define FireSuperWeapon                  func_8003EB04
#define MissileLock                      func_8003EC2C
#define RearMissileLock                  func_8003EDA4
#define InitWeapons                      func_8003F0C8
#define BumpUp                           func_8003F3AC
#define BumpDown                         func_8003F4CC
#define BumpFlatten                      func_8003F5E8
#define FlattenTrack                     func_8003F720
#define InitialiseBumps                  func_8003F78C
#define UpdateQuake                      func_8003FA2C
#define HitReboundMissile                func_8003FFD0
#define UpdateReboundDelay               func_80040164
#define UpdateMissile                    func_800401C8
#define InitMissile                      func_80040E24
#define UpdateMinigunBullet              func_80041210
#define UpdateMinigun                    func_80041A94
#define InitMinigun                      func_80041B68
#define InitMinigunBullet                func_80041C6C
#define UpdateMine                       func_800420C8
#define InitQuake                        func_80042660
#define InitPlasma                       func_8004294C
#define UpdatePlasma                     func_80042C40
#define InitRocket                       func_80043618
#define SetDebrisPrimColour              func_8004393C
#define UpdateShipDebris                 func_800439BC
#define InitShipDebris                   func_80043BF4
#define InitRocketLeft                   func_80044244
#define InitRocketRight                  func_800446AC
#define UpdateElecBolt                   func_80044B20
#define InitElecBolt                     func_800454E8
#define InitMine                         func_80045820
#define InitTrackTrap                    func_80045998
#define UpdateTrackTrap                  func_80045B24
#define InitSmartBomb                    func_80045F6C
#define KillMines                        func_8004610C
#define UpdateSmartBomb                  func_800461FC
#define InitShield                       func_80046824
#define UpdateShield                     func_800469AC
#define InitCloak                        func_80047A90
#define UpdateCloak                      func_80047B08
#define CycleWeaponGrids                 func_80047F64
#define GetFireVector                    func_80048334
#define Weapon2Mine                      func_800484FC
#define GetShipTeam                      func_8004861C
#define GetNewWeapon                     func_800486AC
#define GetNewDroneWeapon                func_80049254
#define UpdateRocket                     func_800498B4
#define GetWeaponDamage                  func_8004A370
#define ScaleChallengeDamage             func_8004A44C
#define DamageCollide                    func_8004A488
#define CheckPitShips                    func_8004AF8C
#define UpdateFadeScreen                 func_8004B25C
#define DrawScreenOverlay                func_8004B430
#define FlipShip                         func_8004B678
#define GetCheatWeapon                   func_8004B72C
#define randWep                          func_8004B80C
#define WADLoadPrm                       func_80011278
#define NewObjectInstance                func_800116A4
#define LoadCmpFiles                     func_80016994
#define WADLoadCompressedTextureSequence func_80017534
#define ratan2                           func_8001BBE0
#define PlayNote                         func_8001BCE8
#define GetFaceVertex                    func_8001F06C
#define UpdateCameraAttractDestroyed     func_8002291C
#define FindNearestSection               func_80023A98
#define GetMagnitude                     func_80023EE4
#define sar                              func_80023F70
#define ang                              func_80023F8C
#define newRand                          func_80023FCC
#define GetDroneShield                   func_80027FE8
#define UpdateDroneShip                  func_80028220
#define getShipInfrontLAP                func_8002AA2C
#define getShipBehindLAP                 func_8002AB18
#define humanToDroneSpeed                func_8002AC0C
#define PlaneLineCollision               func_8002E2A4
#define GetShipPosition                  func_80038108
#define InitTrackTrapFace                func_80008CAC
#define SetShake                         func_8000D5D4
#define SetRumble                        func_8000D5EC
#define ResetRenderList                  func_800513F0
#define InitRenderList                   func_80051474
#define FlushRenderList                  func_80051520
#define AddRenderGroup                   func_800517C0
#define AddRandomParticles               func_800682C0
#define AddExplode                       func_8006A20C
#define AddCloakHitEffect                func_8006A274
#define AddCycloneFlash                  func_8006A438
#define AddCycloneTrail                  func_8006A498
#define AddPitSpark                      func_8006BB34
#define SetSfxVolume                     func_80071FE8
#define StopSfx                          func_8007200C
#define PlaySfx                          func_80072048
#define SetSfxFreqOffset                 func_8007206C

#define g_demoShip                       D_80089FA4
#define g_multiPlayer                    D_8008A000
#define cheatfade                        D_80091252
#define cheatWeapon                      D_80091254
#define cheatDamage                      D_80091256
#define cheatSuperCombo                  D_80091258
#define superWeaponFlag                  D_800914A4
#define g_superWeaponActive1             D_800914A8
#define g_superWeaponActive2             D_800914AC
#define rearLockCycle                    D_800914B0
#define missileLockTimer                 D_800914BC
#define seedWep                          D_800914C0
#define fadeAmount                       D_800914C4
#define g_weaponDamageTable              D_800914D0
#define g_pitHealTable                   D_80091510
#define g_pitHealScale                   D_80091520
#define shieldActive                     D_80091530
#define weaponInserted                   D_80091534
#define minigunFrame                     D_80091538
#define g_debrisLife                     D_8009153E
#define g_debrisSpeed                    D_80091540
#define g_debrisGravity                  D_80091544
#define g_weaponChance1P                 D_80091548
#define g_weaponChanceMP                 D_80091568
#define g_droneWeaponChance0             D_80091588
#define g_droneWeaponChance1             D_800915A8
#define g_droneWeaponChance2             D_800915C8
#define g_droneWeaponChance3             D_800915E8
#define previousPlayerDamage             D_80091608
#define g_pitFade                        D_80091944
#define g_unk800923F9                    D_800919A9
#define g_superComboUnlocked             D_8009452E
#define nextParticleSlot                 D_80095FC0
#define shieldSfx                        D_800968A8
#define g_pitSfxHandle                   D_800968AC
#define g_engineHandle                   D_800968B4
#define g_turbHandle                     D_800968B8
#define g_chargeSfxHandle                D_800968BC
#define shipDamage                       D_800A92C0
#define g_mtxPool                        D_800AE4C0
#define g_mtxIndex                       D_800AE514
#define CameraUpdate                     D_800AE624
#define shipClassIndex                   D_800AE6A0
#define numTrackSections                 D_800AE6A2
#define CameraUpdate2                    D_800B33F8
#define CameraUpdate3                    D_800B33FC
#define CameraUpdate4                    D_800B3400
#define ownShip                          D_800B3414
#define shipDeathCount                   D_800B74A0
#define gameData                         D_800B74E0
#define g_raceClass                      D_800B74E8
#define pauseData                        D_800B7550
#define ownShip2                         D_800B756C
#define ownShip3                         D_800B7570
#define ownShip4                         D_800B7574
#define pauseRequests                    D_800B75F0
#define attractMode                      D_800B7600
#define gameType                         D_800B7604
#define g_attractMode                    D_800B7660
#define ships                            D_800B767C
#define playerPosition                   D_800B7682
#define particleSlots                    D_800B7698
#define player2Position                  D_800B76AA
#define player3Position                  D_800B76AC
#define player4Position                  D_800B76B4
#define g_lastRammingShip                D_800B8490
#define g_fireLock                       D_800B84A4
#define g_weaponCharge                   D_800B84A8
#define fadeActive                       D_800BACC0
#define fadeBlue                         D_800BACC8
#define shieldRed                        D_800BACD0
#define fadeGreen                        D_800BAD10
#define fadeRed                          D_800BAD18
#define shieldGreen                      D_800BAD20
#define reboundTarget                    D_800BAD5C
#define D_800AD78F                       D_800BAD5F
#define g_fadeColour                     D_800BAD60
#define g_weaponLockFlag                 D_80097D10
#define weaponRenderList                 D_800C48C0
#define g_perfectLap                     D_800914B4
#define g_logoSprite                     D_800CE1E0
#define previousPlayerDamage2            D_8009160C
#define previousPlayerDamage3            D_80091610
#define previousPlayerDamage4            D_80091614
#else
#define UpdateWeapons                    func_800368E0
#define InsertWeapon                     func_80036A40
#define FireWeapon                       func_800372B8
#define FireSuperWeapon                  func_80037414
#define MissileLock                      func_8003753C
#define RearMissileLock                  func_800376B4
#define InitWeapons                      func_800379D8
#define BumpUp                           func_80037CBC
#define BumpDown                         func_80037DDC
#define BumpFlatten                      func_80037EF8
#define FlattenTrack                     func_80038030
#define InitialiseBumps                  func_8003809C
#define UpdateQuake                      func_8003833C
#define HitReboundMissile                func_800388E0
#define UpdateReboundDelay               func_80038A74
#define UpdateMissile                    func_80038AD8
#define InitMissile                      func_800396B4
#define UpdateMinigunBullet              func_80039A18
#define UpdateMinigun                    func_8003A220
#define InitMinigun                      func_8003A2F4
#define InitMinigunBullet                func_8003A3F8
#define UpdateMine                       func_8003A7C8
#define InitQuake                        func_8003AD60
#define InitPlasma                       func_8003AFC8
#define UpdatePlasma                     func_8003B23C
#define InitRocket                       func_8003BC14
#define SetDebrisPrimColour              func_8003BEB0
#define UpdateShipDebris                 func_8003BF30
#define InitShipDebris                   func_8003C168
#define InitRocketLeft                   func_8003C7B8
#define InitRocketRight                  func_8003CBA0
#define UpdateElecBolt                   func_8003CF94
#define InitElecBolt                     func_8003D8D8
#define InitMine                         func_8003DB88
#define InitTrackTrap                    func_8003DD00
#define UpdateTrackTrap                  func_8003DE8C
#define InitSmartBomb                    func_8003E2D4
#define KillMines                        func_8003E474
#define UpdateSmartBomb                  func_8003E564
#define InitShield                       func_8003EB8C
#define UpdateShield                     func_8003ED14
#define InitCloak                        func_8003FDF8
#define UpdateCloak                      func_8003FE70
#define CycleWeaponGrids                 func_800402CC
#define GetFireVector                    func_8004069C
#define Weapon2Mine                      func_80040864
#define GetShipTeam                      func_80040984
#define GetNewWeapon                     func_80040A14
#define GetNewDroneWeapon                func_800415BC
#define UpdateRocket                     func_80041B90
#define GetWeaponDamage                  func_8004264C
#define ScaleChallengeDamage             func_80042728
#define DamageCollide                    func_80042764
#define CheckPitShips                    func_80043268
#define UpdateFadeScreen                 func_80043538
#define DrawScreenOverlay                func_8004370C
#define FlipShip                         func_80043954
#define GetCheatWeapon                   func_80043A08
#define randWep                          func_80043AE8
#define WADLoadPrm                       func_8000A458
#define NewObjectInstance                func_8000A884
#define LoadCmpFiles                     func_8000FB74
#define WADLoadCompressedTextureSequence func_80010714
#define ratan2                           func_80014C30
#define PlayNote                         func_80014D38
#define GetFaceVertex                    func_8001812C
#define UpdateCameraAttractDestroyed     func_8001B968
#define FindNearestSection               func_8001CA78
#define GetMagnitude                     func_8001CEC4
#define sar                              func_8001CF50
#define ang                              func_8001CF6C
#define newRand                          func_8001CFAC
#define GetDroneShield                   func_80020CF8
#define UpdateDroneShip                  func_80020F30
#define getShipInfrontLAP                func_80023588
#define getShipBehindLAP                 func_80023674
#define humanToDroneSpeed                func_80023768
#define PlaneLineCollision               func_80026C00
#define GetShipPosition                  func_80030A1C
#define InitTrackTrapFace                func_80044DF8
#define SetShake                         func_80049720
#define SetRumble                        func_80049738
#define ResetRenderList                  func_8004F660
#define InitRenderList                   func_8004F6E4
#define FlushRenderList                  func_8004F790
#define AddRenderGroup                   func_8004FA30
#define AddRandomParticles               func_800664EC
#define AddExplode                       func_800683D8
#define AddCloakHitEffect                func_80068440
#define AddCycloneFlash                  func_80068604
#define AddCycloneTrail                  func_80068664
#define AddPitSpark                      func_80069D00
#define SetSfxVolume                     func_80070048
#define StopSfx                          func_8007006C
#define PlaySfx                          func_800700A8
#define SetSfxFreqOffset                 func_800700CC

#define g_demoShip                       D_8008A974
#define g_multiPlayer                    D_8008A9D0
#define cheatfade                        D_80091C22
#define cheatWeapon                      D_80091C24
#define cheatDamage                      D_80091C26
#define cheatSuperCombo                  D_80091C28
#define superWeaponFlag                  D_80091E74
#define g_superWeaponActive1             D_80091E78
#define g_superWeaponActive2             D_80091E7C
#define rearLockCycle                    D_80091E80
#define missileLockTimer                 D_80091E8C
#define seedWep                          D_80091E90
#define fadeAmount                       D_80091E94
#define g_weaponDamageTable              D_80091EA0
#define g_pitHealTable                   D_80091EE0
#define g_pitHealScale                   D_80091EF0
#define shieldActive                     D_80091F00
#define weaponInserted                   D_80091F04
#define minigunFrame                     D_80091F08
#define g_debrisLife                     D_80091F0E
#define g_debrisSpeed                    D_80091F10
#define g_debrisGravity                  D_80091F14
#define g_weaponChance1P                 D_80091F18
#define g_weaponChanceMP                 D_80091F38
#define g_droneWeaponChance0             D_80091F58
#define g_droneWeaponChance1             D_80091F78
#define g_droneWeaponChance2             D_80091F98
#define g_droneWeaponChance3             D_80091FB8
#define previousPlayerDamage             D_80091FD8
#define g_pitFade                        D_80092394
#define g_unk800923F9                    D_800923F9
#define g_superComboUnlocked             D_80094F7E
#define nextParticleSlot                 D_80096A08
#define shieldSfx                        D_800972F8
#define g_pitSfxHandle                   D_800972FC
#define g_engineHandle                   D_80097304
#define g_turbHandle                     D_80097308
#define g_chargeSfxHandle                D_8009730C
#define shipDamage                       D_8009BCE0
#define g_mtxPool                        D_800A0EE0
#define g_mtxIndex                       D_800A0F34
#define CameraUpdate                     D_800A1044
#define shipClassIndex                   D_800A10C0
#define numTrackSections                 D_800A10C2
#define CameraUpdate2                    D_800A5E18
#define CameraUpdate3                    D_800A5E1C
#define CameraUpdate4                    D_800A5E20
#define ownShip                          D_800A5E2C
#define shipDeathCount                   D_800A9EC0
#define gameData                         D_800A9F00
#define g_raceClass                      D_800A9F08
#define pauseData                        D_800A9F70
#define ownShip2                         D_800A9F8C
#define ownShip3                         D_800A9F90
#define ownShip4                         D_800A9F94
#define pauseRequests                    D_800AA010
#define attractMode                      D_800AA020
#define gameType                         D_800AA024
#define g_attractMode                    D_800AA088
#define ships                            D_800AA0A4
#define playerPosition                   D_800AA0C0
#define particleSlots                    D_800AA0C4
#define player2Position                  D_800AA0D6
#define player3Position                  D_800AA0D8
#define player4Position                  D_800AA0E0
#define g_lastRammingShip                D_800AAEC0
#define g_fireLock                       D_800AAED4
#define g_weaponCharge                   D_800AAED8
#define fadeActive                       D_800AD6F0
#define fadeBlue                         D_800AD6F8
#define shieldRed                        D_800AD700
#define fadeGreen                        D_800AD740
#define fadeRed                          D_800AD748
#define shieldGreen                      D_800AD750
#define reboundTarget                    D_800AD78C
#define D_800AD78F                       D_800AD78F
#define g_fadeColour                     D_800AD790
#define g_weaponLockFlag                 D_800AD820
#define weaponRenderList                 D_800C4580
#define g_perfectLap                     D_80091E84
#define g_logoSprite                     D_800CDEA0
#define previousPlayerDamage2            D_80091FDC
#define previousPlayerDamage3            D_80091FE0
#define previousPlayerDamage4            D_80091FE4
#endif
/* END ALIASES */

#define NO_OF_SHIPS     15
#define NO_OF_WEAPONS   52
#define NO_OF_SHAPES    60
#define MAX_PLAYERS     4

/* EU (PAL) removed gspL3DEX_fifo, expanding each player's particle vertex slice from 2 to 3 (SParticle 0xE8 vs 0xA8). */
#ifdef VERSION_EU
#define PARTICLE_VERTS_PER_PLAYER   3
#else
#define PARTICLE_VERTS_PER_PLAYER   2
#endif

/* WeaponData.type / InsertWeapon types */
#define WEAPON_ROCKET       1
#define WEAPON_MISSILE      2
#define WEAPON_MINE         3
#define WEAPON_ELEC_BOLT    5
#define WEAPON_ENERGY       6
#define WEAPON_AUTOPILOT    7
#define WEAPON_SHIELD       8
#define WEAPON_TURBO        9
#define WEAPON_ROCKET_LEFT  10
#define WEAPON_ROCKET_RIGHT 11
#define WEAPON_QUAKE        12
#define WEAPON_MISSILE2     13
#define WEAPON_SMART_BOMB   14

typedef struct Vector {
    /* 0x00 */ s32 vx;
    /* 0x04 */ s32 vy;
    /* 0x08 */ s32 vz;
} Vector;

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

typedef struct Object {
    /* 0x00 */ char name[16];
    /* 0x10 */ s16 vertexCount;
    /* 0x14 */ void *vertices;
    /* 0x18 */ s16 normalCount;
    /* 0x1C */ void *normals;
    /* 0x20 */ s16 primitiveCount;
    /* 0x24 */ void *primitives;
    /* 0x28 */ Vtx *vtx;
    /* 0x2C */ struct RenderGroup *group;
    /* 0x30 */ Skeleton *skeleton;
    /* 0x34 */ s32 extent;
    /* 0x38 */ s16 flags;
    /* 0x3A */ s16 index;
    /* 0x3C */ struct Object *next;
} Object;

typedef struct Face {
    /* 0x00 */ s16 vertex[4];
    /* 0x08 */ SVector normal;
    /* 0x0E */ u8 texture;
    /* 0x0F */ u8 flags;
    /* 0x10 */ u8 red;
    /* 0x11 */ u8 green;
    /* 0x12 */ u8 blue;
} Face; /* size = 0x14 */

#define TRACK_BASE 1

struct TrackSection;

typedef union SectionPtr {
    struct TrackSection *ptr;
    s32 index;
} SectionPtr;

typedef struct TrackSection {
    /* 0x00 */ SectionPtr junction;
    /* 0x04 */ SectionPtr prevSection;
    /* 0x08 */ SectionPtr nextSection;
    /* 0x0C */ Vector centre;
    /* 0x18 */ s16 version;
    /* 0x1A */ s16 pad;
    /* 0x1C */ s16 *objects;
    /* 0x20 */ s16 objectCount;
    /* 0x24 */ s16 *northViewSection[3];
    /* 0x30 */ s16 *southViewSection[3];
    /* 0x3C */ s16 *eastViewSection[3];
    /* 0x48 */ s16 *westViewSection[3];
    /* 0x54 */ s16 *allViewSection[3];
    /* 0x60 */ s16 northViewCount[3];
    /* 0x66 */ s16 southViewCount[3];
    /* 0x6C */ s16 eastViewCount[3];
    /* 0x72 */ s16 westViewCount[3];
    /* 0x78 */ s16 allViewCount[3];
    /* 0x7E */ s16 high[4];
    /* 0x86 */ s16 med[4];
    /* 0x8E */ s16 faceStart;
    /* 0x90 */ s16 faceCount;
    /* 0x92 */ s16 globalRadius;
    /* 0x94 */ s16 localRadius;
    /* 0x96 */ s16 flags;
    /* 0x98 */ s16 secNo;
    /* 0x9A */ s8 bump;
    /* 0x9B */ s8 snow;
} TrackSection; /* size = 0x9C */

typedef struct Colors {
    /* 0x00 */ u8 red;
    /* 0x01 */ u8 green;
    /* 0x02 */ u8 blue;
} Colors;

typedef struct TrackData {
    /* 0x000 */ s32 vertexCount;
    /* 0x004 */ s32 faceCount;
    /* 0x008 */ s32 sectionCount;
    /* 0x00C */ s32 vtxCount;
    /* 0x010 */ VECTOR *vertices;
    /* 0x014 */ Vtx *vtxBuf[2];
    /* 0x01C */ Face *faces;
    /* 0x020 */ TrackSection *sections;
    /* 0x024 */ u8 unk24[0x334];
    /* 0x358 */ Colors *colCopy;
} TrackData;

/* One entry of the race line table: track distance of each section (16 bytes). */
typedef struct RaceLine {
    /* 0x00 */ s16 distance;
    /* 0x02 */ s16 pad[7];
} RaceLine;

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
    /* 0x0A2 */ s16 unkA2;
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
    /* 0x104 */ void (*update)();
} ShipData; /* size = 0x108 */

typedef struct ChallengeEntry {
    /* 0x00 */ u8 unk00[0x0C];
    /* 0x0C */ u32 weaponMask;
    /* 0x10 */ u8 unk10[0x1C];
} ChallengeEntry; /* size = 0x2C */

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
    /* 0x16 */ u8 challengeNo;
    /* 0x17 */ u8 unk17;
    /* 0x18 */ s32 ownShips[4];
    /* 0x28 */ s32 screenLayout[4];
    /* 0x38 */ s32 playerPad[4];
    /* 0x48 */ u8 unk48[0x10];
    /* 0x58 */ u8 unk58;
    /* 0x59 */ u8 unk59[5];
    /* 0x5E */ u8 superCombo;
    /* 0x5F */ u8 unk5F[5];
    /* 0x64 */ ChallengeEntry *challengeTable;
} FrontendOptions;

/* One weapon grid pad entry (16 bytes). */
typedef struct WeaponGridZone {
    /* 0x00 */ TrackSection *section;
    /* 0x04 */ u16 attr;
    /* 0x06 */ s16 angle;
    /* 0x08 */ s32 count;
    /* 0x0C */ u8 r;
    /* 0x0D */ u8 g;
    /* 0x0E */ u8 b;
    /* 0x0F */ u8 pad;
} WeaponGridZone;

typedef struct FadeColour {
    /* 0x00 */ u8 r;
    /* 0x01 */ u8 g;
    /* 0x02 */ u8 b;
    /* 0x03 */ u8 pad;
} FadeColour;

/* Shared per-race configuration (gameData). */
typedef struct GameData {
    /* 0x00 */ u8 unk00[5];
    /* 0x05 */ u8 trackNo;
} GameData;

typedef struct RenderGroupEntry {
    /* 0x00 */ s32 textureIndex;
    /* 0x04 */ Gfx *dl;
} RenderGroupEntry;

typedef struct RenderGroup {
    /* 0x00 */ u32 count;
    /* 0x04 */ RenderGroupEntry *entries;
} RenderGroup;

typedef struct RenderNode {
    /* 0x00 */ struct RenderNode *next;
    /* 0x04 */ Mtx *mtx;
    /* 0x08 */ Gfx *dl;
} RenderNode;

typedef struct RenderList {
    /* 0x00 */ u32 bucketCount;
    /* 0x04 */ u32 textureStart;
    /* 0x08 */ RenderNode *buckets;
    /* 0x0C */ RenderNode untextured;
} RenderList;

/* One entry of the weapon shape table: the object drawn for a weapon, and whether it is in use. */
typedef struct WeaponShape {
    /* 0x00 */ Object *shapePtr;
    /* 0x04 */ s16 used;
    /* 0x06 */ s16 pad;
} WeaponShape;

typedef struct TrackCamera {
    /* 0x00 */ Skeleton *camPos;
} TrackCamera;

typedef struct SParticle {
    /* 0x00 */ Vector pos;
    /* 0x0C */ Vector prevPos;
    /* 0x18 */ s32 life;
    /* 0x1C */ SVECTOR vel;
    /* 0x24 */ u8 type;
    /* 0x25 */ u8 flags;
    /* 0x26 */ u8 pad26[2];
    /* 0x28 */ Vtx vtx[MAX_PLAYERS][PARTICLE_VERTS_PER_PLAYER];
} SParticle; /* size = 0xA8 (US), 0xE8 (EU) */

typedef struct DebrisPrim {
    /* 0x00 */ s16 type;
} DebrisPrim;

typedef struct ShieldGT3 {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
    /* 0x04 */ s16 coords[3];
    /* 0x0A */ u8 pad0A[0x1A];
} ShieldGT3; /* size = 0x24 */

typedef struct ShieldG4 {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
    /* 0x04 */ s16 coords[4];
    /* 0x0C */ u8 pad0C[0x10];
} ShieldG4; /* size = 0x1C */

typedef union ShieldPoly {
    DebrisPrim *prim;
    ShieldGT3 *gt3;
    ShieldG4 *g4;
} ShieldPoly;

struct WeaponData;

typedef Gfx *(*WeaponUpdate)(ShipData *ships, WeaponShape *shapes, struct WeaponData *weapon,
                             struct WeaponData *weapons, TrackCamera *camera, Gfx *gfx);

/* One weapon in flight (0x48 bytes); the weapon table holds NO_OF_WEAPONS of them. */
typedef struct WeaponData {
    /* 0x00 */ s16 count;
    /* 0x02 */ s16 bumpStep;
    /* 0x04 */ s16 shapeNo;
    /* 0x06 */ s16 pad06;
    /* 0x08 */ TrackSection *section;
    /* 0x0C */ VECTOR accel;
    /* 0x1C */ VECTOR velocity;
    /* 0x2C */ VECTOR pos;
    /* 0x3C */ s16 hdg;
    /* 0x3E */ s16 pitch;
    /* 0x40 */ s8 target;
    /* 0x41 */ u8 owner;
    /* 0x42 */ u8 type;
    /* 0x43 */ u8 pad43;
    /* 0x44 */ WeaponUpdate update;
} WeaponData; /* size = 0x48 */

#define IS_PLAYER(n) ((n) == ownShip || (n) == ownShip2 || (n) == ownShip3 || (n) == ownShip4)

extern u8 _startupDataFile[];
extern s32 g_demoShip;
extern s32 g_multiPlayer;
extern s16 cheatfade;
extern u8 cheatWeapon;
extern u8 cheatDamage;
extern u8 cheatSuperCombo;
extern s32 superWeaponFlag;
extern s32 g_superWeaponActive1;
extern s32 g_superWeaponActive2;
extern u8 rearLockCycle[4];
extern s8 g_perfectLap[2];
extern s16 missileLockTimer;
extern s32 seedWep;
extern s16 fadeAmount[4];
extern s32 g_weaponDamageTable[16];
extern s32 g_pitHealTable[4];
extern s32 g_pitHealScale[4];
extern u8 shieldActive;
extern s32 weaponInserted;
extern u8 minigunFrame;
extern s16 g_debrisLife;
extern s32 g_debrisSpeed;
extern s32 g_debrisGravity;
extern u8 g_weaponChance1P[3][10];
extern u8 g_weaponChanceMP[3][10];
extern u8 g_droneWeaponChance0[3][10];
extern u8 g_droneWeaponChance1[3][10];
extern u8 g_droneWeaponChance2[3][10];
extern u8 g_droneWeaponChance3[3][10];
extern s32 previousPlayerDamage;
extern s32 previousPlayerDamage2;
extern s32 previousPlayerDamage3;
extern s32 previousPlayerDamage4;
extern s32 g_pitFade;
extern u8 g_unk800923F9;
extern u8 g_superComboUnlocked;
extern s32 nextParticleSlot;
extern s32 shieldSfx;
extern s32 g_pitSfxHandle;
extern s32 g_engineHandle;
extern s32 g_turbHandle;
extern s32 g_chargeSfxHandle;
extern s32 shipDamage[];
extern Mtx *g_mtxPool;
extern s32 g_mtxIndex;
extern void (*CameraUpdate)();
extern u16 shipClassIndex;
extern s16 numTrackSections;
extern void (*CameraUpdate2)();
extern void (*CameraUpdate3)();
extern void (*CameraUpdate4)();
extern s32 ownShip;
extern s32 shipDeathCount[];
extern GameData gameData;
extern s8 g_raceClass;
extern s32 pauseData;
extern s32 ownShip2;
extern s32 ownShip3;
extern s32 ownShip4;
extern s32 pauseRequests[4];
extern u8 attractMode;
extern s32 gameType;
extern s32 g_attractMode;
extern ShipData *ships;
extern u16 playerPosition;
extern SParticle *particleSlots;
extern u16 player2Position;
extern u16 player3Position;
extern u16 player4Position;
extern s32 g_lastRammingShip;
extern u8 g_fireLock[4];
extern s16 g_weaponCharge[NO_OF_SHIPS];
extern u8 fadeActive[4];
extern s16 fadeBlue[4];
extern s32 shieldRed[NO_OF_SHIPS];
extern s16 fadeGreen[4];
extern s16 fadeRed[4];
extern s32 shieldGreen[NO_OF_SHIPS];
extern s32 reboundTarget;
extern FadeColour g_fadeColour[4];
extern s32 g_weaponLockFlag;
extern RenderList weaponRenderList;
extern FrontendOptions frontendOptions_g;
extern TrackData *track;
extern RaceLine *g_unkAlloc9600;
extern s16 g_unk8008A970;
extern WADFILE *g_startupImage;
extern uSprite g_logoSprite;

s16 rsin(s16 angle);
s16 rcos(s16 angle);
s32 ratan2(s32 y, s32 x);
s32 SquareRoot0(u32 a);
s32 GetMagnitude(VECTOR vec);
s32 sar(s32 value, s32 shift);
s16 ang(s16 angle);
s32 newRand(void);
s32 GetDroneShield(s32 shipNo);
s16 GetShipPosition(ShipData *ships, s32 shipNo);
void UpdateCameraAttractDestroyed();
TrackSection *FindNearestSection(TrackSection *trk, VECTOR pos);
void GetFaceVertex(TrackSection *section, Face *face, s32 vertex, VECTOR *out);
s32 PlaneLineCollision(VECTOR point, VECTOR pos, SVector normal);
void AddExplode(s32 x, s32 y, s32 z, s32 arg3);
void AddCloakHitEffect(s32 x, s32 y, s32 z, s32 arg3);
void AddCycloneFlash(s32 x, s32 y, s32 z, s32 size, u8 *rgb);
void AddCycloneTrail(VECTOR *pos, VECTOR *vel, s32 arg2, u8 type);
void AddPitSpark(VECTOR *pos, s32 arg1);
void AddRandomParticles(SParticle *particles, s32 count, VECTOR *pos, VECTOR *vel);
void InitTrackTrapFace(WeaponData *weapon, ShipData *ship);
void SetSfxVolume(s32 handle, s32 volume);
void StopSfx(s32 *handle);
void PlayNote(s32 id, s32 vol, s32 a2, s32 a3, s32 pitch);
void SetRumble(s32 time, s32 player);
void SetShake(s32 strength, s32 player);
s16 getShipInfrontLAP(ShipData *ships, s16 shipNo);
s16 getShipBehindLAP(ShipData *ships, s16 shipNo);
void humanToDroneSpeed(ShipData *ships, s32 shipNo);
void UpdateDroneShip();
s32 PlaySfx(s32 number, s32 volume, s32 pan, s32 restartflag, s32 priority);
void SetSfxFreqOffset(s32 handle, f32 offset);
void LoadCmpFiles(void *files);
void *WADLoadCompressedTextureSequence(WADFILE *wad);
Object *WADLoadPrm(WADFILE *wad, RenderList *list);
Object *NewObjectInstance(Object *object);
void ResetRenderList(RenderList *list);
void InitRenderList(RenderList *list, u32 textureStart, u32 bucketCount);
Gfx *FlushRenderList(RenderList *list, Gfx *gfx);
void AddRenderGroup(RenderList *list, RenderGroup *group, Mtx *mtx);

Gfx *UpdateWeapons(ShipData *ships, WeaponShape *shapes, WeaponData *weapons, TrackCamera *camera, Gfx *gfx);
void InsertWeapon(ShipData *ship, WeaponData *weapons, s32 owner, u8 type);
void FireWeapon(ShipData *ship, WeaponData *weapons, s32 owner, u8 type);
void FireSuperWeapon(ShipData *ship, WeaponData *weapons, s32 owner, u8 count);
void MissileLock(s32 shipNo, ShipData *ships, u8 weapon);
void RearMissileLock(s32 shipNo, ShipData *ships);
void InitWeapons(WeaponShape *shapes, Skeleton *camPos, WeaponData *weapons);
void BumpUp(TrackSection *bump);
void BumpDown(TrackSection *bump);
void BumpFlatten(TrackSection *bump);
void FlattenTrack(void);
void InitialiseBumps(void);
Gfx *UpdateQuake(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons, TrackCamera *camera, Gfx *gfx);
Gfx *HitReboundMissile(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons, TrackCamera *camera, s32 shipNo, Gfx *gfx);
Gfx *UpdateReboundDelay(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons, TrackCamera *camera, Gfx *gfx);
Gfx *UpdateMissile(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons, TrackCamera *camera, Gfx *gfx);
Gfx *InitMissile(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons, TrackCamera *camera, Gfx *gfx);
Gfx *UpdateMinigunBullet(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons, TrackCamera *camera, Gfx *gfx);
Gfx *UpdateMinigun(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons, TrackCamera *camera, Gfx *gfx);
Gfx *InitMinigun(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons, TrackCamera *camera, Gfx *gfx);
Gfx *InitMinigunBullet(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons, TrackCamera *camera, Gfx *gfx);
Gfx *UpdateMine(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons, TrackCamera *camera, Gfx *gfx);
Gfx *InitQuake(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons, TrackCamera *camera, Gfx *gfx);
Gfx *InitPlasma(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons, TrackCamera *camera, Gfx *gfx);
Gfx *UpdatePlasma(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons, TrackCamera *camera, Gfx *gfx);
Gfx *InitRocket(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons, TrackCamera *camera, Gfx *gfx);
void SetDebrisPrimColour(Gfx *dl, u8 r, u8 g, u8 b, s32 a);
Gfx *UpdateShipDebris(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons, TrackCamera *camera, Gfx *gfx);
Gfx *InitShipDebris(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons, TrackCamera *camera, Gfx *gfx);
Gfx *InitRocketLeft(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons, TrackCamera *camera, Gfx *gfx);
Gfx *InitRocketRight(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons, TrackCamera *camera, Gfx *gfx);
Gfx *UpdateElecBolt(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons, TrackCamera *camera, Gfx *gfx);
Gfx *InitElecBolt(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons, TrackCamera *camera, Gfx *gfx);
Gfx *InitMine(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons, TrackCamera *camera, Gfx *gfx);
Gfx *InitTrackTrap(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons, TrackCamera *camera, Gfx *gfx);
Gfx *UpdateTrackTrap(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons, TrackCamera *camera, Gfx *gfx);
Gfx *InitSmartBomb(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons, TrackCamera *camera, Gfx *gfx);
void KillMines(WeaponData *weapon, WeaponData *weapons, WeaponShape *shapes);
Gfx *UpdateSmartBomb(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons, TrackCamera *camera, Gfx *gfx);
Gfx *InitShield(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons, TrackCamera *camera, Gfx *gfx);
Gfx *UpdateShield(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons, TrackCamera *camera, Gfx *gfx);
Gfx *InitCloak(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons, TrackCamera *camera, Gfx *gfx);
Gfx *UpdateCloak(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons, TrackCamera *camera, Gfx *gfx);
void CycleWeaponGrids(ShipData *ships, WeaponGridZone *grids, s32 noGrids);
void GetFireVector(VECTOR *result, VECTOR target, Face *facePtr, VECTOR shipPos, TrackSection *section);
void Weapon2Mine(WeaponData *weapon, WeaponData *weapons, WeaponShape *shapes, s32 flag);
s16 GetShipTeam(s32 shipNo);
void GetNewWeapon(ShipData *ships, s32 shipNo);
void GetNewDroneWeapon(ShipData *ships, s32 shipNo);
Gfx *UpdateRocket(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons, TrackCamera *camera, Gfx *gfx);
void GetWeaponDamage(s32 *damage, s32 kind);
void ScaleChallengeDamage(s32 *damage, s32 kind);
void DamageCollide(s32 damageType, s32 shipNo, WeaponData *weapon);
void CheckPitShips(void);
void UpdateFadeScreen(s32 player);
Gfx *DrawScreenOverlay(s32 player, s32 fullScreen, Gfx *gfx, u8 isDim);
void FlipShip(ShipData *ship);
void GetCheatWeapon(ShipData *ships, s32 shipNo);
s32 randWep(void);

#endif /* WIPEOUT64_WEAPON_H */
