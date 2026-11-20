#ifndef WIPEOUT64_HUD_H
#define WIPEOUT64_HUD_H

#include "common.h"

/*
 * TEMPORARY: radscn.h only declares FrontendOptions up to numPlayers. The HUD reads
 * further fields, so rename radscn.h's declaration and declare the full layout below.
 */
#define FrontendOptions FrontendOptionsHead
#define frontendOptions_g frontendOptionsHead_g
#include "radscn.h"
#undef FrontendOptions
#undef frontendOptions_g

#define InitFontSize InitFontSize_n64init
#include "n64init.h"
#undef InitFontSize
#include <ultra64.h>

/*
 * Temporary US/EU symbol-address aliases for check.py until canonical names
 * are added to config/{us,eu}/symbol_addrs.txt.
 */
#ifdef VERSION_EU
/* HUD functions (defined in src/hud.c) */
#define InitSpeedoSprites                func_80031BA0
#define FillSpeedo                       func_80032148
#define AddTime                          func_800328F0
#define TimeToString                     func_80033028
#define AddText                          func_800330E8
#define AddNumber                        func_800333EC
#define NumberToString                   func_8003371C
#define AddJustifyNumber                 func_80033784
#define AddDelEnd                        func_80033B68
#define CentreText                       func_80034244
#define CentreTextAt                     func_80034648
#define DrawPlayerHud                    func_80034A54
#define GetShipPosition                  func_80038108
#define copyInTempLapTimes               func_80038464
#define DisplayRaceInfo                  func_80038524
#define AlterCheckPoint                  func_8003B0A4
#define DisplayContinues                 func_8003B944

/* External functions */
#define LoadCmpFiles                     func_80016994
#define WADLoadCompressedTextureSequence func_80017534
#define PlayNote                         func_8001BCE8
#define UpdateCameraAttractInternal      func_80022CBC
#define UpdateCameraAttractMode          func_80023158
#define GetAng                           func_80023CB4
#define GetMagnitude                     func_80023EE4
#define UpdatePlayerShipStart            func_800271A4
#define UpdatePlayerShip                 func_800275E4
#define GetDroneShield                   func_80027FE8
#define UpdateDroneShip                  func_80028220
#define humanToDroneSpeed                func_8002AC0C
#define UpdateRescueDroid                func_8002F1C0
#define DrawButtonPrompt                 func_8003CE4C
#define DrawFontNumber                   func_8003D220
#define DrawFontText                     func_8003D388
#define DrawFontTextYellow               func_8003D60C
#define AddCentreText                    func_8003D8A4
#define InitFontUnused                func_8003DBB0
#define GetBlinkState                    func_8005DD5C
#define UpdateBlink                      func_8005DD70
#define CheckForHiScore                  func_8006D050
#define InitGhostShip                    func_800709B0
#define StoreGhostReplay                 func_80070DC0
#define GetCdTrackName                   func_80070EB0
#define GetCdTrackArtist                 func_80070ECC
#define SetSfxFreqOffset                 func_8007206C
#define MusStartEffect                   func_8007360C
#define MusHandleSetVolume               func_80073984

