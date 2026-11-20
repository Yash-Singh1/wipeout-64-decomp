/*
 * Trail.c — Ship engine flares, missile lock reticles, and exhaust trails.
 *
 * Linked in early_game in Europe. USA draws lock-on reticles via gSPLine3D
 * (5-vertex stride); Europe switches to F3DEX gSP1Triangle wedges (8-vertex
 * stride), adds 0x4000 box culling, adjusts fade rate (14 vs 16), and enables
 * exhaust ribbons for 2P/3P human players.
 */
#include "trail.h"

void DrawFlare(VECTOR *pos, MATRIX *mat, Vtx *vtx, s32 ship, s32 player) {
    s32 size;
    s32 shift;
    MATRIX unusedMat;
    Quat unusedQuat;
    VECTOR dist;
    VECTOR diag1;
    VECTOR diag2;
    SVECTOR xAxis;
    SVECTOR yAxis;
    SVECTOR zAxis;
    s32 p0x, p0y, p0z;
    s32 p1x, p1y, p1z;
    s32 x1, y1, z1;
    s32 x2, y2, z2;
    s32 ox, oy, oz;
    s32 zDist;

    if (ship < NO_OF_SHIPS) {
        if (ships[ship].combatAttr & SHIP_COMBAT_DESTROYED) {
            return;
        }
        if (frontendOptions_g.numPlayers == 1) {
            if (ship != ownShip || !(ships[ship].attr & SHIP_ATTR_RACING)) {
                size = ships[ship].speed;
                size <<= 3;
                if (size > 120) {
                    size = 120;
                }
                shift = 4;
            } else {
                size = ships[ship].thrust >> 5;
                size <<= 3;
                if (size > 80) {
                    size = 80;
                }
                shift = 3;
            }
        } else if (frontendOptions_g.numPlayers == 2) {
            if ((ship != ownShip && ship != ownShip2) || !(ships[ship].attr & SHIP_ATTR_RACING)) {
                size = ships[ship].speed;
                size <<= 3;
                if (size > 120) {
                    size = 120;
                }
                shift = 3;
            } else {
                size = ships[ship].thrust >> 5;
                size <<= 3;
                if (size > 80) {
                    size = 80;
                }
                shift = 3;
            }
        } else if (frontendOptions_g.numPlayers == 3) {
            if ((ship != ownShip && ship != ownShip2 && ship != ownShip3) ||
                !(ships[ship].attr & SHIP_ATTR_RACING)) {
                size = ships[ship].speed;
                size <<= 3;
                if (size > 120) {
                    size = 120;
                }
                shift = 4;
            } else {
                size = ships[ship].thrust >> 5;
                size <<= 3;
                if (size > 80) {
                    size = 80;
                }
                shift = 3;
            }
        } else if (frontendOptions_g.numPlayers == 4) {
            if ((ship != ownShip && ship != ownShip2 && ship != ownShip3 && ship != ownShip4) ||
                !(ships[ship].attr & SHIP_ATTR_RACING)) {
                size = ships[ship].speed;
                size <<= 3;
                if (size > 120) {
                    size = 120;
                }
                shift = 4;
            } else {
                size = ships[ship].thrust >> 5;
                size <<= 3;
                if (size > 80) {
                    size = 80;
                }
                shift = 3;
            }
        }
    }

    xAxis.vx = mat->m[0][0];
    xAxis.vy = mat->m[0][1];
    xAxis.vz = mat->m[0][2];
    yAxis.vx = mat->m[1][0];
    yAxis.vy = mat->m[1][1];
    yAxis.vz = mat->m[1][2];
    zAxis.vx = mat->m[2][0];
    zAxis.vy = mat->m[2][1];
    zAxis.vz = mat->m[2][2];

    dist.vx = (pos->vx >> 1) + mat->t[0];
    dist.vy = (pos->vy >> 1) + mat->t[1];
    dist.vz = (pos->vz >> 1) + mat->t[2];
    zDist = (dist.vx * zAxis.vx + dist.vy * zAxis.vy + dist.vz * zAxis.vz) >> 12;

    diag1.vx = (xAxis.vx + yAxis.vx) << 1;
    diag1.vy = (xAxis.vy + yAxis.vy) << 1;
    diag1.vz = (xAxis.vz + yAxis.vz) << 1;

    diag2.vx = (xAxis.vx - yAxis.vx) << 1;
    diag2.vy = (xAxis.vy - yAxis.vy) << 1;
    diag2.vz = (xAxis.vz - yAxis.vz) << 1;

    ox = (xAxis.vx << shift) >> 12;
    oy = (xAxis.vy << shift) >> 12;
    oz = (xAxis.vz << shift) >> 12;
    p0x = pos->vx - ox;
    p0y = pos->vy - oy;
    p0z = pos->vz - oz;
    p1x = pos->vx + ox;
    p1y = pos->vy + oy;
    p1z = pos->vz + oz;
    vtx[0].v.ob[0] = p0x >> 1;
    vtx[0].v.ob[1] = p0y >> 1;
    vtx[0].v.ob[2] = p0z >> 1;
    vtx[1].v.ob[0] = p1x >> 1;
    vtx[1].v.ob[1] = p1y >> 1;
    vtx[1].v.ob[2] = p1z >> 1;

    x1 = p0x - ((diag2.vx * size) >> 12);
    y1 = p0y - ((diag2.vy * size) >> 12);
    z1 = p0z - ((diag2.vz * size) >> 12);
    x2 = p1x - ((diag2.vx * size) >> 12);
    y2 = p1y - ((diag2.vy * size) >> 12);
    z2 = p1z - ((diag2.vz * size) >> 12);
    vtx[2].v.ob[0] = x1 >> 1;
    vtx[2].v.ob[1] = y1 >> 1;
    vtx[2].v.ob[2] = z1 >> 1;
    vtx[3].v.ob[0] = x2 >> 1;
    vtx[3].v.ob[1] = y2 >> 1;
    vtx[3].v.ob[2] = z2 >> 1;

    x1 = p0x + ((diag2.vx * size) >> 12);
    y1 = p0y + ((diag2.vy * size) >> 12);
    z1 = p0z + ((diag2.vz * size) >> 12);
    x2 = p1x + ((diag2.vx * size) >> 12);
    y2 = p1y + ((diag2.vy * size) >> 12);
    z2 = p1z + ((diag2.vz * size) >> 12);
    vtx[4].v.ob[0] = x1 >> 1;
    vtx[4].v.ob[1] = y1 >> 1;
    vtx[4].v.ob[2] = z1 >> 1;
    vtx[5].v.ob[0] = x2 >> 1;
    vtx[5].v.ob[1] = y2 >> 1;
    vtx[5].v.ob[2] = z2 >> 1;

    x1 = p0x - ((diag1.vx * (size >> 1)) >> 12);
    y1 = p0y - ((diag1.vy * (size >> 1)) >> 12);
    z1 = p0z - ((diag1.vz * (size >> 1)) >> 12);
    x2 = p1x - ((diag1.vx * (size >> 1)) >> 12);
    y2 = p1y - ((diag1.vy * (size >> 1)) >> 12);
    z2 = p1z - ((diag1.vz * (size >> 1)) >> 12);
    vtx[6].v.ob[0] = x1 >> 1;
    vtx[6].v.ob[1] = y1 >> 1;
    vtx[6].v.ob[2] = z1 >> 1;
    vtx[7].v.ob[0] = x2 >> 1;
    vtx[7].v.ob[1] = y2 >> 1;
    vtx[7].v.ob[2] = z2 >> 1;

    x1 = p0x + ((diag1.vx * (size >> 1)) >> 12);
    y1 = p0y + ((diag1.vy * (size >> 1)) >> 12);
    z1 = p0z + ((diag1.vz * (size >> 1)) >> 12);
    x2 = p1x + ((diag1.vx * (size >> 1)) >> 12);
    y2 = p1y + ((diag1.vy * (size >> 1)) >> 12);
    z2 = p1z + ((diag1.vz * (size >> 1)) >> 12);
    vtx[8].v.ob[0] = x1 >> 1;
    vtx[8].v.ob[1] = y1 >> 1;
    vtx[8].v.ob[2] = z1 >> 1;
    vtx[9].v.ob[0] = x2 >> 1;
    vtx[9].v.ob[1] = y2 >> 1;
    vtx[9].v.ob[2] = z2 >> 1;

    if (__builtin_abs(zDist) < 10000) {
        p0x = pos->vx - ((xAxis.vx * size) >> 12);
        p0y = pos->vy - ((xAxis.vy * size) >> 12);
        p0z = pos->vz - ((xAxis.vz * size) >> 12);
        p1x = pos->vx + ((xAxis.vx * size) >> 12);
        p1y = pos->vy + ((xAxis.vy * size) >> 12);
        p1z = pos->vz + ((xAxis.vz * size) >> 12);
        x1 = pos->vx - ((yAxis.vx * size) >> 12);
        y1 = pos->vy - ((yAxis.vy * size) >> 12);
        z1 = pos->vz - ((yAxis.vz * size) >> 12);
        x2 = pos->vx + ((yAxis.vx * size) >> 12);
        y2 = pos->vy + ((yAxis.vy * size) >> 12);
        z2 = pos->vz + ((yAxis.vz * size) >> 12);
        vtx[10].v.ob[0] = pos->vx >> 1;
        vtx[10].v.ob[1] = pos->vy >> 1;
        vtx[10].v.ob[2] = pos->vz >> 1;
        vtx[11].v.ob[0] = p0x >> 1;
        vtx[11].v.ob[1] = p0y >> 1;
        vtx[11].v.ob[2] = p0z >> 1;
        vtx[12].v.ob[0] = p1x >> 1;
        vtx[12].v.ob[1] = p1y >> 1;
        vtx[12].v.ob[2] = p1z >> 1;
        vtx[13].v.ob[0] = x1 >> 1;
        vtx[13].v.ob[1] = y1 >> 1;
        vtx[13].v.ob[2] = z1 >> 1;
        vtx[14].v.ob[0] = x2 >> 1;
        vtx[14].v.ob[1] = y2 >> 1;
        vtx[14].v.ob[2] = z2 >> 1;
        gSPVertex(g_glistp++, OS_K0_TO_PHYSICAL(vtx), 15, 0);
        gDPPipeSync(g_glistp++);
        gSP2Triangles(g_glistp++, 10, 11, 13, 0, 10, 13, 12, 0);
        gSP2Triangles(g_glistp++, 10, 14, 11, 0, 10, 12, 14, 0);
    } else {
        gSPVertex(g_glistp++, OS_K0_TO_PHYSICAL(vtx), 10, 0);
        gDPPipeSync(g_glistp++);
    }

    gSP2Triangles(g_glistp++, 0, 2, 3, 0, 0, 3, 1, 0);
    gSP2Triangles(g_glistp++, 0, 1, 4, 0, 1, 5, 4, 0);
    gSP2Triangles(g_glistp++, 0, 6, 7, 0, 0, 7, 1, 0);
    gSP2Triangles(g_glistp++, 0, 1, 8, 0, 1, 9, 8, 0);
    gDPPipeSync(g_glistp++);
}

