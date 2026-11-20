#ifndef WIPEOUT64_N64INIT_H
#define WIPEOUT64_N64INIT_H

#include "common.h"
#include "dheap.h"
#include "mutex.h"
#include "radscn.h"
#include "wadfile.h"
#include <ultra64.h>
#include <PR/sched.h>

#define IDLE_THREAD_ID      5
#define MAIN_THREAD_ID      6
#define IDLE_THREAD_PRI     10
#define MAIN_THREAD_PRI     10
#define SCHEDULER_PRI       13

#define IDLE_STACKSIZE      0x800
#define MAIN_STACKSIZE      0x1000

#define NUM_PI_MSGS         8
#define NUM_DMA_MSGS        4
#define NUM_SCHED_MSGS      24
#define NUM_TASK_FLAGS      8

#define MIN_FRAME_TIME      0.034

#define TEMP_HEAP_SIZE      0x184C00

/* Scheduler messages the render tasks post on completion (OSScMsg.type). */
#define SC_MSG_TASK0_DONE   11
#define SC_MSG_TASK1_DONE   10
#define SC_MSG_TASK2_DONE   9
#define SC_MSG_TASK3_DONE   8
#define SC_MSG_TASK4_DONE   12
#define SC_MSG_TASK5_DONE   7
#define SC_MSG_TASK6_DONE   13

extern OSViMode osViModeTable[];
extern u64 g_idleThreadStack[IDLE_STACKSIZE / sizeof(u64)];
extern u64 g_mainThreadStack[MAIN_STACKSIZE / sizeof(u64)];
extern OSMesg g_piMsgs[NUM_PI_MSGS];
extern OSMesgQueue g_piMsgQ;
extern OSThread g_idleThread;
extern OSThread g_mainThread;
extern OSViMode *g_viMode;
extern f32 g_tvRate;
extern OSMesgQueue g_dmaMsgQ;
extern f32 g_frameTime;
extern OSMesgQueue g_schedMsgQ;
extern OSTime g_lastFrameTime;
extern WADFILE *g_startupData;
extern OSSched g_scheduler;
extern OSMesg g_dmaMsgs[NUM_DMA_MSGS];
extern u64 g_schedStack[OS_SC_STACKSIZE / sizeof(u64)];
extern WADFILE *g_startupImage;
extern OSMesg g_schedMsgs[NUM_SCHED_MSGS];
extern OSScClient g_schedClient;
extern OSMesgQueue *g_schedCmdQ;
extern OSMesgQueue g_frameMsgQ;
extern OSMesgQueue g_retraceMsgQ;
extern s32 g_mainThreadDisabled;
extern s32 g_vblankCount;
extern s32 g_idleRunning;
extern s32 g_stallFrames;
extern s32 g_stallVblank;
extern s32 g_contReadPending;
extern s32 g_resetCount;
extern volatile s32 g_retraceCount;
extern s16 g_taskFlags[NUM_TASK_FLAGS];
extern u8 g_contEnabled;
extern s16 g_unk8008A970;
extern DHeapBlock *g_tempHeap;
extern OSMesgQueue g_siMsgQ;
extern Mtx g_flipYMtx;
extern void *g_unkAlloc9600;
extern void *g_unkAlloc60;
extern DHeapBlock *g_heapB;
extern DHeapBlock *g_heapA;
extern u8 g_heapRegionStart[];
extern u8 g_heapRegionEnd[];

/* ROM file offsets (relative to _romFilesBase) loaded at startup. */
extern u8 _startupDataFile[];
extern u8 _startupImageFile[];

void __assert(const char *expr, const char *file, s32 line);

void InitAudio(void);
void StopAudio(void);
void InitGfx(void);
void InitPad(void);
void InitHud(void);
void InitFontSize(s32 arg);
void InitFont(void);
void StopRumbleAll(s32 arg);
void CpuInit(void);

void main(void);
void InitTvTiming(void);
void IdleThreadEntry(void *arg);
void MainThreadEntry(void *arg);
void N64InitHelper_80066100(void);
void DmaRead(u32 romAddr, void *dst, s32 length);
void InitN64Hardware(void);

#endif /* WIPEOUT64_N64INIT_H */