/* External globals */
#define g_demoShip                       D_80089FA4
#define g_raceModeFlag                   D_80089FB2
#define startLinePos                     D_8008A478
#define g_infiniteTimeFlag               D_80091250
#define cheatTime                        D_80091257
#define g_prevLapRecord                  D_800912EC
#define g_ghostLapTime0                  D_800912F0
#define g_ghostBestLapTime               D_800912F4
#define g_prevSplitDelta                 D_800912F8
#define g_splitLapDelta                  D_800912FC
#define g_showSplitDelta                 D_80091300
#define g_ttLapCount                     D_80091304
#define g_ghostLoaded                    D_80091308
#define g_ghostEnabled                   D_8009130C
#define g_font16Widths                   D_80091310
#define g_font12Widths                   D_8009132C
#define g_font8Widths                    D_80091380
#define g_font3Widths                    D_800913D0
#define g_classLaps                      D_80091410
#define g_trackNameScroll                D_80091414
#define g_trackNameFade                  D_80091418
#define g_hudFlashState                  D_8009141C
#define g_shieldFlashFrame               D_80091420
#define g_hudFrameCount                  D_80091424
#define g_lapRecFlashTimer               D_80091428
#define g_sessionBestFlashTimer          D_8009142C
#define g_splitFlashTimer                D_80091430
#define g_challengeMedalLevel            D_80091434
#define g_raceInfoFrameCount             D_80091438
#define g_checkPointFlashFrame           D_8009143C
#define g_checkPointBannerTimer          D_80091440
#define g_raceInfoOverlayArg             D_80091498
#define rearLockCycle                    D_800914B0
#define g_perfectLapFlag                 D_800914B4
#define g_perfectLapTimer                D_800914B8
#define g_resultSfxPending                    D_800917F4
#define g_timeoutSfxPending                    D_800917F5
#define g_raceChallenges                 D_80094194
#define g_ttChallenges                   D_800942C8
#define g_weaponChallenges               D_800943FC
#define g_superComboChallenges           D_80094530
#define g_multiplayerFlag                D_80095F44
#define g_podiumShips                    D_80095F80
#define g_podiumLocked                   D_80095F8C
#define g_checkPoints                    D_800A8FC8
#define g_playerFinished                 D_800A8FF8
#define checkPointReached                D_800A9228
#define g_finishPosition                 D_800A9248
#define shipDamage                       D_800A92C0
#define g_font12TexStart                 D_800A9300
#define g_font3TexStart                  D_800AE4C4
#define g_font16TexStart                 D_800AE510
#define g_font8TexStart                  D_800AE518
#define CameraUpdate                     D_800AE624
#define numTrackSections                 D_800AE6A2
#define g_savedLapTimes                  D_800AE6A8
#define g_textureTable                   D_800B2778
#define CameraUpdate2                    D_800B33F8
#define CameraUpdate3                    D_800B33FC
#define CameraUpdate4                    D_800B3400
#define g_hudTexStart                    D_800B3404
#define attractDelayFrames               D_800B3408
#define ownShip                          D_800B3414
#define g_ghostReplay                    D_800B3418
#define g_shipDeathCount                 D_800B74A0
#define pauseData                        D_800B7550
#define ownShip2                         D_800B756C
#define ownShip3                         D_800B7570
#define ownShip4                         D_800B7574
#define g_ghostLapTimes                  D_800B75D0
#define pauseRequests                    D_800B75F0
#define attractMode                      D_800B7600
#define g_gameType                       D_800B7604
#define noCheckPoints                    D_800B7601
#define g_spriteCount                    D_800B7610
#define g_raceEndState                   D_800B7614
#define resetData                        D_800B7640
#define g_attractMode                    D_800B7660
#define playerPosition                   D_800B7682
#define g_continuesLeft                  D_800B76A8
#define player2Position                  D_800B76AA
#define player3Position                  D_800B76AC
#define player4Position                  D_800B76B4
#define g_raceInfoFontCtx                D_800B8B1C
#define g_lastCheckSfxTime               D_800B8D90
#define g_speedoMPShieldTex              D_800B8D98
#define g_speedoMPThrustTex              D_800B8D9C
#define g_font3                          D_800B8DA0
#define g_mpWeaponSpriteP2               D_800B92E0
#define g_mpWeaponSpriteP3               D_800B92F8
#define g_mpWeaponSpriteP4               D_800B9310
#define g_textEndX                       D_800B9328
#define g_delIconSprite                  D_800B9330
#define g_endIconSprite                  D_800B9348
#define g_font8                          D_800B9360
#define g_speedo1PWeaponTex              D_800B98A0
#define g_hudTempTex                     D_800B98A4
#define g_mpShieldSpriteP2               D_800B98A8
#define g_mpShieldSpriteP3               D_800B98C0
#define g_mpShieldSpriteP4               D_800B98D8
#define g_font12                         D_800B98F0
#define g_font16                         D_800B9E30
#define g_mpThrustSpriteP2               D_800BA370
#define g_mpThrustSpriteP3               D_800BA388
#define g_mpThrustSpriteP4               D_800BA3A0
#define g_speedoMPWeaponTex              D_800BA3B8
#define g_rearLockSprite                 D_800BA540
#define g_plusSprite                     D_800BA558
#define g_minusSprite                    D_800BA570
#define g_speedo1PShieldTex              D_800BA61C
#define g_speedo1PThrustTex              D_800BA620
#define checkTimer                       D_800CE1D0
#define checkTimer2                      D_800CE9F8
#define checkTimer3                      D_800CE9FC
#define checkTimer4                      D_800CEA00
#define g_hiScoreTable                   D_800F1A10
#else
/* HUD functions (defined in src/hud.c) */
#define InitSpeedoSprites                func_8002A490
#define FillSpeedo                       func_8002AA38
#define AddTime                          func_8002B1E0
#define TimeToString                     func_8002B91C
#define AddText                          func_8002B9E4
#define AddNumber                        func_8002BCE8
#define NumberToString                   func_8002C018
#define AddJustifyNumber                 func_8002C080
#define AddDelEnd                        func_8002C464
#define CentreText                       func_8002CB40
#define CentreTextAt                     func_8002CF44
#define DrawPlayerHud                    func_8002D350
#define GetShipPosition                  func_80030A1C
#define copyInTempLapTimes               func_80030D78
#define DisplayRaceInfo                  func_80030E38
#define AlterCheckPoint                  func_800339C0
#define DisplayContinues                 func_80034250

