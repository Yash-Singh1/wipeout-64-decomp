#ifndef WIPEOUT64_CPU_H
#define WIPEOUT64_CPU_H

#include "common.h"
#include "dheap.h"
#include "gte.h"
#include "skeleton.h"
#include "wadfile.h"
#ifndef WIPEOUT64_RADSCN_H
#define FrontendOptions FrontendOptionsHead
#define frontendOptions_g frontendOptionsHead_g
#include "radscn.h"
#undef FrontendOptions
#undef frontendOptions_g
#endif
#include "gfx.h"
#include "n64render.h"
#include <ultra64.h>
#include <PR/sched.h>
#include <PR/ucode.h>

void *memcpy(void *dst, const void *src, size_t len);

typedef struct FrontendOptions {
    /* 0x00 */ s32 team;
    /* 0x04 */ u8 teamClass;
    /* 0x05 */ u8 unk05[3];
    /* 0x08 */ u32 trackFile;
    /* 0x0C */ s32 trackNo;
    /* 0x10 */ u8 numPlayers;
    /* 0x11 */ u8 unk11[4];
    /* 0x15 */ u8 gameMode;
    /* 0x16 */ u8 challengeNo;
    /* 0x17 */ u8 unk17;
    /* 0x18 */ s32 ownShips[4];
    /* 0x28 */ s32 splitScreenMode;
    /* 0x2C */ u8 unk2C[0xC];
    /* 0x38 */ s32 controllerPort[4];
    /* 0x48 */ u8 unk48[0x10];
    /* 0x58 */ u8 weaponsOn;
    /* 0x59 */ u8 cameraView;
} FrontendOptions;

typedef struct TrackCamera {
    /* 0x00 */ Skeleton *camPos;
    /* 0x04 */ TrackSection *section;
    /* 0x08 */ s16 hdg;
    /* 0x0A */ s16 pitch;
    /* 0x0C */ s16 roll;
    /* 0x0E */ s16 vhdg;
    /* 0x10 */ s16 vpitch;
    /* 0x12 */ s16 vroll;
    /* 0x14 */ SVECTOR vel;
} TrackCamera; /* size = 0x1C */

typedef struct ShipData {
    /* 0x000 */ u8 unk00[0x10];
    /* 0x010 */ s32 attr;
    /* 0x014 */ VECTOR unitVecNose;
    /* 0x024 */ u8 unk24[0x20];
    /* 0x044 */ VECTOR ppivot;
    /* 0x054 */ VECTOR vpivot;
    /* 0x064 */ u8 unk64[0x14];
    /* 0x078 */ s16 roll;
    /* 0x07A */ s16 vhdg;
    /* 0x07C */ u8 unk7C[0x34];
    /* 0x0B0 */ u16 combatAttr;
    /* 0x0B2 */ u8 unkB2[0x0C];
    /* 0x0BE */ u8 weaponType;
    /* 0x0BF */ u8 unkBF[0x49];
} ShipData; /* size = 0x108 */

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

typedef struct PadInfo {
    /* 0x00 */ u16 unk00;
    /* 0x02 */ u16 pad;
    /* 0x04 */ u16 xpad;
    /* 0x06 */ u16 opad;
    /* 0x08 */ u8 unk08;
    /* 0x09 */ s8 stickX;
    /* 0x0A */ s8 stickY;
    /* 0x0B */ u8 unk0B;
    /* 0x0C */ u16 stickPad;
    /* 0x0E */ u16 xstickPad;
    /* 0x10 */ u16 ostickPad;
} PadInfo; /* size = 0x12 */

typedef struct ChallengeData {
    /* 0x00 */ u8 unk00[0x0C];
    /* 0x0C */ s32 weaponsEnabled;
    /* 0x10 */ u8 unk10[0x1C];
} ChallengeData; /* size = 0x2C */

typedef struct Electric {
    /* 0x00 */ VECTOR start;
    /* 0x10 */ VECTOR end;
    /* 0x20 */ s32 life;
    /* 0x24 */ s32 mode;
#ifdef VERSION_EU
    /* 0x28 */ u8 unk28[0x180];
#else
    /* 0x28 */ u8 unk28[0x100];
#endif
} Electric; /* size = 0x128 (US) / 0x1A8 (EU) */

typedef void (*CameraUpdateFunc)(TrackCamera *, ShipData *, void *, s32);

typedef struct RescueData {
    /* 0x00 */ u8 unk00[0x48];
    /* 0x48 */ Gfx *(*update)(struct RescueData *, ShipData *, Object **, s32, Gfx *);
} RescueData; /* size = 0x4C */

typedef struct RaceOverlayState {
    /* 0x00 */ u8 unk00[0x1A];
    /* 0x1A */ s16 unk1A;
} RaceOverlayState;

extern FrontendOptions frontendOptions_g;

#define CPU_THREAD_ID       2
#define CPU_THREAD_PRI      9
#define CPU_STACKSIZE       0x4000
#define NUM_FRAME_MSGS      4
#define NUM_RETRACE_MSGS    4

#define GFX_TASK_3D         2
#define GFX_TASK_CLEAR      6

#define STATIC_SEGMENT      1
#define CFB_SEGMENT         2
#define RDP_FIFO_LEN        0x80

#ifdef VERSION_EU
#define EFFECT_DL_LEN        750
#define IDLE_LIMIT_FRAMES    3000 /* 60 seconds at 50 Hz (PAL) */
#define DEATH_UNRESCUE_FRAME 5
#define SPURT_MOD            5
#else
#define EFFECT_DL_LEN        512
#define IDLE_LIMIT_FRAMES    3600 /* 60 seconds at 60 Hz (NTSC) */
#define DEATH_UNRESCUE_FRAME 6
#define SPURT_MOD            6
#endif
#define LINE_DL_LEN         1536

