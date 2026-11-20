#include "utils.h"

s32 SectionSearch(ShipData *playerShip) {
    TrackSection *trk, *nearestSection;
    s32 smallestMag;
    VECTOR centreToShip;
    char workspace[40]; /* matches 40-byte unused stack reservation in retail frames */
    s32 mag;
    s32 i;
    s16 juncChecked = 0;
    TrackSection *junc;
    s16 sectionFlags;

    trk = playerShip->nearTrkSect;
    for (i = 0; i < 3; i++) {
        trk = trk->prevSection.ptr;
    }

    sectionFlags = trk->flags;
    centreToShip.vx = playerShip->ppivot.vx - trk->centre.vx;
    centreToShip.vy = (playerShip->ppivot.vy - trk->centre.vy) >> 2;
    centreToShip.vz = playerShip->ppivot.vz - trk->centre.vz;
    smallestMag = (centreToShip.vx * centreToShip.vx) +
                  (centreToShip.vy * centreToShip.vy) +
                  (centreToShip.vz * centreToShip.vz);

    if (trk->junction.index != -1) {
        junc = trk->junction.ptr;
        juncChecked = 1;
    }

    nearestSection = trk;
    for (i = 0; i < 6; i++) {
        trk = trk->nextSection.ptr;
        if (trk->junction.index != -1) {
            junc = trk->junction.ptr;
            juncChecked = 1;
        }
        centreToShip.vx = playerShip->ppivot.vx - trk->centre.vx;
        centreToShip.vy = (playerShip->ppivot.vy - trk->centre.vy) >> 2;
        centreToShip.vz = playerShip->ppivot.vz - trk->centre.vz;
        mag = (centreToShip.vx * centreToShip.vx) +
              (centreToShip.vy * centreToShip.vy) +
              (centreToShip.vz * centreToShip.vz);
        if (mag < smallestMag) {
            smallestMag = mag;
            nearestSection = trk;
        }
    }

    if (juncChecked == 1) {
        if (junc->flags & JunctionStart) {
            trk = junc;
        } else if (sectionFlags == JunctionSection) {
            trk = junc->nextSection.ptr;
        } else {
            trk = junc;
        }

        for (i = 0; i < 6; i++) {
            centreToShip.vx = playerShip->ppivot.vx - trk->centre.vx;
            centreToShip.vy = (playerShip->ppivot.vy - trk->centre.vy) >> 2;
            centreToShip.vz = playerShip->ppivot.vz - trk->centre.vz;
            mag = (centreToShip.vx * centreToShip.vx) +
                  (centreToShip.vy * centreToShip.vy) +
                  (centreToShip.vz * centreToShip.vz);
            if (mag < smallestMag) {
                smallestMag = mag;
                nearestSection = trk;
            }
            if (junc->flags & JunctionEnd) {
                trk = trk->prevSection.ptr;
            } else {
                trk = trk->nextSection.ptr;
            }
        }
    }

    smallestMag = SquareRoot0(smallestMag);
    if (smallestMag > 3700) {
        playerShip->attr |= FLYING;
    } else {
        playerShip->attr &= ~FLYING;
    }
    playerShip->nearTrkSect = nearestSection;
    return smallestMag;
}

