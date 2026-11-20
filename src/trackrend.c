#include "trackrend.h"

#ident "$Revision: 1.34 $"
#ident "$Revision: 1.5 $"

void InitSky(void) {
    s32 trackIdx;
    s32 i;

    trackIdx = frontendOptions_g.trackNo - 1;
    /* Retail preserves a basic-block barrier here (e.g. compiled-out assert/loop). */
    do {
    } while (0);

    for (i = 0; i < 21; i++) {
        g_skyCloudVtx[i].v.ob[0] = g_skyBaseVerts[i].vx;
        g_skyCloudVtx[i].v.ob[1] = g_skyBaseVerts[i].vy;
        g_skyCloudVtx[i].v.ob[2] = g_skyBaseVerts[i].vz;
    }

    for (i = 0; i < 14; i++) {
        g_skyLowerBandVtx[i].v.ob[0] = g_skyUpperBandVtx[i].v.ob[0] = g_skyBaseVerts[i].vx;
        g_skyLowerBandVtx[i].v.ob[2] = g_skyUpperBandVtx[i].v.ob[2] = g_skyBaseVerts[i].vz;

        if (i < 7) {
            g_skyUpperBandVtx[i].v.ob[1] = g_skyBaseVerts[i].vy - 144;
            g_skyUpperBandVtx[i].v.cn[0] = g_skyTopColours[trackIdx].r;
            g_skyUpperBandVtx[i].v.cn[1] = g_skyTopColours[trackIdx].g;
            g_skyUpperBandVtx[i].v.cn[2] = g_skyTopColours[trackIdx].b;
            g_skyUpperBandVtx[i].v.cn[3] = 255;

            g_skyHorizonVtx[i].v.ob[0] = g_skyBaseVerts[i].vx;
            g_skyHorizonVtx[i].v.ob[1] = g_skyBaseVerts[i].vy - 144;
            g_skyHorizonVtx[i].v.ob[2] = g_skyBaseVerts[i].vz;
            g_skyHorizonVtx[i].v.cn[0] = 0;
            g_skyHorizonVtx[i].v.cn[1] = 60;
            g_skyHorizonVtx[i].v.cn[2] = 51;
            g_skyHorizonVtx[i].v.cn[3] = 127;
        } else {
            g_skyUpperBandVtx[i].v.ob[1] = g_skyBaseVerts[i].vy - 72;
            g_skyUpperBandVtx[i].v.cn[0] = g_skyMidTopColours[trackIdx].r;
            g_skyUpperBandVtx[i].v.cn[1] = g_skyMidTopColours[trackIdx].g;
            g_skyUpperBandVtx[i].v.cn[2] = g_skyMidTopColours[trackIdx].b;
            g_skyUpperBandVtx[i].v.cn[3] = 255;

            g_skyHorizonVtx[i].v.ob[0] = g_skyBaseVerts[i].vx;
            g_skyHorizonVtx[i].v.ob[1] = g_skyBaseVerts[i].vy + 100;
            g_skyHorizonVtx[i].v.ob[2] = g_skyBaseVerts[i].vz;
            g_skyHorizonVtx[i].v.cn[0] = 0;
            g_skyHorizonVtx[i].v.cn[1] = 0;
            g_skyHorizonVtx[i].v.cn[2] = 0;
            g_skyHorizonVtx[i].v.cn[3] = 255;
        }

        if (i < 7) {
            g_skyLowerBandVtx[i].v.ob[1] = g_skyBaseVerts[i + 7].vy + 72;
            g_skyLowerBandVtx[i].v.cn[0] = g_skyMidBotColours[trackIdx].r;
            g_skyLowerBandVtx[i].v.cn[1] = g_skyMidBotColours[trackIdx].g;
            g_skyLowerBandVtx[i].v.cn[2] = g_skyMidBotColours[trackIdx].b;
            g_skyLowerBandVtx[i].v.cn[3] = 255;
        } else {
            g_skyLowerBandVtx[i].v.ob[1] = g_skyBaseVerts[i + 7].vy + 144;
            g_skyLowerBandVtx[i].v.cn[0] = g_skyBotColours[trackIdx].r;
            g_skyLowerBandVtx[i].v.cn[1] = g_skyBotColours[trackIdx].g;
            g_skyLowerBandVtx[i].v.cn[2] = g_skyBotColours[trackIdx].b;
            g_skyLowerBandVtx[i].v.cn[3] = 255;
        }
    }
}

s16 GetSkyFacing(s16 angle) {
    if (angle >= -512 && angle < 512) {
        return SKY_DIR_NORTH;
    } else if (angle >= 512 && angle < 1536) {
        return SKY_DIR_WEST;
    } else if (angle >= 1536 || angle < -1536) {
        return SKY_DIR_SOUTH;
    }
    return SKY_DIR_EAST;
}

