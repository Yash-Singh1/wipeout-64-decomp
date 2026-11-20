/*
 * gte.c — PS1 GTE fixed-point math compatibility layer and PlayNote SFX stub.
 *
 * Provides fixed-point trig (ratan2, rsin, rcos), 3D cross product, and matrix
 * transforms (SetRotMatrix, SetTransMatrix, ApplyMatrix, RotTrans) ported from
 * Wipeout XL's PS1 GTE calls, plus PlayNote (preserving the retail quirk where
 * its local musHandle is read uninitialized from $a1).
 */
#ident "$Revision: 7.27 $"
#ident "$Revision: 1.5 $"
#ident "$Revision: 1.34 $"

#include "gte.h"

s32 ratan2(s32 y, s32 x) {
    return ((s16)Atan2Fix16(x, y)) >> 4;
}

s32 rsin(s32 a) {
    return g_sinTable[a & 0xFFF] >> 2;
}

s32 rcos(s32 a) {
    return g_cosTable[a & 0xFFF] >> 2;
}

void OuterProduct12(VECTOR *v0, VECTOR *v1, VECTOR *v2) {
    v2->vx = (v0->vy * v1->vz - v0->vz * v1->vy) >> 12;
    v2->vy = (v0->vz * v1->vx - v0->vx * v1->vz) >> 12;
    v2->vz = (v0->vx * v1->vy - v0->vy * v1->vx) >> 12;
}

void PlayNote(s32 vagID, s32 priority, s32 lrPan, s32 fbPan, s32 pitch) {
    /* Original Wipeout 64 bug: handle is read uninitialized (allocated to $a1 = priority). */
    u32 handle;

    if (g_audioMuted != 1) {
        if (vagID >= 1 && vagID < 72) {
            if (!MusHandleAsk(handle)) {
                MusStartEffect(vagID);
            }
        }
    }
}

void SetTransMatrix(MATRIX *m) {
    __builtin_memcpy(g_gteMatrix.t, m->t, sizeof(m->t));
}

void SetRotMatrix(MATRIX *m) {
    __builtin_memcpy(g_gteMatrix.m, m->m, sizeof(m->m));
}

VECTOR *ApplyMatrix(MATRIX *m, SVECTOR *v0, VECTOR *v1) {
    s32 x = v0->vx;
    s32 y = v0->vy;
    s32 z = v0->vz;
    s32 a, b, c;

    a = (x * m->m[0][0]) >> 12;
    b = (y * m->m[0][1]) >> 12;
    c = (z * m->m[0][2]) >> 12;
    v1->vx = a + b + c + m->t[0];

    a = (x * m->m[1][0]) >> 12;
    b = (y * m->m[1][1]) >> 12;
    c = (z * m->m[1][2]) >> 12;
    v1->vy = a + b + c + m->t[1];

    a = (x * m->m[2][0]) >> 12;
    b = (y * m->m[2][1]) >> 12;
    c = (z * m->m[2][2]) >> 12;
    v1->vz = a + b + c + m->t[2];

    return v1;
}

VECTOR *ApplyMatrixLV(MATRIX *m, VECTOR *v0, VECTOR *v1) {
    s32 x = v0->vx;
    s32 y = v0->vy;
    s32 z = v0->vz;
    s32 a, b, c;

    a = (x * m->m[0][0]) >> 12;
    b = (y * m->m[0][1]) >> 12;
    c = (z * m->m[0][2]) >> 12;
    v1->vx = a + b + c + m->t[0];

    a = (x * m->m[1][0]) >> 12;
    b = (y * m->m[1][1]) >> 12;
    c = (z * m->m[1][2]) >> 12;
    v1->vy = a + b + c + m->t[1];

    a = (x * m->m[2][0]) >> 12;
    b = (y * m->m[2][1]) >> 12;
    c = (z * m->m[2][2]) >> 12;
    v1->vz = a + b + c + m->t[2];

    return v1;
}

void RotTrans(SVECTOR *v0, VECTOR *v1, s32 *flag) {
    ApplyMatrix(&g_gteMatrix, v0, v1);
}

void InitGeom(void) {
}
