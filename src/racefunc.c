#include "racefunc.h"

/*
 * Racefunc.c's initialized data (PauseDList, PauseDListPtr, RaceCompleteDList,
 * RaceCompleteDListPtr, g_numberStrings, g_pauseDListMaxLen, g_raceCompleteDListMaxLen)
 * currently lives in main_data, and the leading 0x48 bytes of racefunc_rodata
 * ($Revision: 1.17 $ and "EIGHT".."ONE", US 0x4BB0..0x4BF8, EU 0x4BA0..0x4BE8)
 * remain there while main_data is unsplit. This file emits the 0x149 bytes of
 * function .rodata (US 0x4BF8..0x4D41, EU 0x4BE8..0x4D31) and zero .data/.bss.
 */

void CheckShipDestroyed(RaceShipData *shipArr, RaceConfigData *config, void *unused, s32 idleLimit) {
    s32 i;
    u8 allDone;
    u8 allPaused;

    allDone = 1;
    allPaused = 1;

    if (g_idleTimer >= idleLimit) {
        for (i = 0; i < frontendOptions_g.numPlayers; i++) {
            if (pauseRequests[i] == 0) {
                allPaused = 0;
            }
        }
        if (allPaused) {
            for (i = 0; i < frontendOptions_g.numPlayers; i++) {
                if (g_raceOutcome[i] != 2) {
                    allDone = 0;
                }
            }
            if (allDone) {
                for (i = 0; i < frontendOptions_g.numPlayers; i++) {
                    pauseRequests[i] = 0;
                }
                config->complete = 0;
            }
        }
    }

    if ((shipArr[ownShip].combatAttr & SHIP_COMBAT_DESTROYED) && (shipArr[ownShip].attr & RACING)) {
        shipArr[ownShip].attr &= ~RACING;
        shipArr[ownShip].attr &= ~VIEW_IN;
#ifdef VERSION_EU
        CameraUpdate = UpdateCameraDestroyed;
#else
        if (frontendOptions_g.numPlayers >= 3) {
            CameraUpdate = UpdateCameraDestroyedMulti;
        } else if (!(shipArr[ownShip].combatAttr & 4)) {
            CameraUpdate = UpdateCameraDestroyed;
        }
#endif
        config->raceInfo[0] = 1;
        pauseRequests[0] = 1;
        g_raceOutcome[0] = 2;
        StopRumbleAll(0);
        g_finishPosition[0] = 15;
        copyInTempLapTimes(shipArr, 0);
    }

    if (frontendOptions_g.numPlayers >= 2) {
        if ((shipArr[ownShip2].combatAttr & SHIP_COMBAT_DESTROYED) && (shipArr[ownShip2].attr & RACING)) {
            shipArr[ownShip2].attr &= ~RACING;
            shipArr[ownShip2].attr &= ~VIEW_IN;
#ifdef VERSION_EU
            CameraUpdate2 = UpdateCameraDestroyed;
#else
            if (frontendOptions_g.numPlayers >= 3) {
                CameraUpdate2 = UpdateCameraDestroyedMulti;
            } else if (!(shipArr[ownShip2].combatAttr & 4)) {
                CameraUpdate2 = UpdateCameraDestroyed;
            }
#endif
            config->raceInfo[1] = 1;
            pauseRequests[1] = 1;
            g_raceOutcome[1] = 2;
            StopRumbleAll(1);
            g_finishPosition[1] = 15;
            copyInTempLapTimes(shipArr, 1);
        }
    }

    if (frontendOptions_g.numPlayers >= 3) {
        if ((shipArr[ownShip3].combatAttr & SHIP_COMBAT_DESTROYED) && (shipArr[ownShip3].attr & RACING)) {
            shipArr[ownShip3].attr &= ~RACING;
            shipArr[ownShip3].attr &= ~VIEW_IN;
#ifdef VERSION_EU
            CameraUpdate3 = UpdateCameraDestroyed;
#else
            if (frontendOptions_g.numPlayers >= 3) {
                CameraUpdate3 = UpdateCameraDestroyedMulti;
            } else if (!(shipArr[ownShip3].combatAttr & 4)) {
                CameraUpdate3 = UpdateCameraDestroyed;
            }
#endif
            config->raceInfo[2] = 1;
            pauseRequests[2] = 1;
            g_raceOutcome[2] = 2;
            StopRumbleAll(2);
            g_finishPosition[2] = 15;
            copyInTempLapTimes(shipArr, 2);
        }
    }

    if (frontendOptions_g.numPlayers == 4) {
        if ((shipArr[ownShip4].combatAttr & SHIP_COMBAT_DESTROYED) && (shipArr[ownShip4].attr & RACING)) {
            shipArr[ownShip4].attr &= ~RACING;
            shipArr[ownShip4].attr &= ~VIEW_IN;
#ifdef VERSION_EU
            CameraUpdate4 = UpdateCameraDestroyed;
#else
            if (frontendOptions_g.numPlayers >= 3) {
                CameraUpdate4 = UpdateCameraDestroyedMulti;
            } else if (!(shipArr[ownShip4].combatAttr & 4)) {
                CameraUpdate4 = UpdateCameraDestroyed;
            }
#endif
            config->raceInfo[3] = 1;
            pauseRequests[3] = 1;
            g_raceOutcome[3] = 2;
            StopRumbleAll(3);
            g_finishPosition[3] = 15;
            copyInTempLapTimes(shipArr, 3);
        }
    }

    allDone = 1;
    for (i = 0; i < frontendOptions_g.numPlayers; i++) {
        if (g_raceOutcome[i] != 2) {
            allDone = 0;
        }
    }
    if (allDone && g_raceEndState == 0) {
        attractDelayFrames = 1;
    }
}