/* External functions */
#define LoadCmpFiles                     func_8000FB74
#define WADLoadCompressedTextureSequence func_80010714
#define PlayNote                         func_80014D38
#define UpdateCameraAttractInternal      func_8001BC94
#define UpdateCameraAttractMode          func_8001C130
#define GetAng                           func_8001CC94
#define GetMagnitude                     func_8001CEC4
#define UpdatePlayerShipStart            func_8001FF04
#define UpdatePlayerShip                 func_8002033C
#define GetDroneShield                   func_80020CF8
#define UpdateDroneShip                  func_80020F30
#define humanToDroneSpeed                func_80023768
#define UpdateRescueDroid                func_80027B20
#define DrawButtonPrompt                 func_8003575C
#define DrawFontNumber                   func_80035B30
#define DrawFontText                     func_80035C98
#define DrawFontTextYellow               func_80035F1C
#define AddCentreText                    func_800361B4
#define InitFontUnused                func_800364C0
#define GetBlinkState                    func_8005BFD4
#define UpdateBlink                      func_8005BFE8
#define CheckForHiScore                  func_8006AFF0
#define InitGhostShip                    func_8006EA10
#define StoreGhostReplay                 func_8006EE20
#define GetCdTrackName                   func_8006EF10
#define GetCdTrackArtist                 func_8006EF2C
#define SetSfxFreqOffset                 func_800700CC
#define MusStartEffect                   func_8007386C
#define MusHandleSetVolume               func_80073BE4

