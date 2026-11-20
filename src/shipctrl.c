#include "shipctrl.h"

#ident "$Revision: 1.17 $"
#ident "$Revision: 7.27 $"
#ident "$Revision: 1.5 $"

#ifdef VERSION_EU
#define FRAMES_PER_SECOND       50
#define START_COUNTDOWN_FRAMES  166
#define START_Y_CAPTURE_FRAME   165
#define START_MUSIC_FRAME       156
#define LAUNCH_WINDOW_FRAMES    75
#define COUNTDOWN_STEP_1        41
#define COUNTDOWN_STEP_2        83
#define COUNTDOWN_STEP_3        125
#define MULTI_BONUS_FRAMES      1000
#define THRUST_INC              19
#define THRUST_DEC              9
#define THRUST_DEAD_DEC         76
#define PITCH_INC               36
#define DEATH_COUNT_1P          50
#define DEATH_COUNT_MP          5
#define STICK_X(c)              (((c)->stickX * 60) / 50)
#else
#define FRAMES_PER_SECOND       60
#define START_COUNTDOWN_FRAMES  200
#define START_Y_CAPTURE_FRAME   199
#define START_MUSIC_FRAME       190
#define LAUNCH_WINDOW_FRAMES    90
#define COUNTDOWN_STEP_1        33
#define COUNTDOWN_STEP_2        66
#define COUNTDOWN_STEP_3        100
#define MULTI_BONUS_FRAMES      1200
#define THRUST_INC              16
#define THRUST_DEC              8
#define THRUST_DEAD_DEC         64
#define PITCH_INC               30
#define DEATH_COUNT_1P          60
#define DEATH_COUNT_MP          6
#define STICK_X(c)              ((c)->stickX)
#endif

void UpdateShips(ShipData *shipIndex, WeaponShape *unused, Object **shipShapes,
                 WeaponData *weaponIndex) {
    s16 i;
    s16 activeCount;
    s16 shipNo;
    s16 shipNum;
    VECTOR lastPos;
    VECTOR direction; /* unused: retail reserves 0x38 bytes of stack locals */
    VECTOR trackPos;
    SVECTOR position;

    activeCount = 0;
    for (i = 0; i < NO_OF_SHIPS; i++) {
        if (!(shipIndex[i].combatAttr & SHIP_DESTROYED)) {
            activeCount++;
        }
    }

    if (g_shipUpdatePaused == 0 && activeCount > 0) {
        SortAllShips(shipIndex);
        for (i = 0; i < activeShipCount; i++) {
            shipNo = shipRank[i].shipNo;
            shipNum = shipNo;
            lastPos.vx = shipIndex[shipNo].ppivot.vx;
            lastPos.vy = shipIndex[shipNo].ppivot.vy;
            lastPos.vz = shipIndex[shipNo].ppivot.vz;

            switch (raceType) {
                case RACE_TIME_TRIAL:
                    if (shipNo == ownShip) {
                        shipIndex[shipNo].update(shipIndex, shipShapes, shipNo, weaponIndex,
                                                 &g_padInfo[frontendOptions_g.playerPad[0]]);
                    } else if (shipNo == g_demoShip) {
                        buildGhostReplayData(shipIndex);
                        shipIndex[shipNo].update(shipIndex, shipShapes, shipNo, weaponIndex,
                                                 &ghostControl);
                    }
                    break;

                case RACE_MULTIPLAYER:
                    if (shipNo == ownShip) {
                        shipIndex[shipNo].update(shipIndex, shipShapes, shipNo, weaponIndex,
                                                 &g_padInfo[frontendOptions_g.playerPad[0]]);
                        doShipShipCollision(shipNo, shipIndex, shipShapes);
                    } else if (shipNo == ownShip2) {
                        shipIndex[shipNo].update(shipIndex, shipShapes, shipNo, weaponIndex,
                                                 &g_padInfo[frontendOptions_g.playerPad[1]]);
                        doShipShipCollision(shipNo, shipIndex, shipShapes);
                    } else if (shipNo == ownShip3) {
                        shipIndex[shipNo].update(shipIndex, shipShapes, shipNo, weaponIndex,
                                                 &g_padInfo[frontendOptions_g.playerPad[2]]);
                        doShipShipCollision(shipNo, shipIndex, shipShapes);
                    } else if (shipNo == ownShip4) {
                        shipIndex[shipNo].update(shipIndex, shipShapes, shipNo, weaponIndex,
                                                 &g_padInfo[frontendOptions_g.playerPad[3]]);
                        doShipShipCollision(shipNo, shipIndex, shipShapes);
                    } else if (frontendOptions_g.numPlayers == 2 &&
                               frontendOptions_g.aiShipsEnabled != 0) {
                        shipIndex[shipNo].update(shipIndex, shipShapes, shipNo, weaponIndex,
                                                 &g_padInfo[frontendOptions_g.playerPad[0]]);
                        doShipShipCollision(shipNo, shipIndex, shipShapes);
                    } else if (frontendOptions_g.numPlayers == 3 &&
                               frontendOptions_g.aiShipsEnabled != 0) {
                        shipNo = shipNum;
                        shipIndex[shipNo].update(shipIndex, shipShapes, shipNo, weaponIndex,
                                                 &g_padInfo[frontendOptions_g.playerPad[0]]);
                        doShipShipCollision(shipNo, shipIndex, shipShapes);
                    }
                    break;

                case RACE_CHALLENGE:
                    if (frontendOptions_g.challengeType == CHALLENGE_TIME_TRIAL) {
                        if (shipNo == ownShip) {
                            shipIndex[shipNo].update(shipIndex, shipShapes, shipNo, weaponIndex,
                                                     &g_padInfo[frontendOptions_g.playerPad[0]]);
                        }
                    } else {
                        shipIndex[shipNo].update(shipIndex, shipShapes, shipNo, weaponIndex,
                                                 &g_padInfo[frontendOptions_g.playerPad[0]]);
                        doShipShipCollision(shipNo, shipIndex, shipShapes);
                    }
                    break;

                default:
                    shipNo = shipNum;
                    shipIndex[shipNo].update(shipIndex, shipShapes, shipNo, weaponIndex,
                                             &g_padInfo[frontendOptions_g.playerPad[0]]);
                    doShipShipCollision(shipNo, shipIndex, shipShapes);
                    break;
            }

            lastPos.vx = shipIndex[shipNum].ppivot.vx - lastPos.vx;
            lastPos.vy = shipIndex[shipNum].ppivot.vy - lastPos.vy;
            lastPos.vz = shipIndex[shipNum].ppivot.vz - lastPos.vz;
            shipIndex[shipNum].moveDist = GetMagnitude(lastPos);
        }

        SetUnitVectors(shipIndex);
    }
}