void UpdateTarget(s32 x, s32 y, s32 z, s32 target, s32 player) {
    targetActive[player][targetBank] = 1;
    targetPos[player][targetBank].vx = x;
    targetPos[player][targetBank].vy = y;
    targetPos[player][targetBank].vz = z;
    targetAngles[player][targetBank] = targetSpin;
    targetVtx[player][targetBank][1].v.cn[1] = 0x80;
    targetVtx[player][targetBank][2].v.cn[1] = 0x80;
    targetVtx[player][targetBank][3].v.cn[1] = 0x80;
    targetVtx[player][targetBank][4].v.cn[1] = 0x80;
}

void DrawTarget(VECTOR *pos, MATRIX *mat, s32 ship, s32 player, s32 bank) {
    MATRIX rotMat;
    Quat rotQuat;
    VECTOR vec;
    VECTOR unusedVec;
    SVECTOR xAxis;
    SVECTOR yAxis;
    SVECTOR zAxis;
    s32 size;
    s32 ox1, oy1, oz1;
    s32 ox2, oy2, oz2;
    s32 tx, ty, tz;
#ifdef VERSION_EU
    s32 dx, dy, dz;
#endif

    if (missileLockTimer == 0) {
        size = 240;
    } else {
        size = missileLockTimer * 240;
    }

    xAxis.vx = mat->m[0][0];
    xAxis.vy = mat->m[0][1];
    xAxis.vz = mat->m[0][2];
    yAxis.vx = mat->m[1][0];
    yAxis.vy = mat->m[1][1];
    yAxis.vz = mat->m[1][2];
    zAxis.vx = mat->m[2][0];
    zAxis.vy = mat->m[2][1];
    zAxis.vz = mat->m[2][2];

    AxisAngleToQuat(&rotQuat, targetAngles[player][bank], &zAxis);
    QuatToMatrix(&rotQuat, &rotMat);
    rotMat.t[0] = rotMat.t[1] = rotMat.t[2] = 0;

    ApplyMatrix(&rotMat, &xAxis, &vec);
    xAxis.vx = vec.vx;
    xAxis.vy = vec.vy;
    xAxis.vz = vec.vz;

    ApplyMatrix(&rotMat, &yAxis, &vec);
    yAxis.vx = vec.vx;
    yAxis.vy = vec.vy;
    yAxis.vz = vec.vz;

#ifdef VERSION_EU
    tx = mat->t[0];
    ty = mat->t[1];
    tz = mat->t[2];
    dx = tx + (pos->vx << 1);
    dy = ty + (pos->vy << 1);
    dz = tz + (pos->vz << 1);
    dx = __builtin_abs(dx);
    dy = __builtin_abs(dy);
    dz = __builtin_abs(dz);
    if (dx < 0x4000 && dy < 0x4000 && dz < 0x4000) {
        ox1 = (xAxis.vx * size) >> 12;
        oy1 = (xAxis.vy * size) >> 12;
        oz1 = (xAxis.vz * size) >> 12;
        ox2 = (yAxis.vx * size) >> 12;
        oy2 = (yAxis.vy * size) >> 12;
        oz2 = (yAxis.vz * size) >> 12;

        targetVtx[player][bank][0].v.ob[0] = pos->vx + (mat->t[0] >> 1);
        targetVtx[player][bank][0].v.ob[1] = pos->vy + (mat->t[1] >> 1);
        targetVtx[player][bank][0].v.ob[2] = pos->vz + (mat->t[2] >> 1);

        targetVtx[player][bank][1].v.ob[0] = pos->vx + ((tx - ox1) >> 1);
        targetVtx[player][bank][1].v.ob[1] = pos->vy + ((ty - oy1) >> 1);
        targetVtx[player][bank][1].v.ob[2] = pos->vz + ((tz - oz1) >> 1);
        targetVtx[player][bank][2].v.ob[0] = pos->vx + ((tx + ox1) >> 1);
        targetVtx[player][bank][2].v.ob[1] = pos->vy + ((ty + oy1) >> 1);
        targetVtx[player][bank][2].v.ob[2] = pos->vz + ((tz + oz1) >> 1);

        targetVtx[player][bank][3].v.ob[0] = pos->vx + ((tx - ox2) >> 1);
        targetVtx[player][bank][3].v.ob[1] = pos->vy + ((ty - oy2) >> 1);
        targetVtx[player][bank][3].v.ob[2] = pos->vz + ((tz - oz2) >> 1);
        targetVtx[player][bank][4].v.ob[0] = pos->vx + ((tx + ox2) >> 1);
        targetVtx[player][bank][4].v.ob[1] = pos->vy + ((ty + oy2) >> 1);
        targetVtx[player][bank][4].v.ob[2] = pos->vz + ((tz + oz2) >> 1);

        targetVtx[player][bank][0].v.cn[0] = 0x7F;
        targetVtx[player][bank][0].v.cn[1] = 0;
        targetVtx[player][bank][0].v.cn[2] = 0;
        targetVtx[player][bank][0].v.cn[3] = 0;

        targetVtx[player][bank][1].v.cn[0] = 0x7F;
        targetVtx[player][bank][1].v.cn[2] = 0;
        targetVtx[player][bank][1].v.cn[3] = 0x7F;

        targetVtx[player][bank][2].v.cn[0] = 0x7F;
        targetVtx[player][bank][2].v.cn[2] = 0;
        targetVtx[player][bank][2].v.cn[3] = 0x7F;

        targetVtx[player][bank][3].v.cn[0] = 0x7F;
        targetVtx[player][bank][3].v.cn[2] = 0;
        targetVtx[player][bank][3].v.cn[3] = 0x7F;

        targetVtx[player][bank][4].v.cn[0] = 0x7F;
        targetVtx[player][bank][4].v.cn[2] = 0;
        targetVtx[player][bank][4].v.cn[3] = 0x7F;

        gSPVertex(g_glistp++, OS_K0_TO_PHYSICAL(&targetVtx[player][bank][0]), 5, 0);
        gDPSetRenderMode(g_glistp++, G_RM_AA_ZB_XLU_LINE, G_RM_AA_ZB_XLU_LINE2);
        gDPPipeSync(g_glistp++);
        gSP1Triangle(g_glistp++, 0, 1, 3, 0);
        gSP1Triangle(g_glistp++, 0, 3, 2, 0);
        gSP1Triangle(g_glistp++, 0, 2, 4, 0);
        gSP1Triangle(g_glistp++, 0, 4, 1, 0);
    }
#else
    ox1 = (xAxis.vx * size) >> 12;
    oy1 = (xAxis.vy * size) >> 12;
    oz1 = (xAxis.vz * size) >> 12;
    ox2 = (yAxis.vx * size) >> 12;
    oy2 = (yAxis.vy * size) >> 12;
    oz2 = (yAxis.vz * size) >> 12;

    tx = mat->t[0];
    ty = mat->t[1];
    tz = mat->t[2];

    targetVtx[player][bank][0].v.ob[0] = pos->vx + (mat->t[0] >> 1);
    targetVtx[player][bank][0].v.ob[1] = pos->vy + (mat->t[1] >> 1);
    targetVtx[player][bank][0].v.ob[2] = pos->vz + (mat->t[2] >> 1);

    targetVtx[player][bank][1].v.ob[0] = pos->vx + ((tx - ox1) >> 1);
    targetVtx[player][bank][1].v.ob[1] = pos->vy + ((ty - oy1) >> 1);
    targetVtx[player][bank][1].v.ob[2] = pos->vz + ((tz - oz1) >> 1);
    targetVtx[player][bank][2].v.ob[0] = pos->vx + ((tx + ox1) >> 1);
    targetVtx[player][bank][2].v.ob[1] = pos->vy + ((ty + oy1) >> 1);
    targetVtx[player][bank][2].v.ob[2] = pos->vz + ((tz + oz1) >> 1);

    targetVtx[player][bank][3].v.ob[0] = pos->vx + ((tx - ox2) >> 1);
    targetVtx[player][bank][3].v.ob[1] = pos->vy + ((ty - oy2) >> 1);
    targetVtx[player][bank][3].v.ob[2] = pos->vz + ((tz - oz2) >> 1);
    targetVtx[player][bank][4].v.ob[0] = pos->vx + ((tx + ox2) >> 1);
    targetVtx[player][bank][4].v.ob[1] = pos->vy + ((ty + oy2) >> 1);
    targetVtx[player][bank][4].v.ob[2] = pos->vz + ((tz + oz2) >> 1);

    targetVtx[player][bank][0].v.cn[0] = 0x7F;
    targetVtx[player][bank][0].v.cn[1] = 0;
    targetVtx[player][bank][0].v.cn[2] = 0;
    targetVtx[player][bank][0].v.cn[3] = 0;

    targetVtx[player][bank][1].v.cn[0] = 0x7F;
    targetVtx[player][bank][1].v.cn[2] = 0;
    targetVtx[player][bank][1].v.cn[3] = 0x7F;

    targetVtx[player][bank][2].v.cn[0] = 0x7F;
    targetVtx[player][bank][2].v.cn[2] = 0;
    targetVtx[player][bank][2].v.cn[3] = 0x7F;

    targetVtx[player][bank][3].v.cn[0] = 0x7F;
    targetVtx[player][bank][3].v.cn[2] = 0;
    targetVtx[player][bank][3].v.cn[3] = 0x7F;

    targetVtx[player][bank][4].v.cn[0] = 0x7F;
    targetVtx[player][bank][4].v.cn[2] = 0;
    targetVtx[player][bank][4].v.cn[3] = 0x7F;

    gSPVertex(g_glistp++, OS_K0_TO_PHYSICAL(&targetVtx[player][bank][0]), 5, 0);
    gDPSetRenderMode(g_glistp++, G_RM_AA_ZB_XLU_LINE, G_RM_AA_ZB_XLU_LINE2);
    gDPPipeSync(g_glistp++);
    gSPLine3D(g_glistp++, 1, 3, 1);
    gSPLine3D(g_glistp++, 3, 2, 1);
    gSPLine3D(g_glistp++, 2, 4, 1);
    gSPLine3D(g_glistp++, 4, 1, 1);
#endif
}

