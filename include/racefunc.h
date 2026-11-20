#ifndef WIPEOUT64_RACEFUNC_H
#define WIPEOUT64_RACEFUNC_H

#include "common.h"
#include <ultra64.h>
/*
 * TEMPORARY: radscn.h only declares FrontendOptions up to numPlayers.
 * Rename its declaration while including radscn.h (same pattern as race.h).
 */
#define FrontendOptions FrontendOptionsHead
#define frontendOptions_g frontendOptionsHead_g
#include "radscn.h"
#undef FrontendOptions
#undef frontendOptions_g

/*
 * Temporary US/EU symbol-address aliases for check.py until canonical names
 * are added to config/{us,eu}/symbol_addrs.txt.
 */
#ifdef VERSION_EU
#define CheckShipDestroyed          func_8006E190
#define CheckForButtonPress         func_8006E594
#define Restart                     func_8006E618
#define QuitGame                    func_8006EBEC
#define CheckAttractQuit            func_8006ECBC
#define RaceComplete                func_8006ED50
#define CheckGamePause              func_8006FF0C
#define SelectPauseOption           func_80070050
#define PerformPauseOperations      func_80070150
#define ResetRace                   func_80070820
#define AppendPauseDList            func_80070898
#define AppendRaceCompleteDList     func_80070924
#define InitGhostShip               func_800709B0
#define RecordGhostInput            func_80070C28
#define ReplayGhostInput            func_80070CF4
#define StoreGhostReplay            func_80070DC0

/* External functions */
#define ResumeAudio                 func_8000FE38
#define PauseAudio                  func_8000FEF0
#define initShipData                func_80019580
#define PlayNote                    func_8001BCE8
#define InitAttractMode             func_8001F7C0
#define InitRaceData                func_8001F9D0
#define InitCamera                  func_80021B50
#define UpdateCameraRaceStart       func_80022038
#define UpdateCameraDestroyed       func_8002291C
#define UpdateCameraAttractMode     func_80023158
#define UpdatePlayerShipStart       func_800271A4
#define SetUpFirstCheckPoint                   func_80027400
#define UpdatePlayerShip            func_800275E4
#define UpdateDroneShipStart        func_80027F30
#define UpdateDroneShip             func_80028220
#define humanToDroneSpeed           func_8002AC0C
#define GetMenuKey                  func_8002EC5C
#define InitRescueDroid             func_80030048
#define CentreText                  func_80034244
#define copyInTempLapTimes          func_80038464
#define DisplayRaceInfo             func_80038524
#define DisplayContinues            func_8003B944
#define FlattenTrack                func_8003F720
#define ResetStartBoom              func_800094DC
#define ResetLightStart             func_8000AB84
#define ResetRumbleEffects                     func_8000D728
#define ResetFrontendMenu                      func_80053650
#define CheckNextChallenge          func_80056C90
#define InitExplode                 func_800689E4
#define InitRing                    func_80069850
#define InitElectric                           func_8006BB2C
#define InitScoreTabData            func_8006CFD8
#define CheckForHiScore             func_8006D050
#define EnterHighScoreIfNeeded      func_8006D330
#define InGameReset                 func_80070EE8
#define InGameConfirm               func_80071208
#define DeSelectScreen              func_800714A8
#define StartEngineSfx              func_80071EC0
#define StopEngineSfx               func_80071F78
#define StopSfx                     func_8007200C

