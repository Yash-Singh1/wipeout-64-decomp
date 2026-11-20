#ifndef WIPEOUT64_GFX2D_H
#define WIPEOUT64_GFX2D_H

#include "common.h"
#include <ultra64.h>
#include <PR/sched.h>
#include <PR/gs2dex.h>

#ifdef VERSION_EU
#define GLIST_LEN 0x900
#else
#define GLIST_LEN 0x880
#endif

#define RDP_FIFO_LEN 0x80

/* libultra 2.0H value (2.0I headers say 0xC00). */
#define YIELD_DATA_SIZE 0x900

/* Fields of the per-frame graphics buffer that gfx2d.c touches; the rest is not mapped yet. */
typedef struct GFXInfo {
    /* 0x000 */ u8 pad000[0x2D8];
    /* 0x2D8 */ OSScTask task[2];
    /* 0x3A8 */ Gfx dp[GLIST_LEN];
    /* 0x47A8 / 0x4BA8 (EU) */ u8 pad47A8[0x3300];
    /* 0x7AA8 / 0x7EA8 */ OSScMsg msg;
    /* 0x7AC8 / 0x7EC8 */ u8 pad7AC8[0xE0];
    /* 0x7BA8 / 0x7FA8 */ u16 *cfb;
    /* 0x7BAC / 0x7FAC */ u8 pad7BAC[0x8];
    /* 0x7BB4 / 0x7FB4 */ u32 flags;
} GFXInfo;

/* Only the fields of frontend.c's options that gfx2d.c reads. */
typedef struct {
    /* 0x00 */ u8 pad00[0x10];
    /* 0x10 */ u8 numPlayers;
    /* 0x11 */ u8 pad11[0x7];
    /* 0x18 */ s32 ownShip[4];
    /* 0x28 */ s32 screenLayout[4];
} FrontendOptions;

/* Only the field of the 0x108-byte ship record that gfx2d.c reads. */
typedef struct {
    /* 0x000 */ u8 pad000[0x10];
    /* 0x010 */ s32 attr;
    /* 0x014 */ u8 pad014[0xF4];
} ShipData;

/* Racefunc.c state polled before drawing the race overlay. */
typedef struct {
    /* 0x00 */ u8 pad00[0x12];
    /* 0x12 */ s16 unk12;
    /* 0x14 */ s32 unk14;
    /* 0x18 */ s32 unk18;
} RaceOverlayState;

extern OSMesgQueue g_schedMsgQ;
extern FrontendOptions frontendOptions_g;

/* RDP and RSP state display lists in the static segment (segment 1). */
extern Gfx D_10001B0[];
extern Gfx D_10001D8[];
#define g_rspInitDL D_10001B0
#define g_rdpInitDL D_10001D8

/* Temporary names until these are added to config/{us,eu}/symbol_addrs.txt. */
extern long long int n_aspMainTextStart[];
#define rspbootTextEnd n_aspMainTextStart

