#include "radscn.h"

#define ABS(x) ((x) < 0 ? -(x) : (x))

s32 SquareRoot0(u32 a) {
    u32 x;
    u32 shift;

    x = a;
    if (a != 0) {
        shift = 0;
        while ((a & 0xC0000000) == 0) {
            a <<= 2;
            shift++;
        }

        a >>= 23;
        a++;
        a >>= 1;
        a = g_sqrtTable[a];
        if (shift != 0) {
            a += 1 << (shift - 1);
            a >>= shift;
        }

        a = (a + (x / a)) >> 1;
        if (a > 0xFFFF) {
            a = 0xFFFF;
        }
    }

    return a;
}

void SceneRadiusCheck(TrackSection *section, Object *objects) {
    TrackSection *visSection;
    s16 j;
    Object *object;
    MATRIX *matrix;
    Vector *sectionCentre;
    s16 objectCount;
    Vector vector;
    s64 distance;
    s16 objectList[ObjectListMax];
    s16 withinLocal;
    s16 withinGlobal;
    s16 allCount;

    DFree(g_heap, section->objects);
    section->objectCount = 0;

    object = objects;
    objectCount = 0;
    allCount = 0;

    while (object) {
        withinLocal = 0;
        withinGlobal = 0;

        matrix = &object->skeleton->relative;
        sectionCentre = &section->centre;

        vector.vx = matrix->t[0] - sectionCentre->vx;
        vector.vy = matrix->t[1] - sectionCentre->vy;
        vector.vz = matrix->t[2] - sectionCentre->vz;

        if (section->flags & Jump) {
            if ((ABS(vector.vx) < (section->globalRadius * 2)) &&
                (ABS(vector.vy) < (section->globalRadius * 2)) &&
                (ABS(vector.vz) < (section->globalRadius * 2))) {
                distance = SquareRoot0((vector.vx * vector.vx) + (vector.vy * vector.vy) + (vector.vz * vector.vz));
                distance = distance - object->extent;
                distance -= 1024;
                if (distance < section->globalRadius) {
                    withinGlobal = 1;
                }
            }
        } else {
            if ((ABS(vector.vx) < section->globalRadius) &&
                (ABS(vector.vy) < section->globalRadius) &&
                (ABS(vector.vz) < section->globalRadius)) {
                distance = SquareRoot0((vector.vx * vector.vx) + (vector.vy * vector.vy) + (vector.vz * vector.vz));
                distance = distance - object->extent;
                distance -= 1024;
                if (distance < section->globalRadius) {
                    withinGlobal = 1;
                }
            }
        }

        if (withinGlobal) {
            visSection = section;
            for (j = 0; (!withinLocal) && (j < 30); j++) {
                matrix = &object->skeleton->relative;
                sectionCentre = &visSection->centre;

                vector.vx = matrix->t[0] - sectionCentre->vx;
                vector.vy = matrix->t[1] - sectionCentre->vy;
                vector.vz = matrix->t[2] - sectionCentre->vz;

                if ((ABS(vector.vx) < section->localRadius) &&
                    (ABS(vector.vy) < section->localRadius) &&
                    (ABS(vector.vz) < section->localRadius)) {
                    distance = SquareRoot0((vector.vx * vector.vx) + (vector.vy * vector.vy) + (vector.vz * vector.vz));
                    distance = distance - object->extent;
                    distance -= 1024;
                    if (distance < section->localRadius) {
                        withinLocal = 1;
                    }
                }
                visSection = visSection->nextSection.ptr;
            }

            visSection = section;
            for (j = 0; (!withinLocal) && (j < 30); j++) {
                matrix = &object->skeleton->relative;
                sectionCentre = &visSection->centre;

                vector.vx = matrix->t[0] - sectionCentre->vx;
                vector.vy = matrix->t[1] - sectionCentre->vy;
                vector.vz = matrix->t[2] - sectionCentre->vz;

                if ((ABS(vector.vx) < section->localRadius) &&
                    (ABS(vector.vy) < section->localRadius) &&
                    (ABS(vector.vz) < section->localRadius)) {
                    distance = SquareRoot0((vector.vx * vector.vx) + (vector.vy * vector.vy) + (vector.vz * vector.vz));
                    distance = distance - object->extent;
                    distance -= 1024;
                    if (distance < section->localRadius) {
                        withinLocal = 1;
                    }
                }
                visSection = visSection->prevSection.ptr;
            }
        }

        if (withinLocal) {
            if (objectCount < ObjectListMax) {
                objectList[objectCount++] = allCount;
            } else {
                Error("Radius.c:SceneRadiusCheck: Maximum number of objects exceeded", Fatal);
            }
        }

        object = object->next;
        allCount++;
    }

    section->objects = (s16 *)DAlloc(g_heap, objectCount * sizeof(s16), "Radscn.c", 264);
    if (!section->objects) {
        Error("Error:: radius.c: Not enough memory for section object list", Fatal);
    }

    section->objectCount = objectCount;
    for (j = 0; j < objectCount; j++) {
        section->objects[j] = objectList[j];
    }
}