Gfx *DrawSkyHighRes(TrackCamera *cam, Gfx *glistp) {
    Mtx rotMtx;
    Gfx *dlStart;
    s32 savedX;
    s32 savedY;
    s32 savedZ;
    s32 vtxOffset;
    Mtx *mtx;
    s32 texStart;
    s32 baseTexIdx;
    f32 angle;
    s32 row;
    s32 col;
    s32 texIdx;
    s32 rowBaseTex;
    s32 v1, v2, v3;
    s32 dlLen;

    switch (GetSkyFacing(-cam->hdg)) {
        case SKY_DIR_NORTH:
            angle = 0.0f;
            break;
        case SKY_DIR_WEST:
            angle = 270.0f;
            break;
        case SKY_DIR_SOUTH:
            angle = 180.0f;
            break;
        default:
            angle = 90.0f;
            break;
    }

    dlStart = g_skyGfxPtr;
    vtxOffset = 0;
    ResetRenderList(&g_skyRenderList);

    savedX = cam->camPos->relative.t[0];
    savedY = cam->camPos->relative.t[1];
    savedZ = cam->camPos->relative.t[2];
    guRotate(&rotMtx, angle, 0.0f, 1.0f, 0.0f);

    SetSkeletonPosition(cam->camPos, 0, g_skyOffsetY, 0);
    cam->camPos->update = 1;
    MatrixToN64Mtx(GetSkeletonMatrix(cam->camPos), (N64Mtx *)&g_mtxPool[g_mtxIndex]);
    mtx = &g_mtxPool[g_mtxIndex];
    guMtxCatL(&rotMtx, mtx, mtx);
    g_mtxIndex++;
    texStart = g_skyRenderList.textureStart;

    gDPPipeSync(g_skyGfxPtr++);
    gSPClearGeometryMode(g_skyGfxPtr++, G_SHADING_SMOOTH);
    gDPSetCombineMode(g_skyGfxPtr++, G_CC_DECALRGBA, G_CC_DECALRGBA);

    baseTexIdx = texStart + 1;

    for (row = 0; row < 2; row++) {
        texIdx = baseTexIdx - row;
        rowBaseTex = texIdx;
        gSPVertex(g_skyGfxPtr++, OS_K0_TO_PHYSICAL(&g_skyCloudVtx[vtxOffset]), 26, 0);

        for (col = 0; col < 6; col++) {
            v1 = col + 1;
            v2 = col + 13;
            v3 = col + 14;
            gSPDisplayList(g_skyGfxPtr++, g_textureTable[texIdx]->dl);

            g_skyGfxPtr = ModifyVertexST(g_skyGfxPtr, col, 0, 0, texIdx);
            g_skyGfxPtr = ModifyVertexST(g_skyGfxPtr, v1, 55, 0, texIdx);
            g_skyGfxPtr = ModifyVertexST(g_skyGfxPtr, v2, 0, 35, texIdx);
            g_skyGfxPtr = ModifyVertexST(g_skyGfxPtr, v3, 55, 35, texIdx);
            gSP2Triangles(g_skyGfxPtr++, col, v1, v2, 0, v2, v1, v3, 0);

            g_skyGfxPtr = ModifyVertexST(g_skyGfxPtr, col + 6, 0, 0, texIdx);
            g_skyGfxPtr = ModifyVertexST(g_skyGfxPtr, col + 7, 55, 0, texIdx);
            g_skyGfxPtr = ModifyVertexST(g_skyGfxPtr, col + 19, 0, 35, texIdx);
            g_skyGfxPtr = ModifyVertexST(g_skyGfxPtr, col + 20, 55, 35, texIdx);
            gSP2Triangles(g_skyGfxPtr++, col + 6, col + 7, col + 19, 0, col + 19, col + 7, col + 20, 0);

            texIdx += 2;
            if (texIdx > rowBaseTex + 10) {
                texIdx = rowBaseTex;
            }
        }
        vtxOffset += 13;
    }

    gDPPipeSync(g_skyGfxPtr++);
    gSPTexture(g_skyGfxPtr++, 0, 0, 0, G_TX_RENDERTILE, G_OFF);
    gSPSetGeometryMode(g_skyGfxPtr++, G_SHADING_SMOOTH);
    gDPSetCombineMode(g_skyGfxPtr++, G_CC_SHADE, G_CC_SHADE);

    gSPVertex(g_skyGfxPtr++, OS_K0_TO_PHYSICAL(g_skyUpperBandVtx), 26, 0);
    for (col = 0; col < 12; col++) {
        v1 = col + 1;
        v2 = col + 13;
        v3 = col + 14;
        gSP2Triangles(g_skyGfxPtr++, col, v1, v2, 0, v2, v1, v3, 0);
    }

    gSPVertex(g_skyGfxPtr++, OS_K0_TO_PHYSICAL(g_skyLowerBandVtx), 26, 0);
    for (col = 0; col < 12; col++) {
        v1 = col + 1;
        v2 = col + 13;
        v3 = col + 14;
        gSP2Triangles(g_skyGfxPtr++, col, v1, v2, 0, v2, v1, v3, 0);
    }

    gSPEndDisplayList(g_skyGfxPtr++);
    dlLen = g_skyGfxPtr - g_skyDlPool;
    g_skyDlCount += dlLen;

    SetSkeletonPosition(cam->camPos, savedX, savedY, savedZ);
    AddRenderDisplayList(&g_skyRenderList, &dlStart, mtx);
    glistp = FlushSkyRenderList(&g_skyRenderList, glistp);
    return glistp;
}