/* ShipData.attr / ShipData.combatAttr flags used in UpdateRace. */
#define SHIP_ATTR_VIEW_IN   0x0004
#define SHIP_ATTR_RACING    0x0040
#define SHIP_DESTROYED      0x8000

#define OBJ_FLAG_TRANSLUCENT    0x0008

#define NUM_SLIDERS         3
#define NUM_HOVERERS        2
#define NUM_SPINNERS        15
#define NUM_ROTORS          3

/* Entries of the effect texture WAD used by the animated scenery. */
#define EFFECT_ANIM_TEXTURES    4
#define EFFECT_SPINNER_MODELS   5
#define EFFECT_FLIP_TEXTURES    6
#define EFFECT_ANIM_TEXTURES2   7
#define EFFECT_SPINNER_TEXTURES 8

/* Primitive types used by the animated scenery objects. */
#define TypeFT4 4
#define TypeGT4 8

typedef struct FT4 {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
    /* 0x04 */ s16 coords[4];
    /* 0x0C */ s16 texture;
    /* 0x0E */ u16 cba;
    /* 0x10 */ u16 tsb;
    /* 0x12 */ u8 u0, v0, u1, v1, u2, v2, u3, v3;
    /* 0x1A */ s16 pad1;
    /* 0x1C */ u32 colour;
} FT4; /* size = 0x20 */

typedef struct GT4 {
    /* 0x00 */ s16 type;
    /* 0x02 */ s16 flag;
    /* 0x04 */ s16 coords[4];
    /* 0x0C */ s16 texture;
    /* 0x0E */ u16 cba;
    /* 0x10 */ u16 tsb;
    /* 0x12 */ u8 u0, v0, u1, v1, u2, v2, u3, v3;
    /* 0x1A */ s16 pad1;
    /* 0x1C */ u32 colour[4];
} GT4; /* size = 0x2C */

typedef union Primitive {
    s16 *type;
    FT4 *ft4;
    GT4 *gt4;
} Primitive;

/*
 * Temporary US/EU symbol-name mappings for symbols not yet named in
 * config/{us,eu}/symbol_addrs.txt. Remove once canonical names are added there.
 */
