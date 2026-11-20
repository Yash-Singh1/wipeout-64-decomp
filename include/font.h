#ifndef WIPEOUT64_FONT_H
#define WIPEOUT64_FONT_H

#include "common.h"
#include "n64render.h"
#include "wadfile.h"
#include <ultra64.h>

/*
 * Temporary US/EU symbol mappings until config/{us,eu}/symbol_addrs.txt
 * registers canonical names for font.c functions and external symbols.
 */
#ifdef VERSION_EU
#define GetClassName                    func_8003CD40
#define GetTrackName                    func_8003CDB8
#define DrawButtonPrompt                func_8003CE4C
#define AddNumber                       func_8003D220
#define AddText                         func_8003D388
#define AddTextYellow                   func_8003D60C
#define AddCentreText                   func_8003D8A4
#define AddRightText                    func_8003DB38
#define FontStub_800364C0               func_8003DBB0
#define LoadFontStartupTextures         func_8003DBB8

#define RegisterTextureSequence         func_80016994
#define LoadCompressedTextureSequence   func_800172FC
#define WADLoadCompressedTextureSequence func_80017534

#define g_fontStartTex                  D_8009149C
#define g_menuScreen                    D_8009193C
#define g_menuSprites                   D_8009195C
#define g_hudTexStart                   D_800B3404
#define g_spriteCount                   D_800B7610
#define g_textEndX                      D_800B9328
#define g_buttonATex                    D_800BA6C0
#define g_buttonBTex                    D_800BA6C4
#define g_arrowLeftSprite               D_800BA6C8
#define g_button15Sprite                D_800BA6E0
#define g_fontTexture                   D_800BA6F8
#define g_button14Sprite                D_800BA700
#define g_arrowRightSprite              D_800BA718
#define g_arrowLeftTex                  D_800BA730
#define g_button15Tex                   D_800BA734
#define g_fontSprites                   D_800BA738
#define g_button14Tex                   D_800BAC78
#define g_buttonASprite                 D_800BAC80
#define g_buttonBSprite                 D_800BAC98
#define g_arrowRightTex                 D_800BACB0
#else
#define GetClassName                    func_80035650
#define GetTrackName                    func_800356C8
#define DrawButtonPrompt                func_8003575C
#define AddNumber                       func_80035B30
#define AddText                         func_80035C98
#define AddTextYellow                   func_80035F1C
#define AddCentreText                   func_800361B4
#define AddRightText                    func_80036448
#define FontStub_800364C0               func_800364C0
#define LoadFontStartupTextures         func_800364C8

#define RegisterTextureSequence         func_8000FB74
#define LoadCompressedTextureSequence   func_800104DC
#define WADLoadCompressedTextureSequence func_80010714

#define g_fontStartTex                  D_80091E6C
#define g_menuScreen                    D_8009238C
#define g_menuSprites                   D_800923AC
#define g_hudTexStart                   D_800A5E24
#define g_spriteCount                   D_800AA034
#define g_textEndX                      D_800ABD58
#define g_buttonATex                    D_800AD0F0
#define g_buttonBTex                    D_800AD0F4
#define g_arrowLeftSprite               D_800AD0F8
#define g_button15Sprite                D_800AD110
#define g_fontTexture                   D_800AD128
#define g_button14Sprite                D_800AD130
#define g_arrowRightSprite              D_800AD148
#define g_arrowLeftTex                  D_800AD160
#define g_button15Tex                   D_800AD164
#define g_fontSprites                   D_800AD168
#define g_button14Tex                   D_800AD6A8
#define g_buttonASprite                 D_800AD6B0
#define g_buttonBSprite                 D_800AD6C8
#define g_arrowRightTex                 D_800AD6E0
#endif

#define _hudTexturesFile                D_355160

#define FONT_NUM_GLYPHS                 56
#define FONT_GLYPHS_PER_ROW             13
#define FONT_CHAR_SIZE                  6

extern WADFILE *g_startupImage;
extern s16 g_unk8008A970;
extern u8 _startupDataFile[];
extern u8 _hudTexturesFile[];

extern s32 g_fontStartTex;
extern s32 g_menuScreen;
extern uSprite *g_menuSprites;
extern s16 g_hudTexStart;
extern s32 g_spriteCount;
extern s32 g_textEndX;

extern N64Texture *g_fontTexture;
extern uSprite g_fontSprites[FONT_NUM_GLYPHS];
extern N64Texture *g_arrowLeftTex;
extern uSprite g_arrowLeftSprite;
extern N64Texture *g_arrowRightTex;
extern uSprite g_arrowRightSprite;
extern N64Texture *g_buttonATex;
extern uSprite g_buttonASprite;
extern N64Texture *g_buttonBTex;
extern uSprite g_buttonBSprite;
extern N64Texture *g_button15Tex;
extern uSprite g_button15Sprite;
extern N64Texture *g_button14Tex;
extern uSprite g_button14Sprite;

int sprintf(char *dst, const char *fmt, ...);

void *WADLoadCompressedTextureSequence(WADFILE *wad);
void *LoadCompressedTextureSequence(u32 fileOffset);
void RegisterTextureSequence(void *texTable);

char *GetClassName(s16 classIdx);
char *GetTrackName(s16 trackIdx);
Gfx *DrawButtonPrompt(s16 x, s16 y, s16 type, char *text, Gfx *glistp);
Gfx *AddNumber(s32 num, s32 x, s32 y, s32 size, s32 colour, Gfx *glistp);
Gfx *AddText(char *text, s32 x, s32 y, s32 size, Gfx *glistp);
Gfx *AddTextYellow(char *text, s32 x, s32 y, s32 size, Gfx *glistp);
Gfx *AddCentreText(char *text, s32 x, s32 y, s32 size, Gfx *glistp);
Gfx *AddRightText(char *text, s32 x, s32 y, s32 size, Gfx *glistp);
void FontStub_800364C0(void);
void LoadFontStartupTextures(void);
void InitFont(void);

#endif /* WIPEOUT64_FONT_H */