void SortAllShips(ShipData *shipIndex) {
    s16 i;
    s16 count;
    u16 swapSection;
    u16 swapShip;
    s16 firstSection;
    s16 swapped;
    VECTOR distanceA;
    VECTOR distanceB;
    VECTOR direction;
    s32 angleA;
    s32 angleB;
    s32 projectedA;
    s32 projectedB;
    ShipData *playerShip;
    ShipRank *current;
    ShipRank *next;
    s16 currentShip;
    u16 sectionNumber;

    count = 0;
    for (i = 0; i < activeShipCount; i++) {
        if (!(shipIndex[shipRank[i].shipNo].combatAttr & SHIP_DESTROYED)) {
            currentShip = shipRank[i].shipNo;
            sectionNumber = raceLines[shipIndex[shipRank[i].shipNo].nearSecNo].distance;
            shipRank[count].shipNo = currentShip;
            shipRank[count].position = sectionNumber;
            count++;
        }
    }

    activeShipCount = count;
    if (count >= 2) {
        do {
            swapped = 0;
            for (i = 0; i < activeShipCount - 1; i++) {
                current = &shipRank[i];
                next = &shipRank[i + 1];
                firstSection = current->position;
                if (firstSection < shipRank[i + 1].position) {
                    swapSection = next->position;
                    next->position = firstSection;
                    current->position = swapSection;
                    swapShip = current->shipNo;
                    swapSection = next->shipNo;
                    next->shipNo = swapShip;
                    current->shipNo = swapSection;
                    swapped = 1;
                } else if (firstSection == next->position) {
                    playerShip = &shipIndex[current->shipNo];
                    distanceA.vx = playerShip->ppivot.vx - playerShip->nearTrkSect->centre.vx;
                    distanceA.vy = playerShip->ppivot.vy - playerShip->nearTrkSect->centre.vy;
                    distanceA.vz = playerShip->ppivot.vz - playerShip->nearTrkSect->centre.vz;
                    distanceB.vx = playerShip->ppivot.vx - playerShip->nearTrkSect->centre.vx;
                    distanceB.vy = playerShip->ppivot.vy - playerShip->nearTrkSect->centre.vy;
                    distanceB.vz = playerShip->ppivot.vz - playerShip->nearTrkSect->centre.vz;
                    direction.vx = playerShip->nearTrkSect->nextSection.ptr->centre.vx -
                                   playerShip->nearTrkSect->centre.vx;
                    direction.vy = playerShip->nearTrkSect->nextSection.ptr->centre.vy -
                                   playerShip->nearTrkSect->centre.vy;
                    direction.vz = playerShip->nearTrkSect->nextSection.ptr->centre.vz -
                                   playerShip->nearTrkSect->centre.vz;
                    angleA = GetAng(distanceA, direction);
                    angleB = GetAng(distanceB, direction);
                    projectedA = (rcos(angleA >> 3) * GetMagnitude(distanceA)) >> 12;
                    projectedB = (rcos(angleB >> 3) * GetMagnitude(distanceB)) >> 12;
                    if (projectedA < projectedB) {
                        swapSection = next->position;
                        next->position = current->position;
                        current->position = swapSection;
                        swapShip = current->shipNo;
                        swapSection = next->shipNo;
                        next->shipNo = swapShip;
                        current->shipNo = swapSection;
                    }
                }
            }
        } while (swapped);
    }
}

