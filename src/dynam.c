/*
 * dynam.c — Core anti-gravity ship dynamics and track-surface physics.
 *
 * Computes hover spring forces, pitch/roll/heading integration, speed-up pad
 * boosts, sky visibility culling, and track attributes. Europe rescales
 * acceleration, pitch rate, and boost impulses by 60/50 for 50 Hz PAL.
 */
#include "dynam.h"

#ident "$Revision: 7.27 $"
#ident "$Revision: 1.5 $"
#ident "$Revision: 1.34 $"
#ident "$Revision: 1.17 $"

/*
 * Keeps a ship on the track centre line: on the ground it only pitches the ship to follow
 * the slope, in the air it steers it towards the sections ahead. Nothing calls it. Its
 * frame still holds the unused vectors of the ShipTrkReaction it was copied from.
 */
void ShipFollowTrack(ShipData *shipIndex, Object **shipShapes, s32 shipNo, void *weaponIndex,
                     void *padInfo, s32 scale) {
    VECTOR offset;
    VECTOR direction;
    VECTOR unusedVec0;
    VECTOR targetPos;
    VECTOR unusedVec1;
    VECTOR unusedVec2;
    VECTOR unusedVec3;
    VECTOR unusedVec4;
    ShipData *ship;
    TrackSection *section;
    TrackSection *next;
    Face *face;
    s32 mag;

    ship = &shipIndex[shipNo];
    offset.vx = 0;
    offset.vy = 0;
    offset.vz = 0;
    if (!(ship->attr & FLYING)) {
        face = (Face *)track->faces + ship->nearTrkSect->faceStart;
        while (!(face->flags & TRACK_BASE)) {
            face++;
        }
        followRaceLine(&offset, shipIndex, face, shipNo);
        section = ship->nearTrkSect;
        next = section->nextSection.ptr;
        if ((section->flags & TRACK_JUMP) || (next->flags & TRACK_JUMP) ||
            (section->prevSection.ptr->flags & TRACK_JUMP) || (next->nextSection.ptr->flags & TRACK_JUMP) ||
            (next->nextSection.ptr->nextSection.ptr->flags & TRACK_JUMP)) {
            direction.vx = ship->nearTrkSect->centre.vx - ship->nearTrkSect->prevSection.ptr->centre.vx;
            direction.vy = ship->nearTrkSect->centre.vy - ship->nearTrkSect->prevSection.ptr->centre.vy;
            direction.vz = ship->nearTrkSect->centre.vz - ship->nearTrkSect->prevSection.ptr->centre.vz;
        } else {
            direction.vx = next->centre.vx - section->centre.vx;
            direction.vy = next->centre.vy - section->centre.vy;
            direction.vz = next->centre.vz - section->centre.vz;
        }
        mag = GetMagnitude(direction);
        direction.vx = (direction.vx * ship->speed) / mag;
        direction.vy = (direction.vy * ship->speed) / mag;
        direction.vz = (direction.vz * ship->speed) / mag;
        targetPos.vx = section->centre.vx + offset.vx;
        targetPos.vy = section->centre.vy + offset.vy;
        targetPos.vz = section->centre.vz + offset.vz;
        ship->vpitch = -ratan2(direction.vy, SquareRoot0(direction.vx * direction.vx + direction.vz * direction.vz)) -
                       ship->pitch;
        PAL_SPEED_UP(ship->vpitch);
        ship->pitch += ship->vpitch >> 4;
    } else {
        section = ship->nearTrkSect;
        section = section->nextSection.ptr;
        section = section->nextSection.ptr;
        next = section->nextSection.ptr;
        /* face is never set on this path; the call receives whatever the register holds. */
        moveDownCentre(&offset, ship, face, shipNo);
        ship->speed -= abs(ship->speed * ship->vhdg) >> 14;
        direction.vx = next->centre.vx - section->centre.vx;
        direction.vy = next->centre.vy - section->centre.vy;
        direction.vz = next->centre.vz - section->centre.vz;
        mag = GetMagnitude(direction);
        direction.vx = (direction.vx * ship->speed) / mag;
        direction.vz = (direction.vz * ship->speed) / mag;
        direction.vy = 500;
        GetTargetPos(&targetPos, ship->ppivot, next->centre, ship->nearTrkSect->centre);
        ship->apivot.vx = direction.vx + ((targetPos.vx - ship->ppivot.vx) >> 1);
        ship->apivot.vy = direction.vy;
        ship->apivot.vz = direction.vz + ((targetPos.vz - ship->ppivot.vz) >> 1);
        PAL_SPEED_VECTOR(ship->apivot);
        ship->vpivot.vx += ship->apivot.vx;
        ship->vpitch = -200 - ship->pitch;
        ship->pitch += ship->vpitch >> 4;
        ship->vpivot.vy += ship->apivot.vy;
        ship->vpivot.vz += ship->apivot.vz;
        ship->vhdg = -ratan2(direction.vx, direction.vz) - ship->hdg;
        if (ship->vhdg > 2048) {
            ship->vhdg -= 4096;
        } else if (ship->vhdg < -2048) {
            ship->vhdg += 4096;
        }
        ship->hdg += ship->vhdg >> 4;
    }
    ship->ppivot.vy += (ship->vpivot.vy * scale) >> 6;
    ship->pitch = ang(ship->pitch);
}

