/*
 * cpu.c — Main CPU frame loop, RSP/RDP task builders, and track animators.
 *
 * Coordinates the top-level game state machine, 1P-4P F3DLX 3D display-list
 * tasks, per-track scenery animations (carts, steam vents, claws, flames), and
 * NTSC vs PAL task scheduling (USA submits a separate gspL3DEX_fifo task for
 * 3D lines; Europe removes L3DEX and expands GLIST_LEN from 0x880 to 0x900).
 */
#ident "$Revision: 1.17 $"
#ident "$Revision: 7.27 $"
#ident "$Revision: 1.5 $"
#ident "$Revision: 1.34 $"

#include "cpu.h"
#include "n64init.h"

/* ============================================================================
 * CPU Thread & Main Loop
 * ============================================================================ */

void CpuInit(void) {
    osCreateMesgQueue(&g_frameMsgQ, g_frameMsgs, NUM_FRAME_MSGS);
    osCreateMesgQueue(&g_retraceMsgQ, g_retraceMsgs, NUM_RETRACE_MSGS);
    osCreateThread(&g_cpuThread, CPU_THREAD_ID, CpuThreadEntry, NULL,
                   g_cpuThreadStack + CPU_STACKSIZE / sizeof(u64), CPU_THREAD_PRI);
    osStartThread(&g_cpuThread);
    osSendMesg(&g_frameMsgQ, (OSMesg)1, OS_MESG_BLOCK);
    InitGfxBuffer(1);
}

void UpdateAmbientSfx(u8 reset) {
    if (!reset) {
        if (g_ambientSfxTimer != 0) {
            switch (frontendOptions_g.trackNo) {
            case 1:
                PlayNote(0x28, 0xFF, 0xF, 0xF, 0x100);
                break;
            case 2:
                PlayNote(0x29, 0xFF, 0xF, 0xF, 0x100);
                break;
            case 3:
                PlayNote(0x2A, 0xFF, 0xF, 0xF, 0x100);
                break;
            case 4:
                PlayNote(0x2B, 0xFF, 0xF, 0xF, 0x100);
                break;
            case 5:
                PlayNote(0x2C, 0xFF, 0xF, 0xF, 0x100);
                break;
            case 6:
                PlayNote(0x2D, 0xFF, 0xF, 0xF, 0x100);
                break;
            case 7:
                PlayNote(0x2E, 0xFF, 0xF, 0xF, 0x100);
                break;
            }
            g_ambientSfxTimer--;
        }
    }
    if (reset) {
        g_ambientSfxTimer = 1;
    }
}

void CpuThreadEntry(void *arg) {
    OSTime startTime;
    OSTime curTime;
    OSMesg msg;
    s32 frameIdx;
    s32 i;

    startTime = osGetTime();
    do {
        curTime = osGetTime();
    } while (curTime < startTime + g_delayCycles);

    while (1) {
        osRecvMesg(&g_frameMsgQ, &msg, OS_MESG_BLOCK);
        if (g_gameState == 1 && g_loadDone == 0) {
            if (g_stateDelay == 0) {
                ExitFrontend();
                InitRace();
                if (g_attractMode != 1) {
                    g_raceReady = 0;
                } else {
                    g_raceReady = 1;
                }
                g_loadDone = 1;
                g_stateDelay = 30;
            } else {
                g_stateDelay--;
            }
        }
        if (g_gameState == 4 && g_frontendLoaded == 0) {
            if (g_stateDelay == 0) {
                if (g_firstBoot == 0) {
                    ResetRaceHeaps();
                    ReloadFrontend();
                } else {
                    InitFrontend();
                }
                startTime = osGetTime();
                curTime = osGetTime();
                if (g_firstBoot == 0) {
                    while (curTime < startTime + g_delayCycles) {
                        curTime = osGetTime();
                    }
                }
                g_firstBoot = 0;
                g_frontendLoaded = 1;
                g_stateDelay = 30;
            } else {
                g_stateDelay--;
            }
        }
        if (((u32)msg & 3) == 1) {
            frameIdx = (u32)msg >> 2;
            g_idleRunning = 0;
            g_curFrameIdx = frameIdx;
            SubmitGfxTasks(1 - frameIdx);
            switch (g_gameState) {
            case 0:
                InitGfxBuffer(frameIdx);
                break;
            case 1:
                if (g_loadDone) {
                    if (g_raceReady) {
                        if (UpdateRace()) {
                            RenderRace(frameIdx);
                            UpdateAmbientSfx(0);
                        } else {
                            UpdateAmbientSfx(1);
                        }
                        break;
                    }
                    for (i = 0; !g_raceReady && i < frontendOptions_g.numPlayers; i++) {
                        if ((g_contEnabled >> frontendOptions_g.controllerPort[i]) & 1) {
                            if (g_contButtonPressed[frontendOptions_g.controllerPort[i]] & 0x8000) {
                                g_raceReady = 1;
                                ReinitCont();
                                g_pauseDebounceTimer = 10;
                            }
                        }
                    }
                }
                CreateLoaderGfxTask(frameIdx);
                break;
            case 2:
            case 3:
                break;
            case 4:
                if (g_frontendLoaded) {
                    if (g_engineSfxPlaying) {
                        UpdateRumble(0, 1);
                        UpdateRumble(1, 1);
                        UpdateRumble(2, 1);
                        UpdateRumble(3, 1);
                        StopEngineSfx();
                        g_engineSfxPlaying = 0;
                        StopSfx(&g_shieldSfx);
                        StopSfx(&g_pitSfxHandle);
                        StopSfx(&g_droidVoice);
                        StopSfx(&g_chargeSfxHandle);
                    }
                    UpdateFrontend();
                    RenderFrontend(frameIdx);
                } else {
                    CreateLoaderGfxTask(frameIdx);
                }
                break;
            }
            osSendMesg(&g_retraceMsgQ, NULL, OS_MESG_BLOCK);
        }
    }
}

/* ============================================================================
 * Race Simulation Tick
 * ============================================================================ */

