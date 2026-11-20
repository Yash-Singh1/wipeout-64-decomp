#include "skeleton.h"

#define rsin(a) (g_sinTable[(a) & 0xFFF])
#define rcos(a) (g_sinTable[((a) & 0xFFF) + 1024])

Skeleton *NewSkeleton(void) {
    Skeleton *skeleton;

    skeleton = (Skeleton *)DAlloc(g_heap, sizeof(Skeleton), "Skeleton.c", 42);
    if (!skeleton) {
        return NULL;
    }

    skeleton->relative.m[0][0] = 4096;
    skeleton->relative.m[1][1] = 4096;
    skeleton->relative.m[2][2] = 4096;

    skeleton->super = NULL;
    skeleton->sub = NULL;
    skeleton->next = NULL;

    skeleton->relative.t[0] = 0;
    skeleton->relative.t[1] = 0;
    skeleton->relative.t[2] = 0;

    skeleton->relative.m[0][1] = 0;
    skeleton->relative.m[0][2] = 0;
    skeleton->relative.m[1][0] = 0;
    skeleton->relative.m[1][2] = 0;
    skeleton->relative.m[2][0] = 0;
    skeleton->relative.m[2][1] = 0;

    skeleton->update = 1;

    return skeleton;
}

void SetSkeletonPosition(Skeleton *skeleton, s32 x, s32 y, s32 z) {
    skeleton->relative.t[0] = x;
    skeleton->relative.t[1] = y;
    skeleton->relative.t[2] = z;
    skeleton->update = 1;
}

void SetSkeletonDirection(Skeleton *skeleton, s16 xa, s16 ya, s16 za) {
    s32 sx, sy, sz;
    s32 cx, cy, cz;

    sx = rsin(xa);
    sy = rsin(ya);
    sz = rsin(za);
    cx = rcos(xa);
    cy = rcos(ya);
    cz = rcos(za);

    skeleton->relative.m[0][0] = (cy * cz) >> 16;
    skeleton->relative.m[0][1] = (-cy * sz) >> 16;
    skeleton->relative.m[0][2] = -sy >> 2;

    skeleton->relative.m[1][0] = ((cx * sz) >> 16) - ((((sx * sy) >> 14) * cz) >> 16);
    skeleton->relative.m[1][1] = ((cx * cz) >> 16) + ((((sx * sy) >> 14) * sz) >> 16);
    skeleton->relative.m[1][2] = (-sx * cy) >> 16;

    skeleton->relative.m[2][0] = ((sx * sz) >> 16) + ((((cx * sy) >> 14) * cz) >> 16);
    skeleton->relative.m[2][1] = ((sx * cz) >> 16) - ((((cx * sy) >> 14) * sz) >> 16);
    skeleton->relative.m[2][2] = (cx * cy) >> 16;

    skeleton->update = 1;
}

void SetSkeletonDirectionHPR(Skeleton *skeleton, s16 hdg, s16 pitch, s16 roll) {
    s32 sx, sy, sz;
    s32 cx, cy, cz;

    sy = rsin(hdg);
    sx = rsin(pitch);
    sz = rsin(roll);
    cy = rcos(hdg);
    cx = rcos(pitch);
    cz = rcos(roll);

    skeleton->relative.m[0][0] = ((cy * cz) >> 16) + ((((sy * sz) >> 14) * sx) >> 16);
    skeleton->relative.m[0][1] = ((cy * sz) >> 16) - ((((sy * sx) >> 14) * cz) >> 16);
    skeleton->relative.m[0][2] = (-sy * cx) >> 16;

    skeleton->relative.m[1][0] = (-(sz * cx)) >> 16;
    skeleton->relative.m[1][1] = (cx * cz) >> 16;
    skeleton->relative.m[1][2] = -sx >> 2;

    skeleton->relative.m[2][0] = ((sy * cz) >> 16) - ((((cy * sx) >> 14) * sz) >> 16);
    skeleton->relative.m[2][1] = ((sy * sz) >> 16) + ((((cy * sx) >> 14) * cz) >> 16);
    skeleton->relative.m[2][2] = (cx * cy) >> 16;

    skeleton->update = 1;
}