/* Per camera: whether the sky is drawn, cleared on the parts of each track where it can't be seen. */
void UpdateSkyVisibility(s32 section, s32 shipNo) {
    s32 player;

    player = 0;
    if (shipNo == ownShip) {
        player = 0;
    } else if (shipNo == ownShip2) {
        player = 1;
    } else if (shipNo == ownShip3) {
        player = 2;
    } else if (shipNo == ownShip4) {
        player = 3;
    }

    skyVisible[player] = TRUE;
    switch (frontendOptions_g.trackNo) {
        case 1:
            if (frontendOptions_g.numPlayers < 3 && (section >= 169 && section <= 225) &&
                (ships[shipNo].attr & DIRECTION)) {
                skyVisible[player] = FALSE;
            }
            if (frontendOptions_g.numPlayers < 3 && (section >= 181 && section <= 230) &&
                !(ships[shipNo].attr & DIRECTION)) {
                skyVisible[player] = FALSE;
            }
            break;
        case 2:
            if (frontendOptions_g.numPlayers < 3) {
                if ((section >= 0 && section <= 153) && (ships[shipNo].attr & DIRECTION)) {
                    skyVisible[player] = FALSE;
                }
                if ((section >= 202 && section <= 457) && (ships[shipNo].attr & DIRECTION)) {
                    skyVisible[player] = FALSE;
                }
                if ((section >= 0 && section <= 159) && !(ships[shipNo].attr & DIRECTION)) {
                    skyVisible[player] = FALSE;
                }
                if ((section >= 219 && section <= 457) && !(ships[shipNo].attr & DIRECTION)) {
                    skyVisible[player] = FALSE;
                }
            } else {
                skyVisible[player] = FALSE;
            }
            break;
        case 3:
            if ((section >= 267 && section <= 277) && (ships[shipNo].attr & DIRECTION)) {
                skyVisible[player] = FALSE;
            }
            if (frontendOptions_g.numPlayers < 3 && (section >= 396 && section <= 404) &&
                (ships[shipNo].attr & DIRECTION)) {
                skyVisible[player] = FALSE;
            }
            if ((section >= 433 && section <= 444) && (ships[shipNo].attr & DIRECTION)) {
                skyVisible[player] = FALSE;
            }
            if ((section >= 270 && section <= 284) && !(ships[shipNo].attr & DIRECTION)) {
                skyVisible[player] = FALSE;
            }
            if ((section >= 434 && section <= 450) && !(ships[shipNo].attr & DIRECTION)) {
                skyVisible[player] = FALSE;
            }
            break;
        case 4:
            if ((section >= 100 && section <= 123) && (ships[shipNo].attr & DIRECTION)) {
                skyVisible[player] = FALSE;
            }
            if (frontendOptions_g.numPlayers < 3) {
                if ((section >= 229 && section <= 277) && (ships[shipNo].attr & DIRECTION)) {
                    skyVisible[player] = FALSE;
                }
                if ((section >= 297 && section <= 357) && (ships[shipNo].attr & DIRECTION)) {
                    skyVisible[player] = FALSE;
                }
            }
            if ((section >= 121 && section <= 147) && !(ships[shipNo].attr & DIRECTION)) {
                skyVisible[player] = FALSE;
            }
            if (frontendOptions_g.numPlayers < 3) {
                if ((section >= 231 && section <= 283) && !(ships[shipNo].attr & DIRECTION)) {
                    skyVisible[player] = FALSE;
                }
                if ((section >= 313 && section <= 365) && !(ships[shipNo].attr & DIRECTION)) {
                    skyVisible[player] = FALSE;
                }
            }
            break;
        case 5:
            if (frontendOptions_g.numPlayers < 3 && (section >= 171 && section <= 202) &&
                (ships[shipNo].attr & DIRECTION)) {
                skyVisible[player] = FALSE;
            }
            if ((section >= 345 && section <= 371) && (ships[shipNo].attr & DIRECTION)) {
                skyVisible[player] = FALSE;
            }
            if (frontendOptions_g.numPlayers < 3 && (section >= 185 && section <= 204) &&
                !(ships[shipNo].attr & DIRECTION)) {
                skyVisible[player] = FALSE;
            }
            if ((section >= 347 && section <= 371) && !(ships[shipNo].attr & DIRECTION)) {
                skyVisible[player] = FALSE;
            }
            break;
        case 6:
        case 7:
            break;
    }
}