#ifdef VERSION_EU
#define _effectTexturesFile     D_375E50
#define g_gameState             D_80091730
#define g_gameType              D_800B7604
#define g_playerFinished        D_800A8FF8
#define g_sceneRenderList       D_800C4860
#define g_scene2RenderList      D_800C48F0
#define g_sceneAnimFrame        D_800BF324
#define g_spinnerAngles         D_800BF330
#define g_cpuThreadStack        D_800BF350
#define g_cpuThread             D_800C3350
#define g_retraceMsgs           D_800C3500
#define g_animTex4Start         D_800C3510
#define g_animTex2Count         D_800C3530
#define g_animTex3Start         D_800C3534
#define g_sparkPoints           D_800C3538
#define g_animTexCount          D_800C353C
#define g_frameMsgs             D_800C3540
#define g_animTex2Start         D_800C3568
#define g_animTex4Count         D_800C356C
#define g_animTex3Count         D_800C3570
#define g_animTexStart          D_800C3778
#define g_hoverObjects          D_80091744
#define g_sliderObjects         D_8009174C
#define g_spinnerObjects        D_80091758
#define g_sliderBaseX           D_80091794
#define g_sliderBaseY           D_800917A0
#define g_sliderBaseZ           D_800917AC
#define g_hoverBaseX            D_800917B8
#define g_hoverBaseY            D_800917C0
#define g_hoverBaseZ            D_800917C8
#define g_animTexTarget         D_800917D0
#define g_animTex2Target        D_800917D4
#define g_animTex3Target        D_800917D8
#define g_sparkPointCount       D_800917DC
#define g_animTex4Target        D_800917E0
#define g_rotorObjects          D_800917E8
#define g_rotorAngle            D_80091820
#define g_rotorCount            D_80091824
#define g_animTexFrame          D_80091828
#define g_animTexDelay          D_80091830
#define g_animTexTimer          D_80091838
#define g_sliderTimer           D_80091840
#define g_sliderYOffset         D_8009184C
#define g_spinnerSpeed          D_80091850
#define g_spinnerPitch          D_80091854
#define g_spinnerRoll           D_80091858
#define g_animTex2Frame         D_8009185C
#define g_animTex2Delay         D_80091860
#define g_animTex2Timer         D_80091864
#define g_hoverTimer            D_80091868
#define g_hoverDuration         D_80091870
#define g_animTex3Timer         D_80091878
#define g_animTex3Frame         D_8009187C
#define g_animTex4Timer         D_80091880
#define g_animTex4Frame         D_80091884
#define g_colorGammaLut         D_80091630
#define particleSlots           D_800B7698
#define g_rdpFifo               D_800A6FB0
#define g_yieldData             D_800A5DC0
#define g_dramStack             D_800F7FF0
#define g_multiplayerFlag       D_80095F44
#define g_spinnerRenderList     D_800C4848
#define g_delayCycles           D_800977D0
#define g_loadDone              D_8008A008
#define g_frontendLoaded        D_8008A00C
#define challengeMedalObject    D_8008A010
#define g_sceneryNearClip       D_80091274
#define g_sceneryFarClip        D_80091294
#define g_medalDL               D_800912C8
#define g_medalVtx              D_800912CC
#define g_challengeMedalLevel   D_80091434
#define g_pauseDebounceTimer    D_800917E4
#define g_engineSfxPlaying      D_800917F6
#define g_ambientSfxTimer       D_800917F8
#define g_curFrameIdx           D_800917FC
#define g_stateDelay            D_80091800
#define g_firstBoot             D_80091804
#define g_raceReady             D_80091808
#define g_medalAngle            D_80091888
#define g_lastMedalLevel        D_8009188C
#define g_shieldSfx             D_800968A8
#define g_pitSfxHandle          D_800968AC
#define g_droidVoice            D_800968B0
#define g_chargeSfxHandle       D_800968BC
#define g_mtxPool               D_800AE4C0
#define g_mtxIndex              D_800AE514
#define trackIndex              D_800B340C
#define g_attractMode           D_800B7660
#define g_contButtonPressed     D_800B76C8
#define g_trackDListPtr         D_800B842C
#define g_sceneryPrimCount      D_800B8530
#define g_medalCamSkel          D_800B87FC
#define g_multiPlayer           D_8008A000
#define g_trackBufIdx           D_8008A4D4
#define g_trackVtxCursor        D_8008A4D8
#define g_frameDelayFlag        D_80091818
#define skyVisible              D_80095CE0
#define shipShapes              D_800A9048
#define g_hudShowPause          D_800A9294
#define rescueData              D_800A9310
#define weaponData              D_800A94BC
#define ownShip                 D_800B3414
#define ownShip2                D_800B756C
#define ownShip3                D_800B7570
#define ownShip4                D_800B7574
#define weaponShapes            D_800B7498
#define g_raceOverlay           D_800B74E0
#define g_pauseState            D_800B7550
#define ships                   D_800B767C
#define g_trackDLists           D_800B8430
#define g_medalDList            D_800C3578
#define g_medalRenderList       D_800C47E8
#define g_trailRenderList       D_800C4830
#define g_effectRenderList      D_800C48A8
#define g_lineRenderList        D_800C4908
#define g_skyDlPool             D_800C65A8
#define g_cameras               D_800CBE30
#define g_effectDLists          D_80098F20
#define g_trailDLists           D_8009EF08
#define noWeaponGrids           D_80089FC4
#define g_raceCompleteChoice    D_80089FC8
#define g_enteringHiScore       D_80089FFC
#define g_startDroidTexBase     D_800912DC
#define g_resultSfxPending           D_800917F4
#define g_timeoutSfxPending           D_800917F5
#define g_cameraUpdated           D_8009180C
#define g_pausePlayer           D_80091814
#define challenges              D_80094194
#define RaceCompleteDList       D_80095FF8
#define RaceCompleteDListPtr    D_80095FFC
#define checkPoints             D_800A8FC8
#define weaponGrid              D_800A8FF4
#define g_droidLightObj         D_800A90FC
#define startDroidObjects       D_800A9200
#define g_hudState              D_800A9270
#define lightGroups             D_800AE4C8
#define CameraUpdate            D_800AE624
#define CameraUpdate2           D_800B33F8
#define CameraUpdate3           D_800B33FC
#define CameraUpdate4           D_800B3400
#define attractDelayFrames      D_800B3408
#define g_shipDeathCount        D_800B74A0
#define g_padInfo               D_800B7580
#define pauseRequests           D_800B75F0
#define g_raceEndState          D_800B7614
#define resetData               D_800B7640
#define g_restartRaceFlag       D_800B7680
#define g_idleLimit             D_800BF320
#define electrics               D_800EA478
#else
#define _effectTexturesFile     D_375E50
#define g_gameState             D_80092180
#define g_gameType              D_800AA024
#define g_playerFinished        D_8009BA18
#define g_sceneRenderList       D_800C4520
#define g_scene2RenderList      D_800C45B0
#define g_sceneAnimFrame        D_800BEFE4
#define g_spinnerAngles         D_800BEFF0
#define g_cpuThreadStack        D_800BF010
#define g_cpuThread             D_800C3010
#define g_retraceMsgs           D_800C31C0
#define g_animTex4Start         D_800C31D0
#define g_animTex2Count         D_800C31F0
#define g_animTex3Start         D_800C31F4
#define g_sparkPoints           D_800C31F8
#define g_animTexCount          D_800C31FC
#define g_frameMsgs             D_800C3200
#define g_animTex2Start         D_800C3228
#define g_animTex4Count         D_800C322C
#define g_animTex3Count         D_800C3230
#define g_animTexStart          D_800C3438
#define g_hoverObjects          D_80092194
#define g_sliderObjects         D_8009219C
#define g_spinnerObjects        D_800921A8
#define g_sliderBaseX           D_800921E4
#define g_sliderBaseY           D_800921F0
#define g_sliderBaseZ           D_800921FC
#define g_hoverBaseX            D_80092208
#define g_hoverBaseY            D_80092210
#define g_hoverBaseZ            D_80092218
#define g_animTexTarget         D_80092220
#define g_animTex2Target        D_80092224
#define g_animTex3Target        D_80092228
#define g_sparkPointCount       D_8009222C
#define g_animTex4Target        D_80092230
#define g_rotorObjects          D_80092238
#define g_rotorAngle            D_80092270
#define g_rotorCount            D_80092274
#define g_animTexFrame          D_80092278
#define g_animTexDelay          D_80092280
#define g_animTexTimer          D_80092288
#define g_sliderTimer           D_80092290
#define g_sliderYOffset         D_8009229C
#define g_spinnerSpeed          D_800922A0
#define g_spinnerPitch          D_800922A4
#define g_spinnerRoll           D_800922A8
#define g_animTex2Frame         D_800922AC
#define g_animTex2Delay         D_800922B0
#define g_animTex2Timer         D_800922B4
#define g_hoverTimer            D_800922B8
#define g_hoverDuration         D_800922C0
#define g_animTex3Timer         D_800922C8
#define g_animTex3Frame         D_800922CC
#define g_animTex4Timer         D_800922D0
#define g_animTex4Frame         D_800922D4
#define g_colorGammaLut         D_80092080
#define particleSlots           D_800AA0C4
#define g_rdpFifo               D_800999D0
#define g_yieldData             D_800987E0
#define g_dramStack             D_800FF0E0
#define g_multiplayerFlag       D_80096994
#define g_spinnerRenderList     D_800C4508
#define g_delayCycles           D_80098230
#define g_loadDone              D_8008A9D8
#define g_frontendLoaded        D_8008A9DC
#define challengeMedalObject    D_8008A9E0
#define g_sceneryNearClip       D_80091C44
#define g_sceneryFarClip        D_80091C64
#define g_medalDL               D_80091C98
#define g_medalVtx              D_80091C9C
#define g_challengeMedalLevel   D_80091E04
#define g_pauseDebounceTimer    D_80092234
#define g_engineSfxPlaying      D_80092246
#define g_ambientSfxTimer       D_80092248
#define g_curFrameIdx           D_8009224C
#define g_stateDelay            D_80092250
#define g_firstBoot             D_80092254
#define g_raceReady             D_80092258
#define g_medalAngle            D_800922D8
#define g_lastMedalLevel        D_800922DC
#define g_shieldSfx             D_800972F8
#define g_pitSfxHandle          D_800972FC
#define g_droidVoice            D_80097300
#define g_chargeSfxHandle       D_8009730C
#define g_mtxPool               D_800A0EE0
#define g_mtxIndex              D_800A0F34
#define trackIndex              D_800A5E30
#define g_attractMode           D_800AA088
#define g_contButtonPressed     D_800AA0F8
#define g_trackDListPtr         D_800AAE5C
#define g_sceneryPrimCount      D_800AAF60
#define g_medalCamSkel          D_800AB22C
#define g_multiPlayer           D_8008A9D0
#define g_trackBufIdx           D_8008AEA4
#define g_trackVtxCursor        D_8008AEA8
#define g_frameDelayFlag        D_80092268
#define skyVisible              D_80096730
#define shipShapes              D_8009BA68
#define g_hudShowPause          D_8009BCB4
#define rescueData              D_8009BD30
#define weaponData              D_8009BEDC
#define ownShip                 D_800A5E2C
#define ownShip2                D_800A9F8C
#define ownShip3                D_800A9F90
#define ownShip4                D_800A9F94
#define weaponShapes            D_800A9EB8
#define g_raceOverlay           D_800A9F00
#define g_pauseState            D_800A9F70
#define ships                   D_800AA0A4
#define g_trackDLists           D_800AAE60
#define g_lineDLists            D_800AEA20
#define g_medalDList            D_800C3238
#define g_medalRenderList       D_800C44A8
#define g_trailRenderList       D_800C44F0
#define g_effectRenderList      D_800C4568
#define g_lineRenderList        D_800C45C8
#define g_skyDlPool             D_800C6268
#define g_cameras               D_800CBAF0
#define g_effectDLists          D_800F0640
#define g_trailDLists           D_800F4868
#define noWeaponGrids           D_8008A994
#define g_raceCompleteChoice    D_8008A998
#define g_enteringHiScore       D_8008A9CC
#define g_startDroidTexBase     D_80091CAC
#define g_resultSfxPending           D_80092244
#define g_timeoutSfxPending           D_80092245
#define g_cameraUpdated           D_8009225C
#define g_pausePlayer           D_80092264
#define challenges              D_80094BE4
#define RaceCompleteDList       D_80096A48
#define RaceCompleteDListPtr    D_80096A4C
#define checkPoints             D_8009B9E8
#define weaponGrid              D_8009BA14
#define g_droidLightObj         D_8009BB1C
#define startDroidObjects       D_8009BC20
#define g_hudState              D_8009BC90
#define lightGroups             D_800A0EE8
#define CameraUpdate            D_800A1044
#define CameraUpdate2           D_800A5E18
#define CameraUpdate3           D_800A5E1C
#define CameraUpdate4           D_800A5E20
#define attractDelayFrames      D_800A5E28
#define g_shipDeathCount        D_800A9EC0
#define g_padInfo               D_800A9FA0
#define pauseRequests           D_800AA010
#define g_raceEndState          D_800AA038
#define resetData               D_800AA068
#define g_restartRaceFlag       D_800AA0A8
#define g_idleLimit             D_800BEFE0
#define electrics               D_800E9938
#endif

