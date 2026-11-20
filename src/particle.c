#include "particle.h"

#ident "$Revision: 1.17 $"

/* ============================================================================
 * Particle pool allocation, EU memory-guard canary, and particle spawners
 * ============================================================================ */

SParticle *InitParticle(void) {
    SParticle *particles;

    particles = DAlloc(g_heap, PARTICLE_COUNT * sizeof(SParticle), "Particle.c",
#ifdef VERSION_EU
                       123
#else
                       119
#endif
    );
    ClearParticleTable(particles);
    return particles;
}

#ifdef VERSION_EU
/*
 * Retail Europe (and the Europe prototype) added a two-word sentinel guard in
 * .data right before Particle.c's statics and checks it after writing/drawing
 * the electric bolt table, halting on corruption.
 */
void CheckParticleGuard(void) {
    if (particleGuard[0] != 0xA5A55A5A) {
        for (;;) {
        }
    }
    if (particleGuard[1] != 0x5A5AA5A5) {
        for (;;) {
        }
    }
}
#endif

void AddRandomParticles(SParticle *particles, s32 count, VECTOR *pos, VECTOR *vel) {
    s32 i;
    s32 total;

    total = count;
    for (; count > 0; count--) {
        i = nextParticleSlot;
        nextParticleSlot = (i + 1) % PARTICLE_COUNT;
        if (i > -1) {
            particles[i].pos.vx = pos->vx;
            particles[i].pos.vy = pos->vy;
            particles[i].pos.vz = pos->vz;
            particles[i].vel.vx = (newRand() & 63) - 32 + vel->vx;
            particles[i].vel.vy = (newRand() & 63) - 32;
            particles[i].vel.vz = (newRand() & 63) - 32 + vel->vz;
            particles[i].life = 7;
            particles[i].type = 1;
            particles[i].flags = 0;
            particles[i].vel.pad = total;
        }
    }
}

void AddScrapingParticles(SParticle *particles, s32 count, VECTOR *pos, VECTOR *vel) {
    s32 i;
    s32 total;

    total = count;
    for (; count > 0; count--) {
        i = nextParticleSlot;
        nextParticleSlot = (i + 1) % PARTICLE_COUNT;
        if (i > -1) {
            particles[i].pos.vx = pos->vx;
            particles[i].pos.vy = pos->vy;
            particles[i].pos.vz = pos->vz;
            particles[i].vel.vx = (newRand() & 63) - 32 + vel->vx;
            particles[i].vel.vy = (newRand() & 63) - 32;
            particles[i].vel.vz = (newRand() & 63) - 32 + vel->vz;
            particles[i].life = 7;
            particles[i].type = 2;
            particles[i].flags = 0;
            particles[i].vel.pad = total;
        }
    }
}

void AddEboltParticles(SParticle *particles, s32 count, VECTOR *pos) {
    s32 i;
    s32 total;

    total = count;
    for (; count > 0; count--) {
        i = nextParticleSlot;
        nextParticleSlot = (i + 1) % PARTICLE_COUNT;
        if (i > -1) {
            particles[i].pos.vx = pos->vx;
            particles[i].pos.vy = pos->vy;
            particles[i].pos.vz = pos->vz;
            particles[i].vel.vx = (newRand() & 63) - 31;
            particles[i].vel.vy = (newRand() & 63) - 31;
            particles[i].vel.vz = (newRand() & 63) - 31;
            particles[i].life = 15;
            particles[i].type = 4;
            particles[i].flags = 0;
            particles[i].vel.pad = total;
        }
    }
}

void AddRisingParticles(SParticle *particles, s32 count, VECTOR *pos, s32 fallSpeed) {
    s32 i;
    s32 total;

    total = count;
    for (; count > 0; count--) {
        i = nextParticleSlot;
        nextParticleSlot = (i + 1) % PARTICLE_COUNT;
        if (i > -1) {
            particles[i].pos.vx = pos->vx + newRand() % 1600 - 800;
            particles[i].pos.vy = pos->vy;
            particles[i].pos.vz = pos->vz + newRand() % 1600 - 800;
            particles[i].vel.vx = 0;
            particles[i].vel.vy = -(newRand() & 15) * fallSpeed;
            particles[i].vel.vz = 0;
            particles[i].life = (newRand() & 7) + 8;
            particles[i].type = 2;
            particles[i].flags = 0;
            particles[i].vel.pad = total;
        }
    }
}

void AddPitParticles(SParticle *particles, s32 count, VECTOR *pos, s32 fallSpeed) {
    s32 i;
    s32 total;

    total = count;
    for (; count > 0; count--) {
        i = nextParticleSlot;
        nextParticleSlot = (i + 1) % PARTICLE_COUNT;
        if (i > -1) {
            particles[i].pos.vx = pos->vx + (newRand() & 511) - 256;
            particles[i].pos.vy = pos->vy + 256;
            particles[i].pos.vz = pos->vz + (newRand() & 511) - 256;
            particles[i].vel.vx = 0;
            particles[i].vel.vy = -(newRand() & 7) * fallSpeed;
            particles[i].vel.vz = 0;
            particles[i].life = (newRand() & 7) + 8;
            particles[i].type = 2;
            particles[i].flags = 0;
            particles[i].vel.pad = total;
        }
    }
}

void ClearParticleTable(SParticle *particles) {
    s32 i;

    for (i = 0; i < PARTICLE_COUNT; i++) {
        particles[i].pos.vx = 0;
        particles[i].pos.vy = 0;
        particles[i].pos.vz = 0;
        particles[i].vel.vx = 0;
        particles[i].vel.vy = 0;
        particles[i].vel.vz = 0;
        particles[i].life = 0;
        particles[i].type = 0;
        particles[i].flags = 0;
    }
}

/* ============================================================================
 * Billboard explosions, super-weapon flashes, and cyclone charge orbs
 * ============================================================================ */

void LoadExplodeGFX(void) {
    s32 i;
    s32 start;

    explodeTextureStart = start = TextureTableCount;
    LoadCmpFiles(WADLoadCompressedTextureSequence(g_startupImage));
    for (i = 0; i < EXPLODE_FRAMES; i++) {
        explodeTexture[i] = g_textureTable[start++];
    }
}

void InitExplode(void) {
    s32 i;

    for (i = 0; i < EXPLOSION_COUNT; i++) {
        explosions[i].x = 0;
        explosions[i].y = 0;
        explosions[i].z = 0;
        explosions[i].r = 128;
        explosions[i].g = 128;
        explosions[i].b = 128;
        explosions[i].flash = 0;
        explosions[i].ship = -1;
        explosions[i].size = 0;
        explosions[i].life = 0;
    }
    for (i = CYCLONE_CHARGE_COUNT - 1; i >= 0; i--) {
        cycloneCharges[i].active = 0;
    }
}

void DrawExplode(VECTOR *camera, MATRIX *view, s32 player) {
    SVECTOR right;
    SVECTOR up;
    SVECTOR forward;
    VECTOR corner;
    s32 i;
    s32 frame;
    s32 texture;
    s32 x0, y0, z0;
    s32 x1, y1, z1;
    s32 x2, y2, z2;
    s32 x3, y3, z3;

    right.vx = view->m[0][0] >> 3;
    right.vy = view->m[0][1] >> 3;
    right.vz = view->m[0][2] >> 3;
    up.vx = view->m[1][0] >> 3;
    up.vy = view->m[1][1] >> 3;
    up.vz = view->m[1][2] >> 3;
    forward.vx = view->m[2][0];
    forward.vy = view->m[2][1];
    forward.vz = view->m[2][2];
    texture = -1;
    explosionAnimFrame++;
    for (i = 0; i < EXPLOSION_COUNT; i++) {
        if (explosions[i].life <= 0) {
            continue;
        }
        if (explosions[i].blend == 1 || !(explosionAnimFrame & 1)) {
            explosions[i].life--;
        }
        if ((explosions[i].life >= 15 || explosions[i].flash != 1) &&
            (explosions[i].life >= 17 || explosions[i].flash != 0)) {
            continue;
        }
        if (explosions[i].flash == 1) {
            frame = 29 - explosions[i].life;
        } else {
            frame = 16 - explosions[i].life;
        }
        if (frame < 0) {
            __assert("frame >= 0", "Particle.c",
#ifdef VERSION_EU
                     465
#else
                     447
#endif
            );
        }
        corner.vx = explosions[i].x + (-right.vx >> 1);
        corner.vy = explosions[i].y + (-right.vy >> 1);
        corner.vz = explosions[i].z + (-right.vz >> 1);
        x0 = corner.vx >> 1;
        y0 = corner.vy >> 1;
        z0 = corner.vz >> 1;
        x1 = (corner.vx + right.vx) >> 1;
        y1 = (corner.vy + right.vy) >> 1;
        z1 = (corner.vz + right.vz) >> 1;
        x2 = (corner.vx + (right.vx - up.vx)) >> 1;
        y2 = (corner.vy + (right.vy - up.vy)) >> 1;
        z2 = (corner.vz + (right.vz - up.vz)) >> 1;
        x3 = (corner.vx - up.vx) >> 1;
        y3 = (corner.vy - up.vy) >> 1;
        z3 = (corner.vz - up.vz) >> 1;
        explodeVtx[player][i][0].v.ob[0] = camera->vx + x0;
        explodeVtx[player][i][0].v.ob[1] = camera->vy + y0;
        explodeVtx[player][i][0].v.ob[2] = camera->vz + z0;
        explodeVtx[player][i][1].v.ob[0] = camera->vx + x1;
        explodeVtx[player][i][1].v.ob[1] = camera->vy + y1;
        explodeVtx[player][i][1].v.ob[2] = camera->vz + z1;
        explodeVtx[player][i][2].v.ob[0] = camera->vx + x2;
        explodeVtx[player][i][2].v.ob[1] = camera->vy + y2;
        explodeVtx[player][i][2].v.ob[2] = camera->vz + z2;
        explodeVtx[player][i][3].v.ob[0] = camera->vx + x3;
        explodeVtx[player][i][3].v.ob[1] = camera->vy + y3;
        explodeVtx[player][i][3].v.ob[2] = camera->vz + z3;
        if (explodeTextureStart + frame != texture) {
            texture = explodeTextureStart + frame;
            gSPDisplayList(g_glistp++, g_textureTable[texture]->dl);
        }
        gSPVertex(g_glistp++, OS_K0_TO_PHYSICAL(explodeVtx[player][i]), 4, 0);
        gDPPipeSync(g_glistp++);
        gSP2Triangles(g_glistp++, 0, 1, 2, 0, 0, 3, 2, 0);
    }
}

