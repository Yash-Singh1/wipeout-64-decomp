/*
 * effects.c — Start boom, countdown lights, camera shake, rumble, and overlays.
 *
 * Manages trackside start droids/lights, ship combat visual effects, camera
 * shake decay, and Rumble Pak pulse scheduling. Linked in early_game in Europe,
 * where US helper DrawEffects2 is merged directly into DrawEffects.
 */
#include "effects.h"

void SetDisplayListPrimColour(Gfx *gfx, s32 r, s32 g, s32 b, s32 a, s32 count) {
    while (gfx->dma.cmd != G_ENDDL) {
        if ((s8)gfx->dma.cmd == (s8)G_SETPRIMCOLOR) {
            count--;
            if (count <= 0) {
                break;
            }
        }
        gfx++;
    }
    if (gfx->dma.cmd != G_ENDDL) {
        gDPSetPrimColor(gfx, 0, 0, r, g, b, a);
    }
}

void LoadEffectTextures(void) {
    WADFILE *wad;

    wad = OpenWadFile((u32)_effectTexturesFile);

    wad->current = 11;
    effectTex0Start = TextureTableCount;
    LoadCmpFiles(WADLoadCompressedTextureSequence(wad));

    wad->current = 12;
    effectTex0Count = TextureTableCount - effectTex0Start;
    effectTex1Start = TextureTableCount;
    LoadCmpFiles(WADLoadCompressedTextureSequence(wad));

    wad->current = 13;
    effectTex1Count = TextureTableCount - effectTex1Start;
    effectTex2Start = TextureTableCount;
    LoadCmpFiles(WADLoadCompressedTextureSequence(wad));

    wad->current = 9;
    effectTex2Count = TextureTableCount - effectTex2Start;
    effectTex3Start = TextureTableCount;
    LoadCmpFiles(WADLoadCompressedTextureSequence(wad));

    effectTex3Count = TextureTableCount - effectTex3Start;
    CloseWadFile(wad);
}

