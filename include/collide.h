#ifndef WIPEOUT64_COLLIDE_H
#define WIPEOUT64_COLLIDE_H

#include "common.h"
#include "skeleton.h"

/*
 * Temporary US/EU symbol mappings until config/{us,eu}/symbol_addrs.txt
 * registers canonical names for collide.c functions and external symbols.
 */
#ifdef VERSION_EU
#define doShipShipCollision         func_8002AC50
#define Ship2ShipCollision          func_8002ACD8
#define collideLineToPlane          func_8002B48C
#define Ship2TrackBase              func_8002BD78
#define DroneShip2TrackBase         func_8002C0FC
#define Ship2Track                  func_8002C298
#define BeamCollideReaction         func_8002D5D4
#define NewWingCollision            func_8002D704
#define WingCollision               func_8002DA90
#define NoseCollision               func_8002DED0
#define PlaneLineCollision          func_8002E2A4
#define CheckPolygonCollision       func_8002E374

#define SetRumble                   func_8000D5EC
#define PlayNote                    func_8001BCE8
#define GetFaceVertex               func_8001F06C
#define GetAng                      func_80023CB4
#define ScalarProduct               func_80023DC0
#define CrossProduct                func_80023E08
#define MinusVector                 func_80023EA0
#define GetMagnitude                func_80023EE4
#define sar                         func_80023F70
#define assign                      func_80023FA8
#define getWorldPoint               func_80025F58
#define getShipInfrontLAP           func_8002AA2C
#define DamageCollide               func_8004A488
#define AddRandomParticles          func_800682C0
#define AddScrapingParticles        func_800683F8

#define collideMinAlpha             D_80091240
#define beamCollide                 D_800912D0
#define g_scrapeVol                 D_800968A4
#define ownShip                     D_800B3414
#define ownShip2                    D_800B756C
#define ownShip3                    D_800B7570
#define ownShip4                    D_800B7574
#define ships                       D_800B767C
#define particleSlots               D_800B7698
#define g_collideAttacker           D_800B8490
#else
#define doShipShipCollision         func_800237B0
#define Ship2ShipCollision          func_80023838
#define collideLineToPlane          func_80023FEC
#define Ship2TrackBase              func_800248D8
#define DroneShip2TrackBase         func_80024B2C
#define Ship2Track                  func_80024BF4
#define BeamCollideReaction         func_80025F30
#define NewWingCollision            func_80026060
#define WingCollision               func_800263EC
#define NoseCollision               func_8002682C
#define PlaneLineCollision          func_80026C00
#define CheckPolygonCollision       func_80026CD0

#define SetRumble                   func_80049738
#define PlayNote                    func_80014D38
#define GetFaceVertex               func_8001812C
#define GetAng                      func_8001CC94
#define ScalarProduct               func_8001CDA0
#define CrossProduct                func_8001CDE8
#define MinusVector                 func_8001CE80
#define GetMagnitude                func_8001CEC4
#define sar                         func_8001CF50
#define assign                      func_8001CF88
#define getWorldPoint               func_8001ECBC
#define getShipInfrontLAP           func_80023588
#define DamageCollide               func_80042764
#define AddRandomParticles          func_800664EC
#define AddScrapingParticles        func_80066624

#define collideMinAlpha             D_80091C10
#define beamCollide                 D_80091CA0
#define g_scrapeVol                 D_800972F4
#define ownShip                     D_800A5E2C
#define ownShip2                    D_800A9F8C
#define ownShip3                    D_800A9F90
#define ownShip4                    D_800A9F94
#define ships                       D_800AA0A4
#define particleSlots               D_800AA0C4
#define g_collideAttacker           D_800AAEC0
#endif

/* PAL runs the game at 50Hz, so per-frame impulses are scaled up by 60/50. */
#ifdef VERSION_EU
#define PAL_SPEED(x)                (((x) * 60) / 50)
#else
#define PAL_SPEED(x)                (x)
#endif

