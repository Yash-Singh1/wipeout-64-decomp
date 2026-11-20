#ifndef WIPEOUT64_FRONTEND_H
#define WIPEOUT64_FRONTEND_H

#include "common.h"
#include <ultra64.h>
#include <PR/sched.h>

/*
 * Temporary US/EU symbol mappings until config/{us,eu}/symbol_addrs.txt
 * registers canonical names for frontend.c functions and the globals it uses.
 */
/* AUTO-BEGIN */
#ifdef VERSION_EU
#define SetMenuTransition        func_80053650
#define DrawMenuFrame            func_8005366C
#define AddFrontendDisplayList   func_80054030
#define CreateFrontendGfxTask    func_800540C4
#define DrawMenu                 func_800548C4
#define MainMenu                 func_800549E8
#define StartGame                func_80054BC8
#define MenuControl              func_80054F14
#define MenuControlMulti         func_80055728
#define TrackSelectMenu          func_80055F54
#define ChallengeSelectMenu      func_8005602C
#define RenderMenu3DModel        func_8005E4F8
#define RenderMultiShipModel     func_8005E7B8
#define SetupMultiViewport       func_8005FD78
#define RenderController3DModel  func_80064250
#define InitCredits              func_8006597C
#define RenderChallengeMedal3D   func_80065C34
#define RenderFinalStandings3D   func_80066200

#define StopSfx                  func_8007200C
#define SetShake                 func_8000D5D4
#define PlayNote                 func_8001BCE8
#define DrawButtonPrompt         func_8003CE4C
#define DrawHudText              func_800330E8
#define CentreText               func_80034244
#define SetupGfxTask             func_8004F288
#define Gfx2DMenuTask            func_8003C930
#define ResumeAudio              func_8000FE38

#define g_menuTransition         D_800C5C48
#define g_transitionLength       D_80091974
#define g_menuScreen             D_8009193C
#define g_menuItem               D_80091940
#define g_menuSprites            D_8009195C
#define g_medalSprite            D_80091924
#define g_leagueActive           D_800C5EC8
#define g_feResetFlag            D_800918D8
#define g_audioConfigSfx         D_800968A8
#define g_feDlistStart           D_800918DC
#define g_feDlistEnd             D_800918E0
#define MaxLength                D_80094664
#define g_feDlistSize            D_800918D0
#define g_mainMenu               D_80091C14
#define g_optionsMenu            D_80091D94
#define g_audioConfigMenu        D_80091F94
#define g_controllerConfigMenu   D_80092094
#define g_loadSaveMenu           D_80092114
#define g_gameConfigMenu         D_80092314
#define g_classMenu              D_80092514
#define g_trackMenu              D_80092654
#define g_teamMenu               D_80092854
#define g_challengeMenu          D_800929D4
#define g_raceChallengeMenu      D_80092B14
#define g_raceChallengeTeamMenu  D_80092D94
#define g_bonusMenu              D_80092F14
#define g_leagueMenu             D_80093294
#define g_ttChallengeMenu        D_80093394
#define g_ttChallengeTeamMenu    D_80093614
#define g_weaponChallengeMenu    D_80093794
#define g_weaponChallengeTeamMenu D_80093A14
#define g_superComboChallengeMenu D_80093B94
#define g_superComboChallengeTeamMenu D_80093E14
#define g_multiShipSelectMenu    D_80093F94
#define g_ttRecordsMenu          D_80094014
#define g_raceRecordsMenu        D_80094094
#define g_raceChallenges         D_80094194
#define g_ttChallenges           D_800942C8
#define g_weaponChallenges       D_800943FC
#define g_superComboChallenges   D_80094530
#define g_velocitarUnlocked      D_800942C6
#define g_piranhaUnlocked        D_800943FA
#define g_unk80094F7E            D_8009452E
#define g_attractTimer           D_80094668
#define g_unk800923F9            D_800919A9
#define g_multiNumPlayers        D_800919AC
#define g_multiSplitVertical     D_800919B4
#define g_prevMenuButtons        D_800C53B0
#define g_prevMenuButtons2       D_800C53B2
#define g_menuButtonsPressed     D_800C5C44
#define g_ctrlAnimTimer          D_80091948
#define g_ctrlAnimDir            D_8009194C
#define g_ctrlAnimLength         D_80091950

#define g_raceMode               D_800B7600
#define g_gameType               D_800B7604
#define g_firstContPad           D_800A9304
#define g_gfxInfo                D_800CEA08
#define g_mtxPool                D_800AE4C0
#define g_mtxCount               D_800AE514
#define g_staticSegment          D_800A69C4
#define g_initDls                D_800CB9D0
#define g_frameDelayFlag         D_80091818
#define g_cameras                D_800CBE30
#define ownShip                  D_800B3414
#define ownShip2                 D_800B756C
#define ownShip3                 D_800B7570
#define ownShip4                 D_800B7574
#define g_cheatHistory           D_800B84C8
#define cheatWeapon              D_80091254
#define cheatDamage              D_80091256
#define cheatTime                D_80091257
#define cheatTrack               D_8009125A
#define g_appState               D_80091730
#define g_loadDone               D_8008A008
#define g_demoMode               D_800B7660

#define ApplyGameConfig          func_800562F0
#define SaveControllerConfig     func_80056380
#define RestoreControllerConfig  func_800563A8
#define ApplyAudioConfig         func_800563E8
#define RestoreAudioConfig       func_800564A8
#define RestoreGameConfig        func_80056570
#define OptionsMenu              func_80056600
#define GameConfigMenu           func_80056688
#define GetTrackFileOffset       func_80056C10
#define UnlockNextChallenge      func_80056C90
#define CheckChallengeCategoryComplete func_80056D0C
#define CheckSuperComboUnlocked  func_80056D80
#define CheckAllGoldAwarded      func_80056E30
#define CheckGoldChallengeUnlocked func_80056F34
#define GetBlinkState            func_8005DD5C

#define SetMasterMusicVolume     func_8000FE14
#define SetMasterSfxVolume       func_8000FDF0
#define InitPlasmaPaletteGold    func_800520E8

#define g_savedPadConfig         D_800C5C3C
#define g_padConfigIndex         D_8008A0C4
#define g_savedMusicTrack        D_800C6248
#define g_savedSfxVol            D_800C624C
#define g_savedMusicVol          D_800C6250
#define cheatSuperCombo          D_80091258
#define g_textEndX               D_800B9328
#define g_arrowLeftSprite        D_800BA6C8
#define g_arrowRightSprite       D_800BA718

#define RaceChallengeMenu        func_80056F8C
#define TimeTrialChallengeMenu   func_800571D4
#define WeaponChallengeMenu      func_8005741C
#define SuperComboChallengeMenu  func_80057668
#define ClassSelectMenu          func_800578B4
#define TeamSelectMenu           func_80057948
#define TimeTrialChallengeTeamMenu func_80057A00
#define InitChallengeTeamMenu    func_80057AD0
#define RaceChallengeTeamMenu    func_80057C7C
#define DrawChallengeCategoryBonus func_80057D4C
#define DrawGoldChallengeBonus   func_800580B8
#define DrawSuperComboBonus      func_8005840C
#define DrawAllGoldBonus         func_80058550
#define SwapLeagueStandings      func_800586D4
#define GetPilotName             func_80058704
#define LeagueStandingsMenu      func_800587A8
#define BonusCategoryScreen      func_80058C3C
#define BonusCategoryCheck       func_80058D04
#define BonusGoldScreen          func_80058D6C
#define BonusGoldCheck           func_80058E34
#define BonusSuperComboScreen    func_80058E9C
#define BonusSuperComboCheck     func_80058F64
#define BonusAllGoldScreen       func_80058FD8
#define WeaponChallengeTeamMenu  func_800590F0
#define SuperComboChallengeTeamMenu func_800591CC
#define FrontendStub_80057520    func_800592A8
#define DrawMenuBox              func_8005B394
#define DrawShipStats            func_80064580

#define AddCentreText            func_8003D8A4
#define DrawFontText             func_8003D388
#define AddNumber                func_8003D220
#define StartMusic               func_8000FC8C

#define g_challengeTeamMenuInit  D_80091978
#define g_awardTextY             D_800918D4
#define g_leagueStandings        D_800C53E0
#define g_leagueResults          D_800C5B88
#define g_leaguePilotCount       D_800919BC
#define g_bonusMusicDelay        D_80091980
#define g_bonusAllGoldMusicFlag  D_80091954
#define g_bonusCategoryMenu      D_80092F94
#define g_bonusGoldMenu          D_80093094
#define g_bonusSuperComboMenu    D_80093194
#define g_bonusAllGoldMenu       D_80093214
#define g_bonusGoldCheckNext     D_8009303C
#define g_bonusSuperComboCheckNext D_8009313C
#define ships                    D_800B767C

