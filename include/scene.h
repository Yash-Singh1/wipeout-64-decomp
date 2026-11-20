#ifndef WIPEOUT64_SCENE_H
#define WIPEOUT64_SCENE_H

#include "common.h"
#include "dheap.h"
#include "error.h"
#include "files.h"
#include "gte.h"
#include "n64render.h"
#include "radscn.h"
#include "skeleton.h"
#include "wadfile.h"
#include <ultra64.h>

/*
 * Temporary US/EU symbol-name mappings for symbols not yet named in
 * config/{us,eu}/symbol_addrs.txt. Remove once canonical names are added there.
 */
#ifdef VERSION_EU
#define SwapNibbles               func_80010B08
#define ScaleNormals              func_80010B24
#define LoadPrmCommon             func_80010C90
#define WADLoadPrm                func_80011278
#define LoadPrm                   func_8001129C
#define BuildObjectVertices       func_800112C8
#define BuildInstanceDisplayLists func_800113B0
#define NewInstanceWithDlist      func_800114B4
#define NewInstance               func_800116A4
#define ModifyVertexST            func_8001187C
#define CountObjectTextures       func_800118E4
#define InitObjectRenderGroup     func_80011A20
#define CompareObjectName         func_80011BB4
#define BuildObjectDisplayList    func_80011BF0
#define PolyUsesTexture           func_80014810
#define AdvancePoly               func_800148C4
#define CountTextureVertices      func_800149B8
#define CountTexturePolys         func_80014B7C
#define NormaliseVector3f         func_80014C7C
#define CalcFaceNormal            func_80014D3C
#define CalcVertexNormals         func_80014EB8
#define BuildEnvMapDisplayList    func_80015104

#define RegisterTrack1RotObj      func_8004F500
#define RegisterTrack2BasherObj   func_800509CC
#define RegisterTrack2FartObj     func_800508DC
#define RegisterTrack3CartObj     func_8004F5EC
#define RegisterTrack3SteamObj    func_8004F654
#define RegisterTrack5FlameObj    func_80050CC8
#define RegisterTrack6ClawObj     func_80050704
#define RegisterTrack6SteamObj    func_8004FFD8

#define g_maxTextureVerts         D_80089600
#define g_envMapTex               D_80089618
#define g_goldLight               D_80089E18
#define g_silverLight             D_80089E30
#define g_bronzeLight             D_80089E48
#define g_animTextureCount        D_80089FA2
#define errorString               D_800AE520
#else
#define SwapNibbles               func_80009CE8
#define ScaleNormals              func_80009D04
#define LoadPrmCommon             func_80009E70
#define WADLoadPrm                func_8000A458
#define LoadPrm                   func_8000A47C
#define BuildObjectVertices       func_8000A4A8
#define BuildInstanceDisplayLists func_8000A590
#define NewInstanceWithDlist      func_8000A694
#define NewInstance               func_8000A884
#define ModifyVertexST            func_8000AA5C
#define CountObjectTextures       func_8000AAC4
#define InitObjectRenderGroup     func_8000AC00
#define CompareObjectName         func_8000AD94
#define BuildObjectDisplayList    func_8000ADD0
#define PolyUsesTexture           func_8000D9F0
#define AdvancePoly               func_8000DAA4
#define CountTextureVertices      func_8000DB98
#define CountTexturePolys         func_8000DD5C
#define NormaliseVector3f         func_8000DE5C
#define CalcFaceNormal            func_8000DF1C
#define CalcVertexNormals         func_8000E098
#define BuildEnvMapDisplayList    func_8000E2E4

#define RegisterTrack1RotObj      func_8004D734
#define RegisterTrack2BasherObj   func_8004EC00
#define RegisterTrack2FartObj     func_8004EB10
#define RegisterTrack3CartObj     func_8004D820
#define RegisterTrack3SteamObj    func_8004D888
#define RegisterTrack5FlameObj    func_8004EEFC
#define RegisterTrack6ClawObj     func_8004E938
#define RegisterTrack6SteamObj    func_8004E20C

