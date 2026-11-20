#include "droid.h"

#ifdef VERSION_EU
#define RESCUE_THRUST_STEP  19
#define RESCUE_EXIT_COUNT   75
#else
#define RESCUE_THRUST_STEP  16
#define RESCUE_EXIT_COUNT   90
#endif

void UpdateRescueDroid(ShipData *shipIndex, Object **shipShapes, s32 shipNo,
                       void *weaponIndex, PadInfo *padInfo) {
    ShipData *ship;
    TrackSection *section;
    TrackSection *next;
    VECTOR nose;
    VECTOR dist;
    VECTOR *target;
    s32 i;

    if (shipNo == ownShip) {
        target = &droidTarget;
    } else if (shipNo == ownShip2) {
        target = &droidTarget2;
    } else if (shipNo == ownShip3) {
        target = &droidTarget3;
    } else if (shipNo == ownShip4) {
        target = &droidTarget4;
    }

    ship = &shipIndex[shipNo];
    ship->attr |= FLYING;

    if (padInfo->pad & PAD_ANALOG_THRUST) {
        s32 maxThrust = ship->maxThrust;
        s32 inputThrust = padInfo->thrust;
        s32 desiredThrust = (maxThrust * inputThrust) / 200;

        if (desiredThrust > maxThrust) {
            desiredThrust = maxThrust;
        }

        if (inputThrust > 0) {
            if (desiredThrust >= ship->thrust) {
                s32 diff = desiredThrust - ship->thrust;
                s32 step;
                if (diff < RESCUE_THRUST_STEP) {
                    step = diff;
                } else {
                    step = RESCUE_THRUST_STEP;
                }
                ship->thrust += step;
            } else {
                ship->thrust -= RESCUE_THRUST_STEP;
            }
        } else if (ship->thrust >= RESCUE_THRUST_STEP) {
            ship->thrust -= RESCUE_THRUST_STEP;
        }
    } else {
        if (padInfo->pad & PAD_THRUST) {
            ship->thrust += RESCUE_THRUST_STEP;
        } else if (ship->thrust >= RESCUE_THRUST_STEP) {
            ship->thrust -= RESCUE_THRUST_STEP;
        }
        if (ship->thrust > ship->maxThrust) {
            ship->thrust = ship->maxThrust;
        }
    }

    if (ship->updateCount == 500) {
        section = ship->nearTrkSect->prevSection.ptr;
        section = section->prevSection.ptr;
        section = section->prevSection.ptr;
        section = section->prevSection.ptr;

        for (i = 0; i < 10; i++) {
            section = section->nextSection.ptr;
            if (section->flags & 1) {
                break;
            }
        }

        ship->nearTrkSect = section;
        next = section->nextSection.ptr;

        target->vx = (section->centre.vx + next->centre.vx) >> 1;
        target->vy = ((section->centre.vy + next->centre.vy) >> 1) - 2000;
        target->vz = (section->centre.vz + next->centre.vz) >> 1;

        ship->vpivot.vx = 0;
        ship->vpivot.vy = 0;
        ship->vpivot.vz = 0;
        ship->updateCount--;
    }

    next = ship->nearTrkSect->nextSection.ptr;

    if (ship->attr & IN_TOW) {
        target->vx += (next->centre.vx - target->vx) >> 6;
        target->vy += (next->centre.vy - target->vy) >> 6;
        target->vz += (next->centre.vz - target->vz) >> 6;

#ifdef VERSION_EU
        ship->vpivot.vx = ((target->vx - ship->ppivot.vx) * 60) / 50;
        ship->vpivot.vy = ((target->vy - ship->ppivot.vy) * 60) / 50;
        ship->vpivot.vz = ((target->vz - ship->ppivot.vz) * 60) / 50;
#else
        ship->vpivot.vx = target->vx - ship->ppivot.vx;
        ship->vpivot.vy = target->vy - ship->ppivot.vy;
        ship->vpivot.vz = target->vz - ship->ppivot.vz;
#endif

        nose.vx = next->centre.vx - ship->nearTrkSect->centre.vx;
        nose.vy = next->centre.vy - ship->nearTrkSect->centre.vy;
        nose.vz = next->centre.vz - ship->nearTrkSect->centre.vz;

        ship->vhdg = -ratan2(nose.vx, nose.vz) - ship->hdg;
        if (ship->vhdg > 2048) {
            ship->vhdg -= 4096;
        } else if (ship->vhdg < -2048) {
            ship->vhdg += 4096;
        }

        ship->hdg += ship->vhdg >> 6;
        ship->hdg = ang(ship->hdg);
    }

    ship->vpivot.vx -= ship->vpivot.vx >> 4;
    ship->vpivot.vy -= ship->vpivot.vy >> 4;
    ship->vpivot.vz -= ship->vpivot.vz >> 4;

    ship->ppivot.vx += ship->vpivot.vx >> 5;
    ship->ppivot.vy += ship->vpivot.vy >> 5;
    ship->ppivot.vz += ship->vpivot.vz >> 5;

    dist.vx = ship->ppivot.vx - ship->nearTrkSect->centre.vx;
    dist.vy = ship->ppivot.vy - ship->nearTrkSect->centre.vy;
    dist.vz = ship->ppivot.vz - ship->nearTrkSect->centre.vz;

    ship->pitch -= ship->pitch >> 3;
    ship->roll -= ship->roll >> 5;

    if ((ship->attr & IN_TOW) && (GetMagnitude(dist) < 1600)) {
        ship->update = UpdatePlayerShip;
        ship->updateCount = RESCUE_EXIT_COUNT;
        ship->attr &= ~RESCUE;
    }
}

