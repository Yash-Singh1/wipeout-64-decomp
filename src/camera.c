#include "camera.h"

void InitCamera(TrackCamera *camera, TrackSection *section) {
    s32 i;
    VECTOR pos;

    camera->section = section;
    for (i = 0; i < 2; i++) {
        camera->section = camera->section->prevSection.ptr;
    }

    pos.vx = camera->section->centre.vx;
    pos.vy = camera->section->centre.vy;
    pos.vz = camera->section->centre.vz;

    SetSkeletonPosition(camera->camPos, -pos.vx >> 1, -pos.vy >> 1, -pos.vz >> 1);

    camera->hdg = 0;
    camera->pitch = 0;
    camera->roll = 0;

    camera->vel.vx = 0;
    camera->vel.vy = 0;
    camera->vel.vz = 0;
}

void UpdateCameraRaceExternal(TrackCamera *camera, ShipData *camShip,
                              RescueData *rescueDroid, s32 shipNo) {
    VECTOR pos;
    VECTOR bestPath;
    VECTOR distance;
    TrackSection *nextSection;
    s32 *vel;

    if (shipNo == ownShip) {
        vel = &raceExternalVel;
    } else if (shipNo == ownShip2) {
        vel = &raceExternalVel2;
    } else if (shipNo == ownShip3) {
        vel = &raceExternalVel3;
    } else if (shipNo == ownShip4) {
        vel = &raceExternalVel4;
    }

    pos.vx = camShip->ppivot.vx - (camShip->unitVecNose.vx >> 2);
    pos.vy = camShip->ppivot.vy - (camShip->unitVecNose.vy >> 2);
    pos.vz = camShip->ppivot.vz - (camShip->unitVecNose.vz >> 2);
    pos.vy -= 200;

    g_raceStartCamera = 0;
    camera->section = FindNearestSection(camera->section, pos);
    nextSection = camera->section->nextSection.ptr;

    GetTargetPos(&bestPath, pos, nextSection->centre, camera->section->centre);

    distance.vx = pos.vx - bestPath.vx;
    distance.vy = pos.vy - bestPath.vy;
    distance.vz = pos.vz - bestPath.vz;

#ifdef VERSION_EU
    distance.vx = (distance.vx * 60) / 50;
    distance.vy = (distance.vy * 60) / 50;
    distance.vz = (distance.vz * 60) / 50;
#endif

    camera->vel.vx -= distance.vx >> 6;
    camera->vel.vy -= distance.vy >> 6;
    camera->vel.vz -= distance.vz >> 6;

    camera->vel.vx -= sar(camera->vel.vx, 3);
    camera->vel.vy -= sar(camera->vel.vy, 3);
    camera->vel.vz -= sar(camera->vel.vz, 3);

    pos.vx += camera->vel.vx;
    pos.vy += camera->vel.vy;
    pos.vz += camera->vel.vz;

    *vel += GetMagnitude(distance) >> 7;
    *vel -= *vel >> 4;
    pos.vy -= *vel;

    SetSkeletonPosition(camera->camPos, -pos.vx >> 1, -pos.vy >> 1, -pos.vz >> 1);

    camera->pitch = -camShip->pitch;
    camera->roll = 0;
    camera->hdg = -camShip->hdg;

    SetSkeletonDirection(camera->camPos->super, camera->pitch, camera->hdg, 0);
    camera->camPos->super->update = 1;
}

void UpdateCameraRaceInternal(TrackCamera *camera, ShipData *camShip,
                              RescueData *rescueDroid, s32 shipNo) {
    VECTOR unitFinVector;

    g_raceStartCamera = 0;
    camera->section = camShip->nearTrkSect;
    SetUnitFinVector(&unitFinVector, camShip);

    SetSkeletonPosition(camera->camPos,
                        ((unitFinVector.vx >> 5) - camShip->ppivot.vx) >> 1,
                        ((unitFinVector.vy >> 5) - camShip->ppivot.vy) >> 1,
                        ((unitFinVector.vz >> 5) - camShip->ppivot.vz) >> 1);

    camera->pitch = -camShip->pitch;
    camera->hdg = -camShip->hdg;
    camera->roll = -camShip->roll;

    SetSkeletonDirectionRPH(camera->camPos->super, -camShip->roll, -camShip->pitch, -camShip->hdg);
    camera->camPos->super->update = 1;
}

