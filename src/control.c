#include "control.h"

#ifdef VERSION_EU
/* PAL 50Hz timing: 166 frames (~3.32s) matches NTSC's 200 frames (~3.33s). */
#define DRONE_START_COUNT 166
/* PAL scales vhdg/vpitch up by 60/50 per frame, so rescale by 50/60 before turn-drag bleed. */
#define TURN_RATE(rate) ((rate) * 50 / 60)
#else
#define DRONE_START_COUNT 200
#define TURN_RATE(rate) (rate)
#endif

/* Start-of-race hover for an AI ship: bobs around its initial Y height over the countdown,
   then switches to normal drone flight control. */
void UpdateDroneShipStart(ShipData *ships, Object **shipShapes, s32 shipNo) {
    ShipData *ship;

    ship = &ships[shipNo];
    ship->prevShipSect = ship->nearTrkSect;
    SectionSearch(ship);
    if (ship->updateCount == DRONE_START_COUNT) {
        ship->startY = ship->ppivot.vy;
    }
    ship->ppivot.vy = ship->startY + (rsin(ship->updateCount * (shipNo * 3 + 80)) >> 7);
    ship->updateCount--;
    if (ship->updateCount == 0) {
        ship->update = UpdateDroneShip;
    }
}

/* Returns the max shield energy for shipNo: base team shield scaled down by the AI percentage
   for computer-controlled ships, and boosted by 25% for human players in multiplayer races. */
s32 GetDroneShield(s32 shipNo) {
    s32 team;
    s32 ship;
    s32 shield;
    f32 scale;

    team = 0;
    ship = shipNo;
    if (g_unk800923F9 && shipNo == ownShip4) {
        shipNo = ownShip3;
    }
    switch (shipNo) {
        case 0:
        case 1:
        case 2:
            team = 0;
            break;
        case 3:
        case 4:
        case 5:
            team = 1;
            break;
        case 6:
        case 7:
        case 8:
            team = 2;
            break;
        case 9:
        case 10:
        case 11:
            team = 3;
            break;
        case 12:
        case 13:
        case 14:
            team = 4;
            break;
    }
    shield = g_shipMaxShield[team];
    if (g_unk800923F9 && ship == ownShip4) {
        shipNo = ship;
    }
    if ((frontendOptions_g.numPlayers == 1 && shipNo != ownShip) ||
        (frontendOptions_g.numPlayers == 2 && shipNo != ownShip && shipNo != ownShip2) ||
        (frontendOptions_g.numPlayers == 3 && shipNo != ownShip && shipNo != ownShip2 &&
         shipNo != ownShip3) ||
        (frontendOptions_g.numPlayers == 4 && shipNo != ownShip && shipNo != ownShip2 &&
         shipNo != ownShip3 && shipNo != ownShip4)) {
        scale = g_aiShipMaxShield[team];
        scale = scale / 100.0;
        shield = shield * scale;
    }
    if (frontendOptions_g.numPlayers >= 2 &&
        (shipNo == ownShip || shipNo == ownShip2 || shipNo == ownShip3 || shipNo == ownShip4)) {
        scale = shield * 0.25;
        shield = shield + scale;
    }
    return shield;
}

/* Per-frame AI / autopilot ship controller: manages start boost, rubber-banding toward the
   leading human player, weapon firing, junction/pit-lane selection, racing-line steering,
   speed pads, hover physics, attitude updates, and autopilot handoff back to player control. */