/* External globals */
#define g_demoShip                       D_8008A974
#define g_raceModeFlag                   D_8008A982
#define startLinePos                     D_8008AE48
#define g_infiniteTimeFlag               D_80091C20
#define cheatTime                        D_80091C27
#define g_prevLapRecord                  D_80091CBC
#define g_ghostLapTime0                  D_80091CC0
#define g_ghostBestLapTime               D_80091CC4
#define g_prevSplitDelta                 D_80091CC8
#define g_splitLapDelta                  D_80091CCC
#define g_showSplitDelta                 D_80091CD0
#define g_ttLapCount                     D_80091CD4
#define g_ghostLoaded                    D_80091CD8
#define g_ghostEnabled                   D_80091CDC
#define g_font16Widths                   D_80091CE0
#define g_font12Widths                   D_80091CFC
#define g_font8Widths                    D_80091D50
#define g_font3Widths                    D_80091DA0
#define g_classLaps                      D_80091DE0
#define g_trackNameScroll                D_80091DE4
#define g_trackNameFade                  D_80091DE8
#define g_hudFlashState                  D_80091DEC
#define g_shieldFlashFrame               D_80091DF0
#define g_hudFrameCount                  D_80091DF4
#define g_lapRecFlashTimer               D_80091DF8
#define g_sessionBestFlashTimer          D_80091DFC
#define g_splitFlashTimer                D_80091E00
#define g_challengeMedalLevel            D_80091E04
#define g_raceInfoFrameCount             D_80091E08
#define g_checkPointFlashFrame           D_80091E0C
#define g_checkPointBannerTimer          D_80091E10
#define g_raceInfoOverlayArg             D_80091E68
#define rearLockCycle                    D_80091E80
#define g_perfectLapFlag                 D_80091E84
#define g_perfectLapTimer                D_80091E88
#define g_resultSfxPending                    D_80092244
#define g_timeoutSfxPending                    D_80092245
#define g_raceChallenges                 D_80094BE4
#define g_ttChallenges                   D_80094D18
#define g_weaponChallenges               D_80094E4C
#define g_superComboChallenges           D_80094F80
#define g_multiplayerFlag                D_80096994
#define g_podiumShips                    D_800969D0
#define g_podiumLocked                   D_800969DC
#define g_checkPoints                    D_8009B9E8
#define g_playerFinished                 D_8009BA18
#define checkPointReached                D_8009BC48
#define g_finishPosition                 D_8009BC68
#define shipDamage                       D_8009BCE0
#define g_font12TexStart                 D_8009BD20
#define g_font3TexStart                  D_800A0EE4
#define g_font16TexStart                 D_800A0F30
#define g_font8TexStart                  D_800A0F38
#define CameraUpdate                     D_800A1044
#define numTrackSections                 D_800A10C2
#define g_savedLapTimes                  D_800A10C8
#define g_textureTable                   D_800A5198
#define CameraUpdate2                    D_800A5E18
#define CameraUpdate3                    D_800A5E1C
#define CameraUpdate4                    D_800A5E20
#define g_hudTexStart                    D_800A5E24
#define attractDelayFrames               D_800A5E28
#define ownShip                          D_800A5E2C
#define g_ghostReplay                    D_800A5E38
#define g_shipDeathCount                 D_800A9EC0
#define pauseData                        D_800A9F70
#define ownShip2                         D_800A9F8C
#define ownShip3                         D_800A9F90
#define ownShip4                         D_800A9F94
#define g_ghostLapTimes                  D_800A9FF0
#define pauseRequests                    D_800AA010
#define attractMode                      D_800AA020
#define g_gameType                       D_800AA024
#define noCheckPoints                    D_800AA028
#define g_spriteCount                    D_800AA034
#define g_raceEndState                   D_800AA038
#define resetData                        D_800AA068
#define g_attractMode                    D_800AA088
#define playerPosition                   D_800AA0C0
#define g_continuesLeft                  D_800AA0D4
#define player2Position                  D_800AA0D6
#define player3Position                  D_800AA0D8
#define player4Position                  D_800AA0E0
#define g_raceInfoFontCtx                D_800AB54C
#define g_lastCheckSfxTime               D_800AB7C0
#define g_speedoMPShieldTex              D_800AB7C8
#define g_speedoMPThrustTex              D_800AB7CC
#define g_font3                          D_800AB7D0
#define g_mpWeaponSpriteP2               D_800ABD10
#define g_mpWeaponSpriteP3               D_800ABD28
#define g_mpWeaponSpriteP4               D_800ABD40
#define g_textEndX                       D_800ABD58
#define g_delIconSprite                  D_800ABD60
#define g_endIconSprite                  D_800ABD78
#define g_font8                          D_800ABD90
#define g_speedo1PWeaponTex              D_800AC2D0
#define g_hudTempTex                     D_800AC2D4
#define g_mpShieldSpriteP2               D_800AC2D8
#define g_mpShieldSpriteP3               D_800AC2F0
#define g_mpShieldSpriteP4               D_800AC308
#define g_font12                         D_800AC320
#define g_font16                         D_800AC860
#define g_mpThrustSpriteP2               D_800ACDA0
#define g_mpThrustSpriteP3               D_800ACDB8
#define g_mpThrustSpriteP4               D_800ACDD0
#define g_speedoMPWeaponTex              D_800ACDE8
#define g_rearLockSprite                 D_800ACF70
#define g_plusSprite                     D_800ACF88
#define g_minusSprite                    D_800ACFA0
#define g_speedo1PShieldTex              D_800AD04C
#define g_speedo1PThrustTex              D_800AD050
#define checkTimer                       D_800CDE90
#define checkTimer2                      D_800CE6B8
#define checkTimer3                      D_800CE6BC
#define checkTimer4                      D_800CE6C0
#define g_hiScoreTable                   D_800EFED0
#endif

/* ============================================================================
 * Shared structures (PC/N64 HUD, race state, ship, ghost, and challenge tables)
 * ============================================================================ */

typedef struct N64Texture {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 clutSize;
    /* 0x04 */ s16 bpp;
    /* 0x06 */ s16 width;
    /* 0x08 */ s16 height;
    /* 0x0A */ s16 unk0A;
    /* 0x0C */ void *texels;
    /* 0x10 */ void *palette;
} N64Texture;

