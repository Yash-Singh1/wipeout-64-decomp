#ifndef WIPEOUT64_PARTICLE_H
#define WIPEOUT64_PARTICLE_H

#include "common.h"

/*
 * Temporary US/EU symbol mappings until config/{us,eu}/symbol_addrs.txt
 * registers canonical names for Particle.c functions and external symbols.
 */
#ifdef VERSION_EU
#define InitParticle            func_80068230
#define CheckParticleGuard      func_8006827C
#define AddRandomParticles      func_800682C0
#define AddScrapingParticles    func_800683F8
#define AddEboltParticles       func_80068530
#define AddRisingParticles      func_8006864C
#define AddPitParticles         func_800687E4
#define ClearParticleTable      func_80068928
#define LoadExplodeGFX          func_80068968
#define InitExplode             func_800689E4
#define DrawExplode             func_80068A58
#define DrawSuperFlashes        func_80068E18
#define DrawCycloneCharge       func_800691E0
#define InitRing                func_80069850
#define DrawRing                func_80069D00
#define AddSpurtExplode         func_8006A1A4
#define AddExplode              func_8006A20C
#define AddMinigunExplode       func_8006A274
#define AddSmallSpurtExplode    func_8006A2EC
#define AddSmallSmokeExplode    func_8006A358
#define AddSmokeExplode         func_8006A3C4
#define AddRing                 func_8006A438
#define AddSuperFlash           func_8006A498
#define SpawnCycloneCharge      func_8006A528
#define DrawMissileFlare        func_8006A59C
#define DrawRocketFlare         func_8006AC50
#define DrawPlasmaFlare         func_8006B30C
#define InitElectric            func_8006BB2C
#define AddElectric             func_8006BB34
#define AddElectricSegment      func_8006BD1C
#define DrawElectric            func_8006BDB4
#define resetMiscFx             func_8006C2E4
#define UpdateParticles         func_8006C308
#define ApproxDistance3D        func_8006C718
#define DrawPodiumIcons         func_8006C78C
#define ClearCycloneCharges     func_8006CB9C

#define LoadCmpFiles            func_80016994
#define WADLoadCompressedTextureSequence func_80017534
#define newRand                 func_80023FCC

#define TextureTableCount       g_unk8008A970
#define g_superWeaponActive2    D_800914AC
#define g_podiumShips           D_80095F80
#define particleGuard           D_80095F98
#define missileFlareCount       D_80095FA0
#define plasmaFlareCount        D_80095FA8
#define electricSlotIndex       D_80095FB0
#define rocketFlareCount        D_80095FB4
#define explosionAnimFrame      D_80095FBC
#define nextParticleSlot        D_80095FC0
#define cycloneAnimFrame        D_80095FC4
#define cycloneAnimTick         D_80095FC8
#define shadow2TextureStart     D_80098E5C
#define shipShapes              D_800A9048
#define g_mtxPool               D_800AE4C0
#define g_mtxIndex              D_800AE514
#define pauseRequests           D_800B75F0
#define ships                   D_800B767C
#define g_glistp                D_800B842C
#define frontendOptions_g       D_800C5E60
#define cameras                 D_800CBE30
#define podiumVtx               D_800DE980
#define plasmaFlareVtx          D_800DEC80
#define cycloneCharges          D_800DF980
#define cycloneVtx              D_800DF9E0
#define rocketFlareVtx          D_800DFDE0
#define superFlashes            D_800E3260
#define rings                   D_800E3530
#define explosions              D_800E35F0
#define superFlashVtx           D_800E3A70
#define explodeVtx              D_800E5870
#define missileFlareVtx         D_800E8870
#define explodeTextureStart     D_800EA470
#define electrics               D_800EA478
#define ringVtx                 D_800ED978
#define explodeTexture          D_800F1978
#else
#define InitParticle            func_800664A0
#define AddRandomParticles      func_800664EC
#define AddScrapingParticles    func_80066624
#define AddEboltParticles       func_8006675C
#define AddRisingParticles      func_80066878
#define AddPitParticles         func_80066A10
#define ClearParticleTable      func_80066B54
#define LoadExplodeGFX          func_80066B94
#define InitExplode             func_80066C10
#define DrawExplode             func_80066C84
#define DrawSuperFlashes        func_80067044
#define DrawCycloneCharge       func_8006740C
#define InitRing                func_80067A7C
#define DrawRing                func_80067F2C
#define AddSpurtExplode         func_80068370
#define AddExplode              func_800683D8
#define AddMinigunExplode       func_80068440
#define AddSmallSpurtExplode    func_800684B8
#define AddSmallSmokeExplode    func_80068524
#define AddSmokeExplode         func_80068590
#define AddRing                 func_80068604
#define AddSuperFlash           func_80068664
#define SpawnCycloneCharge      func_800686F4
#define DrawMissileFlare        func_80068768
#define DrawRocketFlare         func_80068E1C
#define DrawPlasmaFlare         func_800694D8
#define InitElectric            func_80069CF8
#define AddElectric             func_80069D00
#define AddElectricSegment      func_80069EDC
#define DrawElectric            func_80069F60
#define resetMiscFx             func_8006A3C0
#define UpdateParticles         func_8006A3E4
#define ApproxDistance3D        func_8006A6D4
#define DrawPodiumIcons         func_8006A748
#define ClearCycloneCharges     func_8006AB58