#define ControllerPakMenu        func_800592B0
#define QuitLeagueMenu           func_8005AE1C
#define FinalStandingsMenu       func_8005B048
#define LoadFrontendSprites      func_8005B1CC
#define InitFrontendSprites      func_8005B628
#define InitControllerPak        func_8005C17C
#define SaveControllerPakData    func_8005BFE0
#define LoadControllerPakData    func_8005C09C

#define ReinitCont               func_8004BBE8
#define DrawFontTextYellow       func_8003D60C
#define CentreTextAt             func_80034648
#define LoadCompressedTextureSequence func_800172FC
#define WADLoadCompressedTextureSequence func_80017534
#define LoadCmpFiles             func_80016994

#define g_pakInitDelay           D_8009466C
#define g_pakRetries             D_80094670
#define g_pakConfirm             D_80094674
#define g_pakState               D_80094678
#define g_pakMsgTimer            D_8009467C
#define g_pakTouched             D_80094680
#define g_pakOverwriteConfirmed  D_80094681
#define g_quitLeaguePrompt       D_80094684
#define g_quitLeagueConfirm      D_80094688
#define g_pfsErr                 D_800C53B4
#define g_pfsRepairAttempts      D_800C53B8
#define g_pfsFileNo              D_800C59B8
#define g_pfsCompanyCode         D_80091984
#define g_pfsGameCode            D_80091988
#define g_pfsGameName            D_8009198C
#define g_pfsExtName             D_8009199C
#define g_pfsSaveSize            D_800919A0
#define g_contPads               D_800B76D8
#define g_rumblePresent          D_800BF280
#define g_motorPfs               D_800BF0E0
#define g_finalStandingsMenu     D_80093314
#define g_feWad                  D_800C5C40
#define g_textureTable           D_800B2778
#define g_boxSprites0            D_80091964
#define g_boxSprites1            D_80091968
#define g_boxSprites2            D_800918E4
#define g_boxSprites3            D_80091960
#define g_boxSprites4            D_800918E8
#define g_boxSprites5            D_800918EC
#define g_boxSprites6            D_800918F0
#define g_boxSprites7            D_800918F4
#define g_boxSprites8            D_800918F8
#define g_boxSprites10           D_80091970
#define g_boxSprites11           D_8009196C

#define PackAudioSaveData        func_8005B718
#define PackGameAndScoresSaveData func_8005B744
#define PackControllerSaveData   func_8005B964
#define PackChallengeSaveData    func_8005B978
#define UnpackAudioSaveData      func_8005BAC0
#define UnpackGameAndScoresSaveData func_8005BAEC
#define UnpackControllerSaveData func_8005BCCC
#define RefreshChallengeUnlocks  func_8005BCE0
#define UnpackChallengeSaveData  func_8005BE80
#define StartupLogosAndAutoLoad  func_8005C1E4
#define UnlockAllChallengesCheat func_8005C6A8
#define CheckControllerDetected  func_8005C864
#define StartupWarningAndInit    func_8005C99C
#define CleanupStartupLogos      func_8005E450

#define InitPadSettings          func_8002E620
#define InitRaceResources        func_80030EA0
#define ScoreTabInit             func_8006CD60
#define InitRenderList           func_80051474
#define InitInGamePlasmaPalette  func_80052E4C

#define g_saveBuf                D_800C5458
#define g_hudState               D_800A9270
#define g_hiScoreTable           D_800F1A10
#define g_hiScoreLoaded          D_80089FCA
#define g_logoTimerInit          D_8009468C
#define g_pakAutoLoadState       D_80094690
#define g_pakAutoLoadFind        D_80094694
#define g_pakAutoLoadDone        D_80094698
#define g_startupWarningTimer    D_8009469C
#define g_startupWarningPhase    D_800946A0
#define g_logoStartTime          D_800C53C0
#define g_autoLoadErr            D_800C53C8
#define g_oneSecond              D_800977D0
#define g_unk8008A9DC            D_8008A00C
#define g_startupActive          D_80091979
#define g_allChallengesCheated   D_80094662
#define g_pakMissingAtBoot       D_80091958
#define g_upperSkeletons         D_800B87F8
#define g_feSkeleton             D_800CBEA0
#define g_shipDeathCount         D_800B74A0
#define g_raceOverlay            D_800B74E0
#define g_renderList             D_800C4920
#define g_animTextureCount       D_80089FA2
#define g_loaderSprite           D_800CE1E0
#define g_unk800AD820            D_80097D10

#define FrontendUpdate           func_8005D05C
#define StartAttractDemo         func_8005D968
#define TitleScreen              func_8005DBB8
#define UpdateBlink              func_8005DD70
#define LoadFrontendModels       func_8005DD98
#define CheckAllGoldInitial      func_8005DF48
#define LoadSpinningMedal        func_8005DFF0
#define InitFrontend             func_8005E110
#define TimeTrialRecordsMenu     func_8005EA1C
#define RaceRecordsMenu          func_8005EE8C
#define AudioConfigMenu          func_8005F478
#define LoadAudioSliderSprites   func_8005FB60
#define MultiplayerStartMenu     func_80061CD0
#define RaceChallengeBriefing    func_80063450
#define TimeTrialChallengeBriefing func_80063684
#define WeaponChallengeBriefing  func_8006395C
#define SuperComboChallengeBriefing func_80063C44
#define ControllerConfigMenu     func_80063FB4
#define CheckBootControllerPak   func_80064A64
#define ControllerPakManagerMenu func_80064FE8
#define CreditsMenu              func_800659D4

#define UpdateCheatSequence      func_8002ECE8
#define UpdatePlasma             func_80052A60
#define RenderPlasma             func_80052CA0
#define InitPlasma               func_80052934
#define UpdateWeaponMessage      func_8004B25C
#define DrawScreenOverlay        func_8004B430
#define newRand                  func_80023FCC
#define WADLoadPrm               func_80011278
#define ResetRenderList          func_800513F0
#define AddRenderGroup           func_800517C0
#define FlushRenderList          func_80051520

#define g_feInitialized          D_800946A4
#define g_pakManagerActive       D_800946A8
#define g_startupLogosActive     D_800946AC
#define g_startupWarningActive   D_800946B0
#define g_medalAnimFrame         D_800946B4
#define g_medalAnimTick          D_800946B8
#define g_titleIdleTimer         D_800946BC
#define g_titleSeedCounter       D_800946C0
#define g_titleSeedLocked        D_800946C2
#define g_blinkCounter           D_800946C4
#define g_pakManagerInit         D_800919A8
#define g_medalTexStart          D_80091928
#define g_medalTexCount          D_800C53DC
#define g_unk8009BA0C            D_800A8FEC
#define g_unk8009BA10            D_800A8FF0
#define g_unk800AA084            D_800B765C
#define cheatfade                D_80091252
#define cheatAllChallenges       D_80091255
#define g_idleTimer              D_800B7618
#define g_randSeed               D_80089FA8
#define g_feRenderList           D_800C4950
#define g_feModels               D_800C5A00
#define g_unk800A0EE4            D_800AE4C4
#define g_unk8009234C            D_800918FC
#define g_unk80092350            D_80091900
#define g_unk80092354            D_80091904
#define g_unk80092358            D_80091908
#define g_unk8009235C            D_8009190C
#define g_unk80092360            D_80091910
#define g_unk80092364            D_80091914
#define g_unk80092368            D_80091918
#define g_unk8009236C            D_8009191C
#define g_unk80092370            D_80091920
#define g_modelAngle             D_8009197C
#define g_modelTransforms        D_800919C4
#define g_multiSlots             D_800C5A98

#define InitMultiplayerSlots     func_8005FC38
#define UpdateMultiScreenLayouts func_800603A8
#define UpdateMultiPlayerInput   func_80060598

#define GetTrackName             func_8003CDB8
#define TimeToString             func_80033028
#define GetCdTrackName           func_80070EB0
#define GetCdTrackArtist         func_80070ECC
#define PlaySfx                  func_80072048
#define SetSfxFreqOffset         func_8007206C

#define g_volumeBarSprites       D_800C61E8
#define g_font12                 D_800B98F0
#define g_multi3PlayerLayout     D_800919B8
#define g_multiReadyCount        D_800919B0
#define cheatTeam                D_80091259
#define g_padInfo                D_800B7580
#define g_contButtonPressed      D_800B76C8

#define DrawMultiPlayerSlot      func_80060C14
#define ShipChoiceToShipIdx      func_80061C48
#define DrawChallengeInfo        func_80062228
#define AppendN64FontCode        func_80064B90
#define DrawPakProblemPrompt     func_80064E3C

#define GetButtonName            func_80016414
#define AddDelEnd                func_80033B68
#define GetClassName             func_8003CD40
#define BuildEnvMapDisplayList   func_80015104
#define AddRenderDisplayList     func_80051894
#define FlushSkyRenderList       func_8001EF64