/* External globals */
#define g_ghostShip                 D_80089FA4
#define g_randSeed                  D_80089FA8
#define g_attractQuitFlag           D_80089FAC
#define g_raceModeFlag                            D_80089FB2
#define g_linkPartnerState                        D_80089FB4
#define g_linkMyState               D_80089FB6
#define g_enteringHiScore                         D_80089FFC
#define g_ghostActive               D_8008A004
#define g_pauseExitFlag                           D_8008A00C
#define g_cameraShakeCount                        D_800912C4
#define g_effectFlashTimer                        D_800912D8
#define g_sixTracks                 D_800912E0
#define g_sixXPlayed                D_800912E2
#define g_eightTracks               D_800912E4
#define g_eightXPlayed              D_800912E6
#define g_phantomDone               D_800912E8
#define g_piranhaDone               D_800912EA
#define g_prevLapRecord                           D_800912EC
#define g_ghostLapTime0             D_800912F0
#define g_ghostBestLapTime          D_800912F4
#define g_showSplitDelta                          D_80091300
#define g_ttLapCount                              D_80091304
#define g_ghostLoaded                             D_80091308
#define g_ghostEnabled              D_8009130C
#define g_trackNameScroll           D_80091414
#define g_perfectLap                D_8009141C
#define g_challengeMedalLevel                     D_80091434
#define g_weaponActiveCount                       D_800914A0
#define g_superWeaponActive1        D_800914A8
#define g_superWeaponActive2        D_800914AC
#define g_perfectLapFlag                          D_800914B4
#define g_perfectLapTimer                         D_800914B8
#define g_quakeWaveState                          D_80091530
#define g_effectResetFlag                         D_8008958C
#define g_gameState                 D_80091730
#define g_hudRestartFlag                          D_800917F4
#define g_engineSfxPlaying          D_800917F6
#define g_menuScreen                D_8009193C
#define g_challengeTable0           D_80094194
#define g_challengeTable1           D_800942C8
#define g_challengeTable2           D_800943FC
#define g_challengeTable3           D_80094530
#define g_skyVisible                              D_80095CE0
#define g_cameraSwitchFlag                        D_80095F40
#define g_podiumShips               D_80095F80
#define g_podiumLocked                            D_80095F8C
#define PauseDList                  D_80095FF0
#define PauseDListPtr               D_80095FF4
#define RaceCompleteDList           D_80095FF8
#define RaceCompleteDListPtr        D_80095FFC
#define g_numberStrings             D_80096000
#define g_pauseDListMaxLen          D_80096020
#define g_raceCompleteDListMaxLen   D_80096024
#define g_shieldSfxHandle                         D_800968A8
#define g_weaponSfxHandle                         D_800968AC
#define g_droidVoice                D_800968B0
#define g_chargeSfxHandle           D_800968BC
#define g_raceOutcome               D_800A8FF8
#define g_raceCompleteFlag          D_800A9040
#define shipShapes                  D_800A9048
#define checkPointReached           D_800A9228
#define g_finishPosition            D_800A9248
#define g_scoreData                 D_800A9270
#define g_hudShowPause              D_800A9294
#define g_shipDamage                              D_800A92C0
#define g_firstContPad              D_800A9304
#define rescueData                  D_800A9310
#define CameraUpdate                D_800AE624
#define g_ghostRecord               D_800AE6F8
#define CameraUpdate2               D_800B33F8
#define CameraUpdate3               D_800B33FC
#define CameraUpdate4               D_800B3400
#define attractDelayFrames          D_800B3408
#define ownShip                     D_800B3414
#define g_ghostReplay               D_800B3418
#define g_raceQuitState                           D_800B747C
#define g_shipDeathCount            D_800B74A0
#define gameData                    D_800B74E0
#define pauseData                   D_800B7550
#define ownShip2                    D_800B756C
#define ownShip3                    D_800B7570
#define ownShip4                    D_800B7574
#define g_padInfo                   D_800B7580
#define g_ghostLapTimes             D_800B75D0
#define pauseRequests               D_800B75F0
#define g_attractFlag                             D_800B7600
#define g_gameType                  D_800B7604
#define g_raceEndState              D_800B7614
#define g_idleTimer                 D_800B7618
#define g_raceContext               D_800B7660
#define g_ghostPad                  D_800B7668
#define ships                       D_800B767C
#define g_restartRaceFlag           D_800B7680
#define g_ghostDisabled             D_800B769C
#define g_continuesLeft             D_800B76A8
#define g_fireLock                  D_800B84A4
#define g_raceCompleteFlag2                       D_800B8B18
#define g_flareCount                              D_800BA618
#define g_weaponLockFlag                          D_80097D10
#define g_leagueResults             D_800C5B88
#define cameras                     D_800CBE30
#define g_checkTimer                              D_800CE1D0
#define g_checkTimer2                             D_800CE9F8
#define g_checkTimer3                             D_800CE9FC
#define g_checkTimer4                             D_800CEA00
#define g_continueAnimFrame         D_800F2110
#define g_droneSwitchStep           D_800F2112
#else
#define SetUpFirstCheckPoint     func_80020160
#define ResetRumbleEffects       func_8004986C
#define ResetFrontendMenu        func_800518C0
#define InitElectric             func_80069CF8
#define g_raceModeFlag           D_8008A982
#define g_linkPartnerState       D_8008A984
#define g_enteringHiScore        D_8008A9CC
#define g_pauseExitFlag          D_8008A9DC
#define g_cameraShakeCount       D_80091C94
#define g_effectFlashTimer       D_80091CA8
#define g_prevLapRecord          D_80091CBC
#define g_showSplitDelta         D_80091CD0
#define g_ttLapCount             D_80091CD4
#define g_ghostLoaded            D_80091CD8
#define g_challengeMedalLevel    D_80091E04
#define g_weaponActiveCount      D_80091E70
#define g_perfectLapFlag         D_80091E84
#define g_perfectLapTimer        D_80091E88
#define g_quakeWaveState         D_80091F00
#define g_effectResetFlag        D_8009204C
#define g_hudRestartFlag         D_80092244
#define g_skyVisible             D_80096730
#define g_cameraSwitchFlag       D_80096990
#define g_podiumLocked           D_800969DC
#define g_shieldSfxHandle        D_800972F8
#define g_weaponSfxHandle        D_800972FC
#define g_shipDamage             D_8009BCE0
#define g_raceQuitState          D_800A9E9C
#define g_attractFlag            D_800AA020
#define g_raceCompleteFlag2      D_800AB548
#define g_flareCount             D_800AD048
#define g_weaponLockFlag         D_800AD820
#define g_checkTimer             D_800CDE90
#define g_checkTimer2            D_800CE6B8
#define g_checkTimer3            D_800CE6BC
#define g_checkTimer4            D_800CE6C0
#define CheckShipDestroyed          func_8006C130
#define CheckForButtonPress         func_8006C5F4
#define Restart                     func_8006C678
#define QuitGame                    func_8006CC4C
#define CheckAttractQuit            func_8006CD1C
#define RaceComplete                func_8006CDB0
#define CheckGamePause              func_8006DF6C
#define SelectPauseOption           func_8006E0B0
#define PerformPauseOperations      func_8006E1B0
#define ResetRace                   func_8006E880
#define AppendPauseDList            func_8006E8F8
#define AppendRaceCompleteDList     func_8006E984
#define InitGhostShip               func_8006EA10
#define RecordGhostInput            func_8006EC88
#define ReplayGhostInput            func_8006ED54
#define StoreGhostReplay            func_8006EE20