void InitTrails(void) {
    s32 i, j, k, m;
    u8 ir, ig, ib;
    u8 or_, og, ob;
    u8 cr, cg, cb;

    shipTrails = (Trail *)DAlloc(g_heap, NO_OF_SHIPS * sizeof(Trail), "Trail.c",
#ifdef VERSION_EU
        451
#else
        458
#endif
    );

    if (frontendOptions_g.numPlayers == 1) {
        for (i = 0; i < NO_OF_SHIPS; i++) {
            shipTrails[i].flareVtx[0][0].v.cn[0] = 255;
            shipTrails[i].flareVtx[0][0].v.cn[1] = 255;
            shipTrails[i].flareVtx[0][0].v.cn[2] = 255;
            shipTrails[i].flareVtx[0][0].v.cn[3] = 127;

            shipTrails[i].flareVtx[0][1].v.cn[0] = 255;
            shipTrails[i].flareVtx[0][1].v.cn[1] = 255;
            shipTrails[i].flareVtx[0][1].v.cn[2] = 255;
            shipTrails[i].flareVtx[0][1].v.cn[3] = 127;

            shipTrails[i].flareVtx[0][10].v.cn[0] = 255;
            shipTrails[i].flareVtx[0][10].v.cn[1] = 255;
            shipTrails[i].flareVtx[0][10].v.cn[2] = 255;
            shipTrails[i].flareVtx[0][10].v.cn[3] = 192;

            for (j = 2; j < 15; j++) {
                if (j != 10) {
                    shipTrails[i].flareVtx[0][j].v.cn[0] = 0;
                    shipTrails[i].flareVtx[0][j].v.cn[1] = 0;
                    shipTrails[i].flareVtx[0][j].v.cn[2] = 255;
                    shipTrails[i].flareVtx[0][j].v.cn[3] = 0;
                }
            }

            shipTrails[i].current = 0;
            shipTrails[i].colour[0][0] = 32;
            shipTrails[i].colour[0][1] = 64;
            shipTrails[i].colour[0][2] = 255;
            shipTrails[i].colour[1][0] = 192;
            shipTrails[i].colour[1][1] = 255;
            shipTrails[i].colour[1][2] = 255;
            shipTrails[i].colour[2][0] = 255;
            shipTrails[i].colour[2][1] = 255;
            shipTrails[i].colour[2][2] = 255;

            for (m = 0; m < TRAIL_SEGMENTS; m++) {
                shipTrails[i].segment[m].active = 0;
            }
        }
    } else {
        for (i = 0; i < NO_OF_SHIPS; i++) {
            if (i == ownShip || i == ownShip2 || i == ownShip3 || i == ownShip4 || ships[i].active) {
                if (i == ownShip4) {
                    ir = 255; ig = 255; ib = 255;
                    or_ = 0; og = 0; ob = 255;
                    cr = 32; cg = 64; cb = 255;
                } else if (i == ownShip2) {
                    ir = 255; ig = 255; ib = 0;
                    or_ = 255; og = 0; ob = 0;
                    cr = 255; cg = 64; cb = 32;
                } else if (i == ownShip3) {
                    ir = 0; ig = 255; ib = 255;
                    or_ = 0; og = 255; ob = 0;
                    cr = 64; cg = 255; cb = 32;
                } else if (i == ownShip) {
                    ir = 255; ig = 255; ib = 255;
                    or_ = 255; og = 255; ob = 0;
                    cr = 255; cg = 255; cb = 32;
                } else {
                    ir = 255; ig = 255; ib = 255;
                    or_ = 0; og = 0; ob = 255;
                    cr = 32; cg = 64; cb = 255;
                }

                for (k = 0; k < MAX_PLAYERS; k++) {
                    shipTrails[i].flareVtx[k][0].v.cn[0] = ir;
                    shipTrails[i].flareVtx[k][0].v.cn[1] = ig;
                    shipTrails[i].flareVtx[k][0].v.cn[2] = ib;
                    shipTrails[i].flareVtx[k][0].v.cn[3] = 127;

                    shipTrails[i].flareVtx[k][1].v.cn[0] = ir;
                    shipTrails[i].flareVtx[k][1].v.cn[1] = ig;
                    shipTrails[i].flareVtx[k][1].v.cn[2] = ib;
                    shipTrails[i].flareVtx[k][1].v.cn[3] = 127;

                    shipTrails[i].flareVtx[k][10].v.cn[0] = ir;
                    shipTrails[i].flareVtx[k][10].v.cn[1] = ig;
                    shipTrails[i].flareVtx[k][10].v.cn[2] = ib;
                    shipTrails[i].flareVtx[k][10].v.cn[3] = 192;

                    for (j = 2; j < 15; j++) {
                        if (j != 10) {
                            shipTrails[i].flareVtx[k][j].v.cn[0] = or_;
                            shipTrails[i].flareVtx[k][j].v.cn[1] = og;
                            shipTrails[i].flareVtx[k][j].v.cn[2] = ob;
                            shipTrails[i].flareVtx[k][j].v.cn[3] = 0;
                        }
                    }
                }

                shipTrails[i].current = 0;
                shipTrails[i].colour[0][0] = cr;
                shipTrails[i].colour[0][1] = cg;
                shipTrails[i].colour[0][2] = cb;
                shipTrails[i].colour[1][0] = or_;
                shipTrails[i].colour[1][1] = og;
                shipTrails[i].colour[1][2] = ob;
                shipTrails[i].colour[2][0] = ir;
                shipTrails[i].colour[2][1] = ig;
                shipTrails[i].colour[2][2] = ib;

                for (m = 0; m < TRAIL_SEGMENTS; m++) {
                    shipTrails[i].segment[m].active = 0;
                }
            }
        }
    }
}

