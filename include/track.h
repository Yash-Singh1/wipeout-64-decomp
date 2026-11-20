#ifndef WIPEOUT64_TRACK_H
#define WIPEOUT64_TRACK_H

#include "common.h"
#include "dheap.h"
#include "error.h"
#include "skeleton.h"
#include "wadfile.h"
#include "n64render.h"
#include <ultra64.h>

#ifndef Warning
#define Warning 0
#endif
#ifndef NonFatal
#define NonFatal 1
#endif
#ifndef Fatal
#define Fatal 2
#endif

/* Face.flags */
#define TRACK_BASE        0x01
#define WEAPON_GRID_LEFT  0x02
#define FLIP              0x04
#define WEAPON_GRID_RIGHT 0x08
#define START_GRID        0x10
#define SPEED_UP          0x20
#define PIT_LANE          0x40
#define CHECKPOINT        0x80

/* TrackSection.flags */
#define Jump              0x01
#define JunctionEnd       0x08
#define JunctionStart     0x10
#define Junction          0x20
#define TrackPitStart     0x40

/*
 * Temporary US/EU symbol mappings until config/{us,eu}/symbol_addrs.txt
 * registers canonical names for track.c functions and external symbols.
 */
#ifdef VERSION_EU
#define IndexToPointers          func_8001BFD0
#define FindSectionSkeletonCentre func_8001C090
#define NewTrackSkeleton         func_8001C544
#define CheckSectionInRange      func_8001C5A8
#define BuildSectionVertexRuns   func_8001C950
#define ClearTrackVtxFlags       func_8001CB28
#define InitVertices             func_8001CBB4
#define InitFaces                func_8001D058
#define SortVertexList           func_8001D1F8
#define InitSections             func_8001D254
#define InitTrackDisplayLists    func_8001D3CC
#define InitViewList             func_8001D49C
#define DrawTrackSection         func_8001D670
#define DrawTrackSections        func_8001DD40
#define DrawNewTrack             func_8001E66C
#define FlushTrackRenderList     func_8001EF64
#define GetFaceVertex            func_8001F06C
#define RGBToHSV                 func_8001F0C8

/* External functions */
#define ModifyVertexUV           func_8001187C
#define UpdateCameraRaceStart    func_80022038
#define TrackDirection           func_80030120
#define GammaCorrectColor        func_800513D0

/* External globals */
#define g_pitStartSections       D_8008A478
#define g_camSecIdx              D_8008A4D0
#define g_trackBufIdx            D_8008A4D4
#define g_trackVtxCursor         D_8008A4D8
#define g_highlightFace          D_80091890
#define g_mtxPool                D_800AE4C0
#define g_mtxIndex               D_800AE514
#define CameraUpdate             D_800AE624
#define CameraUpdate2            D_800B33F8
#define ownShip                  D_800B3414
#define gameData                 D_800B74E0
#define ownShip2                 D_800B756C
#define ownShip3                 D_800B7570
#define ownShip4                 D_800B7574
#define pauseRequests            D_800B75F0
#define g_attractMode            D_800B7660
#define ships                    D_800B767C
#define g_trackTriCount          D_800B8424
#define g_sectionVertexInfo      D_800B8428
#define g_trackDListPtr          D_800B842C
#define g_trackDLists            D_800B8430
#define g_trackDListUsed         D_800B8438
#define g_trackDListPct          D_800B843C
#define g_trackDListSize         D_800B8440
#define g_trackVtxAllocCount     D_800B8448
#define g_trackRenderList        D_800C4818
#define cameras                  D_800CBE30