typedef struct VECTOR {
    /* 0x00 */ s32 vx;
    /* 0x04 */ s32 vy;
    /* 0x08 */ s32 vz;
    /* 0x0C */ s32 pad;
} VECTOR;

typedef struct SVECTOR {
    /* 0x00 */ s16 vx;
    /* 0x02 */ s16 vy;
    /* 0x04 */ s16 vz;
    /* 0x06 */ s16 pad;
} SVECTOR;

typedef struct Face {
    /* 0x00 */ s16 vertex[4];
    /* 0x08 */ s16 nx;
    /* 0x0A */ s16 ny;
    /* 0x0C */ s16 nz;
    /* 0x0E */ u8 texture;
    /* 0x0F */ u8 flags;
    /* 0x10 */ u8 red;
    /* 0x11 */ u8 green;
    /* 0x12 */ u8 blue;
    /* 0x13 */ u8 pad13;
} Face; /* size = 0x14 */

typedef struct CheckPoint {
    /* 0x00 */ s16 section;
    /* 0x02 */ s16 time;
    /* 0x04 */ s8 reached;
    /* 0x05 */ s8 pad05;
} CheckPoint; /* size = 0x06 */

typedef struct ConfigData {
    /* 0x00 */ s32 initMagic;
    /* 0x04 */ u8 trackNum;
    /* 0x05 */ u8 actTrackNum;
    /* 0x06 */ s8 shipNum;
    /* 0x07 */ s8 ship2Num;
    /* 0x08 */ s8 shipClass;
    /* 0x09 */ u8 unk09[0x12 - 0x09];
    /* 0x12 */ s16 raceInfo[4];
    /* 0x1A */ s16 complete;
    /* 0x1C */ s16 unk1C;
    /* 0x1E */ s16 highScore;
    /* 0x20 */ u8 unk20[0x2C - 0x20];
    /* 0x2C */ u8 paused;
    /* 0x2D */ u8 unk2D[0x3F - 0x2D];
    /* 0x3F */ u8 phantomChallenge;
    /* 0x40 */ u8 piranhaChallenge;
    /* 0x41 */ u8 unk41[3];
    /* 0x44 */ u8 phantomUnlocked;
    /* 0x45 */ u8 piranhaUnlocked;
    /* 0x46 */ u8 phantomCircuit;
    /* 0x47 */ u8 piranhaCircuit;
} ConfigData;

typedef struct ChallengeTarget {
    /* 0x00 */ u8 unk00[0x10];
    /* 0x10 */ f32 goldTarget1;
    /* 0x14 */ f32 goldTarget2;
    /* 0x18 */ f32 silverTarget1;
    /* 0x1C */ f32 silverTarget2;
    /* 0x20 */ f32 bronzeTarget1;
    /* 0x24 */ f32 bronzeTarget2;
    /* 0x28 */ u8 award;
    /* 0x29 */ u8 unk29[3];
} ChallengeTarget; /* size = 0x2C */

