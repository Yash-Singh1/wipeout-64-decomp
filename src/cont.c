/*
 * cont.c — N64 SI controller polling thread and Rumble Pak worker thread.
 *
 * Spawns two dedicated OS threads: the controller read thread (id 4, pri 8)
 * waiting on the SI message queue, and the Rumble Pak worker thread (id 3,
 * pri 9) dispatching mutex-guarded osMotorStart / osMotorStop calls.
 */
#include "cont.h"

void ContThreadEntry(void *arg) {
    OSMesg dummy;

    for (;;) {
        osRecvMesg(&g_siMsgQ, &dummy, OS_MESG_BLOCK);
        if (g_contReadPending) {
            UnlockMutex();
            ReadPadData();
            g_contReadPending = FALSE;
        }
    }
}

void RumbleThreadEntry(void *arg) {
    RumbleMsg *msg;
    s32 channel;
    s16 type;

    for (;;) {
        osRecvMesg(&g_rumbleMsgQ, (OSMesg *)&msg, OS_MESG_BLOCK);
        LockMutex();
        type = msg->type;
        channel = msg->channel;
        switch (type) {
        case RUMBLE_MSG_START:
            if (osMotorStart(&g_motorPfs[channel]) == 0) {
                g_rumbleActive[channel] = 1;
            }
            break;
        case RUMBLE_MSG_STOP:
            if (osMotorStop(&g_motorPfs[channel]) == 0) {
                g_rumbleActive[channel] = 0;
            }
            break;
        }
        UnlockMutex();
    }
}

void StartRumble(s32 channel) {
    if (g_rumblePresent[channel]) {
        g_rumbleMsgs[channel].type = RUMBLE_MSG_START;
        g_rumbleMsgs[channel].channel = channel;
        osSendMesg(&g_rumbleMsgQ, (OSMesg)&g_rumbleMsgs[channel], OS_MESG_BLOCK);
    }
}

void StopRumble(s32 channel) {
    if (g_rumblePresent[channel]) {
        g_rumbleMsgs[channel].type = RUMBLE_MSG_STOP;
        g_rumbleMsgs[channel].channel = channel;
        osSendMesg(&g_rumbleMsgQ, (OSMesg)&g_rumbleMsgs[channel], OS_MESG_BLOCK);
    }
}

void InitCont(void) {
    s32 i;
    u8 numCont;

    numCont = 0;
    osCreateMesgQueue(&g_rumbleMsgQ, g_rumbleMsgBuf, NUM_RUMBLE_MSGS);
    osCreateThread(&g_rumbleThread, RUMBLE_THREAD_ID, RumbleThreadEntry, NULL,
                   g_rumbleThreadStack + RUMBLE_STACKSIZE / sizeof(u64), RUMBLE_THREAD_PRI);
    osStartThread(&g_rumbleThread);
    osCreateThread(&g_contThread, CONT_THREAD_ID, ContThreadEntry, NULL,
                   g_contThreadStack + CONT_STACKSIZE / sizeof(u64), CONT_THREAD_PRI);
    osStartThread(&g_contThread);

    osContInit(&g_siMsgQ, &g_contEnabled, g_contStatus);
    for (i = 0; i < MAXCONTROLLERS; i++) {
        g_rumblePresent[i] = 0;
        if ((g_contEnabled >> i) & 1) {
            if (g_contStatus[i].type & CONT_JOYPORT) {
                if (g_contStatus[i].status & CONT_CARD_ON) {
                    if (osMotorInit(&g_siMsgQ, &g_motorPfs[i], i) == 0) {
                        g_rumblePresent[i] = 1;
                        StopRumble(i);
                    }
                }
            }
            numCont++;
        }
    }
    osContSetCh(numCont);
}

void ReinitCont(void) {
    s32 i;
    s32 retry;
    s32 ret;

    osContInit(&g_siMsgQ, &g_contEnabled, g_contStatus);
    for (i = 0; i < MAXCONTROLLERS; i++) {
        g_rumblePresent[i] = 0;
        if ((g_contEnabled >> i) & 1) {
            for (retry = 0; retry < 4; retry++) {
                ret = osMotorInit(&g_siMsgQ, &g_motorPfs[i], i);
                if (ret == 0) {
                    break;
                }
            }
            if (ret == 0) {
                g_rumblePresent[i] = 1;
                StopRumble(i);
            }
        }
    }
    osContStartReadData(&g_siMsgQ);
    osContGetReadData(g_contPads);
}

void StopRumbleAll(s32 channel) {
    s32 i;

    if (g_startupActive != 0) {
        return;
    }
    LockMutex();
    if (channel == -1) {
        for (i = 0; i < MAXCONTROLLERS; i++) {
            if (((g_contEnabled >> i) & 1) && g_rumblePresent[i]) {
                osMotorInit(&g_siMsgQ, &g_motorPfs[i], i);
                osMotorStop(&g_motorPfs[i]);
            }
        }
    } else {
        if (((g_contEnabled >> channel) & 1) && g_rumblePresent[channel]) {
            osMotorInit(&g_siMsgQ, &g_motorPfs[channel], channel);
            osMotorStop(&g_motorPfs[channel]);
        }
    }
    UnlockMutex();
}