void UpdateDroneShip(ShipData *ships, Object **shipShapes, s32 shipNo, WeaponData *weaponIndex,
                     PadInfo *padInfo) {
    VECTOR offset;
    VECTOR targetVector;
    VECTOR facePoint;
    VECTOR bestPath;
    VECTOR distance; /* unused: retail reserves 0x10-byte stack slot at 0x70($sp) */
    Vector path1;
    Vector path2;
    VECTOR direction;
    s16 lateral;
    s16 along;
    s16 humanMoveDist;
    s16 humanNo;
    s16 blocked;
    s32 savedMargin;
    ShipData *ship;
    ShipData *humanShip;
    TrackSection *section;
    TrackSection *nextSection;
    Face *face;
    s32 bestRank;
    s16 raceDist;
    s16 aheadDist;
    s16 behindShipDist;
    s16 raceDiff;
    s16 ahead;
    s16 aheadDiff;
    s16 behind;
    s16 front;
    s16 front2;
    s16 shipDist;
    s16 frontDist;
    s16 behindDist;
    s16 frontDiff;
    s16 behindDiff;
    s16 zone;
    s16 steer;
    s16 boostStep;
    s16 i;
    s16 decide;
    u8 weapon;
    s32 magnitude;
    s32 flat;
    s32 alpha;

    savedMargin = 0;
    blocked = 0;
    offset.vx = 0;
    offset.vy = 0;
    offset.vz = 0;

    /* Track the leading human ship for rubber-banding and weapon targeting. */
    if (attractMode != 2) {
        bestRank = playerPosition;
        humanNo = ownShip;
        if (frontendOptions_g.numPlayers == 2 && player2Position < bestRank) {
            bestRank = player2Position;
            humanNo = ownShip2;
        }
        if (frontendOptions_g.numPlayers == 3 && player3Position < bestRank) {
            bestRank = player3Position;
            humanNo = ownShip3;
        }
        if (frontendOptions_g.numPlayers == 4 && player4Position < bestRank) {
            humanNo = ownShip4;
        }
    } else {
        humanNo = ownShip;
    }
    humanShip = &ships[humanNo];
    ship = &ships[shipNo];

    /* Challenge-mode overrides for the AI launch-boost margin table. */
    if (g_gameType == GAME_TYPE_CHALLENGE && frontendOptions_g.gameMode == 1 &&
        frontendOptions_g.gameSubMode == 6) {
        savedMargin = g_aiCatchupMaxThrust[3];
        g_aiCatchupMaxThrust[3] = 5700;
    }
    if (g_gameType == GAME_TYPE_CHALLENGE && frontendOptions_g.gameMode == 1 &&
        frontendOptions_g.gameSubMode == 2) {
        savedMargin = g_aiCatchupMaxThrust[2];
        g_aiCatchupMaxThrust[2] = 6900;
    }

    /* Launch acceleration while boostCount counts down; afterwards creep up to remoteMax. */
    if (ship->boostCount > 0) {
        ship->boostCount--;
        ship->updateCount = 0;
        if (shipNo == startGridOrder[13]) {
            if (ship->remoteMax + (s16)g_aiCatchupMaxThrust[shipClassIndex] * 32 *
                                      rivalMarginScale4[shipClassIndex] / 64 >= ship->speed) {
                ship->speed += ship->remoteMagnitude +
                               (s16)g_aiCatchupThrustMag[shipClassIndex] * rivalAccelScale4[shipClassIndex];
            }
        } else if (shipNo == startGridOrder[12]) {
            if (ship->remoteMax + (s16)g_aiCatchupMaxThrust[shipClassIndex] * 32 *
                                      rivalMarginScale3[shipClassIndex] / 64 >= ship->speed) {
                ship->speed += ship->remoteMagnitude +
                               (s16)g_aiCatchupThrustMag[shipClassIndex] * rivalAccelScale3[shipClassIndex];
            }
        } else if (shipNo == startGridOrder[11]) {
            if (ship->remoteMax + (s16)g_aiCatchupMaxThrust[shipClassIndex] * 32 *
                                      rivalMarginScale2[shipClassIndex] / 64 >= ship->speed) {
                ship->speed += ship->remoteMagnitude +
                               (s16)g_aiCatchupThrustMag[shipClassIndex] * rivalAccelScale2[shipClassIndex];
            }
        } else if (shipNo == startGridOrder[10]) {
            if (ship->remoteMax + (s16)g_aiCatchupMaxThrust[shipClassIndex] * 32 *
                                      rivalMarginScale1[shipClassIndex] / 64 >= ship->speed) {
                ship->speed += ship->remoteMagnitude +
                               (s16)g_aiCatchupThrustMag[shipClassIndex] * rivalAccelScale1[shipClassIndex];
            }
        } else if (ship->remoteMax + (s16)g_aiCatchupMaxThrust[shipClassIndex] / 2 >= ship->speed) {
            ship->speed += ship->remoteMagnitude + (s16)g_aiCatchupThrustMag[shipClassIndex];
        }
    } else if (ship->autopilotCount == 0) {
        if (ship->speed <= ship->remoteMax) {
            ship->speed += ship->remoteMagnitude;
        }
    } else if (ship->speed <= ship->remoteMax * 2) {
        ship->speed += ship->remoteMagnitude * 3;
    }

    if (g_gameType == GAME_TYPE_CHALLENGE && frontendOptions_g.gameMode == 1 &&
        frontendOptions_g.gameSubMode == 6) {
        g_aiCatchupMaxThrust[3] = savedMargin;
    }
    if (g_gameType == GAME_TYPE_CHALLENGE && frontendOptions_g.gameMode == 1 &&
        frontendOptions_g.gameSubMode == 2) {
        g_aiCatchupMaxThrust[2] = savedMargin;
    }

    if (!(ship->attr & FLYING)) {
        /* Steer along the track base face; overtake a slower ship ahead if close. */
        face = (Face *)track->faces + ship->nearTrkSect->faceStart;
        while (!(face->flags & TRACK_BASE)) {
            face++;
        }
        raceDist = raceLines[ships[shipNo].nearSecNo].distance;
        raceDiff = raceDist - raceLines[ships[humanNo].nearSecNo].distance;
        if (raceDiff < 0) {
            raceDiff += numTrackSections;
        }
        ahead = getShipInfrontLAP(ships, shipNo);
        if (ahead != -1) {
            aheadDist = raceLines[ships[ahead].nearSecNo].distance;
            aheadDiff = aheadDist - raceDist;
            if (aheadDiff < 0) {
                aheadDiff += numTrackSections;
            }
            if (aheadDiff < 32 && ships[ahead].moveDist < ships[shipNo].moveDist) {
                lookForWayPast(&offset, ships, shipNo, ahead);
            } else {
                followRaceLine(&offset, ships, face, shipNo);
            }
        } else {
            followRaceLine(&offset, ships, face, shipNo);
        }

        if (ship->autopilotCount == 0) {
            /* Classify distance ahead (+) or behind (-) the human player into rubber-band zones. */
            if (raceDiff > numTrackSections / 2) {
                raceDiff -= numTrackSections;
            }
            zone = NO_HUMAN_ZONE;
            if (raceDiff < 84 && raceDiff > -80) {
                if (raceDiff <= 80 && raceDiff > 40) {
                    zone = 3;
                }
                if (raceDiff <= 40 && raceDiff > 20) {
                    zone = 2;
                }
                if (raceDiff <= 20 && raceDiff > 12) {
                    zone = 1;
                }
                if (raceDiff <= 12 && raceDiff >= -8) {
                    zone = 0;
                }
                if (raceDiff < -8 && raceDiff >= -20) {
                    zone = -1;
                }
                if (raceDiff < -20 && raceDiff >= -40) {
                    zone = -2;
                }
                if (raceDiff < -40 && raceDiff >= -80) {
                    zone = -3;
                }
                steer = g_aiBoostMaxThrust[shipClassIndex];
                humanMoveDist = ships[humanNo].moveDist;
                if (g_gameType == GAME_TYPE_ARCADE ||
                    (g_gameType == GAME_TYPE_LEAGUE && frontendOptions_g.numPlayers == 2)) {
                    if (shipClassIndex == 3) {
                        if (frontendOptions_g.trackNo == 4) {
                            steer = 2100;
                        } else if (frontendOptions_g.trackNo == 6) {
                            steer = 1900;
                        }
                    }
                }
                boostStep = steer / 8;
                if (ship->boostCount == 0) {
                    behind = getShipBehindLAP(ships, shipNo);
                    if (behind == humanNo && activeShipCount > 3) {
                        front = getShipInfrontLAP(ships, shipNo);
                        front2 = getShipInfrontLAP(ships, front);
                        behindShipDist = raceLines[ships[behind].nearSecNo].distance;
                        aheadDist = raceLines[ships[front2].nearSecNo].distance;
                        aheadDiff = aheadDist - behindShipDist;
                        if (aheadDiff < 0) {
                            aheadDiff += numTrackSections;
                        }
                        if (aheadDiff > 0 && aheadDiff < 20) {
                            blocked = 1;
                        }
                    }
                }
                if (blocked == 0 && humanMoveDist > ships[shipNo].moveDist) {
                    if (zone == 2 || zone == -2) {
                        if (ships[humanNo].speed > classSpeedLimit[shipClassIndex] / 2) {
                            if (ship->combatAttr & SHIP_COMBAT_AI_LOW) {
                                if (ship->speed <= ship->remoteMax + steer / 8) {
                                    ship->speed += ship->remoteMagnitude + boostStep / 8;
                                }
                            } else if (ship->combatAttr & SHIP_COMBAT_AI_MID) {
                                if (ship->speed <= ship->remoteMax + steer / 4) {
                                    ship->speed += ship->remoteMagnitude + boostStep / 4;
                                }
                            } else if (ship->combatAttr & SHIP_COMBAT_AI_HIGH) {
                                if (ship->speed <= ship->remoteMax + steer / 2) {
                                    ship->speed += ship->remoteMagnitude + boostStep / 2;
                                }
                            }
                        }
                    }
                    if (humanMoveDist > ships[shipNo].moveDist && (zone == 1 || zone == 0 || zone == -1)) {
                        if (ships[humanNo].speed > classSpeedLimit[shipClassIndex] / 2) {
                            if (ship->combatAttr & SHIP_COMBAT_AI_LOW) {
                                if (ship->speed <= ship->remoteMax + steer / 4) {
                                    ship->speed += ship->remoteMagnitude + boostStep / 4;
                                }
                            } else if (ship->combatAttr & SHIP_COMBAT_AI_MID) {
                                if (ship->speed <= ship->remoteMax + steer / 2) {
                                    ship->speed += ship->remoteMagnitude + boostStep / 2;
                                }
                            } else if (ship->combatAttr & SHIP_COMBAT_AI_HIGH) {
                                if (ship->speed <= ship->remoteMax + steer) {
                                    ship->speed += ship->remoteMagnitude + boostStep;
                                }
                            }
                        }
                    }
                }
            }

            /* Fire the drone's weapon according to its aggression tier. */
            weapon = ship->weapon;
            if (weapon != 0 && zone != NO_HUMAN_ZONE) {
                if ((ship->combatAttr & SHIP_COMBAT_AI_LOW) || (ship->combatAttr & SHIP_COMBAT_AI_MID)) {
                    if (weapon == WEAPON_ROCKET || weapon == WEAPON_MISSILE ||
                        weapon == WEAPON_ELEC_BOLT || weapon == WEAPON_QUAKE) {
                        ship->targetShip = getShipInfrontLAP(ships, shipNo);
                        if (ship->targetShip == ownShip) {
                            if (weapon == WEAPON_MISSILE) {
                                PlayNote(SFX_MISSILE_WARN, 510, 15, 15, 256);
                            }
                            if (weapon == WEAPON_ROCKET) {
                                PlayNote(SFX_ROCKET_WARN, 510, 15, 15, 256);
                            }
                            if (weapon == WEAPON_ELEC_BOLT) {
                                PlayNote(SFX_ELEC_BOLT_WARN, 510, 15, 15, 256);
                            }
                            if (weapon == WEAPON_QUAKE) {
                                PlayNote(SFX_QUAKE_WARN, 510, 15, 15, 256);
                            }
                        }
                        FireWeapon(ship, weaponIndex, shipNo, ship->weapon);
                    } else {
                        FireWeapon(ship, weaponIndex, shipNo, ship->weapon);
                        if (weapon == WEAPON_MINE) {
                            if (raceDiff > 0 && raceDiff <= 40) {
                                PlayNote(SFX_MINE_WARN, 470, 15, 15, 256);
                            }
                        }
                    }
                } else if (ship->combatAttr & SHIP_COMBAT_AI_HIGH) {
                    if (weapon == WEAPON_ROCKET || weapon == WEAPON_MISSILE || weapon == WEAPON_ELEC_BOLT) {
                        front = getShipInfrontLAP(ships, shipNo);
                        behind = getShipBehindLAP(ships, shipNo);
                        if (front == ownShip ||
                            (frontendOptions_g.numPlayers == 2 && front == ownShip2) ||
                            (frontendOptions_g.numPlayers == 3 && (front == ownShip2 || front == ownShip3))) {
                            ship->targetShip = front;
                            if (weapon == WEAPON_MISSILE) {
                                PlayNote(SFX_MISSILE_WARN, 510, 15, 15, 256);
                            }
                            if (weapon == WEAPON_ROCKET) {
                                PlayNote(SFX_ROCKET_WARN, 510, 15, 15, 256);
                            }
                            if (weapon == WEAPON_ELEC_BOLT) {
                                PlayNote(SFX_ELEC_BOLT_WARN, 510, 15, 15, 256);
                            }
                            if (weapon == WEAPON_QUAKE) {
                                PlayNote(SFX_QUAKE_WARN, 510, 15, 15, 256);
                            }
                        } else if (behind == ownShip ||
                                   (frontendOptions_g.numPlayers == 2 && behind == ownShip2) ||
                                   (frontendOptions_g.numPlayers == 3 &&
                                    (behind == ownShip2 || behind == ownShip3))) {
                            ship->targetShip = behind;
                            if (weapon == WEAPON_ELEC_BOLT) {
                                PlayNote(SFX_ELEC_BOLT_WARN, 510, 15, 15, 256);
                            }
                            if (weapon == WEAPON_MISSILE) {
                                PlayNote(SFX_MISSILE_WARN, 510, 15, 15, 256);
                            }
                        } else {
                            shipDist = raceLines[ships[shipNo].nearSecNo].distance;
                            frontDist = raceLines[ships[front].nearSecNo].distance;
                            behindDist = raceLines[ships[behind].nearSecNo].distance;
                            frontDiff = shipDist - frontDist;
                            if (frontDiff < 0) {
                                frontDiff += numTrackSections;
                            }
                            behindDiff = shipDist - behindDist;
                            if (behindDiff < 0) {
                                behindDiff += numTrackSections;
                            }
                            if (abs(frontDiff) > abs(behindDiff)) {
                                ship->targetShip = behind;
                            } else {
                                ship->targetShip = front;
                            }
                        }
                        FireWeapon(ship, weaponIndex, shipNo, ship->weapon);
                    } else {
                        FireWeapon(ship, weaponIndex, shipNo, ship->weapon);
                        if (weapon == WEAPON_MINE) {
                            if (raceDiff > 0 && raceDiff <= 40) {
                                PlayNote(SFX_MINE_WARN, 470, 15, 15, 256);
                            }
                        }
                    }
                }
            }
        }

        /* Choose branch at upcoming junction: pit lane when damaged (not on final lap), else random. */
        section = ship->nearTrkSect->prevSection.ptr;
        for (i = 0; i < 4; i++) {
            section = section->nextSection.ptr;
        }
        if (section->junction.ptr != (TrackSection *)-1 && (section->junction.ptr->flags & JunctionStart)) {
            if (section->secNo == pitLaneSection1[trackIndex] || section->secNo == pitLaneSection2[trackIndex]) {
                ship->attr &= ~JUNC_LEFT;
                if (shipDamage[shipNo] > GetDroneShield(shipNo) / 2) {
                    if (ships[shipNo].lap < g_classLaps[shipClassIndex]) {
                        ship->attr |= JUNC_LEFT;
                    }
                }
            } else {
                decide = newRand() % 2;
                if (decide == 0) {
                    ship->attr |= JUNC_LEFT;
                } else {
                    ship->attr &= ~JUNC_LEFT;
                }
            }
        }

        /* Advance four sections along the chosen junction path. */
        section = ship->nearTrkSect->prevSection.ptr;
        for (i = 0; i < 4; i++) {
            if (section->junction.ptr != (TrackSection *)-1 && (section->junction.ptr->flags & JunctionStart) &&
                (ship->attr & JUNC_LEFT)) {
                section = section->junction.ptr;
            } else {
                section = section->nextSection.ptr;
            }
        }

        /* Target velocity along the track, bled off by turning, then pull toward the racing line. */
        nextSection = section->nextSection.ptr;
        section = ship->nearTrkSect;
        ship->speed -= abs(ship->speed * TURN_RATE(ship->vhdg)) >> 14;
        ship->speed -= abs(ship->speed * TURN_RATE(ship->vpitch)) >> 14;
        if ((ship->nearTrkSect->flags & Jump) || (ship->nearTrkSect->nextSection.ptr->flags & Jump) ||
            (ship->nearTrkSect->prevSection.ptr->flags & Jump) ||
            (ship->nearTrkSect->nextSection.ptr->nextSection.ptr->flags & Jump) ||
            (ship->nearTrkSect->nextSection.ptr->nextSection.ptr->nextSection.ptr->flags & Jump)) {
            targetVector.vx = ship->nearTrkSect->centre.vx - ship->nearTrkSect->prevSection.ptr->centre.vx;
            targetVector.vy = ship->nearTrkSect->centre.vy - ship->nearTrkSect->prevSection.ptr->centre.vy;
            targetVector.vz = ship->nearTrkSect->centre.vz - ship->nearTrkSect->prevSection.ptr->centre.vz;
        } else {
            targetVector.vx = nextSection->centre.vx - section->centre.vx;
            targetVector.vy = nextSection->centre.vy - section->centre.vy;
            targetVector.vz = nextSection->centre.vz - section->centre.vz;
        }
        magnitude = GetMagnitude(targetVector);
        targetVector.vx = (targetVector.vx * ship->speed) / magnitude;
        targetVector.vy = (targetVector.vy * ship->speed) / magnitude;
        targetVector.vz = (targetVector.vz * ship->speed) / magnitude;
        path1.vx = section->centre.vx + offset.vx;
        path1.vy = section->centre.vy + offset.vy;
        path1.vz = section->centre.vz + offset.vz;
        path2.vx = nextSection->centre.vx + offset.vx;
        path2.vy = nextSection->centre.vy + offset.vy;
        path2.vz = nextSection->centre.vz + offset.vz;
        GetTargetPos(&bestPath, ship->ppivot, path2, path1);
        ship->apivot.vx = targetVector.vx + (bestPath.vx - ship->ppivot.vx);
        ship->apivot.vy = targetVector.vy + (bestPath.vy - ship->ppivot.vy);
        ship->apivot.vz = targetVector.vz + (bestPath.vz - ship->ppivot.vz);
#ifdef VERSION_EU
        /* PAL 50Hz acceleration rescaling (60/50). */
        ship->apivot.vx = ship->apivot.vx * 60 / 50;
        ship->apivot.vy = ship->apivot.vy * 60 / 50;
        ship->apivot.vz = ship->apivot.vz * 60 / 50;
#endif
        ship->vpivot.vx += ship->apivot.vx;
        ship->vpivot.vy += ship->apivot.vy;
        ship->vpivot.vz += ship->apivot.vz;

        /* Speed-up pads boost the drone once per pad section when passing over them. */
        calcMyXPos(ships, shipNo, &lateral, &along);
        if ((face->flags & SPEED_UP) && lateral < 0 &&
            (ship->speedPadSection == -1 || ship->speedPadSection != ship->nearTrkSect->secNo)) {
            if (shipClassIndex == 0 && frontendOptions_g.trackNo == 7) {
                ship->speed += 275;
            } else {
                ship->speed += g_aiBoostThrustMag[shipClassIndex];
            }
            ship->speedPadSection = ship->nearTrkSect->secNo;
            face++;
        } else if ((face->flags & SPEED_UP) && lateral > 0 &&
                   (ship->speedPadSection == -1 || ship->speedPadSection != ship->nearTrkSect->secNo)) {
            if (shipClassIndex == 0 && frontendOptions_g.trackNo == 7) {
                ship->speed += 275;
            } else {
                ship->speed += g_aiBoostThrustMag[shipClassIndex];
            }
            ship->speedPadSection = ship->nearTrkSect->secNo;
            face++;
        } else {
            ship->speedPadSection = -1;
            face++;
        }
        if (ships[shipNo].thrustCount > 0) {
            if (shipClassIndex == 0 && frontendOptions_g.trackNo == 7) {
                ship->speed += 275;
            } else {
                ship->speed += g_aiBoostThrustMag[shipClassIndex];
            }
            ships[shipNo].thrustCount--;
        }

        /* Hover repulsion above the track base face. */
        GetFaceVertex(ship->nearTrkSect, face, 0, (Vector *)&facePoint);
        alpha = PlaneLineCollision(facePoint, ship->ppivot, face->normal);
        if (alpha < 50) {
            alpha = 50;
        }
        ship->apivot.vx = ((face->normal.vx << 14) / alpha - (face->normal.vx << 6)) >> 8;
        ship->apivot.vy = ((face->normal.vy << 14) / alpha - (face->normal.vy << 6)) >> 8;
        ship->apivot.vz = ((face->normal.vz << 14) / alpha - (face->normal.vz << 6)) >> 8;
        ship->vpivot.vx += ship->apivot.vx;
        ship->vpivot.vy += ship->apivot.vy;
        ship->vpivot.vz += ship->apivot.vz;

        /* Heading, pitch and roll follow the target velocity vector. */
        flat = SquareRoot0(targetVector.vx * targetVector.vx + targetVector.vz * targetVector.vz);
        ship->vhdg = -ratan2(targetVector.vx, targetVector.vz) - ship->hdg;
        ship->vhdg += ship->ahdg;
        ship->ahdg -= ship->ahdg >> 3;
        if (ship->vhdg > 2048) {
            ship->vhdg -= 4096;
        } else if (ship->vhdg < -2048) {
            ship->vhdg += 4096;
        }
#ifdef VERSION_EU
        /* PAL 50Hz heading rate rescaling (60/50). */
        ship->vhdg = ship->vhdg * 60 / 50;
#endif
        ship->hdg += ship->vhdg >> 4;
        ship->vpitch = -ratan2(targetVector.vy, flat) - ship->pitch;
#ifdef VERSION_EU
        /* PAL 50Hz pitch rate rescaling (60/50). */
        ship->vpitch = ship->vpitch * 60 / 50;
#endif
        ship->pitch += ship->vpitch >> 4;
        ship->vroll += sar(ship->vhdg, 2);
        ship->vroll -= ship->vroll >> 1;
        ship->roll += ship->vroll;
        ship->roll -= ship->roll >> 3;
        GetFaceVertex(ship->nearTrkSect, face, 0, (Vector *)&facePoint);
        if (ship->autopilotCount > 0) {
            TrackSection *next;

            alpha = PlaneLineCollision(facePoint, ship->ppivot, face->normal);
            if (alpha < -300) {
                alpha = -300;
            }
            DroneShip2TrackBase(ship, face, alpha);
            next = ship->nearTrkSect->nextSection.ptr;
            direction.vx = next->centre.vx - ship->nearTrkSect->centre.vx;
            direction.vy = next->centre.vy - ship->nearTrkSect->centre.vy;
            direction.vz = next->centre.vz - ship->nearTrkSect->centre.vz;
            if (direction.vx * ship->unitVecNose.vx + direction.vy * ship->unitVecNose.vy +
                    direction.vz * ship->unitVecNose.vz < 0) {
                ship->attr &= ~DIRECTION;
            } else {
                ship->attr |= DIRECTION;
            }
        }
    } else {
        /* Airborne: steer toward the track three sections ahead while falling. */
        section = ship->nearTrkSect;
        section = section->nextSection.ptr;
        section = section->nextSection.ptr;
        nextSection = section->nextSection.ptr;
        moveDownCentre(&offset, humanShip, face, shipNo);
        ship->speed -= abs(ship->speed * ship->vhdg) >> 14;
        targetVector.vx = nextSection->centre.vx - section->centre.vx;
        targetVector.vy = nextSection->centre.vy - section->centre.vy;
        targetVector.vz = nextSection->centre.vz - section->centre.vz;
        magnitude = GetMagnitude(targetVector);
        targetVector.vx = (targetVector.vx * ship->speed) / magnitude;
        targetVector.vz = (targetVector.vz * ship->speed) / magnitude;
        targetVector.vy = 500;
        GetTargetPos(&bestPath, ship->ppivot, nextSection->centre, ship->nearTrkSect->centre);
        ship->apivot.vx = targetVector.vx + ((bestPath.vx - ship->ppivot.vx) >> 1);
        ship->apivot.vy = targetVector.vy;
        ship->apivot.vz = targetVector.vz + ((bestPath.vz - ship->ppivot.vz) >> 1);
#ifdef VERSION_EU
        /* PAL 50Hz airborne acceleration rescaling (60/50). */
        ship->apivot.vx = ship->apivot.vx * 60 / 50;
        ship->apivot.vy = ship->apivot.vy * 60 / 50;
        ship->apivot.vz = ship->apivot.vz * 60 / 50;
#endif
        ship->vpivot.vx += ship->apivot.vx;
        ship->vpivot.vy += ship->apivot.vy;
        ship->vpivot.vz += ship->apivot.vz;
        ship->vpitch = -200 - ship->pitch;
        ship->pitch += ship->vpitch >> 4;
        ship->vhdg = -ratan2(targetVector.vx, targetVector.vz) - ship->hdg;
        if (ship->vhdg > 2048) {
            ship->vhdg -= 4096;
        } else if (ship->vhdg < -2048) {
            ship->vhdg += 4096;
        }
        ship->hdg += ship->vhdg >> 4;
    }

    /* Integrate velocity damping and position. */
    ship->vpivot.vx -= ship->vpivot.vx >> 3;
    ship->vpivot.vy -= ship->vpivot.vy >> 3;
    ship->vpivot.vz -= ship->vpivot.vz >> 3;
    ship->ppivot.vx += ship->vpivot.vx >> 6;
    ship->ppivot.vy += ship->vpivot.vy >> 6;
    ship->ppivot.vz += ship->vpivot.vz >> 6;

    if (ship->attr & ELECTROED) {
        ship->ppivot.vx += (newRand() - 0x4000) % 20;
        ship->ppivot.vy += (newRand() - 0x4000) % 20;
        ship->ppivot.vz += (newRand() - 0x4000) % 20;
        ship->electroCount--;
        if (ship->electroCount <= 0) {
            ship->electroCount = 0;
            ship->attr &= ~ELECTROED;
        }
        if (newRand() % 10 == 0) {
            ship->speed -= ship->speed >> 1;
        }
    }
    if (ship->stallCount > 0) {
        ship->stallCount--;
        ship->thrust -= ship->thrust >> 2;
    }

    ship->hdg = ang(ship->hdg);
    ship->pitch = ang(ship->pitch);
    ship->roll = ang(ship->roll);

    /* Autopilot countdown or manual cancellation via discard button. */
    if (ship->autopilotCount > 0) {
        if (padInfo->xpad & PAD_DISCARD) {
            ship->update = UpdatePlayerShip;
            ship->vhdg = 0;
            ship->autopilotCount = 0;
            PlayNote(SFX_AUTOPILOT_OFF, 255, 15, 15, 256);
        } else {
            ship->autopilotCount--;
            if (ship->autopilotCount == 0) {
                if (ship->roll > -50 && ship->roll < 50) {
                    ship->update = UpdatePlayerShip;
                    ship->vhdg = 0;
                } else {
                    ship->autopilotCount = 1;
                }
            }
        }
    }
}