void CheckForButtonPress(void) {
    s32 i;
    s16 buttons; /* left uninitialised in the original */

    for (i = 0; i < 4; i++) {
        if ((g_contEnabled >> i) & 1) {
            buttons |= g_padInfo[i].xpad;
        }
    }

    if (g_raceContext != 1 && D_800AA020 == 2) {
        if (buttons == 0) {
            g_idleTimer++;
        } else {
            g_idleTimer = 0;
        }
    }
}

void Restart(RaceResetData *reset, s32 mode) {
    s32 i;
    u8 unused[8]; /* leftover from XL InitCDAudio(gameData, unused) */

    StopRumbleAll(-1);

    D_800AD820 = 0;
    g_superWeaponActive1 = 0;
    g_superWeaponActive2 = 0;
    D_800CDE90 = 0;
    D_800CE6B8 = 0;
    D_800CE6BC = 0;
    D_800CE6C0 = 0;

    for (i = 0; i < 6; i++) {
        reset->checkPoints[i].reached = -1;
        checkPointReached[i][0] = -1;
        checkPointReached[i][1] = -1;
        checkPointReached[i][2] = -1;
        checkPointReached[i][3] = -1;
    }

    for (i = 0; i < 4; i++) {
        g_fireLock[i] = 0;
    }

    ResumeAudio();
    StopAudio();
    FlattenTrack();
    initShipData(track->sections, reset->ships, reset->config);

    for (i = 0; i < frontendOptions_g.numPlayers; i++) {
        InitRescueDroid(&rescueData[i], shipShapes, &ships[frontendOptions_g.ownShip[i]]);
    }

    if (g_gameType == 1 || (g_gameType == 6 && frontendOptions_g.challengeMode == 2)) {
        D_80091CD4 = -1;
        D_80091CD8 = 0;
        D_80091CD0 = 0;
        if (g_gameType == 1) {
            InitGhostShip(reset->config, reset->ships, 0);
        }
    }

    for (i = 0; i < frontendOptions_g.numPlayers; i++) {
        cameras[i].section = reset->ships[frontendOptions_g.ownShip[i]].nearTrkSect;
    }

    ResetStartBoom();
    ResetLightStart();

    for (i = 0; i < 52; i++) {
        reset->weapons[i].count = 0;
        reset->weapons[i].type = 0;
        reset->weaponShapes[i].used = 0;
    }

    for (i = 0; i < 15; i++) {
        D_8009BCE0[i] = 0;
        g_shipDeathCount[i] = -1;
    }

    func_80069CF8();
    InitExplode();
    InitRing();

    CameraUpdate = UpdateCameraRaceStart;
    InitCamera(&cameras[0], ships[ownShip].nearTrkSect);
    if (frontendOptions_g.numPlayers >= 2) {
        CameraUpdate2 = UpdateCameraRaceStart;
        InitCamera(&cameras[1], ships[ownShip2].nearTrkSect);
    }
    if (frontendOptions_g.numPlayers >= 3) {
        CameraUpdate3 = UpdateCameraRaceStart;
        InitCamera(&cameras[2], ships[ownShip3].nearTrkSect);
    }
    if (frontendOptions_g.numPlayers >= 4) {
        CameraUpdate4 = UpdateCameraRaceStart;
        InitCamera(&cameras[3], ships[ownShip4].nearTrkSect);
    }

    D_80096990 = 0;
    g_restartRaceFlag = 0;
    D_80091C94 = 0;
    g_raceCompleteFlag = 0;
    g_linkMyState = 0;
    g_raceEndState = 0;
    g_hudShowPause = 0;
    D_80096990 = 0;
    D_80091E70 = 0;

    for (i = 0; i < 4; i++) {
        D_80091E88[i] = 0;
        D_80091E84[i] = 0;
        g_raceOutcome[i] = 0;
        pauseRequests[i] = 0;
        func_8004986C(i, 1);
        D_8009204C[i] = 0;
        D_80096730[i] = 1;
        gameData.raceInfo[i] = 0;
    }

    g_droneSwitchStep = 0;
    g_continueAnimFrame = 0;
    g_idleTimer = 0;
    D_8008A982 = 0;
    D_8008A984 = 0;

    if (!(mode & 2)) {
        D_80091CBC = 0;
    }

    g_trackNameScroll = 480;
    InitScoreTabData(&g_scoreData);
    g_perfectLap = 0;
    D_800AD048 = 60;

    if (g_engineSfxPlaying != 0) {
        StopEngineSfx();
        g_engineSfxPlaying = 0;
        StopSfx(&D_800972F8);
        StopSfx(&D_800972FC);
        StopSfx(&g_droidVoice);
        StopSfx(&g_chargeSfxHandle);
    }
    if (g_engineSfxPlaying == 0) {
        StartEngineSfx();
        g_engineSfxPlaying = 1;
    }

    g_podiumShips[0] = -1;
    g_podiumShips[1] = -1;
    g_podiumShips[2] = -1;
    D_800969DC[0] = -1;
    D_800969DC[1] = -1;
    D_800969DC[2] = -1;
    D_80091F00 = 0;
    D_80091E04 = 0;
}