void SetSkeletonDirectionRPH(Skeleton *skeleton, s16 roll, s16 pitch, s16 hdg) {
    s32 sx, sy, sz;
    s32 cx, cy, cz;

    sy = rsin(hdg);
    sx = rsin(pitch);
    sz = rsin(roll);
    cy = rcos(hdg);
    cx = rcos(pitch);
    cz = rcos(roll);

    skeleton->relative.m[0][0] = ((cy * cz) >> 16) - ((((sz * sx) >> 14) * sy) >> 16);
    skeleton->relative.m[0][1] = (sz * cx) >> 16;
    skeleton->relative.m[0][2] = (-(sy * cz) >> 16) - ((((sz * sx) >> 14) * cy) >> 16);

    skeleton->relative.m[1][0] = (-(sz * cy) >> 16) - ((((sx * cz) >> 14) * sy) >> 16);
    skeleton->relative.m[1][1] = (cx * cz) >> 16;
    skeleton->relative.m[1][2] = ((sz * sy) >> 16) - ((((sx * cz) >> 14) * cy) >> 16);

    skeleton->relative.m[2][0] = (cx * sy) >> 16;
    skeleton->relative.m[2][1] = sx >> 2;
    skeleton->relative.m[2][2] = (cx * cy) >> 16;

    skeleton->update = 1;
}

MATRIX *GetSkeletonMatrix(Skeleton *skeleton) {
    MATRIX *relMatrix;
    MATRIX *superMatrix;
    MATRIX *absMatrix;

    if (!skeleton->super) {
        return &skeleton->relative;
    }

    if (skeleton->update) {
        relMatrix = &skeleton->relative;
        superMatrix = GetSkeletonMatrix(skeleton->super);
        absMatrix = &skeleton->absolute;

        absMatrix->t[0] = superMatrix->t[0] +
            ((relMatrix->t[0] * superMatrix->m[0][0] +
              relMatrix->t[1] * superMatrix->m[0][1] +
              relMatrix->t[2] * superMatrix->m[0][2]) >> 12);
        absMatrix->t[1] = superMatrix->t[1] +
            ((relMatrix->t[0] * superMatrix->m[1][0] +
              relMatrix->t[1] * superMatrix->m[1][1] +
              relMatrix->t[2] * superMatrix->m[1][2]) >> 12);
        absMatrix->t[2] = superMatrix->t[2] +
            ((relMatrix->t[0] * superMatrix->m[2][0] +
              relMatrix->t[1] * superMatrix->m[2][1] +
              relMatrix->t[2] * superMatrix->m[2][2]) >> 12);

        MulMatrix0(superMatrix, relMatrix, absMatrix);
        skeleton->update = 0;
    }

    return &skeleton->absolute;
}

void MatrixToN64Mtx(MATRIX *src, N64Mtx *dst) {
    s32 e1, e2;

    e1 = src->m[0][0];
    e2 = src->m[1][0];
    e1 <<= 4;
    e2 <<= 4;
    dst->m[0][0] = (e1 & 0xFFFF0000) | (((u32)(e2 & 0xFFFF0000)) >> 16);
    dst->m[2][0] = ((e1 & 0x0000FFFF) << 16) | (e2 & 0x0000FFFF);

    e1 = src->m[0][1];
    e2 = src->m[1][1];
    e1 <<= 4;
    e2 <<= 4;
    dst->m[0][2] = (e1 & 0xFFFF0000) | (((u32)(e2 & 0xFFFF0000)) >> 16);
    dst->m[2][2] = ((e1 & 0x0000FFFF) << 16) | (e2 & 0x0000FFFF);

    e1 = src->m[0][2];
    e2 = src->m[1][2];
    e1 <<= 4;
    e2 <<= 4;
    dst->m[1][0] = (e1 & 0xFFFF0000) | (((u32)(e2 & 0xFFFF0000)) >> 16);
    dst->m[3][0] = ((e1 & 0x0000FFFF) << 16) | (e2 & 0x0000FFFF);

    e1 = -src->m[2][0];
    e1 <<= 4;
    dst->m[0][1] = e1 & 0xFFFF0000;
    dst->m[2][1] = (e1 & 0x0000FFFF) << 16;

    e1 = -src->m[2][1];
    e1 <<= 4;
    dst->m[0][3] = e1 & 0xFFFF0000;
    dst->m[2][3] = (e1 & 0x0000FFFF) << 16;

    e1 = -src->m[2][2];
    e1 <<= 4;
    dst->m[1][1] = e1 & 0xFFFF0000;
    dst->m[3][1] = (e1 & 0x0000FFFF) << 16;

    dst->m[1][2] = ((src->t[0] & 0xFFFF) << 16) | (src->t[1] & 0xFFFF);
    dst->m[1][3] = ((-src->t[2]) << 16) | 1;
    dst->m[3][2] = 0;
    dst->m[3][3] = 0;
}

void SetSkeletonTransform(Skeleton *skeleton) {
    MATRIX *matrix;

    matrix = GetSkeletonMatrix(skeleton);
    SetTransMatrix(matrix);
    SetRotMatrix(matrix);
}