#define g_maxTextureVerts         D_80089FD0
#define g_envMapTex               D_80089FE8
#define g_goldLight               D_8008A7E8
#define g_silverLight             D_8008A800
#define g_bronzeLight             D_8008A818
#define g_animTextureCount        D_8008A972
#define errorString               D_800A0F40
#endif

/* PRM primitive type IDs */
#define TypeF3            1
#define TypeFT3           2
#define TypeF4            3
#define TypeFT4           4
#define TypeG3            5
#define TypeGT3           6
#define TypeG4            7
#define TypeGT4           8
#define TypeLF2           9
#define TypeTSPR          10
#define TypeBSPR          11
#define TypeLSF3          12
#define TypeLSFT3         13
#define TypeLSF4          14
#define TypeLSFT4         15
#define TypeLSG3          16
#define TypeLSGT3         17
#define TypeLSG4          18
#define TypeLSGT4         19
#define TypeSpline        20
#define TypeInfiniteLight 21
#define TypePointLight    22
#define TypeSpotLight     23

/* Primitive flags */
#define ShipEngine        0x0001
#define Translucent       0x0002
#define TileUVMask        0x0078

typedef struct CVECTOR {
    /* 0x00 */ u8 r;
    /* 0x01 */ u8 g;
    /* 0x02 */ u8 b;
    /* 0x03 */ u8 cd;
} CVECTOR;

typedef struct Primitive {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
} Primitive;

typedef struct F3 {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
    /* 0x04 */ s16 coords[3];
    /* 0x0A */ s16 pad1;
    /* 0x0C */ CVECTOR colour;
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
    /* 0x18 */ CVECTOR colour;
} FT3; /* size = 0x1C */

typedef struct F4 {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
    /* 0x04 */ s16 coords[4];
    /* 0x0C */ CVECTOR colour;
} F4; /* size = 0x10 */

typedef struct FT4 {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
    /* 0x04 */ s16 coords[4];
    /* 0x0C */ s16 texture;
    /* 0x0E */ s16 cba;
    /* 0x10 */ s16 tsb;
    /* 0x12 */ u8 u0, v0;
    /* 0x14 */ u8 u1, v1;
    /* 0x16 */ u8 u2, v2;
    /* 0x18 */ u8 u3, v3;
    /* 0x1A */ s16 pad1;
    /* 0x1C */ CVECTOR colour;
} FT4; /* size = 0x20 */

typedef struct G3 {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
    /* 0x04 */ s16 coords[3];
    /* 0x0A */ s16 pad1;
    /* 0x0C */ CVECTOR colour[3];
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
    /* 0x18 */ CVECTOR colour[3];
} GT3; /* size = 0x24 */

typedef struct G4 {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
    /* 0x04 */ s16 coords[4];
    /* 0x0C */ CVECTOR colour[4];
} G4; /* size = 0x1C */

typedef struct GT4 {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
    /* 0x04 */ s16 coords[4];
    /* 0x0C */ s16 texture;
    /* 0x0E */ s16 cba;
    /* 0x10 */ s16 tsb;
    /* 0x12 */ u8 u0, v0;
    /* 0x14 */ u8 u1, v1;
    /* 0x16 */ u8 u2, v2;
    /* 0x18 */ u8 u3, v3;
    /* 0x1A */ s16 pad1;
    /* 0x1C */ CVECTOR colour[4];
} GT4; /* size = 0x2C */

typedef struct SPR {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
    /* 0x04 */ s16 coord;
    /* 0x06 */ s16 width;
    /* 0x08 */ s16 height;
    /* 0x0A */ s16 texture;
    /* 0x0C */ CVECTOR colour;
} SPR; /* size = 0x10 */

typedef struct LSF3 {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
    /* 0x04 */ s16 coords[3];
    /* 0x0A */ s16 normal;
    /* 0x0C */ CVECTOR colour;
} LSF3; /* size = 0x10 */

