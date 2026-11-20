#ifndef WIPEOUT64_PLASMA_H
#define WIPEOUT64_PLASMA_H

#include "common.h"
#include "dheap.h"
#include <ultra64.h>
#include <PR/gs2dex.h>

/*
 * Temporary US/EU symbol-address aliases until canonical names are added to
 * config/{us,eu}/symbol_addrs.txt.
 */
#ifdef VERSION_EU
#define InitPlasmaTable1        func_800518E0
#define InitPlasmaTable2        func_80051A34
#define InitPlasmaPalette       func_80051B9C
#define InitPlasmaPaletteGold   func_800520E8
#define DrawPlasma              func_800527FC
#define InitPlasma              func_80052934
#define UpdatePlasma            func_80052A60
#define RenderPlasma            func_80052CA0
#define InitInGamePlasmaPalette func_80052E4C
#define UpdateInGamePlasma      func_80053394

#define CheckGoldMedalsUnlocked func_80056E30
#define DrawFrontendBackdrop    func_8005B394

#define g_plasmaPhase0          D_800918A0
#define g_plasmaPhase1          D_800918A4
#define g_plasmaPhase2          D_800918A8
#define g_plasmaPhase3          D_800918AC
#define g_plasmaPhase4          D_800918B0
#define g_plasmaPhase5          D_800918B4
#define g_plasmaPhase6          D_800918B8
#define g_plasmaPhase7          D_800918BC
#define g_plasmaColorShift      D_800918C0
#define g_inGamePlasmaAngle     D_800918C4

#define g_inGamePlasmaPhase1    D_800C4970
#define g_inGamePlasmaPhase2    D_800C4972
#define g_inGamePlasmaPhase3    D_800C4974
#define g_inGamePlasmaPhase4    D_800C4976
#define g_inGamePlasmaPhase5    D_800C4978
#define g_plasmaSprite          D_800C4980
#define g_plasmaTable1          D_800C4998
#define g_plasmaTable2          D_800C499C
#define g_plasmaPalette         D_800C49A0
#define g_inGamePlasmaTexture   D_800C49A8
#define g_inGamePlasmaPalette   D_800C51A8
#define g_plasmaTexture         D_800C53A8
#else
#define InitPlasmaTable1        func_8004FB50
#define InitPlasmaTable2        func_8004FCA4
#define InitPlasmaPalette       func_8004FE0C
#define InitPlasmaPaletteGold   func_80050358
#define DrawPlasma              func_80050A6C
#define InitPlasma              func_80050BA4
#define UpdatePlasma            func_80050CD0
#define RenderPlasma            func_80050F10
#define InitInGamePlasmaPalette func_800510BC
#define UpdateInGamePlasma      func_80051604

#define CheckGoldMedalsUnlocked func_800550A8
#define DrawFrontendBackdrop    func_8005960C

#define g_plasmaPhase0          D_800922F0
#define g_plasmaPhase1          D_800922F4
#define g_plasmaPhase2          D_800922F8
#define g_plasmaPhase3          D_800922FC
#define g_plasmaPhase4          D_80092300
#define g_plasmaPhase5          D_80092304
#define g_plasmaPhase6          D_80092308
#define g_plasmaPhase7          D_8009230C
#define g_plasmaColorShift      D_80092310
#define g_inGamePlasmaAngle     D_80092314

#define g_inGamePlasmaPhase1    D_800C4630
#define g_inGamePlasmaPhase2    D_800C4632
#define g_inGamePlasmaPhase3    D_800C4634
#define g_inGamePlasmaPhase4    D_800C4636
#define g_inGamePlasmaPhase5    D_800C4638
#define g_plasmaSprite          D_800C4640
#define g_plasmaTable1          D_800C4658
#define g_plasmaTable2          D_800C465C
#define g_plasmaPalette         D_800C4660
#define g_inGamePlasmaTexture   D_800C4668
#define g_inGamePlasmaPalette   D_800C4E68
#define g_plasmaTexture         D_800C5068
#endif

#define G_CC_PLASMA 0, 0, 0, TEXEL0, TEXEL0, 0, PRIMITIVE, 0

/* Partial view of frontend.c's options struct (only the fields plasma.c reads). */
typedef struct PlasmaFrontendOptions {
    /* 0x00 */ u8 pad00[0x60];
    /* 0x60 */ s32 unk60;
    /* 0x64 */ s32 unk64;
    /* 0x68 */ u8 goldUnlocked;
} PlasmaFrontendOptions;

extern PlasmaFrontendOptions frontendOptions_g;

extern f32 g_plasmaPhase0;
extern f32 g_plasmaPhase1;
extern f32 g_plasmaPhase2;
extern f32 g_plasmaPhase3;
extern f32 g_plasmaPhase4;
extern f32 g_plasmaPhase5;
extern f32 g_plasmaPhase6;
extern f32 g_plasmaPhase7;
extern s32 g_plasmaColorShift;
extern s16 g_inGamePlasmaAngle;

extern s16 g_inGamePlasmaPhase1;
extern s16 g_inGamePlasmaPhase2;
extern s16 g_inGamePlasmaPhase3;
extern s16 g_inGamePlasmaPhase4;
extern s16 g_inGamePlasmaPhase5;
extern uSprite g_plasmaSprite;
extern u8 *g_plasmaTable1;
extern u8 *g_plasmaTable2;
extern u16 *g_plasmaPalette;
extern u8 g_inGamePlasmaTexture[64 * 32];
extern u16 g_inGamePlasmaPalette[256];
extern u8 *g_plasmaTexture;

s16 rsin(s16 angle);
u8 CheckGoldMedalsUnlocked(s32 a0, s32 a1);
Gfx *DrawFrontendBackdrop(s32 a0, s32 a1, s32 a2, s32 a3, s32 w, s32 h, s32 mode, Gfx *glistp);

void InitPlasmaTable1(void);
void InitPlasmaTable2(void);
void InitPlasmaPalette(void);
void InitPlasmaPaletteGold(void);
void DrawPlasma(s32 x1, s32 y1, s32 x2, s32 y2, s32 x3, s32 y3, s32 x4, s32 y4, s32 colorShift);
void InitPlasma(void);
void UpdatePlasma(void);
Gfx *RenderPlasma(Gfx *glistp);
void InitInGamePlasmaPalette(void);
void UpdateInGamePlasma(s32 mode);

#endif /* WIPEOUT64_PLASMA_H */