void DrawSuperFlashes(VECTOR *camera, MATRIX *view, s32 player) {
    SVECTOR right;
    SVECTOR up;
    SVECTOR forward;
    VECTOR corner;
    s32 i;
    s32 j;
    s32 noneActive;
    s32 alpha;
    s32 x0, y0, z0;
    s32 x1, y1, z1;
    s32 x2, y2, z2;
    s32 x3, y3, z3;

    right.vx = view->m[0][0] >> 3;
    right.vy = view->m[0][1] >> 3;
    right.vz = view->m[0][2] >> 3;
    up.vx = view->m[1][0] >> 3;
    up.vy = view->m[1][1] >> 3;
    up.vz = view->m[1][2] >> 3;
    forward.vx = view->m[2][0];
    forward.vy = view->m[2][1];
    forward.vz = view->m[2][2];
    noneActive = 1;
    gSPDisplayList(g_glistp++, g_textureTable[shadow2TextureStart]->dl);
    for (i = 0; i < SUPER_FLASH_COUNT; i++) {
        if (superFlashes[i].life <= 0) {
            continue;
        }
        superFlashes[i].life--;
        alpha = superFlashes[i].life << 4;
        noneActive = 0;
        corner.vx = superFlashes[i].x + ((-right.vx >> 1) + (up.vx >> 1));
        corner.vy = superFlashes[i].y + ((-right.vy >> 1) + (up.vy >> 1));
        corner.vz = superFlashes[i].z + ((-right.vz >> 1) + (up.vz >> 1));
        x0 = corner.vx;
        y0 = corner.vy;
        z0 = corner.vz;
        x1 = corner.vx + right.vx;
        y1 = corner.vy + right.vy;
        z1 = corner.vz + right.vz;
        x2 = corner.vx + (right.vx - up.vx);
        y2 = corner.vy + (right.vy - up.vy);
        z2 = corner.vz + (right.vz - up.vz);
        x3 = corner.vx - up.vx;
        y3 = corner.vy - up.vy;
        z3 = corner.vz - up.vz;
        superFlashVtx[player][i][0].v.ob[0] = camera->vx + (x0 >> 1);
        superFlashVtx[player][i][0].v.ob[1] = camera->vy + (y0 >> 1);
        superFlashVtx[player][i][0].v.ob[2] = camera->vz + (z0 >> 1);
        superFlashVtx[player][i][1].v.ob[0] = camera->vx + (x1 >> 1);
        superFlashVtx[player][i][1].v.ob[1] = camera->vy + (y1 >> 1);
        superFlashVtx[player][i][1].v.ob[2] = camera->vz + (z1 >> 1);
        superFlashVtx[player][i][2].v.ob[0] = camera->vx + (x2 >> 1);
        superFlashVtx[player][i][2].v.ob[1] = camera->vy + (y2 >> 1);
        superFlashVtx[player][i][2].v.ob[2] = camera->vz + (z2 >> 1);
        superFlashVtx[player][i][3].v.ob[0] = camera->vx + (x3 >> 1);
        superFlashVtx[player][i][3].v.ob[1] = camera->vy + (y3 >> 1);
        superFlashVtx[player][i][3].v.ob[2] = camera->vz + (z3 >> 1);
        for (j = 0; j < 4; j++) {
            superFlashVtx[player][i][j].v.cn[0] = superFlashes[i].r;
            superFlashVtx[player][i][j].v.cn[1] = superFlashes[i].g;
            superFlashVtx[player][i][j].v.cn[2] = superFlashes[i].b;
            superFlashVtx[player][i][j].v.cn[3] = alpha;
        }
        gSPVertex(g_glistp++, OS_K0_TO_PHYSICAL(superFlashVtx[player][i]), 4, 0);
        gDPPipeSync(g_glistp++);
        gSP2Triangles(g_glistp++, 0, 1, 2, 0, 0, 3, 2, 0);
    }
    if (noneActive && g_superWeaponActive2) {
        g_superWeaponActive2 = 0;
    }
}