s32 UpdateRace(void) {
    u8 raceScreen[0xC88];
    u8 allFinished;
    s32 i;

    g_idleLimit = IDLE_LIMIT_FRAMES;
    allFinished = 1;
    if (g_attractMode == 1) {
        g_shipDeathCount[0] = -1;
        g_shipDeathCount[1] = -1;
        g_shipDeathCount[2] = -1;
        g_shipDeathCount[3] = -1;
    }

    if (frontendOptions_g.numPlayers >= 1) {
        UpdateSkyVisibility(g_cameras[0].section->secNo, ownShip);
    }
    if (frontendOptions_g.numPlayers >= 2) {
        UpdateSkyVisibility(g_cameras[1].section->secNo, ownShip2);
    }
    if (frontendOptions_g.numPlayers >= 3) {
        UpdateSkyVisibility(g_cameras[2].section->secNo, ownShip3);
    }
    if (frontendOptions_g.numPlayers == 4) {
        UpdateSkyVisibility(g_cameras[3].section->secNo, ownShip4);
    }

    UpdateCheatSequence(0);
    if (g_gameType == 1) {
        RecordGhostInput(ships);
    }

    CheckShipDestroyed(ships, &g_raceOverlay, &g_attractMode, g_idleLimit);

    if (g_pauseDebounceTimer == 0 || --g_pauseDebounceTimer == 0) {
        for (i = 0; g_pausePlayer == -1 && i < frontendOptions_g.numPlayers; i++) {
            if ((g_contEnabled >> frontendOptions_g.controllerPort[i]) & 1) {
                CheckGamePause(ships, &g_pauseState, &g_raceOverlay,
                               &g_padInfo[frontendOptions_g.controllerPort[i]].xpad,
                               &g_pauseState.musicPaused);
                if (g_pauseState.selection >= 0) {
                    g_pausePlayer = i;
                }
            }
        }
    }

    PerformPauseOperations(&g_pauseState, raceScreen, &g_raceOverlay, resetData, &g_pausePlayer);
    CheckForButtonPress();

    if (g_pauseState.selection == -1 && !g_enteringHiScore) {
        StartEngineSfx();
        UpdateShips(ships, weaponShapes, shipShapes, weaponData, &g_raceOverlay);
        AdjustEngineSound(&ships[ownShip], &g_cameras[0]);

        if (ships[ownShip].attr & SHIP_ATTR_RACING) {
            if (ships[ownShip].weaponType == 2 || ships[ownShip].weaponType == 5 ||
                ships[ownShip].weaponType == 18 || ships[ownShip].weaponType == 15) {
                MissileLock(ownShip, ships, ships[ownShip].weaponType);
                if (ships[ownShip].weaponType == 2) {
                    RearMissileLock(ownShip, ships);
                }
            }
            UpdateRumble(0, 0);
            ShakeScreen(0);
        } else {
            UpdateRumble(0, 0);
        }

        if (frontendOptions_g.numPlayers >= 2) {
            if (ships[ownShip2].attr & SHIP_ATTR_RACING) {
                if (ships[ownShip2].weaponType == 2 || ships[ownShip2].weaponType == 5 ||
                    ships[ownShip2].weaponType == 18 || ships[ownShip2].weaponType == 15) {
                    MissileLock(ownShip2, ships, ships[ownShip2].weaponType);
                    if (ships[ownShip2].weaponType == 2) {
                        RearMissileLock(ownShip2, ships);
                    }
                }
                UpdateRumble(1, 0);
                ShakeScreen(1);
            } else {
                UpdateRumble(1, 0);
            }
        }

        if (frontendOptions_g.numPlayers >= 3) {
            if (ships[ownShip3].attr & SHIP_ATTR_RACING) {
                if (ships[ownShip3].weaponType == 2 || ships[ownShip3].weaponType == 5 ||
                    ships[ownShip3].weaponType == 18 || ships[ownShip3].weaponType == 15) {
                    MissileLock(ownShip3, ships, ships[ownShip3].weaponType);
                    if (ships[ownShip3].weaponType == 2) {
                        RearMissileLock(ownShip3, ships);
                    }
                }
                UpdateRumble(2, 0);
                ShakeScreen(2);
            } else {
                UpdateRumble(2, 0);
            }
        }

        if (frontendOptions_g.numPlayers == 4) {
            if (ships[ownShip4].attr & SHIP_ATTR_RACING) {
                if (ships[ownShip4].weaponType == 2 || ships[ownShip4].weaponType == 5 ||
                    ships[ownShip4].weaponType == 18 || ships[ownShip4].weaponType == 15) {
                    MissileLock(ownShip4, ships, ships[ownShip4].weaponType);
                    if (ships[ownShip4].weaponType == 2) {
                        RearMissileLock(ownShip4, ships);
                    }
                }
                UpdateRumble(3, 0);
                ShakeScreen(3);
            } else {
                UpdateRumble(3, 0);
            }
        }

        for (i = 0; i < 32; i++) {
            electrics[i].life = -1;
        }
        UpdateSuperWeapons();
        ColourDroidLights(g_droidLightObj);
        if (frontendOptions_g.numPlayers == 1 && frontendOptions_g.trackNo == 1) {
            UpdateRotors();
        }
        if ((g_gameType == 6 && frontendOptions_g.gameMode != 2) ||
            ((g_gameType == 0 || g_gameType == 3) && frontendOptions_g.weaponsOn)) {
            if (g_gameType != 6 || frontendOptions_g.gameMode != 1 ||
                challenges[frontendOptions_g.challengeNo].weaponsEnabled) {
                UpdateWeaponGrids(ships, weaponGrid, noWeaponGrids);
            }
        }
    } else {
        UpdateRumble(0, 1);
        UpdateRumble(1, 1);
        UpdateRumble(2, 1);
        UpdateRumble(3, 1);
        StopEngineSfx();
        StopSfx(&g_shieldSfx);
        StopSfx(&g_pitSfxHandle);
        StopSfx(&g_droidVoice);
        StopSfx(&g_chargeSfxHandle);
    }

    RaceCompleteDListPtr = RaceCompleteDList;
    for (i = 0; i < frontendOptions_g.numPlayers; i++) {
        if (pauseRequests[i]) {
            RaceCompleteDListPtr = RaceComplete(raceScreen, resetData, &g_raceOverlay,
                                                &g_raceCompleteChoice, g_hudState, ships,
                                                &g_pauseState,
                                                g_padInfo[frontendOptions_g.controllerPort[0]].xpad,
                                                i, RaceCompleteDListPtr);
        }
        if (!pauseRequests[i]) {
            allFinished = 0;
        }
    }

    if (!allFinished) {
        g_resultSfxPending = 1;
        g_timeoutSfxPending = 1;
        if (attractDelayFrames > 0) {
            attractDelayFrames--;
            if ((g_attractMode == 1 && CheckAttractQuit(&g_raceOverlay)) ||
                attractDelayFrames == 0) {
                g_raceEndState = 2;
            }
        }
        if (g_enteringHiScore) {
            RaceCompleteDListPtr = RaceComplete(raceScreen, resetData, &g_raceOverlay,
                                                &g_raceCompleteChoice, g_hudState, ships,
                                                &g_pauseState,
                                                g_padInfo[frontendOptions_g.controllerPort[0]].xpad,
                                                i, RaceCompleteDListPtr);
        }
    }

    gSPEndDisplayList(RaceCompleteDListPtr++);

    if (g_raceEndState > 0) {
        if (!g_enteringHiScore) {
            g_raceEndState--;
        }
        if (g_raceEndState == 0) {
            QuitGame();
            return 0;
        }
    }

    if (g_pauseState.selection < 0) {
        UpdateShipTrails(ships, attractDelayFrames);
        g_cameraUpdated = 1;
        CameraUpdate(&g_cameras[0], &ships[ownShip], &rescueData[0], ownShip);
        if (frontendOptions_g.numPlayers >= 2) {
            CameraUpdate2(&g_cameras[1], &ships[ownShip2], &rescueData[1], ownShip2);
        }
        if (frontendOptions_g.numPlayers >= 3) {
            CameraUpdate3(&g_cameras[2], &ships[ownShip3], &rescueData[2], ownShip3);
        }
        if (frontendOptions_g.numPlayers >= 4) {
            CameraUpdate4(&g_cameras[3], &ships[ownShip4], &rescueData[3], ownShip4);
        }

        if (g_shipDeathCount[ownShip] == DEATH_UNRESCUE_FRAME) {
            ships[ownShip].attr &= ~SHIP_ATTR_VIEW_IN;
        }
        if (g_shipDeathCount[ownShip2] == DEATH_UNRESCUE_FRAME) {
            ships[ownShip2].attr &= ~SHIP_ATTR_VIEW_IN;
        }
        if (g_shipDeathCount[ownShip3] == DEATH_UNRESCUE_FRAME) {
            ships[ownShip3].attr &= ~SHIP_ATTR_VIEW_IN;
        }
        if (g_shipDeathCount[ownShip4] == DEATH_UNRESCUE_FRAME) {
            ships[ownShip4].attr &= ~SHIP_ATTR_VIEW_IN;
        }

        for (i = 0; i < 15; i++) {
            if (!(ships[i].combatAttr & SHIP_DESTROYED)) {
                if (g_shipDeathCount[i] > 0) {
                    if ((i == ownShip && g_shipDeathCount[ownShip] >= 16) ||
                        (i == ownShip2 && g_shipDeathCount[ownShip2] >= 16) ||
                        (i == ownShip3 && g_shipDeathCount[ownShip3] >= 16) ||
                        (i == ownShip4 && g_shipDeathCount[ownShip4] >= 16)) {
                        ships[i].ppivot.vx += ships[i].vpivot.vx >> 6;
                        ships[i].ppivot.vy += ships[i].vpivot.vy >> 6;
                        ships[i].ppivot.vz += ships[i].vpivot.vz >> 6;
                        ships[i].vpivot.vx -= ships[i].vpivot.vx >> 3;
                        ships[i].vpivot.vz -= ships[i].vpivot.vz >> 3;
                        ships[i].vpivot.vy -= ships[i].vpivot.vy >> 3;
                        ships[i].vhdg = 0;
                        ships[i].roll += (ships[i].vpivot.vx + ships[i].vpivot.vz) >> 5;
                    }
                    if (g_shipDeathCount[i] < 15) {
                        s32 spread = (15 - g_shipDeathCount[i]) << 6;

                        /* PAL (50 Hz) spawns death explosions every 5 frames vs. every 8 on NTSC (60 Hz). */
#ifdef VERSION_EU
                        if (g_shipDeathCount[i] % 5 == 1) {
#else
                        if ((g_shipDeathCount[i] & 7) == 7) {
#endif
                            AddExplode(ships[i].ppivot.vx + (newRand() % spread) - (spread >> 1),
                                       ships[i].ppivot.vy - (newRand() % 127),
                                       ships[i].ppivot.vz + (newRand() % spread) - (spread >> 1),
                                       -1);
                        }
                        if (g_shipDeathCount[i] % 6 == 4) {
                            AddSmokeExplode(ships[i].ppivot.vx + (newRand() % spread) - (spread >> 1),
                                            ships[i].ppivot.vy - (newRand() % 127),
                                            ships[i].ppivot.vz + (newRand() % spread) - (spread >> 1),
                                            -1);
                        }
                        spread = (20 - g_shipDeathCount[i]) << 6;
                        if (g_shipDeathCount[i] % SPURT_MOD == 1) {
                            AddSpurtExplode(ships[i].ppivot.vx + (newRand() % spread) - (spread >> 1),
                                            ships[i].ppivot.vy - (newRand() % 127),
                                            ships[i].ppivot.vz + (newRand() % spread) - (spread >> 1),
                                            -1);
                        }
                    }
                    if (g_shipDeathCount[i] == 5) {
                        PlayNote(10, 0xFF, 0xF, 0xF, 0x100);
                        PlayNote(10, 0xFF, 0xF, 0xF, 0x100);
                        PlayNote(10, 0xFF, 0xF, 0xF, 0x100);
                    }
                    if ((newRand() & 3) == 1 && g_shipDeathCount[i] >= 16) {
                        AddSmallSmokeExplode(ships[i].ppivot.vx - (ships[i].unitVecNose.vx >> 4),
                                             ships[i].ppivot.vy - (ships[i].unitVecNose.vy >> 4),
                                             ships[i].ppivot.vz - (ships[i].unitVecNose.vz >> 4),
                                             -1);
                    }
                    if ((g_shipDeathCount[i] & 3) == 1 && g_shipDeathCount[i] >= 16) {
                        AddSmallSpurtExplode((newRand() & 0x1FF) - (ships[i].unitVecNose.vx >> 5) - 0x100,
                                             -(ships[i].unitVecNose.vy >> 5) + 0x80,
                                             (newRand() & 0x1FF) - (ships[i].unitVecNose.vz >> 5) - 0x100,
                                             i);
                    }
                    g_shipDeathCount[i]--;
                    if (i == ownShip) {
                        SetRumble(10, 0);
                    } else if (i == ownShip2) {
                        SetRumble(10, 1);
                    } else if (i == ownShip3) {
                        SetRumble(10, 2);
                    } else if (i == ownShip4) {
                        SetRumble(10, 3);
                    }
                    if (g_shipDeathCount[i] == 0) {
                        if (i != ownShip && i != ownShip2 && i != ownShip3 && i != ownShip4) {
                            ships[i].attr &= ~SHIP_ATTR_RACING;
                            MusStartEffect2(0x3A, 0x100, 0x80, 1, 0x100);
                        }
                        /* Cast nudges loop.c insn count past the 496-insn hoist threshold on PAL so
                         * &frontendOptions_g isn't hoisted ahead of weaponData; folds to identical lhu/ori/sh. */
                        ships[i].combatAttr = (s16)ships[i].combatAttr | SHIP_DESTROYED;
                        if (frontendOptions_g.numPlayers == 1 &&
                            (i == ownShip || i == ownShip2 || i == ownShip3 || i == ownShip4)) {
                            FireSuperWeapon(&ships[i], weaponData, i, 3);
                        } else if (frontendOptions_g.numPlayers == 2 &&
                                   (i == ownShip || i == ownShip2 || i == ownShip3 || i == ownShip4)) {
                            FireSuperWeapon(&ships[i], weaponData, i, 2);
                        } else {
                            FireSuperWeapon(&ships[i], weaponData, i, 1);
                        }
                    }
                }
            }
        }

        for (i = 0; i < frontendOptions_g.numPlayers; i++) {
            UpdateStartDroid(startDroidObjects, ships, checkPoints, g_startDroidTexBase,
                             &g_raceOverlay, g_pauseState.selection, i);
        }
        UpdateLightStart(lightGroups, ships, checkPoints, g_startDroidTexBase,
                         &g_raceOverlay, g_pauseState.selection);
    }

    if (g_restartRaceFlag) {
        if (g_enteringHiScore) {
            return 1;
        }
        ResetRace(resetData, &g_pauseState, 2);
    }
    return 1;
}

/* ============================================================================
 * Split-Screen Viewports & Race Rendering
 * ============================================================================ */

Gfx *SetupRaceViewports(s32 playerIdx, Dynamic *dp, u16 *perspNorm, Gfx *glistp) {
    if (frontendOptions_g.splitScreenMode == 1) {
        switch (playerIdx) {
        case 0:
            guPerspective(&dp->projection, perspNorm, 82.224f, 2.0f / 3.0f, 16.0f, (f32)g_farClip, 1.0f);
            guMtxCatL(&dp->projection, &g_flipYMtx, &dp->projection);
            guOrtho(&dp->ortho, -160.0f, 160.0f, -120.0f, 120.0f, 0.0f, 4000.0f, 1.0f);
            guMtxCatL(&dp->ortho, &g_flipYMtx, &dp->ortho);
            gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&dp->projection),
                      G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
            gSPPerspNormalize(glistp++, *perspNorm);
            gSPViewport(glistp++, &g_vpLeftHalf);
            break;
        case 1:
            guPerspective(&dp->projection, perspNorm, 82.224f, 4.0f / 3.0f, 16.0f, (f32)g_farClip, 1.0f);
            guMtxCatL(&dp->projection, &g_flipYMtx, &dp->projection);
            guOrtho(&dp->ortho, -160.0f, 160.0f, -120.0f, 120.0f, 0.0f, 4000.0f, 1.0f);
            guMtxCatL(&dp->ortho, &g_flipYMtx, &dp->ortho);
            gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&dp->projection),
                      G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
            gSPPerspNormalize(glistp++, *perspNorm);
            gSPViewport(glistp++, &g_vpTopRight);
            break;
        case 2:
            guPerspective(&dp->projection, perspNorm, 82.224f, 4.0f / 3.0f, 16.0f, (f32)g_farClip, 1.0f);
            guMtxCatL(&dp->projection, &g_flipYMtx, &dp->projection);
            guOrtho(&dp->ortho, -160.0f, 160.0f, -120.0f, 120.0f, 0.0f, 4000.0f, 1.0f);
            guMtxCatL(&dp->ortho, &g_flipYMtx, &dp->ortho);
            gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&dp->projection),
                      G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
            gSPPerspNormalize(glistp++, *perspNorm);
            gSPViewport(glistp++, &g_vpBottomRight);
            break;
        }
    } else if (frontendOptions_g.splitScreenMode == 3) {
        switch (playerIdx) {
        case 0:
            guPerspective(&dp->projection, perspNorm, 82.224f, 8.0f / 3.0f, 16.0f, (f32)g_farClip, 1.0f);
            guMtxCatL(&dp->projection, &g_flipYMtx, &dp->projection);
            guOrtho(&dp->ortho, -160.0f, 160.0f, -120.0f, 120.0f, 0.0f, 4000.0f, 1.0f);
            guMtxCatL(&dp->ortho, &g_flipYMtx, &dp->ortho);
            gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&dp->projection),
                      G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
            gSPPerspNormalize(glistp++, *perspNorm);
            gSPViewport(glistp++, &g_vpTopHalf);
            break;
        case 1:
            guPerspective(&dp->projection, perspNorm, 82.224f, 4.0f / 3.0f, 16.0f, (f32)g_farClip, 1.0f);
            guMtxCatL(&dp->projection, &g_flipYMtx, &dp->projection);
            guOrtho(&dp->ortho, -160.0f, 160.0f, -120.0f, 120.0f, 0.0f, 4000.0f, 1.0f);
            guMtxCatL(&dp->ortho, &g_flipYMtx, &dp->ortho);
            gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&dp->projection),
                      G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
            gSPPerspNormalize(glistp++, *perspNorm);
            gSPViewport(glistp++, &g_vpBottomLeft);
            break;
        case 2:
            guPerspective(&dp->projection, perspNorm, 82.224f, 4.0f / 3.0f, 16.0f, (f32)g_farClip, 1.0f);
            guMtxCatL(&dp->projection, &g_flipYMtx, &dp->projection);
            guOrtho(&dp->ortho, -160.0f, 160.0f, -120.0f, 120.0f, 0.0f, 4000.0f, 1.0f);
            guMtxCatL(&dp->ortho, &g_flipYMtx, &dp->ortho);
            gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&dp->projection),
                      G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
            gSPPerspNormalize(glistp++, *perspNorm);
            gSPViewport(glistp++, &g_vpBottomRight);
            break;
        }
    } else {
        switch (playerIdx) {
        case 0:
            gSPViewport(glistp++, &g_vpTopLeft);
            break;
        case 1:
            gSPViewport(glistp++, &g_vpTopRight);
            break;
        case 2:
            gSPViewport(glistp++, &g_vpBottomLeft);
            break;
        }
    }
    return glistp;
}