/* Static display list segment (segment 1) symbols */
#define g_viewports             D_1000000
#define g_vpLeftHalf            D_1000010
#define g_vpTopHalf             D_1000030
#define g_vpTopLeft             D_1000050
#define g_vpTopRight            D_1000060
#define g_vpBottomLeft          D_1000070
#define g_vpBottomRight         D_1000080
#define g_rspInitDL             D_1000090
#define g_rdpInitDL             D_10000B0
#define g_clearZBufferDL        D_1000130
#define g_clearCfbDL            D_1000168

#ifdef VERSION_EU
#define UpdateAmbientSfx        func_8004BFC0
#define CpuThreadEntry          func_8004C0CC
#define UpdateRace              func_8004C540
#define SetupRaceViewports      func_8004DA04
#define RenderRace              func_8004E03C
#define SetupGfxTask            func_8004F288
#define InitGfxBuffer           func_8004F378
#define UpdateRotors            func_8004F46C
#define AddRotor                func_8004F500
#define LoadAnimTextures        func_8004F538
#define ResetSliders            func_8004F5BC
#define AddSlider               func_8004F5EC
#define InitAnimTexObject       func_8004F654
#define UpdateAnimTexture       func_8004F790
#define UpdateSlider            func_8004F8B0
#define UpdateSliders           func_8004FA78
#define ResetSpinners           func_8004FBD8
#define UpdateSpinner           func_8004FC00
#define UpdateSpinners          func_8004FD7C
#define LoadSpinners            func_8004FDD4
#define LoadAnimTextures2       func_8004FF54
#define InitAnimTex2Object      func_8004FFD8
#define UpdateAnimTexture2      func_80050114
#define ResetHoverers           func_80050200
#define UpdateHoverer           func_80050230
#define AddHoverer              func_80050704
#define UpdateHoverers          func_8005078C
#define LoadFlipTextures        func_80050804
#define AddSparkPoint           func_800508DC
#define InitAnimTex3Object      func_800509CC
#define ResetSparkPoints        func_80050A68
#define UpdateAnimTexture3      func_80050A88
#define UpdateSparks            func_80050B68
#define LoadAnimTextures4       func_80050C44
#define InitAnimTex4Object      func_80050CC8
#define ResetAnimTexture4       func_80050D70
#define UpdateAnimTexture4      func_80050D80
#define UpdateAnimTextures4     func_80050E64
#define DrawSpinners            func_80050E94
#define DrawFinishMedal         func_80051174
#define GammaCorrectColor       func_800513D0
#define WADLoadPrm              func_80011278
#define RegisterTextureSequence func_80016994
#define FlipTexture             func_80016A8C
#define WADLoadCompressedTextureSequence func_80017534
#define newRand                 func_80023FCC
#define AddParticle             func_800687E4
#define BuildEnvMapDisplayList  func_80015104
#define ResetRaceHeaps          func_80031128
#define InitRace                func_80031180
#define CreateLoaderGfxTask     func_8003BD48
#define UpdateRumble            func_8000D728
#define ReinitCont              func_8004BBE8
#define RenderFrontend          func_800540C4
#define InitFrontend            func_8005B628
#define UpdateFrontend          func_8005D05C
#define ReloadFrontend          func_8005E110
#define ExitFrontend            func_8005E450
#define StopEngineSfx           func_80071F78
#define StopSfx                 func_8007200C
#define DrawShips               func_80017EC0
#define DrawStartDroidAndLights func_80018B00
#define DrawNewTrack            func_8001E66C
#define FlushTrackRenderList    func_8001EF64
#define DrawLandscape           func_800301F4
#define Gfx2DRaceTask           func_8003C454
#define UpdateWeapons           func_8003DFD0
#define DrawEffects             func_80007DE0
#define DrawMedalEffects        func_80008864
#define UpdateEffectAnims       func_80008974
#define DrawTrailEffects        func_800089F8
#define DrawSky                 func_80066F5C
#define DrawSkyHorizon          func_80067600
#define resetMiscFx             func_8006C2E4
#define ClearCycloneCharges     func_8006CB9C
#define UpdateSkyVisibility     func_80024614
#define UpdateShips             func_800269B0
#define UpdateCheatSequence     func_8002ECE8
#define FireSuperWeapon         func_8003EB04
#define MissileLock             func_8003EC2C
#define RearMissileLock         func_8003EDA4
#define UpdateWeaponGrids       func_80047F64
#define UpdateSuperWeapons      func_8004AF8C
#define ColourDroidLights       func_80009568
#define UpdateStartDroid        func_800098A0
#define UpdateLightStart        func_8000ACFC
#define SetRumble               func_8000D5EC
#define ShakeScreen             func_8000D61C
#define AddSpurtExplode         func_8006A1A4
#define AddExplode              func_8006A20C
#define AddSmallSpurtExplode    func_8006A2EC
#define AddSmallSmokeExplode    func_8006A358
#define AddSmokeExplode         func_8006A3C4
#define CheckShipDestroyed      func_8006E190
#define CheckForButtonPress     func_8006E594
#define QuitGame                func_8006EBEC
#define CheckAttractQuit        func_8006ECBC
#define RaceComplete            func_8006ED50
#define CheckGamePause          func_8006FF0C
#define PerformPauseOperations  func_80070150
#define ResetRace               func_80070820
#define RecordGhostInput        func_80070C28
#define AdjustEngineSound       func_80071BF0
#define StartEngineSfx          func_80071EC0
#define UpdateShipTrails        func_8000F92C
#define MusStartEffect2         func_800736B8
#else
#define UpdateAmbientSfx        func_8004A0E0
#define CpuThreadEntry          func_8004A1EC
#define UpdateRace              func_8004A660
#define SetupRaceViewports      func_8004BB10
#define RenderRace              func_8004C148
#define SetupGfxTask            func_8004D470
#define InitGfxBuffer           func_8004D5A4
#define UpdateRotors            func_8004D6A0
#define AddRotor                func_8004D734
#define LoadAnimTextures        func_8004D76C
#define ResetSliders            func_8004D7F0
#define AddSlider               func_8004D820
#define InitAnimTexObject       func_8004D888
#define UpdateAnimTexture       func_8004D9C4
#define UpdateSlider            func_8004DAE4
#define UpdateSliders           func_8004DCAC
#define ResetSpinners           func_8004DE0C
#define UpdateSpinner           func_8004DE34
#define UpdateSpinners          func_8004DFB0
#define LoadSpinners            func_8004E008
#define LoadAnimTextures2       func_8004E188
#define InitAnimTex2Object      func_8004E20C
#define UpdateAnimTexture2      func_8004E348
#define ResetHoverers           func_8004E434
#define UpdateHoverer           func_8004E464
#define AddHoverer              func_8004E938
#define UpdateHoverers          func_8004E9C0
#define LoadFlipTextures        func_8004EA38
#define AddSparkPoint           func_8004EB10
#define InitAnimTex3Object      func_8004EC00
#define ResetSparkPoints        func_8004EC9C
#define UpdateAnimTexture3      func_8004ECBC
#define UpdateSparks            func_8004ED9C
#define LoadAnimTextures4       func_8004EE78
#define InitAnimTex4Object      func_8004EEFC
#define ResetAnimTexture4       func_8004EFA4
#define UpdateAnimTexture4      func_8004EFB4
#define UpdateAnimTextures4     func_8004F098
#define DrawSpinners            func_8004F0C8
#define DrawFinishMedal         func_8004F3A8
#define GammaCorrectColor       func_8004F63C
#define WADLoadPrm              func_8000A458
#define RegisterTextureSequence func_8000FB74
#define FlipTexture             func_8000FC6C
#define WADLoadCompressedTextureSequence func_80010714
#define newRand                 func_8001CFAC
#define AddParticle             func_80066A10
#define BuildEnvMapDisplayList  func_8000E2E4
#define ResetRaceHeaps          func_80029A18
#define InitRace                func_80029A70
#define CreateLoaderGfxTask     func_80034658
#define UpdateRumble            func_8004986C
#define ReinitCont              func_80049D08
#define RenderFrontend          func_80052334
#define InitFrontend            func_800598A0
#define UpdateFrontend          func_8005B2D4
#define ReloadFrontend          func_8005C388
#define ExitFrontend            func_8005C6C8
#define StopEngineSfx           func_8006FFD8
#define StopSfx                 func_8007006C
#define DrawShips               func_800110A0
#define DrawStartDroidAndLights func_80011CE0
#define DrawNewTrack            func_80017760
#define FlushTrackRenderList    func_80018024
#define DrawLandscape           func_80028AE4
#define Gfx2DRaceTask           func_80034D64
#define UpdateWeapons           func_800368E0
#define DrawEffects             func_80043CC0
#define DrawLineEffects         func_800443F4
#define DrawMedalEffects        func_800449C4
#define UpdateEffectAnims       func_80044AD4
#define DrawTrailEffects        func_80044B58
#define DrawSky                 func_800651EC
#define DrawSkyHorizon          func_80065890
#define resetMiscFx             func_8006A3C0
#define ClearCycloneCharges     func_8006AB58
#define UpdateSkyVisibility     func_8001D540
#define UpdateShips             func_8001F710
#define UpdateCheatSequence     func_80027648
#define FireSuperWeapon         func_80037414
#define MissileLock             func_8003753C
#define RearMissileLock         func_800376B4
#define UpdateWeaponGrids       func_800402CC
#define UpdateSuperWeapons      func_80043268
#define ColourDroidLights       func_800456B4
#define UpdateStartDroid        func_800459EC
#define UpdateLightStart        func_80046E48
#define SetRumble               func_80049738
#define ShakeScreen             func_80049760
#define AddSpurtExplode         func_80068370
#define AddExplode              func_800683D8
#define AddSmallSpurtExplode    func_800684B8
#define AddSmallSmokeExplode    func_80068524
#define AddSmokeExplode         func_80068590
#define CheckShipDestroyed      func_8006C130
#define CheckForButtonPress     func_8006C5F4
#define QuitGame                func_8006CC4C
#define CheckAttractQuit        func_8006CD1C
#define RaceComplete            func_8006CDB0
#define CheckGamePause          func_8006DF6C
#define PerformPauseOperations  func_8006E1B0
#define ResetRace               func_8006E880
#define RecordGhostInput        func_8006EC88
#define AdjustEngineSound       func_8006FC50
#define StartEngineSfx          func_8006FF20
#define UpdateShipTrails        func_800721A0
#define MusStartEffect2         func_80073918
#endif

