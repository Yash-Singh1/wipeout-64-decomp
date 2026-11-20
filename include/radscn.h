#ifndef WIPEOUT64_RADSCN_H
#define WIPEOUT64_RADSCN_H

#include "common.h"
#include "error.h"
#include "skeleton.h"

#define ObjectListMax 128
#define Jump 1

#define Warning 0
#define NonFatal 1
#define Fatal 2

typedef struct Vector {
    /* 0x00 */ s32 vx;
    /* 0x04 */ s32 vy;
    /* 0x08 */ s32 vz;
} Vector;

typedef struct Object {
    /* 0x00 */ char name[16];
    /* 0x10 */ s16 vertexCount;
    /* 0x14 */ void *vertices;
    /* 0x18 */ s16 normalCount;
    /* 0x1C */ void *normals;
    /* 0x20 */ s16 primitiveCount;
    /* 0x24 */ void *primitives;
    /* 0x28 */ void *vtx;
    /* 0x2C */ void *bspTree;
    /* 0x30 */ Skeleton *skeleton;
    /* 0x34 */ s32 extent;
    /* 0x38 */ s16 flags;
    /* 0x3A */ s16 index;
    /* 0x3C */ struct Object *next;
} Object;

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
} TrackSection;

typedef struct TrackData {
    /* 0x000 */ s32 vertexCount;
    /* 0x004 */ s32 faceCount;
    /* 0x008 */ s32 sectionCount;
    /* 0x00C */ s32 vtxCount;
    /* 0x010 */ Vector *vertices;
    /* 0x014 */ void *vtxBuf[2];
    /* 0x01C */ void *faces;
    /* 0x020 */ TrackSection *sections;
    /* 0x024 */ s32 unk24;
    /* 0x028 */ Skeleton **sectionSkeletons;
    /* 0x02C */ s32 skeletonCount;
    /* 0x030 */ Skeleton *skeletons[200];
    /* 0x350 */ void *vtxExtra;
    /* 0x354 */ s16 *viewList;
    /* 0x358 */ void *colCopy;
} TrackData;

typedef struct FrontendOptions {
    /* 0x00 */ s32 team;
    /* 0x04 */ u8 teamClass;
    /* 0x05 */ u8 unk05[7];
    /* 0x0C */ s32 trackNo;
    /* 0x10 */ u8 numPlayers;
} FrontendOptions;

extern u32 g_sqrtTable[];
extern Object **trackObjects;
extern TrackData *track;
extern FrontendOptions frontendOptions_g;
extern s32 g_farClip;

TrackSection *NextIndexSection(TrackSection *section);

s32 SquareRoot0(u32 a);
void SceneRadiusCheck(TrackSection *section, Object *objects);
void SceneRadiusCheck2(Object *objects);

#endif /* WIPEOUT64_RADSCN_H */