void RenderRace(s32 frameIdx) {
    u16 perspNorm;
    Gfx *taskStart0;
    Gfx *taskStart5;
    Gfx *taskStart4;
    Gfx *taskStart2;
    Gfx *taskStart1;
    GFXInfo *info;
    Dynamic *dp;
    Gfx *glistp;
    s32 i = 0;
    s32 vpBase;
    f64 aspectWidth;
    f64 aspectHeight;

    switch (frontendOptions_g.numPlayers) {
    case 1:
        vpBase = 0;
        break;
    case 2:
        vpBase = 3;
        if (frontendOptions_g.splitScreenMode == 1) {
            vpBase = 1;
        }
        break;
    case 3:
    case 4:
        vpBase = 5;
        break;
    }

    info = &g_gfxInfo[frameIdx];
    dp = &info->dp;
    glistp = dp->glist;
    info->mode = 1;

    gDPFullSync(glistp++);
    gSPEndDisplayList(glistp++);
    SetupGfxTask(info, 6, dp->glist, glistp);

    g_taskFlags[6] = 22;
    g_mtxIndex = 0;
    g_mtxPool = (N64Mtx *)dp->mtxPool;

    taskStart4 = glistp;
    gSPSegment(glistp++, 0, 0);
    gSPSegment(glistp++, STATIC_SEGMENT, OS_K0_TO_PHYSICAL(g_staticSegment));
    gSPSegment(glistp++, CFB_SEGMENT, OS_K0_TO_PHYSICAL(info->cfb));

    if (frontendOptions_g.numPlayers == 2) {
        aspectWidth = 2.0;
        if (frontendOptions_g.splitScreenMode != 1) {
            aspectWidth = 8.0;
        }
        aspectHeight = 3.0;
    } else {
        aspectWidth = 4.0;
        aspectHeight = 3.0;
    }
    guPerspective(&dp->projection, &perspNorm, 82.224f, aspectWidth / aspectHeight,
                  16.0f, (f32)g_farClip, 1.0f);
    guMtxCatL(&dp->projection, &g_flipYMtx, &dp->projection);
    guOrtho(&dp->ortho, -160.0f, 160.0f, -120.0f, 120.0f, 0.0f, 4000.0f, 1.0f);
    guMtxCatL(&dp->ortho, &g_flipYMtx, &dp->ortho);

    gSPDisplayList(glistp++, g_rdpInitDL);
    gSPViewport(glistp++, &g_viewports[i]);
    gSPDisplayList(glistp++, g_rspInitDL);
    gSPDisplayList(glistp++, g_clearZBufferDL);
    gSPDisplayList(glistp++, g_clearCfbDL);
    gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&dp->ortho),
              G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPDisplayList(glistp++, g_initDls[3]);

    g_trackDListPtr = g_skyDlPool;
    for (i = 0; i < frontendOptions_g.numPlayers; i++) {
        if (skyVisible[i]) {
            if (frontendOptions_g.numPlayers == 3) {
                glistp = SetupRaceViewports(i, dp, &perspNorm, glistp);
            } else {
                gSPViewport(glistp++, &g_viewports[vpBase + i]);
            }
            glistp = DrawSky(&g_cameras[i], glistp);
        } else if (frontendOptions_g.trackNo == 2) {
            if (frontendOptions_g.numPlayers == 3) {
                glistp = SetupRaceViewports(i, dp, &perspNorm, glistp);
            } else {
                gSPViewport(glistp++, &g_viewports[vpBase + i]);
            }
            glistp = DrawSkyHorizon(&g_cameras[i], glistp);
        }
    }

    gDPFullSync(glistp++);
    gSPEndDisplayList(glistp++);
    SetupGfxTask(info, 4, taskStart4, glistp);

    g_taskFlags[4] = 20;
    taskStart1 = glistp;
    gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&dp->projection),
              G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPSegment(glistp++, 0, 0);
    gSPSegment(glistp++, STATIC_SEGMENT, OS_K0_TO_PHYSICAL(g_staticSegment));
    gSPSegment(glistp++, CFB_SEGMENT, OS_K0_TO_PHYSICAL(info->cfb));
    gSPDisplayList(glistp++, g_rdpInitDL);
    gSPDisplayList(glistp++, g_rspInitDL);
    gSPDisplayList(glistp++, g_initDls[1]);
    gSPPerspNormalize(glistp++, perspNorm);

    g_trackVtxCursor = 0;
    g_trackDListPtr = g_trackDLists[g_trackBufIdx];
    for (i = 0; i < frontendOptions_g.numPlayers; i++) {
        if (frontendOptions_g.numPlayers == 3) {
            glistp = SetupRaceViewports(i, dp, &perspNorm, glistp);
        } else {
            gSPViewport(glistp++, &g_viewports[vpBase + i]);
        }
        glistp = DrawNewTrack(&g_cameras[i], i, glistp);
    }

    if (frontendOptions_g.numPlayers < 3) {
        gSPDisplayList(glistp++, g_initDls[2]);
        for (i = 0; i < frontendOptions_g.numPlayers; i++) {
            if (frontendOptions_g.numPlayers == 3) {
                glistp = SetupRaceViewports(i, dp, &perspNorm, glistp);
            } else {
                gSPViewport(glistp++, &g_viewports[vpBase + i]);
            }
            switch (i) {
            case 0:
                glistp = DrawLandscape(&ships[ownShip], &g_cameras[0], glistp);
                break;
            case 1:
                glistp = DrawLandscape(&ships[ownShip2], &g_cameras[1], glistp);
                break;
            case 2:
                glistp = DrawLandscape(&ships[ownShip3], &g_cameras[2], glistp);
                break;
            case 3:
                glistp = DrawLandscape(&ships[ownShip4], &g_cameras[3], glistp);
                break;
            }
        }
    }

    gDPFullSync(glistp++);
    gSPEndDisplayList(glistp++);
    SetupGfxTask(info, 1, taskStart1, glistp);

    g_taskFlags[1] = 17;
    taskStart0 = glistp;
    gSPSegment(glistp++, 0, 0);
    gSPSegment(glistp++, STATIC_SEGMENT, OS_K0_TO_PHYSICAL(g_staticSegment));
    gSPSegment(glistp++, CFB_SEGMENT, OS_K0_TO_PHYSICAL(info->cfb));
    gSPDisplayList(glistp++, g_rdpInitDL);
    gSPDisplayList(glistp++, g_rspInitDL);
    gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&dp->projection),
              G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPDisplayList(glistp++, g_initDls[0]);
    gSPPerspNormalize(glistp++, perspNorm);

    for (i = 0; i < frontendOptions_g.numPlayers; i++) {
        if (frontendOptions_g.numPlayers == 3) {
            glistp = SetupRaceViewports(i, dp, &perspNorm, glistp);
        } else {
            gSPViewport(glistp++, &g_viewports[vpBase + i]);
        }
        gDPSetRenderMode(glistp++, G_RM_ZB_OPA_SURF, G_RM_ZB_OPA_SURF2);
        glistp = DrawStartDroidAndLights(glistp, &g_cameras[i], i);
        glistp = rescueData[i].update(&rescueData[i], &ships[frontendOptions_g.ownShips[i]],
                                      shipShapes, frontendOptions_g.ownShips[i], glistp);
        switch (i) {
        case 0:
            g_multiPlayer = (g_pauseState.selection == -1);
            glistp = DrawShips(ships, shipShapes, frontendOptions_g.ownShips[0], &g_cameras[0], 0, glistp);
            if (frontendOptions_g.trackNo == 4) {
                UpdateSpinners();
                glistp = DrawSpinners(&g_cameras[0], glistp);
            } else if (frontendOptions_g.trackNo == 3) {
                UpdateSliders();
            } else if (frontendOptions_g.trackNo == 2) {
                UpdateSparks();
            } else if (frontendOptions_g.trackNo == 5) {
                UpdateAnimTextures4();
            } else if (frontendOptions_g.trackNo == 6) {
                UpdateHoverers();
            }
            glistp = UpdateWeapons(ships, weaponShapes, weaponData, &g_cameras[0], glistp);
            g_multiPlayer = 0;
            break;
        case 1:
            glistp = DrawShips(ships, shipShapes, frontendOptions_g.ownShips[1], &g_cameras[1], i, glistp);
            if (frontendOptions_g.trackNo == 4) {
                glistp = DrawSpinners(&g_cameras[1], glistp);
            }
            glistp = UpdateWeapons(ships, weaponShapes, weaponData, &g_cameras[1], glistp);
            break;
        case 2:
            glistp = DrawShips(ships, shipShapes, frontendOptions_g.ownShips[2], &g_cameras[2], i, glistp);
            if (frontendOptions_g.trackNo == 4) {
                glistp = DrawSpinners(&g_cameras[2], glistp);
            }
            glistp = UpdateWeapons(ships, weaponShapes, weaponData, &g_cameras[2], glistp);
            break;
        case 3:
            glistp = DrawShips(ships, shipShapes, frontendOptions_g.ownShips[3], &g_cameras[3], i, glistp);
            if (frontendOptions_g.trackNo == 4) {
                glistp = DrawSpinners(&g_cameras[3], glistp);
            }
            glistp = UpdateWeapons(ships, weaponShapes, weaponData, &g_cameras[3], glistp);
            break;
        }
    }

    gDPFullSync(glistp++);
    gSPEndDisplayList(glistp++);
    SetupGfxTask(info, 0, taskStart0, glistp);

    g_taskFlags[0] = 16;
    taskStart5 = glistp;
    gSPSegment(glistp++, 0, 0);
    gSPSegment(glistp++, STATIC_SEGMENT, OS_K0_TO_PHYSICAL(g_staticSegment));
    gSPSegment(glistp++, CFB_SEGMENT, OS_K0_TO_PHYSICAL(info->cfb));
    gSPDisplayList(glistp++, g_rdpInitDL);
    gSPDisplayList(glistp++, g_rspInitDL);
    gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&dp->projection),
              G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPDisplayList(glistp++, g_initDls[5]);
    gSPPerspNormalize(glistp++, perspNorm);

    resetMiscFx();
    ResetRenderList(&g_effectRenderList);
    if (g_pauseState.selection == -1) {
        for (i = 0; i < frontendOptions_g.numPlayers; i++) {
            g_trackDListPtr = g_effectDLists[i];
            glistp = DrawEffects(weaponData, ships, i, dp, &perspNorm, glistp);
        }
    }
    glistp = FlushTrackRenderList(&g_effectRenderList, glistp);

    ResetRenderList(&g_trailRenderList);
    if (g_pauseState.selection == -1) {
        /* PAL iterates split-screen trail draws in reverse player order. */
#ifdef VERSION_EU
        for (i = frontendOptions_g.numPlayers - 1; i >= 0; i--) {
#else
        for (i = 0; i < frontendOptions_g.numPlayers; i++) {
#endif
            g_trackDListPtr = g_trailDLists[i];
            DrawTrailEffects(dp, &perspNorm, i);
        }
    }
    glistp = FlushTrackRenderList(&g_trailRenderList, glistp);

    gDPFullSync(glistp++);
    gSPEndDisplayList(glistp++);
    SetupGfxTask(info, 5, taskStart5, glistp);

    g_taskFlags[5] = 21;
    ClearCycloneCharges();
    ResetRenderList(&g_lineRenderList);
    taskStart2 = glistp;
    gSPSegment(glistp++, 0, 0);
    gSPSegment(glistp++, STATIC_SEGMENT, OS_K0_TO_PHYSICAL(g_staticSegment));
    gSPSegment(glistp++, CFB_SEGMENT, OS_K0_TO_PHYSICAL(info->cfb));
    gSPDisplayList(glistp++, g_rdpInitDL);
    gSPDisplayList(glistp++, g_rspInitDL);

    if (g_gameType == 6 && g_playerFinished[0] == 1 && !g_raceOverlay.unk1A && !g_hudShowPause) {
        gDPSetDepthImage(glistp++, OS_K0_TO_PHYSICAL(g_zBuffer));
        gDPPipeSync(glistp++);
        gDPSetCycleType(glistp++, G_CYC_FILL);
        gDPSetColorImage(glistp++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 320, OS_K0_TO_PHYSICAL(g_zBuffer));
        gDPSetFillColor(glistp++, (GPACK_ZDZ(G_MAXFBZ, 0) << 16) | GPACK_ZDZ(G_MAXFBZ, 0));
        gDPFillRectangle(glistp++, 0, 80, 319, 192);
        gDPPipeSync(glistp++);
        gDPSetColorImage(glistp++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 320, OS_K0_TO_PHYSICAL(info->cfb));
        gDPPipeSync(glistp++);
        gDPSetCycleType(glistp++, G_CYC_1CYCLE);
        gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&dp->projection),
                  G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
        gSPDisplayList(glistp++, g_initDls[2]);
        gSPPerspNormalize(glistp++, perspNorm);
        ResetRenderList(&g_medalRenderList);
        g_trackDListPtr = g_medalDList;
        glistp = DrawMedalEffects(dp, &perspNorm, glistp);
        glistp = FlushTrackRenderList(&g_medalRenderList, glistp);
    }
    /* NTSC uses task 2 for L3DEX line effects when not showing the finish medal; PAL omits L3DEX. */