#define g_multiFrameCounter      D_800919C0
#define g_multiSlotsInit         D_800946C8
#define g_briefingTextX          D_800946CC
#define g_briefingTextY          D_800946D0
#define g_briefingTextStep       D_800946D4
#define g_weaponPulseAlpha       D_800946D8
#define g_pakMgrCursor           D_800946DC
#define g_pakMgrConfirmDelete    D_800946E0
#define g_pakMgrErrorShown       D_800946E4
#define g_pakMgrWarnActive       D_800946E5
#define g_pakMgrNeedRefresh      D_800946E8
#define g_pakMgrWarnTimer        D_800946EC
#define g_creditsLines           D_800946F0
#define g_creditsTopY            D_80095CB0
#define g_creditsBottomY         D_80095CB4
#define g_creditsLineSpacing     D_80095CB8
#define g_creditsScrollSpeed     D_80095CBC
#define g_creditsMusicInit       D_80095CC0
#define g_creditsFrameCount      D_80095CC4
#define g_envMapCachedModel      D_80095CC8
#define g_envMapCachedAward      D_80095CD0
#define g_raceChallengeBriefingMenu D_80092D14
#define g_ttChallengeBriefingMenu D_80093594
#define g_weaponChallengeBriefingMenu D_80093994
#define g_superComboChallengeBriefingMenu D_80093D94
#define g_creditsMenu            D_80094114
#define g_contStatus             D_800A9258
#define g_bootPakErr             D_800C6254
#define g_pakMgrMaxFiles         D_800C53CC
#define g_pakMgrFilesUsed        D_800C53D0
#define g_pakMgrFreeBlocks       D_800C53D4
#define g_pakMgrPrevButtons      D_800C53D8
#define g_pakMgrSlotStatus       D_800C59C0
#define g_pakMgrSlotState        D_800C5C58
#define g_medalDlBuf             D_800C5EE8
#define shipShapes               D_800A9048
#else
#define SetMenuTransition        func_800518C0
#define DrawMenuFrame            func_800518DC
#define AddFrontendDisplayList   func_800522A0
#define CreateFrontendGfxTask    func_80052334
#define DrawMenu                 func_80052B3C
#define MainMenu                 func_80052C60
#define StartGame                func_80052E40
#define MenuControl              func_8005318C
#define MenuControlMulti         func_800539A0
#define TrackSelectMenu          func_800541CC
#define ChallengeSelectMenu      func_800542A4
#define RenderMenu3DModel        func_8005C770
#define RenderMultiShipModel     func_8005CA30
#define SetupMultiViewport       func_8005DFF0
#define RenderController3DModel  func_800624DC
#define InitCredits              func_80063C08
#define RenderChallengeMedal3D   func_80063EC0
#define RenderFinalStandings3D   func_8006448C

#define StopSfx                  func_8007006C
#define SetShake                 func_80049720
#define PlayNote                 func_80014D38
#define DrawButtonPrompt         func_8003575C
#define DrawHudText              func_8002B9E4
#define CentreText               func_8002CB40
#define SetupGfxTask             func_8004D470
#define Gfx2DMenuTask            func_80035240
#define ResumeAudio              func_80009008

#define g_menuTransition         D_800C5908
#define g_transitionLength       D_800923C4
#define g_menuScreen             D_8009238C
#define g_menuItem               D_80092390
#define g_menuSprites            D_800923AC
#define g_medalSprite            D_80092374
#define g_leagueActive           D_800C5B88
#define g_feResetFlag            D_80092328
#define g_audioConfigSfx         D_800972F8
#define g_feDlistStart           D_8009232C
#define g_feDlistEnd             D_80092330
#define MaxLength                D_800950B4
#define g_feDlistSize            D_80092320
#define g_mainMenu               D_80092664
#define g_optionsMenu            D_800927E4
#define g_audioConfigMenu        D_800929E4
#define g_controllerConfigMenu   D_80092AE4
#define g_loadSaveMenu           D_80092B64
#define g_gameConfigMenu         D_80092D64
#define g_classMenu              D_80092F64
#define g_trackMenu              D_800930A4
#define g_teamMenu               D_800932A4
#define g_challengeMenu          D_80093424
#define g_raceChallengeMenu      D_80093564
#define g_raceChallengeTeamMenu  D_800937E4
#define g_bonusMenu              D_80093964
#define g_leagueMenu             D_80093CE4
#define g_ttChallengeMenu        D_80093DE4
#define g_ttChallengeTeamMenu    D_80094064
#define g_weaponChallengeMenu    D_800941E4
#define g_weaponChallengeTeamMenu D_80094464
#define g_superComboChallengeMenu D_800945E4
#define g_superComboChallengeTeamMenu D_80094864
#define g_multiShipSelectMenu    D_800949E4
#define g_ttRecordsMenu          D_80094A64
#define g_raceRecordsMenu        D_80094AE4
#define g_raceChallenges         D_80094BE4
#define g_ttChallenges           D_80094D18
#define g_weaponChallenges       D_80094E4C
#define g_superComboChallenges   D_80094F80
#define g_velocitarUnlocked      D_80094D16
#define g_piranhaUnlocked        D_80094E4A
#define g_unk80094F7E            D_80094F7E
#define g_attractTimer           D_800950B8
#define g_unk800923F9            D_800923F9
#define g_multiNumPlayers        D_800923FC
#define g_multiSplitVertical     D_80092404
#define g_prevMenuButtons        D_800C5070
#define g_prevMenuButtons2       D_800C5072
#define g_menuButtonsPressed     D_800C5904
#define g_ctrlAnimTimer          D_80092398
#define g_ctrlAnimDir            D_8009239C
#define g_ctrlAnimLength         D_800923A0

#define g_raceMode               D_800AA020
#define g_gameType               D_800AA024
#define g_firstContPad           D_8009BD24
#define g_gfxInfo                D_800CE6C8
#define g_mtxPool                D_800A0EE0
#define g_mtxCount               D_800A0F34
#define g_staticSegment          D_800993E4
#define g_initDls                D_800CB690
#define g_frameDelayFlag         D_80092268
#define g_cameras                D_800CBAF0
#define ownShip                  D_800A5E2C
#define ownShip2                 D_800A9F8C
#define ownShip3                 D_800A9F90
#define ownShip4                 D_800A9F94
#define g_cheatHistory           D_800AAEF8
#define cheatWeapon              D_80091C24
#define cheatDamage              D_80091C26
#define cheatTime                D_80091C27
#define cheatTrack               D_80091C2A
#define g_appState               D_80092180
#define g_loadDone               D_8008A9D8
#define g_demoMode               D_800AA088

#define ApplyGameConfig          func_80054568
#define SaveControllerConfig     func_800545F8
#define RestoreControllerConfig  func_80054620
#define ApplyAudioConfig         func_80054660
#define RestoreAudioConfig       func_80054720
#define RestoreGameConfig        func_800547E8
#define OptionsMenu              func_80054878
#define GameConfigMenu           func_80054900
#define GetTrackFileOffset       func_80054E88
#define UnlockNextChallenge      func_80054F08
#define CheckChallengeCategoryComplete func_80054F84
#define CheckSuperComboUnlocked  func_80054FF8
#define CheckAllGoldAwarded      func_800550A8
#define CheckGoldChallengeUnlocked func_800551AC
#define GetBlinkState            func_8005BFD4

#define SetMasterMusicVolume     func_80008FE4
#define SetMasterSfxVolume       func_80008FC0
#define InitPlasmaPaletteGold    func_80050358

#define g_savedPadConfig         D_800C58FC
#define g_padConfigIndex         D_8008AA94
#define g_savedMusicTrack        D_800C5F08
#define g_savedSfxVol            D_800C5F0C
#define g_savedMusicVol          D_800C5F10
#define cheatSuperCombo          D_80091C28
#define g_textEndX               D_800ABD58
#define g_arrowLeftSprite        D_800AD0F8
#define g_arrowRightSprite       D_800AD148

#define RaceChallengeMenu        func_80055204
#define TimeTrialChallengeMenu   func_8005544C
#define WeaponChallengeMenu      func_80055694
#define SuperComboChallengeMenu  func_800558E0
#define ClassSelectMenu          func_80055B2C
#define TeamSelectMenu           func_80055BC0
#define TimeTrialChallengeTeamMenu func_80055C78
#define InitChallengeTeamMenu    func_80055D48
#define RaceChallengeTeamMenu    func_80055EF4
#define DrawChallengeCategoryBonus func_80055FC4
#define DrawGoldChallengeBonus   func_80056330
#define DrawSuperComboBonus      func_80056684
#define DrawAllGoldBonus         func_800567C8
#define SwapLeagueStandings      func_8005694C
#define GetPilotName             func_8005697C
#define LeagueStandingsMenu      func_80056A20
#define BonusCategoryScreen      func_80056EB4
#define BonusCategoryCheck       func_80056F7C
#define BonusGoldScreen          func_80056FE4
#define BonusGoldCheck           func_800570AC
#define BonusSuperComboScreen    func_80057114
#define BonusSuperComboCheck     func_800571DC
#define BonusAllGoldScreen       func_80057250
#define WeaponChallengeTeamMenu  func_80057368
#define SuperComboChallengeTeamMenu func_80057444
#define FrontendStub_80057520    func_80057520
#define DrawMenuBox              func_8005960C
#define DrawShipStats            func_8006280C

