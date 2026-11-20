#include "race.h"

#ident "$Revision: 1.17 $"
#ident "$Revision: 7.27 $"
#ident "$Revision: 1.5 $"

void InitRaceResources(void) {
    u32 i;
    SpriteImage *image;

    func_80048124();
    ships = DAlloc(g_heap, 0x1158, "Race.c", 1706);
    LoadShipShapes(0, ships, shipShapes, cameras[0].camPos);
    func_8006F804();
    func_8006F578();
    for (i = 0; i < NO_OF_HUD_SPRITES; i++) {
        image = spriteTextures[spriteTextureBase + i];
        guSprite2DInit(&hudSprites[i], (u8 *)image->pixels - K0BASE, (u8 *)image->palette - K0BASE,
                       image->width, image->width, image->height, G_IM_FMT_CI, image->is8Bit != 0,
                       0, 0);
    }

    g_startupImage = OpenWadFileNvc((u32)_startupImageFile);
    func_8002A490(D_8009BE60, D_800A1048);
    g_startupImage->current += 2;
    D_80091CAC = g_unk8008A970;
    g_startupImage->current += 1;
    func_80048B34(D_800AAF70, D_800AB650);

    weaponShapes = DAlloc(g_heap, 0x200, "Race.c", 1777);
    weaponData = DAlloc(g_heap, 0xEC0, "Race.c", 1778);
    func_800379D8(weaponShapes, cameras[0].camPos, weaponData, D_800AB4F8);
    func_8004F6E4(D_800C4568, 0, 1);
    func_8004F6E4(D_800C45C8, 0, 1);
    func_8004F6E4(D_800C44F0, 0, 1);
    func_8004F6E4(D_800C44A8, 0, 1);
    func_80066B94();
    LoadStartDroid(startDroidObjects, "sroid", cameras[0].camPos, checkPoints, &gameData);
    LoadStartDroid(lightGroups, "light", cameras[0].camPos, checkPoints, &gameData);
}

void ResetRaceHeaps(void) {
    DInit(g_tempHeap, TEMP_HEAP_SIZE);
    g_unk8008A970 = 0;
    g_heap = g_heapA = g_heapB = g_tempHeap;
}

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

    g_unk8008A970 = 0;
    startup = OpenWadFileNvc((u32)_startupDataFile);
    gameData.unk2E = 1;
    tempGameData = &gameData;
    D_8008A998 = 0;
    gameData.shipClass = frontendOptions_g.teamClass;
    D_800A10C0 = gameData.shipClass;
    gameData.actTrackNum = frontendOptions_g.trackNo;
    gameData.trackNum = gameData.actTrackNum - 1;
    D_800A5E30 = gameData.trackNum;
    if (D_800AA024 == 1) {
        if (frontendOptions_g.trackNo != D_8009BA0C || D_8009BA10 != frontendOptions_g.teamClass ||
            D_800AA084 != frontendOptions_g.team) {
            D_80091CC0 = 0;
            D_800A5E46 = 1;
        }
        D_8009BA0C = frontendOptions_g.trackNo;
        D_8009BA10 = frontendOptions_g.teamClass;
        D_800AA084 = frontendOptions_g.team;
    }

    track = DAlloc(g_heap, sizeof(TrackData), "Race.c", 2009);
    if (track == NULL) {
        __assert("track", "Race.c", 2010);
    }
    wad = OpenWadFile(frontendOptions_g.trackFile);
    wad->current += 6;
    D_80096994 = frontendOptions_g.numPlayers >= 2;
    D_800C5BA0 = WADInitScene(prmObjects, &prmCount, cameras[0].camPos, &gameData, wad);
    func_800648F0();
    textureStart = g_unk8008A970;
    wad->current = 4;
    LoadCmpFiles(WADLoadCompressedTextureSequence(wad));
    D_800969A0 = g_unk8008A970 - textureStart;
    func_8004F6E4(D_800C44D8, textureStart, D_800969A0);
    wad->current = 1;
    WADInitFaces(wad);
    WADInitSections(wad);
    WADInitViewList(wad);
    wad->current = 0;
    WADInitVertices(wad);
    wad->current = 4;
    func_800164C0();

    obj = prmObjects[D_80096998];
    while (obj->next != NULL) {
        obj = obj->next;
    }
    obj->next = prmObjects[D_8009699C];
    SceneRadiusCheck2(prmObjects[D_80096998]);
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
    D_800C5910 = 0;
    InitRaceLine(track, &gameData);

    switch (frontendOptions_g.trackNo) {
        case 2:
            func_8004EA38();
            break;
        case 5:
            func_8004EE78();
            break;
        case 4:
            func_8004E008();
            break;
        case 3:
            func_8004D76C();
            break;
        case 6:
            func_8004E188();
            break;
    }

    initShipData(track->sections, ships, &gameData);
    if (D_800AA024 == 1) {
        func_800146CC(ownShip);
    } else if (D_800AA024 == 3) {
        func_80014884();
    } else if (ownShip >= 9 && ownShip <= 11) {
        func_800146CC(ownShip);
    }
    InitDroidThings(startDroidObjects, "sroid", cameras[0].camPos, checkPoints, &gameData);
    InitDroidThings(lightGroups, "light", cameras[0].camPos, checkPoints, &gameData);
    func_8001F078(ships);
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
        D_8009BCE0[i] = 0;
    }
    func_80070F1C();
    particleSlots = InitParticle();
    InitExplode();
    InitRing();
    func_8003809C();
    pauseData.menu = 2;
    pauseData.selection = 0;
    func_8006E880(resetData, &pauseData, D_800AA088);
    if (D_800AA088 == 1) {
        func_80008E5C();
    }
    D_80091C98 = 0;
    D_80091C9C = 0;
    D_80091E04 = 0;

    if (D_800AA024 == 6) {
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
        challengeMedalObject = WADLoadPrm(startup, D_800C45F8);
        if (challengeMedalObject == NULL) {
            __assert("challengeMedalObject", "Race.c", 2340);
        }
        challengeMedalObject->skeleton->super = cameras[0].camPos;
    } else {
        challengeMedalObject = NULL;
    }
    CloseWadFileNvc(startup);
    func_80043BA0();
}