/* External functions */
#define ResumeAudio                 func_80009008
#define PauseAudio                  func_800090C0
#define initShipData                func_80012760
#define PlayNote                    func_80014D38
#define InitAttractMode             func_80018880
#define InitRaceData                func_80018A90
#define InitCamera                  func_8001AC10
#define UpdateCameraRaceStart       func_8001B084
#define UpdateCameraDestroyedMulti  func_8001B7C4
#define UpdateCameraDestroyed       func_8001B968
#define UpdateCameraAttractMode     func_8001C130
#define UpdatePlayerShipStart       func_8001FF04
#define UpdatePlayerShip            func_8002033C
#define UpdateDroneShipStart        func_80020C40
#define UpdateDroneShip             func_80020F30
#define humanToDroneSpeed           func_80023768
#define GetMenuKey                  func_800275BC
#define InitRescueDroid             func_80028944
#define CentreText                  func_8002CB40
#define copyInTempLapTimes          func_80030D78
#define DisplayRaceInfo             func_80030E38
#define DisplayContinues            func_80034250
#define FlattenTrack                func_80038030
#define ResetStartBoom              func_80045628
#define ResetLightStart             func_80046CD0
#define CheckNextChallenge          func_80054F08
#define InitExplode                 func_80066C10
#define InitRing                    func_80067A7C
#define InitScoreTabData            func_8006AF78
#define CheckForHiScore             func_8006AFF0
#define EnterHighScoreIfNeeded      func_8006B2D0
#define InGameReset                 func_8006EF48
#define InGameConfirm               func_8006F268
#define DeSelectScreen              func_8006F508
#define StartEngineSfx              func_8006FF20
#define StopEngineSfx               func_8006FFD8
#define StopSfx                     func_8007006C