void QuitGame(void) {
    StopRumbleAll(-1);
    ResumeAudio();
    StopAudio();
    func_8004986C(0, 1);
    func_8004986C(1, 1);
    func_8004986C(2, 1);
    func_8004986C(3, 1);
    StopEngineSfx();
    g_engineSfxPlaying = 0;
    StopSfx(&D_800972F8);
    StopSfx(&D_800972FC);
    StopSfx(&g_droidVoice);
    StopSfx(&g_chargeSfxHandle);
    D_800AA020 = 2;
    if (g_gameType != 4) {
        g_gameType = 99;
    }
    g_gameState = 4;
    D_8008A9DC = 0;
}

u8 CheckAttractQuit(RaceConfigData *config) {
    if ((g_firstContPad.button == 0x1000 && !(ships[ownShip].combatAttr & SHIP_COMBAT_DESTROYED)) ||
        attractDelayFrames == 0) {
        config->unk0C = 99;
        g_gameType = 99;
        if (attractDelayFrames != 0) {
            g_attractQuitFlag = 1;
        }
        g_raceEndState = 1;
        return 1;
    }
    return 0;
}

Gfx *RaceComplete(void *screen, void *unused, RaceConfigData *config, s16 *choice,
                  RaceScoreData *score, RaceShipData *shipArr, void *pause, u16 pad,
                  s32 player, Gfx *glistp) {
    char text[32];
    u8 allRaceInfo;
    u8 allDone;
    u8 hasNextChallenge;
    u8 isLeague;
    s32 i;
    s32 j;
    s32 k;
    s32 circuitCount;
    u16 xstick;

    allDone = 1;
    hasNextChallenge = 0;
    allRaceInfo = 1;
    isLeague = 0;

    D_8009204C[player] = 0;
    func_8004986C(player, 1);

    xstick = g_padInfo[frontendOptions_g.contPort[0]].xstickPad;
    if (xstick & 0x10) {
        pad |= 0x10;
    }
    if (xstick & 0x40) {
        pad |= 0x40;
    }
    if (xstick & 0x80) {
        pad |= 0x80;
    }
    if (xstick & 0x20) {
        pad |= 0x20;
    }

    for (i = 0; i < frontendOptions_g.numPlayers; i++) {
        if (g_raceOutcome[i] != 2) {
            allDone = 0;
        }
    }

    if (config->complete) {
        if (g_gameType == 4 && (config->phantomChallenge != 0 || config->piranhaChallenge != 0) &&
            (g_finishPosition[0] == 1 || g_finishPosition[0] >= 4)) {
            if (g_finishPosition[0] == 1) {
                if (config->phantomChallenge == 1 && config->phantomCircuit == 5) {
                    config->phantomUnlocked = 1;
                    g_phantomDone = 1;
                    g_linkMyState = 3;
                    g_raceEndState = 2;
                    g_gameType = 99;
                } else if (config->piranhaChallenge == 1 && config->piranhaCircuit == 7) {
                    config->piranhaUnlocked = 1;
                    g_piranhaDone = 1;
                    g_linkMyState = 3;
                    g_raceEndState = 2;
                    g_gameType = 99;
                } else {
                    if (config->phantomChallenge == 1) {
                        glistp = CentreText("PHANTOM CHALLENGE", 60, 36, 8, 1, glistp);
                        circuitCount = config->phantomCircuit;
                    } else if (config->piranhaChallenge == 1) {
                        glistp = CentreText("PIRANHA CHALLENGE", 60, 36, 8, 1, glistp);
                        circuitCount = config->piranhaCircuit;
                    }
                    StringCopy(text, g_numberStrings[circuitCount]);
                    if (circuitCount == 0) {
                        sprintf(text, " CIRCUIT COMPLETE");
                    } else {
                        sprintf(text, " CIRCUITS COMPLETED");
                    }
                    glistp = CentreText(text, 60, 50, 8, 1, glistp);

                    if (GetMenuKey(0, pad)) {
                        PlayNote(26, 255, 15, 15, 256);
                        (*choice)--;
                        if (*choice < 0) {
                            *choice = 0;
                        }
                    }
                    if (GetMenuKey(1, pad)) {
                        PlayNote(26, 255, 15, 15, 256);
                        (*choice)++;
                        if (*choice > 1) {
                            *choice = 1;
                        }
                    }

                    if (*choice == 0) {
                        glistp = CentreText("NEXT CIRCUIT", 60, 110, 8, 0, glistp);
                        glistp = CentreText("END CHALLENGE", 60, 124, 8, 1, glistp);
                    } else if (*choice == 1) {
                        glistp = CentreText("NEXT CIRCUIT", 60, 110, 8, 1, glistp);
                        glistp = CentreText("END CHALLENGE", 60, 124, 8, 0, glistp);
                    }

                    glistp = DeSelectScreen(screen, glistp, 0);

                    if (GetMenuKey(5, pad)) {
                        PlayNote(27, 255, 15, 15, 256);
                        config->complete = 0;
                        for (j = 0; j < frontendOptions_g.numPlayers; j++) {
                            pauseRequests[j] = 0;
                        }
                        if (*choice == 0) {
                            if (config->phantomChallenge == 1) {
                                config->phantomCircuit++;
                            } else {
                                config->piranhaCircuit++;
                            }
                            g_linkMyState = 3;
                            g_raceEndState = 2;
                        } else if (*choice == 1) {
                            g_linkMyState = 3;
                            g_raceEndState = 2;
                        }
                    }
                }
            } else {
                glistp = DeSelectScreen(screen, glistp, 0);
                glistp = CentreText("YOU DID NOT QUALIFY", 60, 36, 8, 1, glistp);
                if (g_continuesLeft > 0) {
                    DisplayContinues(D_800AB548, 176, 168);
                    if (g_continueAnimFrame == 0) {
                        if (GetMenuKey(0, pad)) {
                            PlayNote(26, 255, 15, 15, 256);
                            (*choice)--;
                            if (*choice < 0) {
                                *choice = 0;
                            }
                        }
                        if (GetMenuKey(1, pad)) {
                            PlayNote(26, 255, 15, 15, 256);
                            (*choice)++;
                            if (*choice > 1) {
                                *choice = 1;
                            }
                        }
                    }
                    if (*choice == 0) {
                        glistp = CentreText("CONTINUE GAME", 60, 110, 8, 0, glistp);
                        glistp = CentreText("GO TO MENU", 60, 124, 8, 1, glistp);
                    } else if (*choice == 1) {
                        glistp = CentreText("CONTINUE GAME", 60, 110, 8, 1, glistp);
                        glistp = CentreText("GO TO MENU", 60, 124, 8, 0, glistp);
                    }
                } else {
                    glistp = CentreText("GO TO MENU", 60, 124, 8, 0, glistp);
                }

                glistp = DeSelectScreen(screen, glistp, 0);

                if (g_continueAnimFrame == 0) {
                    if (GetMenuKey(5, pad)) {
                        PlayNote(27, 255, 15, 15, 256);
                        if (*choice == 0 && g_continuesLeft > 0) {
                            g_continueAnimFrame = 1;
                            PlayNote(4, 480, 15, 15, 320);
                        } else if (*choice == 1 || g_continuesLeft == 0) {
                            config->complete = 0;
                            for (j = 0; j < frontendOptions_g.numPlayers; j++) {
                                pauseRequests[j] = 0;
                            }
                            g_linkMyState = 3;
                            g_raceEndState = 2;
                        }
                    }
                } else if (g_continueAnimFrame == 30) {
                    config->complete = 0;
                    for (j = 0; j < frontendOptions_g.numPlayers; j++) {
                        pauseRequests[j] = 0;
                    }
                    g_restartRaceFlag = 1;
                    g_continuesLeft--;
                } else {
                    g_continueAnimFrame++;
                }
            }
        } else if (D_800AA020 == 2) {
            if ((g_sixTracks == 1 && g_sixXPlayed == 0) ||
                (g_eightTracks == 1 && g_eightXPlayed == 0)) {
                g_raceEndState = 1;
                for (j = 0; j < frontendOptions_g.numPlayers; j++) {
                    pauseRequests[j] = 0;
                }
            } else {
                if (g_gameType == 6) {
                    hasNextChallenge = CheckNextChallenge(frontendOptions_g.challengeTable,
                                                          frontendOptions_g.challengeClass,
                                                          frontendOptions_g.challengeIndex);
                }
                if (g_gameType == 4) {
                    glistp = CentreText("MEDAL ACHIEVED", 60, 36, 8, 1, glistp);
                    glistp = CentreText("FREE CONTINUE AWARDED", 60, 50, 8, 1, glistp);
                }
                if (GetMenuKey(0, pad)) {
                    if (hasNextChallenge) {
                        if (*choice == 1) {
                            *choice = 2;
                            PlayNote(26, 255, 15, 15, 256);
                        } else if (*choice == 2) {
                            *choice = 0;
                            PlayNote(26, 255, 15, 15, 256);
                        }
                    } else {
                        (*choice)--;
                        if (*choice < 0) {
                            *choice = 0;
                        } else {
                            PlayNote(26, 255, 15, 15, 256);
                        }
                    }
                }
                if (GetMenuKey(1, pad)) {
                    if (hasNextChallenge) {
                        if (*choice == 0) {
                            *choice = 2;
                            PlayNote(26, 255, 15, 15, 256);
                        } else if (*choice == 2) {
                            *choice = 1;
                            PlayNote(26, 255, 15, 15, 256);
                        }
                    } else {
                        (*choice)++;
                        if (*choice > 1) {
                            *choice = 1;
                        } else {
                            PlayNote(26, 255, 15, 15, 256);
                        }
                    }
                }

                glistp = DeSelectScreen(screen, glistp, 0);

                if (*choice == 0) {
                    if (g_gameType == 6) {
                        glistp = CentreText("REPLAY CHALLENGE", 60, 110, 8, 0, glistp);
                    } else {
                        glistp = CentreText("REPLAY RACE", 60, 110, 8, 0, glistp);
                    }
                    if (hasNextChallenge) {
                        glistp = CentreText("NEXT CHALLENGE", 60, 124, 8, 1, glistp);
                        glistp = CentreText("GO TO MENU", 60, 138, 8, 1, glistp);
                    } else {
                        glistp = CentreText("GO TO MENU", 60, 124, 8, 1, glistp);
                    }
                } else if (*choice == 1) {
                    if (g_gameType == 6) {
                        glistp = CentreText("REPLAY CHALLENGE", 60, 110, 8, 1, glistp);
                    } else {
                        glistp = CentreText("REPLAY RACE", 60, 110, 8, 1, glistp);
                    }
                    if (hasNextChallenge) {
                        glistp = CentreText("NEXT CHALLENGE", 60, 124, 8, 1, glistp);
                        glistp = CentreText("GO TO MENU", 60, 138, 8, 0, glistp);
                    } else {
                        glistp = CentreText("GO TO MENU", 60, 124, 8, 0, glistp);
                    }
                } else if (*choice == 2) {
                    if (g_gameType == 6) {
                        glistp = CentreText("REPLAY CHALLENGE", 60, 110, 8, 1, glistp);
                    } else {
                        glistp = CentreText("REPLAY RACE", 60, 110, 8, 1, glistp);
                    }
                    glistp = CentreText("NEXT CHALLENGE", 60, 124, 8, 0, glistp);
                    glistp = CentreText("GO TO MENU", 60, 138, 8, 1, glistp);
                }

                if (GetMenuKey(5, pad)) {
                    PlayNote(27, 255, 15, 15, 256);
                    config->complete = 0;
                    for (j = 0; j < frontendOptions_g.numPlayers; j++) {
                        pauseRequests[j] = 0;
                    }
                    if (*choice == 0) {
                        g_restartRaceFlag = 1;
                    } else if (*choice == 1) {
                        g_gameType = 99;
                        g_linkMyState = 3;
                        g_raceEndState = 2;
                    } else if (*choice == 2) {
                        g_linkMyState = 3;
                        g_raceEndState = 2;
                        if (frontendOptions_g.challengeClass == g_challengeTable0) {
                            g_menuScreen = 9;
                        } else if (frontendOptions_g.challengeClass == g_challengeTable1) {
                            g_menuScreen = 10;
                        } else if (frontendOptions_g.challengeClass == g_challengeTable2) {
                            g_menuScreen = 11;
                        } else if (frontendOptions_g.challengeClass == g_challengeTable3) {
                            g_menuScreen = 12;
                        }
                        frontendOptions_g.challengeTable[frontendOptions_g.challengeIndex].completed = 1;
                        frontendOptions_g.challengeTable[frontendOptions_g.challengeIndex + 1].completed = 0;
                    }
                }
            }
        }
    } else if (config->raceInfo[player]) {
        /* Dead branch left over in retail; cleared by jump2 after reload, forcing player reload */
        if (allDone) {
            allDone = 0;
        }
        glistp = DisplayRaceInfo(config, score, shipArr, glistp, player);
        D_80092244 = 0;
    } else {
        if (config->highScore) {
            if (!allDone) {
                if (g_gameType != 3) {
                    CheckForHiScore(config, score, shipArr, ownShip, 0);
                } else {
                    isLeague = 1;
                }
            }
            if ((allDone && g_gameType == 3) || isLeague) {
                config->highScore = 0;
                if (!score->gotHiScore) {
                    config->complete = 0;
                    g_raceCompleteFlag = 1;
                    for (j = 0; j < frontendOptions_g.numPlayers; j++) {
                        pauseRequests[j] = 0;
                    }
                    g_linkMyState = 3;
                    g_raceEndState = 2;
                    func_800518C0(22);
                    g_menuScreen = 22;
                    g_leagueResults[g_podiumShips[0]].firstCount++;
                    g_leagueResults[g_podiumShips[1]].secondCount++;
                    g_leagueResults[g_podiumShips[2]].thirdCount++;
                    for (k = 0; k < 15; k++) {
                        if (shipArr[k].unkF4) {
                            g_leagueResults[k].points += shipArr[k].unkF0;
                        }
                    }
                    frontendOptions_g.unk7C--;
                }
            }
            config->highScore = 0;
            if (!score->gotHiScore) {
                g_raceCompleteFlag = 1;
                config->complete = 1;
            }
        }
        if (score->gotHiScore) {
            score->displayScore = 1;
        }
        if (score->displayScore) {
            glistp = DeSelectScreen(screen, glistp, 0);
            glistp = EnterHighScoreIfNeeded(config, score, pad, glistp);
            config->highScore = 0;
        }
    }

    if ((player == 0 && frontendOptions_g.numPlayers == 1) ||
        (player == 1 && frontendOptions_g.numPlayers == 2) ||
        (player == 2 && frontendOptions_g.numPlayers == 3) ||
        (player == 3 && frontendOptions_g.numPlayers == 4)) {
        for (i = 0; i < frontendOptions_g.numPlayers; i++) {
            if (config->raceInfo[i] == 0) {
                allRaceInfo = 0;
            }
        }
        if (allRaceInfo) {
            if (GetMenuKey(5, pad)) {
                PlayNote(27, 255, 15, 15, 256);
                for (i = 0; i < frontendOptions_g.numPlayers; i++) {
                    config->raceInfo[i] = 0;
                }
                config->highScore = 1;
            }
        }
    }

    if (config->complete) {
        InitScoreTabData(score);
    }

    return glistp;
}