typedef struct FrontendOptions {
    /* 0x00 */ s32 team;
    /* 0x04 */ u8 teamClass;
    /* 0x05 */ u8 unk05[3];
    /* 0x08 */ u32 trackFile;
    /* 0x0C */ s32 trackNo;
    /* 0x10 */ u8 numPlayers;
    /* 0x11 */ u8 unk11[4];
    /* 0x15 */ u8 challengeMode;
    /* 0x16 */ u8 challengeIndex;
    /* 0x17 */ u8 unk17;
    /* 0x18 */ s32 ownShips[4];
    /* 0x28 */ s32 screenLayout[4];
    /* 0x38 */ s32 contPort[4];
    /* 0x48 */ u8 unk48[0x13];
    /* 0x5B */ u8 multiTimer;
    /* 0x5C */ u8 unk5C[4];
    /* 0x60 */ void *challengeTable;
    /* 0x64 */ ChallengeTarget *challengeClass;
    /* 0x68 */ u8 unk68[8];
    /* 0x70 */ s32 musicTrack;
} FrontendOptions;

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
    /* 0x09A */ s16 unk09A;
    /* 0x09C */ s16 thrust;
    /* 0x09E */ s16 maxThrust;
    /* 0x0A0 */ u8 unk0A0[0x10];
    /* 0x0B0 */ u16 combatAttr;
    /* 0x0B2 */ s16 numToSpeed;
    /* 0x0B4 */ s32 distance;
    /* 0x0B8 */ s16 trackDistance;
    /* 0x0BA */ u16 remoteMagnitude;
    /* 0x0BC */ u16 remoteMax;
    /* 0x0BE */ u8 weapon;
    /* 0x0BF */ u8 lastWeapon;
    /* 0x0C0 */ u8 rearFire;
    /* 0x0C1 */ s8 targetShip;
    /* 0x0C2 */ s8 rearTarget;
    /* 0x0C3 */ u8 padC3;
    /* 0x0C4 */ s16 electroCount;
    /* 0x0C6 */ s16 autopilotCount;
    /* 0x0C8 */ s16 stallCount;
    /* 0x0CA */ s8 lap;
    /* 0x0CB */ s8 reverseLap;
    /* 0x0CC */ s16 nearSecNo;
    /* 0x0CE */ s16 prevSecNo;
    /* 0x0D0 */ s32 lapTimes[5];
    /* 0x0E4 */ s32 updateCount;
    /* 0x0E8 */ s32 padE8;
    /* 0x0EC */ s32 startY;
    /* 0x0F0 */ s32 eliminations;
    /* 0x0F4 */ u8 activeInLeague;
    /* 0x0F5 */ u8 unkF5[7];
    /* 0x0FC */ s16 finishPosition;
    /* 0x0FE */ u8 unkFE[6];
    /* 0x104 */ void (*update)();
} ShipData; /* size = 0x108 */

typedef struct GhostReplay {
    /* 0x0000 */ s32 randSeed;
    /* 0x0004 */ s32 keyIndex;
    /* 0x0008 */ s32 keyCount;
    /* 0x000C */ u16 padUnk00;
    /* 0x000E */ s16 disabled;
    /* 0x0010 */ s16 magic;
    /* 0x0012 */ u8 actTrackNum;
    /* 0x0013 */ u8 shipClass;
    /* 0x0014 */ ShipData ship;
    /* 0x011C */ u16 keys[0x1FA4];
    /* 0x4064 */ s32 bestLapTime;
    /* 0x4068 */ s32 lapTimes[6];
} GhostReplay; /* size = 0x4080 */

typedef struct HudGlyph {
    /* 0x00 */ s16 x;
    /* 0x02 */ s16 y;
    /* 0x04 */ u16 width;
    /* 0x06 */ s16 pad;
    /* 0x08 */ uSprite sprite;
} HudGlyph; /* size = 0x20 */

typedef struct RaceLineEntry {
    /* 0x00 */ s16 dist;
    /* 0x02 */ s16 pad[7];
} RaceLineEntry; /* size = 0x10 */

typedef struct HiScoreRecord {
    /* 0x00 */ u8 unk00[0x10];
    /* 0x10 */ s32 lapTime;
    /* 0x14 */ u8 unk14[0x08];
} HiScoreRecord; /* size = 0x1C */

typedef struct RaceResetData {
    /* 0x00 */ ShipData *ships;
    /* 0x04 */ ConfigData *config;
} RaceResetData;

typedef struct ScoreData {
    /* 0x00 */ u8 unk00[0x20];
    /* 0x20 */ s32 totalTime;
    /* 0x24 */ u8 displayScore;
    /* 0x25 */ u8 unk25[0x17];
    /* 0x3C */ s32 bestLapTime;
    /* 0x40 */ u8 unk40[0x08];
    /* 0x48 */ u8 gotHiScore;
} ScoreData;

/* ============================================================================
 * External globals
 * ============================================================================ */

extern FrontendOptions frontendOptions_g;
extern N64Texture *g_textureTable[];

/* Player ship indices & positions */
extern s32 ownShip;
extern s32 ownShip2;
extern s32 ownShip3;
extern s32 ownShip4;
extern s16 playerPosition;
extern s16 player2Position;
extern s16 player3Position;
extern s16 player4Position;