Gfx *DrawSky(TrackCamera *cam, Gfx *glistp) {
    Mtx rotMtx;
    Gfx *dlStart;
    s32 savedX;
    s32 savedY;
    s32 savedZ;
    s32 vtxOffset;
    Mtx *mtx;
    s32 texStart;
    s32 baseTexIdx;
    f32 angle;
    s32 row;
    s32 col;
    s32 texIdx;
    s32 rowBaseTex;
    s32 v1, v2, v3;
    s32 dlLen;

    switch (GetSkyFacing(-cam->hdg)) {
        case SKY_DIR_NORTH:
            angle = 0.0f;
            break;
        case SKY_DIR_WEST:
            angle = 270.0f;
            break;
        case SKY_DIR_SOUTH:
            angle = 180.0f;
            break;
        default:
            angle = 90.0f;
            break;
    }

    dlStart = g_skyGfxPtr;
    vtxOffset = 0;
    ResetRenderList(&g_skyRenderList);

    savedX = cam->camPos->relative.t[0];
    savedY = cam->camPos->relative.t[1];
    savedZ = cam->camPos->relative.t[2];
    guRotate(&rotMtx, angle, 0.0f, 1.0f, 0.0f);

    SetSkeletonPosition(cam->camPos, 0, g_skyOffsetY, 0);
    cam->camPos->update = 1;
    MatrixToN64Mtx(GetSkeletonMatrix(cam->camPos), (N64Mtx *)&g_mtxPool[g_mtxIndex]);
    mtx = &g_mtxPool[g_mtxIndex];
    guMtxCatL(&rotMtx, mtx, mtx);
    g_mtxIndex++;
    texStart = g_skyRenderList.textureStart;

    gDPPipeSync(g_skyGfxPtr++);
    gSPClearGeometryMode(g_skyGfxPtr++, G_SHADING_SMOOTH);
    gDPSetCombineMode(g_skyGfxPtr++, G_CC_DECALRGBA, G_CC_DECALRGBA);

    baseTexIdx = texStart + 13;

    for (row = 0; row < 2; row++) {
        texIdx = baseTexIdx - row;
        rowBaseTex = texIdx;
        gSPVertex(g_skyGfxPtr++, OS_K0_TO_PHYSICAL(&g_skyCloudVtx[vtxOffset]), 26, 0);

        for (col = 0; col < 3; col++) {
            v1 = col + 1;
            v2 = col + 7;
            v3 = col + 8;
            gSPDisplayList(g_skyGfxPtr++, g_textureTable[texIdx]->dl);

            g_skyGfxPtr = ModifyVertexST(g_skyGfxPtr, col, 0, 0, texIdx);
            g_skyGfxPtr = ModifyVertexST(g_skyGfxPtr, v1, 55, 0, texIdx);
            g_skyGfxPtr = ModifyVertexST(g_skyGfxPtr, v2, 0, 35, texIdx);
            g_skyGfxPtr = ModifyVertexST(g_skyGfxPtr, v3, 55, 35, texIdx);
            gSP2Triangles(g_skyGfxPtr++, col, v1, v2, 0, v2, v1, v3, 0);

            g_skyGfxPtr = ModifyVertexST(g_skyGfxPtr, col + 3, 0, 0, texIdx);
            g_skyGfxPtr = ModifyVertexST(g_skyGfxPtr, col + 4, 55, 0, texIdx);
            g_skyGfxPtr = ModifyVertexST(g_skyGfxPtr, col + 10, 0, 35, texIdx);
            g_skyGfxPtr = ModifyVertexST(g_skyGfxPtr, col + 11, 55, 35, texIdx);
            gSP2Triangles(g_skyGfxPtr++, col + 3, col + 4, col + 10, 0, col + 10, col + 4, col + 11, 0);

            texIdx += 2;
            if (texIdx > rowBaseTex + 4) {
                texIdx = rowBaseTex;
            }
        }
        vtxOffset += 7;
    }

    gDPPipeSync(g_skyGfxPtr++);
    gSPTexture(g_skyGfxPtr++, 0, 0, 0, G_TX_RENDERTILE, G_OFF);
    gSPSetGeometryMode(g_skyGfxPtr++, G_SHADING_SMOOTH);
    gDPSetCombineMode(g_skyGfxPtr++, G_CC_SHADE, G_CC_SHADE);

    gSPVertex(g_skyGfxPtr++, OS_K0_TO_PHYSICAL(g_skyUpperBandVtx), 14, 0);
    for (col = 0; col < 6; col++) {
        v1 = col + 1;
        v2 = col + 7;
        v3 = col + 8;
        gSP2Triangles(g_skyGfxPtr++, col, v1, v2, 0, v2, v1, v3, 0);
    }

    gSPVertex(g_skyGfxPtr++, OS_K0_TO_PHYSICAL(g_skyLowerBandVtx), 14, 0);
    for (col = 0; col < 6; col++) {
        v1 = col + 1;
        v2 = col + 7;
        v3 = col + 8;
        gSP2Triangles(g_skyGfxPtr++, col, v1, v2, 0, v2, v1, v3, 0);
    }

    gSPEndDisplayList(g_skyGfxPtr++);
    dlLen = g_skyGfxPtr - g_skyDlPool;
    g_skyDlCount += dlLen;

    SetSkeletonPosition(cam->camPos, savedX, savedY, savedZ);
    AddRenderDisplayList(&g_skyRenderList, &dlStart, mtx);
    glistp = FlushSkyRenderList(&g_skyRenderList, glistp);
    return glistp;
}