/*
 * Core ship-to-track dynamics: computes thrust, hover reaction force, speed-up pads, drag,
 * attitude control, jump/landing handling, and rescue droid triggers for grounded and flying ships.
 */
void ShipTrkReaction(ShipData *playerShip, Object **shipShapes) {
    VECTOR reaction;
    VECTOR targetVelocity;
    VECTOR nosePos;
    VECTOR facePoint;
    VECTOR targetPos;
    VECTOR toTarget;
    VECTOR direction;
    VECTOR speedVector;
    VECTOR nextDirection;
    s16 boost;
    s16 brake;
    s32 speed;
    s32 clearance;
    s32 altitudeDifference;
    s32 friction;
    s32 mass;
    s32 alpha;
    Face *face;
    TrackSection *current;
    TrackSection *neighbour;
    TrackSection *next;

    if (playerShip->thrustCount == 0) {
        boost = 1;
    } else if ((u16)(playerShip->thrustCount - 1) < 2) {
        boost = 3;
    } else if (playerShip->thrustCount >= 3) {
        boost = 6;
    }
    playerShip->thrustVector.vx = (playerShip->thrust * (playerShip->unitVecNose.vx * boost)) >> 6;
    playerShip->thrustVector.vy = (playerShip->thrust * (playerShip->unitVecNose.vy * boost)) >> 6;
    playerShip->thrustVector.vz = (playerShip->thrust * (playerShip->unitVecNose.vz * boost)) >> 6;
    speedVector.vx = playerShip->vpivot.vx >> 1;
    speedVector.vy = playerShip->vpivot.vy >> 1;
    speedVector.vz = playerShip->vpivot.vz >> 1;
    speed = GetMagnitude(speedVector) << 1;
    if (playerShip->autopilotCount == 0) {
        playerShip->speed = speed >> 1;
    }
    targetVelocity.vx = sar(speed * playerShip->unitVecNose.vx, 12);
    targetVelocity.vy = sar(speed * playerShip->unitVecNose.vy, 12);
    targetVelocity.vz = sar(speed * playerShip->unitVecNose.vz, 12);

    if (playerShip->nearTrkSect->flags & TRACK_JUMP) {
        face = (Face *)track->faces + playerShip->nearTrkSect->faceStart;
        while (!(face->flags & TRACK_BASE)) {
            face++;
        }
        GetFaceVertex(playerShip->nearTrkSect, face, 0, &facePoint);
        alpha = PlaneLineCollision(facePoint, playerShip->ppivot, face->normal);
        if (CheckPolygonCollision(face, playerShip->ppivot, alpha, 2, playerShip->nearTrkSect) == 0) {
            playerShip->attr |= FLYING;
        }
    }

    playerShip->pitchLocked = FALSE;
    if (!(playerShip->attr & FLYING)) {
        face = (Face *)track->faces + playerShip->nearTrkSect->faceStart;
        while (!(face->flags & TRACK_BASE)) {
            face++;
        }
        Ship2Track(playerShip, face, shipShapes);
        if ((face->flags & SPEED_UP) && (playerShip->attr & LSIDE)) {
            playerShip->vpivot.vx += PAL_SPEED(playerShip->nearTrkSect->nextSection.ptr->centre.vx -
                                               playerShip->nearTrkSect->centre.vx);
            playerShip->vpivot.vy += PAL_SPEED(playerShip->nearTrkSect->nextSection.ptr->centre.vy -
                                               playerShip->nearTrkSect->centre.vy);
            playerShip->vpivot.vz += PAL_SPEED(playerShip->nearTrkSect->nextSection.ptr->centre.vz -
                                               playerShip->nearTrkSect->centre.vz);
        }
        face++;
        if ((face->flags & SPEED_UP) && !(playerShip->attr & LSIDE)) {
            playerShip->vpivot.vx += PAL_SPEED(playerShip->nearTrkSect->nextSection.ptr->centre.vx -
                                               playerShip->nearTrkSect->centre.vx);
            playerShip->vpivot.vy += PAL_SPEED(playerShip->nearTrkSect->nextSection.ptr->centre.vy -
                                               playerShip->nearTrkSect->centre.vy);
            playerShip->vpivot.vz += PAL_SPEED(playerShip->nearTrkSect->nextSection.ptr->centre.vz -
                                               playerShip->nearTrkSect->centre.vz);
        }
        face--;
        if (!(playerShip->attr & LSIDE)) {
            face++;
        }
        GetFaceVertex(playerShip->nearTrkSect, face, 0, &facePoint);
        alpha = PlaneLineCollision(facePoint, playerShip->ppivot, face->normal);
        if (alpha < -300) {
            alpha = -300;
        }
        clearance = alpha;
        Ship2TrackBase(playerShip, face, clearance);
        brake = playerShip->r_brake_rot + playerShip->l_brake_rot;
        playerShip->apivot.vx = (targetVelocity.vx - playerShip->vpivot.vx) / (playerShip->skid + (brake >> 2));
        playerShip->apivot.vy = (targetVelocity.vy - playerShip->vpivot.vy) / (playerShip->skid + (brake >> 2));
        playerShip->apivot.vz = (targetVelocity.vz - playerShip->vpivot.vz) / (playerShip->skid + (brake >> 2));
        if (clearance < 75) {
            alpha = 75;
        }
        reaction.vx = (face->normal.vx << 14) / alpha;
        reaction.vy = (face->normal.vy << 14) / alpha;
        reaction.vz = (face->normal.vz << 14) / alpha;
        reaction.vx -= face->normal.vx << 6;
        reaction.vy -= face->normal.vy << 6;
        reaction.vz -= face->normal.vz << 6;
        reaction.vy += 30000;
        mass = playerShip->mass;
        altitudeDifference = playerShip->nearTrkSect->centre.vy - playerShip->ppivot.vy;
        if (altitudeDifference > 704) {
            if (mass == 110) {
                playerShip->ppivot.vy += 80;
            }
            if (playerShip->vpivot.vy < 0) {
                playerShip->vpivot.vy >>= 1;
            }
            playerShip->ppivot.vy += 16;
        }
        reaction.vy += altitudeDifference << 6;
        playerShip->apivot.vx += (reaction.vx + playerShip->thrustVector.vx) / mass;
        playerShip->apivot.vy += (reaction.vy + playerShip->thrustVector.vy) / mass;
        playerShip->apivot.vz += (reaction.vz + playerShip->thrustVector.vz) / mass;
        PAL_SPEED_VECTOR(playerShip->apivot);
        friction = (playerShip->resistance * (74 - (brake >> 3))) >> 7;
        playerShip->vpivot.vx += playerShip->apivot.vx;
        playerShip->vpivot.vy += playerShip->apivot.vy;
        playerShip->vpivot.vz += playerShip->apivot.vz;
        playerShip->vpivot.vx -= playerShip->vpivot.vx / friction;
        playerShip->vpivot.vy -= playerShip->vpivot.vy / friction;
        playerShip->vpivot.vz -= playerShip->vpivot.vz / friction;
        playerShip->ppivot.vx += sar(playerShip->vpivot.vx, 6);
        playerShip->ppivot.vy += sar(playerShip->vpivot.vy, 6);
        playerShip->ppivot.vz += sar(playerShip->vpivot.vz, 6);
        nosePos.vx = (playerShip->unitVecNose.vx >> 5) + playerShip->ppivot.vx;
        nosePos.vy = (playerShip->unitVecNose.vy >> 5) + playerShip->ppivot.vy;
        nosePos.vz = (playerShip->unitVecNose.vz >> 5) + playerShip->ppivot.vz;
        alpha = PlaneLineCollision(facePoint, nosePos, face->normal);
        if (playerShip->rotationCorrectionDisabled == 0) {
            clearance -= alpha;
            if (alpha < 600) {
                if (!playerShip->pitchLocked) {
                    s16 vpitch = playerShip->vpitch + 5;
                    playerShip->vpitch = vpitch + clearance;
                }
            } else if (!playerShip->pitchLocked) {
                playerShip->vpitch -= 50;
            }
            if (!playerShip->pitchLocked) {
                playerShip->vpitch -= playerShip->vpitch >> 2;
            }
            playerShip->hdg +=
                sar((s16)sar(playerShip->r_brake_rot - playerShip->l_brake_rot, 3) * playerShip->speed, 15);
            playerShip->vroll += sar(playerShip->vhdg, 5);
            playerShip->vroll -= playerShip->vroll >> 1;
        }
    } else {
        if (playerShip->vpivot.vy < playerShip->nearTrkSect->centre.vy) {
            face = (Face *)track->faces + playerShip->nearTrkSect->faceStart;
            while (!(face->flags & TRACK_BASE)) {
                face++;
            }
            Ship2Track(playerShip, face, shipShapes);
        }
        neighbour = playerShip->nearTrkSect->nextSection.ptr;
        GetTargetPos(&targetPos, playerShip->ppivot, neighbour->centre, playerShip->nearTrkSect->centre);
        toTarget.vx = targetPos.vx - playerShip->ppivot.vx;
        toTarget.vy = targetPos.vy - playerShip->ppivot.vy;
        toTarget.vz = targetPos.vz - playerShip->ppivot.vz;
        if (toTarget.vy > 0) {
            toTarget.vy >>= 7;
        } else {
            toTarget.vx <<= 3;
            toTarget.vy <<= 3;
            toTarget.vz <<= 3;
        }
        if (GetMagnitude(toTarget) > 32000) {
            playerShip->update = UpdateRescueDroid;
            playerShip->updateCount = 500;
            playerShip->thrust = 0;
            droidFlyin = 1000;
            playerShip->attr |= IN_TOW | RESCUE;
            if (g_gameType == GAME_TIME_TRIAL) {
                recordDisabled = 1;
            }
        }
        brake = playerShip->r_brake_rot + playerShip->l_brake_rot;
        playerShip->apivot.vx = (targetVelocity.vx - playerShip->vpivot.vx) / ((brake << 2) + 20);
        playerShip->apivot.vy = (targetVelocity.vy - playerShip->vpivot.vy) / ((brake << 2) + 20);
        playerShip->apivot.vz = (targetVelocity.vz - playerShip->vpivot.vz) / ((brake << 2) + 20);
        altitudeDifference = playerShip->nearTrkSect->centre.vy - playerShip->ppivot.vy;
        if (altitudeDifference > 704) {
            /* mass is only set on the ground path; this reads whatever it held. */
            if (mass == 110) {
                playerShip->ppivot.vy += 80;
            }
            if (playerShip->vpivot.vy < 0) {
                playerShip->vpivot.vy >>= 1;
            }
            playerShip->ppivot.vy += 16;
        }
        playerShip->apivot.vx += playerShip->thrustVector.vx / playerShip->mass;
        playerShip->apivot.vy += (playerShip->thrustVector.vy + 80000) / playerShip->mass;
        playerShip->apivot.vz += playerShip->thrustVector.vz / playerShip->mass;
        PAL_SPEED_VECTOR(playerShip->apivot);
        friction = (playerShip->resistance * (74 - (brake >> 3))) >> 7;
        playerShip->vpivot.vx += playerShip->apivot.vx;
        playerShip->vpivot.vy += playerShip->apivot.vy;
        playerShip->vpivot.vz += playerShip->apivot.vz;
        playerShip->vpivot.vx -= playerShip->vpivot.vx / friction;
        playerShip->vpivot.vy -= playerShip->vpivot.vy / friction;
        playerShip->vpivot.vz -= playerShip->vpivot.vz / friction;
        playerShip->ppivot.vx += sar(playerShip->vpivot.vx, 6);
        playerShip->ppivot.vy += sar(playerShip->vpivot.vy, 6);
        playerShip->ppivot.vz += sar(playerShip->vpivot.vz, 6);
        playerShip->vpitch -= PAL_SPEED(50);
        playerShip->vpitch -= playerShip->vpitch >> 2;
        playerShip->hdg +=
            sar((s16)sar(playerShip->r_brake_rot - playerShip->l_brake_rot, 3) * playerShip->speed, 15);
        playerShip->vroll += sar(playerShip->vhdg, 5);
        playerShip->vroll -= sar(playerShip->vroll, 1);
    }

    playerShip->hdg += sar(playerShip->vhdg, 6);
    if (!playerShip->pitchLocked) {
        playerShip->pitch += playerShip->vpitch >> 4;
    }
    playerShip->roll += playerShip->vroll;
    playerShip->roll -= playerShip->roll >> 3;
    playerShip->hdg = ang(playerShip->hdg);
    if (!playerShip->pitchLocked) {
        playerShip->pitch = ang(playerShip->pitch);
    }
    playerShip->roll = ang(playerShip->roll);

    neighbour = playerShip->nearTrkSect->prevSection.ptr;
    current = playerShip->nearTrkSect;
    if (!(playerShip->attr & DIRECTION) && !(current->flags & TRACK_JUMP) && (neighbour->flags & TRACK_JUMP)) {
        if (!playerShip->pitchLocked) {
            playerShip->pitch -= playerShip->vpitch >> 4;
        }
        playerShip->ppivot.vx -= sar(playerShip->vpivot.vx, 6);
        playerShip->ppivot.vy -= sar(playerShip->vpivot.vy, 6);
        playerShip->ppivot.vz -= sar(playerShip->vpivot.vz, 6);
        direction.vx = playerShip->nearTrkSect->nextSection.ptr->centre.vx - playerShip->nearTrkSect->centre.vx;
        direction.vy = playerShip->nearTrkSect->nextSection.ptr->centre.vy - playerShip->nearTrkSect->centre.vy;
        direction.vz = playerShip->nearTrkSect->nextSection.ptr->centre.vz - playerShip->nearTrkSect->centre.vz;
        playerShip->vpivot.vx = direction.vx << 2;
        playerShip->vpivot.vy = direction.vy << 2;
        playerShip->vpivot.vz = direction.vz << 2;
        playerShip->ppivot.vx += sar(playerShip->vpivot.vx, 6);
        playerShip->ppivot.vy += sar(playerShip->vpivot.vy, 6);
        playerShip->ppivot.vz += sar(playerShip->vpivot.vz, 6);
    }

    next = playerShip->nearTrkSect->nextSection.ptr;
    nextDirection.vx = next->centre.vx - playerShip->nearTrkSect->centre.vx;
    nextDirection.vy = next->centre.vy - playerShip->nearTrkSect->centre.vy;
    nextDirection.vz = next->centre.vz - playerShip->nearTrkSect->centre.vz;
    if (nextDirection.vx * playerShip->unitVecNose.vx + nextDirection.vy * playerShip->unitVecNose.vy +
                nextDirection.vz * playerShip->unitVecNose.vz <
            0 &&
        playerShip->ppivot.vy >= playerShip->nearTrkSect->centre.vy + 1000 && (playerShip->attr & FLYING) &&
        (current->flags & TRACK_JUMP) && !(current->prevSection.ptr->flags & TRACK_JUMP)) {
        playerShip->update = UpdateRescueDroid;
        playerShip->updateCount = 500;
        playerShip->thrust = 0;
        droidFlyin = 1000;
        playerShip->attr |= IN_TOW | RESCUE;
    }
}