extern u8 _effectTexturesFile[];

extern u64 n_aspMainTextStart[];
extern u64 g_rdpFifo[RDP_FIFO_LEN];
extern u64 g_yieldData[OS_YIELD_DATA_SIZE / sizeof(u64)];
extern u64 g_dramStack[SP_DRAM_STACK_SIZE64];
extern Vp g_viewports[];
extern Vp g_vpLeftHalf;
extern Vp g_vpTopHalf;
extern Vp g_vpTopLeft;
extern Vp g_vpTopRight;
extern Vp g_vpBottomLeft;
extern Vp g_vpBottomRight;
extern Gfx g_rspInitDL[];
extern Gfx g_rdpInitDL[];
extern Gfx g_clearZBufferDL[];
extern Gfx g_clearCfbDL[];
extern s32 g_multiplayerFlag;
extern RenderList g_spinnerRenderList;

extern s32 g_gameState;
extern s32 g_gameType;
extern u8 g_playerFinished[];
extern void *particleSlots;
extern RenderList g_sceneRenderList;
extern RenderList g_scene2RenderList;

extern u64 g_cpuThreadStack[CPU_STACKSIZE / sizeof(u64)];
extern OSThread g_cpuThread;
extern OSMesg g_retraceMsgs[NUM_RETRACE_MSGS];
extern OSMesg g_frameMsgs[NUM_FRAME_MSGS];