#define AddCentreText            func_800361B4
#define DrawFontText             func_80035C98
#define AddNumber                func_80035B30
#define StartMusic               func_80008E5C

#define g_challengeTeamMenuInit  D_800923C8
#define g_awardTextY             D_80092324
#define g_leagueStandings        D_800C50A0
#define g_leagueResults          D_800C5848
#define g_leaguePilotCount       D_8009240C
#define g_bonusMusicDelay        D_800923D0
#define g_bonusAllGoldMusicFlag  D_800923A4
#define g_bonusCategoryMenu      D_800939E4
#define g_bonusGoldMenu          D_80093AE4
#define g_bonusSuperComboMenu    D_80093BE4
#define g_bonusAllGoldMenu       D_80093C64
#define g_bonusGoldCheckNext     D_80093A8C
#define g_bonusSuperComboCheckNext D_80093B8C
#define ships                    D_800AA0A4

#define ControllerPakMenu        func_80057528
#define QuitLeagueMenu           func_80059094
#define FinalStandingsMenu       func_800592C0
#define LoadFrontendSprites      func_80059444
#define InitFrontendSprites      func_800598A0
#define InitControllerPak        func_8005A3F4
#define SaveControllerPakData    func_8005A258
#define LoadControllerPakData    func_8005A314

#define ReinitCont               func_80049D08
#define DrawFontTextYellow       func_80035F1C
#define CentreTextAt             func_8002CF44
#define LoadCompressedTextureSequence func_800104DC
#define WADLoadCompressedTextureSequence func_80010714
#define LoadCmpFiles             func_8000FB74

#define g_pakInitDelay           D_800950BC
#define g_pakRetries             D_800950C0
#define g_pakConfirm             D_800950C4
#define g_pakState               D_800950C8
#define g_pakMsgTimer            D_800950CC
#define g_pakTouched             D_800950D0
#define g_pakOverwriteConfirmed  D_800950D1
#define g_quitLeaguePrompt       D_800950D4
#define g_quitLeagueConfirm      D_800950D8
#define g_pfsErr                 D_800C5074
#define g_pfsRepairAttempts      D_800C5078
#define g_pfsFileNo              D_800C5678
#define g_pfsCompanyCode         D_800923D4
#define g_pfsGameCode            D_800923D8
#define g_pfsGameName            D_800923DC
#define g_pfsExtName             D_800923EC
#define g_pfsSaveSize            D_800923F0
#define g_contPads               D_800AA108
#define g_rumblePresent          D_800BEF40
#define g_motorPfs               D_800BEDA0
#define g_finalStandingsMenu     D_80093D64
#define g_feWad                  D_800C5900
#define g_textureTable           D_800A5198
#define g_boxSprites0            D_800923B4
#define g_boxSprites1            D_800923B8
#define g_boxSprites2            D_80092334
#define g_boxSprites3            D_800923B0
#define g_boxSprites4            D_80092338
#define g_boxSprites5            D_8009233C
#define g_boxSprites6            D_80092340
#define g_boxSprites7            D_80092344
#define g_boxSprites8            D_80092348
#define g_boxSprites10           D_800923C0
#define g_boxSprites11           D_800923BC

#define PackAudioSaveData        func_80059990
#define PackGameAndScoresSaveData func_800599BC
#define PackControllerSaveData   func_80059BDC
#define PackChallengeSaveData    func_80059BF0
#define UnpackAudioSaveData      func_80059D38
#define UnpackGameAndScoresSaveData func_80059D64
#define UnpackControllerSaveData func_80059F44
#define RefreshChallengeUnlocks  func_80059F58
#define UnpackChallengeSaveData  func_8005A0F8
#define StartupLogosAndAutoLoad  func_8005A45C
#define UnlockAllChallengesCheat func_8005A920
#define CheckControllerDetected  func_8005AADC
#define StartupWarningAndInit    func_8005AC14
#define CleanupStartupLogos      func_8005C6C8

#define InitPadSettings          func_80026F80
#define InitRaceResources        func_80029790
#define ScoreTabInit             func_8006AD10
#define InitRenderList           func_8004F6E4
#define InitInGamePlasmaPalette  func_800510BC

#define g_saveBuf                D_800C5118
#define g_hudState               D_8009BC90
#define g_hiScoreTable           D_800EFED0
#define g_hiScoreLoaded          D_8008A99A
#define g_logoTimerInit          D_800950DC
#define g_pakAutoLoadState       D_800950E0
#define g_pakAutoLoadFind        D_800950E4
#define g_pakAutoLoadDone        D_800950E8
#define g_startupWarningTimer    D_800950EC
#define g_startupWarningPhase    D_800950F0
#define g_logoStartTime          D_800C5080
#define g_autoLoadErr            D_800C5088
#define g_oneSecond              D_80098230
#define g_unk8008A9DC            D_8008A9DC
#define g_startupActive          D_800923C9
#define g_allChallengesCheated   D_800950B2
#define g_pakMissingAtBoot       D_800923A8
#define g_upperSkeletons         D_800AB228
#define g_feSkeleton             D_800CBB60
#define g_shipDeathCount         D_800A9EC0
#define g_raceOverlay            D_800A9F00
#define g_renderList             D_800C45E0
#define g_animTextureCount       D_8008A972
#define g_loaderSprite           D_800CDEA0
#define g_unk800AD820            D_800AD820

#define FrontendUpdate           func_8005B2D4
#define StartAttractDemo         func_8005BBE0
#define TitleScreen              func_8005BE30
#define UpdateBlink              func_8005BFE8
#define LoadFrontendModels       func_8005C010
#define CheckAllGoldInitial      func_8005C1C0
#define LoadSpinningMedal        func_8005C268
#define InitFrontend             func_8005C388
#define TimeTrialRecordsMenu     func_8005CC94
#define RaceRecordsMenu          func_8005D104
#define AudioConfigMenu          func_8005D6F0
#define LoadAudioSliderSprites   func_8005DDD8
#define MultiplayerStartMenu     func_8005FF5C
#define RaceChallengeBriefing    func_800616DC
#define TimeTrialChallengeBriefing func_80061910
#define WeaponChallengeBriefing  func_80061BE8
#define SuperComboChallengeBriefing func_80061ED0
#define ControllerConfigMenu     func_80062240
#define CheckBootControllerPak   func_80062CF0
#define ControllerPakManagerMenu func_80063274
#define CreditsMenu              func_80063C60

#define UpdateCheatSequence      func_80027648
#define UpdatePlasma             func_80050CD0
#define RenderPlasma             func_80050F10
#define InitPlasma               func_80050BA4
#define UpdateWeaponMessage      func_80043538
#define DrawScreenOverlay        func_8004370C
#define newRand                  func_8001CFAC
#define WADLoadPrm               func_8000A458
#define ResetRenderList          func_8004F660
#define AddRenderGroup           func_8004FA30
#define FlushRenderList          func_8004F790

#define g_feInitialized          D_800950F4
#define g_pakManagerActive       D_800950F8
#define g_startupLogosActive     D_800950FC
#define g_startupWarningActive   D_80095100
#define g_medalAnimFrame         D_80095104
#define g_medalAnimTick          D_80095108
#define g_titleIdleTimer         D_8009510C
#define g_titleSeedCounter       D_80095110
#define g_titleSeedLocked        D_80095112
#define g_blinkCounter           D_80095114
#define g_pakManagerInit         D_800923F8
#define g_medalTexStart          D_80092378
#define g_medalTexCount          D_800C509C
#define g_unk8009BA0C            D_8009BA0C
#define g_unk8009BA10            D_8009BA10
#define g_unk800AA084            D_800AA084
#define cheatfade                D_80091C22
#define cheatAllChallenges       D_80091C25
#define g_idleTimer              D_800AA03C
#define g_randSeed               D_8008A978
#define g_feRenderList           D_800C4610
#define g_feModels               D_800C56C0
#define g_unk800A0EE4            D_800A0EE4
#define g_unk8009234C            D_8009234C
#define g_unk80092350            D_80092350
#define g_unk80092354            D_80092354
#define g_unk80092358            D_80092358
#define g_unk8009235C            D_8009235C
#define g_unk80092360            D_80092360
#define g_unk80092364            D_80092364
#define g_unk80092368            D_80092368
#define g_unk8009236C            D_8009236C
#define g_unk80092370            D_80092370
#define g_modelAngle             D_800923CC
#define g_modelTransforms        D_80092414
#define g_multiSlots             D_800C5758

#define InitMultiplayerSlots     func_8005DEB0
#define UpdateMultiScreenLayouts func_8005E620
#define UpdateMultiPlayerInput   func_8005E810

