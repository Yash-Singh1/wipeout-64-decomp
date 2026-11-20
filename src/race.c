/*
 * Race.c — Race resource allocation and per-race setup.
 *
 * InitRaceResources() loads persistent ship, weapon, HUD, and start-droid
 * assets once at boot/mode entry. ResetRaceHeaps() clears the temporary race
 * heap between tracks, and InitRace() loads the selected track WAD, builds
 * display lists, initializes AI/player ships, cameras, particles, trails,
 * and challenge medals.
 */
#include "race.h"

#ident "$Revision: 1.17 $"
#ident "$Revision: 7.27 $"
#ident "$Revision: 1.5 $"

/* Allocates persistent race assets (ships, HUD sprites, weapons, effects, droids). */
void InitRaceResources(void) {
    u32 i;
    SpriteImage *image;

    InitEffects();
    ships = DAlloc(g_heap, 0x1158, "Race.c", 1706);
    LoadShipShapes(0, ships, shipShapes, cameras[0].camPos);
    InspectLoadAiShips();
    InspectLoadAiDamage();
    for (i = 0; i < NO_OF_HUD_SPRITES; i++) {
        image = spriteTextures[spriteTextureBase + i];
        guSprite2DInit(&hudSprites[i], (u8 *)image->pixels - K0BASE, (u8 *)image->palette - K0BASE,
                       image->width, image->width, image->height, G_IM_FMT_CI, image->is8Bit != 0,
                       0, 0);
    }

    g_startupImage = OpenWadFileNvc((u32)_startupImageFile);
    InitSpeedoSprites(g_hudTimers, g_hudLaps);
    g_startupImage->current += 2;
    g_effectTexStart = TextureTableCount;
    g_startupImage->current += 1;
    InitShipEffects(g_shipEffectA, g_shipEffectB);

    weaponShapes = DAlloc(g_heap, 0x200, "Race.c", 1777);
    weaponData = DAlloc(g_heap, 0xEC0, "Race.c", 1778);
    InitWeapons(weaponShapes, cameras[0].camPos, weaponData, g_weaponTargetData);
    InitRenderList(g_miscRenderListC, 0, 1);
    InitRenderList(g_trailRenderList, 0, 1);
    InitRenderList(g_miscRenderListB, 0, 1);
    InitRenderList(g_miscRenderListA, 0, 1);
    LoadExplodeGFX();
    LoadStartDroid(startDroidObjects, "sroid", cameras[0].camPos, checkPoints, &gameData);
    LoadStartDroid(lightGroups, "light", cameras[0].camPos, checkPoints, &gameData);
}

/* Reinitializes the temporary race heap and redirects all heap pointers to it. */
void ResetRaceHeaps(void) {
    DInit(g_tempHeap, TEMP_HEAP_SIZE);
    TextureTableCount = 0;
    g_heap = g_heapA = g_heapB = g_tempHeap;
}

