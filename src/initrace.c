/*
 * Initrace.c — Checkpoint, weapon grid, attract mode, and racing-line setup.
 *
 * Populates checkpoint bonus times, weapon/special track grids, attract-mode
 * timeout (1800 frames USA vs 1500 frames Europe), and the AI racing-line
 * distance table.
 */
#include "initrace.h"

void InitCheckPoints(CheckPoints *checkPoints, ConfigData *config) {
    char suffix[8]; /* not referenced, but the retail frame still reserves it */
    u32 fileOffset;
    s32 fileSize;
    char *fileBuffer;
    s16 checkPoint;

    /* An unknown class leaves fileOffset unset, as the retail code does. */
    switch (config->shipClass) {
        case 0:
            fileOffset = g_romFileOffsets[g_trackFileIds[frontendOptions_g.trackNo - 1].f2];
            break;
        case 1:
            fileOffset = g_romFileOffsets[g_trackFileIds[frontendOptions_g.trackNo - 1].f3];
            break;
        case 2:
            fileOffset = g_romFileOffsets[g_trackFileIds[frontendOptions_g.trackNo - 1].f4];
            break;
        case 3:
            fileOffset = g_romFileOffsets[g_trackFileIds[frontendOptions_g.trackNo - 1].f5];
            break;
    }

    noCheckPoints = 0;
    fileSize = FileLength(fileOffset);
    if (fileSize <= 0) {
        noCheckPoints = 1;
        return;
    }

    fileBuffer = DAlloc(g_heap, fileSize + 32, "Initrace.c", 121);
    if (!fileBuffer) {
        sprintf(errorString, "initrace.c : InitCheckPoints (): Failed to allocate memory for file \n");
        Error(errorString, Fatal);
    }
    if (LoadFile(fileOffset, fileBuffer) != fileSize) {
        sprintf(errorString, "initrace.c:InitCheckPoints(): Failed to load file \n");
        Error(errorString, Fatal);
    }

    __builtin_memcpy(checkPoints, fileBuffer, MAX_CHECKPOINTS * sizeof(CheckPoints));
    for (checkPoint = 0; checkPoint < MAX_CHECKPOINTS; checkPoint++) {
        checkPoints[checkPoint].reached = -1;
        checkPointReached[checkPoint][0] = -1;
        checkPointReached[checkPoint][1] = -1;
        checkPointReached[checkPoint][2] = -1;
        checkPointReached[checkPoint][3] = -1;
    }
    DFree(g_heap, fileBuffer);
}

void InitWeaponGrids(WeaponGrid *weaponGrids, TrackData *trackData, s32 *gridCount, s16 *specialCount,
                     SpecialGrid *specialGrids) {
    TrackSection *section;
    TrackSection *original;
    s32 number;
    s16 more;
    s16 branch;

    section = trackData->sections;
    more = 1;
    branch = 1;
    number = 0;
    *gridCount = 0;
    roadSectionCount = 0;
    do {
        section->secNo = number;
        *gridCount = SetTrackAttributes(section, number, weaponGrids, *gridCount, specialCount, specialGrids);
        if (section->junction.index != -1 && (section->junction.ptr->flags & JunctionStart)) {
            original = section;
            section = section->junction.ptr;
            while (branch) {
                number++;
                section->secNo = number;
                *gridCount = SetTrackAttributes(section, number, weaponGrids, *gridCount, specialCount,
                                                specialGrids);
                section = section->nextSection.ptr;
                if (section->junction.index != -1) {
                    branch = 0;
                }
            }
            branch = 1;
            section = original->nextSection.ptr;
            number++;
            section->secNo = number;
            *gridCount = SetTrackAttributes(section, number, weaponGrids, *gridCount, specialCount, specialGrids);
            roadSectionCount++;
            if (section == trackData->sections) {
                more = 0;
            }
        }
        section = section->nextSection.ptr;
        roadSectionCount++;
        number++;
        if (section == trackData->sections) {
            more = 0;
        }
    } while (more);
    weaponGridSections = number;
}

void InitAttractMode(s32 mode, ShipData *ships) {
    if (mode == 1) {
        CameraUpdate = UpdateCameraAttractMode;
        InitCamera(&cameras[0], ships[ownShip].nearTrkSect);
        ships[ownShip].update = UpdateDroneShipStart;
        ships[ownShip].attr &= ~VIEW_IN;
        ships[ownShip].attr &= ~RACING;
        if (frontendOptions_g.numPlayers >= 2) {
            CameraUpdate2 = UpdateCameraAttractMode;
            InitCamera(&cameras[1], ships[ownShip2].nearTrkSect);
            ships[ownShip2].update = UpdateDroneShipStart;
            ships[ownShip2].attr &= ~VIEW_IN;
            ships[ownShip2].attr &= ~RACING;
        }
        if (frontendOptions_g.numPlayers >= 3) {
            CameraUpdate3 = UpdateCameraAttractMode;
            InitCamera(&cameras[2], ships[ownShip3].nearTrkSect);
            ships[ownShip3].update = UpdateDroneShipStart;
            ships[ownShip3].attr &= ~VIEW_IN;
            ships[ownShip3].attr &= ~RACING;
        }
        if (frontendOptions_g.numPlayers >= 4) {
            CameraUpdate4 = UpdateCameraAttractMode;
            InitCamera(&cameras[3], ships[ownShip4].nearTrkSect);
            ships[ownShip4].update = UpdateDroneShipStart;
            ships[ownShip4].attr &= ~VIEW_IN;
            ships[ownShip4].attr &= ~RACING;
        }
        attractDelayFrames = ATTRACT_FRAMES;
    }
}