TrackSection *FindNearestSection(TrackSection *trk, VECTOR pos) {
    TrackSection *nearestSection;
    s32 smallestMag;
    VECTOR centreToPos;
    char workspace[40]; /* matches 40-byte unused stack reservation in retail frames */
    s32 mag;
    s32 i;
    s16 juncChecked = 0;
    TrackSection *junc;
    s16 sectionFlags;

    for (i = 0; i < 3; i++) {
        trk = trk->prevSection.ptr;
    }

    sectionFlags = trk->flags;
    centreToPos.vx = pos.vx - trk->centre.vx;
    centreToPos.vy = pos.vy - trk->centre.vy;
    centreToPos.vz = pos.vz - trk->centre.vz;
    smallestMag = (centreToPos.vx * centreToPos.vx) +
                  (centreToPos.vy * centreToPos.vy) +
                  (centreToPos.vz * centreToPos.vz);
    nearestSection = trk;
    if (trk->junction.index != -1) {
        junc = trk->junction.ptr;
        juncChecked = 1;
    }

    for (i = 0; i < 6; i++) {
        trk = trk->nextSection.ptr;
        if (trk->junction.index != -1) {
            junc = trk->junction.ptr;
            juncChecked = 1;
        }
        centreToPos.vx = pos.vx - trk->centre.vx;
        centreToPos.vy = pos.vy - trk->centre.vy;
        centreToPos.vz = pos.vz - trk->centre.vz;
        mag = (centreToPos.vx * centreToPos.vx) +
              (centreToPos.vy * centreToPos.vy) +
              (centreToPos.vz * centreToPos.vz);
        if (mag < smallestMag) {
            smallestMag = mag;
            nearestSection = trk;
        }
    }

    if (juncChecked == 1) {
        if (junc->flags & JunctionStart) {
            trk = junc;
        } else if (sectionFlags == JunctionSection) {
            trk = junc->nextSection.ptr;
        } else {
            trk = junc;
        }

        for (i = 0; i < 6; i++) {
            centreToPos.vx = pos.vx - trk->centre.vx;
            centreToPos.vy = pos.vy - trk->centre.vy;
            centreToPos.vz = pos.vz - trk->centre.vz;
            mag = (centreToPos.vx * centreToPos.vx) +
                  (centreToPos.vy * centreToPos.vy) +
                  (centreToPos.vz * centreToPos.vz);
            if (mag < smallestMag) {
                smallestMag = mag;
                nearestSection = trk;
            }
            if (junc->flags & JunctionEnd) {
                trk = trk->prevSection.ptr;
            } else {
                trk = trk->nextSection.ptr;
            }
        }
    }

    return nearestSection;
}

s32 abs(s32 value) {
    s32 result;

    result = value;
    if (result < 0) {
        result = -result;
    }
    return result;
}

s32 GetAng(VECTOR vec1, VECTOR vec2) {
    s32 product;
    s32 mag;

    product = ScalarProduct(vec1, vec2);
    mag = (GetMagnitude(vec1) * GetMagnitude(vec2)) >> 12;
    if (mag == 0) {
        product = 4095;
    } else {
        product = product / mag;
    }
    if (product > 4095) {
        product = 4095;
    } else if (product < -4095) {
        product = -4095;
    }
    return arccos(product);
}

s32 ScalarProduct(VECTOR v, VECTOR w) {
    return v.vx * w.vx + v.vy * w.vy + v.vz * w.vz;
}

void CrossProduct(VECTOR *out, VECTOR a, VECTOR b) {
    out->vx = (a.vy * b.vz - a.vz * b.vy) >> 9;
    out->vy = (a.vz * b.vx - a.vx * b.vz) >> 9;
    out->vz = (a.vx * b.vy - a.vy * b.vx) >> 9;
}

void MinusVector(VECTOR start, VECTOR end, VECTOR *result) {
    result->vx = end.vx - start.vx;
    result->vy = end.vy - start.vy;
    result->vz = end.vz - start.vz;
}

s32 GetMagnitude(VECTOR vec1) {
    s32 mag;

    mag = ((vec1.vx * vec1.vx) + (vec1.vy * vec1.vy)) + (vec1.vz * vec1.vz);
    mag = SquareRoot0(mag);
    return mag;
}

s32 arccos(s32 value) {
    s32 angle;
    s32 absValue;

    if (value < 0) {
        absValue = -value;
    } else {
        absValue = value;
    }
    angle = g_arccosTable[absValue];
    if (value < 0) {
        angle = 8192 + (8192 - angle);
    }
    return angle;
}

s32 sar(s32 a, s32 n) {
    if (a > 0) {
        return a >> n;
    } else {
        a = (-a) >> n;
        return -a;
    }
}

s32 ang(s32 angle) {
    angle &= 0xFFF;
    if (angle < 0x801) {
        return angle;
    }
    return angle - 0x1000;
}

void assign(VECTOR *out, VECTOR v) {
    out->vx = v.vx;
    out->vy = v.vy;
    out->vz = v.vz;
}

s32 newRand(void) {
    s32 seed;

    seed = (g_randSeed >> 1) ^ (g_randSeed * 8);
    g_randSeed = seed;
    return seed & 0x7FFF;
}