/* External globals */
#define g_ghostShip                 D_8008A974
#define g_randSeed                  D_8008A978
#define g_attractQuitFlag           D_8008A97C
#define g_linkMyState               D_8008A986
#define g_ghostActive               D_8008A9D4
#define g_sixTracks                 D_80091CB0
#define g_sixXPlayed                D_80091CB2
#define g_eightTracks               D_80091CB4
#define g_eightXPlayed              D_80091CB6
#define g_phantomDone               D_80091CB8
#define g_piranhaDone               D_80091CBA
#define g_ghostLapTime0             D_80091CC0
#define g_ghostBestLapTime          D_80091CC4
#define g_ghostEnabled              D_80091CDC
#define g_trackNameScroll           D_80091DE4
#define g_perfectLap                D_80091DEC
#define g_superWeaponActive1        D_80091E78
#define g_superWeaponActive2        D_80091E7C
#define g_gameState                 D_80092180
#define g_engineSfxPlaying          D_80092246
#define g_menuScreen                D_8009238C
#define g_challengeTable0           D_80094BE4
#define g_challengeTable1           D_80094D18
#define g_challengeTable2           D_80094E4C
#define g_challengeTable3           D_80094F80
#define g_podiumShips               D_800969D0
#define PauseDList                  D_80096A40
#define PauseDListPtr               D_80096A44
#define RaceCompleteDList           D_80096A48
#define RaceCompleteDListPtr        D_80096A4C
#define g_numberStrings             D_80096A50
#define g_pauseDListMaxLen          D_80096A70
#define g_raceCompleteDListMaxLen   D_80096A74
#define g_droidVoice                D_80097300
#define g_chargeSfxHandle           D_8009730C
#define g_raceOutcome               D_8009BA18
#define g_raceCompleteFlag          D_8009BA60
#define shipShapes                  D_8009BA68
#define checkPointReached           D_8009BC48
#define g_finishPosition            D_8009BC68
#define g_scoreData                 D_8009BC90
#define g_hudShowPause              D_8009BCB4
#define g_firstContPad              D_8009BD24
#define rescueData                  D_8009BD30
#define CameraUpdate                D_800A1044
#define g_ghostRecord               D_800A1118
#define CameraUpdate2               D_800A5E18
#define CameraUpdate3               D_800A5E1C
#define CameraUpdate4               D_800A5E20
#define attractDelayFrames          D_800A5E28
#define ownShip                     D_800A5E2C
#define g_ghostReplay               D_800A5E38
#define g_shipDeathCount            D_800A9EC0
#define gameData                    D_800A9F00
#define pauseData                   D_800A9F70
#define ownShip2                    D_800A9F8C
#define ownShip3                    D_800A9F90
#define ownShip4                    D_800A9F94
#define g_padInfo                   D_800A9FA0
#define g_ghostLapTimes             D_800A9FF0
#define pauseRequests               D_800AA010
#define g_gameType                  D_800AA024
#define g_raceEndState              D_800AA038
#define g_idleTimer                 D_800AA03C
#define g_raceContext               D_800AA088
#define g_ghostPad                  D_800AA090
#define ships                       D_800AA0A4
#define g_restartRaceFlag           D_800AA0A8
#define g_ghostDisabled             D_800AA0C8
#define g_continuesLeft             D_800AA0D4
#define g_fireLock                  D_800AAED4
#define g_leagueResults             D_800C5848
#define cameras                     D_800CBAF0
#define g_continueAnimFrame         D_800F05D0
#define g_droneSwitchStep           D_800F05D2
#endif

#define VIEW_IN                 0x0004
#define RACING                  0x0040
#define SHIP_COMBAT_DESTROYED   0x8000

#define GHOST_MAGIC             0x3039
#define GHOST_MAX_KEYS          0x1F9C
#define PAUSE_DLIST_SIZE        500
#define RACE_COMPLETE_DLIST_SIZE 3000

#ifdef VERSION_EU
#define GHOST_START_UPDATE_COUNT 166
#else
#define SetUpFirstCheckPoint     func_80020160
#define ResetRumbleEffects       func_8004986C
#define ResetFrontendMenu        func_800518C0
#define InitElectric             func_80069CF8
#define g_raceModeFlag           D_8008A982
#define g_linkPartnerState       D_8008A984
#define g_enteringHiScore        D_8008A9CC
#define g_pauseExitFlag          D_8008A9DC
#define g_cameraShakeCount       D_80091C94
#define g_effectFlashTimer       D_80091CA8
#define g_prevLapRecord          D_80091CBC
#define g_showSplitDelta         D_80091CD0
#define g_ttLapCount             D_80091CD4
#define g_ghostLoaded            D_80091CD8
#define g_challengeMedalLevel    D_80091E04
#define g_weaponActiveCount      D_80091E70
#define g_perfectLapFlag         D_80091E84
#define g_perfectLapTimer        D_80091E88
#define g_quakeWaveState         D_80091F00
#define g_effectResetFlag        D_8009204C
#define g_hudRestartFlag         D_80092244
#define g_skyVisible             D_80096730
#define g_cameraSwitchFlag       D_80096990
#define g_podiumLocked           D_800969DC
#define g_shieldSfxHandle        D_800972F8
#define g_weaponSfxHandle        D_800972FC
#define g_shipDamage             D_8009BCE0
#define g_raceQuitState          D_800A9E9C
#define g_attractFlag            D_800AA020
#define g_raceCompleteFlag2      D_800AB548
#define g_flareCount             D_800AD048
#define g_weaponLockFlag         D_800AD820
#define g_checkTimer             D_800CDE90
#define g_checkTimer2            D_800CE6B8
#define g_checkTimer3            D_800CE6BC
#define g_checkTimer4            D_800CE6C0
#define GHOST_START_UPDATE_COUNT 200
#endif