/* Object index of the ship collision hull used for ship-to-ship tests. */
#define SHIP_COLLISION_SHAPE        30

/* Primitive types */
#define TypeF3                      1
#define TypeFT3                     2
#define TypeG3                      5
#define TypeGT3                     6

/* Face.flags */
#define TRACK_BASE                  0x01

/* TrackSection.flags */
#define JunctionEnd                 0x08
#define JunctionStart               0x10

/* ShipData.attr */
#define DIRECTION                   0x0008
#define WALL_COLL                   0x0020
#define SHIP_COLL                   0x0080

/* ShipData.combatAttr */
#define SHIP_SHIELDED               0x0001
#define SHIP_NO_COLLIDE             0x0100

/* Damage types passed to DamageCollide */
#define DAMAGE_WALL                 0x81
#define DAMAGE_SHIP                 0x82

/* Sound effect IDs */
#define SFX_SHIP_BUMP               0x03
#define SFX_WALL_SCRAPE             0x18

typedef struct VECTOR {
    /* 0x00 */ s32 vx;
    /* 0x04 */ s32 vy;
    /* 0x08 */ s32 vz;
    /* 0x0C */ s32 pad;
} VECTOR;

typedef struct Vector {
    /* 0x00 */ s32 vx;
    /* 0x04 */ s32 vy;
    /* 0x08 */ s32 vz;
} Vector;

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
    /* 0x96 */ u16 flags;
    /* 0x98 */ u16 secNo;
    /* 0x9A */ s8 bump;
    /* 0x9B */ s8 snow;
} TrackSection; /* size = 0x9C */

typedef struct TrackData {
    /* 0x000 */ s32 vertexCount;
    /* 0x004 */ s32 faceCount;
    /* 0x008 */ s32 sectionCount;
    /* 0x00C */ s32 vtxCount;
    /* 0x010 */ VECTOR *vertices;
    /* 0x014 */ void *vtxBuf[2];
    /* 0x01C */ Face *faces;
    /* 0x020 */ TrackSection *sections;
} TrackData;

typedef struct Primitive {
    /* 0x00 */ s16 type;
} Primitive;

typedef struct F3 {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
    /* 0x04 */ s16 coords[3];
    /* 0x0A */ s16 pad1;
    /* 0x0C */ u8 colour[4];
} F3; /* size = 0x10 */

typedef struct FT3 {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
    /* 0x04 */ s16 coords[3];
    /* 0x0A */ s16 texture;
    /* 0x0C */ s16 cba;
    /* 0x0E */ s16 tsb;
    /* 0x10 */ u8 u0, v0;
    /* 0x12 */ u8 u1, v1;
    /* 0x14 */ u8 u2, v2;
    /* 0x16 */ s16 pad1;
    /* 0x18 */ u8 colour[4];
} FT3; /* size = 0x1C */

typedef struct G3 {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
    /* 0x04 */ s16 coords[3];
    /* 0x0A */ s16 pad1;
    /* 0x0C */ u8 colour[3][4];
} G3; /* size = 0x18 */

typedef struct GT3 {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
    /* 0x04 */ s16 coords[3];
    /* 0x0A */ s16 texture;
    /* 0x0C */ s16 cba;
    /* 0x0E */ s16 tsb;
    /* 0x10 */ u8 u0, v0;
    /* 0x12 */ u8 u1, v1;
    /* 0x14 */ u8 u2, v2;
    /* 0x16 */ s16 pad1;
    /* 0x18 */ u8 colour[3][4];
} GT3; /* size = 0x24 */

typedef union Poly {
    Primitive *prim;
    F3 *f3;
    FT3 *ft3;
    G3 *g3;
    GT3 *gt3;
} Poly;