#define LoadCmpFiles            func_8000FB74
#define WADLoadCompressedTextureSequence func_80010714
#define newRand                 func_8001CFAC

#define TextureTableCount       g_unk8008A970
#define g_superWeaponActive2    D_80091E7C
#define g_podiumShips           D_800969D0
#define missileFlareCount       D_800969E8
#define plasmaFlareCount        D_800969F0
#define electricSlotIndex       D_800969F8
#define rocketFlareCount        D_800969FC
#define explosionAnimFrame      D_80096A04
#define nextParticleSlot        D_80096A08
#define cycloneAnimFrame        D_80096A0C
#define cycloneAnimTick         D_80096A10
#define shipShapes              D_8009BA68
#define g_mtxPool               D_800A0EE0
#define g_mtxIndex              D_800A0F34
#define pauseRequests           D_800AA010
#define ships                   D_800AA0A4
#define g_glistp                D_800AAE5C
#define shadow2TextureStart     D_800AE96C
#define frontendOptions_g       D_800C5B20
#define cameras                 D_800CBAF0
#define podiumVtx               D_800DDE40
#define plasmaFlareVtx          D_800DE140
#define cycloneCharges          D_800DEE40
#define cycloneVtx              D_800DEEA0
#define rocketFlareVtx          D_800DF2A0
#define superFlashes            D_800E2720
#define rings                   D_800E29F0
#define explosions              D_800E2AB0
#define superFlashVtx           D_800E2F30
#define explodeVtx              D_800E4D30
#define missileFlareVtx         D_800E7D30
#define explodeTextureStart     D_800E9930
#define electrics               D_800E9938
#define ringVtx                 D_800EBE38
#define explodeTexture          D_800EFE38
#endif

#include "dheap.h"
#include "gte.h"
#include "n64render.h"
#include "quat.h"
#include "radscn.h"
#include "skeleton.h"
#include "wadfile.h"
#include <ultra64.h>

#define PARTICLE_COUNT          256
#define EXPLOSION_COUNT         48
#define EXPLODE_FRAMES          32
#define SUPER_FLASH_COUNT       30
#define RING_COUNT              8
#define CYCLONE_CHARGE_COUNT    4
#define MISSILE_FLARE_COUNT     8
#define ROCKET_FLARE_COUNT      15
#define PLASMA_FLARE_COUNT      4
#define ELECTRIC_COUNT          32
#define PODIUM_ICON_COUNT       3
#define MAX_PLAYERS             4

#define SHIP_COMBAT_DESTROYED   0x8000

/*
 * Retail Europe removed the gspL3DEX_fifo line microcode and renders both
 * particle streaks and electric bolts as thin F3DEX triangles instead of
 * gSPLine3D segments, increasing per-player vertex counts.
 */
#ifdef VERSION_EU
#define PARTICLE_VERTS_PER_PLAYER   3
#define ELECTRIC_VERTS_PER_PLAYER   6
#define CYCLONE_MAX_CHARGE          25
#else
#define PARTICLE_VERTS_PER_PLAYER   2
#define ELECTRIC_VERTS_PER_PLAYER   4
#define CYCLONE_MAX_CHARGE          30
#endif