typedef struct ChallengeEntry {
    /* 0x00 */ u8 unk00[0x24];
    /* 0x24 */ u8 completed;
    /* 0x25 */ u8 unk25[0x1B];
} ChallengeEntry; /* size = 0x40 */

typedef struct RaceFrontendOptions {
    /* 0x00 */ s32 team;
    /* 0x04 */ u8 teamClass;
    /* 0x08 */ u32 trackFile;
    /* 0x0C */ s32 trackNo;
    /* 0x10 */ u8 numPlayers;
    /* 0x11 */ u8 unk11[4];
    /* 0x15 */ u8 challengeMode;
    /* 0x16 */ u8 challengeIndex;
    /* 0x18 */ s32 ownShip[4];
    /* 0x28 */ s32 screenLayout[4];
    /* 0x38 */ s32 contPort[4];
    /* 0x48 */ u8 unk48[0x18];
    /* 0x60 */ ChallengeEntry *challengeTable;
    /* 0x64 */ void *challengeClass;
    /* 0x68 */ u8 unk68[0x14];
    /* 0x7C */ u8 leagueRacesLeft;
} RaceFrontendOptions;

typedef struct RaceConfigData {
    /* 0x00 */ s32 initMagic;
    /* 0x04 */ u8 trackNum;
    /* 0x05 */ u8 actTrackNum;
    /* 0x06 */ s8 shipNum;
    /* 0x07 */ s8 ship2Num;
    /* 0x08 */ s8 shipClass;
    /* 0x09 */ u8 unk09[3];
    /* 0x0C */ u8 exitCode;
    /* 0x0D */ u8 unk0D[5];
    /* 0x12 */ s16 raceInfo[4];
    /* 0x1A */ s16 complete;
    /* 0x1C */ s16 unk1C;
    /* 0x1E */ s16 highScore;
    /* 0x20 */ u8 unk20[0x0C];
    /* 0x2C */ u8 paused;
    /* 0x2D */ u8 unk2D[0x12];
    /* 0x3F */ u8 phantomChallenge;
    /* 0x40 */ u8 piranhaChallenge;
    /* 0x41 */ u8 unk41[3];
    /* 0x44 */ u8 phantomUnlocked;
    /* 0x45 */ u8 piranhaUnlocked;
    /* 0x46 */ u8 phantomCircuit;
    /* 0x47 */ u8 piranhaCircuit;
} RaceConfigData;

typedef void (*ShipUpdateFunc)();
typedef void (*CameraUpdateFunc)();

typedef struct RaceShipData {
    /* 0x000 */ s16 highResTableIndex;
    /* 0x002 */ s16 lowResTableIndex;
    /* 0x004 */ s32 unk04;
    /* 0x008 */ TrackSection *nearTrkSect;
    /* 0x00C */ TrackSection *prevShipSect;
    /* 0x010 */ s32 attr;
    /* 0x014 */ u8 unk14[0x9C];
    /* 0x0B0 */ u16 combatAttr;
    /* 0x0B2 */ s16 unkB2;
    /* 0x0B4 */ u8 unkB4[0x0A];
    /* 0x0BE */ u8 weapon;
    /* 0x0BF */ u8 unkBF[0x25];
    /* 0x0E4 */ s32 updateCount;
    /* 0x0E8 */ u8 unkE8[0x08];
    /* 0x0F0 */ s32 eliminations;
    /* 0x0F4 */ u8 activeInLeague;
    /* 0x0F5 */ u8 unkF5[0x0F];
    /* 0x104 */ ShipUpdateFunc update;
} RaceShipData; /* size = 0x108 */

