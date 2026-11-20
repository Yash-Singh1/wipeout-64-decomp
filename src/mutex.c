#include "mutex.h"

void InitMutex(void) {
    g_mutexInit = 1;
    osCreateMesgQueue(&g_mutexQueue, g_mutexBuf, 4);
    osSendMesg(&g_mutexQueue, NULL, OS_MESG_NOBLOCK);
}

void LockMutex(void) {
    OSMesg msg;

    if (!g_mutexInit) {
        InitMutex();
    }
    osRecvMesg(&g_mutexQueue, &msg, OS_MESG_BLOCK);
}

void UnlockMutex(void) {
    osSendMesg(&g_mutexQueue, NULL, OS_MESG_NOBLOCK);
}
