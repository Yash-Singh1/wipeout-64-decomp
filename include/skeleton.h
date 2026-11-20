#ifndef WIPEOUT64_SKELETON_H
#define WIPEOUT64_SKELETON_H

#include "common.h"
#include "dheap.h"

typedef struct {
    s16 m[3][3];
    s16 pad;
    s32 t[3];
} MATRIX;

typedef struct Skeleton {
    MATRIX relative;
    MATRIX absolute;
    s16 update;
    s16 pad;
    struct Skeleton *super;
    struct Skeleton *sub;
    struct Skeleton *next;
} Skeleton;

typedef union {
    s32 m[4][4];
    s64 force_structure_alignment;
} N64Mtx;

extern s16 g_sinTable[];

MATRIX *MulMatrix0(MATRIX *m0, MATRIX *m1, MATRIX *m2);
void SetTransMatrix(MATRIX *m);
void SetRotMatrix(MATRIX *m);

Skeleton *NewSkeleton(void);
void SetSkeletonPosition(Skeleton *skeleton, s32 x, s32 y, s32 z);
void SetSkeletonDirection(Skeleton *skeleton, s16 xa, s16 ya, s16 za);
void SetSkeletonDirectionHPR(Skeleton *skeleton, s16 hdg, s16 pitch, s16 roll);
void SetSkeletonDirectionRPH(Skeleton *skeleton, s16 roll, s16 pitch, s16 hdg);
MATRIX *GetSkeletonMatrix(Skeleton *skeleton);
void MatrixToN64Mtx(MATRIX *src, N64Mtx *dst);
void SetSkeletonTransform(Skeleton *skeleton);

#endif /* WIPEOUT64_SKELETON_H */