/* Loads the active track WAD and initializes all per-race game systems. */
void InitRace(void) {
    s16 prmCount;
    WADFILE *startup;
    WADFILE *wad;
    Object *obj;
    s32 i;
    s16 textureStart;

    prmCount = 0;
    RaceCompleteDList = DAlloc(g_heap, 0x5DE0, "Race.c", 1968);
    if (RaceCompleteDList == NULL) {
        __assert("RaceCompleteDList", "Race.c", 1969);
    }
    PauseDList = DAlloc(g_heap, 0xFC0, "Race.c", 1971);
    if (PauseDList == NULL) {
        __assert("PauseDList", "Race.c", 1972);
    }

    TextureTableCount = 0;
    startup = OpenWadFileNvc((u32)_startupDataFile);
    gameData.unk2E = 1;
    tempGameData = &gameData;
    g_raceSectionResetFlag = 0;
    gameData.shipClass = frontendOptions_g.teamClass;
    g_shipClassIndex = gameData.shipClass;
    gameData.actTrackNum = frontendOptions_g.trackNo;
    gameData.trackNum = gameData.actTrackNum - 1;
    g_trackIndex = gameData.trackNum;
    if (g_gameType == 1) {
        if (frontendOptions_g.trackNo != g_prevTrackNo || g_prevTeamClass != frontendOptions_g.teamClass ||
            g_prevTeam != frontendOptions_g.team) {
            g_ghostLapTime0 = 0;
            g_ghostResetFlag = 1;
        }
        g_prevTrackNo = frontendOptions_g.trackNo;
        g_prevTeamClass = frontendOptions_g.teamClass;
        g_prevTeam = frontendOptions_g.team;
    }

    track = DAlloc(g_heap, sizeof(TrackData), "Race.c", 2009);
    if (track == NULL) {
        __assert("track", "Race.c", 2010);
    }
    wad = OpenWadFile(frontendOptions_g.trackFile);
    wad->current += 6;
    g_multiplayerFlag = frontendOptions_g.numPlayers >= 2;
    g_sceneObjectCount = WADInitScene(prmObjects, &prmCount, cameras[0].camPos, &gameData, wad);
    InitSky();
    textureStart = TextureTableCount;
    wad->current = 4;
    LoadCmpFiles(WADLoadCompressedTextureSequence(wad));
    g_trackTextureCount = TextureTableCount - textureStart;
    InitRenderList(g_trackRenderList, textureStart, g_trackTextureCount);
    wad->current = 1;
    WADInitFaces(wad);
    WADInitSections(wad);
    WADInitViewList(wad);
    wad->current = 0;
    WADInitVertices(wad);
    wad->current = 4;
    InitTrackDisplayLists();

    /* Temporarily splice both scenery object lists so SceneRadiusCheck2 bins them together. */
    obj = prmObjects[g_scenePrmIndex];
    while (obj->next != NULL) {
        obj = obj->next;
    }
    obj->next = prmObjects[g_scene2PrmIndex];
    SceneRadiusCheck2(prmObjects[g_scenePrmIndex]);
    obj->next = NULL;
    CloseWadFile(wad);

    for (i = 0; i < NO_OF_CHECKPOINTS; i++) {
        checkPoints[i].bonusSeconds = 0;
        checkPoints[i].section = -1;
        checkPoints[i].done = -1;
    }

    noSpecialGrids = 0;
    weaponGrid = DAlloc(g_heap, 0x420, "Race.c", 2107);
    if (weaponGrid == NULL) {
        __assert("weaponGrid", "Race.c", 2108);
    }
    InitWeaponGrids(weaponGrid, track, &noWeaponGrids, &noSpecialGrids, checkPoints, &gameData);
    InitCheckPoints(checkPoints, &gameData);
    g_raceLineFlags = 0;
    InitRaceLine(track, &gameData);

    /* Initialize per-track animated scenery objects (carts, steam vents, claws, flames). */
    switch (frontendOptions_g.trackNo) {
        case 2:
            SetupTrack2Anim();
            break;
        case 5:
            SetupTrack5Anim();
            break;
        case 4:
            SetupTrack4Anim();
            break;
        case 3:
            SetupTrack3Anim();
            break;
        case 6:
            SetupTrack6Anim();
            break;
    }

    initShipData(track->sections, ships, &gameData);
    if (g_gameType == 1) {
        InitShipShadows(ownShip);
    } else if (g_gameType == 3) {
        InitShipRenderLists();
    } else if (ownShip >= 9 && ownShip <= 11) {
        InitShipShadows(ownShip);
    }
    InitDroidThings(startDroidObjects, "sroid", cameras[0].camPos, checkPoints, &gameData);
    InitDroidThings(lightGroups, "light", cameras[0].camPos, checkPoints, &gameData);
    SetUnitVectors(ships);
    for (i = 0; i < frontendOptions_g.numPlayers; i++) {
        InitRescueDroid(&rescueData[i], shipShapes, &ships[frontendOptions_g.ownShips[i]]);
    }

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

    InitRaceResetData(resetData, ships, &gameData, weaponData, weaponShapes, rescueData, shipShapes,
                      checkPoints);
    for (i = 0; i < NO_OF_DAMAGE_SLOTS; i++) {
        shipDamage[i] = 0;
    }
    InitTrails();
    particleSlots = InitParticle();
    InitExplode();
    InitRing();
    ResetWeapons();
    pauseData.menu = 2;
    pauseData.selection = 0;
    ResetRace(resetData, &pauseData, g_attractMode);
    if (g_attractMode == 1) {
        StartMusic();
    }
    g_raceTimer0 = 0;
    g_raceTimer1 = 0;
    g_challengeMedalLevel = 0;

    if (g_gameType == 6) {
        switch (frontendOptions_g.challengeMedal) {
            case 3:
                startup->current = 14;
                break;
            case 1:
                startup->current = 15;
                break;
            case 2:
                startup->current = 16;
                break;
            case 4:
                startup->current = 17;
                break;
        }
        challengeMedalObject = WADLoadPrm(startup, g_shipRenderList);
        if (challengeMedalObject == NULL) {
            __assert("challengeMedalObject", "Race.c", 2340);
        }
        challengeMedalObject->skeleton->super = cameras[0].camPos;
    } else {
        challengeMedalObject = NULL;
    }
    CloseWadFileNvc(startup);
    ResetEffects();
}
