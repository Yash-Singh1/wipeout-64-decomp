#ifndef WIPEOUT64_CONT_H
#define WIPEOUT64_CONT_H

#include "common.h"
#include "mutex.h"
#include "n64init.h"
#include <ultra64.h>

#define RUMBLE_THREAD_ID    3
#define CONT_THREAD_ID      4
#define RUMBLE_THREAD_PRI   9
#define CONT_THREAD_PRI     8
#define RUMBLE_STACKSIZE    0x2000
#define CONT_STACKSIZE      0x2000
#define NUM_RUMBLE_MSGS     4

#define RUMBLE_MSG_STOP     0
#define RUMBLE_MSG_START    1

typedef struct RumbleMsg {
    /* 0x00 */ s16 type;
    /* 0x04 */ s32 channel;
    /* 0x08 */ s32 unk08;
} RumbleMsg;

/* Temporary mappings for unnamed functions/globals until added to config/{us,eu}/symbol_addrs.txt. */
#ifdef VERSION_EU
#define ContThreadEntry     func_8004B830
#define RumbleThreadEntry   func_8004B894
#define StartRumble         func_8004B988
#define StopRumble          func_8004B9EC
#define InitCont            func_8004BA4C
#define ReinitCont          func_8004BBE8
#define ReadPadData         func_80016094

#define g_startupActive     D_80091979
#define g_contStatus        D_800A9258
#define g_contPads          D_800B76D8
#define g_rumbleMsgBuf      D_800BAD70
#define g_rumbleThread      D_800BAD80
#define g_contThread        D_800BAF30
#define g_rumbleThreadStack D_800BB0E0
#define g_contThreadStack   D_800BD0E0
#define g_motorPfs          D_800BF0E0
#define g_rumblePresent     D_800BF280
#define g_rumbleMsgs        D_800BF290
#define g_rumbleMsgQ        D_800BF2C0
#define g_rumbleActive      D_800BF2D8
#else
#define ContThreadEntry     func_80049950
#define RumbleThreadEntry   func_800499B4
#define StartRumble         func_80049AA8
#define StopRumble          func_80049B0C
#define InitCont            func_80049B6C
#define ReinitCont          func_80049D08
#define ReadPadData         func_8000F274

#define g_startupActive     D_800923C9
#define g_contStatus        D_8009BC78
#define g_contPads          D_800AA108
#define g_rumbleMsgBuf      D_800BAA30
#define g_rumbleThread      D_800BAA40
#define g_contThread        D_800BABF0
#define g_rumbleThreadStack D_800BADA0
#define g_contThreadStack   D_800BCDA0
#define g_motorPfs          D_800BEDA0
#define g_rumblePresent     D_800BEF40
#define g_rumbleMsgs        D_800BEF50
#define g_rumbleMsgQ        D_800BEF80
#define g_rumbleActive      D_800BEF98
#endif

extern u8 g_startupActive;
extern OSContStatus g_contStatus[MAXCONTROLLERS];
extern OSContPad g_contPads[MAXCONTROLLERS];
extern OSMesg g_rumbleMsgBuf[NUM_RUMBLE_MSGS];
extern OSThread g_rumbleThread;
extern OSThread g_contThread;
extern u64 g_rumbleThreadStack[RUMBLE_STACKSIZE / sizeof(u64)];
extern u64 g_contThreadStack[CONT_STACKSIZE / sizeof(u64)];
extern OSPfs g_motorPfs[MAXCONTROLLERS];
extern s32 g_rumblePresent[MAXCONTROLLERS];
extern RumbleMsg g_rumbleMsgs[MAXCONTROLLERS];
extern OSMesgQueue g_rumbleMsgQ;
extern s32 g_rumbleActive[MAXCONTROLLERS];

void ReadPadData(void);

void ContThreadEntry(void *arg);
void RumbleThreadEntry(void *arg);
void StartRumble(s32 channel);
void StopRumble(s32 channel);
void InitCont(void);
void ReinitCont(void);
void StopRumbleAll(s32 channel);

#endif /* WIPEOUT64_CONT_H */