void CheckGamePause(RaceShipData *shipArr, RacePauseMenu *pause, RaceConfigData *config,
                    u16 *xpad, s32 *musicPaused) {
    if ((shipArr[ownShip].attr & RACING) ||
        (frontendOptions_g.numPlayers >= 2 && (shipArr[ownShip2].attr & RACING)) ||
        (frontendOptions_g.numPlayers >= 3 && (shipArr[ownShip3].attr & RACING)) ||
        (frontendOptions_g.numPlayers == 4 && (shipArr[ownShip4].attr & RACING))) {
        if (GetMenuKey(6, *xpad) && pause->selection == -1) {
            PauseAudio();
            PlayNote(27, 255, 15, 15, 256);
            pause->selection = 0;
            *xpad = 0;
            StopRumbleAll(-1);
        }
    }
}

void SelectPauseOption(s32 *selection, s32 canMove, RaceConfigData *config,
                       RacePauseMenu *pause, u16 pad) {
    if (config->paused == 0) {
        if (GetMenuKey(0, pad) && canMove) {
            (*selection)--;
            if (*selection < 0) {
                *selection = 0;
            } else {
                PlayNote(26, 255, 15, 15, 256);
            }
        } else if (GetMenuKey(1, pad) && canMove) {
            (*selection)++;
            if (frontendOptions_g.numPlayers >= 2) {
                if (*selection > 3) {
                    *selection = 3;
                } else {
                    PlayNote(26, 255, 15, 15, 256);
                }
            } else {
                if (*selection > 2) {
                    *selection = 2;
                } else {
                    PlayNote(26, 255, 15, 15, 256);
                }
            }
        }
    }
}

