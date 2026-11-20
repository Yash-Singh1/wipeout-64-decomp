#ifndef WIPEOUT64_PAUSE_H
#define WIPEOUT64_PAUSE_H

#include "common.h"
#include <ultra64.h>

/*
 * Temporary US/EU symbol-address aliases for check.py until canonical names
 * are added to config/{us,eu}/symbol_addrs.txt.
 */
#ifdef VERSION_EU
#define GetCdTrackName      func_80070EB0
#define GetCdTrackArtist    func_80070ECC
#define InGameReset         func_80070EE8
#define InGameConfirm       func_80071208
#define DeSelectScreen      func_800714A8

#define CentreText          func_80034244
#define AddText             func_800330E8
#define SetVolume           func_8001BFC4
#define GetMenuKey          func_8002EC5C
#define PlayNote            func_8001BCE8
#define DrawScreenOverlay   func_8004B430

#define g_cdTrackNames      D_80096030
#define g_cdTrackArtists    D_80096058
#define g_pauseOpenSteps    D_80096082
#define g_pauseCloseSteps   D_80096083
#define g_pausePrevChoice   D_80096085
#define g_pauseBoxClosing   D_80096086
#define g_pauseBoxOpen      D_80096088
#define g_pauseBoxClosed    D_8009608A
#define g_ambientVoice1     D_800968AA
#define g_gameType          D_800B7604
#define g_pauseBoxWidth     D_800F2120
#define g_pauseBoxHeight    D_800F2122
#define g_pauseBoxX         D_800F2124
#define g_pauseBoxY         D_800F2126
#else
#define GetCdTrackName      func_8006EF10
#define GetCdTrackArtist    func_8006EF2C
#define InGameReset         func_8006EF48
#define InGameConfirm       func_8006F268
#define DeSelectScreen      func_8006F508

#define CentreText          func_8002CB40
#define AddText             func_8002B9E4
#define SetVolume           func_80015014
#define GetMenuKey          func_800275BC
#define PlayNote            func_80014D38
#define DrawScreenOverlay   func_8004370C

#define g_cdTrackNames      D_80096A80
#define g_cdTrackArtists    D_80096AA8
#define g_pauseOpenSteps    D_80096AD2
#define g_pauseCloseSteps   D_80096AD3
#define g_pausePrevChoice   D_80096AD5
#define g_pauseBoxClosing   D_80096AD6
#define g_pauseBoxOpen      D_80096AD8
#define g_pauseBoxClosed    D_80096ADA
#define g_ambientVoice1     D_800972FA
#define g_gameType          D_800AA024
#define g_pauseBoxWidth     D_800F05E0
#define g_pauseBoxHeight    D_800F05E2
#define g_pauseBoxX         D_800F05E4
#define g_pauseBoxY         D_800F05E6
#endif

#define GAME_TYPE_LEAGUE    3

#define MENU_KEY_LEFT       2
#define MENU_KEY_RIGHT      3
#define MENU_KEY_SELECT     5

#define PAUSE_STAGE_NONE        0
#define PAUSE_STAGE_REQUESTED   2

#define PAUSE_AUDIO         3

typedef struct PauseState {
    s32 selection;
    s32 cdPause;
    u8 unk08[8];
    u32 frameCount;
    u8 unk14[3];
    u8 stage;
    u8 confirmed;
    u8 confirmChoice;
} PauseState;

extern char *g_cdTrackNames[10];
extern char *g_cdTrackArtists[10];

extern u8 g_pauseOpenSteps;
extern u8 g_pauseCloseSteps;
extern u8 g_pausePrevChoice;
extern u8 g_pauseBoxClosing;
extern s16 g_pauseBoxOpen;
extern s16 g_pauseBoxClosed;

extern s16 g_ambientVoice1;
extern s32 g_gameType;

extern s16 g_pauseBoxWidth;
extern s16 g_pauseBoxHeight;
extern s16 g_pauseBoxX;
extern s16 g_pauseBoxY;

Gfx *CentreText(const char *text, s32 x, s32 y, s32 size, s32 colour, Gfx *glistp);
Gfx *AddText(const char *text, s32 x, s32 y, s32 size, s32 colour, Gfx *glistp);
void SetVolume(s16 voice, s32 volume);
s16 GetMenuKey(s16 key, u16 pad);
void PlayNote(s32 sound, s32 volume, s32 lrPan, s32 fbPan, s32 pitch);
Gfx *DrawScreenOverlay(s32 player, s32 fullScreen, Gfx *glistp, u8 isDim);

char *GetCdTrackName(s32 track);
char *GetCdTrackArtist(s32 track);
Gfx *InGameReset(PauseState *pause, void *game, void *polys, u16 pad, Gfx *glistp);
Gfx *InGameConfirm(u8 *confirmed, s32 mode, s16 pad, u8 *selection, s32 *frameCount, Gfx *glistp);
Gfx *DeSelectScreen(void *sel, Gfx *glistp, s32 player);

#endif /* WIPEOUT64_PAUSE_H */
