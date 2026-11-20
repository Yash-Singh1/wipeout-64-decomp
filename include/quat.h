#ifndef WIPEOUT64_QUAT_H
#define WIPEOUT64_QUAT_H

#include "common.h"
#include "gte.h"

typedef struct {
    s32 w;
    s32 x;
    s32 y;
    s32 z;
} Quat;

void AxisAngleToQuat(Quat *q, s32 angle, SVECTOR *axis);
void QuatToMatrix(Quat *q, MATRIX *m);

#endif /* WIPEOUT64_QUAT_H */