s32 PerformPauseOperations(RacePauseMenu *pause, void *screen, RaceConfigData *config,
                           RaceResetData *reset, s32 *pausePlayer) {
    u16 pad;
    u16 xstick;
    u8 i;

    PauseDListPtr = PauseDList;

    if (pause->selection > -1 && D_8008A982 == 0) {
        if (*pausePlayer >= 0) {
            xstick = g_padInfo[frontendOptions_g.contPort[*pausePlayer]].xstickPad;
            pad = g_padInfo[frontendOptions_g.contPort[*pausePlayer]].xpad;
            if (xstick & 0x10) {
                pad |= 0x10;
            }
            if (xstick & 0x40) {
                pad |= 0x40;
            }
            if (xstick & 0x80) {
                pad |= 0x80;
            }
            if (xstick & 0x20) {
                pad |= 0x20;
            }

            SelectPauseOption(&pause->selection, pause->showOptions, config, pause, pad);

            if (pause->showOptions) {
                PauseDListPtr = DeSelectScreen(screen, PauseDListPtr, 0);
                if (frontendOptions_g.numPlayers >= 2) {
                    PauseDListPtr = DeSelectScreen(screen, PauseDListPtr, 1);
                }
                if (frontendOptions_g.numPlayers >= 3) {
                    PauseDListPtr = DeSelectScreen(screen, PauseDListPtr, 2);
                }
                if (frontendOptions_g.numPlayers == 4) {
                    PauseDListPtr = DeSelectScreen(screen, PauseDListPtr, 3);
                }
                PauseDListPtr = InGameReset(pause, config, screen, pad, PauseDListPtr);
                pause->confirmChoice = 0;
            } else {
                PauseDListPtr = DeSelectScreen(screen, PauseDListPtr, 0);
                if (frontendOptions_g.numPlayers >= 2) {
                    PauseDListPtr = DeSelectScreen(screen, PauseDListPtr, 1);
                }
                if (frontendOptions_g.numPlayers >= 3) {
                    PauseDListPtr = DeSelectScreen(screen, PauseDListPtr, 2);
                }
                if (frontendOptions_g.numPlayers == 4) {
                    PauseDListPtr = DeSelectScreen(screen, PauseDListPtr, 3);
                }
                PauseDListPtr = InGameConfirm(&pause->confirmed, pause->selection, pad,
                                              &pause->confirmChoice, &pause->frameCount, PauseDListPtr);
            }

            if (pause->confirmed == 0) {
                pause->showOptions = 1;
                pause->confirmed = 2;
                pause->selection = 0;
            }

            if (GetMenuKey(6, pad)) {
                pause->showOptions = 1;
                pause->confirmed = 2;
                pause->selection = -1;
                *pausePlayer = -1;
                pause->command = 0;
                ResumeAudio();
                gSPEndDisplayList(PauseDListPtr++);
                return 0;
            }

            if (GetMenuKey(5, pad) && pause->showOptions == 0 && pause->confirmed == 1 && pause->stage != 1) {
                pause->confirmed = 2;
                pause->showOptions = 1;
                if ((g_gameType == 0 || g_gameType == 1 || g_gameType == 6) &&
                    (pause->selection == 1 || pause->selection == 2) &&
                    g_scoreData.gotHiScore != 0) {
                    g_scoreData.displayScore = 1;
                    D_8008A9CC = 1;
                    if (ships[ownShip].update == UpdatePlayerShipStart) {
                        ships[ownShip].update = UpdateDroneShipStart;
                    } else {
                        ships[ownShip].update = UpdateDroneShip;
                    }
                    CameraUpdate = UpdateCameraAttractMode;
                    ships[ownShip].attr &= ~VIEW_IN;
                    ships[ownShip].attr &= ~RACING;
                    humanToDroneSpeed(ships, ownShip);
                    g_padInfo[frontendOptions_g.contPort[0]].xpad &= ~0x4000;
                }

                switch (pause->selection) {
                    case 0:
                        pause->selection = -1;
                        *pausePlayer = -1;
                        pause->command = 0;
                        ResumeAudio();
                        break;
                    case 1:
                        pause->selection = -1;
                        *pausePlayer = -1;
                        pause->command = 1;
                        if (g_gameType == 4) {
                            g_droneSwitchStep = 1;
                            D_80091CA8 = 1;
                        } else {
                            g_restartRaceFlag = 1;
                            D_80091CA8 = 1;
                        }
                        break;
                    case 2:
                        pause->selection = -1;
                        *pausePlayer = -1;
                        pause->command = 2;
                        if (g_gameType == 3) {
                            for (i = 0; i < frontendOptions_g.numPlayers; i++) {
                                pauseRequests[i] = 0;
                            }
                            g_linkMyState = 3;
                            g_raceEndState = 2;
                            func_800518C0(22);
                            g_menuScreen = 22;
                            pause->selection = -1;
                            *pausePlayer = -1;
                            pause->command = 2;
                            if (pause->stage == 2) {
                                config->unk0C = 99;
                                g_linkMyState = 3;
                                g_raceEndState = 2;
                            }
                        }
                        if (pause->stage == 2) {
                            config->unk0C = 99;
                            g_linkMyState = 3;
                            g_raceEndState = 2;
                            gSPEndDisplayList(PauseDListPtr++);
                            return 1;
                        }
                        break;
                    case 3:
                        pause->selection = -1;
                        *pausePlayer = -1;
                        pause->command = 2;
                        if (g_gameType == 3) {
                            func_800518C0(1);
                            g_menuScreen = 1;
                        }
                        if (pause->stage == 2) {
                            config->unk0C = 99;
                            g_linkMyState = 3;
                            g_raceEndState = 2;
                            gSPEndDisplayList(PauseDListPtr++);
                            return 1;
                        }
                        break;
                }
            }

            if (GetMenuKey(5, pad) && pause->showOptions != 0 && pause->stage != 1) {
                PlayNote(27, 255, 15, 15, 256);
                if (pause->selection == 1 || pause->selection == 2 || pause->selection == 3) {
                    pause->showOptions = 0;
                } else if (pause->selection == 0) {
                    ResumeAudio();
                    pause->command = 0;
                    pause->selection = -1;
                    *pausePlayer = -1;
                    D_80091CA8 = 1;
                }
            }
        }
    }

    gSPEndDisplayList(PauseDListPtr++);
    return 0;
}