/* Projects one vertex of objectCollide into camera space: the camera and upper skeletons are
   parked at the origin while the object's transform is set, then their translations are restored. */
void getWorldPoint(VECTOR *output, Object *object, Object *objectCollide, s32 pointNo) {
    VECTOR cameraPosition;
    VECTOR worldPosition;
    s32 flag;

    cameraPosition.vx = cameras[0].camPos->relative.t[0];
    cameraPosition.vy = cameras[0].camPos->relative.t[1];
    cameraPosition.vz = cameras[0].camPos->relative.t[2];
    worldPosition.vx = upperSkeletons[0]->relative.t[0];
    worldPosition.vy = upperSkeletons[0]->relative.t[1];
    worldPosition.vz = upperSkeletons[0]->relative.t[2];
    SetSkeletonPosition(cameras[0].camPos, 0, 0, 0);
    SetSkeletonDirection(cameras[0].camPos, 0, 0, 0);
    SetSkeletonPosition(upperSkeletons[0], 0, 0, 0);
    SetSkeletonDirection(upperSkeletons[0], 0, 0, 0);
    SetSkeletonTransform(object->skeleton);
    cameras[0].camPos->relative.t[0] = cameraPosition.vx;
    cameras[0].camPos->relative.t[1] = cameraPosition.vy;
    cameras[0].camPos->relative.t[2] = cameraPosition.vz;
    upperSkeletons[0]->relative.t[0] = worldPosition.vx;
    upperSkeletons[0]->relative.t[1] = worldPosition.vy;
    upperSkeletons[0]->relative.t[2] = worldPosition.vz;
    RotTrans((SVECTOR *)objectCollide->vertices + pointNo, output, &flag);
}

