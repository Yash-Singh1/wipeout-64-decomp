#include "collide.h"

#ident "$Revision: 1.17 $"

void doShipShipCollision(s32 shipNo, ShipData *shipIndex, Object **shipShapes) {
    s16 infrontShip;

    if (!(shipIndex[shipNo].combatAttr & SHIP_NO_COLLIDE)) {
        infrontShip = getShipInfrontLAP(shipIndex, shipNo);
        if (infrontShip != -1) {
            Ship2ShipCollision(shipIndex, shipShapes, infrontShip, shipNo);
        }
    }
}

void Ship2ShipCollision(ShipData *shipIndex, Object **shipShapes, s32 i, s32 j) {
    s32 mag;
    VECTOR distance;
    VECTOR unused; /* unused: retail reserves 0x10-byte stack slot */
    VECTOR vc;
    s32 ownMass;
    s32 remMass;
    VECTOR ship2ship;
    VECTOR ship1React;
    VECTOR ship2React;
    VECTOR soundPos;
    VECTOR sparkPos;

    distance.vx = (shipIndex[i].ppivot.vx - shipIndex[j].ppivot.vx) >> 3;
    distance.vy = (shipIndex[i].ppivot.vy - shipIndex[j].ppivot.vy) >> 3;
    distance.vz = (shipIndex[i].ppivot.vz - shipIndex[j].ppivot.vz) >> 3;
    mag = GetMagnitude(distance);
    if (mag < 120) {
        if (collideLineToPlane(j, i, shipShapes, shipIndex) == 1) {
            if (j == ownShip || j == ownShip2 || j == ownShip3 || j == ownShip4) {
                ownMass = shipIndex[j].mass;
            } else {
                ownMass = shipIndex[j].mass * 2;
            }
            /* The retail game tests j again here, so remote ships always count double. */
            if (j == ownShip || j == ownShip2 || j == ownShip3 || j == ownShip4) {
                remMass = shipIndex[i].mass;
            } else {
                remMass = shipIndex[i].mass * 2;
            }
            vc.vx = (shipIndex[j].vpivot.vx * ownMass + shipIndex[i].vpivot.vx * remMass) / (ownMass + remMass);
            vc.vy = (shipIndex[j].vpivot.vy * ownMass + shipIndex[i].vpivot.vy * remMass) / (ownMass + remMass);
            vc.vz = (shipIndex[j].vpivot.vz * ownMass + shipIndex[i].vpivot.vz * remMass) / (ownMass + remMass);
            ship1React.vx = (vc.vx - shipIndex[j].vpivot.vx) >> 1;
            ship1React.vy = (vc.vy - shipIndex[j].vpivot.vy) >> 1;
            ship1React.vz = (vc.vz - shipIndex[j].vpivot.vz) >> 1;
            ship2React.vx = (vc.vx - shipIndex[i].vpivot.vx) >> 1;
            ship2React.vy = (vc.vy - shipIndex[i].vpivot.vy) >> 1;
            ship2React.vz = (vc.vz - shipIndex[i].vpivot.vz) >> 1;
            shipIndex[j].ppivot.vx -= sar(shipIndex[j].vpivot.vx, 6);
            shipIndex[j].ppivot.vy -= sar(shipIndex[j].vpivot.vy, 6);
            shipIndex[j].ppivot.vz -= sar(shipIndex[j].vpivot.vz, 6);
            shipIndex[i].ppivot.vx -= sar(shipIndex[i].vpivot.vx, 6);
            shipIndex[i].ppivot.vy -= sar(shipIndex[i].vpivot.vy, 6);
            shipIndex[i].ppivot.vz -= sar(shipIndex[i].vpivot.vz, 6);
            shipIndex[j].vpivot.vx = vc.vx + ship1React.vx;
            shipIndex[j].vpivot.vy = vc.vy + ship1React.vy;
            shipIndex[j].vpivot.vz = vc.vz + ship1React.vz;
            shipIndex[i].vpivot.vx = vc.vx + ship2React.vx;
            shipIndex[i].vpivot.vy = vc.vy + ship2React.vy;
            shipIndex[i].vpivot.vz = vc.vz + ship2React.vz;
            ship2ship.vx = shipIndex[i].ppivot.vx - shipIndex[j].ppivot.vx;
            ship2ship.vy = shipIndex[i].ppivot.vy - shipIndex[j].ppivot.vy;
            ship2ship.vz = shipIndex[i].ppivot.vz - shipIndex[j].ppivot.vz;
            shipIndex[i].vpivot.vx += ship2ship.vx << 2;
            shipIndex[i].vpivot.vy += ship2ship.vy << 2;
            shipIndex[i].vpivot.vz += ship2ship.vz << 2;
            shipIndex[j].vpivot.vx -= ship2ship.vx << 2;
            shipIndex[j].vpivot.vy -= ship2ship.vy << 2;
            shipIndex[j].vpivot.vz -= ship2ship.vz << 2;
            shipIndex[j].ppivot.vx += sar(shipIndex[j].vpivot.vx, 6);
            shipIndex[j].ppivot.vy += sar(shipIndex[j].vpivot.vy, 6);
            shipIndex[j].ppivot.vz += sar(shipIndex[j].vpivot.vz, 6);
            shipIndex[i].ppivot.vx += sar(shipIndex[i].vpivot.vx, 6);
            shipIndex[i].ppivot.vy += sar(shipIndex[i].vpivot.vy, 6);
            shipIndex[i].ppivot.vz += sar(shipIndex[i].vpivot.vz, 6);
            if (!(shipIndex[i].attr & SHIP_COLL) && !(shipIndex[j].attr & SHIP_COLL)) {
                soundPos.vx = (shipIndex[i].ppivot.vx + shipIndex[j].ppivot.vx) >> 1;
                soundPos.vy = (shipIndex[i].ppivot.vy + shipIndex[j].ppivot.vy) >> 1;
                soundPos.vz = (shipIndex[i].ppivot.vz + shipIndex[j].ppivot.vz) >> 1;
                g_collideAttacker = j;
                DamageCollide(DAMAGE_SHIP, i, NULL);
                g_collideAttacker = i;
                DamageCollide(DAMAGE_SHIP, j, NULL);
                if (i == ownShip || j == ownShip) {
                    SetRumble(3, 0);
                    PlayNote(SFX_SHIP_BUMP, 0xFF, 15, 15, 0x100);
                }
                if (frontendOptions_g.numPlayers >= 2 && (i == ownShip2 || j == ownShip2)) {
                    SetRumble(3, 1);
                    PlayNote(SFX_SHIP_BUMP, 0xFF, 15, 15, 0x100);
                }
                if (frontendOptions_g.numPlayers >= 3 && (i == ownShip3 || j == ownShip3)) {
                    SetRumble(3, 2);
                    PlayNote(SFX_SHIP_BUMP, 0xFF, 15, 15, 0x100);
                }
                if (frontendOptions_g.numPlayers == 4 && (i == ownShip4 || j == ownShip4)) {
                    SetRumble(3, 3);
                    PlayNote(SFX_SHIP_BUMP, 0xFF, 15, 15, 0x100);
                }
                sparkPos.vx = shipIndex[i].vpivot.vx >> 6;
                sparkPos.vy = shipIndex[i].vpivot.vy >> 6;
                sparkPos.vz = shipIndex[i].vpivot.vz >> 6;
                if (frontendOptions_g.numPlayers == 1) {
                    AddRandomParticles(particleSlots, 8, &soundPos, &sparkPos);
                }
            }
            shipIndex[i].attr |= SHIP_COLL;
            shipIndex[j].attr |= SHIP_COLL;
        }
    } else {
        shipIndex[i].attr &= ~SHIP_COLL;
        shipIndex[j].attr &= ~SHIP_COLL;
    }
}

