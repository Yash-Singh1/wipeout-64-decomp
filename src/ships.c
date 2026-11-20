/*
 * Ships.c — Ship model loading, instancing, per-class initialization, and draw.
 *
 * Loads team ship PRM meshes, initializes all 15 race ships (scaling thrust,
 * drag, mass, and AI speed tables for 50 Hz PAL in Europe), computes per-player
 * ship visibility, and renders ships, ghost ships, shields, and track shadows.
 */
#include "ships.h"

#ident "$Revision: 7.27 $"
#ident "$Revision: 1.5 $"
#ident "$Revision: 1.34 $"

#ifdef VERSION_EU
/* PAL runs at 50 Hz instead of NTSC's 60 Hz: scale countdown frames and turn rates. */
#define START_COUNTDOWN_FRAMES  166
#define SCALE_TURN(x)           (((x) * 60) / 50)
#else
#define START_COUNTDOWN_FRAMES  200
#define SCALE_TURN(x)           (x)
#endif

Gfx *DrawShips(ShipData *shipIndex, Object **shipShapes, s32 camShip, TrackCamera *camera,
               s32 player, Gfx *glistp) {
    s32 i;
    VECTOR distance;
    VECTOR viewPos;
    SVECTOR toCentre;
    VECTOR shipViewPos[NO_OF_SHIPS]; /* unused: retail reserves 0x180 bytes of stack locals */
    VECTOR shipUnused[9];
    s32 flag;
    s32 setColour;
    s32 showShip;
    s32 mag;
    s16 baseIndex;
    s16 diff;
    Object *obj;
    Face *face;
    MATRIX *mat;
    s32 playerNo;
    s32 lodDist;
    s32 drawDist;
    u8 r, g, b;
    s32 x, y, z;
    s32 maxCoord;

    playerNo = player;
    switch (frontendOptions_g.numPlayers) {
        case 1:
            drawDist = 1200;
            lodDist = 800;
            break;
        case 2:
            drawDist = 1000;
            lodDist = 600;
            break;
        case 3:
            drawDist = 450;
            lodDist = 400;
            break;
        case 4:
            drawDist = 450;
            lodDist = 400;
            break;
    }

    g_polyCount = 0;
    if (g_shipUpdatePaused) {
        ResetRenderList(&g_shipRenderList);
        for (i = 0; i < 3; i++) {
            obj = shipShapes[shipIndex[i].highResTableIndex];
            SetSkeletonPosition(obj->skeleton, shipIndex[i].ppivot.vx >> 1,
                                shipIndex[i].ppivot.vy >> 1, shipIndex[i].ppivot.vz >> 1);
            SetSkeletonDirectionHPR(obj->skeleton, shipIndex[i].hdg, shipIndex[i].pitch,
                                    shipIndex[i].roll);
            mat = GetSkeletonMatrix(obj->skeleton);
            MatrixToN64Mtx(mat, &g_mtxPool[g_mtxIndex]);
            AddRenderGroup(&g_shipRenderList, (RenderGroup *)obj->bspTree,
                           (Mtx *)&g_mtxPool[g_mtxIndex]);
            g_mtxIndex++;
        }
        return FlushRenderList(&g_shipRenderList, glistp);
    }

    ResetRenderList(&g_shipRenderList);
    proximity = 0;
    baseIndex = raceLines[camera->section->secNo].distance;

    for (i = 0; i < NO_OF_SHIPS; i++) {
        shipIndex[i].combatAttr |= SHIP_CLIPPED;
        distance.vx = (shipIndex[i].ppivot.vx + (camera->camPos->relative.t[0] << 1)) >> 3;
        distance.vy = (shipIndex[i].ppivot.vy + (camera->camPos->relative.t[1] << 1)) >> 3;
        distance.vz = (shipIndex[i].ppivot.vz + (camera->camPos->relative.t[2] << 1)) >> 3;
        mag = GetMagnitude(distance);

        toCentre.vx = shipIndex[i].ppivot.vx + (camera->camPos->relative.t[0] << 1);
        toCentre.vy = shipIndex[i].ppivot.vy + (camera->camPos->relative.t[1] << 1);
        toCentre.vz = shipIndex[i].ppivot.vz + (camera->camPos->relative.t[2] << 1);
        SetSkeletonTransform(camera->camPos->super);
        RotTrans(&toCentre, &viewPos, &flag);

        if (mag < drawDist) {
            proximity |= 1 << i;
            shipIndex[i].attr |= VISIBLE;
            shipIndex[i].visible[playerNo] = 1;
        }

        if (!(shipIndex[i].combatAttr & SHIP_DESTROYED) && shipIndex[i].visible[playerNo] &&
            (viewPos.vz > 0 || i == camShip)) {
            if ((raceType == RACE_CHAMPIONSHIP || raceType == RACE_CHALLENGE) && i == ownShip) {
                obj = shipShapes[shipIndex[i].highResTableIndex];
                setColour = 1;
            } else if (raceType == RACE_TIME_TRIAL) {
                obj = shipShapes[shipIndex[i].highResTableIndex];
                setColour = 1;
            } else if (mag < lodDist) {
                obj = shipShapes[shipIndex[i].lowResTableIndex];
                setColour = 1;
            } else {
                obj = shipShapes[shipIndex[i].farResTableIndex];
                setColour = 0;
            }

            obj->skeleton->super = camera->camPos;
            SetSkeletonPosition(obj->skeleton, shipIndex[i].ppivot.vx >> 1,
                                shipIndex[i].ppivot.vy >> 1, shipIndex[i].ppivot.vz >> 1);
            SetSkeletonDirectionHPR(obj->skeleton, shipIndex[i].hdg, shipIndex[i].pitch,
                                    shipIndex[i].roll);

            switch (playerNo) {
                case 0:
                    if (i == ownShip) {
                        showShip = 0;
                        if (shipIndex[i].attr & VIEW_IN) {
                            break;
                        }
                    }
                    showShip = 1;
                    break;
                case 1:
                    if (i == ownShip2 && ((shipIndex[i].attr & VIEW_IN) || pauseRequests[1])) {
                        showShip = 0;
                    } else {
                        showShip = 1;
                    }
                    break;
                case 2:
                    if (i == ownShip3 && ((shipIndex[i].attr & VIEW_IN) || pauseRequests[2])) {
                        showShip = 0;
                    } else {
                        showShip = 1;
                    }
                    break;
                case 3:
                    if (i == ownShip4 && ((shipIndex[i].attr & VIEW_IN) || pauseRequests[3])) {
                        showShip = 0;
                    } else {
                        showShip = 1;
                    }
                    break;
            }

            if (showShip == 1 || (shipIndex[i].attr & REMOTE_VIEW)) {
                diff = baseIndex - raceLines[shipIndex[i].nearTrkSect->secNo].distance;
                if (diff < 0) {
                    diff += numTrackSections;
                }
                if (diff > numTrackSections / 2) {
                    diff -= numTrackSections;
                }
                if (diff > -40 && diff < 40) {
                    shipIndex[i].combatAttr &= ~SHIP_CLIPPED;
                    face = (Face *)track->faces + shipIndex[i].nearTrkSect->faceStart;
                    while (!(face->flags & TRACK_BASE)) {
                        face++;
                    }
                    if (!(shipIndex[i].attr & LSIDE)) {
                        face++;
                    }

                    r = (face->red + g_colorGammaLut[face->red]) >> 1;
                    g = (face->green + g_colorGammaLut[face->green]) >> 1;
                    b = (face->blue + g_colorGammaLut[face->blue]) >> 1;

                    mat = GetSkeletonMatrix(obj->skeleton);
                    x = abs(mat->t[0]);
                    z = abs(mat->t[2]);
                    y = abs(mat->t[1]);
                    maxCoord = x;
                    if (y > maxCoord) {
                        maxCoord = y;
                    }
                    if (z > maxCoord) {
                        maxCoord = z;
                    }
                    if (abs(maxCoord) <= 0x3000) {
                        if (setColour) {
                            SetGroupPrimColour((RenderGroup *)obj->bspTree, r, g, b);
                        }

                        if (raceType == RACE_MULTIPLAYER) {
                            if ((frontendOptions_g.numPlayers == 2 &&
                                 (i == ownShip || i == ownShip2)) ||
                                (frontendOptions_g.numPlayers == 3 &&
                                 (i == ownShip || i == ownShip2 || i == ownShip3)) ||
                                (frontendOptions_g.numPlayers == 4 &&
                                 (i == ownShip || i == ownShip2 || i == ownShip3 || i == ownShip4))) {
                                MatrixToN64Mtx(mat, &g_mtxPool[g_mtxIndex]);
                                g_polyCount += obj->primitiveCount;
                                if (i == ownShip) {
                                    SetGroupPrimColour(g_playerShipGroup[0], r, g, b);
                                    if (shipIndex[i].combatAttr & SHIP_CLOAKED) {
                                        AddRenderGroup(&g_shipRenderList, g_playerShipXluGroup[0],
                                                       (Mtx *)&g_mtxPool[g_mtxIndex]);
                                    } else {
                                        AddRenderGroup(&g_shipRenderList, g_playerShipGroup[0],
                                                       (Mtx *)&g_mtxPool[g_mtxIndex]);
                                    }
                                } else if (i == ownShip2) {
                                    SetGroupPrimColour(g_playerShipGroup[1], r, g, b);
                                    if (shipIndex[i].combatAttr & SHIP_CLOAKED) {
                                        AddRenderGroup(&g_shipRenderList, g_playerShipXluGroup[1],
                                                       (Mtx *)&g_mtxPool[g_mtxIndex]);
                                    } else {
                                        AddRenderGroup(&g_shipRenderList, g_playerShipGroup[1],
                                                       (Mtx *)&g_mtxPool[g_mtxIndex]);
                                    }
                                } else if (i == ownShip3) {
                                    SetGroupPrimColour(g_playerShipGroup[2], r, g, b);
                                    if (shipIndex[i].combatAttr & SHIP_CLOAKED) {
                                        AddRenderGroup(&g_shipRenderList, g_playerShipXluGroup[2],
                                                       (Mtx *)&g_mtxPool[g_mtxIndex]);
                                    } else {
                                        AddRenderGroup(&g_shipRenderList, g_playerShipGroup[2],
                                                       (Mtx *)&g_mtxPool[g_mtxIndex]);
                                    }
                                } else if (i == ownShip4) {
                                    SetGroupPrimColour(g_playerShipGroup[3], r, g, b);
                                    if (shipIndex[i].combatAttr & SHIP_CLOAKED) {
                                        AddRenderGroup(&g_shipRenderList, g_playerShipXluGroup[3],
                                                       (Mtx *)&g_mtxPool[g_mtxIndex]);
                                    } else {
                                        AddRenderGroup(&g_shipRenderList, g_playerShipGroup[3],
                                                       (Mtx *)&g_mtxPool[g_mtxIndex]);
                                    }
                                }
                                g_mtxIndex++;
                            } else if ((frontendOptions_g.numPlayers == 2 &&
                                        frontendOptions_g.aiShipsEnabled) ||
                                       (frontendOptions_g.numPlayers == 3 &&
                                        frontendOptions_g.aiShipsEnabled && shipIndex[i].activeInLeague)) {
                                MatrixToN64Mtx(mat, &g_mtxPool[g_mtxIndex]);
                                g_polyCount += obj->primitiveCount;
                                if (shipIndex[i].combatAttr & SHIP_CLOAKED) {
                                    AddRenderGroup(&g_shipRenderList, g_ghostShipGroup,
                                                   (Mtx *)&g_mtxPool[g_mtxIndex]);
                                } else {
                                    AddRenderGroup(&g_shipRenderList, (RenderGroup *)obj->bspTree,
                                                   (Mtx *)&g_mtxPool[g_mtxIndex]);
                                }
                                g_mtxIndex++;
                            }
                        } else if (raceType == RACE_TIME_TRIAL) {
                            if (i == ownShip) {
                                MatrixToN64Mtx(mat, &g_mtxPool[g_mtxIndex]);
                                g_polyCount += obj->primitiveCount;
                                AddRenderGroup(&g_shipRenderList, (RenderGroup *)obj->bspTree,
                                               (Mtx *)&g_mtxPool[g_mtxIndex]);
                                g_mtxIndex++;
                            }
                            if (i == g_demoShip && g_ghostActive) {
                                MatrixToN64Mtx(mat, &g_mtxPool[g_mtxIndex]);
                                g_polyCount += obj->primitiveCount;
                                AddRenderGroup(&g_shipRenderList, g_ghostShipGroup,
                                               (Mtx *)&g_mtxPool[g_mtxIndex]);
                                g_mtxIndex++;
                            }
                        } else if (raceType == RACE_CHALLENGE &&
                                   frontendOptions_g.challengeType == CHALLENGE_TIME_TRIAL_2) {
                            if (i == ownShip) {
                                MatrixToN64Mtx(mat, &g_mtxPool[g_mtxIndex]);
                                g_polyCount += obj->primitiveCount;
                                AddRenderGroup(&g_shipRenderList, (RenderGroup *)obj->bspTree,
                                               (Mtx *)&g_mtxPool[g_mtxIndex]);
                                g_mtxIndex++;
                            }
                        } else {
                            MatrixToN64Mtx(mat, &g_mtxPool[g_mtxIndex]);
                            g_polyCount += obj->primitiveCount;
                            if (shipIndex[i].combatAttr & SHIP_CLOAKED) {
                                AddRenderGroup(&g_shipRenderList, g_ghostShipGroup,
                                               (Mtx *)&g_mtxPool[g_mtxIndex]);
                            } else {
                                AddRenderGroup(&g_shipRenderList, (RenderGroup *)obj->bspTree,
                                               (Mtx *)&g_mtxPool[g_mtxIndex]);
                            }
                            g_mtxIndex++;
                        }
                    }
                }
            }
        } else if (!(mag < 2900)) {
            shipIndex[i].attr &= ~VISIBLE;
            shipIndex[i].visible[playerNo] = 0;
        }
    }

    return FlushRenderList(&g_shipRenderList, glistp);
}