Gfx *DrawEffects(WeaponData *weapons, ShipData *ships, s32 player, Mtx *projMtx, u16 *perspNorm, Gfx *glistp) {
    MATRIX rotA0, rotA1, rotB0, rotB1, rotC0, rotC1;
    VECTOR camPos;
    Quat quat;
    SVECTOR axis;
    CVECTOR flashCol;
    Gfx *dlStart;
    MATRIX *camMat;
    N64Mtx *savedMtx;
    s32 mtxIdx;
    s32 vpBase;
    s32 used;
    s32 i;
#ifdef VERSION_EU
    s32 j;
    s32 playerShip;
    s32 targetShip;
#endif

    switch (frontendOptions_g.numPlayers) {
        case 1:
            vpBase = 0;
            break;
        case 2:
            if (frontendOptions_g.splitMode == 1) {
                vpBase = 1;
            } else {
                vpBase = 3;
            }
            break;
        case 3:
        case 4:
            vpBase = 5;
            break;
    }

    dlStart = g_glistp;
    if (frontendOptions_g.numPlayers == 3) {
        g_glistp = Set3PlayerViewport(player, projMtx, perspNorm, g_glistp);
    } else {
        gSPViewport(g_glistp++, &g_vpFullScreen[vpBase + player]);
    }

    gDPSetRenderMode(g_glistp++, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF2);
    gDPSetCombineMode(g_glistp++, G_CC_SHADE, G_CC_SHADE);

    camPos.vx = cameras[player].camPos->relative.t[0];
    camPos.vy = cameras[player].camPos->relative.t[1];
    camPos.vz = cameras[player].camPos->relative.t[2];
    mtxIdx = g_mtxIndex;
    SetSkeletonPosition(cameras[player].camPos, 0, 0, 0);
    cameras[player].camPos->update = 1;
    camMat = GetSkeletonMatrix(cameras[player].camPos);
    MatrixToN64Mtx(camMat, &g_mtxPool[g_mtxIndex]);
    gSPMatrix(g_glistp++, OS_K0_TO_PHYSICAL(&g_mtxPool[g_mtxIndex]),
              G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    g_mtxIndex++;
    savedMtx = &g_mtxPool[mtxIdx];

    axis.vx = camMat->m[2][0];
    axis.vy = camMat->m[2][1];
    axis.vz = camMat->m[2][2];

    AxisAngleToQuat(&quat, g_effectTexScroll2, &axis);
    QuatToMatrix(&quat, &rotA0);
    rotA0.t[0] = rotA0.t[1] = rotA0.t[2] = 0;

    AxisAngleToQuat(&quat, g_effectTexScroll2 + 256, &axis);
    QuatToMatrix(&quat, &rotA1);
    rotA1.t[0] = rotA1.t[1] = rotA1.t[2] = 0;

    AxisAngleToQuat(&quat, g_effectTexScroll0, &axis);
    QuatToMatrix(&quat, &rotB0);
    rotB0.t[0] = rotB0.t[1] = rotB0.t[2] = 0;

    AxisAngleToQuat(&quat, g_effectTexScroll0 + 256, &axis);
    QuatToMatrix(&quat, &rotB1);
    rotB1.t[0] = rotB1.t[1] = rotB1.t[2] = 0;

    AxisAngleToQuat(&quat, g_effectTexScroll1, &axis);
    QuatToMatrix(&quat, &rotC0);
    rotC0.t[0] = rotC0.t[1] = rotC0.t[2] = 0;

    AxisAngleToQuat(&quat, g_effectTexScroll1 + 256, &axis);
    QuatToMatrix(&quat, &rotC1);
    rotC1.t[0] = rotC1.t[1] = rotC1.t[2] = 0;

#ifdef VERSION_EU
    /* Europe merges US DrawEffects2 into DrawEffects (single combined effects pass). */
    for (i = 0; i < NO_OF_SHIPS; i++) {
        if ((ships[i].attr & 0x8000) && !(ships[i].combatAttr & 0x8000)) {
            for (j = 0; j < 2;) {
                AddShieldSparks(&ships[i].ppivot, 0);
                AddShieldSparks(&ships[i].ppivot, 1);
                if (i == ownShip) {
                    j++;
                } else {
                    j++;
                }
            }
        }
    }
#endif

    for (i = 0; i < NO_OF_WEAPONS; i++) {
        if (weapons[i].type == 2 && weapons[i].update == (void *)UpdateMissile && weapons[i].count > 0) {
            DrawMissileTrail(&weapons[i].pos, &camPos, camMat, &rotB0, &rotB1, player);
        }
        if (weapons[i].type == 1 && weapons[i].update == (void *)UpdateRocket && weapons[i].count > 0) {
            DrawRocketTrail(&weapons[i].pos, &camPos, camMat, &rotA0, &rotA1, player);
        }
        if (weapons[i].type == 18 && weapons[i].update == (void *)UpdateMissile && weapons[i].count > 0) {
            DrawSuperTrail(&weapons[i].pos, &camPos, camMat, &rotC0, &rotC1, player, 0);
        }
        if (weapons[i].type == 21 && weapons[i].update == (void *)UpdateMissile && weapons[i].count > 0) {
            DrawSuperTrail(&weapons[i].pos, &camPos, camMat, &rotC0, &rotC1, player, 1);
        }
        if (weapons[i].count == -1 || weapons[i].count == -2) {
            if (weapons[i].count == -1) {
                flashCol.r = flashCol.g = flashCol.b = 255;
                AddExplosionFlash(weapons[i].pos.vx, weapons[i].pos.vy, weapons[i].pos.vz, 12, &flashCol);
            }
            if (weapons[i].type == 20) {
                AddLargeExplosion(weapons[i].pos.vx, weapons[i].pos.vy, weapons[i].pos.vz, -1);
            } else {
                AddSmallExplosion(weapons[i].pos.vx, weapons[i].pos.vy, weapons[i].pos.vz, -1);
            }
            weapons[i].count = 0;
        }
#ifdef VERSION_EU
        if (weapons[i].type == 5 && weapons[i].update == (void *)UpdateElecBolt && weapons[i].count > 0) {
            AddEboltParticles(particleSlots, 8, &weapons[i].pos);
            for (j = 0; j < 3; j++) {
                AddShieldSparks(&weapons[i].pos, 0);
                AddShieldSparks(&weapons[i].pos, 1);
            }
        }
#endif
    }

    DrawExplosions(&camPos, camMat, player);

#ifdef VERSION_EU
    switch (player) {
        case 0:
            playerShip = ownShip;
            break;
        case 1:
            playerShip = ownShip2;
            break;
        case 2:
            playerShip = ownShip3;
            break;
        case 3:
            playerShip = ownShip4;
            break;
    }

    if ((ships[playerShip].attr & 0x40) &&
        (ships[playerShip].weaponType == 2 || ships[playerShip].weaponType == 5 ||
         ships[playerShip].weaponType == 18) &&
        ships[playerShip].targetShip < NO_OF_SHIPS) {
        targetShip = ships[playerShip].targetShip;
        {
            s32 tx = ships[targetShip].ppivot.vx;
            s32 ty = ships[targetShip].ppivot.vy;
            s32 tz = ships[targetShip].ppivot.vz;
            UpdateTarget(tx, ty, tz, targetShip, player);
        }
        if (missileLockTimer > 0) {
            for (i = 0; i < TARGET_BANKS; i++) {
                if (targetActive[player][i]) {
                    camMat->t[0] = targetPos[player][i].vx;
                    camMat->t[1] = targetPos[player][i].vy;
                    camMat->t[2] = targetPos[player][i].vz;
                    targetVtx[player][i][0].v.cn[1] -= 14;
                    targetVtx[player][i][1].v.cn[1] -= 14;
                    targetVtx[player][i][2].v.cn[1] -= 14;
                    targetVtx[player][i][3].v.cn[1] -= 14;
                    targetVtx[player][i][4].v.cn[1] -= 14;
                    targetVtx[player][i][5].v.cn[1] -= 14;
                    DrawTarget(&camPos, camMat, playerShip, player, i);
                }
            }
        } else {
            camMat->t[0] = ships[targetShip].ppivot.vx;
            camMat->t[1] = ships[targetShip].ppivot.vy;
            camMat->t[2] = ships[targetShip].ppivot.vz;
            DrawTarget(&camPos, camMat, playerShip, player, targetBank);
        }
    } else {
        for (i = 0; i < TARGET_BANKS; i++) {
            targetActive[player][i] = 0;
        }
    }

    DrawParticleSystem(&camPos, particleSlots, player);
    DrawShieldSparks(&camPos, player);
#endif

    gDPSetRenderMode(g_glistp++, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF2);
    gDPPipelineMode(g_glistp++, G_PM_1PRIMITIVE);
    gDPSetCombineMode(g_glistp++, G_CC_MODULATERGBA, G_CC_MODULATERGBA);
    DrawExplode(&camPos, camMat, player);

    DrawShadow(&camPos, shipShapes, ships, &cameras[player], player);

    gDPSetCombineMode(g_glistp++, G_CC_MODULATERGBA, G_CC_MODULATERGBA);
    gDPSetRenderMode(g_glistp++, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF2);
    DrawPlasmaWall(&camPos, player);

    DrawSmoke(&camPos, camMat, player);

    gDPPipelineMode(g_glistp++, G_PM_NPRIMITIVE);
    gDPSetCombineMode(g_glistp++, G_CC_SHADE, G_CC_SHADE);
    gDPPipeSync(g_glistp++);
    gSPEndDisplayList(g_glistp++);

    g_dlistUsed = (s32)g_glistp - (s32)dlStart;
#ifdef VERSION_EU
    g_dlistPct = (u32)g_dlistUsed / 60;
#else
    g_dlistPct = (g_dlistUsed * 100) >> 12;
#endif
    AddRenderDisplayList(&g_effectsRenderList, &dlStart, (Mtx *)savedMtx);
    SetSkeletonPosition(cameras[player].camPos, camPos.vx, camPos.vy, camPos.vz);
    return glistp;
}

#ifndef VERSION_EU
Gfx *DrawEffects2(WeaponData *weapons, ShipData *ships, s32 player, Mtx *projMtx, u16 *perspNorm, Gfx *glistp) {
    VECTOR camPos;
    Quat unusedQuat;
    SVECTOR unusedAxis;
    Gfx *dlStart;
    MATRIX *camMat;
    N64Mtx *savedMtx;
    s32 vpBase;
    s32 i, j;
    s32 playerShip;
    s32 targetShip;
    s32 used;

    switch (frontendOptions_g.numPlayers) {
        case 1:
            vpBase = 0;
            break;
        case 2:
            if (frontendOptions_g.splitMode == 1) {
                vpBase = 1;
            } else {
                vpBase = 3;
            }
            break;
        case 3:
        case 4:
            vpBase = 5;
            break;
    }

    dlStart = g_glistp;
    if (frontendOptions_g.numPlayers == 3) {
        g_glistp = Set3PlayerViewport(player, projMtx, perspNorm, g_glistp);
    } else {
        gSPViewport(g_glistp++, &g_vpFullScreen[vpBase + player]);
    }

    camPos.vx = cameras[player].camPos->relative.t[0];
    camPos.vy = cameras[player].camPos->relative.t[1];
    camPos.vz = cameras[player].camPos->relative.t[2];
    SetSkeletonPosition(cameras[player].camPos, 0, 0, 0);
    cameras[player].camPos->update = 1;
    camMat = GetSkeletonMatrix(cameras[player].camPos);
    MatrixToN64Mtx(camMat, &g_mtxPool[g_mtxIndex]);
    savedMtx = &g_mtxPool[g_mtxIndex++];

    switch (player) {
        case 0:
            playerShip = ownShip;
            break;
        case 1:
            playerShip = ownShip2;
            break;
        case 2:
            playerShip = ownShip3;
            break;
        case 3:
            playerShip = ownShip4;
            break;
    }

    for (i = 0; i < NO_OF_SHIPS; i++) {
        if ((ships[i].attr & 0x8000) && !(ships[i].combatAttr & 0x8000)) {
            for (j = 0; j < 2; j++) {
                VECTOR *pos;
                AddShieldSparks(&ships[i].ppivot, 0);
                /* Preserves hoisted %hi(ownShip) ($s5) and shipPtr ($s2) / i ($s3) regalloc */
                if (i == ownShip) {
                    pos = &ships[i].ppivot;
                } else {
                    pos = &ships[i].ppivot;
                }
                AddShieldSparks(pos, 1);
            }
        }
    }

    for (i = 0; i < NO_OF_WEAPONS; i++) {
        if (weapons[i].type == 5 && weapons[i].update == (void *)UpdateElecBolt && weapons[i].count > 0) {
            AddEboltParticles(particleSlots, 8, &weapons[i].pos);
            for (j = 0; j < 3; j++) {
                AddShieldSparks(&weapons[i].pos, 0);
                AddShieldSparks(&weapons[i].pos, 1);
            }
        }
    }

    if ((ships[playerShip].attr & 0x40) &&
        (ships[playerShip].weaponType == 2 || ships[playerShip].weaponType == 5 ||
         ships[playerShip].weaponType == 18) &&
        ships[playerShip].targetShip < NO_OF_SHIPS) {
        targetShip = ships[playerShip].targetShip;
        {
            s32 tx = ships[targetShip].ppivot.vx;
            s32 ty = ships[targetShip].ppivot.vy;
            s32 tz = ships[targetShip].ppivot.vz;
            UpdateTarget(tx, ty, tz, targetShip, player);
        }
        if (missileLockTimer > 0) {
            for (i = 0; i < TARGET_BANKS; i++) {
                if (targetActive[player][i]) {
                    camMat->t[0] = targetPos[player][i].vx;
                    camMat->t[1] = targetPos[player][i].vy;
                    camMat->t[2] = targetPos[player][i].vz;
                    targetVtx[player][i][1].v.cn[1] -= 14;
                    targetVtx[player][i][2].v.cn[1] -= 14;
                    targetVtx[player][i][3].v.cn[1] -= 14;
                    targetVtx[player][i][4].v.cn[1] -= 14;
                    DrawTarget(&camPos, camMat, ownShip, player, i);
                }
            }
        } else {
            camMat->t[0] = ships[targetShip].ppivot.vx;
            camMat->t[1] = ships[targetShip].ppivot.vy;
            camMat->t[2] = ships[targetShip].ppivot.vz;
            DrawTarget(&camPos, camMat, ownShip, player, targetBank);
        }
    } else {
        for (i = 0; i < TARGET_BANKS; i++) {
            targetActive[player][i] = 0;
        }
    }

    DrawShieldSparks(&camPos, player);
    DrawParticleSystem(&camPos, particleSlots, player);

    gSPEndDisplayList(g_glistp++);
    used = (s32)g_glistp - (s32)dlStart;
    g_dlistUsed = used;
    g_dlistPct = (used * 100) / 9216;
    AddRenderDisplayList(&g_effects2RenderList, &dlStart, (Mtx *)savedMtx);
    SetSkeletonPosition(cameras[player].camPos, camPos.vx, camPos.vy, camPos.vz);
    return glistp;
}
#endif

Gfx *DrawMedalEffects(void *projMtx, void *perspNorm, Gfx *glistp) {
    Gfx *dlStart;
    s32 mtxIdx;

    dlStart = g_glistp;
    gSPViewport(g_glistp++, &g_vpFullScreen[0]);
    gDPPipelineMode(g_glistp++, G_PM_1PRIMITIVE);
    mtxIdx = g_mtxIndex;
    if (g_gameMode == 6) {
        DrawFinishMedal(&cameras[0], projMtx);
    }
    gDPPipelineMode(g_glistp++, G_PM_NPRIMITIVE);
    gDPPipeSync(g_glistp++);
    gSPEndDisplayList(g_glistp++);
    AddRenderDisplayList(&g_medalRenderList, &dlStart, (Mtx *)&g_mtxPool[mtxIdx]);
    return glistp;
}

void UpdateEffectAnims(void) {
    g_targetFlash = 0;
    targetSpin += 128;
    if (missileLockTimer > 0) {
        missileLockTimer--;
    }
    targetBank = (targetBank + 1) & 7;
    g_effectTexScroll0 = (g_effectTexScroll0 + 160) & 0xFFF;
    g_effectTexScroll2 = (g_effectTexScroll2 - 96) & 0xFFF;
    g_effectTexScroll1 = (g_effectTexScroll1 + 320) & 0xFFF;
}

void DrawTrailEffects(void *projMtx, void *perspNorm, s32 player) {
    Gfx *dlStart;
    s32 mtxIdx;
    s32 vpBase;
    s32 used;

    switch (frontendOptions_g.numPlayers) {
        case 1:
            vpBase = 0;
            break;
        case 2:
            if (frontendOptions_g.splitMode == 1) {
                vpBase = 1;
            } else {
                vpBase = 3;
            }
            break;
        case 3:
        case 4:
            vpBase = 5;
            break;
    }

    dlStart = g_glistp;
    gSPViewport(g_glistp++, &g_vpFullScreen[vpBase + player]);
    gDPSetRenderMode(g_glistp++, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF2);
    gDPPipelineMode(g_glistp++, G_PM_1PRIMITIVE);
    gDPSetCombineMode(g_glistp++, G_CC_SHADE, G_CC_SHADE);
    gDPPipeSync(g_glistp++);
    mtxIdx = g_mtxIndex;
    DrawShipTrails(projMtx, perspNorm, player);
    gDPPipeSync(g_glistp++);
    gDPSetCombineMode(g_glistp++, G_CC_MODULATERGBA, G_CC_MODULATERGBA);
    gDPPipelineMode(g_glistp++, G_PM_1PRIMITIVE);
    if (g_demoMode != 1 && !g_playerFinished[player]) {
        DrawTargetOverlay(projMtx, perspNorm, player);
    }
    DrawParticles(projMtx, perspNorm, player);
#ifdef VERSION_EU
    gDPPipeSync(g_glistp++);
#endif
    gDPPipelineMode(g_glistp++, G_PM_NPRIMITIVE);
    gSPEndDisplayList(g_glistp++);
    used = (s32)g_glistp - (s32)dlStart;
    g_dlistUsed = used;
    g_dlistPct = (used * 100) / 4096;
    AddRenderDisplayList(&g_trailRenderList, &dlStart, (Mtx *)&g_mtxPool[mtxIdx]);
}

void InitPlasmaWall(WeaponData *weapon, ShipData *ship) {
    Face *face;
    VECTOR v0, v1, v2, v3;
    VECTOR base;
    VECTOR left;
    VECTOR right;
    s32 i, j;

    g_plasmaWall.weapon = weapon;
    g_plasmaWall.timer = weapon->count;
    if (weapon->faceIndex > track->faceCount) {
        weapon->faceIndex = 0;
    }
    face = &((Face *)track->faces)[weapon->faceIndex];

    v0.vx = ((VECTOR *)track->vertices)[face->vertex[0]].vx;
    v0.vy = ((VECTOR *)track->vertices)[face->vertex[0]].vy;
    v0.vz = ((VECTOR *)track->vertices)[face->vertex[0]].vz;

    v1.vx = ((VECTOR *)track->vertices)[face->vertex[1]].vx;
    v1.vy = ((VECTOR *)track->vertices)[face->vertex[1]].vy;
    v1.vz = ((VECTOR *)track->vertices)[face->vertex[1]].vz;

    v2.vx = ((VECTOR *)track->vertices)[face->vertex[2]].vx;
    v2.vy = ((VECTOR *)track->vertices)[face->vertex[2]].vy;
    v2.vz = ((VECTOR *)track->vertices)[face->vertex[2]].vz;

    v3.vx = ((VECTOR *)track->vertices)[face->vertex[3]].vx;
    v3.vy = ((VECTOR *)track->vertices)[face->vertex[3]].vy;
    v3.vz = ((VECTOR *)track->vertices)[face->vertex[3]].vz;

    left.vx = (v0.vx + v3.vx) >> 1;
    left.vy = (v0.vy + v3.vy) >> 1;
    left.vz = (v0.vz + v3.vz) >> 1;

    right.vx = (v1.vx + v2.vx) >> 1;
    right.vy = (v1.vy + v2.vy) >> 1;
    right.vz = (v1.vz + v2.vz) >> 1;

    g_plasmaWall.shipPos.vx = ship->ppivot.vx;
    g_plasmaWall.shipPos.vy = ship->ppivot.vy;
    g_plasmaWall.shipPos.vz = ship->ppivot.vz;
    g_plasmaWall.section = ship->nearTrkSect;

    g_plasmaWall.dir.vx = (v3.vx - v0.vx) >> 6;
    g_plasmaWall.dir.vy = (v3.vy - v0.vy) >> 6;
    g_plasmaWall.dir.vz = (v3.vz - v0.vz) >> 6;

    base.vx = left.vx;
    base.vy = left.vy;
    base.vz = left.vz;

    g_plasmaWall.corners[0].vx = left.vx;
    g_plasmaWall.corners[0].vy = left.vy;
    g_plasmaWall.corners[0].vz = left.vz;

    g_plasmaWall.corners[1].vx = right.vx;
    g_plasmaWall.corners[1].vy = right.vy;
    g_plasmaWall.corners[1].vz = right.vz;

    g_plasmaWall.corners[2].vx = left.vx;
    g_plasmaWall.corners[2].vy = left.vy - 1024;
    g_plasmaWall.corners[2].vz = left.vz;

    g_plasmaWall.corners[3].vx = right.vx;
    g_plasmaWall.corners[3].vy = right.vy - 1024;
    g_plasmaWall.corners[3].vz = right.vz;

    for (i = 0; i < MAX_PLAYERS; i++) {
        for (j = 0; j < 4; j++) {
            g_plasmaWall.vtx[i][j].v.cn[0] = 192;
            g_plasmaWall.vtx[i][j].v.cn[1] = 192;
            g_plasmaWall.vtx[i][j].v.cn[2] = 192;
            g_plasmaWall.vtx[i][j].v.cn[3] = 0;
        }
        g_plasmaWall.vtx[i][0].v.tc[0] = 0;
        g_plasmaWall.vtx[i][0].v.tc[1] = 0;
        g_plasmaWall.vtx[i][1].v.tc[0] = 0xFFF;
        g_plasmaWall.vtx[i][1].v.tc[1] = 0;
        g_plasmaWall.vtx[i][2].v.tc[0] = 0;
        g_plasmaWall.vtx[i][2].v.tc[1] = 0x7FF;
        g_plasmaWall.vtx[i][3].v.tc[0] = 0xFFF;
        g_plasmaWall.vtx[i][3].v.tc[1] = 0x7FF;
    }
}

void DrawPlasmaWall(VECTOR *camPos, s32 player) {
    s32 i;
    Face *face;
    VECTOR v0, v1, v2, v3;
    VECTOR left, right, unused, mid;

    if (g_plasmaWall.timer > 0) {
        UpdateInGamePlasma(0);
        if (g_plasmaWall.section != g_plasmaWall.weapon->section) {
            face = &((Face *)track->faces)[g_plasmaWall.section->faceStart];
            while (!(face->flags & 1)) {
                face++;
            }
            if (g_plasmaWall.weapon->sideFlag == 0) {
                face++;
            }

            v0.vx = ((VECTOR *)track->vertices)[face->vertex[0]].vx;
            v0.vy = ((VECTOR *)track->vertices)[face->vertex[0]].vy;
            v0.vz = ((VECTOR *)track->vertices)[face->vertex[0]].vz;

            v1.vx = ((VECTOR *)track->vertices)[face->vertex[1]].vx;
            v1.vy = ((VECTOR *)track->vertices)[face->vertex[1]].vy;
            v1.vz = ((VECTOR *)track->vertices)[face->vertex[1]].vz;

            v2.vx = ((VECTOR *)track->vertices)[face->vertex[2]].vx;
            v2.vy = ((VECTOR *)track->vertices)[face->vertex[2]].vy;
            v2.vz = ((VECTOR *)track->vertices)[face->vertex[2]].vz;

            v3.vx = ((VECTOR *)track->vertices)[face->vertex[3]].vx;
            v3.vy = ((VECTOR *)track->vertices)[face->vertex[3]].vy;
            v3.vz = ((VECTOR *)track->vertices)[face->vertex[3]].vz;

            left.vx = (v0.vx + v3.vx) >> 1;
            left.vy = (v0.vy + v3.vy) >> 1;
            left.vz = (v0.vz + v3.vz) >> 1;

            right.vx = (v1.vx + v2.vx) >> 1;
            right.vy = (v1.vy + v2.vy) >> 1;
            right.vz = (v1.vz + v2.vz) >> 1;

            mid.vx = (left.vx + right.vx) >> 1;
            mid.vy = (left.vy + right.vy) >> 1;
            mid.vz = (left.vz + right.vz) >> 1;

            if (frontendOptions_g.numPlayers == 1) {
                AddEboltParticles(particleSlots, 10, &mid);
            }
            g_plasmaWall.section = g_plasmaWall.section->nextSection.ptr;
        }

        gDPLoadTLUT_pal256(g_glistp++, g_inGamePlasmaPalette);
        gDPLoadTextureBlock(g_glistp++, g_inGamePlasmaTexture, G_IM_FMT_CI, G_IM_SIZ_8b, 64, 32, 0,
                            G_TX_WRAP, G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        gSPTexture(g_glistp++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);
        gDPSetTextureLUT(g_glistp++, G_TT_RGBA16);
        gDPLoadSync(g_glistp++);

        for (i = 0; i < 4; i++) {
            g_plasmaWall.vtx[player][i].v.ob[0] = (g_plasmaWall.corners[i].vx >> 1) + camPos->vx;
            g_plasmaWall.vtx[player][i].v.ob[1] = (g_plasmaWall.corners[i].vy >> 1) + camPos->vy;
            g_plasmaWall.vtx[player][i].v.ob[2] = (g_plasmaWall.corners[i].vz >> 1) + camPos->vz;
#ifdef VERSION_EU
            /* PAL: shorter plasma wall duration (50 Hz vs 60 Hz) */
            if (g_plasmaWall.timer > 184) {
#else
            if (g_plasmaWall.timer > 224) {
#endif
                g_plasmaWall.vtx[player][i].v.cn[3] += 8;
            } else if (g_plasmaWall.timer < 32) {
                g_plasmaWall.vtx[player][i].v.cn[3] -= 3;
            }
        }

        if (player == 0) {
            g_plasmaWall.timer--;
        }

        gSPVertex(g_glistp++, OS_K0_TO_PHYSICAL(g_plasmaWall.vtx[player]), 4, 0);
        gDPPipeSync(g_glistp++);
        gSP2Triangles(g_glistp++, 0, 1, 3, 0, 0, 3, 2, 0);
    } else if (g_superWeaponActive1 != 0) {
        g_superWeaponActive1 = 0;
    }
}

void ResetStartBoom(void) {
    s32 i, j;
    Object *obj;
    GT4 *poly;

    for (i = 0; i < startBoomCount; i++) {
        obj = startBoomObjs[i];
        poly = (GT4 *)obj->primitives;
        for (j = 0; j < START_BOOM_POLYS; j++) {
            poly->colour[0].r = 40;
            poly->colour[0].g = 40;
            poly->colour[0].b = 40;
            poly->colour[1].r = 40;
            poly->colour[1].g = 40;
            poly->colour[1].b = 40;
            poly->colour[2].r = 40;
            poly->colour[2].g = 40;
            poly->colour[2].b = 40;
            poly->colour[3].r = 40;
            poly->colour[3].g = 40;
            poly->colour[3].b = 40;
            poly++;
        }
    }
}

void ColourDroidLights(Object *droid) {
    s16 red, green, blue;
    s32 i;
    u32 j;
    s16 primitiveCount;
    Poly poly;

    droidLightPhase += 140;
    red = (rsin(droidLightPhase) >> 5) + 128;
    if (red > 255) {
        red = 255;
    }
    green = (rsin(droidLightPhase + 75) >> 5) + 128;
    blue = (rsin((droidLightPhase >> 1) + 32) >> 5) + 128;
    if (blue > 255) {
        blue = 255;
    }

    poly.prim = droid->primitives;
    primitiveCount = droid->primitiveCount;
    for (i = 0; i < primitiveCount; i++) {
        switch (poly.prim->type) {
            case TypeFT4:
                if (i == 1) {
                    for (j = 0; j < ((RenderGroup *)droid->bspTree)->count; j++) {
                        SetDisplayListPrimColour(((RenderGroup *)droid->bspTree)->entries[j].dl,
                                                 blue >> 1, blue >> 1, blue, 255, i);
                    }
                }
                poly.ft4++;
                break;
            case TypeGT3:
                if (i == 2) {
                    for (j = 0; j < ((RenderGroup *)droid->bspTree)->count; j++) {
                        SetDisplayListPrimColour(((RenderGroup *)droid->bspTree)->entries[j].dl,
                                                 blue >> 1, blue >> 1, blue, 255, i);
                    }
                }
                poly.gt3++;
                break;
            case TypeGT4:
                if (i == 17) {
                    ((Vtx *)droid->vtx)[poly.gt4->coords[0]].v.cn[0] = red;
                    ((Vtx *)droid->vtx)[poly.gt4->coords[1]].v.cn[0] = red;
                    ((Vtx *)droid->vtx)[poly.gt4->coords[3]].v.cn[0] = red;
                    ((Vtx *)droid->vtx)[poly.gt4->coords[2]].v.cn[0] = red;
                    ((Vtx *)droid->vtx)[poly.gt4->coords[0]].v.cn[1] = 40;
                    ((Vtx *)droid->vtx)[poly.gt4->coords[1]].v.cn[1] = 40;
                    ((Vtx *)droid->vtx)[poly.gt4->coords[3]].v.cn[1] = 40;
                    ((Vtx *)droid->vtx)[poly.gt4->coords[2]].v.cn[1] = 40;
                    ((Vtx *)droid->vtx)[poly.gt4->coords[0]].v.cn[2] = 40;
                    ((Vtx *)droid->vtx)[poly.gt4->coords[1]].v.cn[2] = 40;
                    ((Vtx *)droid->vtx)[poly.gt4->coords[3]].v.cn[2] = 40;
                    ((Vtx *)droid->vtx)[poly.gt4->coords[2]].v.cn[2] = 40;
                }
                poly.gt4 += 1;
                break;
        }
    }
}

void UpdateStartDroid(Object **shapes, ShipData *ships, CheckPoint *checkPoints, s16 textureBase,
                      ConfigData *game, s32 pause, s32 player) {
    VECTOR direction;
    s16 i, selected, distance;
    s32 angle;
    u16 ownSection;
    TrackSection *section, *previous;
    Object *shape;
    FT4 *poly;
    s16 primCount;
    s16 j, k;
    s32 maxDist;

    maxDist = 72;
    /* Cross-jumped arms stop cse2 from hoisting &frontendOptions_g across numPlayers */
    if (frontendOptions_g.numPlayers >= 2) {
        maxDist = 48;
        i = 0;
    } else {
        i = 0;
    }

    startDroidState[player].signType[0] = 10;
    startDroidState[player].signType[1] = 10;

    ownSection = raceLines[ships[frontendOptions_g.ownShips[player]].nearSecNo].distance;
    for (i = 0; i < NO_OF_CHECKPOINTS; i++) {
        if (checkPoints[i].section > 0) {
            distance = ownSection - raceLines[checkPoints[i].section].distance;
            if (distance < 0) {
                distance += numTrackSections;
            }
            if (distance > numTrackSections >> 1) {
                distance -= numTrackSections;
            }
            if (distance >= 0 ? distance <= maxDist : -distance <= maxDist) {
                break;
            }
        }
    }
    selected = i;

    for (i = (selected == NO_OF_CHECKPOINTS); i < 2; i++) {
        if (i == 0) {
            section = droidStartSections[selected];
            if (ships[frontendOptions_g.ownShips[player]].lapNo == 0) {
                startDroidState[player].signType[0] = 2;
            } else if (ships[frontendOptions_g.ownShips[player]].lapNo == classLaps[game->shipClass] &&
                       checkPoints[selected].section == startLinePos[frontendOptions_g.trackNo] &&
                       g_gameMode != 1 &&
                       (g_gameMode != 6 || frontendOptions_g.challengeMedal != 2)) {
                startDroidState[player].signType[0] = 4;
            } else if (frontendOptions_g.unk5B == 0 && g_multiplayerFlag != 0) {
                startDroidState[player].signType[0] = 5;
            } else {
                startDroidState[player].signType[0] = 6;
            }
        } else {
            section = droidStartSections[NO_OF_CHECKPOINTS];
            distance = ownSection - raceLines[section->secNo].distance;
            if (distance < 0) {
                distance += numTrackSections;
            }
            if (distance > numTrackSections >> 1) {
                distance -= numTrackSections;
            }
            if (distance >= 0 ? distance <= maxDist : -distance <= maxDist) {
                startDroidState[player].signType[1] = (droidSideTable[frontendOptions_g.trackNo] != 0) ? 3 : 0;
            }
        }

        /* Cross-jumped check keeps player ($s6) priority above player << 2 ($s7) */
        if (player == 0) {
            section = section->nextSection.ptr;
        } else {
            section = section->nextSection.ptr;
        }
        if ((i == 0 && startDroidState[player].signType[0] != 10) ||
            (i == 1 && startDroidState[player].signType[1] != 10)) {
            previous = section->prevSection.ptr;
            direction.vx = section->centre.vx - previous->centre.vx;
            direction.vy = section->centre.vy - previous->centre.vy;
            direction.vz = section->centre.vz - previous->centre.vz;
            angle = ratan2(direction.vx, direction.vz);
            angle = 4096 - angle;
            if (i == 0) {
                startDroidState[player].angle[0] = angle;
                startDroidState[player].pos[0].vx = section->centre.vx >> 1;
                startDroidState[player].pos[0].vy = (section->centre.vy - 2500) >> 1;
                startDroidState[player].pos[0].vz = section->centre.vz >> 1;
            } else {
                startDroidState[player].angle[1] = angle;
                startDroidState[player].pos[1].vx = section->centre.vx >> 1;
                startDroidState[player].pos[1].vy = (section->centre.vy - 2500) >> 1;
                startDroidState[player].pos[1].vz = section->centre.vz >> 1;
            }
        }
    }

    if (player == 0 && pause == -1) {
        droidBlinkTick++;
    }

    for (i = 0; i < 2; i++) {
        shape = NULL;
        if (i == 0) {
            if (startDroidState[player].signType[0] != 10) {
                shape = shapes[startDroidState[player].signType[0]];
            }
        } else {
            if (startDroidState[player].signType[1] != 10) {
                shape = shapes[startDroidState[player].signType[1]];
            }
        }
        if (shape != NULL) {
            poly = (FT4 *)shape->primitives;
            primCount = shape->primitiveCount;
            for (j = 0; j < primCount; j++) {
                if (poly->type == TypeFT4) {
                    if (droidBlinkTick % 20 < 10) {
                        for (k = 0; k < ((RenderGroup *)shape->bspTree)->count; k++) {
                            SetDisplayListPrimColour(((RenderGroup *)shape->bspTree)->entries[k].dl,
                                                     255, 255, 255, 255, 1);
                        }
                    } else {
                        for (k = 0; k < ((RenderGroup *)shape->bspTree)->count; k++) {
                            SetDisplayListPrimColour(((RenderGroup *)shape->bspTree)->entries[k].dl,
                                                     128, 128, 128, 255, 1);
                        }
                    }
                    poly++;
                }
            }
        }
    }
}

void LoadStartDroid(Object **objects, char *name, Skeleton *parent) {
    char buf[256];
    s16 texIndex;
    Object *obj;
    RenderList *renderList;
    s32 i, j;
    s32 count = 0;
    s16 primCount;
    Poly poly;

    if (DMemCmp(name, "sroid", 5) == 0) {
        renderList = &g_sroidRenderList;
        texIndex = TextureTableCount;
        g_startupImage->current++;
        g_startupData->current = 0;
        LoadCmpFiles(WADLoadCompressedTextureSequence(g_startupData));
    } else {
        renderList = &g_lightRenderList;
        texIndex = TextureTableCount;
        g_startupImage->current++;
        g_startupData->current = 4;
        LoadCmpFiles(WADLoadCompressedTextureSequence(g_startupData));

        guSprite2DInit(&startLightSprites[0], (void *)OS_K0_TO_PHYSICAL(g_textureTable[texIndex + 11]->texels),
                       (void *)OS_K0_TO_PHYSICAL(g_textureTable[texIndex + 11]->palette),
                       g_textureTable[texIndex + 11]->width, g_textureTable[texIndex + 11]->width,
                       g_textureTable[texIndex + 11]->height, G_IM_FMT_CI, G_IM_SIZ_4b, 0, 0);
        guSprite2DInit(&startLightSprites[1], (void *)OS_K0_TO_PHYSICAL(g_textureTable[texIndex + 1]->texels),
                       (void *)OS_K0_TO_PHYSICAL(g_textureTable[texIndex + 1]->palette),
                       g_textureTable[texIndex + 1]->width, g_textureTable[texIndex + 1]->width,
                       g_textureTable[texIndex + 1]->height, G_IM_FMT_CI, G_IM_SIZ_4b, 0, 0);
        guSprite2DInit(&startLightSprites[2], (void *)OS_K0_TO_PHYSICAL(g_textureTable[texIndex]->texels),
                       (void *)OS_K0_TO_PHYSICAL(g_textureTable[texIndex]->palette),
                       g_textureTable[texIndex]->width, g_textureTable[texIndex]->width,
                       g_textureTable[texIndex]->height, G_IM_FMT_CI, G_IM_SIZ_4b, 0, 0);
        guSprite2DInit(&startLightSprites[3], (void *)OS_K0_TO_PHYSICAL(g_textureTable[texIndex + 10]->texels),
                       (void *)OS_K0_TO_PHYSICAL(g_textureTable[texIndex + 10]->palette),
                       g_textureTable[texIndex + 10]->width, g_textureTable[texIndex + 10]->width,
                       g_textureTable[texIndex + 10]->height, G_IM_FMT_CI, G_IM_SIZ_4b, 0, 0);
    }

    InitRenderList(renderList, texIndex, TextureTableCount - texIndex);
    g_startupImage->current++;
    obj = WADLoadPrm(g_startupData, renderList, g_startupImage);
    *objects = obj;
    while (obj != NULL) {
        obj->skeleton->super = parent;
        objects[count++] = obj;
        obj = obj->next;
    }

    if (DMemCmp(name, "sroid", 5) == 0) {
        objects[8] = NewInstance(objects[7]);
        objects[9] = NewInstance(objects[1]);
        objects[2]->skeleton->super = objects[1]->skeleton;
        objects[6]->skeleton->super = objects[1]->skeleton;
        objects[5]->skeleton->super = objects[1]->skeleton;
        objects[4]->skeleton->super = objects[1]->skeleton;
        objects[0]->skeleton->super = objects[9]->skeleton;
        objects[3]->skeleton->super = objects[9]->skeleton;
    } else {
        objects[2]->skeleton->super = objects[3]->skeleton;
        objects[4]->skeleton->super = objects[3]->skeleton;
        objects[0]->skeleton->super = objects[3]->skeleton;
        objects[1]->skeleton->super = objects[3]->skeleton;
        objects[5]->skeleton->super = objects[3]->skeleton;

        for (i = 0; i < LIGHT_GROUP_OBJECTS; i++) {
            lightGroups[1][i] = NewInstance(lightGroups[0][i]);
            lightGroups[2][i] = NewInstance(lightGroups[0][i]);
        }

        for (i = 1; i < MAX_LIGHT_GROUPS; i++) {
            lightGroups[i][2]->skeleton->super = lightGroups[i][3]->skeleton;
            lightGroups[i][4]->skeleton->super = lightGroups[i][3]->skeleton;
            lightGroups[i][0]->skeleton->super = lightGroups[i][3]->skeleton;
            lightGroups[i][1]->skeleton->super = lightGroups[i][3]->skeleton;
            lightGroups[i][5]->skeleton->super = lightGroups[i][3]->skeleton;
        }

        obj = lightGroups[0][4];
        poly.prim = obj->primitives;
        primCount = obj->primitiveCount;
        for (j = 0; j < primCount; j++) {
            if (poly.prim->type == TypeGT4) {
                lightTex0 = poly.gt4->texture + renderList->textureStart;
                ((Vtx *)obj->vtx)[poly.gt4->coords[0]].v.cn[2] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[1]].v.cn[2] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[3]].v.cn[2] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[2]].v.cn[2] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[0]].v.cn[1] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[1]].v.cn[1] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[3]].v.cn[1] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[2]].v.cn[1] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[0]].v.cn[0] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[1]].v.cn[0] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[3]].v.cn[0] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[2]].v.cn[0] = 192;
                poly.gt4++;
            }
        }

        obj = lightGroups[0][0];
        poly.prim = obj->primitives;
        primCount = obj->primitiveCount;
        for (j = 0; j < primCount; j++) {
            if (poly.prim->type == TypeGT4) {
                lightTex1 = poly.gt4->texture + renderList->textureStart;
                ((Vtx *)obj->vtx)[poly.gt4->coords[0]].v.cn[2] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[1]].v.cn[2] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[3]].v.cn[2] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[2]].v.cn[2] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[0]].v.cn[1] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[1]].v.cn[1] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[3]].v.cn[1] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[2]].v.cn[1] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[0]].v.cn[0] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[1]].v.cn[0] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[3]].v.cn[0] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[2]].v.cn[0] = 192;
                poly.gt4++;
            }
        }

        obj = lightGroups[0][1];
        poly.prim = obj->primitives;
        primCount = obj->primitiveCount;
        for (j = 0; j < primCount; j++) {
            if (poly.prim->type == TypeGT4) {
                lightTex2 = poly.gt4->texture + renderList->textureStart;
                ((Vtx *)obj->vtx)[poly.gt4->coords[0]].v.cn[2] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[1]].v.cn[2] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[3]].v.cn[2] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[2]].v.cn[2] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[0]].v.cn[1] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[1]].v.cn[1] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[3]].v.cn[1] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[2]].v.cn[1] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[0]].v.cn[0] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[1]].v.cn[0] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[3]].v.cn[0] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[2]].v.cn[0] = 192;
                poly.gt4++;
            }
        }

        obj = lightGroups[0][5];
        poly.prim = obj->primitives;
        primCount = obj->primitiveCount;
        for (j = 0; j < primCount; j++) {
            if (poly.prim->type == TypeGT4) {
                lightTex3 = poly.gt4->texture + renderList->textureStart;
                ((Vtx *)obj->vtx)[poly.gt4->coords[0]].v.cn[2] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[1]].v.cn[2] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[3]].v.cn[2] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[2]].v.cn[2] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[0]].v.cn[1] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[1]].v.cn[1] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[3]].v.cn[1] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[2]].v.cn[1] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[0]].v.cn[0] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[1]].v.cn[0] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[3]].v.cn[0] = 192;
                ((Vtx *)obj->vtx)[poly.gt4->coords[2]].v.cn[0] = 192;
                poly.gt4 += 1;
            }
        }
    }
}