#define GetTrackName             func_800356C8
#define TimeToString             func_8002B91C
#define GetCdTrackName           func_8006EF10
#define GetCdTrackArtist         func_8006EF2C
#define PlaySfx                  func_800700A8
#define SetSfxFreqOffset         func_800700CC

#define g_volumeBarSprites       D_800C5EA8
#define g_font12                 D_800AC320
#define g_multi3PlayerLayout     D_80092408
#define g_multiReadyCount        D_80092400
#define cheatTeam                D_80091C29
#define g_padInfo                D_800A9FA0
#define g_contButtonPressed      D_800AA0F8

#define DrawMultiPlayerSlot      func_8005EE8C
#define ShipChoiceToShipIdx      func_8005FED4
#define DrawChallengeInfo        func_800604B4
#define AppendN64FontCode        func_80062E1C
#define DrawPakProblemPrompt     func_800630C8

#define GetButtonName            func_8000F5F4
#define AddDelEnd                func_8002C464
#define GetClassName             func_80035650
#define BuildEnvMapDisplayList   func_8000E2E4
#define AddRenderDisplayList     func_8004FB04
#define FlushSkyRenderList       func_80018024

#define g_multiFrameCounter      D_80092410
#define g_multiSlotsInit         D_80095118
#define g_briefingTextX          D_8009511C
#define g_briefingTextY          D_80095120
#define g_briefingTextStep       D_80095124
#define g_weaponPulseAlpha       D_80095128
#define g_pakMgrCursor           D_8009512C
#define g_pakMgrConfirmDelete    D_80095130
#define g_pakMgrErrorShown       D_80095134
#define g_pakMgrWarnActive       D_80095135
#define g_pakMgrNeedRefresh      D_80095138
#define g_pakMgrWarnTimer        D_8009513C
#define g_creditsLines           D_80095140
#define g_creditsTopY            D_80096700
#define g_creditsBottomY         D_80096704
#define g_creditsLineSpacing     D_80096708
#define g_creditsScrollSpeed     D_8009670C
#define g_creditsMusicInit       D_80096710
#define g_creditsFrameCount      D_80096714
#define g_envMapCachedModel      D_80096718
#define g_envMapCachedAward      D_80096720
#define g_raceChallengeBriefingMenu D_80093764
#define g_ttChallengeBriefingMenu D_80093FE4
#define g_weaponChallengeBriefingMenu D_800943E4
#define g_superComboChallengeBriefingMenu D_800947E4
#define g_creditsMenu            D_80094B64
#define g_contStatus             D_8009BC78
#define g_bootPakErr             D_800C5F14
#define g_pakMgrMaxFiles         D_800C508C
#define g_pakMgrFilesUsed        D_800C5090
#define g_pakMgrFreeBlocks       D_800C5094
#define g_pakMgrPrevButtons      D_800C5098
#define g_pakMgrSlotStatus       D_800C5680
#define g_pakMgrSlotState        D_800C5918
#define g_medalDlBuf             D_800C5BA8
#define shipShapes               D_8009BA68
#endif
/* AUTO-END */

#ifdef VERSION_EU
#define FRAME_RATE 50
#define GLIST_LEN  0x900
#else
#define FRAME_RATE 60
#define GLIST_LEN  0x880
#endif

#define STATIC_SEGMENT 1
#define CFB_SEGMENT    2
#define MTX_POOL_LEN   200

#define G_CC_DECALRGB_PRIMA 0, 0, 0, TEXEL0, TEXEL0, 0, PRIMITIVE, 0

#define g_viewports      D_1000000
#define g_rspInitDL      D_1000090
#define g_rdpInitDL      D_10000B0
#define g_clearZBufferDL D_1000130
#define g_clearCfbDL     D_1000168

#define _track1File      D_27F80
#define _track2File      D_80710
#define _track3File      D_F0F60
#define _track4File      D_17CCA0
#define _track5File      D_1ECCC0
#define _track6File      D_2669E0
#define _track7File      D_2D1DA0
#define _feSpritesWad    D_386830
#define _feSpritesCmp0   D_3DB430
#define _feSpritesCmp1   D_3E2A60
#define _feExtraModelsWad D_355F90
#define _feMedalWad      D_375E50

#define ASSERT(expr, line) ((expr) ? (void)0 : __assert(#expr, "frontend.c", line))

typedef struct Dynamic {
    Mtx projection;
    Mtx ortho;
    Mtx modeling[2];
    Gfx glist[GLIST_LEN];
    Mtx mtxPool[MTX_POOL_LEN];
} Dynamic;

typedef struct GFXInfo {
    OSScTask task[7];
    OSScTask task2D;
    OSScTask taskExtra;
    Dynamic dp;
    OSScMsg doneMsg;
    OSScMsg taskMsgs[7];
    void *cfb;
    Gfx *glist2D;
    s32 active;
    u32 flags;
} GFXInfo;

typedef struct SkeletonStub {
    /* 0x00 */ s16 m[3][3];
    /* 0x12 */ u8 unk12[0x32];
    /* 0x44 */ struct SkeletonStub *super;
    /* 0x48 */ u8 unk48[8];
} SkeletonStub; /* size = 0x50 */

typedef struct CameraStub {
    /* 0x00 */ SkeletonStub *camPos;
    /* 0x04 */ u8 data[0x18];
} CameraStub; /* size = 0x1C */

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
    /* 0x2A */ u8 unk2A[0x1E];
    /* 0x48 */ u8 pendingRecords;
    /* 0x49 */ u8 displayedRecords;
} HiScoreData;

typedef struct MenuItem {
    /* 0x00 */ s32 row;
    /* 0x04 */ char text[32];
    /* 0x24 */ u8 selected;
    /* 0x25 */ u8 enabled;
    /* 0x28 */ s32 nextScreen;
    /* 0x2C */ s32 prevScreen;
    /* 0x30 */ u8 value;
    /* 0x31 */ u8 numValues;
    /* 0x32 */ u8 pad32[2];
    /* 0x34 */ u32 trackFile;
    /* 0x38 */ u8 gameType;
    /* 0x39 */ u8 hidden;
    /* 0x3A */ u8 pad3A[2];
    /* 0x3C */ s32 modelIdx;
} MenuItem; /* size = 0x40 */

typedef struct ChallengeData {
    /* 0x00 */ u32 allowedTeams;
    /* 0x04 */ u8 teamClass;
    /* 0x05 */ u8 pad05[3];
    /* 0x08 */ s32 trackNo;
    /* 0x0C */ u32 weaponsMask;
    /* 0x10 */ f32 goldTarget;
    /* 0x14 */ f32 goldKills;
    /* 0x18 */ f32 silverTarget;
    /* 0x1C */ f32 silverKills;
    /* 0x20 */ f32 bronzeTarget;
    /* 0x24 */ f32 bronzeKills;
    /* 0x28 */ u8 award;
    /* 0x29 */ u8 unk29;
    /* 0x2A */ u8 unlocked;
    /* 0x2B */ u8 unk2B;
} ChallengeData; /* size = 0x2C */

typedef struct CreditLine {
    /* 0x00 */ s32 flags;
    /* 0x04 */ s32 y;
    /* 0x08 */ char text[52];
    /* 0x3C */ s32 extraSpacing;
} CreditLine; /* size = 0x40 */

typedef struct LeagueStanding {
    /* 0x00 */ s32 shipIdx;
    /* 0x04 */ s32 points;
} LeagueStanding; /* size = 0x08 */

typedef struct LeagueResult {
    /* 0x00 */ u8 first;
    /* 0x01 */ u8 second;
    /* 0x02 */ u8 third;
    /* 0x03 */ u8 pad03;
    /* 0x04 */ s32 eliminations;
    /* 0x08 */ s32 points;
} LeagueResult; /* size = 0x0C */

typedef struct ShipDataStub {
    /* 0x000 */ u8 unk00[0xF4];
    /* 0x0F4 */ u8 activeInLeague;
    /* 0x0F5 */ u8 unkF5[0x13];
} ShipDataStub; /* size = 0x108 */

typedef struct N64TextureStub {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 clutSize;
    /* 0x04 */ s16 bpp;
    /* 0x06 */ s16 width;
    /* 0x08 */ s16 height;
    /* 0x0A */ s16 pad0A;
    /* 0x0C */ void *pixels;
    /* 0x10 */ void *clut;
} N64TextureStub;

typedef struct WadFileStub {
    /* 0x000 */ u32 fileOffset;
    /* 0x004 */ s16 count;
    /* 0x006 */ s16 current;
} WadFileStub;

typedef struct ObjectStub {
    /* 0x00 */ u8 unk00[0x2C];
    /* 0x2C */ void *renderGroup;
    /* 0x30 */ SkeletonStub *skeleton;
    /* 0x34 */ u8 unk34[8];
    /* 0x3C */ struct ObjectStub *next;
} ObjectStub;

typedef struct ModelTransform {
    /* 0x00 */ s32 roll;
    /* 0x04 */ s32 pitch;
    /* 0x08 */ s32 y;
    /* 0x0C */ s32 z;
} ModelTransform; /* size = 0x10 */