void InitRaceData(PauseMenu *race) {
    s32 i;

    race->selection = -1;
    race->showOptions = 1;
    race->stage = 2;
    race->musicPaused = 0;
    race->confirmed = 2;
    race->confirmChoice = 0;
    race->command = -1;
    race->frameCount = 0;
    race->count = 0;
    race->pad14 = 0;
    race->active = 0;
    for (i = MAX_PLAYERS - 1; i >= 0; i--) {
        pauseRequests[i] = 0;
    }
}

void InitRaceResetData(ResetData *reset, ShipData *ships, ConfigData *config, WeaponData *weapons,
                       WeaponShape *weaponShapes, RescueData *rescue, Object **shipShapes,
                       CheckPoints *checkPoints) {
    reset->ships = ships;
    reset->config = config;
    reset->weapons = weapons;
    reset->weaponShapes = weaponShapes;
    reset->rescue = rescue;
    reset->shipShapes = shipShapes;
    reset->checkPoints = checkPoints;
}

void InitRaceLine(TrackData *trackData, ConfigData *config) {
    s16 sections[732];
    Vector vertex;
    s16 active = 1;
    s16 start[24] = { 0, 0x80, 0x80, 0x80, 0x80, 0xA4, 0x80, -0x18, 0x18, 0, 0, 0,
                      0, 0x40, 0, 0, 0, 0x30, 0, 0, 0x58, 0, 0, 0 };
    TrackSection *section;
    TrackSection *saved;
    s16 *list;
    RaceLine *line;
    Face *face;
    s16 more;
    s32 distance;
    s32 branchDistance;
    s32 mainDistance;
    s32 branchPosition;
    s32 mainPosition;
    s32 branchStep;
    s32 mainStep;
    s32 minDistance;
    s32 i;
    s16 offset;
    s16 value;

    section = trackData->sections;
    list = sections;
    distance = 0;
    line = raceLines;
    while (active) {
        line[section->secNo].distance = distance;
        line[section->secNo].x = 0;
        line[section->secNo].y = 0;
        line[section->secNo].z = 0;
        *list++ = section->secNo;
        if (section->junction.index != -1 && (section->junction.ptr->flags & JunctionStart)) {
            saved = section;
            section = saved->junction.ptr;
            branchDistance = 0;
            branchPosition = distance;
            mainPosition = distance;
            mainDistance = 0;
            more = 1;
            do {
                section = section->nextSection.ptr;
                branchDistance++;
                if (section->junction.index != -1) {
                    more = 0;
                }
            } while (more);
            /* Stepping from saved through section (here and below) is what the retail
               schedule and register allocation need. */
            section = saved;
            section = section->nextSection.ptr;
            more = 1;
            mainDistance++;
            section = section->nextSection.ptr;
            do {
                section = section->nextSection.ptr;
                mainDistance++;
                if (section->junction.index != -1) {
                    more = 0;
                }
            } while (more);
            mainDistance += 2;
            if (mainDistance >= branchDistance) {
                minDistance = branchDistance;
                mainStep = (minDistance * 4) / mainDistance;
                branchStep = 4;
            } else {
                minDistance = mainDistance;
                mainStep = 4;
                branchStep = (minDistance * 4) / branchDistance;
            }
            section = saved;
            section = section->junction.ptr;
            for (i = 0; i < branchDistance; i++) {
                line[section->secNo].distance = branchPosition;
                line[section->secNo].x = 0;
                line[section->secNo].y = 0;
                line[section->secNo].z = 0;
                section = section->nextSection.ptr;
                branchPosition += branchStep;
            }
            section = saved;
            for (i = 0; i < mainDistance; i++) {
                line[section->secNo].distance = mainPosition;
                line[section->secNo].x = 0;
                line[section->secNo].y = 0;
                line[section->secNo].z = 0;
                mainPosition += mainStep;
                *list++ = section->secNo;
                section = section->nextSection.ptr;
            }
            distance += minDistance * 4;
            line[section->secNo].distance = distance;
            line[section->secNo].x = 0;
            line[section->secNo].y = 0;
            line[section->secNo].z = 0;
            *list++ = section->secNo;
            if (section == trackData->sections) {
                active = 0;
            }
        }
        section = section->nextSection.ptr;
        distance += 4;
        if (section == trackData->sections) {
            active = 0;
        }
    }
    *list = -1;
    numTrackSections = distance;

    section = trackData->sections;
    for (i = 0; i < weaponGridSections; i++) {
        face = (Face *)trackData->faces + section->faceStart;
        while (!(face->flags & TRACK_BASE)) {
            face++;
        }
        GetFaceVertex(section, face, 0, &vertex);
        raceLines[section->secNo].x = vertex.vx;
        raceLines[section->secNo].y = vertex.vy;
        raceLines[section->secNo].z = vertex.vz;
        section++;
    }

    offset = start[config->actTrackNum];
    for (i = 0; i < weaponGridSections; i++) {
        value = raceLines[i].distance - offset;
        if (value >= numTrackSections) {
            value -= numTrackSections;
        }
        if (value < 0) {
            value += numTrackSections;
        }
        raceLines[i].distance = value;
    }
}