Gfx *DrawStartDroidAndLights(Gfx *glistp, TrackCamera *camera, s32 player) {
    Object *obj;
    MATRIX *mat;
    s32 i;
    s16 diff;
    s32 maxDist;

    maxDist = 72;
    if (frontendOptions_g.numPlayers >= 2) {
        maxDist = 48;
    }

    ResetRenderList(&g_startDroidRenderList);
    ResetRenderList(&g_lightRenderList);

    if (g_startDroidState[player].leftState != START_DROID_HIDDEN) {
        obj = startDroidObjects[1];
        SetSkeletonDirectionHPR(obj->skeleton, g_startDroidState[player].leftHdg, 0, 0);
        SetSkeletonPosition(obj->skeleton, g_startDroidState[player].leftPos.vx,
                            g_startDroidState[player].leftPos.vy,
                            g_startDroidState[player].leftPos.vz);
        obj->skeleton->super = camera->camPos;
        mat = GetSkeletonMatrix(obj->skeleton);
        if (frontendOptions_g.numPlayers == 1) {
            obj = startDroidObjects[7];
            SetSkeletonDirectionHPR(obj->skeleton, g_startDroidState[player].leftHdg, 0, 0);
            SetSkeletonPosition(obj->skeleton, g_startDroidState[player].leftPos.vx,
                                g_startDroidState[player].leftPos.vy,
                                g_startDroidState[player].leftPos.vz);
            obj->skeleton->super = camera->camPos;
            mat = GetSkeletonMatrix(obj->skeleton);
        }
        MatrixToN64Mtx(mat, &g_mtxPool[g_mtxIndex]);
        g_polyCount += obj->primitiveCount;
        AddRenderGroup(&g_startDroidRenderList, (RenderGroup *)obj->bspTree,
                       (Mtx *)&g_mtxPool[g_mtxIndex]);
        g_mtxIndex++;

        obj = startDroidObjects[g_startDroidState[player].leftState];
        SetSkeletonPosition(obj->skeleton, 0, 0, -600);
        obj->skeleton->update = 1;
        mat = GetSkeletonMatrix(obj->skeleton);
        MatrixToN64Mtx(mat, &g_mtxPool[g_mtxIndex]);
        g_polyCount += obj->primitiveCount;
        AddRenderGroup(&g_startDroidRenderList, (RenderGroup *)obj->bspTree,
                       (Mtx *)&g_mtxPool[g_mtxIndex]);
        g_mtxIndex++;
    }

    if (g_startDroidState[player].rightState != START_DROID_HIDDEN) {
        obj = startDroidObjects[9];
        SetSkeletonDirectionHPR(obj->skeleton, g_startDroidState[player].rightHdg, 0, 0);
        SetSkeletonPosition(obj->skeleton, g_startDroidState[player].rightPos.vx,
                            g_startDroidState[player].rightPos.vy,
                            g_startDroidState[player].rightPos.vz);
        obj->skeleton->super = camera->camPos;
        mat = GetSkeletonMatrix(obj->skeleton);
        if (frontendOptions_g.numPlayers == 1) {
            obj = startDroidObjects[8];
            SetSkeletonDirectionHPR(obj->skeleton, g_startDroidState[player].rightHdg, 0, 0);
            SetSkeletonPosition(obj->skeleton, g_startDroidState[player].rightPos.vx,
                                g_startDroidState[player].rightPos.vy,
                                g_startDroidState[player].rightPos.vz);
            obj->skeleton->super = camera->camPos;
            mat = GetSkeletonMatrix(obj->skeleton);
        }
        MatrixToN64Mtx(mat, &g_mtxPool[g_mtxIndex]);
        g_polyCount += obj->primitiveCount;
        AddRenderGroup(&g_startDroidRenderList, (RenderGroup *)obj->bspTree,
                       (Mtx *)&g_mtxPool[g_mtxIndex]);
        g_mtxIndex++;

        obj = startDroidObjects[g_startDroidState[player].rightState];
        SetSkeletonPosition(obj->skeleton, 0, 0, -600);
        obj->skeleton->update = 1;
        mat = GetSkeletonMatrix(obj->skeleton);
        MatrixToN64Mtx(mat, &g_mtxPool[g_mtxIndex]);
        g_polyCount += obj->primitiveCount;
        AddRenderGroup(&g_startDroidRenderList, (RenderGroup *)obj->bspTree,
                       (Mtx *)&g_mtxPool[g_mtxIndex]);
        g_mtxIndex++;
    }

    glistp = FlushRenderList(&g_startDroidRenderList, glistp);

    if (!(frontendOptions_g.trackNo == 3 &&
          (u16)(camera->section->secNo - 11) < 14)) {
        for (i = 0; i < lightCount; i++) {
            diff = raceLines[ships[frontendOptions_g.ownShips[player]].nearSecNo].distance -
                   raceLines[lightSections[i]->secNo].distance;
            if (diff < 0) {
                diff += numTrackSections;
            }
            if (diff > (numTrackSections >> 1)) {
                diff -= numTrackSections;
            }
            if ((diff < 0 ? -diff : diff) <= maxDist) {
                obj = lightGroups[i][LIGHT_GROUP_SKELETON];
                obj->skeleton->super = camera->camPos;
                obj->skeleton->update = 1;
                mat = GetSkeletonMatrix(obj->skeleton);
                if (frontendOptions_g.numPlayers == 1) {
                    MatrixToN64Mtx(mat, &g_mtxPool[g_mtxIndex]);
                    g_polyCount += obj->primitiveCount;
                    AddRenderGroup(&g_lightRenderList, (RenderGroup *)obj->bspTree,
                                   (Mtx *)&g_mtxPool[g_mtxIndex]);
                    g_mtxIndex++;
                }
                obj = lightGroups[i][lightState[i]];
                obj->skeleton->update = 1;
                mat = GetSkeletonMatrix(obj->skeleton);
                MatrixToN64Mtx(mat, &g_mtxPool[g_mtxIndex]);
                g_polyCount += obj->primitiveCount;
                AddRenderGroup(&g_lightRenderList, (RenderGroup *)obj->bspTree,
                               (Mtx *)&g_mtxPool[g_mtxIndex]);
                g_mtxIndex++;
            }
        }
    }

    return FlushRenderList(&g_lightRenderList, glistp);
}