typedef struct MultiPlayerSlot {
    /* 0x00 */ s32 joined;
    /* 0x04 */ s32 ready;
    /* 0x08 */ char defaultName[4];
    /* 0x0C */ s32 shipChoice;
    /* 0x10 */ s32 screenLayout;
    /* 0x14 */ s32 state;
    /* 0x18 */ s32 padConfig;
    /* 0x1C */ s32 savedPadConfig;
    /* 0x20 */ char nameEntered[9];
    /* 0x29 */ u8 selectedChar;
    /* 0x2A */ s16 alphabetIndex;
    /* 0x2C */ s16 nameCursor;
    /* 0x2E */ u8 dispChar[2];
    /* 0x30 */ u8 enteringName;
    /* 0x31 */ u8 firstAlphabetIndex;
    /* 0x32 */ u8 storeRecords;
    /* 0x33 */ u8 pad33;
    /* 0x34 */ s32 inputDelay;
    /* 0x38 */ s16 nameEndX;
    /* 0x3A */ u16 pad3A;
} MultiPlayerSlot; /* size = 0x3C */

typedef struct FontGlyph {
    /* 0x00 */ u16 unk00;
    /* 0x02 */ u16 unk02;
    /* 0x04 */ u16 width;
    /* 0x06 */ u8 unk06[0x1A];
} FontGlyph; /* size = 0x20 */

typedef struct PadInfoStub {
    /* 0x00 */ u8 unk00[4];
    /* 0x04 */ u16 buttons;
    /* 0x06 */ u8 unk06[8];
    /* 0x0E */ u16 stickDirs;
    /* 0x10 */ u16 unk10;
} PadInfoStub; /* size = 0x12 */

typedef struct MenuTransition {
    /* 0x00 */ s32 screen;
    /* 0x04 */ s32 timer;
} MenuTransition;

typedef struct FrontendOptions {
    /* 0x00 */ s32 team;
    /* 0x04 */ u8 teamClass;
    /* 0x08 */ u32 trackFile;
    /* 0x0C */ s32 trackNo;
    /* 0x10 */ u8 numPlayers;
    /* 0x11 */ u8 unk11[2];
    /* 0x13 */ u8 mode;
    /* 0x14 */ u8 unk14;
    /* 0x15 */ u8 challengeMode;
    /* 0x16 */ u8 challengeIndex;
    /* 0x18 */ s32 ownShip[4];
    /* 0x28 */ s32 screenLayout[4];
    /* 0x38 */ s32 contPort[4];
    /* 0x48 */ char playerNames[4][4];
    /* 0x58 */ u8 weaponsOn;
    /* 0x59 */ u8 defaultView;
    /* 0x5A */ u8 catchUp;
    /* 0x5B */ u8 checkpoints;
    /* 0x5C */ u8 computerShips;
    /* 0x5D */ u8 leagueRaces;
    /* 0x5E */ u8 cyclone;
    /* 0x5F */ u8 pad5F;
    /* 0x60 */ MenuItem *challengeMenu;
    /* 0x64 */ ChallengeData *challengeData;
    /* 0x68 */ u8 allGoldAwarded;
    /* 0x69 */ u8 allGoldSeen;
    /* 0x6A */ u8 pad6A[2];
    /* 0x6C */ s32 musicTrack;
    /* 0x70 */ s32 unk70;
    /* 0x74 */ s32 musicVol;
    /* 0x78 */ s32 sfxVol;
    /* 0x7C */ u8 unk7C;
} FrontendOptions;

typedef struct musHandleTag *musHandle;

extern FrontendOptions frontendOptions_g;
extern void *g_heap;
extern s32 g_farClip;
extern Mtx g_flipYMtx;

extern Vp g_viewports[];
extern Gfx g_rspInitDL[];
extern Gfx g_rdpInitDL[];
extern Gfx g_clearZBufferDL[];
extern Gfx g_clearCfbDL[];
extern Gfx g_initDls[];

extern GFXInfo g_gfxInfo[2];
extern Mtx *g_mtxPool;
extern s32 g_mtxCount;
extern void *g_staticSegment;
extern s32 g_frameDelayFlag;
extern CameraStub g_cameras[4];

extern MenuTransition g_menuTransition;
extern s32 g_transitionLength;
extern s32 g_menuScreen;
extern s8 g_menuItem;
extern uSprite *g_menuSprites;
extern uSprite *g_medalSprite;
extern u8 g_leagueActive;
extern u8 g_feResetFlag;
extern musHandle g_audioConfigSfx;
extern Gfx *g_feDlistStart;
extern Gfx *g_feDlistEnd;
extern s32 MaxLength;
extern s32 g_feDlistSize;
#define FRONTEND_DLIST_SIZE g_feDlistSize

extern MenuItem g_mainMenu[];
extern MenuItem g_optionsMenu[];
extern MenuItem g_audioConfigMenu[];
extern MenuItem g_controllerConfigMenu[];
extern MenuItem g_loadSaveMenu[];
extern MenuItem g_gameConfigMenu[];
extern MenuItem g_classMenu[];
extern MenuItem g_trackMenu[];
extern MenuItem g_teamMenu[];
extern MenuItem g_challengeMenu[];
extern MenuItem g_raceChallengeMenu[];
extern MenuItem g_raceChallengeTeamMenu[];
extern MenuItem g_bonusMenu[];
extern MenuItem g_leagueMenu[];
extern MenuItem g_ttChallengeMenu[];
extern MenuItem g_ttChallengeTeamMenu[];
extern MenuItem g_weaponChallengeMenu[];
extern MenuItem g_weaponChallengeTeamMenu[];
extern MenuItem g_superComboChallengeMenu[];
extern MenuItem g_superComboChallengeTeamMenu[];
extern MenuItem g_multiShipSelectMenu[];
extern MenuItem g_ttRecordsMenu[];
extern MenuItem g_raceRecordsMenu[];

extern ChallengeData g_raceChallenges[7];
extern ChallengeData g_ttChallenges[7];
extern ChallengeData g_weaponChallenges[7];
extern ChallengeData g_superComboChallenges[7];

extern u8 g_velocitarUnlocked;
extern u8 g_piranhaUnlocked;
extern u8 g_unk80094F7E;
extern s32 g_attractTimer;
extern u8 g_raceMode;
extern s32 g_gameType;
extern OSContPad g_firstContPad;
extern u8 g_unk800923F9;
extern s32 g_multiNumPlayers;
extern s32 g_multiSplitVertical;
extern u16 g_prevMenuButtons;
extern u16 g_prevMenuButtons2;
extern u16 g_menuButtonsPressed;
extern s32 g_ctrlAnimTimer;
extern s32 g_ctrlAnimDir;
extern s32 g_ctrlAnimLength;

extern s32 ownShip;
extern s32 ownShip2;
extern s32 ownShip3;
extern s32 ownShip4;
extern u16 g_cheatHistory[16];
extern u8 cheatWeapon;
extern u8 cheatDamage;
extern u8 cheatTime;
extern u8 cheatTrack;
extern s32 g_appState;
extern s32 g_loadDone;
extern s32 g_demoMode;
extern s32 g_savedPadConfig;
extern s32 g_padConfigIndex[4];
extern s32 g_savedMusicTrack;
extern s32 g_savedSfxVol;
extern s32 g_savedMusicVol;
extern u8 cheatSuperCombo;
extern s32 g_textEndX;
extern uSprite g_arrowLeftSprite;
extern uSprite g_arrowRightSprite;

extern u8 g_challengeTeamMenuInit;
extern s32 g_awardTextY;
extern LeagueStanding g_leagueStandings[15];
extern LeagueResult g_leagueResults[15];
extern u8 g_leaguePilotCount;
extern s32 g_bonusMusicDelay;
extern s32 g_bonusAllGoldMusicFlag;
extern MenuItem g_bonusCategoryMenu[];
extern MenuItem g_bonusGoldMenu[];
extern MenuItem g_bonusSuperComboMenu[];
extern MenuItem g_bonusAllGoldMenu[];
extern s32 g_bonusGoldCheckNext;
extern s32 g_bonusSuperComboCheckNext;
extern ShipDataStub *ships;

extern OSMesgQueue g_siMsgQ;
extern s16 g_unk8008A970;
extern s32 g_pakInitDelay;
extern s32 g_pakRetries;
extern s32 g_pakConfirm;
extern s32 g_pakState;
extern s32 g_pakMsgTimer;
extern u8 g_pakTouched;
extern u8 g_pakOverwriteConfirmed;
extern s32 g_quitLeaguePrompt;
extern s32 g_quitLeagueConfirm;
extern u32 g_pfsErr;
extern s32 g_pfsRepairAttempts;
extern s32 g_pfsFileNo;
extern u16 g_pfsCompanyCode;
extern u32 g_pfsGameCode;
extern u8 g_pfsGameName[16];
extern u8 g_pfsExtName[4];
extern s32 g_pfsSaveSize;
extern OSContPad g_contPads[4];
extern s32 g_rumblePresent;
extern OSPfs g_motorPfs;
extern MenuItem g_finalStandingsMenu[];
extern WadFileStub *g_feWad;
extern N64TextureStub *g_textureTable[];
extern uSprite *g_boxSprites0;
extern uSprite *g_boxSprites1;
extern uSprite *g_boxSprites2;
extern uSprite *g_boxSprites3;
extern uSprite *g_boxSprites4;
extern uSprite *g_boxSprites5;
extern uSprite *g_boxSprites6;
extern uSprite *g_boxSprites7;
extern uSprite *g_boxSprites8;
extern uSprite *g_boxSprites10;
extern uSprite *g_boxSprites11;

