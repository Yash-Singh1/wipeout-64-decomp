#ifndef WIPEOUT64_HISCORES_H
#define WIPEOUT64_HISCORES_H

#include "common.h"
#include "error.h"
#include "pad.h"

#ifdef VERSION_EU
#define HISCORE_FRAME_RATE  50
#else
#define HISCORE_FRAME_RATE  60
#endif

#define HISCORE_MAGIC           0x25345327
#define HISCORE_LAP_RECORD      1
#define HISCORE_RACE_RECORD     2

#define GAME_TYPE_TIME_TRIAL    1
#define GAME_TYPE_CHALLENGE     6

#define MENU_KEY_SELECT         5

#define ALPHA_DELETE            26
#define ALPHA_END               27

/*
 * Temporary US/EU symbol-address aliases for check.py until canonical names
 * are added to config/{us,eu}/symbol_addrs.txt.
 */
#ifdef VERSION_EU
#define ScoreTabInit        func_8006CD60
#define InitScoreTabData    func_8006CFD8
#define CheckForHiScore     func_8006D050
#define DisplayScoreTab     func_8006D330
#define EnterHighScore      func_8006DA64

#define PlayNote            func_8001BCE8
#define GetMenuKey          func_8002EC5C
#define AddTime             func_800328F0
#define TimeToString        func_80033028
#define AddText             func_800330E8
#define AddDelEnd           func_80033B68
#define CentreText          func_80034244
#define DrawFontText        func_8003D388

#define g_hiScoreLoaded     D_80089FCA
#define g_enteringHiScore   D_80089FFC
#define g_classLaps         D_80091410
#define g_oldRaceTime       D_80095FD0
#define g_oldLapTime        D_80095FD4
#define g_nameStored        D_80095FD8
#define g_firstHiScoreInit  D_80095FDC
#define g_scoreTabFrames    D_80095FE0
#define g_raceCompleteFlag  D_800A9040
#define g_savedLapTimes     D_800AE6A8
#define ownShip             D_800B3414
#define g_gameType          D_800B7604
#define g_myBestLap         D_800B7634
#define g_myBestRace        D_800B76B0
#define g_font12            D_800B98F0
#define g_oldRaceTimeStr    D_800F1A00
#define g_oldLapTimeStr     D_800F1A08
#define g_hiScoreTable      D_800F1A10
#else
#define ScoreTabInit        func_8006AD10
#define InitScoreTabData    func_8006AF78
#define CheckForHiScore     func_8006AFF0
#define DisplayScoreTab     func_8006B2D0
#define EnterHighScore      func_8006BA04

#define PlayNote            func_80014D38
#define GetMenuKey          func_800275BC
#define AddTime             func_8002B1E0
#define TimeToString        func_8002B91C
#define AddText             func_8002B9E4
#define AddDelEnd           func_8002C464
#define CentreText          func_8002CB40
#define DrawFontText        func_80035C98

#define g_hiScoreLoaded     D_8008A99A
#define g_enteringHiScore   D_8008A9CC
#define g_classLaps         D_80091DE0
#define g_oldRaceTime       D_80096A20
#define g_oldLapTime        D_80096A24
#define g_nameStored        D_80096A28
#define g_firstHiScoreInit  D_80096A2C
#define g_scoreTabFrames    D_80096A30
#define g_raceCompleteFlag  D_8009BA60
#define g_savedLapTimes     D_800A10C8
#define ownShip             D_800A5E2C
#define g_gameType          D_800AA024
#define g_myBestLap         D_800AA05C
#define g_myBestRace        D_800AA0DC
#define g_font12            D_800AC320
#define g_oldRaceTimeStr    D_800EFEC0
#define g_oldLapTimeStr     D_800EFEC8
#define g_hiScoreTable      D_800EFED0
#endif

typedef struct FrontendOptions {
    /* 0x00 */ s32 team;
    /* 0x04 */ u8 teamClass;
    /* 0x08 */ u32 trackFile;
    /* 0x0C */ s32 trackNo;
    /* 0x10 */ u8 numPlayers;
    /* 0x11 */ u8 pad11[3];
    /* 0x14 */ u8 unk14;
    /* 0x15 */ u8 challengeMedal;
    /* 0x18 */ s32 ownShips[4];
    /* 0x28 */ s32 screenLayout[4];
    /* 0x38 */ s32 playerController[4];
} FrontendOptions;

typedef struct ConfigData {
    /* 0x00 */ s32 initMagic;
    /* 0x04 */ u8 trackNum;
    /* 0x05 */ u8 actTrackNum;
    /* 0x06 */ s8 shipNum;
    /* 0x07 */ s8 ship2Num;
    /* 0x08 */ s8 shipClass;
    /* 0x09 */ u8 unk09[0x1A - 0x09];
    /* 0x1A */ s16 complete;
    /* 0x1C */ s16 unk1C;
    /* 0x1E */ s16 unk1E;
} ConfigData;