#ifndef VERSION_EU
    else {
        gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&dp->projection),
                  G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
        gSPDisplayList(glistp++, g_initDls[5]);
        gSPPerspNormalize(glistp++, perspNorm);
        if (g_pauseState.selection == -1) {
            for (i = 0; i < frontendOptions_g.numPlayers; i++) {
                g_trackDListPtr = g_lineDLists[i];
                glistp = DrawLineEffects(weaponData, ships, i, dp, &perspNorm, glistp);
            }
        }
        glistp = FlushTrackRenderList(&g_lineRenderList, glistp);
    }
#endif

    gDPFullSync(glistp++);
    gSPEndDisplayList(glistp++);
    SetupGfxTask(info, 2, taskStart2, glistp);

    g_taskFlags[2] = 18;
    gDPFullSync(glistp++);
    gSPEndDisplayList(glistp++);
    info->overlayDListPtr = (s32)glistp;
    Gfx2DRaceTask(glistp, info);
    UpdateEffectAnims();
    g_trackBufIdx ^= 1;
    if (g_frameDelayFlag) {
        s32 j;

        for (j = 0; j < 416; j++) {
        }
    }
    osWritebackDCache(&info->dp, (u32)glistp - (u32)&info->dp);
}

/* ============================================================================
 * RSP / RDP Task Setup
 * ============================================================================ */