/* HUD sprite textures & font glyph tables */
extern s32 g_spriteCount;
extern s32 g_textEndX;
extern s16 g_hudTexStart;
extern s16 g_font3TexStart;
extern s16 g_font8TexStart;
extern s16 g_font12TexStart;
extern s16 g_font16TexStart;
extern s16 g_font16Widths[];
extern s16 g_font12Widths[];
extern s16 g_font8Widths[];
extern s16 g_font3Widths[];
extern HudGlyph g_font3[];
extern HudGlyph g_font8[];
extern HudGlyph g_font12[];
extern HudGlyph g_font16[];
extern uSprite g_delIconSprite;
extern uSprite g_endIconSprite;
extern uSprite g_rearLockSprite;
extern uSprite g_plusSprite;
extern uSprite g_minusSprite;

/* Speedometer textures & split-screen player bar sprites */
extern N64Texture *g_hudTempTex;
extern N64Texture *g_speedo1PThrustTex;
extern N64Texture *g_speedo1PShieldTex;
extern N64Texture *g_speedo1PWeaponTex;
extern N64Texture *g_speedoMPThrustTex;
extern N64Texture *g_speedoMPShieldTex;
extern N64Texture *g_speedoMPWeaponTex;
extern uSprite g_mpThrustSpriteP2;
extern uSprite g_mpThrustSpriteP3;
extern uSprite g_mpThrustSpriteP4;
extern uSprite g_mpShieldSpriteP2;
extern uSprite g_mpShieldSpriteP3;
extern uSprite g_mpShieldSpriteP4;
extern uSprite g_mpWeaponSpriteP2;
extern uSprite g_mpWeaponSpriteP3;
extern uSprite g_mpWeaponSpriteP4;

/* Race, checkpoint, ghost & challenge HUD state */
extern s32 g_demoShip;
extern s16 g_raceModeFlag;
extern s16 startLinePos[];
extern u8 g_infiniteTimeFlag;
extern u8 cheatTime;
extern s32 g_prevLapRecord;
extern s32 g_ghostLapTime0;
extern s32 g_ghostBestLapTime;
extern s32 g_prevSplitDelta;
extern s32 g_splitLapDelta;
extern s32 g_showSplitDelta;
extern s32 g_ttLapCount;
extern s32 g_ghostLoaded;
extern s32 g_ghostEnabled;
extern u8 g_classLaps[];
extern s32 g_trackNameScroll;
extern s32 g_trackNameFade;
extern u16 g_hudFlashState;
extern s32 g_shieldFlashFrame;
extern s32 g_hudFrameCount;
extern s32 g_lapRecFlashTimer;
extern s32 g_sessionBestFlashTimer;
extern s32 g_splitFlashTimer;
extern s32 g_challengeMedalLevel;
extern s32 g_raceInfoFrameCount;
extern s32 g_checkPointFlashFrame;
extern s32 g_checkPointBannerTimer[4];
extern s32 g_raceInfoOverlayArg;
extern u8 rearLockCycle[4];
extern u8 g_perfectLapFlag[4];
extern u8 g_perfectLapTimer[4];
extern u8 g_resultSfxPending;
extern u8 g_timeoutSfxPending;
extern ChallengeTarget g_raceChallenges[];
extern ChallengeTarget g_ttChallenges[];
extern ChallengeTarget g_weaponChallenges[];
extern ChallengeTarget g_superComboChallenges[];
extern s32 g_multiplayerFlag;
extern s32 g_podiumShips[3];
extern s32 g_podiumLocked[3];
extern CheckPoint g_checkPoints[6];
extern u8 g_playerFinished[4];
extern s8 checkPointReached[6][4];
extern s16 g_finishPosition[4];
extern s32 shipDamage[];
extern void *CameraUpdate;
extern void *CameraUpdate2;
extern void *CameraUpdate3;
extern void *CameraUpdate4;
extern s16 numTrackSections;
extern s32 g_savedLapTimes[5][4];
extern s32 attractDelayFrames;
extern GhostReplay g_ghostReplay;
extern s32 g_shipDeathCount[];
extern s32 pauseData;
extern s32 g_ghostLapTimes[];
extern s32 pauseRequests[4];
extern u8 attractMode;
extern s32 g_gameType;
extern u8 noCheckPoints;
extern s32 g_raceEndState;
extern RaceResetData resetData;
extern s32 g_attractMode;
extern u8 g_continuesLeft;
extern s32 g_raceInfoFontCtx;
extern OSTime g_lastCheckSfxTime;
extern s32 checkTimer;
extern s32 checkTimer2;
extern s32 checkTimer3;
extern s32 checkTimer4;
extern HiScoreRecord g_hiScoreTable[16][4];