extern s32 g_sceneAnimFrame;

extern Object *g_sliderObjects[NUM_SLIDERS];
extern s32 g_sliderBaseX[NUM_SLIDERS];
extern s32 g_sliderBaseY[NUM_SLIDERS];
extern s32 g_sliderBaseZ[NUM_SLIDERS];
extern s32 g_sliderTimer[NUM_SLIDERS];
extern s32 g_sliderYOffset;

extern Object *g_hoverObjects[NUM_HOVERERS];
extern s32 g_hoverBaseX[NUM_HOVERERS];
extern s32 g_hoverBaseY[NUM_HOVERERS];
extern s32 g_hoverBaseZ[NUM_HOVERERS];
extern s32 g_hoverTimer[NUM_HOVERERS];
extern s32 g_hoverDuration[NUM_HOVERERS];

extern Object *g_spinnerObjects[NUM_SPINNERS];
extern s16 g_spinnerAngles[NUM_SPINNERS];
extern s32 g_spinnerSpeed;
extern s32 g_spinnerPitch;
extern s32 g_spinnerRoll;

extern Object *g_rotorObjects[NUM_ROTORS];
extern s32 g_rotorAngle;
extern s32 g_rotorCount;

extern s32 g_animTexTarget;
extern s32 g_animTexStart;
extern s32 g_animTexCount;
extern s32 g_animTexFrame[];
extern s32 g_animTexDelay[];
extern s32 g_animTexTimer[];