void UpdatePlayerShipStart(ShipData *shipIndex, Object **shipShapes, s32 shipNo,
                           WeaponData *weaponIndex, PadInfo *controller) {
    ShipData *playerShip;

    if ((shipIndex[ownShip].combatAttr & SHIP_DESTROYED) ||
        (frontendOptions_g.numPlayers >= 2 && (shipIndex[ownShip2].combatAttr & SHIP_DESTROYED)) ||
        (frontendOptions_g.numPlayers >= 3 && (shipIndex[ownShip3].combatAttr & SHIP_DESTROYED)) ||
        (frontendOptions_g.numPlayers == 4 && (shipIndex[ownShip4].combatAttr & SHIP_DESTROYED))) {
        return;
    }

    playerShip = &shipIndex[shipNo];
    GeneralStartProcs(playerShip);
    if (playerShip->updateCount == START_Y_CAPTURE_FRAME) {
        playerShip->startY = playerShip->ppivot.vy;
    }
    playerShip->ppivot.vy = playerShip->startY + (rsin(playerShip->updateCount * 80) >> 7);

    if (playerShip->updateCount == 0) {
        startCountdown = 0;
        if (playerShip->attr & RACING) {
            playerShip->updateCount = LAUNCH_WINDOW_FRAMES;
            playerShip->update = UpdatePlayerShip;
        } else {
            playerShip->update = UpdateDroneShip;
        }
    }

    if (g_unk80092234 == 0 && (playerShip->attr & RACING)) {
        SetBrake(playerShip, controller);
        SetView(playerShip, controller, shipNo);
    }

    if (controller->pad & PAD_THRUST) {
        playerShip->thrust += THRUST_INC;
    } else if (playerShip->thrust >= THRUST_INC) {
        playerShip->thrust -= THRUST_INC;
    }

    if (playerShip->thrust > playerShip->maxThrust) {
        playerShip->thrust = playerShip->maxThrust;
    }

    if (raceType == RACE_TIME_TRIAL ||
        (raceType == RACE_CHALLENGE && frontendOptions_g.challengeType == CHALLENGE_TIME_TRIAL)) {
        playerShip->thrust = ((playerShip->maxThrust >> 3) * 6) - 1;
    }
}

void SetUpFirstCheckPoint(void) {
    TrackSection *sect;
    Face *face;
    s16 i;

    checkTimer = classCheckTime[tempGameData->shipClass];
    if (frontendOptions_g.numPlayers >= 2 && frontendOptions_g.catchUp != 0) {
        checkTimer += MULTI_BONUS_FRAMES;
    }

    sect = track->sections;
    while (sect->secNo != startLinePos[tempGameData->actTrackNum]) {
        sect = sect->nextSection.ptr;
    }

    face = (Face *)track->faces + sect->faceStart;
    while ((face->flags & TRACK_BASE) == 0) {
        face++;
    }

    if (face->flags & CHECKPOINT) {
        for (i = 0; i < MAX_CHECKPOINTS; i++) {
            if (checkPoints[i].section == sect->secNo) {
                checkPoints[i].reached = 1;
                checkPointReached[i][0] = 1;
                checkPointReached[i][1] = 1;
                checkPointReached[i][2] = 1;
                checkPointReached[i][3] = 1;
                checkTimer += checkPoints[i].time * FRAMES_PER_SECOND;
            }
        }
    }

    if (raceType == RACE_TIME_TRIAL ||
        (raceType == RACE_CHALLENGE && frontendOptions_g.challengeType == CHALLENGE_TIME_TRIAL)) {
        checkTimer = 0;
    }

    if (frontendOptions_g.numPlayers >= 2) {
        checkTimer2 = checkTimer;
        checkTimer3 = checkTimer;
        checkTimer4 = checkTimer;
    }
}