typedef struct Object {
    /* 0x00 */ char name[16];
    /* 0x10 */ s16 vertexCount;
    /* 0x14 */ void *vertices;
    /* 0x18 */ s16 normalCount;
    /* 0x1C */ void *normals;
    /* 0x20 */ s16 primitiveCount;
    /* 0x24 */ Primitive *primitives;
    /* 0x28 */ void *vtx;
    /* 0x2C */ void *bspTree;
    /* 0x30 */ Skeleton *skeleton;
    /* 0x34 */ s32 extent;
    /* 0x38 */ s16 flags;
    /* 0x3A */ s16 index;
    /* 0x3C */ struct Object *next;
} Object; /* size = 0x40 */

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
    /* 0x0B2 */ u8 unkB2[0x108 - 0xB2];
} ShipData; /* size = 0x108 */

typedef struct FrontendOptions {
    /* 0x00 */ s32 team;
    /* 0x04 */ u8 teamClass;
    /* 0x05 */ u8 unk05[7];
    /* 0x0C */ s32 trackNo;
    /* 0x10 */ u8 numPlayers;
    /* 0x11 */ u8 unk11[7];
    /* 0x18 */ s32 ownShips[4];
} FrontendOptions;

typedef struct SParticle SParticle;
typedef struct WeaponData WeaponData;

extern TrackData *track;
extern FrontendOptions frontendOptions_g;
extern s32 collideMinAlpha;
extern s16 beamCollide;
extern s16 g_scrapeVol;
extern s32 ownShip;
extern s32 ownShip2;
extern s32 ownShip3;
extern s32 ownShip4;
extern ShipData *ships;
extern SParticle *particleSlots;
extern s32 g_collideAttacker;

void __assert(const char *expr, const char *file, s32 line);
void SetRumble(s32 time, s32 player);
void PlayNote(s32 sound, s32 volume, s32 lrPan, s32 fbPan, s32 pitch);
void GetFaceVertex(TrackSection *section, Face *face, s32 vertex, VECTOR *out);
s32 GetAng(VECTOR vec1, VECTOR vec2);
s32 ScalarProduct(VECTOR v, VECTOR w);
void CrossProduct(VECTOR *out, VECTOR a, VECTOR b);
void MinusVector(VECTOR start, VECTOR end, VECTOR *result);
s32 GetMagnitude(VECTOR vec);
s32 sar(s32 a, s32 n);
void assign(VECTOR *out, VECTOR v);
void getWorldPoint(VECTOR *output, Object *object, Object *objectCollide, s32 pointNo);
s16 getShipInfrontLAP(ShipData *shipIndex, s16 shipNo);
void DamageCollide(s32 damageType, s32 shipNo, WeaponData *weapon);
void AddRandomParticles(SParticle *particles, s32 count, VECTOR *pos, VECTOR *vel);
void AddScrapingParticles(SParticle *particles, s32 count, VECTOR *pos, VECTOR *vel);

void doShipShipCollision(s32 shipNo, ShipData *shipIndex, Object **shipShapes);
void Ship2ShipCollision(ShipData *shipIndex, Object **shipShapes, s32 i, s32 j);
s32 collideLineToPlane(s32 shipNo1, s32 shipNo2, Object **shipShapes, ShipData *shipIndex);
void Ship2TrackBase(ShipData *playerShip, Face *facePtr, s32 alpha);
void DroneShip2TrackBase(ShipData *playerShip, Face *facePtr, s32 alpha);
void Ship2Track(ShipData *playerShip, Face *facePtr, Object **shipShapes);
void BeamCollideReaction(ShipData *playerShip);
void NewWingCollision(ShipData *playerShip, Face *facePtr, s32 direction, s32 alpha);
void WingCollision(ShipData *playerShip, Face *facePtr, s32 direction, s32 alpha);
void NoseCollision(ShipData *playerShip, Face *facePtr, s32 direction, s32 alpha);
s32 PlaneLineCollision(VECTOR planePoint, VECTOR shipPos, SVector normal);
s32 CheckPolygonCollision(Face *facePtr, VECTOR pos, s32 alpha, s32 side, TrackSection *section);

#endif /* WIPEOUT64_COLLIDE_H */