/*
 * SN64 GCC 2.7.2 reloads shipShapes[1..4] after each ->next store unless the five
 * pointers are staged before storing, and local-alloc ranks o0 ahead of o4 when o4
 * is loaded first unless o1..o4 carry an extra flow reference folded away by combine.
 */
static inline Object *StageShipLink(Object *obj) {
    return (Object *)((u32)obj | ((u32)obj & 1));
}

void loadShipData(TrackSection *section, ShipData *shipIndex, Object **shipShapes, Skeleton *camPos) {
    Object *obj;
    s16 shipTextures;
    s16 droidTextures;
    s16 count = 0;
    s32 i;

    if (sillyShips) {
        highResTable[0] = 0;
        highResTable[1] = 5;
        highResTable[2] = 10;
        highResTable[3] = 1;
        highResTable[4] = 6;
        highResTable[5] = 11;
        highResTable[6] = 3;
        highResTable[7] = 8;
        highResTable[8] = 13;
        highResTable[9] = 4;
        highResTable[10] = 9;
        highResTable[11] = 14;
        highResTable[12] = 2;
        highResTable[13] = 7;
        highResTable[14] = 12;
    }

    shipTextures = TextureTableCount;
    shipTex = shipTextures;
    g_startupData->current = STARTUP_SHIP_ENTRY;
    LoadCmpFiles(WADLoadCompressedTextureSequence(g_startupData));
    droidTextures = TextureTableCount;
    LoadCmpFiles(LoadCompressedTextureSequence((u32)_droidTexturesFile));
    InitRenderList(&g_shipRenderList, shipTextures, droidTextures - shipTextures);
    InitRenderList(&g_droidRenderList, droidTextures, TextureTableCount - droidTextures);

    obj = shipShapes[0] = WADLoadPrm(g_startupData, &g_shipRenderList);
    while (obj) {
        obj->skeleton->super = camPos;
        shipShapes[count] = obj;
        count++;
        obj = obj->next;
    }
    {
        Object *o4 = shipShapes[4];
        Object *o0 = shipShapes[0];
        Object *o1 = shipShapes[1];
        Object *o2 = shipShapes[2];
        Object *o3 = shipShapes[3];

        o0->next = StageShipLink(o1);
        o1->next = StageShipLink(o2);
        o2->next = StageShipLink(o3);
        o3->next = StageShipLink(o4);
        o4->next = NULL;
    }
    shipShapes[15] = shipShapes[5];
    shipShapes[16] = shipShapes[6];
    shipShapes[17] = shipShapes[7];
    shipShapes[18] = shipShapes[8];
    shipShapes[19] = shipShapes[9];

    for (count = 0; count < NO_OF_SHIPS; count++) {
        shipIndex[count].highResTableIndex = highResTable[count];
        shipIndex[count].lowResTableIndex = highResTable[count] + NO_OF_SHIPS;
        shipIndex[count].farResTableIndex = highResTable[count] + NO_OF_SHIPS * 2;
    }

    shipShapes[5] = NewObjectInstance(shipShapes[0]);
    shipShapes[6] = NewObjectInstance(shipShapes[1]);
    shipShapes[7] = NewObjectInstance(shipShapes[2]);
    shipShapes[8] = NewObjectInstance(shipShapes[3]);
    shipShapes[9] = NewObjectInstance(shipShapes[4]);
    shipShapes[10] = NewObjectInstance(shipShapes[0]);
    shipShapes[11] = NewObjectInstance(shipShapes[1]);
    shipShapes[12] = NewObjectInstance(shipShapes[2]);
    shipShapes[13] = NewObjectInstance(shipShapes[3]);
    shipShapes[14] = NewObjectInstance(shipShapes[4]);
    shipShapes[20] = NewObjectInstance(shipShapes[15]);
    shipShapes[21] = NewObjectInstance(shipShapes[16]);
    shipShapes[22] = NewObjectInstance(shipShapes[17]);
    shipShapes[23] = NewObjectInstance(shipShapes[18]);
    shipShapes[24] = NewObjectInstance(shipShapes[19]);
    shipShapes[25] = NewObjectInstance(shipShapes[15]);
    shipShapes[26] = NewObjectInstance(shipShapes[16]);
    shipShapes[27] = NewObjectInstance(shipShapes[17]);
    shipShapes[28] = NewObjectInstance(shipShapes[18]);
    shipShapes[29] = NewObjectInstance(shipShapes[19]);

    obj = shipShapes[30] = WADLoadPrm(g_startupData, &g_shipRenderList);
    while (obj) {
        obj->skeleton->super = camPos;
        obj = obj->next;
    }
    for (i = 1; i < NO_OF_SHIPS; i++) {
        shipShapes[30 + i] = NewObjectInstance(shipShapes[30]);
    }

    obj = shipShapes[RescueDroidModel] = LoadPrm((u32)_droidModelFile, &g_droidRenderList);
    while (obj) {
        obj->skeleton->super = camPos;
        obj = obj->next;
    }
}