#define TRACK_LINE(n)            ((n) + 1)
#else
#define CheckVersion             func_80015020
#define IndexToPointers          func_800150BC
#define FindSectionSkeletonCentre func_8001517C
#define NewTrackSkeleton         func_80015630
#define CheckSectionInRange      func_80015694
#define BuildSectionVertexRuns   func_80015A3C
#define ClearTrackVtxFlags       func_80015C14
#define InitVertices             func_80015CA0
#define InitFaces                func_80016144
#define SortVertexList           func_800162E4
#define InitSections             func_80016340
#define InitTrackDisplayLists    func_800164C0
#define InitViewList             func_80016590
#define DrawTrackSection         func_80016764
#define DrawTrackSections        func_80016E34
#define DrawNewTrack             func_80017760
#define FlushTrackRenderList     func_80018024
#define GetFaceVertex            func_8001812C
#define RGBToHSV                 func_80018188

/* External functions */
#define ModifyVertexUV           func_8000AA5C
#define TrackDirection           func_80028A10
#define GammaCorrectColor        func_8004F63C

/* External globals */
#define g_pitStartSections       D_8008AE48
#define g_camSecIdx              D_8008AEA0
#define g_trackBufIdx            D_8008AEA4
#define g_trackVtxCursor         D_8008AEA8
#define g_raceStartCamera        D_8008FAD8
#define g_highlightFace          D_800922E0
#define g_playerFinished         D_8009BA18
#define g_mtxPool                D_800A0EE0
#define g_mtxIndex               D_800A0F34
#define ownShip                  D_800A5E2C
#define gameData                 D_800A9F00
#define ownShip2                 D_800A9F8C
#define ownShip3                 D_800A9F90
#define ownShip4                 D_800A9F94
#define pauseRequests            D_800AA010
#define g_attractMode            D_800AA088
#define ships                    D_800AA0A4
#define g_trackTriCount          D_800AAE54
#define g_sectionVertexInfo      D_800AAE58
#define g_trackDListPtr          D_800AAE5C
#define g_trackDLists            D_800AAE60
#define g_trackDListUsed         D_800AAE68
#define g_trackDListPct          D_800AAE6C
#define g_trackDListSize         D_800AAE70
#define g_trackVtxAllocCount     D_800AAE78
#define g_trackRenderList        D_800C44D8
#define cameras                  D_800CBAF0

#define TRACK_LINE(n)            (n)
#endif

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

typedef struct SVector {
    /* 0x00 */ s16 vx;
    /* 0x02 */ s16 vy;
    /* 0x04 */ s16 vz;
} SVector;

typedef struct Colors {
    /* 0x00 */ u8 red;
    /* 0x01 */ u8 green;
    /* 0x02 */ u8 blue;
} Colors;

typedef struct Face {
    /* 0x00 */ s16 vertex[4];
    /* 0x08 */ SVector normal;
    /* 0x0E */ u8 texture;
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
    /* 0x014 */ Vtx *vtxBuf[2];
    /* 0x01C */ Face *faces;
    /* 0x020 */ TrackSection *sections;
    /* 0x024 */ s32 unk24;
    /* 0x028 */ Skeleton **sectionSkeletons;
    /* 0x02C */ s32 skeletonCount;
    /* 0x030 */ Skeleton *skeletons[200];
    /* 0x350 */ Vtx *vtxExtra;
    /* 0x354 */ s16 *viewList;
    /* 0x358 */ Colors *colCopy;
} TrackData; /* size = 0x35C */

typedef struct SectionVertexInfo {
    /* 0x00 */ s32 totalVerts;
    /* 0x04 */ s32 runs[4]; /* [count0, start0, count1, start1] */
} SectionVertexInfo; /* size = 0x14 */

typedef struct FrontendOptions {
    /* 0x00 */ s32 team;
    /* 0x04 */ u8 teamClass;
    /* 0x05 */ u8 unk05[7];
    /* 0x0C */ s32 trackNo;
    /* 0x10 */ u8 numPlayers;
} FrontendOptions;

typedef struct ConfigData {
    /* 0x00 */ s32 initMagic;
    /* 0x04 */ u8 trackNum;
    /* 0x05 */ u8 actTrackNum;
    /* 0x06 */ s8 shipNum;
    /* 0x07 */ s8 ship2Num;
    /* 0x08 */ s8 shipClass;
} ConfigData;