extern u8 _feSpritesWad[];
extern u8 _feSpritesCmp0[];
extern u8 _feSpritesCmp1[];

extern void *g_mainHeap;
extern void *g_tempHeap;
extern void *g_heapA;
extern void *g_heapB;
extern u8 g_contEnabled;
extern WadFileStub *g_startupImage;
extern WadFileStub *g_startupData;

extern u8 g_saveBuf[0x560];
extern HiScoreData g_hudState;
extern HiScore g_hiScoreTable[16][4];
extern u8 g_hiScoreLoaded;
extern s32 g_logoTimerInit;
extern s32 g_pakAutoLoadState;
extern s32 g_pakAutoLoadFind;
extern s32 g_pakAutoLoadDone;
extern s32 g_startupWarningTimer;
extern s32 g_startupWarningPhase;
extern OSTime g_logoStartTime;
extern u32 g_autoLoadErr;
extern OSTime g_oneSecond;
extern s32 g_unk8008A9DC;
extern u8 g_startupActive;
extern u8 g_allChallengesCheated;
extern s32 g_pakMissingAtBoot;
extern SkeletonStub *g_upperSkeletons[4];
extern SkeletonStub *g_feSkeleton;
extern s32 g_shipDeathCount[15];
extern u8 g_raceOverlay[];
extern u8 g_renderList[];
extern s16 g_animTextureCount;
extern uSprite g_loaderSprite;
extern s32 g_unk800AD820;

extern s32 g_feInitialized;
extern s32 g_pakManagerActive;
extern s32 g_startupLogosActive;
extern s32 g_startupWarningActive;
extern s32 g_medalAnimFrame;
extern s32 g_medalAnimTick;
extern s32 g_titleIdleTimer;
extern u16 g_titleSeedCounter;
extern u8 g_titleSeedLocked;
extern s32 g_blinkCounter;
extern u8 g_pakManagerInit;
extern s32 g_medalTexStart;
extern s32 g_medalTexCount;
extern s32 g_unk8009BA0C;
extern u8 g_unk8009BA10;
extern s32 g_unk800AA084;
extern s16 cheatfade;
extern u8 cheatAllChallenges;
extern s16 g_idleTimer;
extern u32 g_randSeed;
extern u8 g_feRenderList[];
extern ObjectStub *g_feModels[40];
extern s16 g_unk800A0EE4;
extern void *g_unk8009234C;
extern void *g_unk80092350;
extern void *g_unk80092354;
extern void *g_unk80092358;
extern Gfx *g_unk8009235C;
extern void *g_unk80092360;
extern Gfx *g_unk80092364;
extern void *g_unk80092368;
extern Gfx *g_unk8009236C;
extern void *g_unk80092370;
extern s32 g_modelAngle;
extern ModelTransform g_modelTransforms[];
extern MultiPlayerSlot g_multiSlots[4];

extern uSprite g_volumeBarSprites[4];
extern FontGlyph g_font12[];
extern u32 g_multi3PlayerLayout;
extern s32 g_multiReadyCount;
extern u8 cheatTeam;
extern PadInfoStub g_padInfo[4];
extern u16 g_contButtonPressed[4];

extern s32 g_multiFrameCounter;
extern s32 g_multiSlotsInit;
extern s32 g_briefingTextX;
extern s32 g_briefingTextY;
extern s32 g_briefingTextStep;
extern s32 g_weaponPulseAlpha;
extern s32 g_pakMgrCursor;
extern s32 g_pakMgrConfirmDelete;
extern u8 g_pakMgrErrorShown;
extern u8 g_pakMgrWarnActive;
extern s32 g_pakMgrNeedRefresh;
extern s32 g_pakMgrWarnTimer;
extern CreditLine g_creditsLines[87];
extern s32 g_creditsTopY;
extern s32 g_creditsBottomY;
extern s32 g_creditsLineSpacing;
extern s32 g_creditsScrollSpeed;
extern s32 g_creditsMusicInit;
extern s32 g_creditsFrameCount;
extern s32 g_envMapCachedModel;
extern u8 g_envMapCachedAward;
extern MenuItem g_raceChallengeBriefingMenu[];
extern MenuItem g_ttChallengeBriefingMenu[];
extern MenuItem g_weaponChallengeBriefingMenu[];
extern MenuItem g_superComboChallengeBriefingMenu[];
extern MenuItem g_creditsMenu[];
extern OSContStatus g_contStatus[4];
extern s32 g_bootPakErr;
extern s32 g_pakMgrMaxFiles;
extern s32 g_pakMgrFilesUsed;
extern s32 g_pakMgrFreeBlocks;
extern u16 g_pakMgrPrevButtons;
extern s32 g_pakMgrSlotStatus[16];
extern OSPfsState g_pakMgrSlotState[16];
extern Gfx g_medalDlBuf[];
extern ObjectStub *shipShapes[5];

extern u8 _feExtraModelsWad[];
extern u8 _feMedalWad[];

void *memcpy(void *dst, const void *src, unsigned int len);
char *strcpy(char *dst, const char *src);
char *StringCopy(char *dst, const char *src);
char *StringCat(char *dst, const char *src);
char *GetButtonName(u16 button, u8 configIdx);
Gfx *AddDelEnd(u8 icon, s32 x, s32 y, s32 size, s32 colour, Gfx *glistp);
char *GetClassName(s16 classIdx);
void BuildEnvMapDisplayList(ObjectStub *obj, Dynamic *dp, s32 modelIdx, u8 lightType, void **vtxOut, Gfx **dlOut);
void AddRenderDisplayList(void *list, Gfx **dlPtr, Mtx *mtx);
Gfx *FlushSkyRenderList(void *list, Gfx *glistp);

Gfx *DrawMultiPlayerSlot(s32 playerIdx, Gfx *glistp);
s32 ShipChoiceToShipIdx(u32 choice);
Gfx *DrawChallengeInfo(MenuItem *menu, Gfx *glistp);
void AppendN64FontCode(s32 code, char *out);
void DrawPakProblemPrompt(void);
char *GetTrackName(s16 trackIdx);
void TimeToString(s32 time, s32 flag, char *out);
char *GetCdTrackName(s32 track);
char *GetCdTrackArtist(s32 track);
musHandle PlaySfx(s32 number, s32 volume, s32 pan, s32 restartflag, s32 priority);
s32 SetSfxFreqOffset(musHandle handle, f32 offset);
void DInit(void *heap, s32 size);
WadFileStub *OpenWadFile(u32 fileOffset);
void CloseWadFile(WadFileStub *wad);
void InitFontSize(s32 size);
void SetSkeletonDirectionHPR(SkeletonStub *sk, s16 h, s16 p, s16 r);
void *GetSkeletonMatrix(SkeletonStub *sk);
void MatrixToN64Mtx(void *mat, Mtx *mtx);

void UpdateCheatSequence(s32 arg0);
void UpdatePlasma(void);
Gfx *RenderPlasma(Gfx *glistp);
void InitPlasma(void);
void UpdateWeaponMessage(s32 player);
Gfx *DrawScreenOverlay(s32 player, s32 fullScreen, Gfx *glistp, u8 isDim);
s32 newRand(void);
ObjectStub *WADLoadPrm(WadFileStub *wad, void *renderList);
void ResetRenderList(void *list);
void AddRenderGroup(void *list, void *group, Mtx *mtx);
Gfx *FlushRenderList(void *list, Gfx *glistp);

void FrontendUpdate(void);
void StartAttractDemo(void);
Gfx *TitleScreen(Gfx *glistp);
void UpdateBlink(void);
void LoadFrontendModels(void);
void CheckAllGoldInitial(void);
void LoadSpinningMedal(void);
void InitFrontend(void);
Gfx *TimeTrialRecordsMenu(Gfx *glistp);
Gfx *RaceRecordsMenu(Gfx *glistp);
Gfx *AudioConfigMenu(Gfx *glistp);
void LoadAudioSliderSprites(void);
void InitMultiplayerSlots(void);
void UpdateMultiScreenLayouts(void);
void UpdateMultiPlayerInput(s32 playerIdx);
Gfx *MultiplayerStartMenu(Gfx *glistp);
Gfx *RaceChallengeBriefing(Gfx *glistp);
Gfx *TimeTrialChallengeBriefing(Gfx *glistp);
Gfx *WeaponChallengeBriefing(Gfx *glistp);
Gfx *SuperComboChallengeBriefing(Gfx *glistp);
Gfx *ControllerConfigMenu(Gfx *glistp);
u8 CheckBootControllerPak(OSMesgQueue *siMsgQ);
s32 ControllerPakManagerMenu(OSMesgQueue *siMsgQ);
Gfx *CreditsMenu(Gfx *glistp);