/* The point on the line cent1-cent2 closest to pos, from the angle at cent1 (law of cosines). */
void GetTargetPos(VECTOR *result, VECTOR pos, Vector cent1, Vector cent2) {
    register s32 a_mag;
    register s32 b_mag;
    register s32 c_mag;
    VECTOR a;
    VECTOR b;
    VECTOR c;
    s32 a_sq;
    s32 b_sq;
    s32 c_sq;
    s32 cos_alpha;
    s32 angle;

    a.vx = pos.vx - cent1.vx;
    a.vy = pos.vy - cent1.vy;
    a.vz = pos.vz - cent1.vz;
    b.vx = cent2.vx - cent1.vx;
    b.vy = cent2.vy - cent1.vy;
    b.vz = cent2.vz - cent1.vz;
    c.vx = pos.vx - cent2.vx;
    c.vy = pos.vy - cent2.vy;
    c.vz = pos.vz - cent2.vz;
    a_mag = GetMagnitude(a);
    b_mag = GetMagnitude(b);
    c_mag = GetMagnitude(c);
    a_sq = a_mag * a_mag;
    b_sq = b_mag * b_mag;
    c_sq = c_mag * c_mag;
    if ((a_mag != 0) && (b_mag != 0)) {
        cos_alpha = (((-b_sq) - (a_sq + c_sq)) << 12) / ((a_mag * b_mag) << 1);
        angle = arccos(cos_alpha) >> 2;
        c_mag = (rcos(angle) * a_mag) >> 12;
        result->vx = cent1.vx - ((b.vx * c_mag) / b_mag);
        result->vy = cent1.vy - ((b.vy * c_mag) / b_mag);
        result->vz = cent1.vz - ((b.vz * c_mag) / b_mag);
    }
}