void NewTrailSegment(s32 ship, VECTOR *left, VECTOR *right) {
    s32 i;
    s32 speed;
    s16 intensity;
    s32 current;
    TrailSegment *segment;

    speed = ships[ship].speed >> 1;
    intensity = speed;
    if (speed > 255) {
        intensity = 255;
    }

    for (i = 0; i < TRAIL_SEGMENTS; i++) {
        segment = &shipTrails[ship].segment[i];
        segment->r -= TRAIL_FADE;
        segment->g -= TRAIL_FADE;
        segment->b -= TRAIL_FADE;
        segment->a -= TRAIL_FADE;
        if (segment->r < 0) {
            segment->r = 0;
        }
        if (segment->g < 0) {
            segment->g = 0;
        }
        if (segment->b < 0) {
            segment->b = 0;
        }
        if (segment->a < 0) {
            segment->a = 0;
        }
    }

    current = shipTrails[ship].current + (TRAIL_SEGMENTS - 1);
    current %= TRAIL_SEGMENTS;
    segment = &shipTrails[ship].segment[current];
    shipTrails[ship].current = current;

    segment->active = 1;
    segment->left.vx = left->vx;
    segment->left.vy = left->vy;
    segment->left.vz = left->vz;
    segment->right.vx = right->vx;
    segment->right.vy = right->vy;
    segment->right.vz = right->vz;

    if (!(newRand() & 7)) {
        segment->r = (shipTrails[ship].colour[1][0] * intensity) >> 8;
        segment->g = (shipTrails[ship].colour[1][1] * intensity) >> 8;
        segment->b = (shipTrails[ship].colour[1][2] * intensity) >> 8;
        segment->a = (128 * intensity) >> 8;
    } else {
        segment->r = (shipTrails[ship].colour[0][0] * intensity) >> 8;
        segment->g = (shipTrails[ship].colour[0][1] * intensity) >> 8;
        segment->b = (shipTrails[ship].colour[0][2] * intensity) >> 8;
        segment->a = (128 * intensity) >> 8;
    }
}