/* Clears the lateral racing-line offset to steer down the section centre. */
void moveDownCentre(VECTOR *relative, ShipData *ships, Face *face, s32 shipNo) {
    relative->vx = 0;
    relative->vy = 0;
    relative->vz = 0;
}

/* Computes the offset from the base face's first vertex to the racing-line point of the section. */
void followRaceLine(VECTOR *relative, ShipData *ships, Face *face, s32 shipNo) {
    Vector vertex;
    s16 section;

    section = ships[shipNo].nearSecNo;
    GetFaceVertex(ships[shipNo].nearTrkSect, face, 0, &vertex);
    relative->vx = raceLines[section].x - vertex.vx;
    relative->vy = raceLines[section].y - vertex.vy;
    relative->vz = raceLines[section].z - vertex.vz;
}

/* Steers left or right around a slower ship ahead when level laterally and space is available. */
void lookForWayPast(VECTOR *relative, ShipData *ships, s16 shipNo, s16 otherShip) {
    s16 lateral;
    s16 width;
    s16 otherLateral;
    s16 otherWidth;
    s16 leftRoom;
    s16 rightRoom;
    s16 clear;
    Face *face;

    clear = 0;
    face = (Face *)track->faces + ships[shipNo].nearTrkSect->faceStart;
    while (!(face->flags & TRACK_BASE)) {
        face++;
    }
    calcMyXPos(ships, otherShip, &otherLateral, &otherWidth);
    calcMyXPos(ships, shipNo, &lateral, &width);
    if (otherLateral > otherWidth) {
        otherLateral = otherWidth;
    }
    if (otherLateral < -otherWidth) {
        otherLateral = -otherWidth;
    }
    if (lateral > width) {
        lateral = width;
    }
    if (lateral < -width) {
        lateral = -width;
    }
    if (abs(otherLateral - lateral) < 20) {
        leftRoom = abs(-otherWidth - otherLateral);
        rightRoom = otherWidth - otherLateral;
        if (leftRoom > 20) {
            clear |= 1;
        }
        if (rightRoom > 20) {
            clear |= 2;
        }
        if (otherLateral < lateral && (clear & 2)) {
            goRight(relative, ships, face, shipNo);
        } else if (lateral < otherLateral && (clear & 1)) {
            goLeft(relative, ships, face, shipNo);
        }
    } else {
        followRaceLine(relative, ships, face, shipNo);
    }
}