typedef struct RaceTrackCamera {
    /* 0x00 */ Skeleton *camPos;
    /* 0x04 */ TrackSection *section;
    /* 0x08 */ s16 hdg, pitch, roll;
    /* 0x0E */ s16 vhdg, vpitch, vroll;
    /* 0x14 */ s16 vel[4];
} RaceTrackCamera; /* size = 0x1C */

typedef struct RaceCheckPoint {
    /* 0x00 */ s16 section;
    /* 0x02 */ s16 time;
    /* 0x04 */ s8 reached;
    /* 0x05 */ s8 pad05;
} RaceCheckPoint;

typedef struct RaceWeaponData {
    /* 0x00 */ s16 count;
    /* 0x02 */ u8 unk02[0x40];
    /* 0x42 */ u8 type;
    /* 0x43 */ u8 unk43[5];
} RaceWeaponData; /* size = 0x48 */

typedef struct RaceWeaponShape {
    /* 0x00 */ u8 unk00[4];
    /* 0x04 */ s16 used;
    /* 0x06 */ s16 pad06;
} RaceWeaponShape; /* size = 0x08 */

typedef struct RaceRescueData {
    /* 0x00 */ u8 data[0x4C];
} RaceRescueData;

typedef struct RaceResetData {
    /* 0x00 */ RaceShipData *ships;
    /* 0x04 */ RaceConfigData *config;
    /* 0x08 */ RaceWeaponData *weapons;
    /* 0x0C */ RaceWeaponShape *weaponShapes;
    /* 0x10 */ RaceRescueData *rescue;
    /* 0x14 */ Object **shipShapes;
    /* 0x18 */ RaceCheckPoint *checkPoints;
} RaceResetData;

typedef struct RacePauseMenu {
    /* 0x00 */ s32 selection;
    /* 0x04 */ s32 musicPaused;
    /* 0x08 */ s32 command;
    /* 0x0C */ s32 count;
    /* 0x10 */ s32 frameCount;
    /* 0x14 */ u8 pad14;
    /* 0x15 */ u8 active;
    /* 0x16 */ u8 showOptions;
    /* 0x17 */ u8 stage;
    /* 0x18 */ u8 confirmed;
    /* 0x19 */ u8 confirmChoice;
} RacePauseMenu;

typedef struct RaceScoreData {
    /* 0x00 */ u8 unk00[0x24];
    /* 0x24 */ u8 displayScore;
    /* 0x25 */ u8 unk25[0x23];
    /* 0x48 */ u8 gotHiScore;
} RaceScoreData;

typedef struct RacePadInfo {
    /* 0x00 */ u16 unk00;
    /* 0x02 */ u16 pad;
    /* 0x04 */ u16 xpad;
    /* 0x06 */ u16 opad;
    /* 0x08 */ u8 unk08;
    /* 0x09 */ u8 stickX;
    /* 0x0A */ u8 stickY;
    /* 0x0B */ u8 unk0B;
    /* 0x0C */ u16 stickPad;
    /* 0x0E */ u16 xstickPad;
    /* 0x10 */ u16 ostickPad;
} RacePadInfo; /* size = 0x12 */

typedef struct LeagueResult {
    /* 0x00 */ u8 firstCount;
    /* 0x01 */ u8 secondCount;
    /* 0x02 */ u8 thirdCount;
    /* 0x03 */ u8 pad03;
    /* 0x04 */ s32 points;
    /* 0x08 */ s32 unk08;
} LeagueResult; /* size = 0x0C */

typedef struct GhostReplay {
    /* 0x0000 */ s32 randSeed;
    /* 0x0004 */ s32 keyIndex;
    /* 0x0008 */ s32 keyCount;
    /* 0x000C */ u16 padUnk00;
    /* 0x000E */ s16 disabled;
    /* 0x0010 */ s16 magic;
    /* 0x0012 */ u8 actTrackNum;
    /* 0x0013 */ u8 shipClass;
    /* 0x0014 */ RaceShipData ship;
    /* 0x011C */ u16 keys[0x1FA4];
    /* 0x4064 */ s32 bestLapTime;
    /* 0x4068 */ s32 lapTimes[6];
} GhostReplay; /* size = 0x4080 */

/* Extern globals */
extern RaceFrontendOptions frontendOptions_g;
extern u8 g_contEnabled;