typedef struct ShipData {
    /* 0x000 */ u8 unk00[0xCA];
    /* 0x0CA */ s8 lap;
    /* 0x0CB */ s8 reverseLap;
    /* 0x0CC */ s16 nearSecNo;
    /* 0x0CE */ s16 prevSecNo;
    /* 0x0D0 */ s32 lapTimes[5];
    /* 0x0E4 */ s32 updateCount;
    /* 0x0E8 */ s32 padE8;
    /* 0x0EC */ s32 startY;
    /* 0x0F0 */ s32 eliminations;
    /* 0x0F4 */ u8 unkF4[0x108 - 0xF4];
} ShipData;

typedef struct HiScore {
    /* 0x00 */ char name[4];
    /* 0x04 */ s32 time;
    /* 0x08 */ u8 ship;
    /* 0x09 */ char lapName[4];
    /* 0x10 */ s32 lapTime;
    /* 0x14 */ u8 lapShip;
    /* 0x18 */ s32 eliminations;
} HiScore; /* size = 0x1C */

typedef struct HiScoreData {
    /* 0x00 */ u8 nameEntered[9];
    /* 0x09 */ u8 selectedChar;
    /* 0x0A */ s16 alphabetIndex;
    /* 0x0C */ s16 nameCursor;
    /* 0x0E */ u8 dispChar[2];
    /* 0x10 */ s32 cursorBlinkFrames;
    /* 0x14 */ u8 enteringName;
    /* 0x15 */ u8 firstAlphabetIndex;
    /* 0x16 */ u8 storeRecords;
    /* 0x17 */ u8 pad17;
    /* 0x18 */ s32 finishDelayFrames;
    /* 0x1C */ s32 inputDelayFrames;
    /* 0x20 */ s32 totalTime;
    /* 0x24 */ u8 highScore;
    /* 0x25 */ u8 displayHiScore;
    /* 0x26 */ s16 nameEndX;
    /* 0x28 */ u8 confirm;
    /* 0x29 */ u8 lConfirm;
    /* 0x2A */ u8 pad2A[6];
    /* 0x30 */ s32 raceTime;
    /* 0x34 */ u8 racePilot;
    /* 0x35 */ u8 pad35[7];
    /* 0x3C */ s32 lapTime;
    /* 0x40 */ u8 lapPilot;
    /* 0x41 */ u8 pad41[3];
    /* 0x44 */ s32 raceEliminations;
    /* 0x48 */ u8 pendingRecords;
    /* 0x49 */ u8 displayedRecords;
} HiScoreData;

typedef struct FontGlyph {
    /* 0x00 */ u16 unk00;
    /* 0x02 */ u16 unk02;
    /* 0x04 */ u16 width;
    /* 0x06 */ u8 unk06[0x20 - 0x06];
} FontGlyph;

extern FrontendOptions frontendOptions_g;
extern u8 g_hiScoreLoaded;
extern s32 g_enteringHiScore;
extern u8 g_classLaps[4];
extern s32 g_oldRaceTime;
extern s32 g_oldLapTime;
extern s32 g_nameStored;
extern s32 g_firstHiScoreInit;
extern s32 g_scoreTabFrames;
extern u8 g_raceCompleteFlag;
extern s32 g_savedLapTimes[5][4];
extern s32 ownShip;
extern s32 g_gameType;
extern s32 g_myBestLap;
extern s32 g_myBestRace;
extern FontGlyph g_font12[];
extern char g_oldRaceTimeStr[8];
extern char g_oldLapTimeStr[8];
extern HiScore g_hiScoreTable[16][4];

void PlayNote(s32 sound, s32 volume, s32 lrPan, s32 fbPan, s32 pitch);
s16 GetMenuKey(s16 key, u16 pad);
Gfx *AddTime(s32 time, s32 x, s32 y, s32 size, s32 colour, u8 showDecimal, Gfx *glistp);
void TimeToString(s32 time, s32 flag, char *out);
Gfx *AddText(const char *text, s32 x, s32 y, s32 size, s32 colour, Gfx *glistp);
Gfx *AddDelEnd(u8 icon, s32 x, s32 y, s32 size, s32 colour, Gfx *glistp);
Gfx *CentreText(const char *text, s32 x, s32 y, s32 size, s32 colour, Gfx *glistp);
Gfx *DrawFontText(const char *text, s32 x, s32 y, s32 size, Gfx *glistp);

void ScoreTabInit(ConfigData *config);
void InitScoreTabData(HiScoreData *score);
void CheckForHiScore(ConfigData *config, HiScoreData *score, ShipData *ships, s32 shipNum, u8 lap);
Gfx *DisplayScoreTab(ConfigData *config, HiScoreData *score, u16 pad, Gfx *glistp);
Gfx *EnterHighScore(ConfigData *config, HiScoreData *score, u16 pad, Gfx *glistp);

#endif /* WIPEOUT64_HISCORES_H */
