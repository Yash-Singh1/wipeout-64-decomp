/*
 * scnanim.c — Scenery object prefix dispatch and landscape visibility renderer.
 *
 * Evaluates track camera facing direction and queues visible trackside PRM
 * objects (matching prefix tags B_, A_, D_, C_, V_, Z_, H_, J_ for waving,
 * rotating, glowing, and translucent scenery) into the scenery render lists.
 */
#include "scnanim.h"

u8 TrackDirection(TrackCamera *camera, s32 *angleOut) {
    TrackSection *nextSection;
    SVECTOR dir;
    VECTOR unused; /* unreferenced, but the retail frame reserves 0x10 bytes */

    nextSection = camera->section->nextSection.ptr;

    dir.vx = nextSection->centre.vx - camera->section->centre.vx;
    dir.vy = nextSection->centre.vy - camera->section->centre.vy;
    dir.vz = nextSection->centre.vz - camera->section->centre.vz;

    *angleOut = camera->hdg - ratan2(dir.vx, dir.vz);
    *angleOut = ang(*angleOut);

    if ((*angleOut >= -512) && (*angleOut < 512)) {
        return ShipForwards;
    } else if ((*angleOut >= 512) && (*angleOut < 1536)) {
        return ShipRight;
    } else if ((*angleOut >= 1536) || (*angleOut < -1536)) {
        return ShipBackwards;
    } else {
        return ShipLeft;
    }
}