void SetupGfxTask(GFXInfo *info, s32 taskIdx, Gfx *glistStart, Gfx *glistEnd) {
    OSScTask *t = &info->task[taskIdx];

    t->list.t.ucode_boot = rspbootTextStart;
    t->list.t.ucode_boot_size = (u32)n_aspMainTextStart - (u32)rspbootTextStart;
    /* PAL dropped L3DEX and uses F3DLX for every graphics task. */
#ifdef VERSION_EU
    t->list.t.ucode = gspF3DLX_fifoTextStart;
    t->list.t.ucode_data = gspF3DLX_fifoDataStart;
#else
    if (taskIdx != GFX_TASK_3D || (g_gameType == 6 && g_playerFinished[0] == 1)) {
        t->list.t.ucode = gspF3DLX_fifoTextStart;
        t->list.t.ucode_data = gspF3DLX_fifoDataStart;
    } else {
        t->list.t.ucode = gspL3DEX_fifoTextStart;
        t->list.t.ucode_data = gspL3DEX_fifoDataStart;
    }
#endif
    t->list.t.data_ptr = (u64 *)glistStart;
    t->list.t.data_size = ((s32)glistEnd - (s32)glistStart) * sizeof(Gfx);
    t->list.t.type = M_GFXTASK;
    t->list.t.ucode_data_size = SP_UCODE_DATA_SIZE;
    t->list.t.dram_stack = g_dramStack;
    t->list.t.dram_stack_size = SP_DRAM_STACK_SIZE8;
    t->list.t.output_buff = g_rdpFifo;
    t->list.t.output_buff_size = g_rdpFifo + RDP_FIFO_LEN;
    t->list.t.yield_data_ptr = g_yieldData;
    t->list.t.yield_data_size = OS_YIELD_DATA_SIZE;
    t->msgQ = &g_schedMsgQ;
    t->list.t.flags = 0;
    t->next = NULL;
    t->msg = (OSMesg)&info->taskMsg[taskIdx];
    t->flags = OS_SC_NEEDS_RSP | OS_SC_NEEDS_RDP;
    t->framebuffer = info->cfb;
    t->totalTime = 0;
}