void initShipData(TrackSection *section, ShipData *shipIndex, ConfigData *options) {
    VECTOR delta;
    VECTOR v0;
    VECTOR v2;
    VECTOR mid;
    TrackSection *startSections[NO_OF_SHIPS];
    u8 gridSides[16];
    u16 droneSpeedGroups[16] = {
        0x10, 0x20, 0x40, 0x10, 0x20, 0x40, 0x10, 0x20,
        0x40, 0x10, 0x20, 0x40, 0x10, 0x20, 0x40, 0x10
    };
    s16 i;
    s16 j;
    s16 startPosition;
    s16 nextGridSide;
    s16 startSectionNo;
    Face *face;
    TrackSection *ahead;
    s32 team;
    s32 p;

    for (i = 0; i < MAX_PLAYERS; i++) {
        shakeAmount[i] = 0;
    }

    startSectionNo = startLinePos[options->actTrackNum];
    while (1) {
        if (section->secNo == startSectionNo) {
            break;
        }
        section++;
    }

    for (i = 0; i < 5; i++) {
        section = section->prevSection.ptr;
    }

    if (raceType == RACE_TIME_TRIAL ||
        (raceType == RACE_CHALLENGE && frontendOptions_g.challengeType == CHALLENGE_TIME_TRIAL_2)) {
        for (i = 0; i < 21; i++) {
            section = section->prevSection.ptr;
        }
    }

    nextGridSide = 0;
    for (i = 0; i < NO_OF_SHIPS; i++) {
        startSections[i] = section;
        gridSides[i] = nextGridSide;
        section = section->prevSection.ptr;
        if (nextGridSide == 0) {
            section = section->prevSection.ptr;
        }
        nextGridSide ^= 1;
    }

    if (raceType == RACE_TIME_TRIAL) {
        gridPosition[g_demoShip] = 0;
        gridPosition[ownShip] = 0;
    } else if (raceType == RACE_CHALLENGE &&
               frontendOptions_g.challengeType == CHALLENGE_TIME_TRIAL_2) {
        gridPosition[ownShip] = 0;
    } else if ((frontendOptions_g.mode == 3 && !frontendOptions_g.aiShipsEnabled) ||
               frontendOptions_g.numPlayers == 4) {
        switch (frontendOptions_g.numPlayers) {
            case 2:
                gridPosition[ownShip] = 0;
                gridPosition[ownShip2] = 1;
                break;
            case 3:
                gridPosition[ownShip] = 0;
                gridPosition[ownShip2] = 1;
                gridPosition[ownShip3] = 2;
                break;
            case 4:
                gridPosition[ownShip] = 0;
                gridPosition[ownShip2] = 1;
                gridPosition[ownShip3] = 2;
                gridPosition[ownShip4] = 3;
                break;
        }
    } else if (frontendOptions_g.numPlayers == 3 && frontendOptions_g.aiShipsEnabled == 1) {
        gridPosition[ownShip] = 0;
        gridPosition[ownShip2] = 1;
        gridPosition[ownShip3] = 2;
        if ((ownShip >= 6 && ownShip <= 8) &&
            (ownShip2 >= 6 && ownShip2 <= 8) &&
            (ownShip3 >= 6 && ownShip3 <= 8)) {
            gridPosition[5] = 3;
        } else {
            gridPosition[8] = 3;
        }
    } else if (frontendOptions_g.numPlayers < 3 && frontendOptions_g.aiShipsEnabled == 1) {
        s16 groups;
        s16 next;

        groups = 5;
        for (i = 0; i < 3; i++) {
            startGridOrder[i * groups] = 11 - i;
            startGridOrder[i * groups + 1] = 8 - i;
            startGridOrder[i * groups + 2] = 2 - i;
            startGridOrder[i * groups + 3] = 14 - i;
            startGridOrder[i * groups + 4] = 5 - i;
        }
        next = 0;
        for (i = 0; i < groups * 3; i++) {
            if (startGridOrder[i] != ownShip && startGridOrder[i] != ownShip2 &&
                startGridOrder[i] != ownShip3 && startGridOrder[i] != ownShip4) {
                gridPosition[startGridOrder[i]] = next;
                next++;
            }
        }
        if (attractMode == 2) {
            gridPosition[ownShip] = next;
        } else if (attractMode == 3) {
            gridPosition[ownShip] = next;
            next++;
            if (frontendOptions_g.numPlayers >= 2) {
                gridPosition[ownShip2] = next;
                next++;
            }
            if (frontendOptions_g.numPlayers >= 3) {
                gridPosition[ownShip3] = next;
                next++;
            }
            if (frontendOptions_g.numPlayers == 4) {
                gridPosition[ownShip4] = next;
            }
        }
    }

    for (i = 0; i < NO_OF_SHIPS; i++) {
        g_weaponCharge[i] = 0;
        startPosition = gridPosition[i];

        shipIndex[i].attr = VISIBLE | RACING | DIRECTION;
        shipIndex[i].targetShip = -1;
        shipIndex[i].rearTarget = -1;
        shipIndex[i].visible[3] = 1;
        shipIndex[i].visible[2] = 1;
        shipIndex[i].visible[1] = 1;
        shipIndex[i].visible[0] = 1;
        shipIndex[i].vpivot.vx = 0;
        shipIndex[i].vpivot.vy = 0;
        shipIndex[i].vpivot.vz = 0;
        shipIndex[i].apivot.vx = 0;
        shipIndex[i].apivot.vy = 0;
        shipIndex[i].apivot.vz = 0;
        shipIndex[i].hdg = 0;
        shipIndex[i].pitch = 0;
        shipIndex[i].roll = 0;
        shipIndex[i].vhdg = 0;
        shipIndex[i].vpitch = 0;
        shipIndex[i].vroll = 0;
        shipIndex[i].ahdg = 0;
        shipIndex[i].r_brake_rot = 0;
        shipIndex[i].l_brake_rot = 0;
        shipIndex[i].speed = 0;
        shipIndex[i].shipInitializationValue = 0;
        shipIndex[i].thrust = 0;
        shipIndex[i].thrustCount = 0;
        shipIndex[i].rotationCorrectionDisabled = 0;
        shipIndex[i].weapon = 0;
        shipIndex[i].lastWeapon = 0;
        shipIndex[i].electroCount = 0;
        shipIndex[i].autopilotCount = 0;
        shipIndex[i].stallCount = 0;
        shipIndex[i].activeInLeague = 0;
        shipIndex[i].lap = 0;
        shipIndex[i].reverseLap = 0;
        shipIndex[i].eliminations = 0;
        shipIndex[i].unkF5 = 0;
        shipIndex[i].unkF8 = -1;
        shipIndex[i].finishPosition = 0;
        for (j = 0; j < 5; j++) {
            shipIndex[i].lapTimes[j] = 0;
        }
        shipIndex[i].updateCount = START_COUNTDOWN_FRAMES;

        if ((frontendOptions_g.numPlayers == 1 && i == ownShip) ||
            (frontendOptions_g.numPlayers == 2 && (i == ownShip || i == ownShip2)) ||
            (frontendOptions_g.numPlayers == 3 &&
             (i == ownShip || i == ownShip2 || i == ownShip3)) ||
            (frontendOptions_g.numPlayers == 4 &&
             (i == ownShip || i == ownShip2 || i == ownShip3 || i == ownShip4))) {
            shipIndex[i].combatAttr = 0;
            shipIndex[i].activeInLeague = 1;
        } else if (raceType == RACE_TIME_TRIAL) {
            if (i != g_demoShip) {
                shipIndex[i].combatAttr |= SHIP_DESTROYED;
            } else {
                shipIndex[i].combatAttr &= ~SHIP_DESTROYED;
            }
        } else if (raceType == RACE_MULTIPLAYER &&
                   (((frontendOptions_g.numPlayers == 2 || frontendOptions_g.numPlayers == 3) &&
                     !frontendOptions_g.aiShipsEnabled) ||
                    frontendOptions_g.numPlayers == 4)) {
            shipIndex[i].combatAttr |= SHIP_DESTROYED;
        } else if (raceType == RACE_MULTIPLAYER && frontendOptions_g.numPlayers == 3 &&
                   frontendOptions_g.aiShipsEnabled) {
            if ((ownShip >= 6 && ownShip <= 8) &&
                (ownShip2 >= 6 && ownShip2 <= 8) &&
                (ownShip3 >= 6 && ownShip3 <= 8)) {
                if (i == 5) {
                    shipIndex[i].combatAttr = droneSpeedGroups[i];
                    shipIndex[i].activeInLeague = 1;
                } else {
                    shipIndex[i].combatAttr |= SHIP_DESTROYED;
                }
            } else {
                if (i != 8) {
                    shipIndex[i].combatAttr |= SHIP_DESTROYED;
                } else {
                    shipIndex[i].combatAttr = droneSpeedGroups[i];
                    shipIndex[i].activeInLeague = 1;
                }
            }
        } else if (raceType == RACE_CHALLENGE &&
                   frontendOptions_g.challengeType == CHALLENGE_TIME_TRIAL_2) {
            shipIndex[i].combatAttr |= SHIP_DESTROYED;
        } else {
            shipIndex[i].combatAttr = droneSpeedGroups[i];
            shipIndex[i].activeInLeague = 1;
        }

        if ((u16)i <= 2) {
            team = 0;
        } else if ((u16)(i - 3) <= 2) {
            team = 1;
        } else if ((u16)(i - 6) <= 2) {
            team = 2;
        } else if ((u16)(i - 9) <= 2) {
            team = 3;
        } else {
            team = 4;
        }

        switch (options->shipClass) {
            case CLASS_VECTOR:
                shipIndex[i].mass = shipMassClass0[team];
                shipIndex[i].maxThrust = shipMaxThrustClass0[team];
                shipIndex[i].skid = shipSkidClass0[team];
                shipIndex[i].resistance = shipResistanceClass0[team];
                break;
            case CLASS_VENOM:
                shipIndex[i].mass = shipMassClass1[team];
                shipIndex[i].maxThrust = shipMaxThrustClass1[team];
                shipIndex[i].skid = shipSkidClass1[team];
                shipIndex[i].resistance = shipResistanceClass1[team];
                break;
            case CLASS_RAPIER:
                shipIndex[i].mass = shipMassClass2[team];
                shipIndex[i].maxThrust = shipMaxThrustClass2[team];
                shipIndex[i].skid = shipSkidClass2[team];
                shipIndex[i].resistance = shipResistanceClass2[team];
                break;
            case CLASS_PHANTOM:
                shipIndex[i].mass = shipMassClass3[team];
                shipIndex[i].maxThrust = shipMaxThrustClass3[team];
                shipIndex[i].skid = shipSkidClass3[team];
                shipIndex[i].resistance = shipResistanceClass3[team];
                break;
        }

        switch (options->shipClass) {
            case CLASS_VECTOR:
                shipIndex[i].headingInc = SCALE_TURN(shipHeadingIncClass0[team]);
                shipIndex[i].maxHeading = SCALE_TURN(shipMaxHeadingClass0[team]);
                break;
            case CLASS_VENOM:
                shipIndex[i].headingInc = SCALE_TURN(shipHeadingIncClass1[team]);
                shipIndex[i].maxHeading = SCALE_TURN(shipMaxHeadingClass1[team]);
                break;
            case CLASS_RAPIER:
                shipIndex[i].headingInc = SCALE_TURN(shipHeadingIncClass2[team]);
                shipIndex[i].maxHeading = SCALE_TURN(shipMaxHeadingClass2[team]);
                break;
            case CLASS_PHANTOM:
                shipIndex[i].headingInc = SCALE_TURN(shipHeadingIncClass3[team]);
                shipIndex[i].maxHeading = SCALE_TURN(shipMaxHeadingClass3[team]);
                break;
        }

        if (i == ownShip || i == ownShip2 || i == ownShip3 || i == ownShip4 ||
            (i == g_demoShip && raceType == RACE_TIME_TRIAL)) {
            shipIndex[i].update = UpdatePlayerShipStart;
        } else {
            shipIndex[i].update = UpdateDroneShipStart;
        }

        switch (options->shipClass) {
            case CLASS_VECTOR:
                shipIndex[i].remoteMax = g_aiMaxThrustVector[i];
                shipIndex[i].remoteMagnitude = g_aiThrustMagVector[i];
                break;
            case CLASS_VENOM:
                shipIndex[i].remoteMax = g_aiMaxThrustVenom[i];
                shipIndex[i].remoteMagnitude = g_aiThrustMagVenom[i];
                break;
            case CLASS_RAPIER:
                shipIndex[i].remoteMax = g_aiMaxThrustRapier[i];
                shipIndex[i].remoteMagnitude = g_aiThrustMagRapier[i];
                break;
            case CLASS_PHANTOM:
                shipIndex[i].remoteMax = g_aiMaxThrustPhantom[i];
                shipIndex[i].remoteMagnitude = g_aiThrustMagPhantom[i];
                break;
        }

        if (raceType == RACE_CHAMPIONSHIP ||
            (raceType == RACE_MULTIPLAYER && frontendOptions_g.numPlayers == 2) ||
            (raceType == RACE_MULTIPLAYER && frontendOptions_g.numPlayers == 3)) {
            switch (frontendOptions_g.teamClass) {
                case CLASS_VENOM:
                    if (frontendOptions_g.trackNo == 6) {
                        shipIndex[i].remoteMax -= 200;
                    }
                    break;
                case CLASS_RAPIER:
                    if (frontendOptions_g.trackNo == 6) {
                        shipIndex[i].remoteMax -= 300;
                    }
                    break;
                case CLASS_PHANTOM:
                    if (frontendOptions_g.trackNo == 6) {
                        shipIndex[i].remoteMax -= 500;
                    }
                    if (frontendOptions_g.trackNo == 5) {
                        shipIndex[i].remoteMax -= 500;
                    }
                    if (frontendOptions_g.trackNo == 4) {
                        shipIndex[i].remoteMax -= 500;
                    }
                    if (frontendOptions_g.trackNo == 7) {
                        shipIndex[i].remoteMax -= 500;
                    }
                    break;
            }
        } else if (raceType == RACE_CHALLENGE && frontendOptions_g.challengeType == CHALLENGE_RACE) {
            switch (frontendOptions_g.teamClass) {
                case CLASS_RAPIER:
                    if (frontendOptions_g.trackNo == 6) {
                        shipIndex[i].remoteMax -= 300;
                    }
                    break;
                case CLASS_PHANTOM:
                    if (frontendOptions_g.trackNo == 5) {
                        shipIndex[i].remoteMax -= 500;
                    }
                    break;
            }
        }

        if (startPosition == 0 || startPosition == 1) {
            shipIndex[i].remoteMax += options->shipClass * 30;
        }

        SetSpread(shipIndex, options, startPosition, i, bigGrid);

        shipIndex[i].nearTrkSect = startSections[startPosition];
        shipIndex[i].prevShipSect = startSections[startPosition];

        face = (Face *)track->faces + startSections[startPosition]->faceStart;
        while (!(face->flags & TRACK_BASE)) {
            face++;
        }
        if (gridSides[startPosition]) {
            face++;
        }

        GetFaceVertex(startSections[startPosition], face, 0, &v0);
        GetFaceVertex(startSections[startPosition], face, 2, &v2);
        mid.vx = (v0.vx + v2.vx) >> 1;
        mid.vy = (v0.vy + v2.vy) >> 1;
        mid.vz = (v0.vz + v2.vz) >> 1;

        shipIndex[i].ppivot.vx = mid.vx + ((face->normal.vx * 300) >> 12);
        shipIndex[i].ppivot.vy = mid.vy + ((face->normal.vy * 300) >> 12);
        shipIndex[i].ppivot.vz = mid.vz + ((face->normal.vz * 300) >> 12);
        {
            s16 secNo = startSections[startPosition]->secNo;
            s32 dist;

            shipIndex[i].nearSecNo = secNo;
            shipIndex[i].prevSecNo = secNo;
            dist = raceLines[secNo].distance;
            shipIndex[i].distance = dist;
            shipIndex[i].trackDistance = dist;
        }

        ahead = startSections[startPosition]->nextSection.ptr;
        delta.vx = ahead->centre.vx - startSections[startPosition]->centre.vx;
        delta.vy = ahead->centre.vy - startSections[startPosition]->centre.vy;
        delta.vz = ahead->centre.vz - startSections[startPosition]->centre.vz;
        shipIndex[i].hdg = -ratan2(delta.vx, delta.vz);
    }

    activeShipCount = 0;
    for (i = 0; i < NO_OF_SHIPS; i++) {
        if (!(shipIndex[i].combatAttr & SHIP_DESTROYED)) {
            shipRank[activeShipCount].position = raceLines[shipIndex[i].nearSecNo].distance;
            shipRank[activeShipCount].shipNo = i;
            activeShipCount++;
        }
    }

    for (p = 0; p < frontendOptions_g.numPlayers; p++) {
        if (p == 0) {
            g_finishPosition[0] = gridPosition[ownShip] + 1;
        }
        if (p == 1) {
            g_finishPosition[1] = gridPosition[ownShip2] + 1;
        }
        if (p == 2) {
            g_finishPosition[2] = gridPosition[ownShip3] + 1;
        }
        if (p == 3) {
            g_finishPosition[3] = gridPosition[ownShip4] + 1;
        }
    }

    if (g_fourPlayerSameTeam) {
        if ((u32)ownShip3 <= 2) {
            team = 0;
        } else if ((u32)(ownShip3 - 3) <= 2) {
            team = 1;
        } else if ((u32)(ownShip3 - 6) <= 2) {
            team = 2;
        } else if ((u32)(ownShip3 - 9) <= 2) {
            team = 3;
        } else {
            team = 4;
        }

        switch (options->shipClass) {
            case CLASS_VECTOR:
                shipIndex[ownShip4].mass = shipMassClass0[team];
                shipIndex[ownShip4].maxThrust = shipMaxThrustClass0[team];
                shipIndex[ownShip4].skid = shipSkidClass0[team];
                shipIndex[ownShip4].resistance = shipResistanceClass0[team];
                break;
            case CLASS_VENOM:
                shipIndex[ownShip4].mass = shipMassClass1[team];
                shipIndex[ownShip4].maxThrust = shipMaxThrustClass1[team];
                shipIndex[ownShip4].skid = shipSkidClass1[team];
                shipIndex[ownShip4].resistance = shipResistanceClass1[team];
                break;
            case CLASS_RAPIER:
                shipIndex[ownShip4].mass = shipMassClass2[team];
                shipIndex[ownShip4].maxThrust = shipMaxThrustClass2[team];
                shipIndex[ownShip4].skid = shipSkidClass2[team];
                shipIndex[ownShip4].resistance = shipResistanceClass2[team];
                break;
            case CLASS_PHANTOM:
                shipIndex[ownShip4].mass = shipMassClass3[team];
                shipIndex[ownShip4].maxThrust = shipMaxThrustClass3[team];
                shipIndex[ownShip4].skid = shipSkidClass3[team];
                shipIndex[ownShip4].resistance = shipResistanceClass3[team];
                break;
        }

        switch (options->shipClass) {
            case CLASS_VECTOR:
                shipIndex[ownShip4].headingInc = SCALE_TURN(shipHeadingIncClass0[team]);
                shipIndex[ownShip4].maxHeading = SCALE_TURN(shipMaxHeadingClass0[team]);
                break;
            case CLASS_VENOM:
                shipIndex[ownShip4].headingInc = SCALE_TURN(shipHeadingIncClass1[team]);
                shipIndex[ownShip4].maxHeading = SCALE_TURN(shipMaxHeadingClass1[team]);
                break;
            case CLASS_RAPIER:
                shipIndex[ownShip4].headingInc = SCALE_TURN(shipHeadingIncClass2[team]);
                shipIndex[ownShip4].maxHeading = SCALE_TURN(shipMaxHeadingClass2[team]);
                break;
            case CLASS_PHANTOM:
                shipIndex[ownShip4].headingInc = SCALE_TURN(shipHeadingIncClass3[team]);
                shipIndex[ownShip4].maxHeading = SCALE_TURN(shipMaxHeadingClass3[team]);
                break;
        }
    }
}