/* Updates every ship's nose/wing/fin basis vectors, nearest track section, and left/right lane flag. */
void SetUnitVectors(ShipData *shipIndex) {
    s16 sx;
    s16 sy;
    s16 sz;
    s16 cx;
    s16 cy;
    s16 cz;
    s32 i;
    VECTOR direction;
    VECTOR vertex0;
    VECTOR vertex1;
    VECTOR toFaceVector;
    VECTOR facePoint; /* unused: keeps the stack frame size */
    s32 toFace;
    Face *facePtr;

    for (i = 0; i < 15; i++) {
        sy = rsin(shipIndex[i].hdg);
        cy = rcos(shipIndex[i].hdg);
        sx = rsin(shipIndex[i].pitch);
        cx = rcos(shipIndex[i].pitch);
        sz = rsin(shipIndex[i].roll);
        cz = rcos(shipIndex[i].roll);
        shipIndex[i].unitVecNose.vx = -((sy * cx) >> 12);
        shipIndex[i].unitVecNose.vy = -sx;
        shipIndex[i].unitVecNose.vz = (cy * cx) >> 12;
        shipIndex[i].unitVecWing.vx = ((cy * cz) >> 12) + ((((sy * sz) >> 12) * sx) >> 12);
        shipIndex[i].unitVecWing.vy = (-(sz * cx)) >> 12;
        shipIndex[i].unitVecWing.vz = ((sy * cz) >> 12) - ((((cy * sx) >> 12) * sz) >> 12);
        OuterProduct12(&shipIndex[i].unitVecNose, &shipIndex[i].unitVecWing, &shipIndex[i].unitVecFin);
        shipIndex[i].prevShipSect = shipIndex[i].nearTrkSect;
        SectionSearch(&shipIndex[i]);
        shipIndex[i].prevSecNo = shipIndex[i].prevShipSect->secNo;
        shipIndex[i].nearSecNo = shipIndex[i].nearTrkSect->secNo;

        facePtr = (Face *)track->faces + shipIndex[i].nearTrkSect->faceStart;
        while (!(facePtr->flags & TRACK_BASE)) {
            facePtr++;
        }

        GetFaceVertex(shipIndex[i].nearTrkSect, facePtr, 0, &vertex0);
        GetFaceVertex(shipIndex[i].nearTrkSect, facePtr, 1, &vertex1);
        toFaceVector.vx = vertex0.vx - vertex1.vx;
        toFaceVector.vy = vertex0.vy - vertex1.vy;
        toFaceVector.vz = vertex0.vz - vertex1.vz;
        direction.vx = shipIndex[i].nearTrkSect->centre.vx - shipIndex[i].ppivot.vx;
        direction.vy = shipIndex[i].nearTrkSect->centre.vy - shipIndex[i].ppivot.vy;
        direction.vz = shipIndex[i].nearTrkSect->centre.vz - shipIndex[i].ppivot.vz;
        toFace = direction.vx * toFaceVector.vx + direction.vy * toFaceVector.vy + direction.vz * toFaceVector.vz;
        if (toFace > 0) {
            shipIndex[i].attr |= LSIDE;
        } else {
            shipIndex[i].attr &= ~LSIDE;
        }
    }
}