void InitGfxBuffer(s32 bufferIdx) {
    GFXInfo *info = &g_gfxInfo[bufferIdx];
    Gfx *glistp = info->dp.glist;
    OSScTask *t;

    info->mode = 2;
    gSPSegment(glistp++, 0, 0);
    gSPSegment(glistp++, STATIC_SEGMENT, OS_K0_TO_PHYSICAL(g_staticSegment));
    gSPSegment(glistp++, CFB_SEGMENT, OS_K0_TO_PHYSICAL(info->cfb));
    gSPDisplayList(glistp++, g_rdpInitDL);
    gSPDisplayList(glistp++, g_clearCfbDL);
    gDPFullSync(glistp++);
    gSPEndDisplayList(glistp++);
    SetupGfxTask(info, GFX_TASK_CLEAR, info->dp.glist, glistp);
    t = &info->task[GFX_TASK_CLEAR];
    t->flags = OS_SC_NEEDS_RSP | OS_SC_NEEDS_RDP | OS_SC_LAST_TASK | OS_SC_SWAPBUFFER;
    t->msg = (OSMesg)&info->doneMsg;
}

/* ============================================================================
 * Track Scenery Animations (Rotors, Sliders, Spinners, Hoverers, Sparks, Textures)
 * ============================================================================ */

void UpdateRotors(void) {
    s32 i;

    for (i = 0; i < NUM_ROTORS; i++) {
        if (g_rotorObjects[i] != NULL) {
            SetSkeletonDirection(g_rotorObjects[i]->skeleton, 0, g_rotorAngle, 0);
        }
    }
    g_rotorAngle += 32;
    if (g_rotorAngle > 4096) {
        g_rotorAngle -= 4096;
    }
}

void AddRotor(Object *obj) {
    g_rotorObjects[g_rotorCount++] = obj;
    if (g_rotorCount == NUM_ROTORS) {
        g_rotorCount = 0;
    }
}

void LoadAnimTextures(void) {
    WADFILE *wad = OpenWadFile((u32)_effectTexturesFile);

    wad->current = EFFECT_ANIM_TEXTURES;
    g_animTexStart = TextureTableCount;
    RegisterTextureSequence(WADLoadCompressedTextureSequence(wad));
    g_animTexCount = TextureTableCount - g_animTexStart;
    CloseWadFile(wad);
}

void ResetSliders(void) {
    s32 i;

    for (i = 0; i < NUM_SLIDERS; i++) {
        g_sliderObjects[i] = NULL;
    }
    g_animTexTarget = -1;
}

void AddSlider(Object *obj, s32 i) {
    g_sliderObjects[i] = obj;
    g_sliderBaseX[i] = obj->skeleton->relative.t[0];
    g_sliderBaseY[i] = g_sliderObjects[i]->skeleton->relative.t[1];
    g_sliderBaseZ[i] = g_sliderObjects[i]->skeleton->relative.t[2];
}

void InitAnimTexObject(Object *obj) {
    Primitive prim;
    s16 *coords;
    s16 count;
    s32 i;

    prim.type = obj->primitives;
    obj->flags |= OBJ_FLAG_TRANSLUCENT;
    count = obj->primitiveCount;
    for (i = 0; i < count; i++) {
        switch (*prim.type) {
        case TypeFT4:
            coords = prim.ft4->coords;
            ((Vtx *)obj->vtx)[coords[0]].v.cn[3] = 0xFF;
            ((Vtx *)obj->vtx)[coords[1]].v.cn[3] = 0;
            ((Vtx *)obj->vtx)[coords[2]].v.cn[3] = 0xFF;
            ((Vtx *)obj->vtx)[coords[3]].v.cn[3] = 0;
            g_animTexTarget = prim.ft4->texture + g_sceneRenderList.textureStart;
            prim.ft4 += 1;
            break;
        case TypeGT4:
            coords = prim.gt4->coords;
            ((Vtx *)obj->vtx)[coords[0]].v.cn[3] = 0xFF;
            ((Vtx *)obj->vtx)[coords[1]].v.cn[3] = 0;
            ((Vtx *)obj->vtx)[coords[2]].v.cn[3] = 0xFF;
            ((Vtx *)obj->vtx)[coords[3]].v.cn[3] = 0;
            g_animTexTarget = prim.gt4->texture + g_sceneRenderList.textureStart;
            prim.gt4 += 1;
            break;
        }
    }
}

void UpdateAnimTexture(s32 i) {
    N64Texture *dst = g_textureTable[g_animTexTarget];
    N64Texture *src = g_textureTable[g_animTexStart + g_animTexFrame[i]];

    if (dst->type == 0) {
        memcpy(dst->texels, src->texels, (dst->width * dst->height) >> 1);
    } else {
        memcpy(dst->texels, src->texels, dst->width * dst->height);
    }
    if (++g_animTexTimer[i] >= g_animTexDelay[i]) {
        if (++g_animTexFrame[i] >= g_animTexCount) {
            g_animTexFrame[i] = 0;
        }
        g_animTexTimer[i] = 1;
    }
}

void UpdateSlider(s32 i, s32 reset) {
    s32 startX, endX, startZ, endZ, duration;

    if (reset) {
        g_sliderTimer[i] = 0;
        return;
    }
    switch (i) {
    case 0:
        startX = 3000;
        endX = -4000;
        startZ = -1500;
        endZ = 2100;
        duration = 90;
        break;
    case 1:
        startX = -2700;
        endX = 4600;
        startZ = -610;
        endZ = 1050;
        duration = 60;
        break;
    case 2:
        startX = -2400;
        endX = 1600;
        startZ = 6000;
        endZ = -5000;
        duration = 150;
        break;
    }
    SetSkeletonPosition(g_sliderObjects[i]->skeleton,
                        g_sliderBaseX[i] + startX + g_sliderTimer[i] * (endX - startX) / duration,
                        g_sliderBaseY[i] + g_sliderYOffset,
                        g_sliderBaseZ[i] + startZ + g_sliderTimer[i] * (endZ - startZ) / duration);
    if (++g_sliderTimer[i] > duration) {
        g_sliderTimer[i] = duration;
    }
}

void UpdateSliders(void) {
    s32 i;

    for (i = 0; i < NUM_SLIDERS; i++) {
        if (g_sliderObjects[i] != NULL) {
            UpdateSlider(i, 0);
        }
    }
    if (g_sceneAnimFrame % 150 == 0) {
        UpdateSlider(0, 1);
    }
    if (g_sceneAnimFrame % 120 == 0) {
        UpdateSlider(1, 1);
    }
    if (g_sceneAnimFrame % 240 == 0) {
        UpdateSlider(2, 1);
    }
    if (g_animTexTarget != -1) {
        UpdateAnimTexture(0);
    }
    if (++g_sceneAnimFrame > 4320000) {
        g_sceneAnimFrame = 0;
    }
}

void ResetSpinners(void) {
    s32 i;

    for (i = 0; i < NUM_SPINNERS; i++) {
        g_spinnerObjects[i] = NULL;
    }
}

void UpdateSpinner(s32 i) {
    s32 xs[NUM_SPINNERS] = {
        21635, 26985, 32025, 31585, 29535, 27435, 23935, 22065,
        25435, 30535, 28495, 24955, 22985, 29635, 23555,
    };
    s32 zs[NUM_SPINNERS] = {
        -45891, -66631, -87950, -86061, -77301, -68531, -55781, -47631,
        -61611, -81640, -72901, -59831, -51731, -75891, -53900,
    };
    s32 speed = g_spinnerSpeed;

    if (i >= 0) {
        if (i < 8) {
            speed = -speed;
        } else if (i == 13) {
            speed = -speed;
        }
    }
    g_spinnerAngles[i] += speed;
    SetSkeletonDirectionHPR(g_spinnerObjects[i]->skeleton, g_spinnerPitch, g_spinnerRoll,
                            g_spinnerAngles[i]);
    SetSkeletonPosition(g_spinnerObjects[i]->skeleton, xs[i] >> 1, i >> 1, zs[i] >> 1);
}

void UpdateSpinners(void) {
    s32 i;

    for (i = 0; i < NUM_SPINNERS; i++) {
        if (g_spinnerObjects[i] != NULL) {
            UpdateSpinner(i);
        }
    }
}