void ResetRace(RaceResetData *reset, RacePauseMenu *pause, s32 mode) {
    Restart(reset, mode);
    InitRaceData(pause);
    if (mode != 1) {
        attractDelayFrames = 0;
    }
    InitAttractMode(mode, ships);
    gameData.complete = 0;
    func_80020160();
    D_800AD048 = 60;
}

Gfx *AppendPauseDList(Gfx *glistp) {
    s32 length;

    length = PauseDListPtr - PauseDList;
    if (length > g_pauseDListMaxLen) {
        g_pauseDListMaxLen = length;
        if (!(g_pauseDListMaxLen < PAUSE_DLIST_SIZE)) {
            __assert("MaxLength < PAUSE_DLIST_SIZE", "Racefunc.c", 1811);
        }
    }
    gSPDisplayList(glistp++, PauseDList);
    return glistp;
}

Gfx *AppendRaceCompleteDList(Gfx *glistp) {
    s32 length;

    length = RaceCompleteDListPtr - RaceCompleteDList;
    if (length > g_raceCompleteDListMaxLen) {
        g_raceCompleteDListMaxLen = length;
        if (!(g_raceCompleteDListMaxLen < RACE_COMPLETE_DLIST_SIZE)) {
            __assert("MaxLength < RACE_COMPLETE_DLIST_SIZE", "Racefunc.c", 1829);
        }
    }
    gSPDisplayList(glistp++, RaceCompleteDList);
    return glistp;
}