void DrawCycloneCharge(VECTOR *camera, MATRIX *view, s32 player) {
    SVECTOR right;
    SVECTOR up;
    SVECTOR forward;
    VECTOR corner;
    VECTOR boltStartL;
    VECTOR boltStartR;
    VECTOR boltEndL;
    VECTOR boltEndR;
    s32 i;
    s32 j;
    s32 texIdx;
    s32 scale;
    s32 shipNo;
    Object *shape;
    MATRIX *mat;
    s32 x0, y0, z0;
    s32 x1, y1, z1;
    s32 x2, y2, z2;
    s32 x3, y3, z3;

    {
        s32 base = shadow2TextureStart;
        s32 off = cycloneAnimFrame + 4;
        texIdx = base + off;
    }
    if (player == 0) {
        cycloneAnimTick++;
        if (cycloneAnimTick == 2) {
            cycloneAnimFrame++;
            if (cycloneAnimFrame == 8) {
                cycloneAnimFrame = 0;
            }
            cycloneAnimTick = 0;
        }
    }
    gSPDisplayList(g_glistp++, g_textureTable[texIdx]->dl);
    for (i = 0; i < CYCLONE_CHARGE_COUNT; i++) {
        if (!cycloneCharges[i].active) {
            continue;
        }
        shipNo = cycloneCharges[i].ship;
        scale = (cycloneCharges[i].charge * 3) >> 2;
        shape = shipShapes[ships[shipNo].lowResTableIndex];
        shape->skeleton->super = cameras[player].camPos;
        SetSkeletonPosition(shape->skeleton, ships[shipNo].ppivot.vx >> 1,
                            ships[shipNo].ppivot.vy >> 1, ships[shipNo].ppivot.vz >> 1);
        SetSkeletonDirectionHPR(shape->skeleton, ships[shipNo].hdg, ships[shipNo].pitch,
                                ships[shipNo].roll);
        mat = GetSkeletonMatrix(shape->skeleton);
        MatrixToN64Mtx(mat, &g_mtxPool[g_mtxIndex]);
        gSPMatrix(g_glistp++, OS_K0_TO_PHYSICAL(&g_mtxPool[g_mtxIndex]),
                  G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
        g_mtxIndex++;
        right.vx = mat->m[0][0] >> 9;
        right.vy = mat->m[0][1] >> 9;
        right.vz = mat->m[0][2] >> 9;
        up.vx = mat->m[1][0] >> 9;
        up.vy = mat->m[1][1] >> 9;
        up.vz = mat->m[1][2] >> 9;
        forward.vx = mat->m[2][0];
        forward.vy = mat->m[2][1];
        forward.vz = mat->m[2][2];
        right.vx *= scale;
        right.vy *= scale;
        right.vz *= scale;
        up.vx *= scale;
        up.vy *= scale;
        up.vz *= scale;
        corner.vx = cycloneCharges[i].x;
        corner.vy = cycloneCharges[i].y;
        corner.vz = cycloneCharges[i].z;

        boltStartL.vx = boltStartR.vx = ships[shipNo].ppivot.vx + (ships[shipNo].unitVecNose.vx >> 5);
        boltStartL.vy = boltStartR.vy = ships[shipNo].ppivot.vy + (ships[shipNo].unitVecNose.vy >> 5);
        boltStartL.vz = boltStartR.vz = ships[shipNo].ppivot.vz + (ships[shipNo].unitVecNose.vz >> 5);

        boltStartL.vx -= ships[shipNo].unitVecWing.vx >> 6;
        boltStartL.vy -= ships[shipNo].unitVecWing.vy >> 6;
        boltStartL.vz -= ships[shipNo].unitVecWing.vz >> 6;

        boltStartR.vx += ships[shipNo].unitVecWing.vx >> 6;
        boltStartR.vy += ships[shipNo].unitVecWing.vy >> 6;
        boltStartR.vz += ships[shipNo].unitVecWing.vz >> 6;

        boltEndL.vx = ships[shipNo].ppivot.vx + (ships[shipNo].unitVecNose.vx >> 3) - (right.vx >> 2);
        boltEndL.vy = ships[shipNo].ppivot.vy + (ships[shipNo].unitVecNose.vy >> 3) - (right.vy >> 2);
        boltEndL.vz = ships[shipNo].ppivot.vz + (ships[shipNo].unitVecNose.vz >> 3) - (right.vz >> 2);

        boltEndR.vx = ships[shipNo].ppivot.vx + (ships[shipNo].unitVecNose.vx >> 3) + (right.vx >> 2);
        boltEndR.vy = ships[shipNo].ppivot.vy + (ships[shipNo].unitVecNose.vy >> 3) + (right.vy >> 2);
        boltEndR.vz = ships[shipNo].ppivot.vz + (ships[shipNo].unitVecNose.vz >> 3) + (right.vz >> 2);

        AddElectricSegment(&boltStartL, &boltEndL);
        AddElectricSegment(&boltStartR, &boltEndR);
        AddElectricSegment(&boltStartL, &boltStartR);

        /* Original omits (-right.vy >> 1) on the Y corner offset. */
        corner.vx += (-right.vx >> 1) + (up.vx >> 1);
        corner.vy += (up.vy >> 1);
        corner.vz += (-right.vz >> 1) + (up.vz >> 1);
        x0 = corner.vx;
        y0 = corner.vy;
        z0 = corner.vz;
        x1 = corner.vx + right.vx;
        y1 = corner.vy + right.vy;
        z1 = corner.vz + right.vz;
        x2 = corner.vx + (right.vx - up.vx);
        y2 = corner.vy + (right.vy - up.vy);
        z2 = corner.vz + (right.vz - up.vz);
        x3 = corner.vx - up.vx;
        y3 = corner.vy - up.vy;
        z3 = corner.vz - up.vz;
        cycloneVtx[player][i][0].v.ob[0] = x0 >> 1;
        cycloneVtx[player][i][0].v.ob[1] = y0 >> 1;
        cycloneVtx[player][i][0].v.ob[2] = z0 >> 1;
        cycloneVtx[player][i][1].v.ob[0] = x1 >> 1;
        cycloneVtx[player][i][1].v.ob[1] = y1 >> 1;
        cycloneVtx[player][i][1].v.ob[2] = z1 >> 1;
        cycloneVtx[player][i][2].v.ob[0] = x2 >> 1;
        cycloneVtx[player][i][2].v.ob[1] = y2 >> 1;
        cycloneVtx[player][i][2].v.ob[2] = z2 >> 1;
        cycloneVtx[player][i][3].v.ob[0] = x3 >> 1;
        cycloneVtx[player][i][3].v.ob[1] = y3 >> 1;
        cycloneVtx[player][i][3].v.ob[2] = z3 >> 1;
        for (j = 0; j < 4; j++) {
            cycloneVtx[player][i][j].v.cn[0] = cycloneCharges[i].r;
            cycloneVtx[player][i][j].v.cn[1] = cycloneCharges[i].g;
            cycloneVtx[player][i][j].v.cn[2] = cycloneCharges[i].b;
            cycloneVtx[player][i][j].v.cn[3] = cycloneCharges[i].charge << 2;
        }
        gSPVertex(g_glistp++, OS_K0_TO_PHYSICAL(cycloneVtx[player][i]), 4, 0);
        gDPPipeSync(g_glistp++);
        gSP2Triangles(g_glistp++, 0, 1, 2, 0, 0, 3, 2, 0);
    }
}

/* ============================================================================
 * Shock rings, explosion/effect spawners, and projectile/plasma flares
 * ============================================================================ */

void InitRing(void) {
    s32 i;
    s32 j;
    s32 k;

    for (i = 0; i < MAX_PLAYERS; i++) {
        for (j = 0; j < MISSILE_FLARE_COUNT; j++) {
            missileFlareVtx[i][j][0].v.cn[0] = 32;
            missileFlareVtx[i][j][0].v.cn[1] = 64;
            missileFlareVtx[i][j][0].v.cn[2] = 255;
            missileFlareVtx[i][j][0].v.cn[3] = 127;
            for (k = 1; k < 13; k++) {
                if (k == 3 || k == 4 || k == 9 || k == 10) {
                    missileFlareVtx[i][j][k].v.cn[0] = 64;
                    missileFlareVtx[i][j][k].v.cn[1] = 128;
                    missileFlareVtx[i][j][k].v.cn[2] = 255;
                    missileFlareVtx[i][j][k].v.cn[3] = 0;
                } else {
                    missileFlareVtx[i][j][k].v.cn[0] = 32;
                    missileFlareVtx[i][j][k].v.cn[1] = 64;
                    missileFlareVtx[i][j][k].v.cn[2] = 255;
                    missileFlareVtx[i][j][k].v.cn[3] = 0;
                }
            }
        }
    }

    for (i = 0; i < MAX_PLAYERS; i++) {
        for (j = 0; j < ROCKET_FLARE_COUNT; j++) {
            rocketFlareVtx[i][j][0].v.cn[0] = 255;
            rocketFlareVtx[i][j][0].v.cn[1] = 64;
            rocketFlareVtx[i][j][0].v.cn[2] = 32;
            rocketFlareVtx[i][j][0].v.cn[3] = 127;
            for (k = 1; k < 13; k++) {
                if (k == 3 || k == 4 || k == 9 || k == 10) {
                    rocketFlareVtx[i][j][k].v.cn[0] = 255;
                    rocketFlareVtx[i][j][k].v.cn[1] = 128;
                    rocketFlareVtx[i][j][k].v.cn[2] = 64;
                    rocketFlareVtx[i][j][k].v.cn[3] = 0;
                } else {
                    rocketFlareVtx[i][j][k].v.cn[0] = 255;
                    rocketFlareVtx[i][j][k].v.cn[1] = 64;
                    rocketFlareVtx[i][j][k].v.cn[2] = 32;
                    rocketFlareVtx[i][j][k].v.cn[3] = 0;
                }
            }
        }
    }

    for (k = 0; k < MAX_PLAYERS; k++) {
        for (i = 0; i < EXPLOSION_COUNT; i++) {
            for (j = 0; j < 4; j++) {
                explodeVtx[k][i][j].v.cn[0] = 255;
                explodeVtx[k][i][j].v.cn[1] = 255;
                explodeVtx[k][i][j].v.cn[2] = 255;
                explodeVtx[k][i][j].v.cn[3] = 127;
            }
            explodeVtx[k][i][0].v.tc[0] = 256;
            explodeVtx[k][i][0].v.tc[1] = 256;
            explodeVtx[k][i][1].v.tc[0] = 3840;
            explodeVtx[k][i][1].v.tc[1] = 256;
            explodeVtx[k][i][2].v.tc[0] = 3840;
            explodeVtx[k][i][2].v.tc[1] = 3840;
            explodeVtx[k][i][3].v.tc[0] = 256;
            explodeVtx[k][i][3].v.tc[1] = 3840;
        }
    }

    for (i = 0; i < MAX_PLAYERS; i++) {
        for (j = 0; j < PLASMA_FLARE_COUNT; j++) {
            for (k = 0; k < 13; k++) {
                if (k == 0) {
                    plasmaFlareVtx[i][j][0].v.cn[0] = 0;
                    plasmaFlareVtx[i][j][0].v.cn[1] = 255;
                    plasmaFlareVtx[i][j][0].v.cn[2] = 0;
                    plasmaFlareVtx[i][j][0].v.cn[3] = 127;
                } else if (k > 6) {
                    plasmaFlareVtx[i][j][k].v.cn[0] = 0;
                    plasmaFlareVtx[i][j][k].v.cn[1] = 127;
                    plasmaFlareVtx[i][j][k].v.cn[2] = 0;
                    plasmaFlareVtx[i][j][k].v.cn[3] = 31;
                } else {
                    plasmaFlareVtx[i][j][k].v.cn[0] = 0;
                    plasmaFlareVtx[i][j][k].v.cn[1] = 127;
                    plasmaFlareVtx[i][j][k].v.cn[2] = 0;
                    plasmaFlareVtx[i][j][k].v.cn[3] = 127;
                }
            }
        }
    }

    /* Original nests an extra j=0..3 loop that advances by 4 Vtx per step. */
    for (k = 0; k < MAX_PLAYERS; k++) {
        for (i = 0; i < SUPER_FLASH_COUNT; i++) {
            for (j = 0; j < 4; j++) {
                s32 idx = j * 4;
                superFlashVtx[k][i][idx].v.tc[0] = 0;
                superFlashVtx[k][i][idx].v.tc[1] = 256;
                superFlashVtx[k][i][idx + 1].v.tc[0] = 3840;
                superFlashVtx[k][i][idx + 1].v.tc[1] = 256;
                superFlashVtx[k][i][idx + 2].v.tc[0] = 3840;
                superFlashVtx[k][i][idx + 2].v.tc[1] = 4032;
                superFlashVtx[k][i][idx + 3].v.tc[0] = 0;
                superFlashVtx[k][i][idx + 3].v.tc[1] = 4032;
            }
        }
    }

    for (k = 0; k < MAX_PLAYERS; k++) {
        for (i = 0; i < CYCLONE_CHARGE_COUNT; i++) {
            cycloneVtx[k][i][0].v.tc[0] = 0;
            cycloneVtx[k][i][0].v.tc[1] = 1984;
            cycloneVtx[k][i][1].v.tc[0] = 1984;
            cycloneVtx[k][i][1].v.tc[1] = 1984;
            cycloneVtx[k][i][2].v.tc[0] = 1984;
            cycloneVtx[k][i][2].v.tc[1] = 0;
            cycloneVtx[k][i][3].v.tc[0] = 0;
            cycloneVtx[k][i][3].v.tc[1] = 0;
        }
    }

    for (k = 0; k < MAX_PLAYERS; k++) {
        for (i = 0; i < PODIUM_ICON_COUNT; i++) {
            podiumVtx[k][i][0].v.tc[0] = 0;
            podiumVtx[k][i][0].v.tc[1] = 64;
            podiumVtx[k][i][1].v.tc[0] = 3840;
            podiumVtx[k][i][1].v.tc[1] = 64;
            podiumVtx[k][i][2].v.tc[0] = 3840;
            podiumVtx[k][i][2].v.tc[1] = 4032;
            podiumVtx[k][i][3].v.tc[0] = 0;
            podiumVtx[k][i][3].v.tc[1] = 4032;
        }
    }
}

void DrawRing(VECTOR *camera, MATRIX *view, s32 player) {
    /* Unused struct locals preserve the original 0xD8-byte stack layout. */
    VECTOR unusedVec;
    Quat q;
    MATRIX rot;
    VECTOR rotOut;
    SVECTOR baseUp;
    SVECTOR curUp;
    SVECTOR axis;
    VECTOR unusedVec2;
    SVECTOR unusedSvec;
    s32 i;
    s32 j;
    s32 elapsed;
    s32 angle;
    s16 outerRadius;
    s16 innerRadius;
    s32 alpha;
    baseUp.vx = curUp.vx = view->m[1][0] >> 4;
    baseUp.vy = curUp.vy = view->m[1][1] >> 4;
    baseUp.vz = curUp.vz = view->m[1][2] >> 4;
    axis.vx = view->m[2][0];
    axis.vy = view->m[2][1];
    axis.vz = view->m[2][2];

    for (i = 0; i < RING_COUNT; i++) {
        if (rings[i].life <= 0) {
            continue;
        }
        rings[i].life--;
        rings[i].y -= 15;
        if (rings[i].life > 16) {
            continue;
        }
        elapsed = 16 - rings[i].life;
        outerRadius = (elapsed * 3) >> 2;
        innerRadius = ((elapsed - 3) * 3) >> 2;
        if (innerRadius <= 0) {
            innerRadius = 1;
        }
        if (outerRadius <= 0) {
            outerRadius = 1;
        }
        angle = 0;
        for (j = 0; j < 16; j++) {
            AxisAngleToQuat(&q, angle, &axis);
            QuatToMatrix(&q, &rot);
            rot.t[2] = 0;
            rot.t[1] = 0;
            rot.t[0] = 0;
            ApplyMatrix(&rot, &baseUp, &rotOut);
            curUp.vx = rotOut.vx;
            curUp.vy = rotOut.vy;
            curUp.vz = rotOut.vz;
            {
                s32 ox = rings[i].x + curUp.vx * outerRadius;
                s32 oy = rings[i].y + curUp.vy * outerRadius;
                s32 oz = rings[i].z + curUp.vz * outerRadius;
                s32 ix = rings[i].x + curUp.vx * innerRadius;
                s32 iy = rings[i].y + curUp.vy * innerRadius;
                s32 iz = rings[i].z + curUp.vz * innerRadius;
                ringVtx[player][i][j].v.ob[0] = camera->vx + (ox >> 1);
                ringVtx[player][i][j].v.ob[1] = camera->vy + (oy >> 1);
                ringVtx[player][i][j].v.ob[2] = camera->vz + (oz >> 1);
                ringVtx[player][i][j + 16].v.ob[0] = camera->vx + (ix >> 1);
                ringVtx[player][i][j + 16].v.ob[1] = camera->vy + (iy >> 1);
                ringVtx[player][i][j + 16].v.ob[2] = camera->vz + (iz >> 1);
            }
            angle += 256;
        }
        alpha = 128 - (elapsed << 3);
        for (j = 0; j < 16; j++) {
            ringVtx[player][i][j].v.cn[0] = rings[i].r;
            ringVtx[player][i][j].v.cn[1] = rings[i].g;
            ringVtx[player][i][j].v.cn[2] = rings[i].b;
            ringVtx[player][i][j].v.cn[3] = alpha;
            ringVtx[player][i][j + 16].v.cn[0] = rings[i].r;
            ringVtx[player][i][j + 16].v.cn[1] = rings[i].g;
            ringVtx[player][i][j + 16].v.cn[2] = rings[i].b;
            ringVtx[player][i][j + 16].v.cn[3] = 0;
        }
#ifdef VERSION_EU
        {
            s32 dx = rings[i].x + (camera->vx << 1);
            s32 dy = rings[i].y + (camera->vy << 1);
            s32 dz = rings[i].z + (camera->vz << 1);
            dx = __builtin_abs(dx);
            dy = __builtin_abs(dy);
            dz = __builtin_abs(dz);
            if (dx >= 0x4000 || dy >= 0x4000 || dz >= 0x4000) {
                continue;
            }
        }
#endif
        {
            gSPVertex(g_glistp++, OS_K0_TO_PHYSICAL(ringVtx[player][i]), 32, 0);
            gDPPipeSync(g_glistp++);
            for (j = 1; j < 15; j++) {
                gSP2Triangles(g_glistp++, j, j + 1, j + 17, 0, j, j + 17, j + 16, 0);
            }
        }
    }
}

void AddSpurtExplode(s32 x, s32 y, s32 z, s16 ship) {
    s32 i;

    for (i = 0; i < EXPLOSION_COUNT; i++) {
        if (explosions[i].life <= 0) {
            explosions[i].life = 15;
            explosions[i].x = x;
            explosions[i].y = y;
            explosions[i].z = z;
            explosions[i].r = 128;
            explosions[i].g = 128;
            explosions[i].b = 128;
            explosions[i].blend = 1;
            explosions[i].flash = 1;
            explosions[i].ship = ship;
            explosions[i].size = 1;
            return;
        }
    }
}

void AddExplode(s32 x, s32 y, s32 z, s16 ship) {
    s32 i;

    for (i = 0; i < EXPLOSION_COUNT; i++) {
        if (explosions[i].life <= 0) {
            explosions[i].life = 17;
            explosions[i].x = x;
            explosions[i].y = y;
            explosions[i].z = z;
            explosions[i].r = 128;
            explosions[i].g = 128;
            explosions[i].b = 128;
            explosions[i].blend = 1;
            explosions[i].flash = 0;
            explosions[i].ship = ship;
            explosions[i].size = 1;
            return;
        }
    }
}

void AddMinigunExplode(s32 x, s32 y, s32 z, s32 unused) {
    s32 i;

    for (i = 0; i < EXPLOSION_COUNT; i++) {
        if (explosions[i].life <= 0) {
            explosions[i].life = 17;
            explosions[i].x = x;
            explosions[i].y = y;
            explosions[i].z = z;
            explosions[i].r = 32;
            explosions[i].g = 64;
            explosions[i].b = 192;
            explosions[i].blend = 1;
            explosions[i].ship = -1;
            explosions[i].flash = 1;
            explosions[i].size = 2;
            return;
        }
    }
}

void AddSmallSpurtExplode(s32 x, s32 y, s32 z, s16 ship) {
    s32 i;

    for (i = 0; i < EXPLOSION_COUNT; i++) {
        if (explosions[i].life <= 0) {
            explosions[i].life = 17;
            explosions[i].x = x;
            explosions[i].y = y;
            explosions[i].z = z;
            explosions[i].r = 96;
            explosions[i].g = 96;
            explosions[i].b = 96;
            explosions[i].blend = 1;
            explosions[i].ship = ship;
            explosions[i].flash = 1;
            explosions[i].size = 3;
            return;
        }
    }
}

void AddSmallSmokeExplode(s32 x, s32 y, s32 z, s16 ship) {
    s32 i;

    for (i = 0; i < EXPLOSION_COUNT; i++) {
        if (explosions[i].life <= 0) {
            explosions[i].life = 17;
            explosions[i].x = x;
            explosions[i].y = y;
            explosions[i].z = z;
            explosions[i].r = 16;
            explosions[i].g = 16;
            explosions[i].b = 32;
            explosions[i].blend = 2;
            explosions[i].ship = ship;
            explosions[i].flash = 0;
            explosions[i].size = 2;
            return;
        }
    }
}

void AddSmokeExplode(s32 x, s32 y, s32 z, s32 unused) {
    s32 i;

    for (i = 0; i < EXPLOSION_COUNT; i++) {
        if (explosions[i].life <= 0) {
            explosions[i].life = 17;
            explosions[i].x = x;
            explosions[i].y = y;
            explosions[i].z = z;
            explosions[i].r = 16;
            explosions[i].g = 16;
            explosions[i].b = 32;
            explosions[i].blend = 2;
            explosions[i].flash = 0;
            explosions[i].ship = -1;
            explosions[i].size = 1;
            return;
        }
    }
}

void AddRing(s32 x, s32 y, s32 z, s32 unused, u8 *rgb) {
    s32 i;

    for (i = 0; i < RING_COUNT; i++) {
        if (rings[i].life <= 0) {
            rings[i].life = 16;
            rings[i].x = x;
            rings[i].y = y;
            rings[i].z = z;
            rings[i].r = rgb[0];
            rings[i].g = rgb[1];
            rings[i].b = rgb[2];
            return;
        }
    }
}

void AddSuperFlash(VECTOR *pos, s32 unused, s16 life, s32 colourType) {
    s32 i;

    for (i = 0; i < SUPER_FLASH_COUNT; i++) {
        if (superFlashes[i].life <= 0) {
            superFlashes[i].life = life;
            superFlashes[i].x = pos->vx;
            superFlashes[i].y = pos->vy;
            superFlashes[i].z = pos->vz;
            if (colourType == 25) {
                superFlashes[i].r = 255;
                superFlashes[i].g = 255;
                superFlashes[i].b = 255;
            } else if (colourType == 24) {
                superFlashes[i].r = 255;
                superFlashes[i].g = 127;
                superFlashes[i].b = 0;
            } else {
                superFlashes[i].r = 255;
                superFlashes[i].g = 63;
                superFlashes[i].b = 0;
            }
            return;
        }
    }
}

/* Retail Europe rescales the 60 Hz charge cap (30 frames) to 50 Hz PAL (25 frames). */
void SpawnCycloneCharge(s32 shipNo, s32 charge) {
    s32 i;

    for (i = 0; i < CYCLONE_CHARGE_COUNT; i++) {
        if (cycloneCharges[i].active == 0) {
            cycloneCharges[i].x = 0;
            cycloneCharges[i].y = 0;
            cycloneCharges[i].z = 512;
            cycloneCharges[i].r = 255;
            cycloneCharges[i].g = 255;
            cycloneCharges[i].b = 255;
            cycloneCharges[i].active = 1;
            cycloneCharges[i].ship = shipNo;
            if (charge < CYCLONE_MAX_CHARGE) {
                cycloneCharges[i].charge = charge;
            } else {
                cycloneCharges[i].charge = CYCLONE_MAX_CHARGE;
            }
            return;
        }
    }
}

void DrawMissileFlare(VECTOR *pos, VECTOR *camera, MATRIX *view, MATRIX *rot1, MATRIX *rot2,
                      s32 player) {
    /* Unused struct locals preserve the original 0xC8-byte stack layout. */
    VECTOR unusedVec;
    VECTOR vec;
    Quat unusedQuat;
    MATRIX unusedMat;
    SVECTOR xAxis1;
    SVECTOR yAxis1;
    SVECTOR zAxis;
    SVECTOR xAxis2;
    SVECTOR yAxis2;
    s32 x0, y0, z0;
    s32 x1, y1, z1;
    s32 x2, y2, z2;
    s32 x3, y3, z3;

    xAxis2.vx = xAxis1.vx = view->m[0][0] >> 4;
    xAxis2.vy = xAxis1.vy = view->m[0][1] >> 4;
    xAxis2.vz = xAxis1.vz = view->m[0][2] >> 4;
    yAxis2.vx = yAxis1.vx = view->m[1][0] >> 4;
    yAxis2.vy = yAxis1.vy = view->m[1][1] >> 4;
    yAxis2.vz = yAxis1.vz = view->m[1][2] >> 4;
    zAxis.vx = view->m[2][0];
    zAxis.vy = view->m[2][1];
    zAxis.vz = view->m[2][2];

    ApplyMatrix(rot1, &xAxis1, &vec);
    xAxis1.vx = vec.vx;
    xAxis1.vy = vec.vy;
    xAxis1.vz = vec.vz;
    ApplyMatrix(rot1, &yAxis1, &vec);
    yAxis1.vx = vec.vx;
    yAxis1.vy = vec.vy;
    yAxis1.vz = vec.vz;
    ApplyMatrix(rot2, &xAxis2, &vec);
    xAxis2.vx = vec.vx;
    xAxis2.vy = vec.vy;
    xAxis2.vz = vec.vz;
    ApplyMatrix(rot2, &yAxis2, &vec);
    yAxis2.vx = vec.vx;
    yAxis2.vy = vec.vy;
    yAxis2.vz = vec.vz;

    if (missileFlareCount < MISSILE_FLARE_COUNT) {
        x0 = pos->vx;
        y0 = pos->vy;
        z0 = pos->vz;
        x1 = pos->vx + (xAxis1.vx + yAxis1.vx);
        y1 = pos->vy + (xAxis1.vy + yAxis1.vy);
        z1 = pos->vz + (xAxis1.vz + yAxis1.vz);
        x2 = pos->vx + yAxis1.vx;
        y2 = pos->vy + yAxis1.vy;
        z2 = pos->vz + yAxis1.vz;
        x3 = pos->vx + (yAxis1.vx - xAxis1.vx);
        y3 = pos->vy + (yAxis1.vy - xAxis1.vy);
        z3 = pos->vz + (yAxis1.vz - xAxis1.vz);
        missileFlareVtx[player][missileFlareCount][0].v.ob[0] = camera->vx + (x0 >> 1);
        missileFlareVtx[player][missileFlareCount][0].v.ob[1] = camera->vy + (y0 >> 1);
        missileFlareVtx[player][missileFlareCount][0].v.ob[2] = camera->vz + (z0 >> 1);
        missileFlareVtx[player][missileFlareCount][1].v.ob[0] = camera->vx + (x1 >> 1);
        missileFlareVtx[player][missileFlareCount][1].v.ob[1] = camera->vy + (y1 >> 1);
        missileFlareVtx[player][missileFlareCount][1].v.ob[2] = camera->vz + (z1 >> 1);
        missileFlareVtx[player][missileFlareCount][2].v.ob[0] = camera->vx + (x2 >> 1);
        missileFlareVtx[player][missileFlareCount][2].v.ob[1] = camera->vy + (y2 >> 1);
        missileFlareVtx[player][missileFlareCount][2].v.ob[2] = camera->vz + (z2 >> 1);
        missileFlareVtx[player][missileFlareCount][3].v.ob[0] = camera->vx + (x3 >> 1);
        missileFlareVtx[player][missileFlareCount][3].v.ob[1] = camera->vy + (y3 >> 1);
        missileFlareVtx[player][missileFlareCount][3].v.ob[2] = camera->vz + (z3 >> 1);

        x1 = pos->vx - (xAxis1.vx + yAxis1.vx);
        y1 = pos->vy - (xAxis1.vy + yAxis1.vy);
        z1 = pos->vz - (xAxis1.vz + yAxis1.vz);
        x2 = pos->vx - yAxis1.vx;
        y2 = pos->vy - yAxis1.vy;
        z2 = pos->vz - yAxis1.vz;
        x3 = pos->vx - (yAxis1.vx - xAxis1.vx);
        y3 = pos->vy - (yAxis1.vy - xAxis1.vy);
        z3 = pos->vz - (yAxis1.vz - xAxis1.vz);
        missileFlareVtx[player][missileFlareCount][4].v.ob[0] = camera->vx + (x1 >> 1);
        missileFlareVtx[player][missileFlareCount][4].v.ob[1] = camera->vy + (y1 >> 1);
        missileFlareVtx[player][missileFlareCount][4].v.ob[2] = camera->vz + (z1 >> 1);
        missileFlareVtx[player][missileFlareCount][5].v.ob[0] = camera->vx + (x2 >> 1);
        missileFlareVtx[player][missileFlareCount][5].v.ob[1] = camera->vy + (y2 >> 1);
        missileFlareVtx[player][missileFlareCount][5].v.ob[2] = camera->vz + (z2 >> 1);
        missileFlareVtx[player][missileFlareCount][6].v.ob[0] = camera->vx + (x3 >> 1);
        missileFlareVtx[player][missileFlareCount][6].v.ob[1] = camera->vy + (y3 >> 1);
        missileFlareVtx[player][missileFlareCount][6].v.ob[2] = camera->vz + (z3 >> 1);

        x1 = pos->vx + (xAxis2.vx + yAxis2.vx);
        y1 = pos->vy + (xAxis2.vy + yAxis2.vy);
        z1 = pos->vz + (xAxis2.vz + yAxis2.vz);
        x2 = pos->vx + yAxis2.vx;
        y2 = pos->vy + yAxis2.vy;
        z2 = pos->vz + yAxis2.vz;
        x3 = pos->vx + (yAxis2.vx - xAxis2.vx);
        y3 = pos->vy + (yAxis2.vy - xAxis2.vy);
        z3 = pos->vz + (yAxis2.vz - xAxis2.vz);
        missileFlareVtx[player][missileFlareCount][7].v.ob[0] = camera->vx + (x1 >> 1);
        missileFlareVtx[player][missileFlareCount][7].v.ob[1] = camera->vy + (y1 >> 1);
        missileFlareVtx[player][missileFlareCount][7].v.ob[2] = camera->vz + (z1 >> 1);
        missileFlareVtx[player][missileFlareCount][8].v.ob[0] = camera->vx + (x2 >> 1);
        missileFlareVtx[player][missileFlareCount][8].v.ob[1] = camera->vy + (y2 >> 1);
        missileFlareVtx[player][missileFlareCount][8].v.ob[2] = camera->vz + (z2 >> 1);
        missileFlareVtx[player][missileFlareCount][9].v.ob[0] = camera->vx + (x3 >> 1);
        missileFlareVtx[player][missileFlareCount][9].v.ob[1] = camera->vy + (y3 >> 1);
        missileFlareVtx[player][missileFlareCount][9].v.ob[2] = camera->vz + (z3 >> 1);

        x1 = pos->vx - (xAxis2.vx + yAxis2.vx);
        y1 = pos->vy - (xAxis2.vy + yAxis2.vy);
        z1 = pos->vz - (xAxis2.vz + yAxis2.vz);
        x2 = pos->vx - yAxis2.vx;
        y2 = pos->vy - yAxis2.vy;
        z2 = pos->vz - yAxis2.vz;
        x3 = pos->vx - (yAxis2.vx - xAxis2.vx);
        y3 = pos->vy - (yAxis2.vy - xAxis2.vy);
        z3 = pos->vz - (yAxis2.vz - xAxis2.vz);
        missileFlareVtx[player][missileFlareCount][10].v.ob[0] = camera->vx + (x1 >> 1);
        missileFlareVtx[player][missileFlareCount][10].v.ob[1] = camera->vy + (y1 >> 1);
        missileFlareVtx[player][missileFlareCount][10].v.ob[2] = camera->vz + (z1 >> 1);
        missileFlareVtx[player][missileFlareCount][11].v.ob[0] = camera->vx + (x2 >> 1);
        missileFlareVtx[player][missileFlareCount][11].v.ob[1] = camera->vy + (y2 >> 1);
        missileFlareVtx[player][missileFlareCount][11].v.ob[2] = camera->vz + (z2 >> 1);
        missileFlareVtx[player][missileFlareCount][12].v.ob[0] = camera->vx + (x3 >> 1);
        missileFlareVtx[player][missileFlareCount][12].v.ob[1] = camera->vy + (y3 >> 1);
        missileFlareVtx[player][missileFlareCount][12].v.ob[2] = camera->vz + (z3 >> 1);

        gSPVertex(g_glistp++, OS_K0_TO_PHYSICAL(missileFlareVtx[player][missileFlareCount++]), 13, 0);
        gDPPipeSync(g_glistp++);
        gSP2Triangles(g_glistp++, 0, 1, 2, 0, 0, 2, 3, 0);
        gSP2Triangles(g_glistp++, 0, 4, 5, 0, 0, 5, 6, 0);
        gSP2Triangles(g_glistp++, 0, 7, 8, 0, 0, 8, 9, 0);
        gSP2Triangles(g_glistp++, 0, 10, 11, 0, 0, 11, 12, 0);
    }
}

void DrawRocketFlare(VECTOR *pos, VECTOR *camera, MATRIX *view, MATRIX *rot1, MATRIX *rot2,
                     s32 player) {
    /* Unused struct locals preserve the original 0xC8-byte stack layout. */
    VECTOR unusedVec;
    VECTOR vec;
    Quat unusedQuat;
    MATRIX unusedMat;
    SVECTOR xAxis1;
    SVECTOR yAxis1;
    SVECTOR zAxis;
    SVECTOR xAxis2;
    SVECTOR yAxis2;
    s32 x0, y0, z0;
    s32 x1, y1, z1;
    s32 x2, y2, z2;
    s32 x3, y3, z3;

    xAxis2.vx = xAxis1.vx = view->m[0][0] >> 4;
    xAxis2.vy = xAxis1.vy = view->m[0][1] >> 4;
    xAxis2.vz = xAxis1.vz = view->m[0][2] >> 4;
    yAxis2.vx = yAxis1.vx = view->m[1][0] >> 4;
    yAxis2.vy = yAxis1.vy = view->m[1][1] >> 4;
    yAxis2.vz = yAxis1.vz = view->m[1][2] >> 4;
    zAxis.vx = view->m[2][0];
    zAxis.vy = view->m[2][1];
    zAxis.vz = view->m[2][2];

    ApplyMatrix(rot1, &xAxis1, &vec);
    xAxis1.vx = vec.vx;
    xAxis1.vy = vec.vy;
    xAxis1.vz = vec.vz;
    ApplyMatrix(rot1, &yAxis1, &vec);
    yAxis1.vx = vec.vx;
    yAxis1.vy = vec.vy;
    yAxis1.vz = vec.vz;
    ApplyMatrix(rot2, &xAxis2, &vec);
    xAxis2.vx = vec.vx;
    xAxis2.vy = vec.vy;
    xAxis2.vz = vec.vz;
    ApplyMatrix(rot2, &yAxis2, &vec);
    yAxis2.vx = vec.vx;
    yAxis2.vy = vec.vy;
    yAxis2.vz = vec.vz;

    if (rocketFlareCount < ROCKET_FLARE_COUNT) {
        x0 = pos->vx;
        y0 = pos->vy;
        z0 = pos->vz;
        x1 = pos->vx + (xAxis1.vx + yAxis1.vx);
        y1 = pos->vy + (xAxis1.vy + yAxis1.vy);
        z1 = pos->vz + (xAxis1.vz + yAxis1.vz);
        x2 = pos->vx + yAxis1.vx;
        y2 = pos->vy + yAxis1.vy;
        z2 = pos->vz + yAxis1.vz;
        x3 = pos->vx + (yAxis1.vx - xAxis1.vx);
        y3 = pos->vy + (yAxis1.vy - xAxis1.vy);
        z3 = pos->vz + (yAxis1.vz - xAxis1.vz);
        rocketFlareVtx[player][rocketFlareCount][0].v.ob[0] = camera->vx + (x0 >> 1);
        rocketFlareVtx[player][rocketFlareCount][0].v.ob[1] = camera->vy + (y0 >> 1);
        rocketFlareVtx[player][rocketFlareCount][0].v.ob[2] = camera->vz + (z0 >> 1);
        rocketFlareVtx[player][rocketFlareCount][1].v.ob[0] = camera->vx + (x1 >> 1);
        rocketFlareVtx[player][rocketFlareCount][1].v.ob[1] = camera->vy + (y1 >> 1);
        rocketFlareVtx[player][rocketFlareCount][1].v.ob[2] = camera->vz + (z1 >> 1);
        rocketFlareVtx[player][rocketFlareCount][2].v.ob[0] = camera->vx + (x2 >> 1);
        rocketFlareVtx[player][rocketFlareCount][2].v.ob[1] = camera->vy + (y2 >> 1);
        rocketFlareVtx[player][rocketFlareCount][2].v.ob[2] = camera->vz + (z2 >> 1);
        rocketFlareVtx[player][rocketFlareCount][3].v.ob[0] = camera->vx + (x3 >> 1);
        rocketFlareVtx[player][rocketFlareCount][3].v.ob[1] = camera->vy + (y3 >> 1);
        rocketFlareVtx[player][rocketFlareCount][3].v.ob[2] = camera->vz + (z3 >> 1);

        x1 = pos->vx - (xAxis1.vx + yAxis1.vx);
        y1 = pos->vy - (xAxis1.vy + yAxis1.vy);
        z1 = pos->vz - (xAxis1.vz + yAxis1.vz);
        x2 = pos->vx - yAxis1.vx;
        y2 = pos->vy - yAxis1.vy;
        z2 = pos->vz - yAxis1.vz;
        x3 = pos->vx - (yAxis1.vx - xAxis1.vx);
        y3 = pos->vy - (yAxis1.vy - xAxis1.vy);
        z3 = pos->vz - (yAxis1.vz - xAxis1.vz);
        rocketFlareVtx[player][rocketFlareCount][4].v.ob[0] = camera->vx + (x1 >> 1);
        rocketFlareVtx[player][rocketFlareCount][4].v.ob[1] = camera->vy + (y1 >> 1);
        rocketFlareVtx[player][rocketFlareCount][4].v.ob[2] = camera->vz + (z1 >> 1);
        rocketFlareVtx[player][rocketFlareCount][5].v.ob[0] = camera->vx + (x2 >> 1);
        rocketFlareVtx[player][rocketFlareCount][5].v.ob[1] = camera->vy + (y2 >> 1);
        rocketFlareVtx[player][rocketFlareCount][5].v.ob[2] = camera->vz + (z2 >> 1);
        rocketFlareVtx[player][rocketFlareCount][6].v.ob[0] = camera->vx + (x3 >> 1);
        rocketFlareVtx[player][rocketFlareCount][6].v.ob[1] = camera->vy + (y3 >> 1);
        rocketFlareVtx[player][rocketFlareCount][6].v.ob[2] = camera->vz + (z3 >> 1);

        x1 = pos->vx + (xAxis2.vx + yAxis2.vx);
        y1 = pos->vy + (xAxis2.vy + yAxis2.vy);
        z1 = pos->vz + (xAxis2.vz + yAxis2.vz);
        x2 = pos->vx + yAxis2.vx;
        y2 = pos->vy + yAxis2.vy;
        z2 = pos->vz + yAxis2.vz;
        x3 = pos->vx + (yAxis2.vx - xAxis2.vx);
        y3 = pos->vy + (yAxis2.vy - xAxis2.vy);
        z3 = pos->vz + (yAxis2.vz - xAxis2.vz);
        rocketFlareVtx[player][rocketFlareCount][7].v.ob[0] = camera->vx + (x1 >> 1);
        rocketFlareVtx[player][rocketFlareCount][7].v.ob[1] = camera->vy + (y1 >> 1);
        rocketFlareVtx[player][rocketFlareCount][7].v.ob[2] = camera->vz + (z1 >> 1);
        rocketFlareVtx[player][rocketFlareCount][8].v.ob[0] = camera->vx + (x2 >> 1);
        rocketFlareVtx[player][rocketFlareCount][8].v.ob[1] = camera->vy + (y2 >> 1);
        rocketFlareVtx[player][rocketFlareCount][8].v.ob[2] = camera->vz + (z2 >> 1);
        rocketFlareVtx[player][rocketFlareCount][9].v.ob[0] = camera->vx + (x3 >> 1);
        rocketFlareVtx[player][rocketFlareCount][9].v.ob[1] = camera->vy + (y3 >> 1);
        rocketFlareVtx[player][rocketFlareCount][9].v.ob[2] = camera->vz + (z3 >> 1);

        x1 = pos->vx - (xAxis2.vx + yAxis2.vx);
        y1 = pos->vy - (xAxis2.vy + yAxis2.vy);
        z1 = pos->vz - (xAxis2.vz + yAxis2.vz);
        x2 = pos->vx - yAxis2.vx;
        y2 = pos->vy - yAxis2.vy;
        z2 = pos->vz - yAxis2.vz;
        x3 = pos->vx - (yAxis2.vx - xAxis2.vx);
        y3 = pos->vy - (yAxis2.vy - xAxis2.vy);
        z3 = pos->vz - (yAxis2.vz - xAxis2.vz);
        rocketFlareVtx[player][rocketFlareCount][10].v.ob[0] = camera->vx + (x1 >> 1);
        rocketFlareVtx[player][rocketFlareCount][10].v.ob[1] = camera->vy + (y1 >> 1);
        rocketFlareVtx[player][rocketFlareCount][10].v.ob[2] = camera->vz + (z1 >> 1);
        rocketFlareVtx[player][rocketFlareCount][11].v.ob[0] = camera->vx + (x2 >> 1);
        rocketFlareVtx[player][rocketFlareCount][11].v.ob[1] = camera->vy + (y2 >> 1);
        rocketFlareVtx[player][rocketFlareCount][11].v.ob[2] = camera->vz + (z2 >> 1);
        rocketFlareVtx[player][rocketFlareCount][12].v.ob[0] = camera->vx + (x3 >> 1);
        rocketFlareVtx[player][rocketFlareCount][12].v.ob[1] = camera->vy + (y3 >> 1);
        rocketFlareVtx[player][rocketFlareCount][12].v.ob[2] = camera->vz + (z3 >> 1);

        gSPVertex(g_glistp++, OS_K0_TO_PHYSICAL(rocketFlareVtx[player][rocketFlareCount++]), 13, 0);
        gDPPipeSync(g_glistp++);
        gSP2Triangles(g_glistp++, 0, 1, 2, 0, 0, 2, 3, 0);
        gSP2Triangles(g_glistp++, 0, 4, 5, 0, 0, 5, 6, 0);
        gSP2Triangles(g_glistp++, 0, 7, 8, 0, 0, 8, 9, 0);
        gSP2Triangles(g_glistp++, 0, 10, 11, 0, 0, 11, 12, 0);
    }
}

void DrawPlasmaFlare(VECTOR *pos, VECTOR *camera, MATRIX *view, MATRIX *rot1, MATRIX *rot2,
                     s32 player, s32 isRed) {
    /* Unused struct locals preserve the original 0xC0-byte stack layout. */
    VECTOR unusedVec;
    VECTOR vec;
    Quat unusedQuat;
    MATRIX unusedMat;
    SVECTOR xAxis1;
    SVECTOR yAxis1;
    SVECTOR zAxis;
    SVECTOR halfX;
    SVECTOR xAxis2;
    SVECTOR yAxis2;
    s32 i;
    s32 x0, y0, z0;
    s32 x1, y1, z1;
    s32 x2, y2, z2;
    s32 x3, y3, z3;
    s32 x4, y4, z4;

    xAxis2.vx = xAxis1.vx = view->m[0][0] >> 4;
    xAxis2.vy = xAxis1.vy = view->m[0][1] >> 4;
    xAxis2.vz = xAxis1.vz = view->m[0][2] >> 4;
    yAxis2.vx = yAxis1.vx = view->m[1][0] >> 4;
    yAxis2.vy = yAxis1.vy = view->m[1][1] >> 4;
    yAxis2.vz = yAxis1.vz = view->m[1][2] >> 4;
    zAxis.vx = view->m[2][0];
    zAxis.vy = view->m[2][1];
    zAxis.vz = view->m[2][2];

    ApplyMatrix(rot1, &xAxis1, &vec);
    xAxis1.vx = vec.vx;
    xAxis1.vy = vec.vy;
    xAxis1.vz = vec.vz;
    ApplyMatrix(rot1, &yAxis1, &vec);
    yAxis1.vx = vec.vx;
    yAxis1.vy = vec.vy;
    yAxis1.vz = vec.vz;
    ApplyMatrix(rot2, &xAxis2, &vec);
    xAxis2.vx = vec.vx;
    xAxis2.vy = vec.vy;
    xAxis2.vz = vec.vz;
    ApplyMatrix(rot2, &yAxis2, &vec);
    yAxis2.vx = vec.vx;
    yAxis2.vy = vec.vy;
    yAxis2.vz = vec.vz;

    if (plasmaFlareCount < PLASMA_FLARE_COUNT) {
        halfX.vx = xAxis1.vx >> 1;
        halfX.vy = xAxis1.vy >> 1;
        halfX.vz = xAxis1.vz >> 1;
        x0 = pos->vx;
        y0 = pos->vy;
        z0 = pos->vz;
        x1 = pos->vx + (yAxis1.vx - halfX.vx);
        y1 = pos->vy + (yAxis1.vy - halfX.vy);
        z1 = pos->vz + (yAxis1.vz - halfX.vz);
        x2 = pos->vx + (halfX.vx + yAxis1.vx);
        y2 = pos->vy + (halfX.vy + yAxis1.vy);
        z2 = pos->vz + (halfX.vz + yAxis1.vz);
        x3 = pos->vx - xAxis1.vx;
        y3 = pos->vy - xAxis1.vy;
        z3 = pos->vz - xAxis1.vz;
        x4 = pos->vx + xAxis1.vx;
        y4 = pos->vy + xAxis1.vy;
        z4 = pos->vz + xAxis1.vz;
        plasmaFlareVtx[player][plasmaFlareCount][0].v.ob[0] = camera->vx + (x0 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][0].v.ob[1] = camera->vy + (y0 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][0].v.ob[2] = camera->vz + (z0 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][1].v.ob[0] = camera->vx + (x1 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][1].v.ob[1] = camera->vy + (y1 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][1].v.ob[2] = camera->vz + (z1 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][2].v.ob[0] = camera->vx + (x2 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][2].v.ob[1] = camera->vy + (y2 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][2].v.ob[2] = camera->vz + (z2 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][3].v.ob[0] = camera->vx + (x3 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][3].v.ob[1] = camera->vy + (y3 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][3].v.ob[2] = camera->vz + (z3 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][4].v.ob[0] = camera->vx + (x4 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][4].v.ob[1] = camera->vy + (y4 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][4].v.ob[2] = camera->vz + (z4 >> 1);

        x0 = pos->vx + (-halfX.vx - yAxis1.vx);
        y0 = pos->vy + (-halfX.vy - yAxis1.vy);
        z0 = pos->vz + (-halfX.vz - yAxis1.vz);
        x1 = pos->vx + (halfX.vx - yAxis1.vx);
        y1 = pos->vy + (halfX.vy - yAxis1.vy);
        z1 = pos->vz + (halfX.vz - yAxis1.vz);
        plasmaFlareVtx[player][plasmaFlareCount][5].v.ob[0] = camera->vx + (x0 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][5].v.ob[1] = camera->vy + (y0 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][5].v.ob[2] = camera->vz + (z0 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][6].v.ob[0] = camera->vx + (x1 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][6].v.ob[1] = camera->vy + (y1 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][6].v.ob[2] = camera->vz + (z1 >> 1);

        halfX.vx = xAxis2.vx >> 1;
        halfX.vy = xAxis2.vy >> 1;
        halfX.vz = xAxis2.vz >> 1;
        x0 = pos->vx + (yAxis2.vx - halfX.vx);
        y0 = pos->vy + (yAxis2.vy - halfX.vy);
        z0 = pos->vz + (yAxis2.vz - halfX.vz);
        x1 = pos->vx + (halfX.vx + yAxis2.vx);
        y1 = pos->vy + (halfX.vy + yAxis2.vy);
        z1 = pos->vz + (halfX.vz + yAxis2.vz);
        x2 = pos->vx - xAxis2.vx;
        y2 = pos->vy - xAxis2.vy;
        z2 = pos->vz - xAxis2.vz;
        x3 = pos->vx + xAxis2.vx;
        y3 = pos->vy + xAxis2.vy;
        z3 = pos->vz + xAxis2.vz;
        plasmaFlareVtx[player][plasmaFlareCount][7].v.ob[0] = camera->vx + (x0 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][7].v.ob[1] = camera->vy + (y0 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][7].v.ob[2] = camera->vz + (z0 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][8].v.ob[0] = camera->vx + (x1 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][8].v.ob[1] = camera->vy + (y1 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][8].v.ob[2] = camera->vz + (z1 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][9].v.ob[0] = camera->vx + (x2 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][9].v.ob[1] = camera->vy + (y2 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][9].v.ob[2] = camera->vz + (z2 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][10].v.ob[0] = camera->vx + (x3 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][10].v.ob[1] = camera->vy + (y3 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][10].v.ob[2] = camera->vz + (z3 >> 1);

        x0 = pos->vx + (-halfX.vx - yAxis2.vx);
        y0 = pos->vy + (-halfX.vy - yAxis2.vy);
        z0 = pos->vz + (-halfX.vz - yAxis2.vz);
        x1 = pos->vx + (halfX.vx - yAxis2.vx);
        y1 = pos->vy + (halfX.vy - yAxis2.vy);
        z1 = pos->vz + (halfX.vz - yAxis2.vz);
        plasmaFlareVtx[player][plasmaFlareCount][11].v.ob[0] = camera->vx + (x0 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][11].v.ob[1] = camera->vy + (y0 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][11].v.ob[2] = camera->vz + (z0 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][12].v.ob[0] = camera->vx + (x1 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][12].v.ob[1] = camera->vy + (y1 >> 1);
        plasmaFlareVtx[player][plasmaFlareCount][12].v.ob[2] = camera->vz + (z1 >> 1);

        for (i = 0; i < 13; i++) {
            if (isRed == 0) {
                plasmaFlareVtx[player][plasmaFlareCount][i].v.cn[0] = 0;
                plasmaFlareVtx[player][plasmaFlareCount][i].v.cn[1] = 127;
            } else {
                plasmaFlareVtx[player][plasmaFlareCount][i].v.cn[0] = 127;
                plasmaFlareVtx[player][plasmaFlareCount][i].v.cn[1] = 0;
            }
        }

        gSPVertex(g_glistp++, OS_K0_TO_PHYSICAL(plasmaFlareVtx[player][plasmaFlareCount]), 13, 0);
        gDPPipeSync(g_glistp++);
        gSP2Triangles(g_glistp++, 0, 1, 2, 0, 0, 7, 8, 0);
        gSP2Triangles(g_glistp++, 0, 4, 6, 0, 0, 10, 12, 0);
        gSP2Triangles(g_glistp++, 0, 5, 3, 0, 0, 11, 9, 0);
        plasmaFlareCount++;
    }
}
/* ============================================================================
 * Electric bolts, particle streak simulation/rendering, and podium rank icons
 * ============================================================================ */

void InitElectric(void) {
}

void AddElectric(VECTOR *position, s32 mode) {
    /* Unused struct locals left over from the PS1 combined add+draw routine (0x48B frame). */
    VECTOR unusedCamera;
    VECTOR unusedStart;
    VECTOR unusedEnd;
    VECTOR unusedMid;
    SVECTOR unusedVertex;
    s32 index;

    electricSlotIndex++;
    if (electricSlotIndex >= ELECTRIC_COUNT) {
        return;
    }
    index = electricSlotIndex;
    electrics[index].life = 1;
    electrics[index].mode = mode;
    if (mode == 0) {
        electrics[index].start.vx = (newRand() & 255) - 128;
        electrics[index].start.vy = (newRand() & 255) - 128;
        electrics[index].start.vz = (newRand() & 255) - 128;
        electrics[index].end.vx = electrics[index].start.vx * 4 + position->vx;
        electrics[index].end.vy = electrics[index].start.vy * 4 + position->vy;
        electrics[index].end.vz = electrics[index].start.vz * 4 + position->vz;
        electrics[index].start.vx += position->vx;
        electrics[index].start.vy += position->vy;
        electrics[index].start.vz += position->vz;
    } else {
        electrics[index].start.vx = (newRand() % 480) - 240;
        electrics[index].start.vy = position->vy;
        electrics[index].start.vz = (newRand() % 480) - 240;
        electrics[index].end.vx = electrics[index].start.vx * 4 + position->vx;
        electrics[index].end.vy = position->vy + 320;
        electrics[index].end.vz = electrics[index].start.vz * 4 + position->vz;
        electrics[index].start.vx += position->vx;
        electrics[index].start.vz += position->vz;
    }
#ifdef VERSION_EU
    CheckParticleGuard();
#endif
}

void AddElectricSegment(VECTOR *start, VECTOR *end) {
    /* Unused locals carried over from AddElectric (0x48 bytes). */
    VECTOR unusedCamera;
    VECTOR unusedStart;
    VECTOR unusedEnd;
    VECTOR unusedMid;
    SVECTOR unusedVertex;
    s32 index;

    electricSlotIndex++;
    if (electricSlotIndex >= ELECTRIC_COUNT) {
        return;
    }
    index = electricSlotIndex;
    electrics[index].life = 1;
    electrics[index].mode = 0;
    electrics[index].end.vx = end->vx;
    electrics[index].end.vy = end->vy;
    electrics[index].end.vz = end->vz;
    electrics[index].start.vx = start->vx;
    electrics[index].start.vy = start->vy;
    electrics[index].start.vz = start->vz;
#ifdef VERSION_EU
    CheckParticleGuard();
#endif
}

void DrawElectric(VECTOR *camera, s32 player) {
    VECTOR start;
    VECTOR end;
    VECTOR mid1;
    VECTOR mid2;
    s32 i;
    u8 r2, g2, b2;
    u8 r1, g1, b1;

#ifdef VERSION_EU
    gDPSetRenderMode(g_glistp++, G_RM_AA_ZB_XLU_LINE, G_RM_AA_ZB_XLU_LINE2);
    gDPPipeSync(g_glistp++);
#endif
    for (i = 0; i < ELECTRIC_COUNT; i++) {
        if (electrics[i].life == -1) {
            continue;
        }
        start.vx = electrics[i].start.vx;
        start.vy = electrics[i].start.vy;
        start.vz = electrics[i].start.vz;
#ifdef VERSION_EU
        {
            s32 dx = start.vx + (camera->vx << 1);
            s32 dy = start.vy + (camera->vy << 1);
            s32 dz = start.vz + (camera->vz << 1);
            dx = __builtin_abs(dx);
            dy = __builtin_abs(dy);
            dz = __builtin_abs(dz);
            if (dx >= 0x4000 || dy >= 0x4000 || dz >= 0x4000) {
                continue;
            }
        }
#else
        {
            s32 dx = start.vx + (camera->vx << 1);
            s32 dy = start.vy + (camera->vy << 1);
            s32 dz = start.vz + (camera->vz << 1);
            if (dx * dx + dy * dy + dz * dz > 0x0FFFFFFF) {
                continue;
            }
        }
#endif
        end.vx = electrics[i].end.vx;
        end.vy = electrics[i].end.vy;
        end.vz = electrics[i].end.vz;

        mid1.vx = (start.vx + end.vx) >> 1;
        mid1.vy = (start.vy + end.vy) >> 1;
        mid1.vz = (start.vz + end.vz) >> 1;
        mid1.vx += (newRand() & 127) - 63;
        mid1.vy += (newRand() & 127) - 63;
        mid1.vz += (newRand() & 127) - 63;

        mid2.vx = (start.vx + end.vx) >> 1;
        mid2.vy = (start.vy + end.vy) >> 1;
        mid2.vz = (start.vz + end.vz) >> 1;
        mid2.vx += (newRand() & 127) - 63;
        mid2.vy += (newRand() & 127) - 63;
        mid2.vz += (newRand() & 127) - 63;

#ifdef VERSION_EU
        electrics[i].vtx[player][0].v.ob[0] = camera->vx + (start.vx >> 1);
        electrics[i].vtx[player][0].v.ob[1] = camera->vy + (start.vy >> 1);
        electrics[i].vtx[player][0].v.ob[2] = camera->vz + (start.vz >> 1);

        electrics[i].vtx[player][2].v.ob[0] = camera->vx + ((start.vx + 10) >> 1);
        electrics[i].vtx[player][2].v.ob[1] = camera->vy + ((start.vy + 10) >> 1);
        electrics[i].vtx[player][2].v.ob[2] = camera->vz + ((start.vz + 10) >> 1);

        electrics[i].vtx[player][1].v.ob[0] = camera->vx + (mid1.vx >> 1);
        electrics[i].vtx[player][1].v.ob[1] = camera->vy + (mid1.vy >> 1);
        electrics[i].vtx[player][1].v.ob[2] = camera->vz + (mid1.vz >> 1);

        electrics[i].vtx[player][3].v.ob[0] = camera->vx + (mid2.vx >> 1);
        electrics[i].vtx[player][3].v.ob[1] = camera->vy + (mid2.vy >> 1);
        electrics[i].vtx[player][3].v.ob[2] = camera->vz + (mid2.vz >> 1);

        electrics[i].vtx[player][4].v.ob[0] = camera->vx + ((mid2.vx + 10) >> 1);
        electrics[i].vtx[player][4].v.ob[1] = camera->vy + ((mid2.vy + 10) >> 1);
        electrics[i].vtx[player][4].v.ob[2] = camera->vz + ((mid2.vz + 10) >> 1);

        electrics[i].vtx[player][5].v.ob[0] = camera->vx + (end.vx >> 1);
        electrics[i].vtx[player][5].v.ob[1] = camera->vy + (end.vy >> 1);
        electrics[i].vtx[player][5].v.ob[2] = camera->vz + (end.vz >> 1);

        if (electrics[i].mode == 2) {
            r1 = 192;
            g1 = 0;
            b1 = 0;
            r2 = 255;
            g2 = 250;
            b2 = 0;
        } else {
            r1 = 0;
            g1 = 0;
            b1 = 255;
            r2 = 255;
            g2 = 255;
            b2 = 255;
        }

        electrics[i].vtx[player][0].v.cn[0] = r1;
        electrics[i].vtx[player][0].v.cn[1] = g1;
        electrics[i].vtx[player][0].v.cn[2] = b1;
        electrics[i].vtx[player][0].v.cn[3] = 191;

        electrics[i].vtx[player][2].v.cn[0] = r1;
        electrics[i].vtx[player][2].v.cn[1] = g1;
        electrics[i].vtx[player][2].v.cn[2] = b1;
        electrics[i].vtx[player][2].v.cn[3] = 191;

        electrics[i].vtx[player][1].v.cn[0] = (r1 + r2) >> 1;
        electrics[i].vtx[player][1].v.cn[1] = (g1 + g2) >> 1;
        electrics[i].vtx[player][1].v.cn[2] = (b1 + b2) >> 1;
        electrics[i].vtx[player][1].v.cn[3] = 191;

        electrics[i].vtx[player][3].v.cn[0] = (r1 + r2) >> 1;
        electrics[i].vtx[player][3].v.cn[1] = (g1 + g2) >> 1;
        electrics[i].vtx[player][3].v.cn[2] = (b1 + b2) >> 1;
        electrics[i].vtx[player][3].v.cn[3] = 191;

        electrics[i].vtx[player][4].v.cn[0] = (r1 + r2) >> 1;
        electrics[i].vtx[player][4].v.cn[1] = (g1 + g2) >> 1;
        electrics[i].vtx[player][4].v.cn[2] = (b1 + b2) >> 1;
        electrics[i].vtx[player][4].v.cn[3] = 191;

        electrics[i].vtx[player][5].v.cn[0] = r2;
        electrics[i].vtx[player][5].v.cn[1] = g2;
        electrics[i].vtx[player][5].v.cn[2] = b2;
        electrics[i].vtx[player][5].v.cn[3] = 191;

        gSPVertex(g_glistp++, OS_K0_TO_PHYSICAL(electrics[i].vtx[player]), 6, 0);
        gSP1Triangle(g_glistp++, 0, 1, 2, 0);
        gSP1Triangle(g_glistp++, 3, 4, 1, 0);
        gSP1Triangle(g_glistp++, 4, 3, 5, 0);
#else
        electrics[i].vtx[player][0].v.ob[0] = camera->vx + (start.vx >> 1);
        electrics[i].vtx[player][0].v.ob[1] = camera->vy + (start.vy >> 1);
        electrics[i].vtx[player][0].v.ob[2] = camera->vz + (start.vz >> 1);

        electrics[i].vtx[player][1].v.ob[0] = camera->vx + (mid1.vx >> 1);
        electrics[i].vtx[player][1].v.ob[1] = camera->vy + (mid1.vy >> 1);
        electrics[i].vtx[player][1].v.ob[2] = camera->vz + (mid1.vz >> 1);

        electrics[i].vtx[player][2].v.ob[0] = camera->vx + (mid2.vx >> 1);
        electrics[i].vtx[player][2].v.ob[1] = camera->vy + (mid2.vy >> 1);
        electrics[i].vtx[player][2].v.ob[2] = camera->vz + (mid2.vz >> 1);

        electrics[i].vtx[player][3].v.ob[0] = camera->vx + (end.vx >> 1);
        electrics[i].vtx[player][3].v.ob[1] = camera->vy + (end.vy >> 1);
        electrics[i].vtx[player][3].v.ob[2] = camera->vz + (end.vz >> 1);

        if (electrics[i].mode == 2) {
            r1 = 192;
            g1 = 0;
            b1 = 0;
            r2 = 255;
            g2 = 250;
            b2 = 0;
        } else {
            r1 = 0;
            g1 = 0;
            b1 = 255;
            r2 = 255;
            g2 = 255;
            b2 = 255;
        }

        electrics[i].vtx[player][0].v.cn[0] = r1;
        electrics[i].vtx[player][0].v.cn[1] = g1;
        electrics[i].vtx[player][0].v.cn[2] = b1;
        electrics[i].vtx[player][0].v.cn[3] = 191;

        electrics[i].vtx[player][1].v.cn[0] = (r1 + r2) >> 1;
        electrics[i].vtx[player][1].v.cn[1] = (g1 + g2) >> 1;
        electrics[i].vtx[player][1].v.cn[2] = (b1 + b2) >> 1;
        electrics[i].vtx[player][1].v.cn[3] = 191;

        electrics[i].vtx[player][2].v.cn[0] = (r1 + r2) >> 1;
        electrics[i].vtx[player][2].v.cn[1] = (g1 + g2) >> 1;
        electrics[i].vtx[player][2].v.cn[2] = (b1 + b2) >> 1;
        electrics[i].vtx[player][2].v.cn[3] = 191;

        electrics[i].vtx[player][3].v.cn[0] = r2;
        electrics[i].vtx[player][3].v.cn[1] = g2;
        electrics[i].vtx[player][3].v.cn[2] = b2;
        electrics[i].vtx[player][3].v.cn[3] = 191;

        gSPVertex(g_glistp++, OS_K0_TO_PHYSICAL(electrics[i].vtx[player]), 4, 0);
        gDPSetRenderMode(g_glistp++, G_RM_AA_ZB_XLU_LINE, G_RM_AA_ZB_XLU_LINE2);
        gDPPipeSync(g_glistp++);
        gSPLine3D(g_glistp++, 0, 1, 0);
        gSPLine3D(g_glistp++, 2, 1, 0);
        gSPLine3D(g_glistp++, 3, 2, 0);
#endif
    }
#ifdef VERSION_EU
    CheckParticleGuard();
#endif
}

void resetMiscFx(void) {
    electricSlotIndex = 0;
    rocketFlareCount = 0;
    missileFlareCount = 0;
    plasmaFlareCount = 0;
}

void UpdateParticles(VECTOR *camera, SParticle *particles, s32 player) {
    s32 i;
    s32 curX, curY, curZ;
    s32 prevX, prevY, prevZ;
    s32 vx, vy, vz;
    s32 life;
#ifdef VERSION_EU
    s32 dx, dy, dz;

    gDPSetRenderMode(g_glistp++, G_RM_AA_ZB_XLU_LINE, G_RM_AA_ZB_XLU_LINE2);
    gDPPipeSync(g_glistp++);
#endif

    for (i = 0; i < PARTICLE_COUNT; i++) {
        if (particles[i].type == 0) {
            continue;
        }
        life = particles[i].life;
        vx = particles[i].vel.vx;
        vy = particles[i].vel.vy;
        vz = particles[i].vel.vz;
        prevX = particles[i].prevPos.vx = particles[i].pos.vx;
        prevY = particles[i].prevPos.vy = particles[i].pos.vy;
        prevZ = particles[i].prevPos.vz = particles[i].pos.vz;
        particles[i].pos.vx += vx;
        particles[i].pos.vy += vy;
        particles[i].pos.vz += vz;
        curX = particles[i].pos.vx;
        curY = particles[i].pos.vy;
        curZ = particles[i].pos.vz;
        particles[i].life = life - 1;
        if (life > 0) {
            life <<= 5;
            life -= 255;
            if (life >= 0) {
                life = 255;
            }

            switch (particles[i].type) {
                case 1:
                    particles[i].vtx[player][0].v.cn[0] = life;
                    particles[i].vtx[player][0].v.cn[1] = life;
                    particles[i].vtx[player][0].v.cn[2] = life;
                    particles[i].vtx[player][0].v.cn[3] = 127;
#ifdef VERSION_EU
                    particles[i].vtx[player][2].v.cn[0] = life;
                    particles[i].vtx[player][2].v.cn[1] = life;
                    particles[i].vtx[player][2].v.cn[2] = life;
                    particles[i].vtx[player][2].v.cn[3] = 127;
#endif
                    life >>= 1;
                    particles[i].vtx[player][1].v.cn[0] = life;
                    particles[i].vtx[player][1].v.cn[1] = life;
                    particles[i].vtx[player][1].v.cn[2] = life;
                    particles[i].vtx[player][1].v.cn[3] = 31;
                    break;

                case 2:
                    particles[i].vtx[player][0].v.cn[0] = life;
#ifdef VERSION_EU
                    particles[i].vtx[player][2].v.cn[0] = life;
#endif
                    life >>= 2;
#ifdef VERSION_EU
                    curX += vx;
                    curY += vy;
                    curZ += vz;
#endif
                    particles[i].vtx[player][0].v.cn[1] = life;
                    particles[i].vtx[player][0].v.cn[2] = 0;
                    particles[i].vtx[player][0].v.cn[3] = 127;
#ifdef VERSION_EU
                    particles[i].vtx[player][2].v.cn[1] = life;
                    particles[i].vtx[player][2].v.cn[2] = 0;
                    particles[i].vtx[player][2].v.cn[3] = 127;
#endif
                    particles[i].vtx[player][1].v.cn[0] = life;
                    particles[i].vtx[player][1].v.cn[1] = 0;
                    particles[i].vtx[player][1].v.cn[2] = 0;
                    particles[i].vtx[player][1].v.cn[3] = 31;
                    break;

                case 3:
                    particles[i].vtx[player][0].v.cn[0] = 255;
                    particles[i].vtx[player][0].v.cn[1] = 255;
                    particles[i].vtx[player][0].v.cn[2] = 255;
                    particles[i].vtx[player][0].v.cn[3] = 255;
#ifdef VERSION_EU
                    particles[i].vtx[player][2].v.cn[0] = 255;
                    particles[i].vtx[player][2].v.cn[1] = 255;
                    particles[i].vtx[player][2].v.cn[2] = 255;
                    particles[i].vtx[player][2].v.cn[3] = 255;
#endif
                    particles[i].vtx[player][1].v.cn[0] = 0;
                    particles[i].vtx[player][1].v.cn[1] = 0;
                    particles[i].vtx[player][1].v.cn[2] = 255;
                    particles[i].vtx[player][1].v.cn[3] = 127;
                    break;

                case 4:
                    particles[i].vtx[player][0].v.cn[0] = life >> 2;
                    particles[i].vtx[player][0].v.cn[1] = life >> 2;
                    particles[i].vtx[player][0].v.cn[2] = life;
                    particles[i].vtx[player][0].v.cn[3] = 127;
#ifdef VERSION_EU
                    particles[i].vtx[player][2].v.cn[0] = life >> 2;
                    particles[i].vtx[player][2].v.cn[1] = life >> 2;
                    particles[i].vtx[player][2].v.cn[2] = life;
                    particles[i].vtx[player][2].v.cn[3] = 127;
#endif
                    life >>= 2;
                    particles[i].vtx[player][1].v.cn[0] = life >> 2;
                    particles[i].vtx[player][1].v.cn[1] = life >> 2;
                    particles[i].vtx[player][1].v.cn[2] = life;
                    particles[i].vtx[player][1].v.cn[3] = 31;
                    break;

                case 5:
                    particles[i].vtx[player][0].v.cn[0] = 96;
                    particles[i].vtx[player][0].v.cn[1] = 96;
                    particles[i].vtx[player][0].v.cn[2] = 96;
                    particles[i].vtx[player][0].v.cn[3] = 127;
#ifdef VERSION_EU
                    particles[i].vtx[player][2].v.cn[0] = 96;
                    particles[i].vtx[player][2].v.cn[1] = 96;
                    particles[i].vtx[player][2].v.cn[2] = 96;
                    particles[i].vtx[player][2].v.cn[3] = 127;
#endif
                    particles[i].vtx[player][1].v.cn[0] = 1;
                    particles[i].vtx[player][1].v.cn[1] = 1;
                    particles[i].vtx[player][1].v.cn[2] = 1;
                    particles[i].vtx[player][1].v.cn[3] = 31;
                    break;
            }

#ifdef VERSION_EU
            dx = prevX + (camera->vx << 1);
            dy = prevY + (camera->vy << 1);
            dz = prevZ + (camera->vz << 1);
            dx = __builtin_abs(dx);
            dy = __builtin_abs(dy);
            dz = __builtin_abs(dz);
            if (dx < 0x4000 && dy < 0x4000 && dz < 0x4000)
#endif
            {
                u8 wasActive;

                particles[i].vtx[player][0].v.ob[0] = camera->vx + (curX >> 1);
                particles[i].vtx[player][0].v.ob[1] = camera->vy + (curY >> 1);
                particles[i].vtx[player][0].v.ob[2] = camera->vz + (curZ >> 1);
#ifdef VERSION_EU
                particles[i].vtx[player][2].v.ob[0] = camera->vx + ((curX + 20) >> 1);
                particles[i].vtx[player][2].v.ob[1] = camera->vy + (curY >> 1);
                particles[i].vtx[player][2].v.ob[2] = camera->vz + ((curZ + 20) >> 1);
#endif
                particles[i].vtx[player][1].v.ob[0] = camera->vx + (prevX >> 1);
                particles[i].vtx[player][1].v.ob[1] = camera->vy + (prevY >> 1);
                particles[i].vtx[player][1].v.ob[2] = camera->vz + (prevZ >> 1);

                wasActive = particles[i].flags;
                particles[i].flags = 1;
                if (wasActive != 0) {
#ifdef VERSION_EU
                    gSPVertex(g_glistp++, OS_K0_TO_PHYSICAL(particles[i].vtx[player]), 3, 0);
                    gSP1Triangle(g_glistp++, 0, 1, 2, 0);
#else
                    gSPVertex(g_glistp++, OS_K0_TO_PHYSICAL(particles[i].vtx[player]), 2, 0);
                    gDPSetRenderMode(g_glistp++, G_RM_AA_ZB_XLU_LINE, G_RM_AA_ZB_XLU_LINE2);
                    gDPPipeSync(g_glistp++);
                    gSPLine3D(g_glistp++, 0, 1, 0);
#endif
                }
            }
        } else {
            particles[i].flags = 0;
            particles[i].type = 0;
        }
    }
}

/* Fast 3D distance approximation: max(|x|, |y|, |z|) + 1/4 of the other two axes. */
s32 ApproxDistance3D(s32 x, s32 y, s32 z) {
    s32 maxVal;

    if (x < 0) {
        x = -x;
    }
    if (y < 0) {
        y = -y;
    }
    if (z < 0) {
        z = -z;
    }
    maxVal = (x >= y) ? x : y;
    maxVal = (z >= maxVal) ? z : maxVal;
    if (maxVal == x) {
        return x + (y >> 2) + (z >> 2);
    }
    if (maxVal == y) {
        return y + (x >> 2) + (z >> 2);
    }
    return z + (x >> 2) + (y >> 2);
}

void DrawPodiumIcons(VECTOR *camera, MATRIX *view, s32 player) {
    s32 i;
    s32 j;
    s32 shipNo;
    Object *shape;
    MATRIX *mat;
    s32 dx, dy, dz;
    s32 dist;
    s32 alpha;

    for (i = 0; i < PODIUM_ICON_COUNT; i++) {
        shipNo = g_podiumShips[i];
        if (shipNo == -1) {
            continue;
        }
        if (ships[shipNo].combatAttr & SHIP_COMBAT_DESTROYED) {
            continue;
        }
        if (!ships[shipNo].visible[player]) {
            continue;
        }
        if (pauseRequests[player] == 1) {
            continue;
        }
        shape = shipShapes[ships[shipNo].highResTableIndex];
        shape->skeleton->super = cameras[player].camPos;
        SetSkeletonPosition(shape->skeleton, ships[shipNo].ppivot.vx >> 1,
                            ships[shipNo].ppivot.vy >> 1, ships[shipNo].ppivot.vz >> 1);
        SetSkeletonDirectionHPR(shape->skeleton, ships[shipNo].hdg, ships[shipNo].pitch,
                                ships[shipNo].roll);
        mat = GetSkeletonMatrix(shape->skeleton);
        MatrixToN64Mtx(mat, &g_mtxPool[g_mtxIndex]);
        gSPMatrix(g_glistp++, OS_K0_TO_PHYSICAL(&g_mtxPool[g_mtxIndex]),
                  G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
        g_mtxIndex++;
        gSPDisplayList(g_glistp++, g_textureTable[shadow2TextureStart + i + 1]->dl);
        dx = ships[shipNo].ppivot.vx + (cameras[player].camPos->relative.t[0] << 1);
        dy = ships[shipNo].ppivot.vy + (cameras[player].camPos->relative.t[1] << 1);
        dz = ships[shipNo].ppivot.vz + (cameras[player].camPos->relative.t[2] << 1);
        dist = dx * dx + dy * dy + dz * dz;
        if (dist > 0x2FFFFFFF) {
            continue;
        }
        podiumVtx[player][i][0].v.ob[0] = -400;
        podiumVtx[player][i][0].v.ob[1] = -850;
        podiumVtx[player][i][0].v.ob[2] = 0;
        if (frontendOptions_g.numPlayers == 1) {
            podiumVtx[player][i][1].v.ob[0] = 400;
            podiumVtx[player][i][1].v.ob[1] = -850;
            podiumVtx[player][i][1].v.ob[2] = 0;
            podiumVtx[player][i][2].v.ob[0] = 400;
            podiumVtx[player][i][2].v.ob[1] = -50;
            podiumVtx[player][i][2].v.ob[2] = 0;
            podiumVtx[player][i][3].v.ob[0] = -400;
            podiumVtx[player][i][3].v.ob[1] = -50;
            podiumVtx[player][i][3].v.ob[2] = 0;
        } else {
            podiumVtx[player][i][0].v.ob[0] = -600;
            podiumVtx[player][i][0].v.ob[1] = -1075;
            podiumVtx[player][i][0].v.ob[2] = 0;
            podiumVtx[player][i][1].v.ob[0] = 600;
            podiumVtx[player][i][1].v.ob[1] = -1075;
            podiumVtx[player][i][1].v.ob[2] = 0;
            podiumVtx[player][i][2].v.ob[0] = 600;
            podiumVtx[player][i][2].v.ob[1] = -50;
            podiumVtx[player][i][2].v.ob[2] = 0;
            podiumVtx[player][i][3].v.ob[0] = -600;
            podiumVtx[player][i][3].v.ob[1] = -50;
            podiumVtx[player][i][3].v.ob[2] = 0;
        }
        dist = ApproxDistance3D(dx, dy, dz);
        if (dist <= 0x1000) {
            alpha = 0;
        } else if (dist < 0x4000) {
            alpha = (dist - 0x1000) / 48;
        } else {
            alpha = 255;
        }
        for (j = 0; j < 4; j++) {
            podiumVtx[player][i][j].v.cn[0] = 255;
            podiumVtx[player][i][j].v.cn[1] = 255;
            podiumVtx[player][i][j].v.cn[2] = 255;
            podiumVtx[player][i][j].v.cn[3] = alpha;
        }
        gSPVertex(g_glistp++, OS_K0_TO_PHYSICAL(podiumVtx[player][i]), 4, 0);
        gDPPipeSync(g_glistp++);
        gSP2Triangles(g_glistp++, 0, 1, 2, 0, 0, 3, 2, 0);
    }
}

void ClearCycloneCharges(void) {
    s32 i;

    for (i = CYCLONE_CHARGE_COUNT - 1; i >= 0; i--) {
        cycloneCharges[i].active = 0;
    }
}
