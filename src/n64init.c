/*
 * n64init.c — N64 boot entry point (main), VI/PI/scheduler init, and DMA.
 *
 * Sets up the idle and main OS threads, configures NTSC (USA) vs PAL (Europe)
 * video timing, initializes the dynamic heap region, and launches the game.
 */
#include "n64init.h"

static const char rcsid[] = "$Revision: 1.17 $";

/* files.h prototypes InitFileSystem(void), but InitN64Hardware passes g_heapA in $a0. */
void InitFileSystem();

void main(void) {
    char unused[64]; /* not referenced, but the retail frame still reserves it */

#ifdef VERSION_EU
    osTvType = OS_TV_PAL;
#endif
    osInitialize();
    osCreateThread(&g_idleThread, IDLE_THREAD_ID, IdleThreadEntry, NULL,
                   g_idleThreadStack + IDLE_STACKSIZE / sizeof(u64), IDLE_THREAD_PRI);
    osStartThread(&g_idleThread);
}

void InitTvTiming(void) {
    f32 rate;

    rate = 45637500.0f;
    if (osTvType == OS_TV_PAL) {
        rate = 46552500.0f;
    }
    g_tvRate = rate;
}

void IdleThreadEntry(void *arg) {
    s32 i;
    s32 busy;

    osCreatePiManager(OS_PRIORITY_PIMGR, &g_piMsgQ, g_piMsgs, NUM_PI_MSGS);
    osCreateThread(&g_mainThread, MAIN_THREAD_ID, MainThreadEntry, arg,
                   g_mainThreadStack + MAIN_STACKSIZE / sizeof(u64), MAIN_THREAD_PRI);
    busy = 0;
    if (!g_mainThreadDisabled) {
        osStartThread(&g_mainThread);
    }
    osSetThreadPri(NULL, OS_PRIORITY_IDLE);

    for (;;) {
        if (!g_idleRunning) {
            g_idleRunning = TRUE;
        }
        for (i = 0; i < NUM_TASK_FLAGS; i++) {
            if (g_taskFlags[i] == i + 16) {
                busy |= 1 << i;
            } else {
                busy &= ~(1 << i);
            }
        }
        if (busy) {
            if (g_vblankCount != g_stallVblank) {
                g_stallVblank = g_vblankCount;
                g_stallFrames++;
            }
        } else {
            g_stallFrames = 0;
        }
        if (g_stallFrames > 60) {
            if (g_stallFrames == 61) {
                DCheckHeap(g_mainHeap);
                DCheckHeap(g_tempHeap);
            }
        }
    }
}

void MainThreadEntry(void *arg) {
    OSScMsg *msg;
    OSMesg dummy;
    s32 field;
    s32 frameDone;
    f32 frameTime;

    field = 0;
    frameDone = 0;
    msg = NULL;
    InitN64Hardware();
    InitTvTiming();

    for (;;) {
        osRecvMesg(&g_schedMsgQ, (OSMesg *)&msg, OS_MESG_BLOCK);
        switch (msg->type) {
        case OS_SC_RETRACE_MSG:
            osRecvMesg(&g_retraceMsgQ, &dummy, OS_MESG_NOBLOCK);
            g_retraceCount++;
            g_vblankCount++;
            if (frameDone) {
                frameDone = 0;
                g_retraceCount = 0;
                if (g_contEnabled && !g_contReadPending) {
                    g_contReadPending = TRUE;
                    LockMutex();
                    osContStartReadData(&g_siMsgQ);
                }
            }
            break;
        case OS_SC_DONE_MSG:
            g_frameTime = (f32)(osGetTime() - g_lastFrameTime) / g_tvRate;
            frameTime = g_frameTime;
#ifdef VERSION_EU
            if (frontendOptions_g.numPlayers < 3)
#endif
            while (frameTime <= MIN_FRAME_TIME) {
                frameTime = (f32)(osGetTime() - g_lastFrameTime) / g_tvRate;
            }
            g_frameTime = frameTime;
            field = 1 - field;
            osSendMesg(&g_frameMsgQ, (OSMesg)((field << 2) | 1), OS_MESG_BLOCK);
            frameDone = 1;
            break;
        case OS_SC_PRE_NMI_MSG:
            g_resetCount += 2;
            StopRumbleAll(-1);
            N64InitHelper_80066100();
            break;
        case SC_MSG_TASK6_DONE:
            g_taskFlags[6] = 0;
            break;
        case SC_MSG_TASK4_DONE:
            g_taskFlags[4] = 0;
            break;
        case SC_MSG_TASK3_DONE:
            g_taskFlags[3] = 0;
            break;
        case SC_MSG_TASK1_DONE:
            g_taskFlags[1] = 0;
            break;
        case SC_MSG_TASK0_DONE:
            g_taskFlags[0] = 0;
            break;
        case SC_MSG_TASK2_DONE:
            g_taskFlags[2] = 0;
            break;
        case SC_MSG_TASK5_DONE:
            g_taskFlags[5] = 0;
            break;
        }
    }
}