void InitDroidThings(Object **objects, char *name, Skeleton *parent, CheckPoint *points, ConfigData *options) {
    s16 table[7] = { 6, 6, 6, 6, 15, 6, 358 };
    s16 spacing;
    s16 step;
    TrackSection *section;
    s32 i, j;

    spacing = 9;
    step = spacing;
    if (DMemCmp(name, "light", 5) == 0) {
        lightUpdate = 0;
        for (i = 0; i < MAX_LIGHT_GROUPS; i++) {
            lightState[i] = 4;
        }
        section = track->sections;
        for (i = 0; i < track->sectionCount; i++) {
            if (section->secNo == startLinePos[options->actTrackNum] - 4) {
                startDroidSection = section;
                break;
            }
            section = section->nextSection.ptr;
        }
        if (frontendOptions_g.numPlayers == 1) {
            lightCount = 2;
        } else {
            lightCount = 0;
        }
        for (i = 0; i < lightCount; i++) {
            section = startDroidSection;
            for (j = 0; j < spacing; j++) {
                section = section->prevSection.ptr;
            }
            lightSections[i] = section;
            spacing += step;
        }
    } else {
        for (i = 0; i < NO_OF_CHECKPOINTS; i++) {
            section = track->sections;
            for (j = 0; j < track->sectionCount; j++) {
                if (section->secNo == points[i].section) {
                    droidStartSections[i] = section;
                    break;
                }
                section = section->nextSection.ptr;
            }
        }
        section = track->sections;
        droidStartSections[NO_OF_CHECKPOINTS] = droidStartSections[0];
        for (j = 0; j < track->sectionCount; j++) {
            if (section->secNo == table[options->trackNum]) {
                droidStartSections[NO_OF_CHECKPOINTS] = section;
                break;
            }
            section = section->nextSection.ptr;
        }
    }
    DMemCmp(objects[0]->name, "light", 5);
}