void SceneRadiusCheck2(Object *objects) {
    TrackSection *section;
    TrackSection *visSection;
    s16 i, j;
    Object *object;
    MATRIX *matrix;
    Vector *sectionCentre;
    s16 objectCount;
    Vector vector;
    s64 distance;
    s16 objectList[ObjectListMax];
    s16 withinLocal;
    s16 withinGlobal;
    s16 allCount;

    object = objects;
    objectCount = 0;
    while (object) {
        object = object->next;
        objectCount++;
    }

    trackObjects = (Object **)DAlloc(g_heap, (objectCount + 1) * sizeof(Object *), "Radscn.c", 316);
    if (!trackObjects) {
        Error("radscn.c::SceneRadiusCheck(): No memory for global object table", Fatal);
    }

    object = objects;
    for (i = 0; i < objectCount; i++) {
        trackObjects[i] = object;
        object = object->next;
    }

    section = track->sections;
    for (i = 0; i < track->sectionCount; i++) {
        switch (frontendOptions_g.numPlayers) {
            case 1:
                g_farClip = 0x4000;
                break;
            case 2:
                section->globalRadius = 0x5FFF;
                section->localRadius = 0x5FFF;
                g_farClip = 0x3000;
                break;
            case 3:
            case 4:
                section->globalRadius = 0x4AA9;
                section->localRadius = 0x4AA9;
                g_farClip = 0x2555;
                break;
        }

        object = objects;
        objectCount = 0;
        allCount = 0;

        while (object) {
            withinLocal = 0;
            withinGlobal = 0;

            matrix = &object->skeleton->relative;
            sectionCentre = &section->centre;

            vector.vx = (matrix->t[0] << 1) - sectionCentre->vx;
            vector.vy = (matrix->t[1] << 1) - sectionCentre->vy;
            vector.vz = (matrix->t[2] << 1) - sectionCentre->vz;

            if (section->flags & Jump) {
                if ((ABS(vector.vx) < (section->globalRadius * 2)) &&
                    (ABS(vector.vy) < (section->globalRadius * 2)) &&
                    (ABS(vector.vz) < (section->globalRadius * 2))) {
                    distance = SquareRoot0((vector.vx * vector.vx) + (vector.vy * vector.vy) + (vector.vz * vector.vz));
                    distance = distance - object->extent;
                    distance -= 1024;
                    if (distance < section->globalRadius) {
                        withinGlobal = 1;
                    }
                }
            } else {
                if ((ABS(vector.vx) < section->globalRadius) &&
                    (ABS(vector.vy) < section->globalRadius) &&
                    (ABS(vector.vz) < section->globalRadius)) {
                    distance = SquareRoot0((vector.vx * vector.vx) + (vector.vy * vector.vy) + (vector.vz * vector.vz));
                    distance = distance - object->extent;
                    distance -= 1024;
                    if (distance < section->globalRadius) {
                        withinGlobal = 1;
                    }
                }
            }

            if (withinGlobal) {
                visSection = section;
                for (j = 0; (!withinLocal) && (j < 30); j++) {
                    matrix = &object->skeleton->relative;
                    sectionCentre = &visSection->centre;

                    vector.vx = (matrix->t[0] << 1) - sectionCentre->vx;
                    vector.vy = (matrix->t[1] << 1) - sectionCentre->vy;
                    vector.vz = (matrix->t[2] << 1) - sectionCentre->vz;

                    if ((ABS(vector.vx) < section->localRadius) &&
                        (ABS(vector.vy) < section->localRadius) &&
                        (ABS(vector.vz) < section->localRadius)) {
                        distance = SquareRoot0((vector.vx * vector.vx) + (vector.vy * vector.vy) + (vector.vz * vector.vz));
                        distance = distance - object->extent;
                        distance -= 1024;
                        if (distance < section->localRadius) {
                            withinLocal = 1;
                        }
                    }
                    visSection = visSection->nextSection.ptr;
                }

                visSection = section;
                for (j = 0; (!withinLocal) && (j < 30); j++) {
                    matrix = &object->skeleton->relative;
                    sectionCentre = &visSection->centre;

                    vector.vx = (matrix->t[0] << 1) - sectionCentre->vx;
                    vector.vy = (matrix->t[1] << 1) - sectionCentre->vy;
                    vector.vz = (matrix->t[2] << 1) - sectionCentre->vz;

                    if ((ABS(vector.vx) < section->localRadius) &&
                        (ABS(vector.vy) < section->localRadius) &&
                        (ABS(vector.vz) < section->localRadius)) {
                        distance = SquareRoot0((vector.vx * vector.vx) + (vector.vy * vector.vy) + (vector.vz * vector.vz));
                        distance = distance - object->extent;
                        distance -= 1024;
                        if (distance < section->localRadius) {
                            withinLocal = 1;
                        }
                    }
                    visSection = visSection->prevSection.ptr;
                }
            }

            if (withinLocal) {
                if (objectCount < ObjectListMax) {
                    objectList[objectCount++] = allCount;
                } else {
                    Error("Radius.c:SceneRadiusCheck: Maximum number of objects exceeded", Fatal);
                }
            }

            object = object->next;
            allCount++;
        }

        section->objects = (s16 *)DAlloc(g_heap, objectCount * sizeof(s16), "Radscn.c", 503);
        if (!section->objects) {
            Error("Error:: radius.c: Not enough memory for section object list", Fatal);
        }

        section->objectCount = objectCount;
        for (j = 0; j < objectCount; j++) {
            section->objects[j] = objectList[j];
        }

        section = NextIndexSection(section);
    }
}