extern s32 g_animTex2Target;
extern s32 g_animTex2Start;
extern s32 g_animTex2Count;
extern s32 g_animTex2Frame;
extern s32 g_animTex2Delay;
extern s32 g_animTex2Timer;

extern s32 g_animTex3Target;
extern s32 g_animTex3Start;
extern s32 g_animTex3Count;
extern s32 g_animTex3Frame;
extern s32 g_animTex3Timer;

extern s32 g_animTex4Target;
extern s32 g_animTex4Start;
extern s32 g_animTex4Count;
extern s32 g_animTex4Frame;
extern s32 g_animTex4Timer;

extern s32 g_sparkPointCount;
extern VECTOR *g_sparkPoints;

extern u8 g_colorGammaLut[];

extern OSTime g_delayCycles;
extern s32 g_loadDone;
extern s32 g_frontendLoaded;
extern Object *challengeMedalObject;
extern s32 g_sceneryNearClip[8];
extern s32 g_sceneryFarClip[8];
extern Gfx *g_medalDL;
extern Vtx *g_medalVtx;
extern s32 g_challengeMedalLevel;
extern s32 g_pauseDebounceTimer;
extern u8 g_engineSfxPlaying;
extern s32 g_ambientSfxTimer;
extern s32 g_curFrameIdx;
extern s32 g_stateDelay;
extern s32 g_firstBoot;
extern s32 g_raceReady;
extern s32 g_medalAngle;
extern s32 g_lastMedalLevel;
extern u32 g_shieldSfx;
extern u32 g_pitSfxHandle;
extern u32 g_droidVoice;
extern u32 g_chargeSfxHandle;
extern N64Mtx *g_mtxPool;
extern s32 g_mtxIndex;
extern u16 trackIndex;
extern s32 g_attractMode;
extern u16 g_contButtonPressed[4];
extern Gfx *g_trackDListPtr;
extern s32 g_sceneryPrimCount;
extern Skeleton *g_medalCamSkel;
extern s32 g_multiPlayer;
extern s32 g_trackBufIdx;
extern s32 g_trackVtxCursor;
extern s32 g_frameDelayFlag;
extern s16 skyVisible[4];
extern Object *shipShapes[];
extern u8 g_hudShowPause;
extern RescueData rescueData[4];
extern void *weaponData;
extern s32 ownShip;
extern s32 ownShip2;
extern s32 ownShip3;
extern s32 ownShip4;
extern void *weaponShapes;
extern RaceOverlayState g_raceOverlay;
extern RacePauseMenu g_pauseState;
extern ShipData *ships;
extern Gfx *g_trackDLists[];
#ifndef VERSION_EU
extern Gfx g_lineDLists[][LINE_DL_LEN];
#endif
extern Gfx g_medalDList[];
extern RenderList g_medalRenderList;
extern RenderList g_trailRenderList;
extern RenderList g_effectRenderList;
extern RenderList g_lineRenderList;
extern Gfx g_skyDlPool[];
extern TrackCamera g_cameras[4];
extern Gfx g_effectDLists[][EFFECT_DL_LEN];
extern Gfx g_trailDLists[][EFFECT_DL_LEN];
extern s32 noWeaponGrids;
extern s16 g_raceCompleteChoice;
extern s32 g_enteringHiScore;
extern s16 g_startDroidTexBase;
extern u8 g_resultSfxPending;
extern u8 g_timeoutSfxPending;
extern u8 g_cameraUpdated;
extern s32 g_pausePlayer;
extern ChallengeData challenges[];
extern Gfx *RaceCompleteDList;
extern Gfx *RaceCompleteDListPtr;
extern u8 checkPoints[];
extern void *weaponGrid;
extern Object *g_droidLightObj;
extern Object *startDroidObjects[];
extern u8 g_hudState[];
extern Object *lightGroups[];
extern CameraUpdateFunc CameraUpdate;
extern CameraUpdateFunc CameraUpdate2;
extern CameraUpdateFunc CameraUpdate3;
extern CameraUpdateFunc CameraUpdate4;
extern s32 attractDelayFrames;
extern s32 g_shipDeathCount[15];
extern PadInfo g_padInfo[4];
extern s32 pauseRequests[4];
extern s32 g_raceEndState;
extern u8 resetData[];
extern u8 g_restartRaceFlag;
extern s32 g_idleLimit;
extern Electric electrics[32];

