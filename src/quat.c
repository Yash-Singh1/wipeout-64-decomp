#include "quat.h"

void AxisAngleToQuat(Quat *q, s32 angle, SVECTOR *axis) {
    s16 half;
    s32 s;

    half = angle >> 1;
    s = (s16)rsin(half);
    q->w = (s16)rcos(half);
    q->x = (axis->vx * s) >> 12;
    q->y = (axis->vy * s) >> 12;
    q->z = (axis->vz * s) >> 12;
}

void QuatToMatrix(Quat *q, MATRIX *m) {
    s32 x = q->x;
    s32 y = q->y;
    s32 z = q->z;
    s32 w = q->w;
    s32 xy = (x * y) >> 11;
    s32 xz = (x * z) >> 11;
    s32 yz = (y * z) >> 11;
    s32 xx = (x * x) >> 11;
    s32 yy = (y * y) >> 11;
    s32 zz = (z * z) >> 11;
    s32 wx = (w * x) >> 11;
    s32 wy = (w * y) >> 11;
    s32 wz = (w * z) >> 11;

    m->m[0][0] = 4096 - yy - zz;
    m->m[0][1] = xy - wz;
    m->m[0][2] = xz + wy;
    m->m[1][0] = xy + wz;
    m->m[1][1] = 4096 - xx - zz;
    m->m[1][2] = yz - wx;
    m->m[2][0] = xz - wy;
    m->m[2][1] = yz + wx;
    m->m[2][2] = 4096 - xx - yy;
}