/* Projects a ship's lateral position across its track base edge (stored in lateralOffset and
   returned in *lateral), along with the section half-width projection in *along. */
void calcMyXPos(ShipData *ships, s16 shipNo, s16 *lateral, s16 *along) {
    VECTOR edge;
    VECTOR toCentre;
    VECTOR centreToVertex;
    Vector vertex0;
    Vector vertex1;
    Face *face;

    face = (Face *)track->faces + ships[shipNo].nearTrkSect->faceStart;
    while (!(face->flags & TRACK_BASE)) {
        face++;
    }
    GetFaceVertex(ships[shipNo].nearTrkSect, face, 0, &vertex0);
    GetFaceVertex(ships[shipNo].nearTrkSect, face, 1, &vertex1);
    edge.vx = vertex0.vx - vertex1.vx;
    edge.vy = vertex0.vy - vertex1.vy;
    edge.vz = vertex0.vz - vertex1.vz;
    toCentre.vx = ships[shipNo].nearTrkSect->centre.vx - ships[shipNo].ppivot.vx;
    toCentre.vy = ships[shipNo].nearTrkSect->centre.vy - ships[shipNo].ppivot.vy;
    toCentre.vz = ships[shipNo].nearTrkSect->centre.vz - ships[shipNo].ppivot.vz;
    centreToVertex.vx = ships[shipNo].nearTrkSect->centre.vx - vertex1.vx;
    centreToVertex.vy = ships[shipNo].nearTrkSect->centre.vy - vertex1.vy;
    centreToVertex.vz = ships[shipNo].nearTrkSect->centre.vz - vertex1.vz;
    *lateral = -(toCentre.vx * edge.vx + toCentre.vy * edge.vy + toCentre.vz * edge.vz) >> 16;
    *along = (centreToVertex.vx * edge.vx + centreToVertex.vy * edge.vy +
              centreToVertex.vz * edge.vz) >> 16;
    if (*lateral > 0) {
        ships[shipNo].attr |= LSIDE;
    } else {
        ships[shipNo].attr &= ~LSIDE;
    }
    ships[shipNo].lateralOffset = *lateral;
}