void BuildOwnShipTrail(s32 ship, VECTOR *left, VECTOR *right) {
    s32 i;
    s16 intensity;
    TrailSegment *segment;

    if ((frontendOptions_g.numPlayers == 1 && ship == ownShip) ||
        (frontendOptions_g.numPlayers == 2 && (ship == ownShip || ship == ownShip2))) {
        intensity = ships[ship].speed >> 6;
    } else {
        intensity = ships[ship].speed >> 1;
    }

    if (intensity > 255) {
        intensity = 255;
    }

    shipTrails[ship].current = 0;
    for (i = 0; i < TRAIL_SEGMENTS; i++) {
        shipTrails[ship].segment[i].active = 1;
        shipTrails[ship].segment[i].left.vx = left->vx;
        shipTrails[ship].segment[i].left.vy = left->vy;
        shipTrails[ship].segment[i].left.vz = left->vz;
        shipTrails[ship].segment[i].right.vx = right->vx;
        shipTrails[ship].segment[i].right.vy = right->vy;
        shipTrails[ship].segment[i].right.vz = right->vz;
        left->vz -= 128;
        right->vz -= 128;
    }

    segment = &shipTrails[ship].segment[0];
    if (!(newRand() & 7)) {
        segment->r = (shipTrails[ship].colour[1][0] * intensity) >> 8;
        segment->g = (shipTrails[ship].colour[1][1] * intensity) >> 8;
        segment->b = (shipTrails[ship].colour[1][2] * intensity) >> 8;
        segment->a = (128 * intensity) >> 8;
    } else {
        segment->r = (shipTrails[ship].colour[0][0] * intensity) >> 8;
        segment->g = (shipTrails[ship].colour[0][1] * intensity) >> 8;
        segment->b = (shipTrails[ship].colour[0][2] * intensity) >> 8;
        segment->a = (128 * intensity) >> 8;
    }

    for (i = TRAIL_SEGMENTS - 1; i > 0; i--) {
        shipTrails[ship].segment[i].r = shipTrails[ship].segment[i - 1].r - TRAIL_FADE;
        shipTrails[ship].segment[i].g = shipTrails[ship].segment[i - 1].g - TRAIL_FADE;
        shipTrails[ship].segment[i].b = shipTrails[ship].segment[i - 1].b - TRAIL_FADE;
        shipTrails[ship].segment[i].a = shipTrails[ship].segment[i - 1].a - TRAIL_FADE;
        if (shipTrails[ship].segment[i].r < 0) {
            shipTrails[ship].segment[i].r = 0;
        }
        if (shipTrails[ship].segment[i].g < 0) {
            shipTrails[ship].segment[i].g = 0;
        }
        if (shipTrails[ship].segment[i].b < 0) {
            shipTrails[ship].segment[i].b = 0;
        }
        if (shipTrails[ship].segment[i].a < 0) {
            shipTrails[ship].segment[i].a = 0;
        }
    }
}