Gfx *UpdateDroidStart(RescueData *rescueDroid, ShipData *shipIndex, Object **shipShapes,
                      s32 shipNo, Gfx *glistp) {
    TrackCamera *cam;

    if (shipNo == ownShip) {
        cam = &camera;
    } else if (shipNo == ownShip2) {
        cam = &camera2;
    } else if (shipNo == ownShip3) {
        cam = &camera3;
    } else if (shipNo == ownShip4) {
        cam = &camera4;
    }

    if (g_pauseState == -1) {
        rescueDroid->updateCount--;

        if (rescueDroid->updateCount < 400) {
            rescueDroid->acc.vx = (-rsin(rescueDroid->hdg) * rcos(rescueDroid->pitch)) >> 14;
            rescueDroid->acc.vy = 0;
            rescueDroid->acc.vz = (rcos(rescueDroid->hdg) * rcos(rescueDroid->pitch)) >> 14;
            rescueDroid->vhdg = 0;
        } else if (rescueDroid->updateCount < 710) {
            rescueDroid->acc.vx = (-rsin(rescueDroid->hdg) * rcos(rescueDroid->pitch)) >> 15;
            rescueDroid->acc.vy = -140;
            rescueDroid->acc.vz = (rcos(rescueDroid->hdg) * rcos(rescueDroid->pitch)) >> 15;
            if ((frontendOptions_g.trackNo == 3) || (frontendOptions_g.trackNo == 4)) {
                rescueDroid->vhdg = 8;
            } else if (frontendOptions_g.trackNo == 5) {
                rescueDroid->vhdg = 0;
                rescueDroid->acc.vy = -200;
            } else {
                rescueDroid->vhdg = -8;
            }
        } else if (rescueDroid->updateCount == 770) {
            rescueDroid->acc.vy -= 90;
        } else {
            rescueDroid->hdg = shipIndex->hdg;
        }
    }

    glistp = SetDroidVectors(rescueDroid, shipShapes, shipNo, cam, glistp);

    if (rescueDroid->updateCount < 500) {
        rescueDroid->updateCount = 800;
        rescueDroid->droidUpdate = UpdateDroidCircle;
        rescueDroid->pos.vx = rescueDroid->section->centre.vx;
        rescueDroid->pos.vy = rescueDroid->section->centre.vy - 5000;
        rescueDroid->pos.vz = rescueDroid->section->centre.vz;
    }

    return glistp;
}