void ResetLightStart(void) {
    s16 i;
    TrackSection *section;
    TrackSection *prev;
    Object *light;
    VECTOR delta;
    s32 angle;
    Skeleton *skeleton;

    lightUpdate = 0;
    for (i = 0; i < lightCount; i++) {
        lightState[i] = 4;
    }
    for (i = 0; i < lightCount; i++) {
        light = lightGroups[i][LIGHT_GROUP_SKELETON];
        section = lightSections[i];
        prev = section->prevSection.ptr;
        delta.vx = section->centre.vx - prev->centre.vx;
        delta.vy = section->centre.vy - prev->centre.vy;
        delta.vz = section->centre.vz - prev->centre.vz;
        angle = ratan2(delta.vx, delta.vz);
        skeleton = light->skeleton;
        angle = 4096 - angle;
        SetSkeletonDirectionHPR(skeleton, angle, 0, 0);
        SetSkeletonPosition(light->skeleton, section->centre.vx >> 1, (section->centre.vy - 2000) >> 1,
                            section->centre.vz >> 1);
    }
}

/* Copy an animated start-light texture frame (4-bit or 8-bit CI) into the display texture. */
#define COPY_LIGHT_TEXTURE(dstTexIdx, srcStart, srcCount, pal8Size)                  \
    do {                                                                             \
        dstTex = g_textureTable[dstTexIdx];                                          \
        srcTex = g_textureTable[(srcStart) + lightAnimFrame];                        \
        if (dstTex->type == 0) {                                                     \
            memcpy(dstTex->texels, srcTex->texels, (dstTex->width * dstTex->height) >> 1); \
            memcpy(dstTex->palette, srcTex->palette, 32);                            \
        } else {                                                                     \
            memcpy(dstTex->texels, srcTex->texels, dstTex->width * dstTex->height);  \
            memcpy(dstTex->palette, srcTex->palette, (pal8Size));                    \
        }                                                                            \
        lightAnimFrame++;                                                            \
        if (lightAnimFrame >= (srcCount)) {                                          \
            lightAnimFrame = 0;                                                      \
        }                                                                            \
    } while (0)

