#ifndef WIPEOUT64_INIT_H
#define WIPEOUT64_INIT_H

#include "common.h"
#include "dheap.h"
#include "error.h"
#include "files.h"
#include "n64render.h"
#include "radscn.h"
#include "wadfile.h"

#define Clut4Bit 0
#define Clut8Bit 1
#define CLUT4    0
#define CLUT8    1
#define ClutType(tim) ((tim)->flags & 7)

#define TextureTableCount g_unk8008A970

/*
 * Temporary US/EU symbol aliases until these functions and globals are added
 * to config/{us,eu}/symbol_addrs.txt.
 */
#ifdef VERSION_EU
#define WADInitScene                     func_80016600
#define LoadCmpFiles                     func_80016994
#define FlipTexture                      func_80016A8C
#define NewLoadTexture                   func_80016C48
#define ConvertPalette                   func_80017274
#define LoadCompressedTextureSequence    func_800172FC
#define WADLoadCompressedTextureSequence func_80017534
#define GetTextureMasks                  func_8001771C
#define BuildTextureDisplayList          func_800177A8

#define SwapNibbles                      func_80010B08
#define WADLoadPrm                       func_80011278
#define InitTrack2Anim                   func_80050A68
#define InitTrack3Anim                   func_8004F5BC
#define InitTrack4Anim                   func_8004FBD8
#define InitTrack5Anim                   func_80050D70
#define InitTrack6Anim                   func_80050200

#define g_texMemTotal                    D_8008A0E0
#define g_multiplayerFlag                D_80095F44
#define g_scenePrmIndex                  D_80095F48
#define g_scene2PrmIndex                 D_80095F4C
#define numTex                           D_800A94B8
#define sharedPrimTable                  D_800A94C0
#define errorString                      D_800AE520
#define timList                          D_800B76F0
#define wadTimList                       D_800B7D40
#define g_sceneRenderList                D_800C4860
#define g_skyRenderList                  D_800C48D8
#define g_scene2RenderList               D_800C48F0
#else
#define WADInitScene                     func_8000F7E0
#define LoadCmpFiles                     func_8000FB74
#define FlipTexture                      func_8000FC6C
#define NewLoadTexture                   func_8000FE28
#define ConvertPalette                   func_80010454
#define LoadCompressedTextureSequence    func_800104DC
#define WADLoadCompressedTextureSequence func_80010714
#define GetTextureMasks                  func_800108FC
#define BuildTextureDisplayList          func_80010988

#define SwapNibbles                      func_80009CE8
#define WADLoadPrm                       func_8000A458
#define InitTrack2Anim                   func_8004EC9C
#define InitTrack3Anim                   func_8004D7F0
#define InitTrack4Anim                   func_8004DE0C
#define InitTrack5Anim                   func_8004EFA4
#define InitTrack6Anim                   func_8004E434

#define g_texMemTotal                    D_8008AAB0
#define g_multiplayerFlag                D_80096994
#define g_scenePrmIndex                  D_80096998
#define g_scene2PrmIndex                 D_8009699C
#define numTex                           D_8009BED8
#define sharedPrimTable                  D_8009BEE0
#define errorString                      D_800A0F40
#define timList                          D_800AA120
#define wadTimList                       D_800AA770
#define g_sceneRenderList                D_800C4520
#define g_skyRenderList                  D_800C4598
#define g_scene2RenderList               D_800C45B0
#endif

typedef struct ConfigData ConfigData;

typedef struct Tim {
    /* 0x00 */ s32 id;
    /* 0x04 */ s32 flags;
} Tim;

typedef struct TimClut4 {
    /* 0x00 */ s32 id;
    /* 0x04 */ s32 flags;
    /* 0x08 */ s32 clutBytes;
    /* 0x0C */ s16 clutX;
    /* 0x0E */ s16 clutY;
    /* 0x10 */ s16 clutW;
    /* 0x12 */ s16 clutH;
    /* 0x14 */ u16 clut[16];
    /* 0x34 */ s32 textureBytes;
    /* 0x38 */ s16 textureX;
    /* 0x3A */ s16 textureY;
    /* 0x3C */ s16 textureW;
    /* 0x3E */ s16 textureH;
} TimClut4;

typedef struct TimClut8 {
    /* 0x000 */ s32 id;
    /* 0x004 */ s32 flags;
    /* 0x008 */ s32 clutBytes;
    /* 0x00C */ s16 clutX;
    /* 0x00E */ s16 clutY;
    /* 0x010 */ s16 clutW;
    /* 0x012 */ s16 clutH;
    /* 0x014 */ u16 clut[256];
    /* 0x214 */ s32 textureBytes;
    /* 0x218 */ s16 textureX;
    /* 0x21A */ s16 textureY;
    /* 0x21C */ s16 textureW;
    /* 0x21E */ s16 textureH;
} TimClut8;

typedef struct TIMlist {
    /* 0x000 */ char *memBase;
    /* 0x004 */ s32 memOffset[400];
} TIMlist;

typedef struct CompressedTextureSequence {
    /* 0x00 */ s32 textureCount;
    /* 0x04 */ s32 textureSize[1];
} CompressedTextureSequence;

extern s16 g_unk8008A970;
extern s32 g_texMemTotal;
extern s32 g_multiplayerFlag;
extern s32 g_scenePrmIndex;
extern s32 g_scene2PrmIndex;
extern s32 numTex;
extern Object *sharedPrimTable[128][40];
extern char errorString[256];
extern TIMlist timList;
extern TIMlist wadTimList;
extern RenderList g_sceneRenderList;
extern RenderList g_skyRenderList;
extern RenderList g_scene2RenderList;

int sprintf(char *dst, const char *fmt, ...);
void ExpandData(u8 *input, u8 *output);
void SwapNibbles(u8 *bytePtr);
Object *WADLoadPrm(WADFILE *wad, RenderList *renderList);
void InitTrack2Anim(void);
void InitTrack3Anim(void);
void InitTrack4Anim(void);
void InitTrack5Anim(void);
void InitTrack6Anim(void);

s16 WADInitScene(Object **prm, s16 *prmCount, Skeleton *camPos, ConfigData *gameData, WADFILE *wad);
void LoadCmpFiles(TIMlist *timPtr);
void FlipTexture(N64Texture *texture);
N64Texture *NewLoadTexture(Tim *timPtr, s16 translucent, s32 scene);
void ConvertPalette(u16 *pal, s32 count);
TIMlist *LoadCompressedTextureSequence(u32 fileOffset);
TIMlist *WADLoadCompressedTextureSequence(WADFILE *wad);
void GetTextureMasks(N64Texture *texture, s32 *maskS, s32 *maskT);
void BuildTextureDisplayList(N64Texture *texture);

#endif /* WIPEOUT64_INIT_H */