Gfx *DrawSkyHorizon(TrackCamera *cam, Gfx *glistp) {
    Mtx rotMtx;
    Gfx *dlStart;
    s32 savedX;
    s32 savedY;
    s32 savedZ;
    Mtx *mtx;
    f32 angle;
    s32 col;
    s32 v1, v2, v3;
    s32 dlLen;

    switch (GetSkyFacing(-cam->hdg)) {
        case SKY_DIR_NORTH:
            angle = 0.0f;
            break;
        case SKY_DIR_WEST:
            angle = 270.0f;
            break;
        case SKY_DIR_SOUTH:
            angle = 180.0f;
            break;
        default:
            angle = 90.0f;
            break;
    }

    dlStart = g_skyGfxPtr;
    ResetRenderList(&g_skyRenderList);

    savedX = cam->camPos->relative.t[0];
    savedY = cam->camPos->relative.t[1];
    savedZ = cam->camPos->relative.t[2];
    guRotate(&rotMtx, angle, 0.0f, 1.0f, 0.0f);

    SetSkeletonPosition(cam->camPos, 0, g_skyOffsetY, 0);
    cam->camPos->update = 1;
    MatrixToN64Mtx(GetSkeletonMatrix(cam->camPos), (N64Mtx *)&g_mtxPool[g_mtxIndex]);
    mtx = &g_mtxPool[g_mtxIndex];
    guMtxCatL(&rotMtx, mtx, mtx);
    g_mtxIndex++;

    gDPPipeSync(g_skyGfxPtr++);
    gSPTexture(g_skyGfxPtr++, 0, 0, 0, G_TX_RENDERTILE, G_OFF);
    gSPSetGeometryMode(g_skyGfxPtr++, G_SHADING_SMOOTH);
    gDPSetCombineMode(g_skyGfxPtr++, G_CC_SHADE, G_CC_SHADE);

    gSPVertex(g_skyGfxPtr++, OS_K0_TO_PHYSICAL(g_skyHorizonVtx), 14, 0);
    for (col = 0; col < 6; col++) {
        v1 = col + 1;
        v2 = col + 7;
        v3 = col + 8;
        gSP2Triangles(g_skyGfxPtr++, col, v1, v2, 0, v2, v1, v3, 0);
    }

    gSPEndDisplayList(g_skyGfxPtr++);
    dlLen = g_skyGfxPtr - g_skyDlPool;
    g_skyDlCount += dlLen;

    SetSkeletonPosition(cam->camPos, savedX, savedY, savedZ);
    AddRenderDisplayList(&g_skyRenderList, &dlStart, mtx);
    glistp = FlushSkyRenderList(&g_skyRenderList, glistp);
    return glistp;
}