/* ============================================================================
 * External & HUD function prototypes
 * ============================================================================ */

void LoadCmpFiles(void *data);
void *WADLoadCompressedTextureSequence(WADFILE *wad);
void PlayNote(s32 sound, s32 vol, s32 lrPan, s32 fbPan, s32 pitch);
void UpdateCameraAttractInternal();
void UpdateCameraAttractMode();
s32 GetAng(VECTOR a, VECTOR b);
s32 GetMagnitude(VECTOR v);
s16 rcos(s32 angle);
void UpdatePlayerShipStart();
void UpdatePlayerShip();
s32 GetDroneShield(s32 shipNo);
void UpdateDroneShip();
void humanToDroneSpeed(ShipData *shipList, s32 shipNo);
void UpdateRescueDroid();
Gfx *DrawButtonPrompt(s16 x, s16 y, s16 type, char *text, Gfx *glistp);
Gfx *DrawFontNumber(s32 num, s32 x, s32 y, s32 size, s32 colour, Gfx *glistp);
Gfx *DrawFontText(char *text, s32 x, s32 y, s32 size, Gfx *glistp);
Gfx *DrawFontTextYellow(char *text, s32 x, s32 y, s32 size, Gfx *glistp);
Gfx *AddCentreText(char *text, s32 x, s32 y, s32 size, Gfx *glistp);
void InitFontUnused(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5);
u8 GetBlinkState(void);
void UpdateBlink(void);
void CheckForHiScore(ConfigData *config, ScoreData *score, ShipData *shipList, s16 shipNo, s32 flag);
void InitGhostShip(ConfigData *config, ShipData *shipList, s32 flag);
void StoreGhostReplay(ConfigData *config, ScoreData *score, ShipData *ship);
char *GetCdTrackName(s32 track);
char *GetCdTrackArtist(s32 track);
void SetSfxFreqOffset(s32 handle, f32 pitch);
s32 MusStartEffect(s32 sfx);
void MusHandleSetVolume(s32 handle, s32 vol);

/* Functions defined in src/hud.c */
void InitHud(void);
void InitSpeedoSprites(uSprite *sprites, uSprite *mpSprites);
Gfx *FillSpeedo(uSprite *sprites1P, uSprite *spritesMP, ShipData *shipList, ConfigData *config,
                s32 pauseChoice, s32 shipNo, u32 layout, Gfx *glistp);
Gfx *AddTime(s32 time, s32 x, s32 y, s32 size, s32 colour, u8 flags, Gfx *glistp);
void TimeToString(s32 time, s32 flags, char *str);
Gfx *AddText(char *text, s32 x, s32 y, s32 size, s32 colour, Gfx *glistp);
Gfx *AddNumber(s32 num, s32 x, s32 y, s32 size, s32 colour, Gfx *glistp);
void NumberToString(s32 num, char *str);
Gfx *AddJustifyNumber(s32 num, s32 x, s32 y, s32 size, s32 colour, Gfx *glistp);
Gfx *AddDelEnd(u8 dir, s32 x, s32 y, s32 size, s32 colour, Gfx *glistp);
void InitFontSize(u8 size);
Gfx *CentreText(char *text, s32 x, s32 y, s32 size, s32 colour, Gfx *glistp);
Gfx *CentreTextAt(char *text, s32 x, s32 y, s32 size, s32 colour, Gfx *glistp);
Gfx *DrawPlayerHud(ShipData *shipList, uSprite *sprites1P, uSprite *spritesMP, s32 pauseChoice,
                   ConfigData *config, ScoreData *score, s32 player, u32 layout, Gfx *glistp);
s16 GetShipPosition(ShipData *shipList, s32 shipNo);
void copyInTempLapTimes(ShipData *shipList, s32 player);
Gfx *DisplayRaceInfo(ConfigData *config, ScoreData *score, ShipData *shipList, Gfx *glistp, s32 player);
Gfx *AlterCheckPoint(CheckPoint *checkPoints, ShipData *shipList, s32 pauseState, ConfigData *config,
                     s32 player, s32 shipNo, u32 layout, Gfx *glistp);
void DisplayContinues(void);

char *strcpy(char *dst, const char *src);
int sprintf(char *dst, const char *fmt, ...);

#endif /* WIPEOUT64_HUD_H */
