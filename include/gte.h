#ifndef WIPEOUT64_GTE_H
#define WIPEOUT64_GTE_H

#include "common.h"
#include "skeleton.h"

typedef struct {
    s32 vx;
    s32 vy;
    s32 vz;
    s32 pad;
} VECTOR;

typedef struct {
    s16 vx;
    s16 vy;
    s16 vz;
    s16 pad;
} SVECTOR;

#ifdef VERSION_EU
#define g_cosTable D_8008ACE0
#define g_gteMatrix D_800B8400
#define g_audioMuted D_800B7660
#define MusHandleAsk func_8007392C
#define MusStartEffect func_8007360C
#define ratan2 func_8001BBE0
#define OuterProduct12 func_8001BC54
#define PlayNote func_8001BCE8
#define ApplyMatrix func_8001BDA4
#define ApplyMatrixLV func_8001BE9C
#define RotTrans func_8001BF94
#define InitGeom func_8001BFC4
#else
#define g_cosTable D_8008B6B0
#define g_gteMatrix D_800AAE30
#define g_audioMuted D_800AA088
#define MusHandleAsk func_80073B8C
#define MusStartEffect func_8007386C
#define ratan2 func_80014C30
#define OuterProduct12 func_80014CA4
#define PlayNote func_80014D38
#define ApplyMatrix func_80014DF4
#define ApplyMatrixLV func_80014EEC
#define RotTrans func_80014FE4
#define InitGeom func_80015014
#endif

extern s16 g_sinTable[];
extern s16 g_cosTable[];
extern MATRIX g_gteMatrix;
extern s32 g_audioMuted;

s32 Atan2Fix16(s32 x, s32 y);
s32 MusHandleAsk(s32 handle);
void MusStartEffect(s32 sfxId);

s32 ratan2(s32 y, s32 x);
s32 rsin(s32 a);
s32 rcos(s32 a);
void OuterProduct12(VECTOR *v0, VECTOR *v1, VECTOR *v2);
void PlayNote(s32 vagID, s32 handle, s32 a2, s32 a3, s32 a4);
void SetTransMatrix(MATRIX *m);
void SetRotMatrix(MATRIX *m);
VECTOR *ApplyMatrix(MATRIX *m, SVECTOR *v0, VECTOR *v1);
VECTOR *ApplyMatrixLV(MATRIX *m, VECTOR *v0, VECTOR *v1);
void RotTrans(SVECTOR *v0, VECTOR *v1, s32 *flag);
void InitGeom(void);

#endif /* WIPEOUT64_GTE_H */