Gfx *UpdateDroidCircle(RescueData *rescueDroid, ShipData *shipIndex, Object **shipShapes,
                       s32 shipNo, Gfx *glistp) {
    TrackCamera *cam;
    VECTOR targetVector; /* unused: retail reserves 0x20-byte stack frame from removed circling code */
    VECTOR target;

    if (shipNo == ownShip) {
        cam = &camera;
    } else if (shipNo == ownShip2) {
        cam = &camera2;
    } else if (shipNo == ownShip3) {
        cam = &camera3;
    } else if (shipNo == ownShip4) {
        cam = &camera4;
    }

    if (shipIndex->attr & RESCUE) {
        rescueDroid->droidUpdate = UpdateDroidRescue;
        rescueDroid->updateCount = 800;

        if ((g_droidVoice == 0) && (g_pauseState == -1)) {
            g_droidVoice = PlaySfx(SFX_DROID, 255, 128, 0, -1);
        }

        if (shipNo == ownShip) {
            CameraUpdate = UpdateCameraDroidView;
        } else if (shipNo == ownShip2) {
            CameraUpdate2 = UpdateCameraDroidView;
        } else if (shipNo == ownShip3) {
            CameraUpdate3 = UpdateCameraDroidView;
        } else if (shipNo == ownShip4) {
            CameraUpdate4 = UpdateCameraDroidView;
        }

        shipIndex->attr |= REMOTE_VIEW;
        if (shipIndex->nearTrkSect->flags & 1) {
            cam->section = shipIndex->nearTrkSect->nextSection.ptr;
        } else {
            cam->section = shipIndex->nearTrkSect;
        }
    }

    return glistp;
}