Object *WADLoadPrm(WADFILE *wad, RenderList *renderList);
void RegisterTextureSequence(void *textures);
void FlipTexture(N64Texture *texture);
void *WADLoadCompressedTextureSequence(WADFILE *wad);
s32 newRand(void);
void AddParticle(void *slots, s32 type, VECTOR *pos, s32 life);
void BuildEnvMapDisplayList(Object *obj, void *lights, s32 arg2, s32 level, Vtx **vtxOut, Gfx **dlOut);
void ResetRaceHeaps(void);
void InitRace(void);
void CreateLoaderGfxTask(s32 frameIdx);
void UpdateRumble(s32 player, s32 arg1);
void ReinitCont(void);
void RenderFrontend(s32 frameIdx);
void InitFrontend(void);
void UpdateFrontend(void);
void ReloadFrontend(void);
void ExitFrontend(void);
void StopEngineSfx(void);
void StopSfx(u32 *handle);
Gfx *DrawShips(ShipData *shipList, Object **shapes, s32 camShip, TrackCamera *cam, s32 playerIdx, Gfx *glistp);
Gfx *DrawStartDroidAndLights(Gfx *glistp, TrackCamera *cam, s32 playerIdx);
Gfx *DrawNewTrack(TrackCamera *cam, s32 playerIdx, Gfx *glistp);
Gfx *FlushTrackRenderList(RenderList *list, Gfx *glistp);
Gfx *DrawLandscape(ShipData *ship, TrackCamera *cam, Gfx *glistp);
void Gfx2DRaceTask(Gfx *glistp, GFXInfo *info);
Gfx *UpdateWeapons(ShipData *shipList, void *shapes, void *weapons, TrackCamera *cam, Gfx *glistp);
Gfx *DrawEffects(void *weapons, ShipData *shipList, s32 playerIdx, Dynamic *dp, u16 *perspNorm, Gfx *glistp);
#ifndef VERSION_EU
Gfx *DrawLineEffects(void *weapons, ShipData *shipList, s32 playerIdx, Dynamic *dp, u16 *perspNorm, Gfx *glistp);
#endif
Gfx *DrawMedalEffects(Dynamic *dp, u16 *perspNorm, Gfx *glistp);
void UpdateEffectAnims(void);
void DrawTrailEffects(Dynamic *dp, u16 *perspNorm, s32 playerIdx);
Gfx *DrawSky(TrackCamera *cam, Gfx *glistp);
Gfx *DrawSkyHorizon(TrackCamera *cam, Gfx *glistp);
void resetMiscFx(void);
void ClearCycloneCharges(void);
void UpdateSkyVisibility(s32 section, s32 shipNo);
void UpdateShips(ShipData *shipList, void *wShapes, Object **sShapes, void *weapons, RaceOverlayState *overlay);
void UpdateCheatSequence(s32 arg0);
void FireSuperWeapon(ShipData *ship, void *weapons, s32 owner, u8 count);
void MissileLock(s32 shipNo, ShipData *shipList, u8 weapon);
void RearMissileLock(s32 shipNo, ShipData *shipList);
void UpdateWeaponGrids(ShipData *shipList, void *grid, s32 numGrids);
void UpdateSuperWeapons(void);
void ColourDroidLights(Object *droid);
void UpdateStartDroid(Object **shapes, ShipData *shipList, void *chkPts, s16 texBase,
                      RaceOverlayState *overlay, s32 pauseSel, s32 playerIdx);
void UpdateLightStart(Object **lights, ShipData *shipList, void *chkPts, s16 texBase,
                      RaceOverlayState *overlay, s32 pauseSel);
void SetRumble(s32 time, s32 player);
void ShakeScreen(s32 player);
void AddSpurtExplode(s32 x, s32 y, s32 z, s32 ship);
void AddExplode(s32 x, s32 y, s32 z, s32 ship);
void AddSmallSpurtExplode(s32 x, s32 y, s32 z, s32 ship);
void AddSmallSmokeExplode(s32 x, s32 y, s32 z, s32 ship);
void AddSmokeExplode(s32 x, s32 y, s32 z, s32 unused);
void CheckShipDestroyed(ShipData *shipList, RaceOverlayState *overlay, s32 *rumbleFound, s32 idleLimit);
void CheckForButtonPress(void);
void QuitGame(void);
u8 CheckAttractQuit(RaceOverlayState *overlay);
Gfx *RaceComplete(void *screen, void *reset, RaceOverlayState *overlay, s16 *choice,
                  void *hud, ShipData *shipList, RacePauseMenu *pause, u16 xpad,
                  s32 playerIdx, Gfx *glistp);
void CheckGamePause(ShipData *shipList, RacePauseMenu *pause, RaceOverlayState *overlay,
                    u16 *xpad, s32 *musicPaused);
s32 PerformPauseOperations(RacePauseMenu *pause, void *screen, RaceOverlayState *overlay,
                           void *reset, s32 *pausePlayer);
void ResetRace(void *reset, RacePauseMenu *pause, s32 mode);
void RecordGhostInput(ShipData *shipList);
void AdjustEngineSound(ShipData *playerShip, TrackCamera *cam);
void StartEngineSfx(void);
void UpdateShipTrails(ShipData *shipList, s32 attractDelay);
u32 MusStartEffect2(s32 number, s32 volume, s32 pan, s32 restartflag, s32 priority);

void CpuInit(void);
void UpdateAmbientSfx(u8 reset);
void CpuThreadEntry(void *arg);
s32 UpdateRace(void);
Gfx *SetupRaceViewports(s32 playerIdx, Dynamic *dp, u16 *perspNorm, Gfx *glistp);
void RenderRace(s32 frameIdx);
void SetupGfxTask(GFXInfo *info, s32 taskIdx, Gfx *glistStart, Gfx *glistEnd);
void InitGfxBuffer(s32 bufferIdx);
void UpdateRotors(void);
void AddRotor(Object *obj);
void LoadAnimTextures(void);
void ResetSliders(void);
void AddSlider(Object *obj, s32 i);
void InitAnimTexObject(Object *obj);
void UpdateAnimTexture(s32 i);
void UpdateSlider(s32 i, s32 reset);
void UpdateSliders(void);
void ResetSpinners(void);
void UpdateSpinner(s32 i);
void UpdateSpinners(void);
void LoadSpinners(void);
void LoadAnimTextures2(void);
void InitAnimTex2Object(Object *obj);
void UpdateAnimTexture2(void);
void ResetHoverers(void);
void UpdateHoverer(s32 i, s32 reset);
void AddHoverer(Object *obj, s32 i);
void UpdateHoverers(void);
void LoadFlipTextures(void);
void AddSparkPoint(Object *obj);
void InitAnimTex3Object(Object *obj);
void ResetSparkPoints(void);
void UpdateAnimTexture3(void);
void UpdateSparks(void);
void LoadAnimTextures4(void);
void InitAnimTex4Object(Object *obj);
void ResetAnimTexture4(void);
void UpdateAnimTexture4(void);
void UpdateAnimTextures4(void);
Gfx *DrawSpinners(TrackCamera *cam, Gfx *glistp);
void DrawFinishMedal(TrackCamera *cameras, void *lights);
u8 GammaCorrectColor(u8 i);

#endif /* WIPEOUT64_CPU_H */