void DrawTrail(VECTOR *flarePos, MATRIX *mat, Trail *trail, s32 ship, s32 player) {
    s32 oldest;
    s32 i, n;
    s32 lx, ly, lz;
    s32 rx, ry, rz;
    s32 mx, my, mz;
    s32 v, v1, v2;

    lx = trail->segment[trail->current].left.vx;
    ly = trail->segment[trail->current].left.vy;
    lz = trail->segment[trail->current].left.vz;
    rx = trail->segment[trail->current].right.vx;
    ry = trail->segment[trail->current].right.vy;
    rz = trail->segment[trail->current].right.vz;
    mx = (lx + rx) >> 1;
    my = (ly + ry) >> 1;
    mz = (lz + rz) >> 1;
    flarePos->vx = mx;
    flarePos->vy = my;
    flarePos->vz = mz;
    DrawFlare(flarePos, mat, trail->flareVtx[player], ship, player);

    oldest = (trail->current + (TRAIL_SEGMENTS - 1)) % TRAIL_SEGMENTS;
    flarePos->vx = flarePos->vy = flarePos->vz = 0;
    g_flareCount++;

#ifdef VERSION_EU
    if ((frontendOptions_g.numPlayers == 1 && ship == ownShip) ||
        (frontendOptions_g.numPlayers == 2 && (ship == ownShip || ship == ownShip2)) ||
        (frontendOptions_g.numPlayers == 3 && (ship == ownShip || ship == ownShip2 || ship == ownShip3)) ||
        g_demoMode == 1)
#else
    if ((frontendOptions_g.numPlayers == 1 && ship == ownShip) || g_demoMode == 1)
#endif
    {
        for (i = 0; i < 8; i++) {
            if (i != oldest) {
                n = (i + 1) % TRAIL_SEGMENTS;
                if (trail->segment[i].active != 0 && trail->segment[n].active != 0) {
                    v = i * 3;
                    v1 = v + 1;
                    v2 = v + 2;

                    lx = trail->segment[i].left.vx >> 1;
                    ly = trail->segment[i].left.vy >> 1;
                    lz = trail->segment[i].left.vz >> 1;
                    rx = trail->segment[i].right.vx >> 1;
                    ry = trail->segment[i].right.vy >> 1;
                    rz = trail->segment[i].right.vz >> 1;
                    mx = (lx + rx) >> 1;
                    my = (ly + ry) >> 1;
                    mz = (lz + rz) >> 1;

                    trail->trailVtx[player][v].v.ob[0] = lx;
                    trail->trailVtx[player][v].v.ob[1] = ly;
                    trail->trailVtx[player][v].v.ob[2] = lz;
                    trail->trailVtx[player][v1].v.ob[0] = mx;
                    trail->trailVtx[player][v1].v.ob[1] = my;
                    trail->trailVtx[player][v1].v.ob[2] = mz;
                    trail->trailVtx[player][v2].v.ob[0] = rx;
                    trail->trailVtx[player][v2].v.ob[1] = ry;
                    trail->trailVtx[player][v2].v.ob[2] = rz;

                    trail->trailVtx[player][v].v.cn[0] = trail->segment[i].r;
                    trail->trailVtx[player][v].v.cn[1] = trail->segment[i].g;
                    trail->trailVtx[player][v].v.cn[2] = trail->segment[i].b;
                    trail->trailVtx[player][v].v.cn[3] = (u16)trail->segment[i].a >> 4;

                    if (i != 0) {
                        trail->trailVtx[player][v1].v.cn[0] = trail->segment[i].r;
                        trail->trailVtx[player][v1].v.cn[1] = trail->segment[i].g;
                        trail->trailVtx[player][v1].v.cn[2] = trail->segment[i].b;
                    } else {
                        trail->trailVtx[player][v1].v.cn[0] = 255;
                        trail->trailVtx[player][v1].v.cn[1] = 255;
                        trail->trailVtx[player][v1].v.cn[2] = 255;
                    }
                    trail->trailVtx[player][v1].v.cn[3] = trail->segment[i].a;

                    trail->trailVtx[player][v2].v.cn[0] = trail->segment[i].r;
                    trail->trailVtx[player][v2].v.cn[1] = trail->segment[i].g;
                    trail->trailVtx[player][v2].v.cn[2] = trail->segment[i].b;
                    trail->trailVtx[player][v2].v.cn[3] = (u16)trail->segment[i].a >> 4;
                }
            }
        }

        v = 0;
        gSPVertex(g_glistp++, OS_K0_TO_PHYSICAL(trail->trailVtx[player]), 24, 0);
        for (i = 0; i < 6; i++, v += 3) {
            if (i != oldest) {
                n = (i + 1) % TRAIL_SEGMENTS;
                if (trail->segment[i].active != 0 && trail->segment[n].active != 0) {
                    gDPPipeSync(g_glistp++);
                    gSP2Triangles(g_glistp++, v, v + 1, v + 3, 0, v + 1, v + 4, v + 3, 0);
                    gSP2Triangles(g_glistp++, v + 1, v + 2, v + 4, 0, v + 2, v + 5, v + 4, 0);
                }
            }
        }
        gDPPipeSync(g_glistp++);
    }
}