SkeletonStub *NewSkeleton(void);
void SetSkeletonPosition(SkeletonStub *sk, s32 x, s32 y, s32 z);
void SetSkeletonDirection(SkeletonStub *sk, s32 x, s32 y, s32 z);
void InitPadSettings(void);
void InitRaceResources(void);
void ScoreTabInit(void *config);
void InitRenderList(void *list, u32 textureStart, u32 bucketCount);
void InitInGamePlasmaPalette(void);

void *DAlloc(void *heap, s32 size, const char *file, s32 line);
WadFileStub *OpenWadFileNvc(u32 fileOffset);
void CloseWadFileNvc(WadFileStub *wad);
void ReinitCont(void);
Gfx *DrawFontTextYellow(char *text, s32 x, s32 y, s32 size, Gfx *glistp);
Gfx *CentreTextAt(char *text, s32 x, s32 y, s32 size, s32 colour, Gfx *glistp);
void *LoadCompressedTextureSequence(u32 fileOffset);
void *WADLoadCompressedTextureSequence(WadFileStub *wad);
void LoadCmpFiles(void *timList);

extern u8 _track1File[];
extern u8 _track2File[];
extern u8 _track3File[];
extern u8 _track4File[];
extern u8 _track5File[];
extern u8 _track6File[];
extern u8 _track7File[];

void __assert(const char *expr, const char *file, s32 line);
int sprintf(char *dst, const char *fmt, ...);
char *StringAlloc(const char *str);
void DFree(void *heap, void *ptr);

void StopSfx(musHandle *handle);
void SetShake(s32 amount, s32 player);
void PlayNote(s32 sound, s32 volume, s32 lrPan, s32 fbPan, s32 pitch);
Gfx *DrawButtonPrompt(s16 x, s16 y, s16 type, char *text, Gfx *glistp);
Gfx *DrawHudText(char *text, s32 x, s32 y, s32 size, s32 colour, Gfx *glistp);
Gfx *CentreText(char *text, s32 x, s32 y, s32 size, s32 colour, Gfx *glistp);
void SetupGfxTask(GFXInfo *info, s32 taskIdx, Gfx *glistStart, Gfx *glistEnd);
void Gfx2DMenuTask(Gfx *glistStart, GFXInfo *info, s32 mode);
void ResumeAudio(void);
void StopAudio(void);
void SetMasterMusicVolume(s32 volume);
void SetMasterSfxVolume(s32 volume);
void InitPlasmaPaletteGold(void);
Gfx *AddCentreText(char *text, s32 x, s32 y, s32 size, Gfx *glistp);
Gfx *DrawFontText(char *text, s32 x, s32 y, s32 size, Gfx *glistp);
Gfx *AddNumber(s32 num, s32 x, s32 y, s32 size, s32 colour, Gfx *glistp);
void StartMusic(void);

void SetMenuTransition(s32 screen);
Gfx *DrawMenuFrame(MenuItem *menu, Gfx *glistp, u8 drawHeader);
Gfx *AddFrontendDisplayList(Gfx *glistp);
void CreateFrontendGfxTask(s32 frameIdx);
Gfx *DrawMenu(MenuItem *menu, s32 x, s32 y, Gfx *glistp, u8 drawHeader);
Gfx *MainMenu(Gfx *glistp);
void StartGame(void);
u8 MenuControl(MenuItem *menu);
u8 MenuControlMulti(MenuItem *menu, s32 lockVertical);
Gfx *TrackSelectMenu(Gfx *glistp);
Gfx *ChallengeSelectMenu(Gfx *glistp);
void ApplyGameConfig(void);
void SaveControllerConfig(void);
void RestoreControllerConfig(void);
void ApplyAudioConfig(void);
void RestoreAudioConfig(void);
void RestoreGameConfig(void);
Gfx *OptionsMenu(Gfx *glistp);
Gfx *GameConfigMenu(Gfx *glistp);
u32 GetTrackFileOffset(s32 trackNo);
s32 UnlockNextChallenge(MenuItem *menu, ChallengeData *challenges, s32 idx);
u8 CheckChallengeCategoryComplete(MenuItem *menu, ChallengeData *challenges);
u8 CheckSuperComboUnlocked(MenuItem *menu, ChallengeData *challenges);
u8 CheckAllGoldAwarded(MenuItem *menu, ChallengeData *challenges);
u8 CheckGoldChallengeUnlocked(MenuItem *menu, ChallengeData *challenges);
Gfx *RaceChallengeMenu(Gfx *glistp);
Gfx *TimeTrialChallengeMenu(Gfx *glistp);
Gfx *WeaponChallengeMenu(Gfx *glistp);
Gfx *SuperComboChallengeMenu(Gfx *glistp);
Gfx *ClassSelectMenu(Gfx *glistp);
Gfx *TeamSelectMenu(Gfx *glistp);
Gfx *TimeTrialChallengeTeamMenu(Gfx *glistp);
void InitChallengeTeamMenu(MenuItem *menu, ChallengeData *challenges);
Gfx *RaceChallengeTeamMenu(Gfx *glistp);
Gfx *DrawChallengeCategoryBonus(ChallengeData *challenges, Gfx *glistp);
Gfx *DrawGoldChallengeBonus(ChallengeData *challenges, Gfx *glistp);
Gfx *DrawSuperComboBonus(ChallengeData *challenges, Gfx *glistp);
Gfx *DrawAllGoldBonus(ChallengeData *challenges, Gfx *glistp);
void SwapLeagueStandings(LeagueStanding *standings, s32 idx);
char *GetPilotName(s32 shipIdx, char *out);
Gfx *LeagueStandingsMenu(Gfx *glistp);
Gfx *BonusCategoryScreen(Gfx *glistp);
Gfx *BonusCategoryCheck(Gfx *glistp);
Gfx *BonusGoldScreen(Gfx *glistp);
Gfx *BonusGoldCheck(Gfx *glistp);
Gfx *BonusSuperComboScreen(Gfx *glistp);
Gfx *BonusSuperComboCheck(Gfx *glistp);
Gfx *BonusAllGoldScreen(Gfx *glistp);
Gfx *WeaponChallengeTeamMenu(Gfx *glistp);
Gfx *SuperComboChallengeTeamMenu(Gfx *glistp);
Gfx *FrontendStub_80057520(Gfx *glistp);
Gfx *ControllerPakMenu(Gfx *glistp);
Gfx *QuitLeagueMenu(Gfx *glistp);
Gfx *FinalStandingsMenu(Gfx *glistp);
uSprite *LoadFrontendSprites(s32 cols, s32 rows, u32 kind);
Gfx *DrawMenuBox(s32 cols, s32 rows, s32 x, s32 y, s32 w, s32 h, u32 kind, Gfx *glistp);
void InitFrontendSprites(void);
u8 InitControllerPak(void);
u8 SaveControllerPakData(u8 savePad, u8 saveAudio, u8 saveGame, u8 saveChallenges, u8 unused);
u8 LoadControllerPakData(u8 loadPad, u8 loadAudio, u8 loadGame, u8 loadChallenges, u8 unused);
void PackAudioSaveData(void);
void PackGameAndScoresSaveData(void);
void PackControllerSaveData(void);
void PackChallengeSaveData(void);
void UnpackAudioSaveData(void);
void UnpackGameAndScoresSaveData(void);
void UnpackControllerSaveData(void);
void RefreshChallengeUnlocks(void);
void UnpackChallengeSaveData(void);
void StartupLogosAndAutoLoad(void);
void UnlockAllChallengesCheat(void);
u8 CheckControllerDetected(void);
u8 StartupWarningAndInit(void);
void CleanupStartupLogos(void);
Gfx *DrawShipStats(s8 shipIdx, Gfx *glistp);
u8 GetBlinkState(void);

Gfx *RenderMenu3DModel(MenuItem *menu, Gfx *glistp);
Gfx *RenderMultiShipModel(MenuItem *menu, CameraStub *cam, s32 playerIdx, Gfx *glistp);
Gfx *SetupMultiViewport(s32 playerIdx, Dynamic *dp, u16 *perspNorm, Gfx *glistp);
Gfx *RenderController3DModel(MenuItem *menu, Gfx *glistp);
void InitCredits(void);
Gfx *RenderChallengeMedal3D(MenuItem *menu, Dynamic *dp, s32 arg2, Gfx *glistp);
Gfx *RenderFinalStandings3D(Dynamic *dp, Gfx *glistp);

#endif /* WIPEOUT64_FRONTEND_H */