/* Half the track base edge pointing toward the right side of the face. */
void goRight(VECTOR *result, ShipData *ships, Face *face, s32 shipNo) {
    Vector vertex1;
    Vector vertex2;

    GetFaceVertex(ships[shipNo].nearTrkSect, face, 0, &vertex1);
    GetFaceVertex(ships[shipNo].nearTrkSect, face, 1, &vertex2);
    result->vx = (vertex1.vx - vertex2.vx) >> 1;
    result->vy = (vertex1.vy - vertex2.vy) >> 1;
    result->vz = (vertex1.vz - vertex2.vz) >> 1;
}

/* Half the track base edge pointing toward the left side of the face. */
void goLeft(VECTOR *result, ShipData *ships, Face *face, s32 shipNo) {
    Vector vertex1;
    Vector vertex2;

    GetFaceVertex(ships[shipNo].nearTrkSect, face, 1, &vertex1);
    GetFaceVertex(ships[shipNo].nearTrkSect, face, 0, &vertex2);
    result->vx = (vertex1.vx - vertex2.vx) >> 1;
    result->vy = (vertex1.vy - vertex2.vy) >> 1;
    result->vz = (vertex1.vz - vertex2.vz) >> 1;
}

/* Returns the nearest active, targetable ship ahead of shipNo in race order, or -1. */
s16 getShipInfrontLAP(ShipData *ships, s16 shipNo) {
    s16 position;
    s16 attempts;
    s16 candidate;

    for (position = 0; position < activeShipCount; position++) {
        if (shipOrder[position].shipNo == shipNo) {
            break;
        }
    }
    for (attempts = 0; attempts < 15; attempts++) {
        if (position == 0) {
            position = activeShipCount;
        }
        position--;
        candidate = shipOrder[position].shipNo;
        if (!(ships[candidate].combatAttr & DESTROYED) &&
            !(ships[candidate].combatAttr & SHIP_COMBAT_NO_FIRE)) {
            if (candidate != shipNo) {
                return candidate;
            }
        }
    }
    return -1;
}