void LoadSpinners(void) {
    s32 first = 0;
    s16 i;
    WADFILE *wad;
    void *tex;
    s16 textureStart;
    Object *obj;

    if (g_multiplayerFlag) {
        first = 13;
    }
    i = first;
    wad = OpenWadFile((u32)_effectTexturesFile);
    textureStart = TextureTableCount;
    if (g_multiplayerFlag) {
        wad->current++;
        tex = WADLoadCompressedTextureSequence(wad);
    } else {
        tex = WADLoadCompressedTextureSequence(wad);
        wad->current++;
    }
    RegisterTextureSequence(tex);
    InitRenderList(&g_spinnerRenderList, textureStart, TextureTableCount - textureStart);
    if (g_multiplayerFlag) {
        wad->current++;
        g_spinnerObjects[first] = WADLoadPrm(wad, &g_spinnerRenderList);
    } else {
        g_spinnerObjects[first] = WADLoadPrm(wad, &g_spinnerRenderList);
        wad->current++;
    }
    for (obj = g_spinnerObjects[first]; obj != NULL; obj = obj->next) {
        g_spinnerObjects[i] = obj;
        i++;
    }
    wad->current = 8;
    CloseWadFile(wad);
}

void LoadAnimTextures2(void) {
    WADFILE *wad = OpenWadFile((u32)_effectTexturesFile);

    wad->current = EFFECT_ANIM_TEXTURES;
    g_animTex2Start = TextureTableCount;
    RegisterTextureSequence(WADLoadCompressedTextureSequence(wad));
    g_animTex2Count = TextureTableCount - g_animTex2Start;
    CloseWadFile(wad);
}

void InitAnimTex2Object(Object *obj) {
    Primitive prim;
    s16 *coords;
    s16 count;
    s32 i;

    prim.type = obj->primitives;
    obj->flags |= OBJ_FLAG_TRANSLUCENT;
    count = obj->primitiveCount;
    for (i = 0; i < count; i++) {
        switch (*prim.type) {
        case TypeFT4:
            coords = prim.ft4->coords;
            ((Vtx *)obj->vtx)[coords[0]].v.cn[3] = 0xFF;
            ((Vtx *)obj->vtx)[coords[1]].v.cn[3] = 0;
            ((Vtx *)obj->vtx)[coords[2]].v.cn[3] = 0xFF;
            ((Vtx *)obj->vtx)[coords[3]].v.cn[3] = 0;
            g_animTex2Target = prim.ft4->texture + g_sceneRenderList.textureStart;
            prim.ft4 += 1;
            break;
        case TypeGT4:
            coords = prim.gt4->coords;
            ((Vtx *)obj->vtx)[coords[0]].v.cn[3] = 0xFF;
            ((Vtx *)obj->vtx)[coords[1]].v.cn[3] = 0;
            ((Vtx *)obj->vtx)[coords[2]].v.cn[3] = 0xFF;
            ((Vtx *)obj->vtx)[coords[3]].v.cn[3] = 0;
            g_animTex2Target = prim.gt4->texture + g_sceneRenderList.textureStart;
            prim.gt4 += 1;
            break;
        }
    }
}

void UpdateAnimTexture2(void) {
    N64Texture *dst = g_textureTable[g_animTex2Target];
    N64Texture *src = g_textureTable[g_animTex2Start + g_animTex2Frame];

    if (dst->type == 0) {
        memcpy(dst->texels, src->texels, (dst->width * dst->height) >> 1);
    } else {
        memcpy(dst->texels, src->texels, dst->width * dst->height);
    }
    if (++g_animTex2Timer >= g_animTex2Delay) {
        if (++g_animTex2Frame >= g_animTex2Count) {
            g_animTex2Frame = 0;
        }
        g_animTex2Timer = 1;
    }
}

void ResetHoverers(void) {
    s32 i;

    for (i = 0; i < NUM_HOVERERS; i++) {
        g_hoverObjects[i] = NULL;
    }
    g_animTex2Target = -1;
}

void UpdateHoverer(s32 i, s32 reset) {
    s32 startX, startY, startZ, endX, endY, endZ, duration;
    s32 half;

    if (reset) {
        g_hoverTimer[i] = 0;
        g_hoverDuration[i] = newRand();
        while (g_hoverDuration[i] < 0x2000) {
            g_hoverDuration[i] = newRand();
        }
        return;
    }
    switch (i) {
    case 0:
        startX = -3000;
        startY = 240;
        startZ = -80;
        endX = g_hoverDuration[0] * 5000 / 0x7FFF - 3000;
        endY = g_hoverDuration[0] * 30 / 0x7FFF + 240;
        endZ = g_hoverDuration[0] * 80 / 0x7FFF - 80;
        duration = g_hoverDuration[0] * 120 / 0x7FFF;
        break;
    case 1:
        startX = 2000;
        startY = 240;
        endX = -(g_hoverDuration[1] * 5000) / 0x7FFF + 2000;
        endY = startY;
        startZ = 20;
        endZ = -(g_hoverDuration[1] * 80) / 0x7FFF + 20;
        duration = g_hoverDuration[1] * 150 / 0x7FFF;
        break;
    }
    half = duration >> 1;
    if (g_hoverTimer[i] <= half) {
        SetSkeletonPosition(g_hoverObjects[i]->skeleton,
                            g_hoverBaseX[i] + startX + g_hoverTimer[i] * (endX - startX) / half,
                            g_hoverBaseY[i] + startY + g_hoverTimer[i] * (endY - startY) / half,
                            g_hoverBaseZ[i] + startZ + g_hoverTimer[i] * (endZ - startZ) / half);
    } else if (g_hoverTimer[i] <= duration) {
        SetSkeletonPosition(g_hoverObjects[i]->skeleton,
                            g_hoverBaseX[i] + endX + (g_hoverTimer[i] - half) * (startX - endX) / half,
                            g_hoverBaseY[i] + endY + (g_hoverTimer[i] - half) * (startY - endY) / half,
                            g_hoverBaseZ[i] + endZ + (g_hoverTimer[i] - half) * (startZ - endZ) / half);
    }
    if (++g_hoverTimer[i] > duration + 30) {
        g_hoverTimer[i] = 0;
        g_hoverDuration[i] = newRand();
        while (g_hoverDuration[i] < 0x2000) {
            g_hoverDuration[i] = newRand();
        }
    }
}

void AddHoverer(Object *obj, s32 i) {
    g_hoverObjects[i] = obj;
    g_hoverBaseX[i] = obj->skeleton->relative.t[0];
    g_hoverBaseY[i] = g_hoverObjects[i]->skeleton->relative.t[1];
    g_hoverBaseZ[i] = g_hoverObjects[i]->skeleton->relative.t[2];
    UpdateHoverer(i, 1);
}

void UpdateHoverers(void) {
    s32 i;

    for (i = 0; i < NUM_HOVERERS; i++) {
        if (g_hoverObjects[i] != NULL) {
            UpdateHoverer(i, 0);
        }
    }
    if (g_animTex2Target != -1) {
        UpdateAnimTexture2();
    }
}

void LoadFlipTextures(void) {
    WADFILE *wad = OpenWadFile((u32)_effectTexturesFile);
    s32 i;

    wad->current = EFFECT_FLIP_TEXTURES;
    g_animTex3Start = TextureTableCount;
    RegisterTextureSequence(WADLoadCompressedTextureSequence(wad));
    g_animTex3Count = TextureTableCount - g_animTex3Start;
    for (i = 0; i < g_animTex3Count; i++) {
        FlipTexture(g_textureTable[g_animTex3Start + i]);
    }
    CloseWadFile(wad);
}

void AddSparkPoint(Object *obj) {
    VECTOR *points;

    g_sparkPointCount++;
#ifdef VERSION_EU
    points = DAlloc(g_heap, g_sparkPointCount * sizeof(VECTOR), "cpu.c", 3256);
#else
    points = DAlloc(g_heap, g_sparkPointCount * sizeof(VECTOR), "cpu.c", 3258);
#endif
    if (g_sparkPointCount >= 2) {
        memcpy(points, g_sparkPoints, (g_sparkPointCount - 1) * sizeof(VECTOR));
        DFree(g_heap, g_sparkPoints);
    }
    points[g_sparkPointCount - 1].vx = obj->skeleton->relative.t[0] * 2;
    points[g_sparkPointCount - 1].vy = obj->skeleton->relative.t[1] * 2;
    g_sparkPoints = points;
    points[g_sparkPointCount - 1].vz = obj->skeleton->relative.t[2] * 2;
}