void InitGhostShip(RaceConfigData *config, RaceShipData *shipArr, s32 fromLap) {
    RaceShipData *shipPtr;

    g_ghostRecord.randSeed = g_randSeed;
    g_ghostRecord.keyIndex = 0;
    g_ghostRecord.keyCount = 0;
    g_ghostShip = 1;
    g_ghostRecord.padUnk00 = g_padInfo[frontendOptions_g.contPort[0]].unk00;
    g_ghostRecord.disabled = 0;
    g_ghostRecord.magic = GHOST_MAGIC;
    shipPtr = &shipArr[ownShip];
    g_ghostRecord.actTrackNum = config->actTrackNum;
    g_ghostRecord.shipClass = config->shipClass;
    memcpy(&g_ghostRecord.ship, shipPtr, sizeof(RaceShipData));

    g_ghostDisabled = 1;

    if (g_ghostReplay.actTrackNum == config->actTrackNum &&
        g_ghostReplay.shipClass == config->shipClass &&
        g_ghostReplay.disabled == 0 &&
        g_ghostReplay.magic == GHOST_MAGIC) {
        g_ghostEnabled = 1;
        g_ghostReplay.keyIndex = 0;
        g_randSeed = g_ghostReplay.randSeed;
        g_ghostPad.unk00 = g_ghostReplay.padUnk00;
        shipPtr = &shipArr[g_ghostShip];
        memcpy(shipPtr, &g_ghostReplay.ship, sizeof(RaceShipData));
        g_ghostDisabled = 0;
        shipArr[g_ghostShip].combatAttr |= 1;
        g_ghostPad.xpad = 0;
        shipArr[g_ghostShip].update = UpdatePlayerShip;
        shipArr[g_ghostShip].weapon = 9;
        if (fromLap == 0) {
            D_80091CD8 = 1;
            g_ghostBestLapTime = D_800A9E9C;
            g_ghostLapTime0 = D_800A9E9C;
        }
        g_ghostActive = fromLap;
    } else {
        g_ghostDisabled = 1;
        g_ghostEnabled = 0;
        g_ghostActive = 0;
        shipArr[g_ghostShip].update = UpdateDroneShipStart;
        shipArr[g_ghostShip].updateCount = GHOST_START_UPDATE_COUNT;
        shipArr[g_ghostShip].unkB2 = 0;
    }
}