extern s32 g_ghostShip;
extern s32 g_randSeed;
extern s32 g_attractQuitFlag;
extern s16 g_raceModeFlag;
extern s16 g_linkPartnerState;
extern s16 g_linkMyState;
extern s32 g_enteringHiScore;
extern s32 g_ghostActive;
extern s32 g_pauseExitFlag;
extern s32 g_cameraShakeCount;
extern s32 g_effectFlashTimer;
extern s16 g_sixTracks;
extern s16 g_sixXPlayed;
extern s16 g_eightTracks;
extern s16 g_eightXPlayed;
extern s16 g_phantomDone;
extern s16 g_piranhaDone;
extern s32 g_prevLapRecord;
extern s32 g_ghostLapTime0;
extern s32 g_ghostBestLapTime;
extern s32 g_showSplitDelta;
extern s32 g_ttLapCount;
extern s32 g_ghostLoaded;
extern s32 g_ghostEnabled;
extern s32 g_trackNameScroll;
extern s16 g_perfectLap;
extern s32 g_challengeMedalLevel;
extern u8 g_weaponActiveCount;
extern s32 g_superWeaponActive1;
extern s32 g_superWeaponActive2;
extern u8 g_perfectLapFlag[4];
extern u8 g_perfectLapTimer[4];
extern u8 g_quakeWaveState;
extern s32 g_effectResetFlag[4];
extern s32 g_gameState;
extern u8 g_hudRestartFlag;
extern u8 g_engineSfxPlaying;
extern s32 g_menuScreen;
extern u8 g_challengeTable0[];
extern u8 g_challengeTable1[];
extern u8 g_challengeTable2[];
extern u8 g_challengeTable3[];
extern s16 g_skyVisible[4];
extern u8 g_cameraSwitchFlag;
extern s32 g_podiumShips[3];
extern s32 g_podiumLocked[3];
extern Gfx *PauseDList;
extern Gfx *PauseDListPtr;
extern Gfx *RaceCompleteDList;
extern Gfx *RaceCompleteDListPtr;
extern char *g_numberStrings[8];
extern s32 g_pauseDListMaxLen;
extern s32 g_raceCompleteDListMaxLen;
extern u32 g_shieldSfxHandle;
extern u32 g_weaponSfxHandle;
extern u32 g_droidVoice;
extern u32 g_chargeSfxHandle;
extern u8 g_raceOutcome[4];
extern u8 g_raceCompleteFlag;
extern Object *shipShapes[];
extern s8 checkPointReached[6][4];
extern s16 g_finishPosition[4];
extern RaceScoreData g_scoreData;
extern u8 g_hudShowPause;
extern s32 g_shipDamage[15];
extern OSContPad g_firstContPad;
extern RaceRescueData rescueData[4];
extern CameraUpdateFunc CameraUpdate;
extern GhostReplay g_ghostRecord;
extern CameraUpdateFunc CameraUpdate2;
extern CameraUpdateFunc CameraUpdate3;
extern CameraUpdateFunc CameraUpdate4;
extern s32 attractDelayFrames;
extern s32 ownShip;
extern GhostReplay g_ghostReplay;
extern s32 g_raceQuitState;
extern s32 g_shipDeathCount[15];
extern RaceConfigData gameData;
extern RacePauseMenu pauseData;
extern s32 ownShip2;
extern s32 ownShip3;
extern s32 ownShip4;
extern RacePadInfo g_padInfo[4];
extern s32 g_ghostLapTimes[6];
extern s32 pauseRequests[4];
extern u8 g_attractFlag;
extern s32 g_gameType;
extern s32 g_raceEndState;
extern s16 g_idleTimer;
extern s32 g_raceContext;
extern RacePadInfo g_ghostPad;
extern RaceShipData *ships;
extern u8 g_restartRaceFlag;
extern u16 g_ghostDisabled;
extern u8 g_continuesLeft;
extern u8 g_fireLock[4];
extern void *g_raceCompleteFlag2;
extern s32 g_flareCount;
extern s32 g_weaponLockFlag;
extern LeagueResult g_leagueResults[15];
extern RaceTrackCamera cameras[4];
extern s32 g_checkTimer;
extern s32 g_checkTimer2;
extern s32 g_checkTimer3;
extern s32 g_checkTimer4;
extern s16 g_continueAnimFrame;
extern s16 g_droneSwitchStep;