void UpdateCameraRaceStart(TrackCamera *camera, ShipData *camShip,
                           RescueData *rescueDroid, s32 shipNo) {
    VECTOR pos;
    VECTOR beginPos;
    VECTOR unusedVec; /* unused: retail reserves 0x10-byte stack slot */
    VECTOR targetVector;
    TrackSection *nextSection;
    s32 i;
    s32 u;
    s32 t;
    s32 a;
    s32 s;
    s32 mag;

    g_raceStartCamera = 1;
    u = 2;
    a = ((1 - u) << 8) / 50;

    nextSection = camShip->nearTrkSect->nextSection.ptr;
    if ((g_gameType == 1) || ((g_gameType == 6) && (frontendOptions_g.gameMode == 2))) {
        for (i = 0; i < 3; i++) {
            nextSection = nextSection->nextSection.ptr;
        }
    } else {
        for (i = 0; i < 9; i++) {
            nextSection = nextSection->nextSection.ptr;
        }
    }

    pos.vx = nextSection->centre.vx;
    pos.vy = nextSection->centre.vy;
    pos.vz = nextSection->centre.vz;
    pos.vy -= 800;

    beginPos.vx = camShip->nearTrkSect->centre.vx;
    beginPos.vy = camShip->ppivot.vy - 400;
    beginPos.vz = camShip->nearTrkSect->centre.vz;

    t = 200 - camShip->updateCount;
    s = (u * t) << 8;
    s += ((t * t) * a) >> 1;
    s = 25600 - s;

    pos.vx = beginPos.vx - ((((beginPos.vx - pos.vx) * s) / 100) >> 8);
    pos.vy = beginPos.vy - ((((beginPos.vy - pos.vy) * s) / 100) >> 8);
    pos.vz = beginPos.vz - ((((beginPos.vz - pos.vz) * s) / 100) >> 8);

    camera->section = FindNearestSection(camera->section, pos);

    targetVector.vx = camShip->ppivot.vx - pos.vx;
    targetVector.vy = camShip->ppivot.vy - pos.vy;
    targetVector.vz = camShip->ppivot.vz - pos.vz;

    mag = SquareRoot0((targetVector.vx * targetVector.vx) + (targetVector.vz * targetVector.vz));

    SetSkeletonPosition(camera->camPos, -pos.vx >> 1, -pos.vy >> 1, -pos.vz >> 1);
    camera->hdg = ratan2(targetVector.vx, targetVector.vz);
    camera->pitch = ratan2(targetVector.vy, mag);

    SetSkeletonDirection(camera->camPos->super, camera->pitch, camera->hdg, 0);
    camera->camPos->super->update = 1;

    if (camShip->updateCount == 100) {
        if (frontendOptions_g.cameraView == 1) {
            camShip->attr |= VIEW_IN;
            if (shipNo == ownShip) {
                CameraUpdate = UpdateCameraRaceInternal;
            } else if (shipNo == ownShip2) {
                CameraUpdate2 = UpdateCameraRaceInternal;
            } else if (shipNo == ownShip3) {
                CameraUpdate3 = UpdateCameraRaceInternal;
            } else if (shipNo == ownShip4) {
                CameraUpdate4 = UpdateCameraRaceInternal;
            }
        } else if (frontendOptions_g.cameraView == 0) {
            camShip->attr &= ~VIEW_IN;
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

void UpdateCameraAttractCircle(TrackCamera *camera, ShipData *camShip,
                               RescueData *rescueDroid, s32 shipNo) {
    VECTOR pos;
    VECTOR targetVector;
    VECTOR unitFinVector;
    s32 mag;
    s16 *target;

    g_raceStartCamera = 0;
    camera->section = camShip->nearTrkSect;

    if (shipNo == ownShip) {
        target = &cameraTarget;
    } else if (shipNo == ownShip2) {
        target = &cameraTarget2;
    } else if (shipNo == ownShip3) {
        target = &cameraTarget3;
    } else if (shipNo == ownShip4) {
        target = &cameraTarget4;
    }

    if (*target == 0) {
        *target = 150;
    }
    (*target)--;
    if (g_cameraSwitchFlag != 0) {
        *target = 0;
    }
    if (*target == 0) {
        if (shipNo == ownShip) {
            CameraUpdate = UpdateCameraAttractMode;
        } else if (shipNo == ownShip2) {
            CameraUpdate2 = UpdateCameraAttractMode;
        } else if (shipNo == ownShip3) {
            CameraUpdate3 = UpdateCameraAttractMode;
        } else if (shipNo == ownShip4) {
            CameraUpdate4 = UpdateCameraAttractMode;
        }
    }

    SetUnitFinVector(&unitFinVector, camShip);

    pos.vx = camShip->ppivot.vx + (s16)(rsin(camShip->hdg) / 6);
    pos.vy = camShip->ppivot.vy + ((camShip->pitch >> 2) - 200);
    pos.vz = camShip->ppivot.vz - (s16)(rcos(camShip->hdg) / 6);

    pos.vx += (s16)(rsin(*target << 3)) >> 3;
    pos.vy -= 200;
    pos.vz += (s16)(rcos(*target << 3)) >> 3;

    pos.vx -= unitFinVector.vx >> 4;
    pos.vy -= unitFinVector.vy >> 4;
    pos.vz -= unitFinVector.vz >> 4;

    targetVector.vx = camShip->ppivot.vx - pos.vx;
    targetVector.vy = camShip->ppivot.vy - pos.vy;
    targetVector.vz = camShip->ppivot.vz - pos.vz;

    mag = SquareRoot0((targetVector.vx * targetVector.vx) + (targetVector.vz * targetVector.vz));

    SetSkeletonPosition(camera->camPos, -pos.vx >> 1, -pos.vy >> 1, -pos.vz >> 1);
    camera->hdg = ratan2(targetVector.vx, targetVector.vz);
    camera->pitch = ratan2(targetVector.vy, mag);

    SetSkeletonDirection(camera->camPos->super, camera->pitch, camera->hdg, 0);
    camera->camPos->super->update = 1;
}

void UpdateCameraAttractMedal(TrackCamera *camera, ShipData *camShip,
                              RescueData *rescueDroid, s32 shipNo) {
    VECTOR pos;
    VECTOR targetVector;
    VECTOR unitFinVector;
    s32 mag;
    s16 *target;

    g_raceStartCamera = 0;
    camera->section = camShip->nearTrkSect;

    if (shipNo == ownShip) {
        target = &cameraTarget;
    } else if (shipNo == ownShip2) {
        target = &cameraTarget2;
    } else if (shipNo == ownShip3) {
        target = &cameraTarget3;
    } else if (shipNo == ownShip4) {
        target = &cameraTarget4;
    }

    if (*target == 0) {
        *target = 150;
    }
    (*target)--;

    SetUnitFinVector(&unitFinVector, camShip);

    pos.vx = camShip->ppivot.vx;
    pos.vy = camShip->ppivot.vy;
    pos.vz = camShip->ppivot.vz;
    pos.vy -= 1024;

    targetVector.vx = camShip->ppivot.vx - pos.vx;
    targetVector.vy = camShip->ppivot.vy - pos.vy;
    targetVector.vz = camShip->ppivot.vz - pos.vz;

    mag = SquareRoot0((targetVector.vx * targetVector.vx) + (targetVector.vz * targetVector.vz));

    SetSkeletonPosition(camera->camPos, -pos.vx >> 1, -pos.vy >> 1, -pos.vz >> 1);
    camera->hdg = ratan2(targetVector.vx, targetVector.vz);
    camera->pitch = ratan2(targetVector.vy, mag);

    SetSkeletonDirection(camera->camPos->super, camera->pitch, camera->hdg, 0);
    camera->camPos->super->update = 1;
}

void UpdateCameraAttractDestroyed(TrackCamera *camera, ShipData *camShip,
                                  RescueData *rescueDroid, s32 shipNo) {
    VECTOR pos; /* unused: retail reserves 0x30-byte stack frame from removed attract camera code */
    VECTOR targetVector;
    VECTOR unitFinVector;

    g_raceStartCamera = 0;
    ships[shipNo].attr &= ~VIEW_IN;
    UpdateCameraRaceExternal(camera, camShip, rescueDroid, shipNo);
}

void UpdateCameraDroidView(TrackCamera *camera, ShipData *camShip,
                           RescueData *rescueDroid, s32 shipNo) {
    VECTOR pos;
    VECTOR bestPath;
    VECTOR distance;
    TrackSection *previous;
    s32 *vel;

    if (shipNo == ownShip) {
        vel = &droidViewVel;
    } else if (shipNo == ownShip2) {
        vel = &droidViewVel2;
    } else if (shipNo == ownShip3) {
        vel = &droidViewVel3;
    } else if (shipNo == ownShip4) {
        vel = &droidViewVel4;
    }

    pos.vx = camShip->ppivot.vx + (camShip->unitVecNose.vx >> 2);
    pos.vy = camShip->ppivot.vy + (camShip->unitVecNose.vy >> 2);
    pos.vz = camShip->ppivot.vz + (camShip->unitVecNose.vz >> 2);
    pos.vy -= 200;

    g_raceStartCamera = 0;
    camera->section = FindNearestSection(camera->section, pos);
    previous = camera->section->prevSection.ptr;

    GetTargetPos(&bestPath, pos, previous->centre, camera->section->centre);

    distance.vx = pos.vx - bestPath.vx;
    distance.vy = pos.vy - bestPath.vy;
    distance.vz = pos.vz - bestPath.vz;

#ifdef VERSION_EU
    distance.vx = (distance.vx * 60) / 50;
    distance.vy = (distance.vy * 60) / 50;
    distance.vz = (distance.vz * 60) / 50;
#endif

    camera->vel.vx -= distance.vx >> 6;
    camera->vel.vy -= distance.vy >> 6;
    camera->vel.vz -= distance.vz >> 6;

    camera->vel.vx -= sar(camera->vel.vx, 3);
    camera->vel.vy -= sar(camera->vel.vy, 3);
    camera->vel.vz -= sar(camera->vel.vz, 3);

    pos.vx += camera->vel.vx;
    pos.vy += camera->vel.vy;
    pos.vz += camera->vel.vz;

    *vel += GetMagnitude(distance) >> 7;
    *vel -= *vel >> 4;
    pos.vy -= *vel;

    SetSkeletonPosition(camera->camPos, -pos.vx >> 1, -pos.vy >> 1, -pos.vz >> 1);

    camera->pitch = -camShip->pitch;
    camera->roll = 0;
    camera->hdg = -(camShip->hdg + 2048);

    SetSkeletonDirection(camera->camPos->super, camera->pitch, camera->hdg, 0);
    camera->camPos->super->update = 1;
}

void UpdateCameraAttractInternal(TrackCamera *camera, ShipData *camShip,
                                 RescueData *rescueDroid, s32 shipNo) {
    VECTOR unitFinVector;
    s16 *target;

    g_raceStartCamera = 0;

    if (shipNo == ownShip) {
        target = &cameraTarget;
    } else if (shipNo == ownShip2) {
        target = &cameraTarget2;
    } else if (shipNo == ownShip3) {
        target = &cameraTarget3;
    } else if (shipNo == ownShip4) {
        target = &cameraTarget4;
    }

    if (*target == 0) {
        *target = 150;
    }
    (*target)--;
    if (g_cameraSwitchFlag != 0) {
        *target = 0;
    }
    if (*target == 0) {
        if (shipNo == ownShip) {
            CameraUpdate = UpdateCameraAttractMode;
        } else if (shipNo == ownShip2) {
            CameraUpdate2 = UpdateCameraAttractMode;
        } else if (shipNo == ownShip3) {
            CameraUpdate3 = UpdateCameraAttractMode;
        } else if (shipNo == ownShip4) {
            CameraUpdate4 = UpdateCameraAttractMode;
        }
    }

    camera->section = camShip->nearTrkSect;
    SetUnitFinVector(&unitFinVector, camShip);

    SetSkeletonPosition(camera->camPos,
                        ((unitFinVector.vx >> 5) - camShip->ppivot.vx) >> 1,
                        ((unitFinVector.vy >> 5) - camShip->ppivot.vy) >> 1,
                        ((unitFinVector.vz >> 5) - camShip->ppivot.vz) >> 1);

    camera->pitch = -camShip->pitch;
    camera->hdg = -camShip->hdg;

    SetSkeletonDirectionRPH(camera->camPos->super, -camShip->roll, -camShip->pitch, -camShip->hdg);
    camera->camPos->super->update = 1;
}

void UpdateCameraStaticFollow(TrackCamera *camera, ShipData *camShip,
                              RescueData *rescueDroid, s32 shipNo) {
    VECTOR targetVector;
    TrackSection *section;
    s16 *target;
    VECTOR *pos;
    s32 i;
    s32 mag;
    u8 count;

    g_raceStartCamera = 0;

    if (shipNo == ownShip) {
        target = &cameraTarget;
        pos = &cameraStaticPos;
    } else if (shipNo == ownShip2) {
        target = &cameraTarget2;
        pos = &cameraStaticPos2;
    } else if (shipNo == ownShip3) {
        target = &cameraTarget3;
        pos = &cameraStaticPos3;
    } else if (shipNo == ownShip4) {
        target = &cameraTarget4;
        pos = &cameraStaticPos4;
    }

    if (*target == 0) {
        section = ships[shipNo].nearTrkSect->nextSection.ptr;
        if (frontendOptions_g.numPlayers < 3) {
            count = 10;
        } else {
            count = 8;
        }
        for (i = 0; i < count; i++) {
            section = section->nextSection.ptr;
        }
        camera->section = section;
        pos->vx = section->centre.vx;
        pos->vy = section->centre.vy - 500;
        pos->vz = section->centre.vz;
        *target = 150;
    }

    (*target)--;
    if (*target == 0) {
        if (shipNo == ownShip) {
            CameraUpdate = UpdateCameraAttractMode;
        } else if (shipNo == ownShip2) {
            CameraUpdate2 = UpdateCameraAttractMode;
        } else if (shipNo == ownShip3) {
            CameraUpdate3 = UpdateCameraAttractMode;
        } else if (shipNo == ownShip4) {
            CameraUpdate4 = UpdateCameraAttractMode;
        }
    }

    targetVector.vx = camShip->ppivot.vx - pos->vx;
    targetVector.vy = camShip->ppivot.vy - pos->vy;
    targetVector.vz = camShip->ppivot.vz - pos->vz;

    mag = SquareRoot0((targetVector.vx * targetVector.vx) + (targetVector.vz * targetVector.vz));

    SetSkeletonPosition(camera->camPos, -pos->vx >> 1, -pos->vy >> 1, -pos->vz >> 1);
    camera->hdg = ratan2(targetVector.vx, targetVector.vz);
    camera->pitch = ratan2(targetVector.vy, mag);

    SetSkeletonDirection(camera->camPos->super, camera->pitch, camera->hdg, 0);
    camera->camPos->super->update = 1;
}

void UpdateCameraAttractMode(TrackCamera *camera, ShipData *camShip,
                             RescueData *rescueDroid, s32 shipNo) {
    s32 playerIdx;
    s16 *target;

    if (shipNo == ownShip) {
        playerIdx = 0;
        target = &cameraTarget;
    } else if (shipNo == ownShip2) {
        playerIdx = 1;
        target = &cameraTarget2;
    } else if (shipNo == ownShip3) {
        playerIdx = 2;
        target = &cameraTarget3;
    } else if (shipNo == ownShip4) {
        playerIdx = 3;
        target = &cameraTarget4;
    }

    *target = 0;
    g_raceStartCamera = 0;

    if (ships[shipNo].combatAttr & DESTROYED) {
        if (shipNo == ownShip) {
            CameraUpdate = UpdateCameraAttractDestroyed;
        } else if (shipNo == ownShip2) {
            CameraUpdate2 = UpdateCameraAttractDestroyed;
        } else if (shipNo == ownShip3) {
            CameraUpdate3 = UpdateCameraAttractDestroyed;
        } else if (shipNo == ownShip4) {
            CameraUpdate4 = UpdateCameraAttractDestroyed;
        }
        camShip->attr &= ~VIEW_IN;
    } else if (((frontendOptions_g.numPlayers >= 2) && (g_playerFinished[playerIdx] == 1)) ||
               (g_gameType == 6)) {
        if (shipNo == ownShip) {
            CameraUpdate = UpdateCameraAttractInternal;
            camShip->attr |= VIEW_IN;
        } else if (shipNo == ownShip2) {
            CameraUpdate2 = UpdateCameraAttractInternal;
            camShip->attr |= VIEW_IN;
        } else if (shipNo == ownShip3) {
            CameraUpdate3 = UpdateCameraAttractInternal;
            camShip->attr |= VIEW_IN;
        } else if (shipNo == ownShip4) {
            CameraUpdate4 = UpdateCameraAttractInternal;
            camShip->attr |= VIEW_IN;
        }
    } else {
        switch (newRand() % 3) {
            case 0:
                if (shipNo == ownShip) {
                    CameraUpdate = UpdateCameraAttractCircle;
                } else if (shipNo == ownShip2) {
                    CameraUpdate2 = UpdateCameraAttractCircle;
                } else if (shipNo == ownShip3) {
                    CameraUpdate3 = UpdateCameraAttractCircle;
                } else if (shipNo == ownShip4) {
                    CameraUpdate4 = UpdateCameraAttractCircle;
                }
                camShip->attr &= ~VIEW_IN;
                break;

            case 1:
                if (frontendOptions_g.numPlayers >= 2) {
                    if (shipNo == ownShip) {
                        CameraUpdate = UpdateCameraAttractInternal;
                    } else if (shipNo == ownShip2) {
                        CameraUpdate2 = UpdateCameraAttractInternal;
                    } else if (shipNo == ownShip3) {
                        CameraUpdate3 = UpdateCameraAttractInternal;
                    } else if (shipNo == ownShip4) {
                        CameraUpdate4 = UpdateCameraAttractInternal;
                    }
                    camShip->attr |= VIEW_IN;
                } else {
                    if (shipNo == ownShip) {
                        CameraUpdate = UpdateCameraStaticFollow;
                    } else if (shipNo == ownShip2) {
                        CameraUpdate2 = UpdateCameraStaticFollow;
                    } else if (shipNo == ownShip3) {
                        CameraUpdate3 = UpdateCameraStaticFollow;
                    } else if (shipNo == ownShip4) {
                        CameraUpdate4 = UpdateCameraStaticFollow;
                    }
                    camShip->attr &= ~VIEW_IN;
                }
                break;

            case 2:
                if (shipNo == ownShip) {
                    CameraUpdate = UpdateCameraAttractInternal;
                } else if (shipNo == ownShip2) {
                    CameraUpdate2 = UpdateCameraAttractInternal;
                } else if (shipNo == ownShip3) {
                    CameraUpdate3 = UpdateCameraAttractInternal;
                } else if (shipNo == ownShip4) {
                    CameraUpdate4 = UpdateCameraAttractInternal;
                }
                camShip->attr |= VIEW_IN;
                break;
        }
    }

    if (shipNo == ownShip) {
        CameraUpdate(camera, &ships[shipNo], rescueDroid, shipNo);
    } else if ((frontendOptions_g.numPlayers >= 2) && (shipNo == ownShip2)) {
        CameraUpdate2(camera, &ships[shipNo], &rescueDroid[1], shipNo);
    } else if ((frontendOptions_g.numPlayers >= 3) && (shipNo == ownShip3)) {
        CameraUpdate3(camera, &ships[shipNo], &rescueDroid[2], shipNo);
    } else if ((frontendOptions_g.numPlayers >= 4) && (shipNo == ownShip4)) {
        CameraUpdate4(camera, &ships[shipNo], &rescueDroid[3], shipNo);
    }
}

void SetUnitFinVector(VECTOR *fin, ShipData *ship) {
    s16 sy, sx, sz;
    s16 cy, cx, cz;

    sy = rsin(ship->hdg);
    sx = rsin(ship->pitch);
    sz = rsin(ship->roll);
    cy = rcos(ship->hdg);
    cx = rcos(ship->pitch);
    cz = rcos(ship->roll);

    fin->vx = ((cy * sz) >> 12) - ((((sy * sx) >> 12) * cz) >> 12);
    fin->vy = (cx * cz) >> 12;
    fin->vz = ((sy * sz) >> 12) + ((((cy * sx) >> 12) * cz) >> 12);
}