void SetSpread(ShipData *shipIndex, ConfigData *cfg, s16 pos, s16 num, s16 big) {
    s16 boostFrames;
    s32 shipNo;

    shipNo = num;
    boostFrames = 15 - pos;
    switch (cfg->shipClass) {
        case CLASS_VECTOR:
            shipIndex[shipNo].numToSpeed = boostFrames * (s16)g_spreadScaleVector[cfg->trackNum];
            break;
        case CLASS_VENOM:
            shipIndex[shipNo].numToSpeed = boostFrames * (s16)g_spreadScaleVenom[cfg->trackNum];
            if (raceType == RACE_CHALLENGE && frontendOptions_g.challengeType == CHALLENGE_RACE) {
                switch (frontendOptions_g.trackNo) {
                    case 1:
                        shipIndex[num].numToSpeed = boostFrames * 100;
                        break;
                    case 2:
                        break;
                    case 3:
                        break;
                    case 4:
                        shipIndex[num].numToSpeed = boostFrames * 135;
                        break;
                    case 5:
                        break;
                    case 6:
                        break;
                    case 7:
                        break;
                }
            }
            break;
        case CLASS_RAPIER:
            shipIndex[shipNo].numToSpeed = boostFrames * (s16)g_spreadScaleRapier[cfg->trackNum];
            if (raceType == RACE_CHALLENGE && frontendOptions_g.challengeType == CHALLENGE_RACE) {
                switch (frontendOptions_g.trackNo) {
                    case 1:
                        break;
                    case 2:
                        shipIndex[num].numToSpeed = boostFrames * 190;
                        break;
                    case 3:
                        break;
                    case 4:
                        break;
                    case 5:
                        break;
                    case 6:
                        shipIndex[num].numToSpeed = boostFrames * 140;
                        break;
                    case 7:
                        break;
                }
            } else if (raceType == RACE_CHALLENGE && frontendOptions_g.challengeType == CHALLENGE_TIME_TRIAL) {
                if (frontendOptions_g.trackNo == 2) {
                    shipIndex[num].numToSpeed = boostFrames * 175;
                }
            } else if (raceType == RACE_CHAMPIONSHIP && frontendOptions_g.trackNo == 2) {
                shipIndex[num].numToSpeed = boostFrames * 175;
            }
            break;
        case CLASS_PHANTOM:
            shipIndex[shipNo].numToSpeed = boostFrames * (s16)g_spreadScalePhantom[cfg->trackNum];
            if (raceType == RACE_CHALLENGE && frontendOptions_g.challengeType == CHALLENGE_RACE) {
                switch (frontendOptions_g.trackNo) {
                    case 1:
                        break;
                    case 2:
                        break;
                    case 3:
                        shipIndex[num].numToSpeed = boostFrames * 200;
                        break;
                    case 4:
                        break;
                    case 5:
                        shipIndex[num].numToSpeed = boostFrames * 210;
                        break;
                    case 6:
                        break;
                    case 7:
                        shipIndex[num].numToSpeed = boostFrames * 165;
                        break;
                }
            }
            break;
    }
#ifdef VERSION_EU
    /* PAL runs at 50 Hz: scale the 60 Hz start-boost frame count by 50/60. */
    shipIndex[num].numToSpeed = (shipIndex[num].numToSpeed * 50) / 60;
#endif
    if (num == ownShip) {
        shipIndex[num].numToSpeed = 0;
    }
    if (frontendOptions_g.numPlayers >= 2 && num == ownShip2) {
        shipIndex[num].numToSpeed = 0;
    }
    if (frontendOptions_g.numPlayers >= 3 && num == ownShip3) {
        shipIndex[num].numToSpeed = 0;
    }
    if (frontendOptions_g.numPlayers == 4 && num == ownShip4) {
        shipIndex[num].numToSpeed = 0;
    }
}