typedef struct LSFT3 {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
    /* 0x04 */ s16 coords[3];
    /* 0x0A */ s16 normal;
    /* 0x0C */ s16 texture;
    /* 0x0E */ s16 cba;
    /* 0x10 */ s16 tsb;
    /* 0x12 */ u8 u0, v0;
    /* 0x14 */ u8 u1, v1;
    /* 0x16 */ u8 u2, v2;
    /* 0x18 */ s16 pad1;
    /* 0x1A */ CVECTOR colour;
} LSFT3; /* size = 0x1E */

typedef struct LSF4 {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
    /* 0x04 */ s16 coords[4];
    /* 0x0C */ s16 normal;
    /* 0x0E */ s16 pad1;
    /* 0x10 */ CVECTOR colour;
} LSF4; /* size = 0x14 */

typedef struct LSFT4 {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
    /* 0x04 */ s16 coords[4];
    /* 0x0C */ s16 normal;
    /* 0x0E */ s16 texture;
    /* 0x10 */ s16 cba;
    /* 0x12 */ s16 tsb;
    /* 0x14 */ u8 u0, v0;
    /* 0x16 */ u8 u1, v1;
    /* 0x18 */ u8 u2, v2;
    /* 0x1A */ u8 u3, v3;
    /* 0x1C */ s16 pad1;
    /* 0x1E */ CVECTOR colour;
} LSFT4; /* size = 0x22 */

typedef struct LSG3 {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
    /* 0x04 */ s16 coords[3];
    /* 0x0A */ s16 normals[3];
    /* 0x10 */ CVECTOR colour[3];
} LSG3; /* size = 0x1C */

typedef struct LSGT3 {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
    /* 0x04 */ s16 coords[3];
    /* 0x0A */ s16 normals[3];
    /* 0x10 */ s16 texture;
    /* 0x12 */ s16 cba;
    /* 0x14 */ s16 tsb;
    /* 0x16 */ u8 u0, v0;
    /* 0x18 */ u8 u1, v1;
    /* 0x1A */ u8 u2, v2;
    /* 0x1C */ s16 pad1;
    /* 0x1E */ CVECTOR colour[3];
} LSGT3; /* size = 0x2A */

typedef struct LSG4 {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
    /* 0x04 */ s16 coords[4];
    /* 0x0C */ s16 normals[4];
    /* 0x14 */ CVECTOR colour[4];
} LSG4; /* size = 0x24 */

typedef struct LSGT4 {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
    /* 0x04 */ s16 coords[4];
    /* 0x0C */ s16 normals[4];
    /* 0x14 */ s16 texture;
    /* 0x16 */ s16 cba;
    /* 0x18 */ s16 tsb;
    /* 0x1A */ u8 u0, v0;
    /* 0x1C */ u8 u1, v1;
    /* 0x1E */ u8 u2, v2;
    /* 0x20 */ u8 u3, v3;
    /* 0x22 */ s16 pad1;
    /* 0x24 */ CVECTOR colour[4];
} LSGT4; /* size = 0x34 */

typedef struct Spline {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
    /* 0x04 */ VECTOR control1;
    /* 0x14 */ VECTOR position;
    /* 0x24 */ VECTOR control2;
    /* 0x34 */ CVECTOR colour;
} Spline; /* size = 0x38 */

typedef struct InfiniteLight {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
    /* 0x04 */ SVECTOR direction;
    /* 0x0C */ CVECTOR colour;
} InfiniteLight; /* size = 0x10 */

typedef struct PointLight {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
    /* 0x04 */ VECTOR position;
    /* 0x14 */ CVECTOR colour;
    /* 0x18 */ s16 startFalloff;
    /* 0x1A */ s16 endFalloff;
} PointLight; /* size = 0x1C */

typedef struct SpotLight {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
    /* 0x04 */ VECTOR position;
    /* 0x14 */ SVECTOR direction;
    /* 0x1C */ CVECTOR colour;
    /* 0x20 */ s16 startFalloff;
    /* 0x22 */ s16 endFalloff;
    /* 0x24 */ s16 coneAngle;
    /* 0x26 */ s16 spreadAngle;
} SpotLight; /* size = 0x28 */