typedef struct TrackCamera {
    /* 0x00 */ Skeleton *camPos;
    /* 0x04 */ TrackSection *section;
    /* 0x08 */ s16 hdg, pitch, roll;
    /* 0x0E */ s16 vhdg, vpitch, vroll;
    /* 0x14 */ s16 vel[4];
} TrackCamera; /* size = 0x1C */

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
    /* 0x0B2 */ u8 unkB2[0x4C];
    /* 0x0FE */ u8 visible[MAX_PLAYERS];
    /* 0x102 */ u8 unk102[0x06];
} ShipData; /* size = 0x108 */

/* One entry of the particle table shared by sparks, smoke and electric bolts. */
typedef struct SParticle {
    /* 0x00 */ Vector pos;
    /* 0x0C */ Vector prevPos;
    /* 0x18 */ s32 life;
    /* 0x1C */ SVECTOR vel;           /* vel.pad holds the spawning burst count */
    /* 0x24 */ u8 type;               /* 0 = free */
    /* 0x25 */ u8 flags;
    /* 0x26 */ u8 pad26[2];
    /* 0x28 */ Vtx vtx[MAX_PLAYERS][PARTICLE_VERTS_PER_PLAYER];
} SParticle; /* size = 0xA8 (US), 0xE8 (EU) */

/* A billboard explosion, shock ring, or super-weapon flash entry. */
typedef struct Explosion {
    /* 0x00 */ s32 x, y, z;
    /* 0x0C */ u8 size;
    /* 0x0D */ u8 r, g, b;
    /* 0x10 */ s16 flash;             /* 1: plays the flash frames instead of the fireball */
    /* 0x12 */ s16 blend;
    /* 0x14 */ s16 life;              /* free when <= 0 */
    /* 0x16 */ s16 ship;              /* position is relative to this ship when >= 0 */
} Explosion; /* size = 0x18 */

/* An active cyclone charge orb attached to a ship nose. */
typedef struct CycloneCharge {
    /* 0x00 */ s32 x, y, z;
    /* 0x0C */ u8 r, g, b;
    /* 0x0F */ u8 pad0F;
    /* 0x10 */ s16 active;
    /* 0x12 */ s16 charge;
    /* 0x14 */ s32 ship;
} CycloneCharge; /* size = 0x18 */

/* An electric bolt rendered between start and end with two jittered midpoints. */
typedef struct Electric {
    /* 0x00 */ VECTOR start;
    /* 0x10 */ VECTOR end;
    /* 0x20 */ s32 life;              /* -1 when unused */
    /* 0x24 */ s32 mode;              /* 2 = red/yellow super-weapon bolt, else blue/white */
    /* 0x28 */ Vtx vtx[MAX_PLAYERS][ELECTRIC_VERTS_PER_PLAYER];
} Electric; /* size = 0x128 (US), 0x1A8 (EU) */

extern s16 TextureTableCount;
extern WADFILE *g_startupImage;
extern s32 g_superWeaponActive2;
extern s32 g_podiumShips[PODIUM_ICON_COUNT];
#ifdef VERSION_EU
extern u32 particleGuard[2];
#endif
extern s32 missileFlareCount;
extern s32 plasmaFlareCount;
extern s32 electricSlotIndex;
extern s32 rocketFlareCount;
extern u8 explosionAnimFrame;
extern s32 nextParticleSlot;
extern s32 cycloneAnimFrame;
extern s32 cycloneAnimTick;
extern s32 shadow2TextureStart;
extern Object *shipShapes[];
extern N64Mtx *g_mtxPool;
extern s32 g_mtxIndex;
extern s32 pauseRequests[MAX_PLAYERS];
extern ShipData *ships;
extern Gfx *g_glistp;
extern FrontendOptions frontendOptions_g;
extern TrackCamera cameras[MAX_PLAYERS];