void UpdateLightStart(Object **objects, s32 unused1, s32 unused2, s32 unused3, s32 unused4, s32 mode) {
    s16 i;
    s16 j;
    s16 red, green, blue;
    Object *light;
    s16 primCount;
    Poly poly;
    N64Texture *dstTex, *srcTex;

    if (mode != -1) {
        return;
    }

    lightUpdateTick++;
    if (frontendOptions_g.numPlayers != 1 && (startCountdown > 0 || lightGoTimer > 0)) {
        /*
         * Original bug: `i` is uninitialized in the multiplayer branch (it is only
         * initialized by the single-player loop below). Preserved faithfully for byte match.
         */
#ifdef VERSION_EU
        if (startCountdown >= 84) {
#else
        if (startCountdown >= 67) {
#endif
            lightState[i] = 4;
            if (i == 0) {
                COPY_LIGHT_TEXTURE(lightTex0, effectTex0Start, effectTex0Count, 712);
            }
#ifdef VERSION_EU
        } else if (startCountdown >= 42 && startCountdown <= 83) {
#else
        } else if (startCountdown >= 34 && startCountdown <= 66) {
#endif
            lightState[i] = 0;
            if (i == 0) {
                COPY_LIGHT_TEXTURE(lightTex1, effectTex1Start, effectTex1Count, 512);
            }
#ifdef VERSION_EU
        } else if (startCountdown >= 3 && startCountdown <= 41) {
#else
        } else if (startCountdown >= 3 && startCountdown <= 33) {
#endif
            lightState[i] = 1;
            if (i == 0) {
                COPY_LIGHT_TEXTURE(lightTex2, effectTex2Start, effectTex2Count, 512);
            }
        } else if (startCountdown >= 1 && startCountdown <= 2 && lightGoTimer == 0) {
            lightGoTimer = 60;
        } else if ((startCountdown >= 1 && startCountdown <= 2) || lightGoTimer != 0) {
            lightState[i] = 5;
            lightGoTimer--;
            lightGoFlag = 1;
            if (i == 0) {
                COPY_LIGHT_TEXTURE(lightTex3, effectTex3Start, effectTex3Count, 512);
            }
        }
        return;
    }

    for (i = 0; i < lightCount; i++) {
#ifdef VERSION_EU
        if (startCountdown >= 84) {
#else
        if (startCountdown >= 67) {
#endif
            lightState[i] = 4;
            if (i == 0) {
                COPY_LIGHT_TEXTURE(lightTex0, effectTex0Start, effectTex0Count, 712);
            }
#ifdef VERSION_EU
        } else if (startCountdown >= 42 && startCountdown <= 83) {
#else
        } else if (startCountdown >= 34 && startCountdown <= 66) {
#endif
            lightState[i] = 0;
            if (i == 0) {
                COPY_LIGHT_TEXTURE(lightTex1, effectTex1Start, effectTex1Count, 512);
            }
#ifdef VERSION_EU
        } else if (startCountdown >= 1 && startCountdown <= 41) {
#else
        } else if (startCountdown >= 1 && startCountdown <= 33) {
#endif
            lightState[i] = 1;
            if (i == 0) {
                COPY_LIGHT_TEXTURE(lightTex2, effectTex2Start, effectTex2Count, 512);
            }
        } else if (startCountdown == 0) {
            lightState[i] = 5;
            lightGoFlag = 1;
            if (i == 0) {
                COPY_LIGHT_TEXTURE(lightTex3, effectTex3Start, effectTex3Count, 512);
            }
        }

        lightBlinkPhase += 140;
        red = (rsin(lightBlinkPhase) >> 5) + 128;
        if (red > 255) {
            red = 255;
        }
        green = (rsin(lightBlinkPhase + 75) >> 5) + 128;
        if (green > 255) {
            green = 255;
        }
        blue = (rsin((lightBlinkPhase >> 1) + 32) >> 5) + 128;
        if (blue > 255) {
            blue = 255;
        }

        light = lightGroups[i][LIGHT_GROUP_SKELETON];
        if (light != NULL) {
            primCount = light->primitiveCount;
            poly.prim = light->primitives;
            for (j = 0; j < primCount; j++) {
                switch (poly.prim->type) {
                    case TypeGT3:
                        poly.gt3++;
                        break;
                    case TypeGT4:
                        if (j == 15 || j == 18) {
                            ((Vtx *)light->vtx)[poly.gt4->coords[0]].v.cn[0] = red;
                            ((Vtx *)light->vtx)[poly.gt4->coords[1]].v.cn[0] = red;
                            ((Vtx *)light->vtx)[poly.gt4->coords[3]].v.cn[0] = red;
                            ((Vtx *)light->vtx)[poly.gt4->coords[2]].v.cn[0] = red;
                            ((Vtx *)light->vtx)[poly.gt4->coords[0]].v.cn[1] = 40;
                            ((Vtx *)light->vtx)[poly.gt4->coords[1]].v.cn[1] = 40;
                            ((Vtx *)light->vtx)[poly.gt4->coords[3]].v.cn[1] = 40;
                            ((Vtx *)light->vtx)[poly.gt4->coords[2]].v.cn[1] = 40;
                            ((Vtx *)light->vtx)[poly.gt4->coords[0]].v.cn[2] = 40;
                            ((Vtx *)light->vtx)[poly.gt4->coords[1]].v.cn[2] = 40;
                            ((Vtx *)light->vtx)[poly.gt4->coords[3]].v.cn[2] = 40;
                            ((Vtx *)light->vtx)[poly.gt4->coords[2]].v.cn[2] = 40;
                        }
                        poly.gt4 += 1;
                        break;
                }
            }
        }
    }
}

void LoadShadowTextures(void) {
    shadowTextureStart = TextureTableCount;
    LoadCmpFiles(LoadCompressedTextureSequence((u32)_shadowTexturesFile));
    shadow2TextureStart = TextureTableCount;
    LoadCmpFiles(LoadCompressedTextureSequence((u32)_shadow2TexturesFile));
}

void DrawShadow(VECTOR *camPos, Object **shipShapes, ShipData *ships, TrackCamera *camera, s32 player) {
    VECTOR a, b, c, d, facePoint;
    s32 screen[4];
    s32 x[26];
    s32 y[26];
    s32 depth[4];
    SVECTOR vertex[4];
    s32 i;
    s32 alpha;
    Face *face;
    s32 shadowTex;
    s32 draw;
    s32 dx, dy, dz;

    gDPSetCombineMode(g_glistp++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);
    gDPSetPrimColor(g_glistp++, 0, 0, 127, 127, 127, 96);
    gDPPipeSync(g_glistp++);

    shadowTex = shadowTextureStart;
    for (i = 0; i < NO_OF_SHIPS; i++) {
        if (i % 3 == 0) {
            gSPDisplayList(g_glistp++, g_textureTable[shadowTex++]->dl);
        }

        if ((g_gameMode == 3 &&
             (i == ownShip || i == ownShip2 || i == ownShip3 || i == ownShip4 || frontendOptions_g.unk14 != 0)) ||
            (g_gameMode == 1 && i == ownShip) ||
            (g_gameMode == 6 && frontendOptions_g.challengeMedal == 2 && i == ownShip) ||
            (g_gameMode != 1 && g_gameMode != 3)) {
            switch (player) {
                case 0:
                    draw = 1;
                    if (i == ownShip) {
                        if (ships[i].attr & 4) {
                            draw = 0;
                        }
                    }
                    break;
                case 1:
                    draw = 1;
                    if (i == ownShip2) {
                        if (ships[i].attr & 4) {
                            draw = 0;
                        }
                    }
                    break;
                case 2:
                    draw = 1;
                    if (i == ownShip3) {
                        if (ships[i].attr & 4) {
                            draw = 0;
                        }
                    }
                    break;
                case 3:
                    draw = 1;
                    if (i == ownShip4) {
                        if (ships[i].attr & 4) {
                            draw = 0;
                        }
                    }
                    break;
            }

            dx = ships[i].ppivot.vx + (camPos->vx << 1);
            dy = ships[i].ppivot.vy + (camPos->vy << 1);
            dz = ships[i].ppivot.vz + (camPos->vz << 1);
            if (dx * dx + dy * dy + dz * dz > 0xFFFFFF) {
                continue;
            }
            if (ships[i].combatAttr & 0x8000) {
                continue;
            }
            if (draw != 1) {
                continue;
            }
            if (!ships[i].visible[player]) {
                continue;
            }

            face = &((Face *)track->faces)[ships[i].nearTrkSect->faceStart];
            while (!(face->flags & 1)) {
                face++;
            }

            a.vx = (ships[i].ppivot.vx + ((ships[i].unitVecNose.vx * 3) >> 5)) - (ships[i].unitVecWing.vx >> 4);
            a.vy = (ships[i].ppivot.vy + ((ships[i].unitVecNose.vy * 3) >> 5)) - (ships[i].unitVecWing.vy >> 4);
            a.vz = (ships[i].ppivot.vz + ((ships[i].unitVecNose.vz * 3) >> 5)) - (ships[i].unitVecWing.vz >> 4);

            b.vx = (ships[i].ppivot.vx + ((ships[i].unitVecNose.vx * 3) >> 5)) + (ships[i].unitVecWing.vx >> 4);
            b.vy = (ships[i].ppivot.vy + ((ships[i].unitVecNose.vy * 3) >> 5)) + (ships[i].unitVecWing.vy >> 4);
            b.vz = (ships[i].ppivot.vz + ((ships[i].unitVecNose.vz * 3) >> 5)) + (ships[i].unitVecWing.vz >> 4);

            c.vx = (ships[i].ppivot.vx - ((ships[i].unitVecNose.vx * 3) >> 5)) - (ships[i].unitVecWing.vx >> 4);
            c.vy = (ships[i].ppivot.vy - ((ships[i].unitVecNose.vy * 3) >> 5)) - (ships[i].unitVecWing.vy >> 4);
            c.vz = (ships[i].ppivot.vz - ((ships[i].unitVecNose.vz * 3) >> 5)) - (ships[i].unitVecWing.vz >> 4);

            d.vx = (ships[i].ppivot.vx - ((ships[i].unitVecNose.vx * 3) >> 5)) + (ships[i].unitVecWing.vx >> 4);
            d.vy = (ships[i].ppivot.vy - ((ships[i].unitVecNose.vy * 3) >> 5)) + (ships[i].unitVecWing.vy >> 4);
            d.vz = (ships[i].ppivot.vz - ((ships[i].unitVecNose.vz * 3) >> 5)) + (ships[i].unitVecWing.vz >> 4);

            facePoint.vx = ((VECTOR *)track->vertices)[face->vertex[0]].vx;
            facePoint.vy = ((VECTOR *)track->vertices)[face->vertex[0]].vy;
            facePoint.vz = ((VECTOR *)track->vertices)[face->vertex[0]].vz;

            alpha = PlaneLineCollision(facePoint, a, face->normal);
            a.vx -= (alpha * face->normal.vx) >> 12;
            a.vy -= (alpha * face->normal.vy) >> 12;
            a.vz -= (alpha * face->normal.vz) >> 12;

            alpha = PlaneLineCollision(facePoint, b, face->normal);
            b.vx -= (alpha * face->normal.vx) >> 12;
            b.vy -= (alpha * face->normal.vy) >> 12;
            b.vz -= (alpha * face->normal.vz) >> 12;

            alpha = PlaneLineCollision(facePoint, c, face->normal);
            c.vx -= (alpha * face->normal.vx) >> 12;
            c.vy -= (alpha * face->normal.vy) >> 12;
            c.vz -= (alpha * face->normal.vz) >> 12;

            alpha = PlaneLineCollision(facePoint, d, face->normal);
            d.vx -= (alpha * face->normal.vx) >> 12;
            d.vy -= (alpha * face->normal.vy) >> 12;
            d.vz -= (alpha * face->normal.vz) >> 12;

            shadowVtx[player][i][0].v.ob[0] = (a.vx >> 1) + camPos->vx;
            shadowVtx[player][i][0].v.ob[1] = (a.vy >> 1) + camPos->vy - 20;
            shadowVtx[player][i][0].v.ob[2] = (a.vz >> 1) + camPos->vz;

            shadowVtx[player][i][1].v.ob[0] = (b.vx >> 1) + camPos->vx;
            shadowVtx[player][i][1].v.ob[1] = (b.vy >> 1) + camPos->vy - 20;
            shadowVtx[player][i][1].v.ob[2] = (b.vz >> 1) + camPos->vz;

            shadowVtx[player][i][2].v.ob[0] = (c.vx >> 1) + camPos->vx;
            shadowVtx[player][i][2].v.ob[1] = (c.vy >> 1) + camPos->vy - 20;
            shadowVtx[player][i][2].v.ob[2] = (c.vz >> 1) + camPos->vz;

            shadowVtx[player][i][3].v.ob[0] = (d.vx >> 1) + camPos->vx;
            shadowVtx[player][i][3].v.ob[1] = (d.vy >> 1) + camPos->vy - 20;
            shadowVtx[player][i][3].v.ob[2] = (d.vz >> 1) + camPos->vz;

            shadowVtx[player][i][0].v.tc[0] = 0;
            shadowVtx[player][i][0].v.tc[1] = 0;
            shadowVtx[player][i][1].v.tc[0] = 0x7C0;
            shadowVtx[player][i][1].v.tc[1] = 0;
            shadowVtx[player][i][2].v.tc[0] = 0;
            shadowVtx[player][i][2].v.tc[1] = 0xFC0;
            shadowVtx[player][i][3].v.tc[0] = 0x7C0;
            shadowVtx[player][i][3].v.tc[1] = 0xFC0;

            gSPVertex(g_glistp++, OS_K0_TO_PHYSICAL(shadowVtx[player][i]), 4, 0);
            gSP2Triangles(g_glistp++, 0, 1, 3, 0, 0, 3, 2, 0);
        }
    }
}

void InitWeaponIcons(uSprite *weaponIcons, N64Texture **weaponTextures) {
    s16 texIndex;
    s32 i;

    texIndex = TextureTableCount;
    LoadCmpFiles(WADLoadCompressedTextureSequence(g_startupImage));

    weaponTextures[6] = g_textureTable[texIndex++];
    weaponTextures[4] = g_textureTable[texIndex++];
    weaponTextures[5] = g_textureTable[texIndex++];
    weaponTextures[3] = g_textureTable[texIndex++];
    weaponTextures[2] = g_textureTable[texIndex++];
    weaponTextures[14] = g_textureTable[texIndex++];
    weaponTextures[1] = g_textureTable[texIndex++];
    weaponTextures[12] = g_textureTable[texIndex++];
    texIndex++;
    weaponTextures[11] = g_textureTable[texIndex++];
    weaponTextures[0] = g_textureTable[texIndex++];
    weaponTextures[7] = g_textureTable[texIndex++];
    weaponTextures[13] = g_textureTable[texIndex++];
    weaponTextures[8] = g_textureTable[texIndex++];

    texIndex = TextureTableCount;
    g_startupData->current = 2;
    LoadCmpFiles(WADLoadCompressedTextureSequence(g_startupData));

    weaponTexturesMP[6] = g_textureTable[texIndex++];
    weaponTexturesMP[4] = g_textureTable[texIndex++];
    weaponTexturesMP[15] = g_textureTable[texIndex++];
    weaponTexturesMP[5] = g_textureTable[texIndex++];
    weaponTexturesMP[2] = g_textureTable[texIndex++];
    weaponTexturesMP[14] = g_textureTable[texIndex++];
    weaponTexturesMP[1] = g_textureTable[texIndex++];
    weaponTexturesMP[16] = g_textureTable[texIndex++];
    weaponTexturesMP[11] = g_textureTable[texIndex++];
    weaponTexturesMP[0] = g_textureTable[texIndex++];
    weaponTexturesMP[7] = g_textureTable[texIndex++];
    weaponTexturesMP[17] = g_textureTable[texIndex++];
    weaponTexturesMP[18] = g_textureTable[texIndex++];
    weaponTexturesMP[13] = g_textureTable[texIndex++];
    weaponTexturesMP[8] = g_textureTable[texIndex++];

    weaponTextures[15] = g_textureTable[texIndex++];
    weaponTextures[14] = g_textureTable[texIndex++];
    weaponTextures[16] = g_textureTable[texIndex++];
    weaponTextures[17] = g_textureTable[texIndex++];
    weaponTextures[18] = g_textureTable[texIndex++];

    for (i = 0; i < NO_OF_WEAPON_ICONS; i++) {
        if (weaponTextures[i] != NULL) {
            guSprite2DInit(&weaponIcons[i], (void *)OS_K0_TO_PHYSICAL(weaponTextures[i]->texels),
                           (void *)OS_K0_TO_PHYSICAL(weaponTextures[i]->palette), weaponTextures[i]->width,
                           weaponTextures[i]->width, weaponTextures[i]->height, G_IM_FMT_CI, G_IM_SIZ_4b, 0, 0);
        } else {
            guSprite2DInit(&weaponIcons[i], NULL, NULL, 0, 0, 0, G_IM_FMT_CI, G_IM_SIZ_4b, 0, 0);
        }
    }

    for (i = 0; i < NO_OF_WEAPON_ICONS; i++) {
        if (weaponTexturesMP[i] != NULL) {
            guSprite2DInit(&weaponIconsMP[i], (void *)OS_K0_TO_PHYSICAL(weaponTexturesMP[i]->texels),
                           (void *)OS_K0_TO_PHYSICAL(weaponTexturesMP[i]->palette), weaponTexturesMP[i]->width,
                           weaponTexturesMP[i]->width, weaponTexturesMP[i]->height, G_IM_FMT_CI, G_IM_SIZ_4b, 0, 0);
        } else {
            guSprite2DInit(&weaponIconsMP[i], NULL, NULL, 0, 0, 0, G_IM_FMT_CI, G_IM_SIZ_4b, 0, 0);
        }
    }

    guSprite2DInit(&weaponBackingSprite, (void *)OS_K0_TO_PHYSICAL(weaponTextures[3]->texels),
                   (void *)OS_K0_TO_PHYSICAL(weaponTextures[3]->palette), weaponTextures[3]->width,
                   weaponTextures[3]->width, weaponTextures[3]->height, G_IM_FMT_CI, G_IM_SIZ_4b, 0, 0);
}

/* Returns TRUE for weapons eligible for super-combo icon blinking in DrawWeaponIcons. */
s32 IsSuperComboWeapon(s32 weaponType) {
    switch (weaponType) {
        case 1:
        case 2:
        case 3:
        case 5:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
            return TRUE;
        default:
            return FALSE;
    }
}

Gfx *DrawWeaponIcons(uSprite *weaponIcons, void *unused, ShipData *playerShip, s32 viewportMode, Gfx *glistp) {
    s32 x, y;
    s32 weaponType;
    uSprite *icons;
    uSprite *backing;
    s32 backingAlpha;
    s32 iconAlpha;

    if (viewportMode == 0) {
        icons = weaponIcons;
    } else {
        icons = weaponIconsMP;
    }
    backing = &weaponBackingSprite;
    backingAlpha = (viewportMode == 0) ? 65 : 130;
    iconAlpha = (viewportMode == 0) ? 145 : 200;

    switch (viewportMode) {
        case 0:
            x = 542;
#ifdef VERSION_EU
            y = 40;
#else
            y = 80;
#endif
            break;
        case 3:
            x = 574;
#ifdef VERSION_EU
            y = 4;
#else
            y = 68;
#endif
            break;
        case 4:
            x = 574;
#ifdef VERSION_EU
            y = 480;
#else
            y = 484;
#endif
            break;
        case 1:
        case 5:
            x = 300;
#ifdef VERSION_EU
            y = 4;
#else
            y = 68;
#endif
            break;
        case 2:
        case 6:
            x = 900;
#ifdef VERSION_EU
            y = 4;
#else
            y = 68;
#endif
            break;
        case 7:
            x = 300;
#ifdef VERSION_EU
            y = 480;
#else
            y = 484;
#endif
            break;
        case 8:
            x = 900;
#ifdef VERSION_EU
            y = 480;
#else
            y = 484;
#endif
            break;
    }

    if (playerShip->weaponType != 0) {
        goSpriteTimer = -1;
    }

    if (viewportMode != 0) {
        if (startCountdown > 0 || goSpriteTimer > 0) {
            y += 12;
            gDPPipeSync(glistp++);
            gDPSetPrimColor(glistp++, 0, 0, 255, 255, 255, iconAlpha);
            gDPSetCombineMode(glistp++, G_CC_DECALRGB_PRIMA, G_CC_DECALRGB_PRIMA);

#ifdef VERSION_EU
            if (startCountdown > 83 && startCountdown < 100) {
#else
            if (startCountdown > 66 && startCountdown < 100) {
#endif
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&startLightSprites[3]));
                gSPSprite2DScaleFlip(glistp++, 2 << 10, 2 << 10, 0, 0);
                gSPSprite2DDraw(glistp++, x, y);
#ifdef VERSION_EU
            } else if (startCountdown > 41 && startCountdown <= 83) {
#else
            } else if (startCountdown > 33 && startCountdown <= 66) {
#endif
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&startLightSprites[2]));
                gSPSprite2DScaleFlip(glistp++, 2 << 10, 2 << 10, 0, 0);
                gSPSprite2DDraw(glistp++, x, y);
#ifdef VERSION_EU
            } else if (startCountdown > 2 && startCountdown <= 41) {
#else
            } else if (startCountdown > 2 && startCountdown <= 33) {
#endif
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&startLightSprites[1]));
                gSPSprite2DScaleFlip(glistp++, 2 << 10, 2 << 10, 0, 0);
                gSPSprite2DDraw(glistp++, x, y);
            } else if (startCountdown > 0 && startCountdown <= 2) {
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&startLightSprites[0]));
                gSPSprite2DScaleFlip(glistp++, 2 << 10, 2 << 10, 0, 0);
                gSPSprite2DDraw(glistp++, x, y);
                goSpriteTimer = frontendOptions_g.numPlayers * 60;
            } else if (goSpriteTimer > 0) {
                gDPPipeSync(glistp++);
                gDPSetPrimColor(glistp++, 0, 0, 255, 255, 255,
                                (goSpriteTimer * iconAlpha) / (frontendOptions_g.numPlayers * 60));
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&startLightSprites[0]));
                gSPSprite2DScaleFlip(glistp++, 2 << 10, 2 << 10, 0, 0);
                gSPSprite2DDraw(glistp++, x, y);
            }

            if (pauseData.selection < 0) {
                goSpriteTimer--;
            }
            return glistp;
        }
    } else {
        gDPPipeSync(glistp++);
        gDPSetPrimColor(glistp++, 0, 0, 255, 255, 255, backingAlpha);
        gDPSetCombineMode(glistp++, G_CC_DECALRGB_PRIMA, G_CC_DECALRGB_PRIMA);
        gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(backing));
        gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
        gSPSprite2DDraw(glistp++, x, y);
        gDPPipeSync(glistp++);
        gDPSetPrimColor(glistp++, 0, 0, 255, 255, 255, iconAlpha);
        gDPSetCombineMode(glistp++, G_CC_DECALRGB_PRIMA, G_CC_DECALRGB_PRIMA);
    }

    weaponType = playerShip->weaponType;
    if (weaponType != 0) {
        weaponType--;
        if (icons[weaponType].s.SourceImagePointer != NULL) {
            if (IsSuperComboWeapon(weaponType + 1) && (g_superComboUnlocked || cheatSuperCombo)) {
                if (g_gameMode == 0 || g_gameMode == 3) {
                    if (g_superComboOption != 0) {
                        gDPPipeSync(glistp++);
                        gDPSetPrimColor(glistp++, 0, 0, 255, 255, 255, superWeaponBlinkAlpha);
                        gDPSetCombineMode(glistp++, G_CC_DECALRGB_PRIMA, G_CC_DECALRGB_PRIMA);
                        if (pauseData.selection == -1) {
                            superWeaponBlinkAlpha -= 20;
                            if (superWeaponBlinkAlpha <= 50) {
                                superWeaponBlinkAlpha = 200;
                            }
                        }
                    }
                } else if (g_gameMode == 6) {
                    if (frontendOptions_g.challengeMedal == 4 ||
                        (frontendOptions_g.challengeMedal == 3 && frontendOptions_g.challengeSub == 6)) {
                        gDPPipeSync(glistp++);
                        gDPSetPrimColor(glistp++, 0, 0, 255, 255, 255, superWeaponBlinkAlpha);
                        gDPSetCombineMode(glistp++, G_CC_DECALRGB_PRIMA, G_CC_DECALRGB_PRIMA);
                        if (pauseData.selection == -1) {
                            superWeaponBlinkAlpha -= 20;
                            if (superWeaponBlinkAlpha <= 50) {
                                superWeaponBlinkAlpha = 200;
                            }
                        }
                    }
                }
            }
        }
        gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&icons[weaponType]));
        gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
        gSPSprite2DDraw(glistp++, x, y);
    }
    return glistp;
}

void SetShake(s32 amount, s32 player) {
    shakeAmount[player] = amount;
}

void SetRumble(s32 time, s32 player) {
#ifdef VERSION_EU
    if (player >= 0 && player < MAX_PLAYERS)
#endif
    {
        if (rumbleTime[player] < time) {
            rumbleTime[player] = time;
        }
    }
}

void ShakeScreen(s32 player) {
    if (shakeAmount[player] > 0) {
        cameras[player].camPos->relative.t[0] += newRand() % shakeAmount[player] + (shakeAmount[player] >> 1);
        cameras[player].camPos->relative.t[2] += newRand() % shakeAmount[player] + (shakeAmount[player] >> 1);
        shakeAmount[player]--;
    }
}

void UpdateRumble(s32 player, u8 stop) {
    u8 channel = frontendOptions_g.controller[player];

    if (stop) {
        rumbleActive[channel] = 0;
        StopRumble(channel);
        rumbleTime[player] = 0;
    } else if (rumbleTime[player] >= 2) {
        if (!rumbleActive[channel]) {
            StartRumble(channel);
            rumbleActive[channel] = 1;
        }
        rumbleTime[player]--;
    } else {
        rumbleActive[channel] = 0;
        StopRumble(channel);
    }
}