typedef union Prm {
    char *ptr;
    Object *object;
    Skeleton *skeleton;
    SVECTOR *svector;
    Primitive *primitive;
    F3 *f3;
    FT3 *ft3;
    F4 *f4;
    FT4 *ft4;
    G3 *g3;
    GT3 *gt3;
    G4 *g4;
    GT4 *gt4;
    LSF3 *lsf3;
    LSFT3 *lsft3;
    LSF4 *lsf4;
    LSFT4 *lsft4;
    LSG3 *lsg3;
    LSGT3 *lsgt3;
    LSG4 *lsg4;
    LSGT4 *lsgt4;
    SPR *spr;
    Spline *spline;
    PointLight *pointLight;
    SpotLight *spotLight;
    InfiniteLight *infiniteLight;
} Prm;

typedef struct VtxState {
    /* 0x00 */ u8 r;
    /* 0x01 */ u8 g;
    /* 0x02 */ u8 b;
    /* 0x03 */ u8 pad;
    /* 0x04 */ s16 s;
    /* 0x06 */ s16 t;
} VtxState; /* size = 0x08 */

typedef struct Vec3f {
    /* 0x00 */ f32 x;
    /* 0x04 */ f32 y;
    /* 0x08 */ f32 z;
} Vec3f;

extern DHeapBlock *g_heapA;
extern s32 g_maxTextureVerts;
extern u16 g_envMapTex[32 * 32];
extern Lights1 g_goldLight;
extern Lights1 g_silverLight;
extern Lights1 g_bronzeLight;
extern s16 g_animTextureCount;
extern char errorString[256];

int sprintf(char *dst, const char *fmt, ...);
void __assert(const char *expr, const char *file, s32 line);

void RegisterTrack1RotObj(Object *obj);
void RegisterTrack2BasherObj(Object *obj);
void RegisterTrack2FartObj(Object *obj);
void RegisterTrack3CartObj(Object *obj, s32 index);
void RegisterTrack3SteamObj(Object *obj, s32 index);
void RegisterTrack5FlameObj(Object *obj);
void RegisterTrack6ClawObj(Object *obj, s32 index);
void RegisterTrack6SteamObj(Object *obj, s32 index);

void SwapEndian32(void *ptr);
void SwapEndian16(void *ptr);
void SwapNibbles(u8 *bytePtr);
void ScaleNormals(Object *object);
Object *LoadPrmCommon(WADFILE *wad, u32 fileOffset, RenderList *renderList);
Object *WADLoadPrm(WADFILE *wad, RenderList *renderList);
Object *LoadPrm(u32 fileOffset, RenderList *renderList);
Vtx *BuildObjectVertices(Object *object);
void BuildInstanceDisplayLists(Object *dst, Object *src, RenderList *renderList);
Object *NewInstanceWithDlist(Object *source, RenderList *renderList);
Object *NewInstance(Object *source);
Gfx *ModifyVertexST(Gfx *dl, s32 vtx, s32 u, s32 v, s32 texIdx);
s32 CountObjectTextures(Object *object, RenderList *renderList);
void InitObjectRenderGroup(Object *object, s32 texCount, s32 hasUntextured, RenderList *renderList);
s32 CompareObjectName(const char *s1, const char *s2, s32 len);
void BuildObjectDisplayList(Object *object, Vtx *verts, RenderGroupEntry *rData, RenderList *renderList, s32 unused);
s32 PolyUsesTexture(Primitive *poly, s32 texture);
Primitive *AdvancePoly(Primitive *poly);
s32 CountTextureVertices(Object *object, s32 texture);
s32 CountTexturePolys(Object *object, s32 texture);
void NormaliseVector3f(Vec3f *v);
void CalcFaceNormal(s32 i0, s32 i1, s32 i2, Vtx *verts, Vec3f *out);
void CalcVertexNormals(Object *object, Vtx *verts);
void BuildEnvMapDisplayList(Object *object, s32 a1, s32 a2, s32 lightType, Vtx **vtxOut, Gfx **dlOut);

#endif /* WIPEOUT64_SCENE_H */
