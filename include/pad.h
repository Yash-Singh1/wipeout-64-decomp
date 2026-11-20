#ifndef WIPEOUT64_PAD_H
#define WIPEOUT64_PAD_H

#include "common.h"
#include <ultra64.h>

/*
 * Temporary US/EU symbol mappings until config/{us,eu}/symbol_addrs.txt
 * registers canonical names for pad.c functions and external symbols.
 */
#ifdef VERSION_EU
#define ReadPads            func_80016094
#define GetButtonName       func_80016414
#define InitControllers     func_8004BA4C
#define g_padConfigs        D_8008A024
#define g_padConfigIndex    D_8008A0C4
#define g_firstContPad      D_800A9304
#define g_padInfo           D_800B7580
#define g_contButtonHeld    D_800B76C0
#define g_contButtonPressed D_800B76C8
#define g_siMsgs            D_800B76D0
#define g_contPads          D_800B76D8
#else
#define ReadPads            func_8000F274
#define GetButtonName       func_8000F5F4
#define InitControllers     func_80049B6C
#define g_padConfigs        D_8008A9F4
#define g_padConfigIndex    D_8008AA94
#define g_firstContPad      D_8009BD24
#define g_padInfo           D_800A9FA0
#define g_contButtonHeld    D_800AA0F0
#define g_contButtonPressed D_800AA0F8
#define g_siMsgs            D_800AA100
#define g_contPads          D_800AA108
#endif

/* Internal game pad bitmask (PlayStation-derived button bits). */
#define PAD_START       0x0008
#define PAD_UP          0x0010
#define PAD_RIGHT       0x0020
#define PAD_DOWN        0x0040
#define PAD_LEFT        0x0080
#define PAD_L1          0x0400 /* Right airbrake in ReadPads; "BRAKE LEFT" action in GetButtonName */
#define PAD_R1          0x0800 /* Left airbrake in ReadPads; "BRAKE RIGHT" action in GetButtonName */
#define PAD_VIEW        0x1000
#define PAD_FIRE        0x2000
#define PAD_THRUST      0x4000
#define PAD_DISCARD     0x8000

#define STICK_DIGITAL_THRESHOLD 30

typedef struct PadConfig {
    /* 0x00 */ u32 thrust;
    /* 0x04 */ u32 fire;
    /* 0x08 */ u32 discard;
    /* 0x0C */ u32 brakeLeft;
    /* 0x10 */ u32 brakeRight;
    /* 0x14 */ u32 lookBack;
    /* 0x18 */ u32 view;
    /* 0x1C */ u32 start;
} PadConfig;

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
} PadInfo;

extern OSMesgQueue g_siMsgQ;
extern OSMesg g_siMsgs[1];
extern u8 g_contEnabled;
extern OSContPad g_contPads[MAXCONTROLLERS];
extern OSContPad g_firstContPad;
extern PadInfo g_padInfo[MAXCONTROLLERS];
extern u16 g_contButtonHeld[MAXCONTROLLERS];
extern u16 g_contButtonPressed[MAXCONTROLLERS];
extern PadConfig g_padConfigs[];
extern s32 g_padConfigIndex[MAXCONTROLLERS];

void InitControllers(void);

void InitPad(void);
void ReadPads(void);
char *GetButtonName(u16 action, u8 configIndex);

#endif /* WIPEOUT64_PAD_H */