extern Vtx podiumVtx[MAX_PLAYERS][PODIUM_ICON_COUNT][4];
extern Vtx plasmaFlareVtx[MAX_PLAYERS][PLASMA_FLARE_COUNT][13];
extern CycloneCharge cycloneCharges[CYCLONE_CHARGE_COUNT];
extern Vtx cycloneVtx[MAX_PLAYERS][CYCLONE_CHARGE_COUNT][4];
extern Vtx rocketFlareVtx[MAX_PLAYERS][ROCKET_FLARE_COUNT][14];
extern Explosion superFlashes[SUPER_FLASH_COUNT];
extern Explosion rings[RING_COUNT];
extern Explosion explosions[EXPLOSION_COUNT];
extern Vtx superFlashVtx[MAX_PLAYERS][SUPER_FLASH_COUNT][4];
extern Vtx explodeVtx[MAX_PLAYERS][EXPLOSION_COUNT][4];
extern Vtx missileFlareVtx[MAX_PLAYERS][MISSILE_FLARE_COUNT][14];
extern s32 explodeTextureStart;
extern Electric electrics[ELECTRIC_COUNT];
extern Vtx ringVtx[MAX_PLAYERS][RING_COUNT][32];
extern N64Texture *explodeTexture[EXPLODE_FRAMES];

s32 newRand(void);
void LoadCmpFiles(void *files);
void *WADLoadCompressedTextureSequence(WADFILE *wad);
void __assert(const char *expr, const char *file, s32 line);

SParticle *InitParticle(void);
#ifdef VERSION_EU
void CheckParticleGuard(void);
#endif
void AddRandomParticles(SParticle *particles, s32 count, VECTOR *pos, VECTOR *vel);
void AddScrapingParticles(SParticle *particles, s32 count, VECTOR *pos, VECTOR *vel);
void AddEboltParticles(SParticle *particles, s32 count, VECTOR *pos);
void AddRisingParticles(SParticle *particles, s32 count, VECTOR *pos, s32 fallSpeed);
void AddPitParticles(SParticle *particles, s32 count, VECTOR *pos, s32 fallSpeed);
void ClearParticleTable(SParticle *particles);
void LoadExplodeGFX(void);
void InitExplode(void);
void DrawExplode(VECTOR *camera, MATRIX *view, s32 player);
void DrawSuperFlashes(VECTOR *camera, MATRIX *view, s32 player);
void DrawCycloneCharge(VECTOR *camera, MATRIX *view, s32 player);
void InitRing(void);
void DrawRing(VECTOR *camera, MATRIX *view, s32 player);
void AddSpurtExplode(s32 x, s32 y, s32 z, s16 ship);
void AddExplode(s32 x, s32 y, s32 z, s16 ship);
void AddMinigunExplode(s32 x, s32 y, s32 z, s32 unused);
void AddSmallSpurtExplode(s32 x, s32 y, s32 z, s16 ship);
void AddSmallSmokeExplode(s32 x, s32 y, s32 z, s16 ship);
void AddSmokeExplode(s32 x, s32 y, s32 z, s32 unused);
void AddRing(s32 x, s32 y, s32 z, s32 unused, u8 *rgb);
void AddSuperFlash(VECTOR *pos, s32 unused, s16 life, s32 colourType);
void SpawnCycloneCharge(s32 shipNo, s32 charge);
void DrawMissileFlare(VECTOR *pos, VECTOR *camera, MATRIX *view, MATRIX *rot1, MATRIX *rot2, s32 player);
void DrawRocketFlare(VECTOR *pos, VECTOR *camera, MATRIX *view, MATRIX *rot1, MATRIX *rot2, s32 player);
void DrawPlasmaFlare(VECTOR *pos, VECTOR *camera, MATRIX *view, MATRIX *rot1, MATRIX *rot2, s32 player, s32 isRed);
void InitElectric(void);
void AddElectric(VECTOR *position, s32 mode);
void AddElectricSegment(VECTOR *start, VECTOR *end);
void DrawElectric(VECTOR *camera, s32 player);
void resetMiscFx(void);
void UpdateParticles(VECTOR *camera, SParticle *particles, s32 player);
s32 ApproxDistance3D(s32 x, s32 y, s32 z);
void DrawPodiumIcons(VECTOR *camera, MATRIX *view, s32 player);
void ClearCycloneCharges(void);

#endif /* WIPEOUT64_PARTICLE_H */