void SetGroupPrimColour(RenderGroup *group, u8 red, u8 green, u8 blue) {
    u32 i;
    Gfx *dl;

    for (i = 0; i < group->count; i++) {
        dl = group->entries[i].dl;
        while (dl->dma.cmd != G_ENDDL) {
            if (dl->dma.cmd == (s8)G_SETPRIMCOLOR) {
                gDPSetPrimColor(dl, 0, 0, red, green, blue, 0x5F);
            }
            dl++;
        }
    }
}

s32 DisplayListLength(Gfx *dl) {
    s32 count = 0;

    while (dl->dma.cmd != G_ENDDL) {
        dl++;
        count++;
    }
    return count + 1;
}

void CopyDisplayList(Gfx *dst, Gfx *src, s32 restoreOpaque) {
    while (src->dma.cmd != G_ENDDL) {
        *dst++ = *src++;
    }
    if (restoreOpaque == 1) {
        gDPPipeSync(dst++);
        gDPSetRenderMode(dst++, G_RM_ZB_OPA_SURF, G_RM_ZB_OPA_SURF2);
    }
    *dst = *src;
}

void InitGhostShipGroup(s32 shipNo) {
    Object *ship;
    u32 i;
    Gfx *dl;

    ship = g_shipShapes[ships[shipNo].lowResTableIndex];
    g_ghostShipGroup = DAlloc(g_heap, sizeof(RenderGroup), "Ships.c", 2821);
    g_ghostShipGroup->entries = DAlloc(g_heap, ((RenderGroup *)ship->bspTree)->count * sizeof(RenderGroupEntry),
                                       "Ships.c", 2822);
    if (g_ghostShipGroup != NULL && g_ghostShipGroup->entries != NULL) {
        g_ghostShipGroup->count = ((RenderGroup *)ship->bspTree)->count;
        g_ghostShipNo = shipNo;
        for (i = 0; i < ((RenderGroup *)ship->bspTree)->count; i++) {
            g_ghostShipGroup->entries[i].textureIndex = ((RenderGroup *)ship->bspTree)->entries[i].textureIndex;
            g_ghostShipGroup->entries[i].dl =
                DAlloc(g_heap, DisplayListLength(((RenderGroup *)ship->bspTree)->entries[i].dl) * sizeof(Gfx) | 3,
                       "Ships.c", 2844);
            dl = g_ghostShipGroup->entries[i].dl;
            gDPSetRenderMode(dl++, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF2);
            CopyDisplayList(dl, ((RenderGroup *)ship->bspTree)->entries[i].dl, 1);
        }
    }
}