/* Returns the nearest active, targetable ship behind shipNo in race order, or -1. */
s16 getShipBehindLAP(ShipData *ships, s16 shipNo) {
    s16 position;
    s16 attempts;
    s16 candidate;

    for (position = 0; position < activeShipCount; position++) {
        if (shipOrder[position].shipNo == shipNo) {
            break;
        }
    }
    for (attempts = 0; attempts < 15; attempts++) {
        position++;
        if (position >= activeShipCount) {
            position = 0;
        }
        candidate = shipOrder[position].shipNo;
        if (!(ships[candidate].combatAttr & DESTROYED) &&
            !(ships[candidate].combatAttr & SHIP_COMBAT_NO_FIRE)) {
            if (candidate != shipNo) {
                return candidate;
            }
        }
    }
    return -1;
}

/* Converts a human ship's recent frame movement distance into drone speed (clamped to remoteMax)
   when handing off to autopilot or post-race AI control. */
void humanToDroneSpeed(ShipData *ships, s32 shipNo) {
    s32 speed;

    speed = ships[shipNo].moveDist * 9;
    if (ships[shipNo].remoteMax < speed) {
        speed = ships[shipNo].remoteMax;
    }
    ships[shipNo].speed = speed;
    ships[shipNo].boostCount = 0;
    ships[shipNo].ahdg = 0;
}
