/*
 * matrix.c — 3x3 fixed-point matrix multiplication (MulMatrix0).
 *
 * Part of the Skeleton.c translation unit, multiplying 4.12 fixed-point
 * rotation matrices for object hierarchy transforms.
 */
#include "skeleton.h"

MATRIX *MulMatrix0(MATRIX *m0, MATRIX *m1, MATRIX *m2) {
    m2->m[0][0] = (m0->m[0][0] * m1->m[0][0] + m0->m[0][1] * m1->m[1][0] + m0->m[0][2] * m1->m[2][0]) >> 12;
    m2->m[0][1] = (m0->m[0][0] * m1->m[0][1] + m0->m[0][1] * m1->m[1][1] + m0->m[0][2] * m1->m[2][1]) >> 12;
    m2->m[0][2] = (m0->m[0][0] * m1->m[0][2] + m0->m[0][1] * m1->m[1][2] + m0->m[0][2] * m1->m[2][2]) >> 12;

    m2->m[1][0] = (m0->m[1][0] * m1->m[0][0] + m0->m[1][1] * m1->m[1][0] + m0->m[1][2] * m1->m[2][0]) >> 12;
    m2->m[1][1] = (m0->m[1][0] * m1->m[0][1] + m0->m[1][1] * m1->m[1][1] + m0->m[1][2] * m1->m[2][1]) >> 12;
    m2->m[1][2] = (m0->m[1][0] * m1->m[0][2] + m0->m[1][1] * m1->m[1][2] + m0->m[1][2] * m1->m[2][2]) >> 12;

    m2->m[2][0] = (m0->m[2][0] * m1->m[0][0] + m0->m[2][1] * m1->m[1][0] + m0->m[2][2] * m1->m[2][0]) >> 12;
    m2->m[2][1] = (m0->m[2][0] * m1->m[0][1] + m0->m[2][1] * m1->m[1][1] + m0->m[2][2] * m1->m[2][1]) >> 12;
    m2->m[2][2] = (m0->m[2][0] * m1->m[0][2] + m0->m[2][1] * m1->m[1][2] + m0->m[2][2] * m1->m[2][2]) >> 12;

    return m2;
}