void RecordGhostInput(RaceShipData *shipArr) {
    s32 idx;

    if (pauseData.selection == -1) {
        idx = g_ghostRecord.keyIndex;
        if (idx < GHOST_MAX_KEYS) {
            g_ghostRecord.keys[idx] = g_padInfo[frontendOptions_g.contPort[0]].pad & 0xEFF6;
            g_ghostRecord.keys[idx + 1] =
                (g_padInfo[frontendOptions_g.contPort[0]].unk08 << 8) | (s8)g_padInfo[0].stickX;
            g_ghostRecord.keys[idx + 2] = (s8)g_padInfo[frontendOptions_g.contPort[0]].stickY;
            g_ghostRecord.keyIndex += 3;
        }
        g_ghostRecord.keyCount = g_ghostRecord.keyIndex;
    }
}

void ReplayGhostInput(RaceShipData *shipArr) {
    u16 stickPacked;

    g_ghostPad.opad = g_ghostPad.pad;
    if (g_ghostDisabled == 0) {
        if (g_ghostReplay.keyIndex < g_ghostReplay.keyCount) {
            g_ghostPad.pad = g_ghostReplay.keys[g_ghostReplay.keyIndex];
            stickPacked = g_ghostReplay.keys[g_ghostReplay.keyIndex + 1];
            g_ghostPad.unk08 = stickPacked >> 8;
            g_ghostPad.stickX = stickPacked;
            g_ghostPad.stickY = g_ghostReplay.keys[g_ghostReplay.keyIndex + 2];
            g_ghostReplay.keyIndex += 3;
        } else {
            g_ghostDisabled = 1;
            shipArr[g_ghostShip].unkB2 = 0;
            shipArr[g_ghostShip].update = UpdateDroneShip;
            humanToDroneSpeed(shipArr, g_ghostShip);
        }
    }
    g_ghostPad.xpad = (g_ghostPad.pad ^ g_ghostPad.opad) & g_ghostPad.pad;
}

void StoreGhostReplay(void *unused, RaceConfigData *config) {
    s32 i;
    char *dst;
    char *src;

    if (g_ghostRecord.disabled == 0) {
        g_ghostRecord.bestLapTime = g_ghostBestLapTime;
        for (i = 0; i < 6; i++) {
            g_ghostRecord.lapTimes[i] = g_ghostLapTimes[i];
        }
        dst = (char *)&g_ghostReplay;
        src = (char *)&g_ghostRecord;
        memcpy(dst, src, sizeof(GhostReplay));
    }
}
