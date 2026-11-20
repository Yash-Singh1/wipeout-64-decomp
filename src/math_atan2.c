#include "common.h"

extern u16 g_atanTable[];

u32 BitLength(u32 v);

s16 Atan2Fix16(s32 x, s32 y) {
    s32 octant = 0;
    u32 ux;
    u32 uy;
    u32 ratio;
    u32 rem;
    u32 bits;
    s16 angle;

    if (x == 0) {
        if (y == 0) {
            return 0;
        }
        if (y > 0) {
            return 0x4000;
        }
        return -0x4000;
    }

    ux = (x >= 0) ? x : -x;
    uy = (y >= 0) ? y : -y;

    if (ux < uy) {
        u32 tmp = uy;
        uy = ux;
        ux = tmp;
        octant = 1;
    }

    if (uy > 0xFFFF) {
        bits = BitLength(uy);
        if (bits > 20) {
            bits -= 20;
            ux >>= bits;
            uy <<= 12 - bits;
        } else {
            uy <<= 12;
        }
    } else {
        uy <<= 12;
    }

    ratio = uy / ux;
    rem = uy % ux;
    if (rem * 2 > ux) {
        ratio++;
    }

    angle = g_atanTable[ratio];

    if (x < 0) {
        if (y < 0) {
            if (octant) {
                return -0x4000 - angle;
            }
            return angle + -0x8000;
        }
        if (octant) {
            return angle + 0x4000;
        }
        return -0x8000 - angle;
    }

    if (y < 0) {
        if (octant) {
            return angle - 0x4000;
        }
        return -angle;
    }

    if (octant) {
        return 0x4000 - angle;
    }
    return angle;
}

u32 BitLength(u32 v) {
    u32 count = 0;

    while (v != 0) {
        v >>= 1;
        count++;
    }
    return count;
}