typedef struct TrackCamera {
    /* 0x00 */ Skeleton *camPos;
    /* 0x04 */ TrackSection *section;
    /* 0x08 */ s16 hdg, pitch, roll;
    /* 0x0E */ s16 vhdg, vpitch, vroll;
    /* 0x14 */ SVector vel;
    /* 0x1A */ s16 pad;
} TrackCamera; /* size = 0x1C */

typedef struct ShipData {
    /* 0x000 */ u8 unk00[0xB0];
    /* 0x0B0 */ u16 combatAttr;
    /* 0x0B2 */ u8 unkB2[0x108 - 0xB2];
} ShipData; /* size = 0x108 */

typedef void (*CameraUpdateFunc)();

extern TrackData *track;
extern FrontendOptions frontendOptions_g;
extern ConfigData gameData;
extern s16 g_pitStartSections[];
extern s32 g_camSecIdx;
extern s32 g_trackBufIdx;
extern s32 g_trackVtxCursor;
extern s32 g_highlightFace;
extern Mtx *g_mtxPool;
extern s32 g_mtxIndex;
extern s32 ownShip;
extern s32 ownShip2;
extern s32 ownShip3;
extern s32 ownShip4;
extern s32 pauseRequests[4];
extern s32 g_attractMode;
extern ShipData *ships;
extern s32 g_trackTriCount;
extern SectionVertexInfo *g_sectionVertexInfo;
extern Gfx *g_trackDListPtr;
extern Gfx *g_trackDLists[2];
extern s32 g_trackDListUsed;
extern s32 g_trackDListPct;
extern s32 g_trackDListSize;
extern s32 g_trackVtxAllocCount;
extern RenderList g_trackRenderList;
extern TrackCamera cameras[4];

#ifdef VERSION_EU
extern CameraUpdateFunc CameraUpdate;
extern CameraUpdateFunc CameraUpdate2;
void UpdateCameraRaceStart();
#else
extern s32 g_raceStartCamera;
extern u8 g_playerFinished[4];
#endif

s32 SquareRoot0(u32 a);
Gfx *ModifyVertexUV(Gfx *dl, s32 vtxIdx, s32 u, s32 v, s32 texIdx);
u8 TrackDirection(TrackCamera *camera, s32 *angleOut);
u8 GammaCorrectColor(u8 c);

#ifndef VERSION_EU
void CheckVersion(void);
#endif
void IndexToPointers(void);
Vector *FindSectionSkeletonCentre(TrackSection *section);
Skeleton *NewTrackSkeleton(Vector *pos);
s32 CheckSectionInRange(TrackSection *section, Vector *centre);
void BuildSectionVertexRuns(TrackSection *section, SectionVertexInfo *info);
void ClearTrackVtxFlags(void);
void InitVertices(WADFILE *wad);
void InitFaces(WADFILE *wad);
void SortVertexList(s32 *list, s32 count);
void InitSections(WADFILE *wad);
void InitTrackDisplayLists(void);
void InitViewList(WADFILE *wad);
TrackSection *NextIndexSection(TrackSection *section);
void DrawTrackSection(TrackSection *section, Skeleton *camPos, s16 count);
s32 DrawTrackSections(TrackSection *section, Skeleton *camPos, s32 count, s16 dir,
                      s32 angle, TrackCamera *camera, s32 skipJunction);
Gfx *DrawNewTrack(TrackCamera *camera, s32 playerIdx, Gfx *glistp);
Gfx *FlushTrackRenderList(RenderList *list, Gfx *glistp);
void GetFaceVertex(TrackSection *section, Face *face, s32 vertex, Vector *out);
void RGBToHSV(Colors *rgb, Colors *hsv);

#endif /* WIPEOUT64_TRACK_H */