void DrawShipTrails(TrackCamera *camera, void *unused, s32 player) {
    VECTOR pos;
    s32 i;
    Object *obj;
    MATRIX *mat;
    s32 dx, dy, dz;

    g_trailCounter = 0;
    g_flareCount = 0;

    for (i = 0; i < NO_OF_SHIPS; i++) {
        obj = shipShapes[ships[i].highResTableIndex];
        obj->skeleton->super = cameras[player].camPos;
        SetSkeletonPosition(obj->skeleton,
                            ships[i].ppivot.vx >> 1,
                            ships[i].ppivot.vy >> 1,
                            ships[i].ppivot.vz >> 1);
        SetSkeletonDirectionHPR(obj->skeleton, ships[i].hdg, ships[i].pitch, ships[i].roll);
        obj->skeleton->update = 1;
        mat = GetSkeletonMatrix(obj->skeleton);
        MatrixToN64Mtx(mat, &g_mtxPool[g_mtxIndex]);
        gSPMatrix(g_glistp++, OS_K0_TO_PHYSICAL(&g_mtxPool[g_mtxIndex]),
                  G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
        g_mtxIndex++;

        dx = ships[i].ppivot.vx + (cameras[player].camPos->relative.t[0] << 1);
        dy = ships[i].ppivot.vy + (cameras[player].camPos->relative.t[1] << 1);
        dz = ships[i].ppivot.vz + (cameras[player].camPos->relative.t[2] << 1);
        if (dx * dx + dy * dy + dz * dz < 0x5000000) {
            if (frontendOptions_g.numPlayers == 1) {
                if ((ships[i].attr & SHIP_ATTR_VISIBLE) &&
                    !(ships[i].combatAttr & SHIP_COMBAT_DESTROYED) &&
                    (i != ownShip || !(ships[i].attr & SHIP_ATTR_VIEW_IN))) {
                    if (g_gameMode == 6 && frontendOptions_g.challengeMedal == 2) {
                        if (i == ownShip) {
                            DrawTrail(&pos, mat, &shipTrails[i], i, player);
                        }
                    } else if (g_gameMode == 1) {
                        if (i == ownShip || (i == g_demoShip && g_ghostActive)) {
                            DrawTrail(&pos, mat, &shipTrails[i], i, player);
                        }
                    } else {
                        DrawTrail(&pos, mat, &shipTrails[i], i, player);
                    }
                }
            } else {
#ifdef VERSION_EU
                if (ships[i].active && ships[i].visible[player] &&
                    !(ships[i].combatAttr & SHIP_COMBAT_DESTROYED)) {
                    DrawTrail(&pos, mat, &shipTrails[i], i, player);
                }
#else
                if (ships[i].visible[player]) {
                    DrawTrail(&pos, mat, &shipTrails[i], i, player);
                }
#endif
            }
        }
    }
}

void UpdateShipTrails(ShipData *shipIndex, s32 allShips) {
    s32 i;
    VECTOR left;
    VECTOR right;
    s32 isMulti;

    g_ownShipPosX = shipIndex[ownShip].ppivot.vx;
    g_ownShipPosY = shipIndex[ownShip].ppivot.vy;
    g_ownShipPosZ = shipIndex[ownShip].ppivot.vz;

    for (i = 0; i < NO_OF_SHIPS; i++) {
        left.vx = 0;
        left.vz = 0;
        left.vz -= 420;
        switch (i) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 12:
            case 13:
            case 14:
                left.vy = -20;
                break;
            default:
                left.vy = 0;
                break;
        }
        right.vx = left.vx;
        right.vy = left.vy;
        right.vz = left.vz;
        left.vx -= 50;
        right.vx += 50;
        isMulti = (frontendOptions_g.numPlayers - 1) != 0;
        if (isMulti) {
            BuildOwnShipTrail(i, &left, &right);
        } else if (i != ownShip || allShips) {
            BuildOwnShipTrail(i, &left, &right);
        } else if (!(shipIndex[i].attr & SHIP_ATTR_VIEW_IN)) {
            BuildOwnShipTrail(i, &left, &right);
        }
    }
}
