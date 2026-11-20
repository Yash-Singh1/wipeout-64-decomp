#ifndef WIPEOUT64_MUTEX_H
#define WIPEOUT64_MUTEX_H

#include <ultra64.h>
#include "common.h"

extern s32 g_mutexInit;
extern OSMesg g_mutexBuf[4];
extern OSMesgQueue g_mutexQueue;

void InitMutex(void);
void LockMutex(void);
void UnlockMutex(void);

#endif /* WIPEOUT64_MUTEX_H */