void InitAnimTex3Object(Object *obj) {
    Primitive prim;
    s32 i;

    prim.type = obj->primitives;
    g_animTex3Target = -1;
    for (i = 0; i < obj->primitiveCount; i++) {
        switch (*prim.type) {
        case TypeFT4:
            g_animTex3Target = prim.ft4->texture + g_scene2RenderList.textureStart;
            prim.ft4 += 1;
            break;
        case TypeGT4:
            g_animTex3Target = prim.gt4->texture + g_scene2RenderList.textureStart;
            prim.gt4 += 1;
            break;
        }
    }
}

void ResetSparkPoints(void) {
    g_animTex3Target = -1;
    g_sparkPointCount = 0;
    g_sparkPoints = NULL;
}

void UpdateAnimTexture3(void) {
    N64Texture *dst = g_textureTable[g_animTex3Target];
    N64Texture *src = g_textureTable[g_animTex3Start + g_animTex3Frame];

    if (dst->type == 0) {
        memcpy(dst->texels, src->texels, (dst->width * dst->height) >> 1);
    } else {
        memcpy(dst->texels, src->texels, dst->width * dst->height);
    }
    if (++g_animTex3Timer > 0) {
        if (++g_animTex3Frame >= g_animTex3Count) {
            g_animTex3Frame = 0;
        }
        g_animTex3Timer = 1;
    }
}

void UpdateSparks(void) {
    s32 i;

    if (g_animTex3Target != -1) {
        UpdateAnimTexture3();
    }
    if (frontendOptions_g.numPlayers == 1 && g_sparkPointCount != 0) {
        for (i = 0; i < 5; i++) {
            s32 idx = newRand() % g_sparkPointCount;

            AddParticle(particleSlots, 1, &g_sparkPoints[idx], 100);
        }
    }
}

void LoadAnimTextures4(void) {
    WADFILE *wad = OpenWadFile((u32)_effectTexturesFile);

    wad->current = EFFECT_ANIM_TEXTURES2;
    g_animTex4Start = TextureTableCount;
    RegisterTextureSequence(WADLoadCompressedTextureSequence(wad));
    g_animTex4Count = TextureTableCount - g_animTex4Start;
    CloseWadFile(wad);
}

void InitAnimTex4Object(Object *obj) {
    Primitive prim;
    s32 i;

    prim.type = obj->primitives;
    g_animTex4Target = -1;
    obj->flags |= OBJ_FLAG_TRANSLUCENT;
    for (i = 0; i < obj->primitiveCount; i++) {
        switch (*prim.type) {
        case TypeFT4:
            g_animTex4Target = prim.ft4->texture + g_sceneRenderList.textureStart;
            prim.ft4 += 1;
            break;
        case TypeGT4:
            g_animTex4Target = prim.gt4->texture + g_sceneRenderList.textureStart;
            prim.gt4 += 1;
            break;
        }
    }
}

void ResetAnimTexture4(void) {
    g_animTex4Target = -1;
}

void UpdateAnimTexture4(void) {
    N64Texture *dst = g_textureTable[g_animTex4Target];
    N64Texture *src = g_textureTable[g_animTex4Start + g_animTex4Frame];

    if (dst->type == 0) {
        memcpy(dst->texels, src->texels, (dst->width * dst->height) >> 1);
    } else {
        memcpy(dst->texels, src->texels, dst->width * dst->height);
    }
    if (++g_animTex4Timer >= 4) {
        if (++g_animTex4Frame >= g_animTex4Count) {
            g_animTex4Frame = 0;
        }
        g_animTex4Timer = 1;
    }
}

void UpdateAnimTextures4(void) {
    if (g_animTex4Target != -1) {
        UpdateAnimTexture4();
    }
}

Gfx *DrawSpinners(TrackCamera *cam, Gfx *glistp) {
    SVECTOR origin;
    VECTOR projected;
    s32 flag;
    s32 nearClip;
    s32 farClip;
    s32 count;
    s32 i;
    Object *obj;

    ResetRenderList(&g_spinnerRenderList);
    count = 0;
    origin.vx = 0;
    origin.vy = 0;
    origin.vz = 0;
    nearClip = g_sceneryNearClip[trackIndex];
    farClip = g_sceneryFarClip[trackIndex];
    if (frontendOptions_g.trackNo == 4) {
        count = NUM_SPINNERS;
    }
    for (i = 0; i < count; i++) {
        if (frontendOptions_g.trackNo == 4) {
            obj = g_spinnerObjects[i];
        } else {
            obj = NULL;
        }
        if (obj != NULL) {
            s32 extent;
            s32 nearZ;

            obj->skeleton->super = cam->camPos;
            obj->skeleton->update = 1;
            SetSkeletonTransform(obj->skeleton);
            extent = obj->extent >> 1;
            RotTrans(&origin, &projected, &flag);
            nearZ = projected.vz - extent;
            projected.vz += extent;
            if ((projected.vz > nearClip) && (nearZ < farClip)) {
                projected.vx = (projected.vx * 160) / projected.vz;
                projected.vy = (projected.vy * 160) / projected.vz;
                extent = (extent * 160) / projected.vz;
                if ((projected.vx >= -(120 + extent)) &&
                    (projected.vx <= (120 + extent)) &&
                    (projected.vy >= -(120 + extent)) &&
                    (projected.vy <= (120 + extent))) {
                    MatrixToN64Mtx(&obj->skeleton->absolute, &g_mtxPool[g_mtxIndex]);
                    g_sceneryPrimCount += obj->primitiveCount;
                    AddRenderGroup(&g_spinnerRenderList, (RenderGroup *)obj->bspTree,
                                   (Mtx *)&g_mtxPool[g_mtxIndex]);
                    g_mtxIndex++;
                }
            }
        }
    }
    glistp = FlushRenderList(&g_spinnerRenderList, glistp);
    return glistp;
}

/* ============================================================================
 * Challenge Finish Medal & Color Gamma Lookup
 * ============================================================================ */

void DrawFinishMedal(TrackCamera *cameras, void *lights) {
    if (g_playerFinished[0] == 1 && g_challengeMedalLevel > 0) {
        /* NTSC caches the medal DL/Vtx per medal level; PAL rebuilds it each frame. */
#ifndef VERSION_EU
        if (g_medalDL == NULL || g_medalVtx == NULL || g_challengeMedalLevel != g_lastMedalLevel) {
#endif
            g_lastMedalLevel = g_challengeMedalLevel;
            if (g_medalDL != NULL) {
                DFree(g_heap, g_medalDL);
            }
            if (g_medalVtx != NULL) {
                DFree(g_heap, g_medalVtx);
            }
            BuildEnvMapDisplayList(challengeMedalObject, lights, 1, g_challengeMedalLevel,
                                   &g_medalVtx, &g_medalDL);
#ifndef VERSION_EU
        }
#endif
        if (frontendOptions_g.gameMode != 2) {
            SetSkeletonPosition(cameras[1].camPos, 0, 0, 2000);
            SetSkeletonPosition(g_medalCamSkel, 0, 0, 2000);
            SetSkeletonPosition(challengeMedalObject->skeleton, 2300, 560, 0);
        } else {
            SetSkeletonPosition(cameras[1].camPos, 0, 0, 1950);
            SetSkeletonPosition(g_medalCamSkel, 0, 0, 1950);
            SetSkeletonPosition(challengeMedalObject->skeleton, 100, 540, 0);
        }
        SetSkeletonDirection(cameras[1].camPos, 0, 0, 0);
        SetSkeletonDirection(g_medalCamSkel, 0, 0, 0);
        challengeMedalObject->skeleton->super = cameras[1].camPos;
        g_medalAngle += 20;
        if (g_medalAngle >= 4096) {
            g_medalAngle -= 4096;
        }
        SetSkeletonDirectionHPR(challengeMedalObject->skeleton, g_medalAngle, 0, 0);
        MatrixToN64Mtx(GetSkeletonMatrix(challengeMedalObject->skeleton), &g_mtxPool[g_mtxIndex]);
        gSPMatrix(g_trackDListPtr++, OS_K0_TO_PHYSICAL(&g_mtxPool[g_mtxIndex]),
                  G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
        gSPDisplayList(g_trackDListPtr++, g_medalDL);
        g_mtxIndex++;
    }
}

/* Color gamma lookup helper used by track/ship lighting. */
u8 GammaCorrectColor(u8 i) {
    return g_colorGammaLut[i];
}