Gfx *DrawLandscape(ShipData *playerShip, TrackCamera *camera, Gfx *glistp) {
    TrackSection *section;
    Object *object;
    VECTOR projected;
    SVECTOR origin;
    s32 flag;
    s32 extent;
    s32 nearZ;
    s16 i;
    s32 nearClip;
    s32 farClip;
    s32 secNo;

    nearClip = g_sceneryNearClip[g_trackIndex];
    farClip = g_sceneryFarClip[g_trackIndex];

    for (i = 0; i < 128; i++) {
        g_drawnObjects[i] = 0;
    }

    g_sceneryPrimCount = 0;
    ResetRenderList(&g_sceneryRenderList1);
    ResetRenderList(&g_sceneryRenderList2);

    section = camera->section;
    if (section->objectCount > 0) {
        origin.vx = 0;
        origin.vy = 0;
        origin.vz = 0;

        for (i = section->objectCount - 1; i >= 0; i--) {
            if (frontendOptions_g.trackNo == 6) {
                while (i >= 0) {
                    if (!(trackObjects[section->objects[i]]->flags & OBJ_FLAG_TRANSLUCENT)) {
                        break;
                    }
                    i--;
                }
                if (i <= -1) {
                    break;
                }
            } else if (frontendOptions_g.trackNo == 5) {
                while (i >= 0) {
                    if (!(trackObjects[section->objects[i]]->flags & OBJ_FLAG_TRANSLUCENT)) {
                        break;
                    }
                    i--;
                }
                if (i <= -1) {
                    break;
                }

                secNo = section->secNo;
                if ((secNo >= 6) && (secNo <= 78)) {
                    while (i >= 0) {
                        if (!CompareObjectName(trackObjects[section->objects[i]]->name, "B_", 2)) {
                            break;
                        }
                        i--;
                    }
                } else if ((secNo >= 89) && (secNo <= 109)) {
                    while (CompareObjectName(trackObjects[section->objects[i]]->name, "A_", 2)) {
                        i--;
                    }
                } else if ((secNo >= 306) && (secNo <= 338)) {
                    while (CompareObjectName(trackObjects[section->objects[i]]->name, "D_", 2)) {
                        i--;
                    }
                } else if ((secNo >= 357) && (secNo <= 407)) {
                    while (CompareObjectName(trackObjects[section->objects[i]]->name, "C_", 2)) {
                        i--;
                    }
                }

                if (i <= -1) {
                    break;
                }
            } else if (frontendOptions_g.trackNo == 2) {
                secNo = section->secNo;
                if ((secNo >= 200) && (secNo <= 289)) {
                    while (i >= 0) {
                        if (!CompareObjectName(trackObjects[section->objects[i]]->name, "B_", 2)) {
                            break;
                        }
                        i--;
                    }
                } else if (secNo >= 418) {
                    while (CompareObjectName(trackObjects[section->objects[i]]->name, "A_", 2)) {
                        i--;
                    }
                } else if ((secNo >= 0) && (secNo < 60)) {
                    while (CompareObjectName(trackObjects[section->objects[i]]->name, "A_", 2)) {
                        i--;
                    }
                }

                if (i <= -1) {
                    break;
                }
            } else if (frontendOptions_g.trackNo == 3) {
                while (i >= 0) {
                    if (!(trackObjects[section->objects[i]]->flags & OBJ_FLAG_TRANSLUCENT)) {
                        break;
                    }
                    i--;
                }
                if (i <= -1) {
                    break;
                }

                secNo = section->secNo;
                if ((secNo >= 2) && (secNo <= 58)) {
                    while (i >= 0) {
                        if (CompareObjectName(trackObjects[section->objects[i]]->name, "V_", 2)) {
                            i--;
                        } else if (CompareObjectName(trackObjects[section->objects[i]]->name, "C_", 2)) {
                            i--;
                        } else if ((secNo >= 11) && (secNo <= 27) &&
                                   CompareObjectName(trackObjects[section->objects[i]]->name, "Z_", 2)) {
                            i--;
                        } else {
                            break;
                        }
                    }
                } else if ((secNo >= 110) && (secNo <= 158)) {
                    while (CompareObjectName(trackObjects[section->objects[i]]->name, "H_", 2)) {
                        i--;
                    }
                } else if ((secNo >= 240) && (secNo <= 321)) {
                    while (CompareObjectName(trackObjects[section->objects[i]]->name, "V_", 2)) {
                        i--;
                    }
                } else if (secNo >= 356) {
                    while (CompareObjectName(trackObjects[section->objects[i]]->name, "J_", 2)) {
                        i--;
                    }
                }

                if (i <= -1) {
                    break;
                }
            }

            object = trackObjects[section->objects[i]];
            object->skeleton->super = camera->camPos;
            object->skeleton->update = 1;
            SetSkeletonTransform(object->skeleton);

            extent = object->extent >> 1;
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
                    MatrixToN64Mtx(&object->skeleton->absolute, &g_mtxPool[g_mtxIndex]);
                    g_sceneryPrimCount += object->primitiveCount;
                    if (object->flags & OBJ_FLAG_RENDERLIST2) {
                        AddRenderGroup(&g_sceneryRenderList2, (RenderGroup *)object->bspTree,
                                       (Mtx *)&g_mtxPool[g_mtxIndex]);
                    } else {
                        AddRenderGroup(&g_sceneryRenderList1, (RenderGroup *)object->bspTree,
                                       (Mtx *)&g_mtxPool[g_mtxIndex]);
                    }
                    g_mtxIndex++;
                }
            }
        }

        glistp = FlushRenderList(&g_sceneryRenderList2, glistp);
        glistp = FlushRenderList(&g_sceneryRenderList1, glistp);

        if (((frontendOptions_g.trackNo == 6) ||
             (frontendOptions_g.trackNo == 3) ||
             (frontendOptions_g.trackNo == 5)) &&
            (frontendOptions_g.numPlayers == 1)) {
            ResetRenderList(&g_sceneryRenderList1);
            gDPSetRenderMode(glistp++, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF2);

            for (i = section->objectCount - 1; i >= 0; i--) {
                if (frontendOptions_g.trackNo == 6) {
                    while (i >= 0) {
                        if (trackObjects[section->objects[i]]->flags & OBJ_FLAG_TRANSLUCENT) {
                            break;
                        }
                        i--;
                    }
                    if (i == -1) {
                        break;
                    }
                } else if (frontendOptions_g.trackNo == 5) {
                    while (i >= 0) {
                        if (trackObjects[section->objects[i]]->flags & OBJ_FLAG_TRANSLUCENT) {
                            break;
                        }
                        i--;
                    }
                    if (i == -1) {
                        break;
                    }
                } else if (frontendOptions_g.trackNo == 3) {
                    while (i >= 0) {
                        if (trackObjects[section->objects[i]]->flags & OBJ_FLAG_TRANSLUCENT) {
                            break;
                        }
                        i--;
                    }
                    if (i <= -1) {
                        break;
                    }
                }

                object = trackObjects[section->objects[i]];
                object->skeleton->super = camera->camPos;
                object->skeleton->update = 1;
                SetSkeletonTransform(object->skeleton);

                extent = object->extent >> 1;
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
                        MatrixToN64Mtx(&object->skeleton->absolute, &g_mtxPool[g_mtxIndex]);
                        g_sceneryPrimCount += object->primitiveCount;
                        AddRenderGroup(&g_sceneryRenderList1, (RenderGroup *)object->bspTree,
                                       (Mtx *)&g_mtxPool[g_mtxIndex]);
                        g_mtxIndex++;
                    }
                }
            }

            glistp = FlushRenderList(&g_sceneryRenderList1, glistp);
            gDPSetRenderMode(glistp++, G_RM_TEX_EDGE | Z_CMP | Z_UPD,
                             G_RM_TEX_EDGE2 | Z_CMP | Z_UPD);
        }
    }

    g_waveIndex++;
    return glistp;
}