/*
 * SN64 GCC 2.7.2 CSEs shipIndex[shipNo1] across the two skeleton-setup pairs
 * and allocates shipNo1 ahead of shipIndex unless the offset is staged into a
 * local and shipIndex carries an extra reference into flow before combine folds it.
 */
static inline ShipData *ShipAtStaged(ShipData *shipIndex, s32 shipNo, s32 weightBase) {
    s32 addr = shipNo << 5;

    addr += shipNo;
    addr <<= 3;
    if (weightBase) {
        addr += ((s32)shipIndex | ((s32)shipIndex & 1));
    } else {
        addr += (s32)shipIndex;
    }
    return (ShipData *)addr;
}

s32 collideLineToPlane(s32 shipNo1, s32 shipNo2, Object **shipShapes, ShipData *shipIndex) {
    s32 i;
    s32 count;
    Poly poly;
    s16 *coords;
    s16 primitiveCount;
    VECTOR p1, p2, p3;
    VECTOR a, b, c, d;
    VECTOR unused; /* unused: retail reserves 0x10-byte stack slot */
    VECTOR factor1;
    VECTOR term1;
    VECTOR intersection;
    s32 alpha;
    s32 factor2;
    VECTOR DC, BC, AC;
    VECTOR BD, AB, AD;
    VECTOR start;
    VECTOR direction;
    VECTOR normal;
    VECTOR P1P2, P1P3;
    VECTOR vec0, vec1, vec2;
    char workspace[40]; /* unused: retail reserves 40-byte stack slot */
    s32 angle;
    s16 high1, low1, low2, high2;
    Object *shape;

    {
        ShipData *ship1 = ShipAtStaged(shipIndex, shipNo1, TRUE);
        ShipData *ship2;

        shape = shipShapes[ship1->highResTableIndex];
        SetSkeletonPosition(shape->skeleton, ship1->ppivot.vx, ship1->ppivot.vy, ship1->ppivot.vz);
        SetSkeletonDirectionHPR(shape->skeleton, ship1->hdg, ship1->pitch, ship1->roll);

        ship2 = ShipAtStaged(shipIndex, shipNo2, FALSE);
        shape = shipShapes[ship2->highResTableIndex];
        SetSkeletonPosition(shape->skeleton, ship2->ppivot.vx, ship2->ppivot.vy, ship2->ppivot.vz);
        SetSkeletonDirectionHPR(shape->skeleton, ship2->hdg, ship2->pitch, ship2->roll);

        high2 = ship2->highResTableIndex;
        low2 = SHIP_COLLISION_SHAPE;
        high1 = shipIndex[shipNo1].highResTableIndex;
        low1 = SHIP_COLLISION_SHAPE;
    }

    getWorldPoint(&a, shipShapes[high2], shipShapes[low2], 0);
    getWorldPoint(&b, shipShapes[high2], shipShapes[low2], 1);
    getWorldPoint(&c, shipShapes[high2], shipShapes[low2], 2);
    getWorldPoint(&d, shipShapes[high2], shipShapes[low2], 3);
    MinusVector(a, c, &AC);
    MinusVector(b, c, &BC);
    MinusVector(d, c, &DC);
    MinusVector(a, b, &AB);
    MinusVector(b, d, &BD);
    MinusVector(a, d, &AD);

    poly.prim = shipShapes[low2]->primitives;
    primitiveCount = shipShapes[low2]->primitiveCount;
    for (i = 0; i < primitiveCount; i++) {
        switch (poly.prim->type) {
        case TypeF3:
            coords = poly.f3->coords;
            getWorldPoint(&p1, shipShapes[high1], shipShapes[low1], coords[0]);
            getWorldPoint(&p2, shipShapes[high1], shipShapes[low1], coords[1]);
            getWorldPoint(&p3, shipShapes[high1], shipShapes[low1], coords[2]);
            poly.f3++;
            break;
        case TypeG3:
            coords = poly.g3->coords;
            getWorldPoint(&p1, shipShapes[high1], shipShapes[low1], coords[0]);
            getWorldPoint(&p2, shipShapes[high1], shipShapes[low1], coords[1]);
            getWorldPoint(&p3, shipShapes[high1], shipShapes[low1], coords[2]);
            poly.g3++;
            break;
        case TypeFT3:
            coords = poly.ft3->coords;
            getWorldPoint(&p1, shipShapes[high1], shipShapes[low1], coords[0]);
            getWorldPoint(&p2, shipShapes[high1], shipShapes[low1], coords[1]);
            getWorldPoint(&p3, shipShapes[high1], shipShapes[low1], coords[2]);
            poly.ft3++;
            break;
        case TypeGT3:
            coords = poly.gt3->coords;
            getWorldPoint(&p1, shipShapes[high1], shipShapes[low1], coords[0]);
            getWorldPoint(&p2, shipShapes[high1], shipShapes[low1], coords[1]);
            getWorldPoint(&p3, shipShapes[high1], shipShapes[low1], coords[2]);
            poly.gt3++;
            break;
        default:
            __assert("0", "collide.c", 381);
            break;
        }
        MinusVector(p1, p2, &P1P2);
        MinusVector(p1, p3, &P1P3);
        CrossProduct(&normal, P1P2, P1P3);
        for (count = 0; count < 6; count++) {
            switch (count) {
            case 0:
                assign(&start, b);
                assign(&direction, BC);
                break;
            case 1:
                assign(&start, a);
                assign(&direction, AC);
                break;
            case 2:
                assign(&start, d);
                assign(&direction, DC);
                break;
            case 3:
                assign(&start, a);
                assign(&direction, AB);
                break;
            case 4:
                assign(&start, a);
                assign(&direction, AD);
                break;
            case 5:
                assign(&start, b);
                assign(&direction, BD);
                break;
            }
            MinusVector(start, p1, &factor1);
            alpha = ScalarProduct(factor1, normal) << 12;
            factor2 = ScalarProduct(direction, normal);
            if (factor2 != 0) {
                alpha /= factor2;
                term1.vx = (alpha * direction.vx) >> 12;
                term1.vy = (alpha * direction.vy) >> 12;
                term1.vz = (alpha * direction.vz) >> 12;
                intersection.vx = start.vx + term1.vx;
                intersection.vy = start.vy + term1.vy;
                intersection.vz = start.vz + term1.vz;
                if (alpha >= 0 && alpha <= 4096) {
                    vec0.vx = intersection.vx - p1.vx;
                    vec0.vy = intersection.vy - p1.vy;
                    vec0.vz = intersection.vz - p1.vz;
                    vec1.vx = intersection.vx - p2.vx;
                    vec1.vy = intersection.vy - p2.vy;
                    vec1.vz = intersection.vz - p2.vz;
                    vec2.vx = intersection.vx - p3.vx;
                    vec2.vy = intersection.vy - p3.vy;
                    vec2.vz = intersection.vz - p3.vz;
                    angle = GetAng(vec0, vec1);
                    angle += GetAng(vec1, vec2);
                    angle += GetAng(vec2, vec0);
                    if (angle >= 30000) {
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

void Ship2TrackBase(ShipData *playerShip, Face *facePtr, s32 alpha) {
    u8 unused[0x38]; /* unused: retail reserves 0x38 bytes of stack locals */

    if (collideMinAlpha > alpha) {
        collideMinAlpha = alpha;
    }
    if (alpha <= 0) {
        if (playerShip == &ships[ownShip]) {
            SetRumble(3, 0);
            PlayNote(SFX_WALL_SCRAPE, 0xFF, 15, 15, 0x100);
        } else if (frontendOptions_g.numPlayers >= 2 && playerShip == &ships[ownShip2]) {
            SetRumble(3, 1);
            PlayNote(SFX_WALL_SCRAPE, 0xFF, 15, 15, 0x100);
        } else if (frontendOptions_g.numPlayers >= 3 && playerShip == &ships[ownShip3]) {
            SetRumble(3, 2);
            PlayNote(SFX_WALL_SCRAPE, 0xFF, 15, 15, 0x100);
        } else if (frontendOptions_g.numPlayers == 4 && playerShip == &ships[ownShip4]) {
            SetRumble(3, 3);
            PlayNote(SFX_WALL_SCRAPE, 0xFF, 15, 15, 0x100);
        }
        playerShip->vpivot.vx -= playerShip->vpivot.vx >> 3;
        playerShip->vpivot.vy -= playerShip->vpivot.vy >> 3;
        playerShip->vpivot.vz -= playerShip->vpivot.vz >> 3;
        playerShip->vpivot.vx += PAL_SPEED(facePtr->normal.vx);
        playerShip->vpivot.vy += PAL_SPEED(facePtr->normal.vy);
        playerShip->vpivot.vz += PAL_SPEED(facePtr->normal.vz);
        playerShip->vpivot.vx -= PAL_SPEED((facePtr->normal.vx * alpha) >> 4);
        playerShip->vpivot.vy -= PAL_SPEED((facePtr->normal.vy * alpha) >> 4);
        playerShip->vpivot.vz -= PAL_SPEED((facePtr->normal.vz * alpha) >> 4);
    } else if (alpha <= 30) {
        playerShip->vpivot.vx += PAL_SPEED(facePtr->normal.vx);
        playerShip->vpivot.vy += PAL_SPEED(facePtr->normal.vy);
        playerShip->vpivot.vz += PAL_SPEED(facePtr->normal.vz);
    }
}

void DroneShip2TrackBase(ShipData *playerShip, Face *facePtr, s32 alpha) {
    u8 unused[0x38]; /* unused: retail reserves 0x38 bytes of stack locals */

    if (collideMinAlpha > alpha) {
        collideMinAlpha = alpha;
    }
    if (alpha <= 0) {
        playerShip->ppivot.vx += PAL_SPEED(facePtr->normal.vx >> 3);
        playerShip->ppivot.vy += PAL_SPEED(facePtr->normal.vy >> 3);
        playerShip->ppivot.vz += PAL_SPEED(facePtr->normal.vz >> 3);
    } else if (alpha <= 30) {
        playerShip->vpivot.vx += PAL_SPEED(facePtr->normal.vx >> 3);
        playerShip->vpivot.vy += PAL_SPEED(facePtr->normal.vy >> 3);
        playerShip->vpivot.vz += PAL_SPEED(facePtr->normal.vz >> 3);
    }
}

void Ship2Track(ShipData *playerShip, Face *facePtr, Object **shipShapes) {
    VECTOR nosePos;
    VECTOR wingPos;
    VECTOR toTrack;
    VECTOR vertex0;
    VECTOR vertex1;
    VECTOR toFaceVector;
    VECTOR unused; /* unused: retail reserves 0x10-byte stack slot */
    VECTOR facePoint;
    s32 j;
    s32 alpha;
    Face *facePtr2;
    s32 downTrack;
    s16 collided = 0;
    s16 faceIndex = 0;
    s16 wingShift = 4;
    TrackSection *nextSection;

    nosePos.vx = playerShip->ppivot.vx + (playerShip->unitVecNose.vx >> 3);
    nosePos.vy = playerShip->ppivot.vy + (playerShip->unitVecNose.vy >> 3);
    nosePos.vz = playerShip->ppivot.vz + (playerShip->unitVecNose.vz >> 3);
    wingPos.vx = playerShip->ppivot.vx - (playerShip->unitVecWing.vx >> wingShift) - (playerShip->unitVecNose.vx >> wingShift);
    wingPos.vy = playerShip->ppivot.vy - (playerShip->unitVecWing.vy >> wingShift) - (playerShip->unitVecNose.vy >> wingShift);
    wingPos.vz = playerShip->ppivot.vz - (playerShip->unitVecWing.vz >> wingShift) - (playerShip->unitVecNose.vz >> wingShift);

    nextSection = playerShip->nearTrkSect->nextSection.ptr;
    toTrack.vx = nextSection->centre.vx - playerShip->nearTrkSect->centre.vx;
    toTrack.vy = nextSection->centre.vy - playerShip->nearTrkSect->centre.vy;
    toTrack.vz = nextSection->centre.vz - playerShip->nearTrkSect->centre.vz;
    downTrack = toTrack.vx * playerShip->unitVecNose.vx + toTrack.vy * playerShip->unitVecNose.vy +
                toTrack.vz * playerShip->unitVecNose.vz;
    if (downTrack < 0) {
        playerShip->attr &= ~DIRECTION;
    } else {
        playerShip->attr |= DIRECTION;
    }

    GetFaceVertex(playerShip->nearTrkSect, facePtr, 0, &vertex0);
    GetFaceVertex(playerShip->nearTrkSect, facePtr, 1, &vertex1);
    toFaceVector.vx = vertex0.vx - vertex1.vx;
    toFaceVector.vy = vertex0.vy - vertex1.vy;
    toFaceVector.vz = vertex0.vz - vertex1.vz;
    toTrack.vx = playerShip->nearTrkSect->centre.vx - playerShip->ppivot.vx;
    toTrack.vy = playerShip->nearTrkSect->centre.vy - playerShip->ppivot.vy;
    toTrack.vz = playerShip->nearTrkSect->centre.vz - playerShip->ppivot.vz;
    if (toTrack.vx * toFaceVector.vx + toTrack.vy * toFaceVector.vy + toTrack.vz * toFaceVector.vz > 0) {
        facePtr = track->faces + playerShip->nearTrkSect->faceStart;
        for (j = 0; j < playerShip->nearTrkSect->faceCount; j++) {
            if (facePtr->flags & TRACK_BASE) {
                break;
            }
            playerShip->attr |= WALL_COLL;
            if (facePtr == track->faces + playerShip->nearTrkSect->faceStart ||
                playerShip->nearTrkSect->junction.ptr != NULL) {
                GetFaceVertex(playerShip->nearTrkSect, facePtr, 0, &facePoint);
                alpha = PlaneLineCollision(facePoint, nosePos, facePtr->normal);
                if (alpha <= 0) {
                    if (playerShip->nearTrkSect->flags & JunctionStart) {
                        if (CheckPolygonCollision(facePtr, nosePos, alpha, 1, playerShip->nearTrkSect) == 1) {
                            NoseCollision(playerShip, facePtr, -downTrack, alpha);
                            collided = 1;
                        } else {
                            facePtr2 = track->faces + playerShip->nearTrkSect->nextSection.ptr->faceStart;
                            if (CheckPolygonCollision(facePtr2, nosePos, alpha, 1,
                                                      playerShip->nearTrkSect->nextSection.ptr) == 1) {
                                NoseCollision(playerShip, facePtr, -downTrack, alpha);
                                collided = 1;
                            }
                        }
                    } else if (playerShip->nearTrkSect->prevSection.ptr->junction.index != -1 &&
                               (playerShip->nearTrkSect->prevSection.ptr->junction.ptr->flags & JunctionStart)) {
                        if (CheckPolygonCollision(facePtr, nosePos, alpha, 1, playerShip->nearTrkSect) == 1) {
                            NoseCollision(playerShip, facePtr, -downTrack, alpha);
                            collided = 1;
                        } else {
                            facePtr2 = track->faces + playerShip->nearTrkSect->nextSection.ptr->faceStart;
                            if (CheckPolygonCollision(facePtr2, nosePos, alpha, 1,
                                                      playerShip->nearTrkSect->nextSection.ptr) == 1) {
                                NoseCollision(playerShip, facePtr2, -downTrack, alpha);
                                collided = 1;
                            } else {
                                facePtr2 = track->faces + playerShip->nearTrkSect->prevSection.ptr->faceStart;
                                if (CheckPolygonCollision(facePtr2, nosePos, alpha, 1,
                                                          playerShip->nearTrkSect->prevSection.ptr) == 1) {
                                    NoseCollision(playerShip, facePtr2, -downTrack, alpha);
                                    collided = 1;
                                }
                            }
                        }
                    } else if (playerShip->nearTrkSect->nextSection.ptr->junction.index != -1 &&
                               (playerShip->nearTrkSect->nextSection.ptr->junction.ptr->flags & JunctionStart)) {
                        if (CheckPolygonCollision(facePtr, nosePos, alpha, 1, playerShip->nearTrkSect) == 1) {
                            NoseCollision(playerShip, facePtr, -downTrack, alpha);
                            collided = 1;
                        } else {
                            facePtr2 = track->faces + playerShip->nearTrkSect->prevSection.ptr->faceStart;
                            if (CheckPolygonCollision(facePtr2, nosePos, alpha, 1,
                                                      playerShip->nearTrkSect->prevSection.ptr) == 1) {
                                NoseCollision(playerShip, facePtr2, -downTrack, alpha);
                                collided = 1;
                            } else {
                                facePtr2 = track->faces +
                                           playerShip->nearTrkSect->nextSection.ptr->junction.ptr->faceStart;
                                /* Retail passes nextSection.ptr here instead of nextSection.ptr->junction.ptr. */
                                if (CheckPolygonCollision(facePtr2, nosePos, alpha, 1,
                                                          playerShip->nearTrkSect->nextSection.ptr) == 1) {
                                    NoseCollision(playerShip, facePtr2, -downTrack, alpha);
                                    collided = 1;
                                }
                            }
                        }
                    } else if (playerShip->nearTrkSect->flags & JunctionEnd) {
                        if (CheckPolygonCollision(facePtr, nosePos, alpha, 1, playerShip->nearTrkSect) == 1) {
                            NoseCollision(playerShip, facePtr, -downTrack, alpha);
                            collided = 1;
                        } else {
                            facePtr2 = track->faces + playerShip->nearTrkSect->prevSection.ptr->faceStart;
                            if (CheckPolygonCollision(facePtr2, nosePos, alpha, 1,
                                                      playerShip->nearTrkSect->prevSection.ptr) == 1) {
                                NoseCollision(playerShip, facePtr, -downTrack, alpha);
                                collided = 1;
                            }
                        }
                    } else if (playerShip->nearTrkSect->nextSection.ptr->junction.index != -1 &&
                               (playerShip->nearTrkSect->nextSection.ptr->junction.ptr->flags & JunctionEnd)) {
                        if (CheckPolygonCollision(facePtr, nosePos, alpha, 1, playerShip->nearTrkSect) == 1) {
                            NoseCollision(playerShip, facePtr, -downTrack, alpha);
                            collided = 1;
                        } else {
                            facePtr2 = track->faces + playerShip->nearTrkSect->prevSection.ptr->faceStart;
                            if (CheckPolygonCollision(facePtr2, nosePos, alpha, 1,
                                                      playerShip->nearTrkSect->prevSection.ptr) == 1) {
                                NoseCollision(playerShip, facePtr2, -downTrack, alpha);
                                collided = 1;
                            }
                        }
                    } else {
                        NoseCollision(playerShip, facePtr, -downTrack, alpha);
                        collided = 1;
                    }
                }
                wingPos.vx = playerShip->ppivot.vx - (playerShip->unitVecWing.vx >> wingShift) - (playerShip->unitVecNose.vx >> wingShift);
                wingPos.vy = playerShip->ppivot.vy - (playerShip->unitVecWing.vy >> wingShift) - (playerShip->unitVecNose.vy >> wingShift);
                wingPos.vz = playerShip->ppivot.vz - (playerShip->unitVecWing.vz >> wingShift) - (playerShip->unitVecNose.vz >> wingShift);
                alpha = PlaneLineCollision(facePoint, wingPos, facePtr->normal);
                if (alpha <= 0) {
                    if ((playerShip->nearTrkSect->flags & JunctionStart) ||
                        (playerShip->nearTrkSect->flags & JunctionEnd)) {
                        if (CheckPolygonCollision(facePtr, wingPos, alpha, 1, playerShip->nearTrkSect) == 1) {
                            NewWingCollision(playerShip, facePtr, -downTrack, alpha);
                            collided = 1;
                        }
                    } else if (collided) {
                        WingCollision(playerShip, facePtr, -downTrack, alpha);
                    } else {
                        NewWingCollision(playerShip, facePtr, -downTrack, alpha);
                    }
                }
                wingPos.vx = playerShip->ppivot.vx + (playerShip->unitVecWing.vx >> wingShift) - (playerShip->unitVecNose.vx >> wingShift);
                wingPos.vy = playerShip->ppivot.vy + (playerShip->unitVecWing.vy >> wingShift) - (playerShip->unitVecNose.vy >> wingShift);
                wingPos.vz = playerShip->ppivot.vz + (playerShip->unitVecWing.vz >> wingShift) - (playerShip->unitVecNose.vz >> wingShift);
                alpha = PlaneLineCollision(facePoint, wingPos, facePtr->normal);
                if (alpha <= 0) {
                    if ((playerShip->nearTrkSect->flags & JunctionStart) ||
                        (playerShip->nearTrkSect->flags & JunctionEnd)) {
                        if (CheckPolygonCollision(facePtr, wingPos, alpha, 1, playerShip->nearTrkSect) == 1) {
                            NewWingCollision(playerShip, facePtr, -downTrack, alpha);
                            collided = 1;
                        }
                    } else if (collided) {
                        WingCollision(playerShip, facePtr, -downTrack, alpha);
                    } else {
                        NewWingCollision(playerShip, facePtr, -downTrack, alpha);
                    }
                }
            }
            facePtr++;
            if (facePtr->flags & TRACK_BASE) {
                break;
            }
        }
    } else {
        playerShip->attr &= ~WALL_COLL;
        facePtr = track->faces + playerShip->nearTrkSect->faceStart;
        while (!(facePtr->flags & TRACK_BASE)) {
            facePtr++;
            faceIndex++;
        }
        while (facePtr->flags & TRACK_BASE) {
            facePtr++;
            faceIndex++;
        }
        for (j = faceIndex; j < playerShip->nearTrkSect->faceCount; j++) {
            if (playerShip->nearTrkSect->faceCount > 3 || playerShip->nearTrkSect->junction.ptr != NULL) {
                GetFaceVertex(playerShip->nearTrkSect, facePtr, 0, &facePoint);
                alpha = PlaneLineCollision(facePoint, nosePos, facePtr->normal);
                if (alpha <= 0) {
                    if (playerShip->nearTrkSect->flags & JunctionStart) {
                        if (CheckPolygonCollision(facePtr, nosePos, alpha, 1, playerShip->nearTrkSect) == 1) {
                            NoseCollision(playerShip, facePtr, downTrack, alpha);
                            collided = 1;
                        } else {
                            facePtr2 = track->faces + playerShip->nearTrkSect->nextSection.ptr->faceStart;
                            facePtr2 += 3;
                            if (CheckPolygonCollision(facePtr2, nosePos, alpha, 1,
                                                      playerShip->nearTrkSect->nextSection.ptr) == 1) {
                                NoseCollision(playerShip, facePtr, -downTrack, alpha);
                                collided = 1;
                            }
                        }
                    } else if (playerShip->nearTrkSect->prevSection.ptr->junction.index != -1 &&
                               (playerShip->nearTrkSect->prevSection.ptr->junction.ptr->flags & JunctionStart)) {
                        if (CheckPolygonCollision(facePtr, nosePos, alpha, 1, playerShip->nearTrkSect) == 1) {
                            NoseCollision(playerShip, facePtr, downTrack, alpha);
                            collided = 1;
                        } else {
                            facePtr2 = track->faces + playerShip->nearTrkSect->prevSection.ptr->faceStart;
                            facePtr2 += 3;
                            if (CheckPolygonCollision(facePtr2, nosePos, alpha, 1,
                                                      playerShip->nearTrkSect->prevSection.ptr) == 1) {
                                NoseCollision(playerShip, facePtr2, -downTrack, alpha);
                                collided = 1;
                            } else {
                                facePtr2 = track->faces + playerShip->nearTrkSect->nextSection.ptr->faceStart;
                                facePtr2 += 3;
                                if (CheckPolygonCollision(facePtr2, nosePos, alpha, 1,
                                                          playerShip->nearTrkSect->nextSection.ptr) == 1) {
                                    NoseCollision(playerShip, facePtr2, -downTrack, alpha);
                                    collided = 1;
                                }
                            }
                        }
                    } else if (playerShip->nearTrkSect->nextSection.ptr->junction.index != -1 &&
                               (playerShip->nearTrkSect->nextSection.ptr->junction.ptr->flags & JunctionStart)) {
                        if (CheckPolygonCollision(facePtr, nosePos, alpha, 1, playerShip->nearTrkSect) == 1) {
                            NoseCollision(playerShip, facePtr, downTrack, alpha);
                            collided = 1;
                        } else {
                            facePtr2 = track->faces + playerShip->nearTrkSect->prevSection.ptr->faceStart;
                            facePtr2 += 3;
                            if (CheckPolygonCollision(facePtr2, nosePos, alpha, 1,
                                                      playerShip->nearTrkSect->prevSection.ptr) == 1) {
                                NoseCollision(playerShip, facePtr2, -downTrack, alpha);
                                collided = 1;
                            } else {
                                facePtr2 = track->faces +
                                           playerShip->nearTrkSect->nextSection.ptr->junction.ptr->faceStart;
                                facePtr2 += 3;
                                if (CheckPolygonCollision(facePtr2, nosePos, alpha, 1,
                                                          playerShip->nearTrkSect->nextSection.ptr->junction.ptr) == 1) {
                                    NoseCollision(playerShip, facePtr2, -downTrack, alpha);
                                    collided = 1;
                                }
                            }
                        }
                    } else if (playerShip->nearTrkSect->flags & JunctionEnd) {
                        if (CheckPolygonCollision(facePtr, nosePos, alpha, 1, playerShip->nearTrkSect) == 1) {
                            NoseCollision(playerShip, facePtr, -downTrack, alpha);
                            collided = 1;
                        } else {
                            facePtr2 = track->faces + playerShip->nearTrkSect->prevSection.ptr->faceStart;
                            facePtr2 += 3;
                            if (CheckPolygonCollision(facePtr2, nosePos, alpha, 1,
                                                      playerShip->nearTrkSect->prevSection.ptr) == 1) {
                                NoseCollision(playerShip, facePtr2, -downTrack, alpha);
                                collided = 1;
                            }
                        }
                    } else if (playerShip->nearTrkSect->nextSection.ptr->junction.index != -1 &&
                               (playerShip->nearTrkSect->nextSection.ptr->junction.ptr->flags & JunctionEnd)) {
                        if (CheckPolygonCollision(facePtr, nosePos, alpha, 1, playerShip->nearTrkSect) == 1) {
                            NoseCollision(playerShip, facePtr, -downTrack, alpha);
                            collided = 1;
                        } else {
                            facePtr2 = track->faces + playerShip->nearTrkSect->prevSection.ptr->faceStart;
                            facePtr2 += 3;
                            if (CheckPolygonCollision(facePtr2, nosePos, alpha, 1,
                                                      playerShip->nearTrkSect->prevSection.ptr) == 1) {
                                NoseCollision(playerShip, facePtr2, -downTrack, alpha);
                                collided = 1;
                            }
                        }
                    } else {
                        NoseCollision(playerShip, facePtr, downTrack, alpha);
                        collided = 1;
                    }
                }
                wingPos.vx = playerShip->ppivot.vx - (playerShip->unitVecWing.vx >> wingShift) - (playerShip->unitVecNose.vx >> wingShift);
                wingPos.vy = playerShip->ppivot.vy - (playerShip->unitVecWing.vy >> wingShift) - (playerShip->unitVecNose.vy >> wingShift);
                wingPos.vz = playerShip->ppivot.vz - (playerShip->unitVecWing.vz >> wingShift) - (playerShip->unitVecNose.vz >> wingShift);
                alpha = PlaneLineCollision(facePoint, wingPos, facePtr->normal);
                if (alpha <= 0) {
                    if ((playerShip->nearTrkSect->flags & JunctionStart) ||
                        (playerShip->nearTrkSect->flags & JunctionEnd)) {
                        if (CheckPolygonCollision(facePtr, wingPos, alpha, 1, playerShip->nearTrkSect) == 1) {
                            NewWingCollision(playerShip, facePtr, downTrack, alpha);
                            collided = 1;
                        }
                    } else if (collided) {
                        WingCollision(playerShip, facePtr, downTrack, alpha);
                    } else {
                        NewWingCollision(playerShip, facePtr, downTrack, alpha);
                    }
                }
                wingPos.vx = playerShip->ppivot.vx + (playerShip->unitVecWing.vx >> wingShift) - (playerShip->unitVecNose.vx >> wingShift);
                wingPos.vy = playerShip->ppivot.vy + (playerShip->unitVecWing.vy >> wingShift) - (playerShip->unitVecNose.vy >> wingShift);
                wingPos.vz = playerShip->ppivot.vz + (playerShip->unitVecWing.vz >> wingShift) - (playerShip->unitVecNose.vz >> wingShift);
                alpha = PlaneLineCollision(facePoint, wingPos, facePtr->normal);
                if (alpha <= 0) {
                    if ((playerShip->nearTrkSect->flags & JunctionStart) ||
                        (playerShip->nearTrkSect->flags & JunctionEnd)) {
                        if (CheckPolygonCollision(facePtr, wingPos, alpha, 1, playerShip->nearTrkSect) == 1) {
                            NewWingCollision(playerShip, facePtr, downTrack, alpha);
                            collided = 1;
                        }
                    } else if (collided) {
                        WingCollision(playerShip, facePtr, downTrack, alpha);
                    } else {
                        NewWingCollision(playerShip, facePtr, downTrack, alpha);
                    }
                }
            }
            facePtr++;
        }
    }
    if (beamCollide) {
        BeamCollideReaction(playerShip);
    }
}

void BeamCollideReaction(ShipData *playerShip) {
    VECTOR reaction;
    SVector normal;
    s32 normalX, normalY, normalZ;
    Face *facePtr;

    reaction.vx = playerShip->vpivot.vx >> 2;
    reaction.vy = playerShip->vpivot.vy >> 2;
    reaction.vz = playerShip->vpivot.vz >> 2;
    playerShip->vpivot.vx -= reaction.vx * 3;
    playerShip->vpivot.vy -= reaction.vy * 3;
    playerShip->vpivot.vz -= reaction.vz * 3;
    playerShip->ppivot.vx -= playerShip->vpivot.vx >> 6;
    playerShip->ppivot.vy -= playerShip->vpivot.vy >> 6;
    playerShip->ppivot.vz -= playerShip->vpivot.vz >> 6;
    facePtr = track->faces + playerShip->nearTrkSect->faceStart;
    while (!(facePtr++->flags & TRACK_BASE)) {
    }
    normalX = facePtr->normal.vx * 2;
    normal.vx = normalX >> 1;
    normalY = facePtr->normal.vy * 2;
    normal.vy = normalY >> 1;
    normalZ = facePtr->normal.vz * 2;
    normal.vz = normalZ >> 1;
    playerShip->ppivot.vx += normal.vx;
    playerShip->ppivot.vy += normal.vy;
    playerShip->ppivot.vz += normal.vz;
}

void NewWingCollision(ShipData *playerShip, Face *facePtr, s32 direction, s32 alpha) {
    u8 unused[0x38]; /* unused: retail reserves 0x38 bytes of stack locals */
    VECTOR reaction;
    VECTOR wingPos;

    if (playerShip == NULL) {
        __assert("playerShip", "collide.c", 1316);
    }
    if (facePtr == NULL) {
        __assert("facePtr", "collide.c", 1317);
    }
    playerShip->vpivot.vx += facePtr->normal.vx;
    playerShip->vpivot.vy += facePtr->normal.vy >> 1;
    playerShip->vpivot.vz += facePtr->normal.vz;
    if (direction > 0) {
        wingPos.vx = playerShip->ppivot.vx + (playerShip->unitVecWing.vx >> 4) - (playerShip->unitVecNose.vx >> 4);
        wingPos.vy = playerShip->ppivot.vy + (playerShip->unitVecWing.vy >> 4) - (playerShip->unitVecNose.vy >> 4);
        wingPos.vz = playerShip->ppivot.vz + (playerShip->unitVecWing.vz >> 4) - (playerShip->unitVecNose.vz >> 4);
    } else {
        wingPos.vx = playerShip->ppivot.vx - (playerShip->unitVecWing.vx >> 4) - (playerShip->unitVecNose.vx >> 4);
        wingPos.vy = playerShip->ppivot.vy - (playerShip->unitVecWing.vy >> 4) - (playerShip->unitVecNose.vy >> 4);
        wingPos.vz = playerShip->ppivot.vz - (playerShip->unitVecWing.vz >> 4) - (playerShip->unitVecNose.vz >> 4);
    }
    if (playerShip == &ships[ownShip] || playerShip == &ships[ownShip2] || playerShip == &ships[ownShip3] ||
        playerShip == &ships[ownShip4]) {
        if (g_scrapeVol < 200) {
            g_scrapeVol += 100;
        }
        if (g_scrapeVol > 200) {
            g_scrapeVol = 200;
        }
    }
    reaction.vx = playerShip->vpivot.vx >> 6;
    reaction.vy = playerShip->vpivot.vy >> 6;
    reaction.vz = playerShip->vpivot.vz >> 6;
    if (frontendOptions_g.numPlayers == 1) {
        AddScrapingParticles(particleSlots, 2, &wingPos, &reaction);
        AddRandomParticles(particleSlots, 2, &wingPos, &reaction);
    }
    if (playerShip == &ships[ownShip]) {
        SetRumble(3, 0);
    }
    if (frontendOptions_g.numPlayers >= 2 && playerShip == &ships[ownShip2]) {
        SetRumble(3, 1);
    }
    if (frontendOptions_g.numPlayers >= 3 && playerShip == &ships[ownShip3]) {
        SetRumble(3, 2);
    }
    if (frontendOptions_g.numPlayers == 4 && playerShip == &ships[ownShip4]) {
        SetRumble(3, 3);
    }
}

void WingCollision(ShipData *playerShip, Face *facePtr, s32 direction, s32 alpha) {
    VECTOR unused0; /* unused: retail reserves 0x10-byte stack slot */
    VECTOR bounce;
    u8 unused1[0x18]; /* unused: retail reserves 0x18 bytes of stack locals */
    VECTOR reaction;
    VECTOR wingPos;
    s32 magnitude;

    if (alpha < 0) {
        bounce.vx = playerShip->vpivot.vx / abs(alpha);
        bounce.vy = playerShip->vpivot.vy / abs(alpha);
        bounce.vz = playerShip->vpivot.vz / abs(alpha);
        playerShip->vpivot.vx = 0;
        playerShip->vpivot.vy = 0;
        playerShip->vpivot.vz = 0;
        playerShip->vpivot.vx += bounce.vx >> 5;
        playerShip->vpivot.vy += bounce.vy >> 5;
        playerShip->vpivot.vz += bounce.vz >> 5;
    }
    playerShip->vpivot.vx += facePtr->normal.vx * 2;
    playerShip->vpivot.vy += facePtr->normal.vy >> 1;
    playerShip->vpivot.vz += facePtr->normal.vz * 2;
    playerShip->ppivot.vx += playerShip->vpivot.vx >> 6;
    playerShip->ppivot.vy += playerShip->vpivot.vy >> 6;
    playerShip->ppivot.vz += playerShip->vpivot.vz >> 6;
    magnitude = (playerShip->speed >> 5) + 200;
    if (direction > 0) {
        playerShip->vhdg += magnitude;
    } else {
        playerShip->vhdg -= magnitude;
    }
    if (direction > 0) {
        wingPos.vx = playerShip->ppivot.vx + (playerShip->unitVecWing.vx >> 4) - (playerShip->unitVecNose.vx >> 4);
        wingPos.vy = playerShip->ppivot.vy + (playerShip->unitVecWing.vy >> 4) - (playerShip->unitVecNose.vy >> 4);
        wingPos.vz = playerShip->ppivot.vz + (playerShip->unitVecWing.vz >> 4) - (playerShip->unitVecNose.vz >> 4);
    } else {
        wingPos.vx = playerShip->ppivot.vx - (playerShip->unitVecWing.vx >> 4) - (playerShip->unitVecNose.vx >> 4);
        wingPos.vy = playerShip->ppivot.vy - (playerShip->unitVecWing.vy >> 4) - (playerShip->unitVecNose.vy >> 4);
        wingPos.vz = playerShip->ppivot.vz - (playerShip->unitVecWing.vz >> 4) - (playerShip->unitVecNose.vz >> 4);
    }
    /* As in XL, the damage always goes to the first player's ship. */
    if (playerShip == &ships[ownShip]) {
        SetRumble(3, 0);
        PlayNote(SFX_WALL_SCRAPE, 0xFF, 15, 15, 0x100);
        DamageCollide(DAMAGE_WALL, ownShip, NULL);
    } else if (frontendOptions_g.numPlayers >= 2 && playerShip == &ships[ownShip2]) {
        SetRumble(3, 1);
        PlayNote(SFX_WALL_SCRAPE, 0xFF, 15, 15, 0x100);
        DamageCollide(DAMAGE_WALL, ownShip, NULL);
    } else if (frontendOptions_g.numPlayers >= 3 && playerShip == &ships[ownShip3]) {
        SetRumble(3, 2);
        PlayNote(SFX_WALL_SCRAPE, 0xFF, 15, 15, 0x100);
        DamageCollide(DAMAGE_WALL, ownShip, NULL);
    } else if (frontendOptions_g.numPlayers == 4 && playerShip == &ships[ownShip4]) {
        SetRumble(3, 3);
        PlayNote(SFX_WALL_SCRAPE, 0xFF, 15, 15, 0x100);
        DamageCollide(DAMAGE_WALL, ownShip, NULL);
    }
    reaction.vx = playerShip->vpivot.vx >> 6;
    reaction.vy = playerShip->vpivot.vy >> 6;
    reaction.vz = playerShip->vpivot.vz >> 6;
    if (frontendOptions_g.numPlayers == 1) {
        AddRandomParticles(particleSlots, 8, &wingPos, &reaction);
    }
}

void NoseCollision(ShipData *playerShip, Face *facePtr, s32 direction, s32 alpha) {
    VECTOR unused0; /* unused: retail reserves 0x10-byte stack slot */
    VECTOR bounce;
    u8 unused1[0x08]; /* unused: retail reserves 8 bytes of stack locals */
    VECTOR nosePos;
    VECTOR reaction;
    s32 magnitude;
    s32 i;

    if (alpha < 0) {
        bounce.vx = playerShip->vpivot.vx / abs(alpha);
        bounce.vy = playerShip->vpivot.vy / abs(alpha);
        bounce.vz = playerShip->vpivot.vz / abs(alpha);
        playerShip->vpivot.vx = 0;
        playerShip->vpivot.vy = 0;
        playerShip->vpivot.vz = 0;
        playerShip->vpivot.vx += bounce.vx >> 5;
        playerShip->vpivot.vy += bounce.vy >> 5;
        playerShip->vpivot.vz += bounce.vz >> 5;
    }
    playerShip->vpivot.vx += facePtr->normal.vx * 2;
    playerShip->vpivot.vy += facePtr->normal.vy >> 1;
    playerShip->vpivot.vz += facePtr->normal.vz * 2;
    playerShip->ppivot.vx += playerShip->vpivot.vx >> 6;
    playerShip->ppivot.vy += playerShip->vpivot.vy >> 6;
    playerShip->ppivot.vz += playerShip->vpivot.vz >> 6;
    magnitude = (playerShip->speed >> 2) + 400;
    if (direction > 0) {
        playerShip->vhdg += magnitude;
    } else {
        playerShip->vhdg -= magnitude;
    }
    nosePos.vx = playerShip->ppivot.vx + (playerShip->unitVecNose.vx >> 3);
    nosePos.vy = playerShip->ppivot.vy + (playerShip->unitVecNose.vy >> 3);
    nosePos.vz = playerShip->ppivot.vz + (playerShip->unitVecNose.vz >> 3);
    if (playerShip == &ships[ownShip]) {
        SetRumble(3, 0);
        PlayNote(SFX_WALL_SCRAPE, 0xFF, 15, 15, 0x100);
    } else if (frontendOptions_g.numPlayers >= 2 && playerShip == &ships[ownShip2]) {
        SetRumble(3, 1);
        PlayNote(SFX_WALL_SCRAPE, 0xFF, 15, 15, 0x100);
    } else if (frontendOptions_g.numPlayers >= 3 && playerShip == &ships[ownShip3]) {
        SetRumble(3, 2);
        PlayNote(SFX_WALL_SCRAPE, 0xFF, 15, 15, 0x100);
    } else if (frontendOptions_g.numPlayers == 4 && playerShip == &ships[ownShip4]) {
        SetRumble(3, 3);
        PlayNote(SFX_WALL_SCRAPE, 0xFF, 15, 15, 0x100);
    }
    if (!(playerShip->combatAttr & SHIP_SHIELDED)) {
        for (i = 0; i < frontendOptions_g.numPlayers; i++) {
            if (&ships[frontendOptions_g.ownShips[i]] == playerShip) {
                DamageCollide(DAMAGE_WALL, frontendOptions_g.ownShips[i], NULL);
                DamageCollide(DAMAGE_WALL, frontendOptions_g.ownShips[i], NULL);
                break;
            }
        }
    }
    reaction.vx = playerShip->vpivot.vx >> 6;
    reaction.vy = playerShip->vpivot.vy >> 6;
    reaction.vz = playerShip->vpivot.vz >> 6;
    if (frontendOptions_g.numPlayers == 1) {
        AddRandomParticles(particleSlots, 8, &nosePos, &reaction);
    }
}

s32 PlaneLineCollision(VECTOR planePoint, VECTOR shipPos, SVector normal) {
    s32 dotProduct;
    s32 normDotProduct;
    s32 collData;

    dotProduct = (planePoint.vx - shipPos.vx) * normal.vx;
    dotProduct += (planePoint.vy - shipPos.vy) * normal.vy;
    dotProduct += (planePoint.vz - shipPos.vz) * normal.vz;
    normDotProduct = ((-normal.vx * normal.vx) >> 12) + ((-normal.vy * normal.vy) >> 12) +
                     ((-normal.vz * normal.vz) >> 12);
    if (normDotProduct != 0) {
        collData = dotProduct / normDotProduct;
    } else {
        collData = 0;
    }
    return collData;
}

s32 CheckPolygonCollision(Face *facePtr, VECTOR pos, s32 alpha, s32 side, TrackSection *section) {
    VECTOR planePoint;
    VECTOR vertex;
    VECTOR vec0, vec1, vec2, vec3;
    s32 angle;

    planePoint.vx = pos.vx - ((facePtr->normal.vx * alpha) >> 12);
    planePoint.vy = pos.vy - ((facePtr->normal.vy * alpha) >> 12);
    planePoint.vz = pos.vz - ((facePtr->normal.vz * alpha) >> 12);
    GetFaceVertex(section, facePtr, 1, &vertex);
    vec0.vx = planePoint.vx - vertex.vx;
    vec0.vy = planePoint.vy - vertex.vy;
    vec0.vz = planePoint.vz - vertex.vz;
    GetFaceVertex(section, facePtr, 2, &vertex);
    vec1.vx = planePoint.vx - vertex.vx;
    vec1.vy = planePoint.vy - vertex.vy;
    vec1.vz = planePoint.vz - vertex.vz;
    if (side != 1) {
        facePtr++;
    }
    GetFaceVertex(section, facePtr, 0, &vertex);
    vec2.vx = planePoint.vx - vertex.vx;
    vec2.vy = planePoint.vy - vertex.vy;
    vec2.vz = planePoint.vz - vertex.vz;
    GetFaceVertex(section, facePtr, 3, &vertex);
    vec3.vx = planePoint.vx - vertex.vx;
    vec3.vy = planePoint.vy - vertex.vy;
    vec3.vz = planePoint.vz - vertex.vz;
    angle = GetAng(vec0, vec2);
    angle += GetAng(vec2, vec3);
    angle += GetAng(vec3, vec1);
    angle += GetAng(vec1, vec0);
    return angle > 30000;
}