#ifdef VERSION_EU
#define g_gfxTaskTemplate   D_80091450
#define g_staticSegment     D_800A69C4
#define g_globalDL          D_800CBC50
#define ships               D_800B767C
#define g_hudState          D_800A9270
#define g_hudShowPause      D_800A9294
#define g_shipEffectA       D_800B8540
#define g_shipEffectB       D_800B8C20
#define g_hudTimers         D_800A9440
#define g_hudLaps           D_800AE628
#define g_raceOverlay       D_800B74E0
#define g_raceOverlayMode   D_800B7550
#define g_logoSprite        D_800CE1E0
#define g_playerMessage     D_800BACC0
#define g_rdpFifo           D_800A6FB0
#define g_yieldData         D_800A5DC0
#define g_dramStack         D_800F7FF0
#define g_orthoMtx          D_800BA640
#define g_identityMtx       D_800BA680
#define g_zBuffer           D_800F8400
#define DrawShipEffect      func_8000CF48
#define DrawPlayerHud       func_80034A54
#define DrawLargeText       func_800330E8
#define DrawText            func_8003D388
#define DrawRaceOverlay     func_80070924
#define DrawRaceMessages    func_80070898
#define DrawWeaponMessage   func_8004B430
#define RenderPlasma        func_80052CA0
#define RenderFrontend      func_80054030
#define DrawSplitScreenBorders  func_8003C250
#define Gfx2DRaceTask       func_8003C454
#define Gfx2DMenuTask       func_8003C930
#define Gfx2DInitDisplayList func_8003CB88
#else
#define g_gfxTaskTemplate   D_80091E20
#define g_staticSegment     D_800993E4
#define g_globalDL          D_800CB910
#define ships               D_800AA0A4
#define g_hudState          D_8009BC90
#define g_hudShowPause      D_8009BCB4
#define g_shipEffectA       D_800AAF70
#define g_shipEffectB       D_800AB650
#define g_hudTimers         D_8009BE60
#define g_hudLaps           D_800A1048
#define g_raceOverlay       D_800A9F00
#define g_raceOverlayMode   D_800A9F70
#define g_logoSprite        D_800CDEA0
#define g_playerMessage     D_800AD6F0
#define g_rdpFifo           D_800999D0
#define g_yieldData         D_800987E0
#define g_dramStack         D_800FF0E0
#define g_orthoMtx          D_800AD070
#define g_identityMtx       D_800AD0B0
#define g_zBuffer           D_800FF500
#define DrawShipEffect      func_80049094
#define DrawPlayerHud       func_8002D350
#define DrawLargeText       func_8002B9E4
#define DrawText            func_80035C98
#define DrawRaceOverlay     func_8006E984
#define DrawRaceMessages    func_8006E8F8
#define DrawWeaponMessage   func_8004370C
#define RenderPlasma        func_80050F10
#define RenderFrontend      func_800522A0
#define DrawSplitScreenBorders  func_80034B60
#define Gfx2DRaceTask       func_80034D64
#define Gfx2DMenuTask       func_80035240
#define Gfx2DInitDisplayList func_80035498
#endif

extern OSTask g_gfxTaskTemplate;
extern void *g_staticSegment;
extern Gfx g_globalDL[];
extern ShipData *ships;
extern u8 g_hudState[];
extern u8 g_hudShowPause;
extern u8 g_shipEffectA[];
extern u8 g_shipEffectB[];
extern u8 g_hudTimers[];
extern u8 g_hudLaps[];
extern RaceOverlayState g_raceOverlay;
extern s32 g_raceOverlayMode;
extern uSprite g_logoSprite;
extern u8 g_playerMessage[];
extern u64 g_rdpFifo[RDP_FIFO_LEN];
extern u64 g_yieldData[YIELD_DATA_SIZE / sizeof(u64)];
extern u64 g_dramStack[SP_DRAM_STACK_SIZE64];
extern Mtx g_orthoMtx;
extern Mtx g_identityMtx;
extern u16 g_zBuffer[];

void __assert(const char *expr, const char *file, s32 line);

Gfx *DrawShipEffect(void *a, void *b, ShipData *ship, s32 screenLayout, Gfx *glist);
Gfx *DrawPlayerHud(ShipData *ships, void *timers, void *laps, s32 mode, RaceOverlayState *overlay,
                   void *hud, s32 player, s32 screenLayout, Gfx *glist);
Gfx *DrawLargeText(char *text, s32 x, s32 y, s32 size, s32 colour, Gfx *glist);
Gfx *DrawText(char *text, s32 x, s32 y, s32 size, Gfx *glist);
Gfx *DrawRaceOverlay(Gfx *glist);
Gfx *DrawRaceMessages(Gfx *glist);
Gfx *DrawWeaponMessage(s32 player, s32 a1, Gfx *glist, s32 a3);
Gfx *RenderPlasma(Gfx *glist);
Gfx *RenderFrontend(Gfx *glist);

Gfx *DrawSplitScreenBorders(Gfx *glist);
void Gfx2DRaceTask(Gfx *glistp, GFXInfo *i);
void Gfx2DMenuTask(Gfx *glistp, GFXInfo *i, s32 plasma);
void Gfx2DInitDisplayList(Gfx *glistp, s32 mode);

#endif /* WIPEOUT64_GFX2D_H */