Gfx *UpdateDroidRescue(RescueData *rescueDroid, ShipData *shipIndex, Object **shipShapes,
                       s32 shipNo, Gfx *glistp) {
    TrackCamera *cam;
    VECTOR targetVector; /* unused: retail reserves 0x20-byte stack slot */
    VECTOR target;
    s16 count = 0;

    if (shipNo == ownShip) {
        cam = &camera;
    } else if (shipNo == ownShip2) {
        cam = &camera2;
    } else if (shipNo == ownShip3) {
        cam = &camera3;
    } else if (shipNo == ownShip4) {
        cam = &camera4;
    }

    if (g_pauseState == -1) {
        rescueDroid->acc.vx = 0;
        rescueDroid->acc.vy = 0;
        rescueDroid->acc.vz = 0;
        rescueDroid->vel.vx = 0;
        rescueDroid->vel.vy = 0;
        rescueDroid->vel.vz = 0;

        rescueDroid->pos.vx = shipIndex->ppivot.vx;
        rescueDroid->pos.vy = shipIndex->ppivot.vy - droidFlyin;
        rescueDroid->pos.vz = shipIndex->ppivot.vz;
        rescueDroid->vhdg = 0;
        rescueDroid->hdg = shipIndex->hdg;

        if (droidFlyin > 350) {
            droidFlyin -= 50;
        }
    }

    if (!(shipIndex->attr & RESCUE)) {
        if (g_droidVoice != 0) {
            StopSfx(&g_droidVoice);
        }

        rescueDroid->droidUpdate = UpdateDroidCircle;
        rescueDroid->updateCount = 800;

        shipIndex->attr &= ~REMOTE_VIEW;
        if (shipIndex->attr & VIEW_IN) {
            if (shipNo == ownShip) {
                CameraUpdate = UpdateCameraRaceInternal;
            } else if (shipNo == ownShip2) {
                CameraUpdate2 = UpdateCameraRaceInternal;
            } else if (shipNo == ownShip3) {
                CameraUpdate3 = UpdateCameraRaceInternal;
            } else if (shipNo == ownShip4) {
                CameraUpdate4 = UpdateCameraRaceInternal;
            }
        } else {
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

        while (!(rescueDroid->section->flags & 1)) {
            count++;
            rescueDroid->section = rescueDroid->section->prevSection.ptr;
            if (count == track->sectionCount) {
                break;
            }
        }
    }

    glistp = SetDroidVectors(rescueDroid, shipShapes, shipNo, cam, glistp);
    return glistp;
}

Gfx *SetDroidVectors(RescueData *rescueDroid, Object **shipShapes, s32 shipNo,
                     TrackCamera *cam, Gfx *glistp) {
    VECTOR dist;

    ResetRenderList(&g_droidRenderList);

    if (g_pauseState == -1) {
        rescueDroid->vel.vx += rescueDroid->acc.vx;
        rescueDroid->vel.vy += rescueDroid->acc.vy;
        rescueDroid->vel.vz += rescueDroid->acc.vz;

        rescueDroid->vel.vx -= rescueDroid->vel.vx >> 3;
        rescueDroid->vel.vy -= rescueDroid->vel.vy >> 3;
        rescueDroid->vel.vz -= rescueDroid->vel.vz >> 3;

        rescueDroid->pos.vx += rescueDroid->vel.vx >> 6;
        rescueDroid->pos.vy += rescueDroid->vel.vy >> 6;
        rescueDroid->pos.vz += rescueDroid->vel.vz >> 6;

        rescueDroid->hdg += rescueDroid->vhdg;
        rescueDroid->hdg = ang(rescueDroid->hdg);
    }

    SetSkeletonPosition(shipShapes[RescueDroidModel]->skeleton,
                        rescueDroid->pos.vx >> 1,
                        rescueDroid->pos.vy >> 1,
                        rescueDroid->pos.vz >> 1);
    SetSkeletonDirectionHPR(shipShapes[RescueDroidModel]->skeleton,
                            rescueDroid->hdg,
                            rescueDroid->pitch,
                            rescueDroid->roll);
    shipShapes[RescueDroidModel]->skeleton->update = 1;
    shipShapes[RescueDroidModel]->skeleton->super = cam->camPos;

    dist.vx = ships[shipNo].ppivot.vx - rescueDroid->pos.vx;
    dist.vy = ships[shipNo].ppivot.vy - rescueDroid->pos.vy;
    dist.vz = ships[shipNo].ppivot.vz - rescueDroid->pos.vz;

    if (GetMagnitude(dist) < 15000) {
        MatrixToN64Mtx(GetSkeletonMatrix(shipShapes[RescueDroidModel]->skeleton),
                       &g_mtxPool[g_mtxIndex]);
        g_polyCount += shipShapes[RescueDroidModel]->primitiveCount;
        AddRenderGroup(&g_droidRenderList,
                       (RenderGroup *)shipShapes[RescueDroidModel]->bspTree,
                       &g_mtxPool[g_mtxIndex]);
        g_mtxIndex++;
    }

    glistp = FlushRenderList(&g_droidRenderList, glistp);
    return glistp;
}

void InitRescueDroid(RescueData *rescueDroid, Object **shipShapes, ShipData *shipIndex) {
    s32 i = 0;

    rescueDroid->section = track->sections;
    while (1) {
        rescueDroid->section = rescueDroid->section->nextSection.ptr;
        if (rescueDroid->section->flags & 1) {
            break;
        }
        i++;
        if (i > track->sectionCount) {
            break;
        }
    }

    rescueDroid->pos.vx = shipIndex->ppivot.vx;
    rescueDroid->pos.vy = shipIndex->ppivot.vy - 400;
    rescueDroid->pos.vz = shipIndex->ppivot.vz;

    rescueDroid->vel.vx = 0;
    rescueDroid->vel.vy = 0;
    rescueDroid->vel.vz = 0;

    rescueDroid->acc.vx = 0;
    rescueDroid->acc.vy = 0;
    rescueDroid->acc.vz = 0;

    rescueDroid->hdg = shipIndex->hdg;
    rescueDroid->pitch = shipIndex->pitch;
    rescueDroid->roll = shipIndex->roll;

    rescueDroid->vhdg = 0;
    rescueDroid->vpitch = 0;
    rescueDroid->vroll = 0;

    rescueDroid->updateCount = 800;
    rescueDroid->modelID = RescueDroidModel;

    rescueDroid->hdg = 0;
    rescueDroid->pitch = 0;
    rescueDroid->roll = 0;

    rescueDroid->droidUpdate = UpdateDroidStart;
}