void InitPlayerShipGroups(void) {
    s32 i;
    s32 shipNo;
    u32 j;
    Object *ship;
    Gfx *dl;

    for (i = 0; i < frontendOptions_g.numPlayers; i++) {
        switch (i) {
            case 0:
                shipNo = ownShip;
                break;
            case 1:
                shipNo = ownShip2;
                break;
            case 2:
                shipNo = ownShip3;
                break;
            case 3:
                if (g_fourPlayerSameTeam) {
                    shipNo = ownShip3;
                } else {
                    shipNo = ownShip4;
                }
                break;
        }
        ship = g_shipShapes[ships[shipNo].lowResTableIndex];

        g_playerShipGroup[i] = DAlloc(g_heap, sizeof(RenderGroup), "Ships.c", 2883);
        g_playerShipGroup[i]->entries = DAlloc(g_heap, ((RenderGroup *)ship->bspTree)->count * sizeof(RenderGroupEntry),
                                               "Ships.c", 2884);
        if (g_playerShipGroup[i] == NULL || g_playerShipGroup[i]->entries == NULL) {
            return;
        }
        g_playerShipGroup[i]->count = ((RenderGroup *)ship->bspTree)->count;
        for (j = 0; j < ((RenderGroup *)ship->bspTree)->count; j++) {
            g_playerShipGroup[i]->entries[j].textureIndex = ((RenderGroup *)ship->bspTree)->entries[j].textureIndex;
            g_playerShipGroup[i]->entries[j].dl =
                DAlloc(g_heap, DisplayListLength(((RenderGroup *)ship->bspTree)->entries[j].dl) * sizeof(Gfx),
                       "Ships.c", 2905);
            CopyDisplayList(g_playerShipGroup[i]->entries[j].dl, ((RenderGroup *)ship->bspTree)->entries[j].dl, 0);
        }

        if (shipNo >= 9 && shipNo <= 12) {
            g_playerShipXluGroup[i] = DAlloc(g_heap, sizeof(RenderGroup), "Ships.c", 2919);
            g_playerShipXluGroup[i]->entries =
                DAlloc(g_heap, ((RenderGroup *)ship->bspTree)->count * sizeof(RenderGroupEntry), "Ships.c", 2920);
            if (g_playerShipXluGroup[i] == NULL || g_playerShipXluGroup[i]->entries == NULL) {
                return;
            }
            g_playerShipXluGroup[i]->count = ((RenderGroup *)ship->bspTree)->count;
            for (j = 0; j < ((RenderGroup *)ship->bspTree)->count; j++) {
                g_playerShipXluGroup[i]->entries[j].textureIndex =
                    ((RenderGroup *)ship->bspTree)->entries[j].textureIndex;
                g_playerShipXluGroup[i]->entries[j].dl =
                    DAlloc(g_heap, DisplayListLength(((RenderGroup *)ship->bspTree)->entries[j].dl) * sizeof(Gfx) | 3,
                           "Ships.c", 2941);
                dl = g_playerShipXluGroup[i]->entries[j].dl;
                gDPSetRenderMode(dl++, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF2);
                CopyDisplayList(dl, ((RenderGroup *)ship->bspTree)->entries[j].dl, 1);
            }
        }
    }
}