void UpdatePlayerShip(ShipData *shipIndex, Object **shipShapes, s32 shipNo,
                      WeaponData *weaponIndex, PadInfo *controller) {
    s32 unusedPad[4]; /* unused: retail reserves 0x10-byte slot before deadZone */
    s16 destroyed = 0;
    s16 deadZone[4] = { 0, 6, 12, 18 };
    ShipData *ship;
    s16 targetTurn;
    s16 maxThrust;
    s16 launchThrust;
    s16 lower;
    s16 upper;

    ship = &shipIndex[shipNo];
    if (shipDeathCount[shipNo] != -1) {
        ShipTrkReaction(ship, shipShapes, shipNo);
        return;
    }
    if (ship->combatAttr & SHIP_DESTROYED) {
        return;
    }

    if (ship->electroCount > 0) {
        ship->electroCount--;
    } else if (ship->electroCount == 0) {
        ship->attr &= ~ELECTROED;
    }

    g_unk80091E60 = 0;

    if (abs(STICK_X(controller)) - deadZone[1] <= 0) {
        controller->stickX = 0;
    }
    if (turnDivisor == 0) {
        turnDivisor = 1;
    }
    targetTurn = (((-STICK_X(controller)) * (ship->maxHeading + 400)) << 1) / turnDivisor;

    if (targetTurn > ship->vhdg) {
        if (targetTurn - ship->vhdg >= -ship->headingInc) {
            ship->vhdg += targetTurn - ship->vhdg;
        }
        if (ship->vhdg <= 0) {
            ship->vhdg -= ship->headingInc;
        } else {
            ship->vhdg -= ship->headingInc << 1;
        }
    } else if (targetTurn < ship->vhdg) {
        if (targetTurn - ship->vhdg <= ship->headingInc) {
            ship->vhdg += targetTurn - ship->vhdg;
        } else if (ship->vhdg >= 0) {
            ship->vhdg -= ship->headingInc;
        } else {
            ship->vhdg -= ship->headingInc << 1;
        }
    }

    if (ship->vhdg < -ship->maxHeading) {
        ship->vhdg = -ship->maxHeading;
    }
    if (ship->vhdg > ship->maxHeading) {
        ship->vhdg = ship->maxHeading;
    }

    if (ship->attr & ELECTROED) {
        if (shipNo == ownShip && (ship->attr & VIEW_IN)) {
            SetShake(2, 0);
        }
        if (frontendOptions_g.numPlayers >= 2 && shipNo == ownShip2 && (ship->attr & VIEW_IN)) {
            SetShake(2, 1);
        }
        if (frontendOptions_g.numPlayers >= 3 && shipNo == ownShip3 && (ship->attr & VIEW_IN)) {
            SetShake(2, 2);
        }
        if (frontendOptions_g.numPlayers == 4 && shipNo == ownShip4 && (ship->attr & VIEW_IN)) {
            SetShake(2, 3);
        }
        ship->vhdg += (newRand() % 600) - 300;
    }

    if (controller->stickY < -30) {
        ship->vpitch += PITCH_INC;
    } else if (controller->stickY > 30) {
        ship->vpitch -= PITCH_INC;
    }

    if (ship->updateCount == LAUNCH_WINDOW_FRAMES) {
        launchThrust = ship->maxThrust >> 3;
        lower = launchThrust * 5;
        upper = launchThrust * 3;
        if (ship->thrust >= lower) {
            if (ship->thrust < (upper << 1)) {
                ship->thrustCount = 9;
                ship->thrust = ship->maxThrust;
                PlayNote(SFX_TURBO, 0xFF, 0xF, 0xF, 0x100);
            } else {
                ship->stallCount = 15;
                PlayNote(SFX_START_STALL, 0xFF, 0xF, 0xF, 0x100);
            }
        }
    }

    if (ship->thrustCount > 0) {
        ship->thrustCount--;
    }
    if (ship->updateCount > 0) {
        ship->updateCount--;
    }

    if (!(ship->combatAttr & SHIP_LOW_SHIELD)) {
        if (raceType == RACE_MULTIPLAYER && frontendOptions_g.catchUp != 0) {
            if (shipNo == ownShip) {
                if (playerPosition == 1) {
                    maxThrust = (u32)(ship->maxThrust * 13) >> 4;
                } else if (playerPosition == 2) {
                    maxThrust = (u32)(ship->maxThrust * 7) >> 3;
                } else if (playerPosition == 3) {
                    maxThrust = (u32)(ship->maxThrust * 15) >> 4;
                } else {
                    maxThrust = ship->maxThrust;
                }
            } else if (shipNo == ownShip2) {
                if (player2Position == 1) {
                    maxThrust = (u32)(ship->maxThrust * 13) >> 4;
                } else if (player2Position == 2) {
                    maxThrust = (u32)(ship->maxThrust * 7) >> 3;
                } else if (player2Position == 3) {
                    maxThrust = (u32)(ship->maxThrust * 15) >> 4;
                } else {
                    maxThrust = ship->maxThrust;
                }
            } else if (shipNo == ownShip3) {
                if (player3Position == 1) {
                    maxThrust = (u32)(ship->maxThrust * 13) >> 4;
                } else if (player3Position == 2) {
                    maxThrust = (u32)(ship->maxThrust * 7) >> 3;
                } else if (player3Position == 3) {
                    maxThrust = (u32)(ship->maxThrust * 15) >> 4;
                } else {
                    maxThrust = ship->maxThrust;
                }
            } else if (shipNo == ownShip4) {
                if (player4Position == 1) {
                    maxThrust = (u32)(ship->maxThrust * 13) >> 4;
                } else if (player4Position == 2) {
                    maxThrust = (u32)(ship->maxThrust * 7) >> 3;
                } else if (player4Position == 3) {
                    maxThrust = (u32)(ship->maxThrust * 15) >> 4;
                } else {
                    maxThrust = ship->maxThrust;
                }
            }
        } else {
            maxThrust = ship->maxThrust;
        }

        if (controller->pad & PAD_THRUST) {
            ship->thrust += THRUST_INC;
        } else if (ship->thrust >= THRUST_DEC) {
            ship->thrust -= THRUST_DEC;
        }
        if (ship->thrust > maxThrust) {
            ship->thrust = maxThrust;
        }
    } else {
        ship->thrust -= THRUST_DEAD_DEC;
        if (ship->thrust < 0) {
            ship->thrust = 0;
            if (ship->speed < 5000U) {
                destroyed = 1;
            }
        }
    }

    if ((ship->attr & ELECTROED) && (newRand() % 80 == 0)) {
        ship->thrust -= ship->thrust >> 2;
    }
    if (ship->stallCount > 0) {
        ship->stallCount--;
        ship->thrust -= ship->thrust >> 2;
    }

    SetBrake(ship, controller);
    SetView(ship, controller, shipNo);
    SetFire(ship, weaponIndex, shipNo, controller);
    SetDitch(ship, weaponIndex, shipNo, controller);
    ShipTrkReaction(ship, shipShapes, shipNo);

    if (destroyed && g_attractMode != 1) {
        if (frontendOptions_g.numPlayers == 1) {
            shipDeathCount[shipNo] = DEATH_COUNT_1P;
        } else {
            shipDeathCount[shipNo] = DEATH_COUNT_MP;
        }
        ship->combatAttr |= SHIP_CRASHED;
        if (ship->attr & VIEW_IN) {
            ship->attr &= ~VIEW_IN;
            if (shipNo == ownShip) {
                CameraUpdate = UpdateCameraRaceExternal;
            } else if (shipNo == ownShip2) {
                CameraUpdate2 = UpdateCameraRaceExternal;
            } else if (shipNo == ownShip3) {
                CameraUpdate3 = UpdateCameraRaceExternal;
            } else if (shipNo == ownShip4) {
                CameraUpdate4 = UpdateCameraRaceExternal;
            }
        }
    }
}

void GeneralStartProcs(ShipData *ship) {
    if (ship->updateCount == START_COUNTDOWN_FRAMES) {
        startCountdown = ship->updateCount;
        countdownActive = 1;
    }

    ship->updateCount--;

    if (ship == &ships[ownShip]) {
        startCountdown--;

        if (ship->updateCount == 0) {
            PlayNote(SFX_COUNTDOWN_GO, 0x1E0, 0xF, 0xF, 200);
            countdownActive = 0;
        }

        if (ship->updateCount == COUNTDOWN_STEP_1) {
            PlayNote(SFX_COUNTDOWN_ONE, 0x1E0, 0xF, 0xF, 220);
        } else if (ship->updateCount == COUNTDOWN_STEP_2) {
            PlayNote(SFX_COUNTDOWN_TWO, 0x1E0, 0xF, 0xF, 220);
        } else if (ship->updateCount == COUNTDOWN_STEP_3) {
            PlayNote(SFX_COUNTDOWN_THREE, 0x1E0, 0xF, 0xF, 220);
        }

        if (startCountdown == START_MUSIC_FRAME) {
            StartMusic();
        }
    }
}