/* Whether weapon grids are lit: in challenges unless weapons are off (or that challenge's
   table says so), and in races with weapons switched on. */
#define WEAPON_GRIDS_LIT()                                                                              \
    (((g_gameType == GAME_CHALLENGE && frontendOptions_g.challengeWeapons != CHALLENGE_WEAPONS_OFF) ||  \
      ((g_gameType == GAME_SINGLE_RACE || g_gameType == GAME_CHAMPIONSHIP) && frontendOptions_g.weaponsOn)) && \
     (g_gameType != GAME_CHALLENGE || frontendOptions_g.challengeWeapons != CHALLENGE_WEAPONS_TABLE ||        \
      challenges[frontendOptions_g.challengeNo].weaponsOn))

/* Colours a section's left/right base faces for weapon grids, speed-up pads, pit lanes and
   checkpoints, and appends its weapon grids and checkpoints to the race tables. */
s32 SetTrackAttributes(TrackSection *section, s16 sectionNo, WeaponGrid *weaponGrid, s32 noWeaponGrids,
                       s16 *checkPointCount, SpecialGrid *specialGrid) {
    Face *facePtr;

    facePtr = (Face *)track->faces + section->faceStart;
    while (!(facePtr->flags & TRACK_BASE)) {
        facePtr++;
    }

    if (facePtr->flags & WEAPON_GRID_LEFT) {
        if (WEAPON_GRIDS_LIT()) {
            facePtr->red = 255;
            facePtr->green = 35;
            facePtr->blue = 117;
        }
        weaponGrid[noWeaponGrids].section = section;
        weaponGrid[noWeaponGrids].count = 0;
        weaponGrid[noWeaponGrids].attr = WEAPON_GRID_LEFT;
        noWeaponGrids++;
    }
    if (facePtr->flags & SPEED_UP) {
        facePtr->red = 35;
        facePtr->green = 35;
        facePtr->blue = 255;
    }
    if (facePtr->flags & PIT_LANE) {
        facePtr->red = 128;
        facePtr->green = 0;
        facePtr->blue = 0;
    }
    if (facePtr->flags & CHECKPOINT) {
        specialGrid[*checkPointCount].count = 10;
        specialGrid[*checkPointCount].section = section->secNo;
        specialGrid[*checkPointCount].attr = -1;
        checkPointReached[*checkPointCount][0] = -1;
        checkPointReached[*checkPointCount][1] = -1;
        checkPointReached[*checkPointCount][2] = -1;
        checkPointReached[*checkPointCount][3] = -1;
        facePtr->red = 255;
        facePtr->green = 255;
        facePtr->blue = 255;
        (*checkPointCount)++;
        if (!((*checkPointCount) < MAX_CHECKPOINTS)) {
            __assert("(*checkPointCount)< MAX_CHECKPOINTS", "Dynam.c", 1282);
        }
    }

    facePtr++;
    if (facePtr->flags & WEAPON_GRID_RIGHT) {
        if (WEAPON_GRIDS_LIT()) {
            facePtr->red = 84;
            facePtr->green = 238;
            facePtr->blue = 117;
        }
        weaponGrid[noWeaponGrids].section = section;
        weaponGrid[noWeaponGrids].count = 0;
        weaponGrid[noWeaponGrids].attr = WEAPON_GRID_RIGHT;
        noWeaponGrids++;
    } else if (facePtr->flags & SPEED_UP) {
        facePtr->red = 35;
        facePtr->green = 35;
        facePtr->blue = 255;
    }
    if (facePtr->flags & PIT_LANE) {
        facePtr->red = 128;
        facePtr->green = 0;
        facePtr->blue = 0;
    }
    if (facePtr->flags & CHECKPOINT) {
        facePtr->red = 255;
        facePtr->green = 255;
        facePtr->blue = 255;
    }
    return noWeaponGrids;
}