/* External functions */
void __assert(const char *expr, const char *file, s32 line);
int sprintf(char *dst, const char *fmt, ...);
void *memcpy(void *dst, const void *src, size_t len);
void StopAudio(void);
void ResumeAudio(void);
void PauseAudio(void);
void StopRumbleAll(s32 player);
void initShipData(TrackSection *sections, RaceShipData *ships, RaceConfigData *config);
void PlayNote(s32 sound, s32 volume, s32 lrPan, s32 fbPan, s32 pitch);
void InitAttractMode(s32 mode, RaceShipData *ships);
void InitRaceData(RacePauseMenu *pause);
void InitCamera(RaceTrackCamera *camera, TrackSection *section);
void UpdateCameraRaceStart();
#ifndef VERSION_EU
void UpdateCameraDestroyedMulti();
#endif
void UpdateCameraDestroyed();
void UpdateCameraAttractMode();
void UpdatePlayerShipStart();
void SetUpFirstCheckPoint(void);
void UpdatePlayerShip();
void UpdateDroneShipStart();
void UpdateDroneShip();
void humanToDroneSpeed(RaceShipData *ships, s32 shipIndex);
s16 GetMenuKey(s16 key, u16 pad);
void InitRescueDroid(RaceRescueData *rescue, Object **shipShapes, RaceShipData *ship);
Gfx *CentreText(const char *text, s32 x, s32 y, s32 size, s32 colour, Gfx *glistp);
void copyInTempLapTimes(RaceShipData *ships, s32 player);
Gfx *DisplayRaceInfo(RaceConfigData *config, RaceScoreData *score, RaceShipData *ships, Gfx *glistp, s32 player);
void DisplayContinues(void *arg0, s32 x, s32 y);
void FlattenTrack(void);
void ResetStartBoom(void);
void ResetLightStart(void);
void ResetRumbleEffects(s32 player, s32 arg1);
void ResetFrontendMenu(s32 screen);
u8 CheckNextChallenge(ChallengeEntry *table, void *challengeClass, u8 challengeIndex);
void InitExplode(void);
void InitRing(void);
void InitElectric(void);
void InitScoreTabData(RaceScoreData *score);
void CheckForHiScore(RaceConfigData *config, RaceScoreData *score, RaceShipData *ships, s32 shipIndex, s32 arg4);
Gfx *EnterHighScoreIfNeeded(RaceConfigData *config, RaceScoreData *score, u16 pad, Gfx *glistp);
Gfx *InGameReset(RacePauseMenu *pause, RaceConfigData *config, void *screen, u16 pad, Gfx *glistp);
Gfx *InGameConfirm(u8 *confirmed, s32 mode, s16 pad, u8 *selection, s32 *frameCount, Gfx *glistp);
Gfx *DeSelectScreen(void *screen, Gfx *glistp, s32 player);
void StartEngineSfx(void);
void StopEngineSfx(void);
void StopSfx(u32 *handle);

/* racefunc.c functions */
void CheckShipDestroyed(RaceShipData *ships, RaceConfigData *config, void *unused, s32 idleLimit);
void CheckForButtonPress(void);
void Restart(RaceResetData *reset, s32 mode);
void QuitGame(void);
u8 CheckAttractQuit(RaceConfigData *config);
Gfx *RaceComplete(void *screen, void *unused, RaceConfigData *config, s16 *choice,
                  RaceScoreData *score, RaceShipData *ships, void *pause, u16 pad,
                  s32 player, Gfx *glistp);
void CheckGamePause(RaceShipData *ships, RacePauseMenu *pause, RaceConfigData *config,
                    u16 *xpad, s32 *musicPaused);
void SelectPauseOption(s32 *selection, s32 canMove, RaceConfigData *config,
                       RacePauseMenu *pause, u16 pad);
s32 PerformPauseOperations(RacePauseMenu *pause, void *screen, RaceConfigData *config,
                           RaceResetData *reset, s32 *pausePlayer);
void ResetRace(RaceResetData *reset, RacePauseMenu *pause, s32 mode);
Gfx *AppendPauseDList(Gfx *glistp);
Gfx *AppendRaceCompleteDList(Gfx *glistp);
void InitGhostShip(RaceConfigData *config, RaceShipData *ships, s32 fromLap);
void RecordGhostInput(RaceShipData *ships);
void ReplayGhostInput(RaceShipData *ships);
void StoreGhostReplay(void *unused, RaceConfigData *config);

#endif /* WIPEOUT64_RACEFUNC_H */