void N64InitHelper_80066100(void) {
    osViBlack(TRUE);
    StopAudio();
    for (;;) {
    }
}

void DmaRead(u32 romAddr, void *dst, s32 length) {
    OSIoMesg ioMsg;
    OSMesg msg;
    s32 retVal;

    osInvalDCache(dst, length);
    retVal = osPiStartDma(&ioMsg, OS_MESG_PRI_NORMAL, OS_READ, romAddr, dst, length, &g_dmaMsgQ);
    if (retVal == -1) {
#ifdef VERSION_EU
        __assert("retVal != -1", "n64init.c", 549);
#else
        __assert("retVal != -1", "n64init.c", 544);
#endif
    }
    osRecvMesg(&g_dmaMsgQ, &msg, OS_MESG_BLOCK);
}

void InitN64Hardware(void) {
    s32 size;

    osCreateMesgQueue(&g_dmaMsgQ, g_dmaMsgs, NUM_DMA_MSGS);
    osCreateMesgQueue(&g_schedMsgQ, g_schedMsgs, NUM_SCHED_MSGS);

    if (osTvType == OS_TV_NTSC) {
        osCreateScheduler(&g_scheduler, g_schedStack + OS_SC_STACKSIZE / sizeof(u64), SCHEDULER_PRI,
                          OS_VI_NTSC_LAN1, 1);
        g_viMode = &osViModeTable[OS_VI_NTSC_LAN1];
    } else if (osTvType == OS_TV_MPAL) {
        osCreateScheduler(&g_scheduler, g_schedStack + OS_SC_STACKSIZE / sizeof(u64), SCHEDULER_PRI,
                          OS_VI_MPAL_LAN1, 1);
        g_viMode = &osViModeTable[OS_VI_MPAL_LAN1];
    } else {
        osCreateScheduler(&g_scheduler, g_schedStack + OS_SC_STACKSIZE / sizeof(u64), SCHEDULER_PRI,
                          OS_VI_PAL_LAN1, 1);
        g_viMode = &osViModeTable[OS_VI_PAL_LAN1];
    }
    osViSetSpecialFeatures(OS_VI_DITHER_FILTER_ON);
    osViSetSpecialFeatures(OS_VI_GAMMA_OFF);
    osScAddClient(&g_scheduler, &g_schedClient, &g_schedMsgQ);
    g_schedCmdQ = osScGetCmdQ(&g_scheduler);

    bzero(g_heapRegionStart, g_heapRegionEnd - g_heapRegionStart);
    size = g_heapRegionEnd - g_heapRegionStart - TEMP_HEAP_SIZE;
    g_heapA = (DHeapBlock *)g_heapRegionStart;
    g_heapB = (DHeapBlock *)(g_heapRegionStart + size);
    DInit(g_heapA, size);
    DInit(g_heapB, TEMP_HEAP_SIZE);
    g_mainHeap = g_heapA;
    g_tempHeap = g_heapB;
    g_heap = g_heapA = g_heapB = g_mainHeap;

    InitGfx();
    InitAudio();
    InitPad();
    guScale(&g_flipYMtx, 1.0f, -1.0f, 1.0f);
    ((void (*)(DHeapBlock *))InitFileSystem)(g_heapA);
    g_startupData = OpenWadFileNvc((u32)_startupDataFile);
    g_unk8008A970 = 550;
    InitHud();
    InitFontSize(8);
    InitFontSize(12);
    InitFontSize(16);
    g_startupImage = OpenWadFileNvc((u32)_startupImageFile);
    g_startupImage->current = 19;
    InitFont();
    CloseWadFileNvc(g_startupImage);
#ifdef VERSION_EU
    g_unkAlloc60 = DAlloc(g_heap, 60, "n64init.c", 710);
    g_unkAlloc9600 = DAlloc(g_heap, 9600, "n64init.c", 711);
#else
    g_unkAlloc60 = DAlloc(g_heap, 60, "n64init.c", 705);
    g_unkAlloc9600 = DAlloc(g_heap, 9600, "n64init.c", 706);
#endif
    g_heap = g_heapA = g_heapB = g_tempHeap;
    CpuInit();
}
