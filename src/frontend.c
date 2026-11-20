/*
 * frontend.c — Title screen, menus, unlocks, options, and Controller Pak UI.
 *
 * Implements the entire frontend state machine: title/attract flow, single-player,
 * multiplayer (2P-4P), time trial, and challenge menus, league standings and
 * 3D podium/ship previews, audio/controller configuration, cheat unlocks, and
 * full N64 Controller Pak (osPfs*) save/load/ghost management.
 */
#include "frontend.h"

static const char rcsid[] = "$Revision: 1.17 $";

void SetMenuTransition(s32 screen) {
    g_menuTransition.screen = screen;
    g_menuTransition.timer = g_transitionLength;
}

Gfx *DrawMenuFrame(MenuItem *menu, Gfx *glistp, u8 drawHeader) {
    char buf[24];
    char *subtitle;
    char *title;
    u32 i;
    s32 topY;
    s32 bottomY;
    s32 promptY;
    s32 textY;
    s32 titleX;

    subtitle = NULL;
    title = NULL;
    gDPPipeSync(glistp++);
    gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);

    if (g_menuTransition.timer != 0) {
        g_menuTransition.timer--;
        if (g_menuTransition.screen != -1) {
            topY = 320 - g_menuTransition.timer * 240 / g_transitionLength;
            bottomY = g_menuTransition.timer * 340 / g_transitionLength + 480;
            if (g_menuTransition.timer == 0) {
                if (g_menuScreen == 7 && g_audioConfigSfx != NULL) {
                    StopSfx(&g_audioConfigSfx);
                }
                g_menuScreen = g_menuTransition.screen;
                if (g_menuTransition.screen == 5) {
                    g_feResetFlag = 0;
                }
                if (g_menuTransition.screen == 35) {
                    InitCredits();
                }
                g_menuTransition.screen = -1;
                g_menuTransition.timer = g_transitionLength;
            }
        } else {
            topY = g_menuTransition.timer * 240 / g_transitionLength + 80;
            bottomY = 820 - g_menuTransition.timer * 340 / g_transitionLength;
            if (g_menuTransition.timer == 0) {
                SetShake(15, 0);
                PlayNote(10, 255, 15, 15, 256);
            }
        }
    } else {
        topY = 80;
        bottomY = 820;
    }

    if (drawHeader) {
        for (i = 9; i < 18; i++) {
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_menuSprites[i]));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, 70 + (i - 9) * 128, topY);
        }
        for (i = 0; i < 9; i++) {
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_menuSprites[i]));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, 70 + i * 128, bottomY);
        }
    }

    promptY = 210 - ((820 - bottomY) >> 2);

    if (menu[g_menuItem].nextScreen != -1) {
        if (g_menuScreen == 22 || g_menuScreen == 13 || g_menuScreen == 14 || g_menuScreen == 15 ||
            g_menuScreen == 16 || g_menuScreen == 36) {
            glistp = DrawButtonPrompt(40, promptY, 16, "CONTINUE", glistp);
        } else if (g_menuScreen == 6 || g_menuScreen == 7 || g_menuScreen == 25) {
            glistp = DrawButtonPrompt(40, promptY, 16, "ACCEPT", glistp);
        } else {
            glistp = DrawButtonPrompt(40, promptY, 16, "SELECT", glistp);
        }
    }

    if (menu[g_menuItem].prevScreen != -1) {
        if (g_menuScreen == 22) {
            glistp = DrawButtonPrompt(212, promptY, 17, "QUIT LEAGUE", glistp);
        } else {
            glistp = DrawButtonPrompt(275, promptY, 17, "BACK", glistp);
        }
    }

    if (drawHeader) {
        titleX = topY + 20;
        if (g_leagueActive) {
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_menuSprites[23]));
        } else {
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_menuSprites[22]));
        }
        gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
        gSPSprite2DDraw(glistp++, 1044, titleX);

        textY = ((topY - 80) >> 2) + 42;

        switch (g_menuScreen) {
            case 0:
            case 38:
                break;
            case 22:
                subtitle = StringAlloc("MULTI PLAYER");
                title = StringAlloc("F5OOO LEAGUE");
                break;
            case 36:
                subtitle = StringAlloc("MULTI PLAYER");
                title = StringAlloc("FINAL STANDINGS");
                break;
            case 24:
                subtitle = StringAlloc("LOAD AND");
                title = StringAlloc("SAVE DATA");
                break;
            case 23:
                subtitle = StringAlloc("LOAD DATA");
                break;
            case 1:
                title = StringAlloc("GAME SELECT");
                break;
            case 2:
                title = StringAlloc("TEAM SELECT");
                break;
            case 3:
                title = StringAlloc("CLASS SELECT");
                break;
            case 28:
            case 29:
            case 30:
            case 31:
            case 32:
            case 33:
            case 34:
                title = StringAlloc("BONUS");
                break;
            case 4:
                title = StringAlloc("TRACK SELECT");
                break;
            case 8:
                subtitle = StringAlloc("CHALLENGE");
                title = StringAlloc("SELECT");
                break;
            case 9:
                subtitle = StringAlloc("RACE");
                title = StringAlloc("CHALLENGES");
                break;
            case 10:
                subtitle = StringAlloc("TIME TRIAL");
                title = StringAlloc("CHALLENGES");
                break;
            case 11:
                subtitle = StringAlloc("WEAPON");
                title = StringAlloc("CHALLENGES");
                break;
            case 12:
                subtitle = StringAlloc("SUPER COMBO");
                title = StringAlloc("CHALLENGES");
                break;
            case 13:
                if (frontendOptions_g.challengeIndex == 6) {
                    subtitle = StringAlloc("RACE GOLD");
                    title = StringAlloc("CHALLENGE");
                } else {
                    subtitle = StringAlloc("RACE");
                    sprintf(buf, "CHALLENGE %i", frontendOptions_g.challengeIndex + 1);
                    title = StringAlloc(buf);
                }
                break;
            case 14:
                if (frontendOptions_g.challengeIndex == 6) {
                    subtitle = StringAlloc("TIME TRIAL GOLD");
                    title = StringAlloc("CHALLENGE");
                } else {
                    subtitle = StringAlloc("TIME TRIAL");
                    sprintf(buf, "CHALLENGE %i", frontendOptions_g.challengeIndex + 1);
                    title = StringAlloc(buf);
                }
                break;
            case 15:
                if (frontendOptions_g.challengeIndex == 6) {
                    subtitle = StringAlloc("WEAPON GOLD");
                    title = StringAlloc("CHALLENGE");
                } else {
                    subtitle = StringAlloc("WEAPON");
                    sprintf(buf, "CHALLENGE %i", frontendOptions_g.challengeIndex + 1);
                    title = StringAlloc(buf);
                }
                break;
            case 16:
                if (frontendOptions_g.challengeIndex == 6) {
                    subtitle = StringAlloc("SUPER COMBO");
                    title = StringAlloc("GOLD CHALLENGE");
                } else {
                    subtitle = StringAlloc("SUPER COMBO");
                    sprintf(buf, "CHALLENGE %i", frontendOptions_g.challengeIndex + 1);
                    title = StringAlloc(buf);
                }
                break;
            case 17:
            case 18:
            case 19:
            case 20:
                subtitle = StringAlloc("CHALLENGE");
                title = StringAlloc("TEAM SELECTION");
                break;
            case 5:
                subtitle = StringAlloc("MULTIPLAYER");
                title = StringAlloc("MALARKY");
                break;
            case 21:
                title = StringAlloc("OPTIONS");
                break;
            case 25:
                title = StringAlloc("GAME CONFIG");
                break;
            case 27:
                subtitle = StringAlloc("TIME TRIAL");
                title = StringAlloc("RECORDS");
                break;
            case 26:
                subtitle = StringAlloc("RACE");
                title = StringAlloc("RECORDS");
                break;
            case 6:
                subtitle = StringAlloc("CONTROLLER");
                title = StringAlloc("CONFIG");
                break;
            case 35:
                subtitle = StringAlloc("WIPEOUT 64");
                title = StringAlloc("CREDITS");
                break;
            case 7:
                title = StringAlloc("AUDIO CONFIG");
                break;
        }

        glistp = DrawHudText(title, 30, textY, 3, 1, glistp);
        DFree(g_heap, title);
        if (subtitle != NULL) {
            glistp = DrawHudText(subtitle, 30, textY - 14, 3, 1, glistp);
            DFree(g_heap, subtitle);
        }
    }

    gDPPipeSync(glistp++);
    return glistp;
}

Gfx *AddFrontendDisplayList(Gfx *glistp) {
    s32 length;

    length = g_feDlistEnd - g_feDlistStart;
    if (length > MaxLength) {
        MaxLength = length;
        ASSERT(MaxLength < FRONTEND_DLIST_SIZE, 1401);
    }
    gSPDisplayList(glistp++, g_feDlistStart);
    return glistp;
}

void CreateFrontendGfxTask(s32 frameIdx) {
    GFXInfo *i;
    Dynamic *dp;
    Gfx *glistp;
    Gfx *taskStart;
    Gfx *task3DStart;
    u16 perspNorm;
    s32 vpBase;
    s32 player;
    s32 delay;
    f64 aspectWidth;
    f64 aspectHeight;

    switch (g_multiNumPlayers) {
        case 0:
        case 1:
            vpBase = 0;
            break;
        case 2:
            if (g_multiSplitVertical == 1) {
                vpBase = 1;
            } else {
                vpBase = 3;
            }
            break;
        case 3:
        case 4:
            vpBase = 5;
            break;
    }

    i = &g_gfxInfo[frameIdx];
    glistp = i->dp.glist;
    i->active = 1;

    {
        Gfx *start = glistp;
        gDPFullSync(glistp++);
        gSPEndDisplayList(glistp++);
        SetupGfxTask(i, 6, start, glistp);
    }

    taskStart = glistp;
    g_mtxCount = 0;
    g_mtxPool = i->dp.mtxPool;

    gSPSegment(glistp++, 0, 0x0);
    gSPSegment(glistp++, STATIC_SEGMENT, OS_K0_TO_PHYSICAL(g_staticSegment));
    gSPSegment(glistp++, CFB_SEGMENT, OS_K0_TO_PHYSICAL(i->cfb));

    dp = &i->dp;
    if (g_multiNumPlayers == 2) {
        aspectWidth = 2.0;
        if (g_multiSplitVertical != 1) {
            aspectWidth = 8.0;
        }
        aspectHeight = 3.0;
    } else {
        aspectWidth = 4.0;
        aspectHeight = 3.0;
    }

    guPerspective(&dp->projection, &perspNorm, 82.224f, aspectWidth / aspectHeight, 16.0f, (f32)g_farClip, 1.0f);
    guMtxCatL(&dp->projection, &g_flipYMtx, &dp->projection);

    guOrtho(&dp->ortho, -160.0f, 160.0f, -120.0f, 120.0f, 0.0f, 4000.0f, 1.0f);
    guMtxCatL(&dp->ortho, &g_flipYMtx, &dp->ortho);

    gSPDisplayList(glistp++, g_rdpInitDL);
    gSPViewport(glistp++, &g_viewports[0]);
    gSPDisplayList(glistp++, g_rspInitDL);
    gSPDisplayList(glistp++, g_clearZBufferDL);
    gSPDisplayList(glistp++, g_clearCfbDL);
    gDPFullSync(glistp++);
    gSPEndDisplayList(glistp++);
    SetupGfxTask(i, 4, taskStart, glistp);

    {
        Gfx *start = glistp;
        gDPFullSync(glistp++);
        gSPEndDisplayList(glistp++);
        SetupGfxTask(i, 3, start, glistp);
    }

    {
        Gfx *start = glistp;
        gDPFullSync(glistp++);
        gSPEndDisplayList(glistp++);
        SetupGfxTask(i, 1, start, glistp);
    }

    task3DStart = glistp;
    gSPSegment(glistp++, 0, 0x0);
    gSPSegment(glistp++, STATIC_SEGMENT, OS_K0_TO_PHYSICAL(g_staticSegment));
    gSPSegment(glistp++, CFB_SEGMENT, OS_K0_TO_PHYSICAL(i->cfb));
    gSPDisplayList(glistp++, g_rdpInitDL);
    gSPDisplayList(glistp++, g_rspInitDL);
    gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&dp->projection), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPDisplayList(glistp++, g_initDls);
    gSPPerspNormalize(glistp++, perspNorm);
    gSPViewport(glistp++, &g_viewports[0]);
    gDPSetRenderMode(glistp++, G_RM_ZB_OPA_SURF, G_RM_ZB_OPA_SURF2);

    switch (g_menuScreen) {
        case 36:
            glistp = RenderFinalStandings3D(dp, glistp);
            break;
        case 29:
            glistp = RenderMenu3DModel(g_bonusMenu, glistp);
            break;
        case 1:
            glistp = RenderMenu3DModel(g_mainMenu, glistp);
            break;
        case 2:
            glistp = RenderMenu3DModel(g_teamMenu, glistp);
            break;
        case 3:
            glistp = RenderMenu3DModel(g_classMenu, glistp);
            break;
        case 4:
            glistp = RenderMenu3DModel(g_trackMenu, glistp);
            break;
        case 8:
            glistp = RenderMenu3DModel(g_challengeMenu, glistp);
            break;
        case 9:
            glistp = RenderChallengeMedal3D(g_raceChallengeMenu, dp, 1, glistp);
            break;
        case 10:
            glistp = RenderChallengeMedal3D(g_ttChallengeMenu, dp, 1, glistp);
            break;
        case 11:
            glistp = RenderChallengeMedal3D(g_weaponChallengeMenu, dp, 1, glistp);
            break;
        case 12:
            glistp = RenderChallengeMedal3D(g_superComboChallengeMenu, dp, 1, glistp);
            break;
        case 17:
            glistp = RenderMenu3DModel(g_raceChallengeTeamMenu, glistp);
            break;
        case 18:
            glistp = RenderMenu3DModel(g_ttChallengeTeamMenu, glistp);
            break;
        case 19:
            glistp = RenderMenu3DModel(g_weaponChallengeTeamMenu, glistp);
            break;
        case 20:
            glistp = RenderMenu3DModel(g_superComboChallengeTeamMenu, glistp);
            break;
        case 5:
            for (player = 0; player < g_multiNumPlayers; player++) {
                if (g_multiNumPlayers == 3) {
                    glistp = SetupMultiViewport(player, dp, &perspNorm, glistp);
                } else {
                    gSPViewport(glistp++, &g_viewports[vpBase + player]);
                }
                gDPSetRenderMode(glistp++, G_RM_ZB_OPA_SURF, G_RM_ZB_OPA_SURF2);
                glistp = RenderMultiShipModel(g_multiShipSelectMenu, &g_cameras[player], player, glistp);
            }
            break;
        case 21:
            glistp = RenderMenu3DModel(g_optionsMenu, glistp);
            break;
        case 27:
            glistp = RenderMenu3DModel(g_ttRecordsMenu, glistp);
            break;
        case 26:
            glistp = RenderMenu3DModel(g_raceRecordsMenu, glistp);
            break;
        case 7:
            glistp = RenderMenu3DModel(g_audioConfigMenu, glistp);
            break;
        case 6:
            glistp = RenderController3DModel(g_controllerConfigMenu, glistp);
            break;
        case 24:
            glistp = RenderMenu3DModel(g_loadSaveMenu, glistp);
            break;
    }

    gDPFullSync(glistp++);
    gSPEndDisplayList(glistp++);
    SetupGfxTask(i, 0, task3DStart, glistp);

    {
        Gfx *start = glistp;
        gDPFullSync(glistp++);
        gSPEndDisplayList(glistp++);
        SetupGfxTask(i, 2, start, glistp);
    }

    {
        Gfx *start = glistp;
        gDPFullSync(glistp++);
        gSPEndDisplayList(glistp++);
        SetupGfxTask(i, 5, start, glistp);
    }

    gDPFullSync(glistp++);
    gSPEndDisplayList(glistp++);

    i->glist2D = glistp;
    Gfx2DMenuTask(glistp, i, 0);

    if (g_frameDelayFlag) {
        for (delay = 0; delay < 416; delay++) {
        }
    }

    osWritebackDCache(&i->dp, (u32)glistp - (u32)&i->dp);
}

Gfx *DrawMenu(MenuItem *menu, s32 x, s32 y, Gfx *glistp, u8 drawHeader) {
    char buf[128];
    u8 i;

    for (i = 0; menu[i].row != -1; i++) {
        if (menu[i].hidden) {
            continue;
        }
        if (!menu[i].enabled) {
            glistp = CentreText(menu[i].text, x, y + menu[i].row * 10, 8, 2, glistp);
        } else {
            glistp = CentreText(menu[i].text, x, y + menu[i].row * 10, 8, menu[i].selected, glistp);
        }
    }
    return DrawMenuFrame(menu, glistp, drawHeader);
}

Gfx *MainMenu(Gfx *glistp) {
    s32 result;
    u8 i;

    if (g_velocitarUnlocked) {
        g_trackMenu[6].enabled = TRUE;
        g_trackMenu[6].hidden = FALSE;
    }
    if (g_piranhaUnlocked) {
        g_teamMenu[4].enabled = TRUE;
        g_teamMenu[4].hidden = FALSE;
    }
    if (g_superComboUnlocked) {
        g_gameConfigMenu[5].enabled = TRUE;
        g_gameConfigMenu[5].hidden = FALSE;
    }

    result = MenuControl(g_mainMenu);
    if (result == 1) {
        frontendOptions_g.mode = g_mainMenu[g_menuItem].value;
        g_raceMode = g_mainMenu[g_menuItem].value;
        g_gameType = g_mainMenu[g_menuItem].gameType;
        frontendOptions_g.numPlayers = g_mainMenu[g_menuItem].numValues;
        SetMenuTransition(g_mainMenu[g_menuItem].nextScreen);
        g_attractTimer = 15 * FRAME_RATE;
        frontendOptions_g.leagueRacesLeft = 0;
        if (g_gameType == 3) {
            frontendOptions_g.unk14 = frontendOptions_g.computerShips;
        } else {
            frontendOptions_g.unk14 = 1;
        }
        for (i = 1; i < 5; i++) {
            frontendOptions_g.ownShip[i - 1] = -1;
            frontendOptions_g.screenLayout[i - 1] = -1;
        }
        g_fourPlayerSameTeam = 0;
    }

    glistp = DrawMenu(g_mainMenu, 130, 60, glistp, 1);

    if (g_firstContPad.button == 0) {
        g_attractTimer--;
    } else {
        g_attractTimer = 15 * FRAME_RATE;
    }
    if (g_attractTimer == 0) {
        g_attractTimer = 15 * FRAME_RATE;
        g_menuScreen = 0;
    }
    return glistp;
}

void StartGame(void) {
    s32 i;

    ownShip = -1;
    ownShip2 = -1;
    ownShip3 = -1;
    ownShip4 = -1;

    switch (frontendOptions_g.numPlayers) {
        case 1:
            frontendOptions_g.screenLayout[0] = 0;
            frontendOptions_g.contPort[0] = 0;
            ownShip = frontendOptions_g.ownShip[0];
            break;
        case 2:
            ownShip = frontendOptions_g.ownShip[0];
            ownShip2 = frontendOptions_g.ownShip[1];
            break;
        case 3:
            ownShip = frontendOptions_g.ownShip[0];
            ownShip2 = frontendOptions_g.ownShip[1];
            ownShip3 = frontendOptions_g.ownShip[2];
            break;
        case 4:
            ownShip = frontendOptions_g.ownShip[0];
            ownShip2 = frontendOptions_g.ownShip[1];
            ownShip3 = frontendOptions_g.ownShip[2];
            ownShip4 = frontendOptions_g.ownShip[3];
            break;
    }

    frontendOptions_g.team = frontendOptions_g.ownShip[0];

    ASSERT(frontendOptions_g.numPlayers <= 4, 1943);
    ASSERT(frontendOptions_g.team < 15, 1944);
    ASSERT(frontendOptions_g.teamClass <= 3, 1945);

    for (i = 1; i <= frontendOptions_g.numPlayers; i++) {
        ASSERT(frontendOptions_g.ownShip[i-1] != -1, 1949);
        ASSERT(frontendOptions_g.screenLayout[i-1] != -1, 1950);
    }

    switch (frontendOptions_g.numPlayers) {
        case 4:
            ASSERT((ownShip4 >= 0) && (ownShip4 < 15), 1957);
        case 3:
            ASSERT((ownShip3 >= 0) && (ownShip3 < 15), 1960);
        case 2:
            ASSERT((ownShip2 >= 0) && (ownShip2 < 15), 1963);
            break;
    }
    ASSERT((ownShip >= 0) && (ownShip < 15), 1966);

    ResumeAudio();
    StopAudio();

    for (i = 0; i < 16; i++) {
        g_cheatHistory[i] = 0;
    }
    if (g_gameType != 0) {
        cheatWeapon = 0;
        cheatTime = 0;
        cheatDamage = 0;
    }
    g_appState = 1;
    g_loadDone = 0;
}

u8 MenuControl(MenuItem *menu) {
    u8 ret;
    s32 cur;
    s32 i;
    s32 step;
    u16 prev;

    step = 1;
    ret = 0;

    if (g_firstContPad.stick_x > 30) {
        g_firstContPad.button |= R_JPAD;
    }
    if (g_firstContPad.stick_x < -30) {
        g_firstContPad.button |= L_JPAD;
    }
    if (g_firstContPad.stick_y > 30) {
        g_firstContPad.button |= U_JPAD;
    }
    if (g_firstContPad.stick_y < -30) {
        g_firstContPad.button |= D_JPAD;
    }

    prev = g_prevMenuButtons;
    g_prevMenuButtons = g_firstContPad.button;
    g_menuButtonsPressed = g_firstContPad.button & (g_firstContPad.button ^ prev);

    if (menu == g_leagueMenu && g_menuTransition.screen != 1) {
        g_menuTransition.timer = 0;
        g_menuTransition.screen = 22;
    }

    if (g_menuTransition.timer > 0) {
        g_menuButtonsPressed = 0;
    }

    for (cur = 0; menu[cur].row != -1; cur++) {
        if (menu[cur].selected == 0) {
            g_menuItem = cur;
            break;
        }
    }

    if (g_menuButtonsPressed & U_JPAD) {
        for (i = 0; menu[i].row != -1; i++) {
            if (menu[i].row == menu[cur].row - step) {
                if (menu[i].enabled && !menu[i].hidden) {
                    menu[i].selected = 0;
                    menu[cur].selected = 1;
                    PlayNote(26, 255, 15, 15, 256);
                    g_menuItem = i;
                    break;
                } else {
                    step++;
                    i = -1;
                }
            }
        }
    } else if (g_menuButtonsPressed & L_JPAD) {
        if (menu == g_gameConfigMenu) {
            if (menu[g_menuItem].value > 0) {
                menu[g_menuItem].value--;
                PlayNote(26, 255, 15, 15, 256);
            }
        } else if (menu == g_ttRecordsMenu) {
            if (menu[g_menuItem].value > 0) {
                menu[g_menuItem].value--;
                PlayNote(26, 255, 15, 15, 256);
            }
        } else if (menu == g_raceRecordsMenu) {
            if (menu[g_menuItem].value > 0) {
                menu[g_menuItem].value--;
                PlayNote(26, 255, 15, 15, 256);
            }
        } else if (menu == g_audioConfigMenu && g_menuItem == 0) {
            if (menu[0].value > 0) {
                menu[0].value--;
                PlayNote(26, 255, 15, 15, 256);
            } else {
                menu[0].value = 8;
                PlayNote(26, 255, 15, 15, 256);
            }
        } else if (menu == g_controllerConfigMenu && g_ctrlAnimTimer == 0) {
            if (menu[g_menuItem].value > 0) {
                menu[g_menuItem].value--;
                PlayNote(26, 255, 15, 15, 256);
                g_ctrlAnimTimer = g_ctrlAnimLength;
                g_ctrlAnimDir = 0;
            }
        }
    } else if (g_menuButtonsPressed & R_JPAD) {
        if (menu == g_gameConfigMenu) {
            if (menu[g_menuItem].value < menu[g_menuItem].numValues - 1) {
                menu[g_menuItem].value++;
                PlayNote(26, 255, 15, 15, 256);
            }
        } else if (menu == g_ttRecordsMenu) {
            if ((menu[g_menuItem].value < 5 && g_velocitarUnlocked == 0) ||
                (menu[g_menuItem].value < 6 && (g_velocitarUnlocked != 0 || cheatTrack != 0))) {
                menu[g_menuItem].value++;
                PlayNote(26, 255, 15, 15, 256);
            }
        } else if (menu == g_raceRecordsMenu) {
            if ((menu[g_menuItem].value < 5 && g_velocitarUnlocked == 0) ||
                (menu[g_menuItem].value < 6 && (g_velocitarUnlocked != 0 || cheatTrack != 0))) {
                menu[g_menuItem].value++;
                PlayNote(26, 255, 15, 15, 256);
            }
        } else if (menu == g_audioConfigMenu && g_menuItem == 0) {
            if (menu[0].value < 8) {
                menu[0].value++;
                PlayNote(26, 255, 15, 15, 256);
            } else if (menu[0].value == 8) {
                menu[0].value = 0;
                PlayNote(26, 255, 15, 15, 256);
            }
        } else if (menu == g_controllerConfigMenu && g_ctrlAnimTimer == 0) {
            if (menu[g_menuItem].value < 3) {
                menu[g_menuItem].value++;
                PlayNote(26, 255, 15, 15, 256);
                g_ctrlAnimTimer = g_ctrlAnimLength;
                g_ctrlAnimDir = 1;
            }
        }
    } else if (g_menuButtonsPressed & D_JPAD) {
        for (i = 0; menu[i].row != -1; i++) {
            if (menu[i].row == menu[cur].row + step) {
                if (menu[i].enabled && !menu[i].hidden) {
                    menu[i].selected = 0;
                    menu[cur].selected = 1;
                    PlayNote(26, 255, 15, 15, 256);
                    g_menuItem = i;
                    break;
                } else {
                    step++;
                    i = -1;
                }
            }
        }
    }

    if (g_prevMenuButtons & L_JPAD) {
        if (menu == g_audioConfigMenu) {
            if ((g_menuItem == 1 && menu[1].value > 0) || (g_menuItem == 2 && menu[2].value > 0)) {
                menu[g_menuItem].value--;
            }
        }
    } else if (g_prevMenuButtons & R_JPAD) {
        if (menu == g_audioConfigMenu) {
            if ((g_menuItem == 1 && menu[1].value < 115) || (g_menuItem == 2 && menu[2].value < 115)) {
                menu[g_menuItem].value++;
            }
        }
    }

    if (g_menuButtonsPressed & B_BUTTON) {
        if (menu[g_menuItem].prevScreen != -1) {
            ret = 2;
            if (g_menuScreen != 22 && g_menuScreen != 24 && g_menuScreen != 23) {
                SetMenuTransition(menu[g_menuItem].prevScreen);
            }
            PlayNote(27, 255, 15, 15, 256);
        }
    } else if (g_menuButtonsPressed & A_BUTTON) {
        if (menu[g_menuItem].nextScreen != -1) {
            ret = 1;
            PlayNote(27, 255, 15, 15, 256);
        }
    } else {
        ret = 0;
    }

    return ret;
}

u8 MenuControlMulti(MenuItem *menu, s32 lockVertical) {
    u8 ret;
    s32 cur;
    s32 i;
    s32 step;
    u16 prev;

    step = 1;

    if (g_firstContPad.stick_x > 30) {
        g_firstContPad.button |= R_JPAD;
    }
    if (g_firstContPad.stick_x < -30) {
        g_firstContPad.button |= L_JPAD;
    }
    if (g_firstContPad.stick_y > 30) {
        g_firstContPad.button |= U_JPAD;
    }
    if (g_firstContPad.stick_y < -30) {
        g_firstContPad.button |= D_JPAD;
    }

    prev = g_prevMenuButtons2;
    g_prevMenuButtons2 = g_firstContPad.button;
    g_menuButtonsPressed = g_firstContPad.button & (g_firstContPad.button ^ prev);

    if (menu == g_leagueMenu && g_menuTransition.screen != 1) {
        g_menuTransition.timer = 0;
        g_menuTransition.screen = 22;
    }

    if (g_menuTransition.timer > 0) {
        g_menuButtonsPressed = 0;
    }

    for (cur = 0; menu[cur].row != -1; cur++) {
        if (menu[cur].selected == 0) {
            g_menuItem = cur;
            break;
        }
    }

    if ((g_menuButtonsPressed & U_JPAD) && !lockVertical) {
        for (i = 0; menu[i].row != -1; i++) {
            if (menu[i].row == menu[cur].row - step) {
                if (menu[i].enabled && !menu[i].hidden) {
                    menu[i].selected = 0;
                    menu[cur].selected = 1;
                    PlayNote(26, 255, 15, 15, 256);
                    g_menuItem = i;
                    break;
                } else {
                    step++;
                    i = -1;
                }
            }
        }
    } else if (g_menuButtonsPressed & L_JPAD) {
        if (menu == g_gameConfigMenu) {
            if (menu[g_menuItem].value > 0) {
                menu[g_menuItem].value--;
                PlayNote(26, 255, 15, 15, 256);
            }
        } else if (menu == g_ttRecordsMenu) {
            if (menu[g_menuItem].value > 0) {
                menu[g_menuItem].value--;
                PlayNote(26, 255, 15, 15, 256);
            }
        } else if (menu == g_raceRecordsMenu) {
            if (menu[g_menuItem].value > 0) {
                menu[g_menuItem].value--;
                PlayNote(26, 255, 15, 15, 256);
            }
        } else if (menu == g_audioConfigMenu && g_menuItem == 0) {
            if (menu[0].value > 0) {
                menu[0].value--;
                PlayNote(26, 255, 15, 15, 256);
            } else {
                menu[0].value = 8;
                PlayNote(26, 255, 15, 15, 256);
            }
        } else if (menu == g_controllerConfigMenu && g_ctrlAnimTimer == 0) {
            if (menu[g_menuItem].value > 0) {
                menu[g_menuItem].value--;
                PlayNote(26, 255, 15, 15, 256);
                g_ctrlAnimTimer = g_ctrlAnimLength;
                g_ctrlAnimDir = 0;
            }
        }
    } else if (g_menuButtonsPressed & R_JPAD) {
        if (menu == g_gameConfigMenu) {
            if (menu[g_menuItem].value < menu[g_menuItem].numValues - 1) {
                menu[g_menuItem].value++;
                PlayNote(26, 255, 15, 15, 256);
            }
        } else if (menu == g_ttRecordsMenu) {
            if ((menu[g_menuItem].value < 5 && g_velocitarUnlocked == 0) ||
                (menu[g_menuItem].value < 6 && (g_velocitarUnlocked != 0 || cheatTrack != 0))) {
                menu[g_menuItem].value++;
                PlayNote(26, 255, 15, 15, 256);
            }
        } else if (menu == g_raceRecordsMenu) {
            if ((menu[g_menuItem].value < 5 && g_velocitarUnlocked == 0) ||
                (menu[g_menuItem].value < 6 && (g_velocitarUnlocked != 0 || cheatTrack != 0))) {
                menu[g_menuItem].value++;
                PlayNote(26, 255, 15, 15, 256);
            }
        } else if (menu == g_audioConfigMenu && g_menuItem == 0) {
            if (menu[0].value < 8) {
                menu[0].value++;
                PlayNote(26, 255, 15, 15, 256);
            } else if (menu[0].value == 8) {
                menu[0].value = 0;
                PlayNote(26, 255, 15, 15, 256);
            }
        } else if (menu == g_controllerConfigMenu && g_ctrlAnimTimer == 0) {
            if (menu[g_menuItem].value < 3) {
                menu[g_menuItem].value++;
                PlayNote(26, 255, 15, 15, 256);
                g_ctrlAnimTimer = g_ctrlAnimLength;
                g_ctrlAnimDir = 1;
            }
        }
    } else if ((g_menuButtonsPressed & D_JPAD) && !lockVertical) {
        for (i = 0; menu[i].row != -1; i++) {
            if (menu[i].row == menu[cur].row + step) {
                if (menu[i].enabled && !menu[i].hidden) {
                    menu[i].selected = 0;
                    menu[cur].selected = 1;
                    PlayNote(26, 255, 15, 15, 256);
                    g_menuItem = i;
                    break;
                } else {
                    step++;
                    i = -1;
                }
            }
        }
    }

    if (g_prevMenuButtons2 & L_JPAD) {
        if (menu == g_audioConfigMenu) {
            if ((g_menuItem == 1 && menu[1].value > 0) || (g_menuItem == 2 && menu[2].value > 0)) {
                menu[g_menuItem].value--;
            }
        }
    } else if (g_prevMenuButtons2 & R_JPAD) {
        if (menu == g_audioConfigMenu) {
            if ((g_menuItem == 1 && menu[1].value < 115) || (g_menuItem == 2 && menu[2].value < 115)) {
                menu[g_menuItem].value++;
            }
        }
    }

    if (g_menuButtonsPressed & B_BUTTON) {
        if (menu[g_menuItem].prevScreen != -1) {
            ret = 2;
            if (g_menuScreen != 22 && g_menuScreen != 24 && g_menuScreen != 23) {
                SetMenuTransition(menu[g_menuItem].prevScreen);
            }
            PlayNote(27, 255, 15, 15, 256);
        }
    } else if (g_menuButtonsPressed & A_BUTTON) {
        if (menu[g_menuItem].nextScreen != -1) {
            ret = 1;
            PlayNote(27, 255, 15, 15, 256);
        }
    } else {
        ret = 0;
    }

    return ret;
}

Gfx *TrackSelectMenu(Gfx *glistp) {
    if (MenuControl(g_trackMenu) == 1) {
        frontendOptions_g.trackNo = g_trackMenu[g_menuItem].value;
        frontendOptions_g.trackFile = g_trackMenu[g_menuItem].trackFile;
        if (frontendOptions_g.mode == 3) {
            if (frontendOptions_g.leagueRacesLeft) {
                g_demoMode = 0;
                StartGame();
            } else {
                SetMenuTransition(g_trackMenu[g_menuItem].gameType);
            }
        } else {
            SetMenuTransition(g_trackMenu[g_menuItem].nextScreen);
        }
    }
    return DrawMenu(g_trackMenu, 130, 60, glistp, 1);
}

Gfx *ChallengeSelectMenu(Gfx *glistp) {
    s32 medalCount = 0;
    s32 medalStartX[7] = { 0x24E, 0x212, 0x1D6, 0x19A, 0x15C, 0x118, 0xDC };
    ChallengeData *challenges;
    s32 drawn = 0;
    s32 i;

    if (MenuControl(g_challengeMenu) == 1) {
        SetMenuTransition(g_challengeMenu[g_menuItem].nextScreen);
    }

    switch (g_menuItem) {
        case 0:
            challenges = g_raceChallenges;
            break;
        case 1:
            challenges = g_ttChallenges;
            break;
        case 2:
            challenges = g_weaponChallenges;
            break;
        case 3:
            challenges = g_superComboChallenges;
            break;
    }

    for (i = 0; i < 7; i++) {
        if (challenges[i].award != 0) {
            medalCount++;
        }
    }

    for (i = 0; i < 7; i++) {
        if (challenges[i].award == 1) {
            gDPPipeSync(glistp++);
            gDPSetCombineMode(glistp++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
            gDPSetPrimColor(glistp++, 0, 0, 155, 45, 0, 255);
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(g_medalSprite));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, medalStartX[medalCount - 1] + drawn * 125, 427);
            drawn++;
        } else if (challenges[i].award == 2) {
            gDPPipeSync(glistp++);
            gDPSetCombineMode(glistp++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
            gDPSetPrimColor(glistp++, 0, 0, 255, 255, 255, 255);
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(g_medalSprite));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, medalStartX[medalCount - 1] + drawn * 125, 427);
            drawn++;
        } else if (challenges[i].award == 3) {
            gDPPipeSync(glistp++);
            gDPSetCombineMode(glistp++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
            gDPSetPrimColor(glistp++, 0, 0, 255, 145, 0, 255);
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(g_medalSprite));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, medalStartX[medalCount - 1] + drawn * 125, 427);
            drawn++;
        }
    }

    return DrawMenu(g_challengeMenu, 130, 60, glistp, 1);
}

void ApplyGameConfig(void) {
    frontendOptions_g.weaponsOn = g_gameConfigMenu[0].value;
    frontendOptions_g.defaultView = g_gameConfigMenu[1].value;
    frontendOptions_g.catchUp = g_gameConfigMenu[2].value;
    frontendOptions_g.checkpoints = g_gameConfigMenu[3].value;
    frontendOptions_g.computerShips = g_gameConfigMenu[4].value;

    if (g_gameConfigMenu[5].value == 0) {
        frontendOptions_g.leagueRaces = 1;
    } else if (g_gameConfigMenu[5].value == 1) {
        frontendOptions_g.leagueRaces = 3;
    } else if (g_gameConfigMenu[5].value == 2) {
        frontendOptions_g.leagueRaces = 5;
    } else if (g_gameConfigMenu[5].value == 3) {
        frontendOptions_g.leagueRaces = 7;
    }

    frontendOptions_g.cyclone = g_gameConfigMenu[6].value;
}

void SaveControllerConfig(void) {
    g_savedPadConfig = g_controllerConfigMenu[g_menuItem].value;
}

void RestoreControllerConfig(void) {
    g_controllerConfigMenu[g_menuItem].value = g_savedPadConfig;
    g_padConfigIndex[0] = g_controllerConfigMenu[g_menuItem].value;
}

void ApplyAudioConfig(void) {
    g_savedMusicTrack = g_audioConfigMenu[0].value = frontendOptions_g.musicTrack;
    g_savedMusicVol = frontendOptions_g.musicVol = g_audioConfigMenu[1].value;
    SetMasterMusicVolume((frontendOptions_g.musicVol * 0x7FFF) / 116);
    g_savedSfxVol = frontendOptions_g.sfxVol = g_audioConfigMenu[2].value;
    SetMasterSfxVolume((frontendOptions_g.sfxVol * 0x7FFF) / 116);
}

void RestoreAudioConfig(void) {
    g_audioConfigMenu[0].value = frontendOptions_g.musicTrack = g_savedMusicTrack;
    frontendOptions_g.musicVol = g_audioConfigMenu[1].value = g_savedMusicVol;
    SetMasterMusicVolume((frontendOptions_g.musicVol * 0x7FFF) / 116);
    frontendOptions_g.sfxVol = g_audioConfigMenu[2].value = g_savedSfxVol;
    SetMasterSfxVolume((frontendOptions_g.sfxVol * 0x7FFF) / 116);
}

void RestoreGameConfig(void) {
    g_gameConfigMenu[0].value = frontendOptions_g.weaponsOn;
    g_gameConfigMenu[1].value = frontendOptions_g.defaultView;
    g_gameConfigMenu[2].value = frontendOptions_g.catchUp;
    g_gameConfigMenu[3].value = frontendOptions_g.checkpoints;
    g_gameConfigMenu[4].value = frontendOptions_g.computerShips;

    if (frontendOptions_g.leagueRaces == 1) {
        g_gameConfigMenu[5].value = 0;
    } else if (frontendOptions_g.leagueRaces == 3) {
        g_gameConfigMenu[5].value = 1;
    } else if (frontendOptions_g.leagueRaces == 5) {
        g_gameConfigMenu[5].value = 2;
    } else if (frontendOptions_g.leagueRaces == 7) {
        g_gameConfigMenu[5].value = 3;
    }

    g_gameConfigMenu[6].value = frontendOptions_g.cyclone;
}

Gfx *OptionsMenu(Gfx *glistp) {
    if (MenuControl(g_optionsMenu) == 1) {
        SetMenuTransition(g_optionsMenu[g_menuItem].nextScreen);
    }
    glistp = DrawMenu(g_optionsMenu, 130, 60, glistp, 1);
    return glistp;
}

Gfx *GameConfigMenu(Gfx *glistp) {
    u8 res;
    u8 i;
    s32 colour;
    s32 y;

    g_gameConfigMenu[6].enabled = FALSE;
    if (g_superComboUnlocked || cheatSuperCombo) {
        g_gameConfigMenu[6].enabled = TRUE;
    }

    res = MenuControl(g_gameConfigMenu);
    if (res == 1) {
        SetMenuTransition(g_gameConfigMenu[g_menuItem].nextScreen);
        ApplyGameConfig();
    } else if (res == 2) {
        SetMenuTransition(g_gameConfigMenu[g_menuItem].nextScreen);
        RestoreGameConfig();
    }

    glistp = DrawMenu(g_gameConfigMenu, 130, 60, glistp, 1);

    for (i = 0; i < 7; i++) {
        if (!g_gameConfigMenu[i].enabled) {
            colour = 2;
        } else {
            colour = g_gameConfigMenu[i].selected;
        }

        y = i * 15 + 70;

        switch (i) {
            case 0:
                glistp = DrawHudText("WEAPONS", 50, y, 8, colour, glistp);
                break;
            case 1:
                glistp = DrawHudText("DEFAULT VIEW", 50, y, 8, colour, glistp);
                break;
            case 2:
                glistp = DrawHudText("CATCH UP", 50, y, 8, colour, glistp);
                break;
            case 3:
                glistp = DrawHudText("CHECKPOINTS", 50, y, 8, colour, glistp);
                break;
            case 4:
                glistp = DrawHudText("COMPUTER SHIPS", 50, y, 8, colour, glistp);
                break;
            case 5:
                glistp = DrawHudText("LEAGUE RACES", 50, y, 8, colour, glistp);
                break;
            case 6:
                glistp = DrawHudText("CYCLONE", 50, y, 8, colour, glistp);
                break;
        }

        if (g_gameConfigMenu[i].value == 0) {
            if (i == 1) {
                glistp = DrawHudText("EXT", 235, y, 8, colour, glistp);
            } else if (i == 5) {
                glistp = DrawHudText("1", 235, y, 8, colour, glistp);
            } else {
                glistp = DrawHudText("OFF", 235, y, 8, colour, glistp);
            }
            if (colour == 0 && GetBlinkState()) {
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_arrowRightSprite));
                gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
                gSPSprite2DDraw(glistp++, (g_textEndX + 5) << 2, (y + 2) << 2);
            }
        } else if (g_gameConfigMenu[i].value == 1) {
            if (i == 5) {
                glistp = DrawHudText("3", 235, y, 8, colour, glistp);
                if (colour == 0 && GetBlinkState()) {
                    gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_arrowRightSprite));
                    gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
                    gSPSprite2DDraw(glistp++, (g_textEndX + 5) << 2, (y + 2) << 2);
                    gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_arrowLeftSprite));
                    gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
                    gSPSprite2DDraw(glistp++, 223 << 2, (y + 2) << 2);
                }
            } else {
                if (i == 1) {
                    glistp = DrawHudText("INT", 235, y, 8, colour, glistp);
                } else {
                    glistp = DrawHudText("ON", 235, y, 8, colour, glistp);
                }
                if (colour == 0 && GetBlinkState()) {
                    gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_arrowLeftSprite));
                    gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
                    gSPSprite2DDraw(glistp++, 223 << 2, (y + 2) << 2);
                }
            }
        } else if (g_gameConfigMenu[i].value == 2) {
            if (i == 5) {
                glistp = DrawHudText("5", 235, y, 8, colour, glistp);
            }
            if (colour == 0 && GetBlinkState()) {
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_arrowRightSprite));
                gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
                gSPSprite2DDraw(glistp++, (g_textEndX + 5) << 2, (y + 2) << 2);
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_arrowLeftSprite));
                gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
                gSPSprite2DDraw(glistp++, 223 << 2, (y + 2) << 2);
            }
        } else if (g_gameConfigMenu[i].value == 3) {
            if (i == 5) {
                glistp = DrawHudText("7", 235, y, 8, colour, glistp);
            }
            if (colour == 0 && GetBlinkState()) {
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_arrowLeftSprite));
                gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
                gSPSprite2DDraw(glistp++, 223 << 2, (y + 2) << 2);
            }
        }
    }

    g_gameConfigMenu[6].enabled = FALSE;
    return glistp;
}

u32 GetTrackFileOffset(s32 trackNo) {
    switch (trackNo) {
        case 2:
            return (u32)_track2File;
        case 3:
            return (u32)_track3File;
        case 4:
            return (u32)_track4File;
        case 5:
            return (u32)_track5File;
        case 6:
            return (u32)_track6File;
        case 7:
            return (u32)_track7File;
        case 1:
        default:
            return (u32)_track1File;
    }
}

s32 UnlockNextChallenge(MenuItem *menu, ChallengeData *challenges, s32 idx) {
    s32 unlocked;

    unlocked = 0;
    if (idx < 5 && challenges[idx].award != 0) {
        menu[idx + 1].enabled = TRUE;
        menu[idx + 1].hidden = FALSE;
    }
    if (idx < 6 && menu[idx + 1].enabled && !menu[idx + 1].hidden) {
        unlocked = 1;
    }
    return unlocked;
}

u8 CheckChallengeCategoryComplete(MenuItem *menu, ChallengeData *challenges) {
    u8 unlocked;
    s32 i;

    unlocked = 0;
    if (!challenges[6].unlocked) {
        for (i = 0; i < 6; i++) {
            if (challenges[i].award == 0) {
                return unlocked;
            }
            if (i == 5) {
                challenges[6].unlocked = TRUE;
                unlocked = 1;
                if (challenges == g_weaponChallenges) {
                    g_gameConfigMenu[6].value = 1;
                    frontendOptions_g.cyclone = 1;
                }
            }
        }
    }
    return unlocked;
}

u8 CheckSuperComboUnlocked(MenuItem *menu, ChallengeData *challenges) {
    s32 i;
    u8 unlocked;

    unlocked = 0;
    if (g_challengeMenu[3].hidden) {
        for (i = 0; i < 6; i++) {
            if (g_raceChallenges[i].award == 0) {
                return unlocked;
            }
        }
        for (i = 0; i < 6; i++) {
            if (g_ttChallenges[i].award == 0) {
                return unlocked;
            }
        }
        for (i = 0; i < 6; i++) {
            if (g_weaponChallenges[i].award == 0) {
                return unlocked;
            }
        }
        g_challengeMenu[3].hidden = FALSE;
        unlocked = 1;
    }
    return unlocked;
}

u8 CheckAllGoldAwarded(MenuItem *menu, ChallengeData *challenges) {
    s32 i;
    u8 unlocked;

    unlocked = 0;
    if (!g_leagueActive) {
        for (i = 0; i < 7; i++) {
            if (g_raceChallenges[i].award != 3) {
                return unlocked;
            }
        }
        for (i = 0; i < 7; i++) {
            if (g_ttChallenges[i].award != 3) {
                return unlocked;
            }
        }
        for (i = 0; i < 7; i++) {
            if (g_weaponChallenges[i].award != 3) {
                return unlocked;
            }
        }
        for (i = 0; i < 7; i++) {
            if (g_superComboChallenges[i].award != 3) {
                return unlocked;
            }
        }
        unlocked = 1;
        g_leagueActive = TRUE;
        InitPlasmaPaletteGold();
    }
    return unlocked;
}

u8 CheckGoldChallengeUnlocked(MenuItem *menu, ChallengeData *challenges) {
    u8 unlocked;
    s32 i;

    unlocked = 0;
    if (!challenges[6].goldSeen) {
        for (i = 0; i < 6; i++) {
            if (challenges[i].award != 3) {
                break;
            }
            if (i == 5) {
                menu[6].enabled = TRUE;
                menu[6].hidden = FALSE;
                challenges[6].goldSeen = TRUE;
                unlocked = 1;
            }
        }
    }
    return unlocked;
}

Gfx *RaceChallengeMenu(Gfx *glistp) {
    if (MenuControl(g_raceChallengeMenu) == 1) {
        frontendOptions_g.teamClass = g_raceChallenges[g_menuItem].teamClass;
        frontendOptions_g.trackNo = g_raceChallenges[g_menuItem].trackNo;
        frontendOptions_g.trackFile = GetTrackFileOffset(g_raceChallenges[g_menuItem].trackNo);
        frontendOptions_g.challengeMode = 1;
        frontendOptions_g.challengeIndex = g_menuItem;
        SetMenuTransition(g_raceChallengeMenu[g_menuItem].nextScreen);
        frontendOptions_g.mode = 2;
        g_raceMode = 2;
        g_gameType = 6;
        frontendOptions_g.challengeMenu = g_raceChallengeMenu;
        frontendOptions_g.challengeData = g_raceChallenges;
        frontendOptions_g.numPlayers = 1;
        g_challengeTeamMenuInit = 0;
    }

    if (g_raceChallenges[g_menuItem].award != 0) {
        gDPPipeSync(glistp++);
        gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
        glistp = AddCentreText("CURRENT AWARD", 160, g_awardTextY, 1, glistp);
        switch (g_raceChallenges[g_menuItem].award) {
            case 1:
                glistp = AddCentreText("BRONZE", 160, g_awardTextY + 7, 1, glistp);
                break;
            case 2:
                glistp = AddCentreText("SILVER", 160, g_awardTextY + 7, 1, glistp);
                break;
            case 3:
                glistp = AddCentreText("GOLD", 160, g_awardTextY + 7, 1, glistp);
                break;
        }
    }

    return DrawMenu(g_raceChallengeMenu, 130, 60, glistp, 1);
}

Gfx *TimeTrialChallengeMenu(Gfx *glistp) {
    if (MenuControl(g_ttChallengeMenu) == 1) {
        frontendOptions_g.teamClass = g_ttChallenges[g_menuItem].teamClass;
        frontendOptions_g.trackNo = g_ttChallenges[g_menuItem].trackNo;
        frontendOptions_g.trackFile = GetTrackFileOffset(g_ttChallenges[g_menuItem].trackNo);
        frontendOptions_g.challengeMode = 2;
        frontendOptions_g.challengeIndex = g_menuItem;
        SetMenuTransition(g_ttChallengeMenu[g_menuItem].nextScreen);
        frontendOptions_g.mode = 2;
        g_raceMode = 2;
        g_gameType = 6;
        frontendOptions_g.challengeMenu = g_ttChallengeMenu;
        frontendOptions_g.challengeData = g_ttChallenges;
        frontendOptions_g.numPlayers = 1;
        g_challengeTeamMenuInit = 0;
    }

    if (g_ttChallenges[g_menuItem].award != 0) {
        gDPPipeSync(glistp++);
        gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
        glistp = AddCentreText("CURRENT AWARD", 160, g_awardTextY, 1, glistp);
        switch (g_ttChallenges[g_menuItem].award) {
            case 1:
                glistp = AddCentreText("BRONZE", 160, g_awardTextY + 7, 1, glistp);
                break;
            case 2:
                glistp = AddCentreText("SILVER", 160, g_awardTextY + 7, 1, glistp);
                break;
            case 3:
                glistp = AddCentreText("GOLD", 160, g_awardTextY + 7, 1, glistp);
                break;
        }
    }

    return DrawMenu(g_ttChallengeMenu, 130, 60, glistp, 1);
}

Gfx *WeaponChallengeMenu(Gfx *glistp) {
    if (MenuControl(g_weaponChallengeMenu) == 1) {
        frontendOptions_g.teamClass = g_weaponChallenges[g_menuItem].teamClass;
        frontendOptions_g.trackNo = g_weaponChallenges[g_menuItem].trackNo;
        frontendOptions_g.trackFile = GetTrackFileOffset(g_weaponChallenges[g_menuItem].trackNo);
        frontendOptions_g.challengeMode = 3;
        frontendOptions_g.challengeIndex = g_menuItem;
        SetMenuTransition(g_weaponChallengeMenu[g_menuItem].nextScreen);
        frontendOptions_g.mode = 2;
        g_raceMode = 2;
        g_gameType = 6;
        frontendOptions_g.challengeMenu = g_weaponChallengeMenu;
        frontendOptions_g.challengeData = g_weaponChallenges;
        frontendOptions_g.numPlayers = 1;
        g_challengeTeamMenuInit = 0;
    }

    if (g_weaponChallenges[g_menuItem].award != 0) {
        gDPPipeSync(glistp++);
        gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
        glistp = AddCentreText("CURRENT AWARD", 160, g_awardTextY, 1, glistp);
        switch (g_weaponChallenges[g_menuItem].award) {
            case 1:
                glistp = AddCentreText("BRONZE", 160, g_awardTextY + 7, 1, glistp);
                break;
            case 2:
                glistp = AddCentreText("SILVER", 160, g_awardTextY + 7, 1, glistp);
                break;
            case 3:
                glistp = AddCentreText("GOLD", 160, g_awardTextY + 7, 1, glistp);
                break;
        }
    }

    return DrawMenu(g_weaponChallengeMenu, 130, 60, glistp, 1);
}

Gfx *SuperComboChallengeMenu(Gfx *glistp) {
    if (MenuControl(g_superComboChallengeMenu) == 1) {
        frontendOptions_g.teamClass = g_superComboChallenges[g_menuItem].teamClass;
        frontendOptions_g.trackNo = g_superComboChallenges[g_menuItem].trackNo;
        frontendOptions_g.trackFile = GetTrackFileOffset(g_superComboChallenges[g_menuItem].trackNo);
        frontendOptions_g.challengeMode = 4;
        frontendOptions_g.challengeIndex = g_menuItem;
        SetMenuTransition(g_superComboChallengeMenu[g_menuItem].nextScreen);
        frontendOptions_g.mode = 2;
        g_raceMode = 2;
        g_gameType = 6;
        frontendOptions_g.challengeMenu = g_superComboChallengeMenu;
        frontendOptions_g.challengeData = g_superComboChallenges;
        frontendOptions_g.numPlayers = 1;
        g_challengeTeamMenuInit = 0;
    }

    if (g_superComboChallenges[g_menuItem].award != 0) {
        gDPPipeSync(glistp++);
        gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
        glistp = AddCentreText("CURRENT AWARD", 160, g_awardTextY, 1, glistp);
        switch (g_superComboChallenges[g_menuItem].award) {
            case 1:
                glistp = AddCentreText("BRONZE", 160, g_awardTextY + 7, 1, glistp);
                break;
            case 2:
                glistp = AddCentreText("SILVER", 160, g_awardTextY + 7, 1, glistp);
                break;
            case 3:
                glistp = AddCentreText("GOLD", 160, g_awardTextY + 7, 1, glistp);
                break;
        }
    }

    return DrawMenu(g_superComboChallengeMenu, 130, 60, glistp, 1);
}

Gfx *ClassSelectMenu(Gfx *glistp) {
    if (MenuControl(g_classMenu) == 1) {
        frontendOptions_g.teamClass = g_classMenu[g_menuItem].value;
        SetMenuTransition(g_classMenu[g_menuItem].nextScreen);
    }
    glistp = DrawMenu(g_classMenu, 130, 60, glistp, 1);
    return glistp;
}

Gfx *TeamSelectMenu(Gfx *glistp) {
    if (MenuControl(g_teamMenu) == 1) {
        frontendOptions_g.ownShip[0] = frontendOptions_g.team = g_teamMenu[g_menuItem].value;
        SetMenuTransition(g_teamMenu[g_menuItem].nextScreen);
        g_demoMode = 0;
        StartGame();
    }
    glistp = DrawMenu(g_teamMenu, 130, 60, glistp, 1);
    glistp = DrawShipStats(g_menuItem, glistp);
    return glistp;
}

Gfx *TimeTrialChallengeTeamMenu(Gfx *glistp) {
    if (!g_challengeTeamMenuInit) {
        InitChallengeTeamMenu(g_ttChallengeTeamMenu, g_ttChallenges);
        g_challengeTeamMenuInit = TRUE;
    }
    if (MenuControl(g_ttChallengeTeamMenu) == 1) {
        frontendOptions_g.ownShip[0] = frontendOptions_g.team = g_ttChallengeTeamMenu[g_menuItem].value;
        g_demoMode = 0;
        g_menuScreen = g_ttChallengeTeamMenu[g_menuItem].nextScreen;
        StartGame();
    }
    glistp = DrawMenu(g_ttChallengeTeamMenu, 130, 60, glistp, 1);
    return glistp;
}

void InitChallengeTeamMenu(MenuItem *menu, ChallengeData *challenges) {
    u8 i;

    for (i = 0; i < 5; i++) {
        menu[i].hidden = TRUE;
        menu[i].selected = 1;
    }
    if (challenges[frontendOptions_g.challengeIndex].allowedTeams & 1) {
        menu[0].hidden = FALSE;
        menu[0].selected = 0;
    }
    if (challenges[frontendOptions_g.challengeIndex].allowedTeams & 8) {
        menu[1].hidden = FALSE;
        if (menu[0].selected != 0) {
            menu[1].selected = 0;
        }
    }
    if (challenges[frontendOptions_g.challengeIndex].allowedTeams & 4) {
        menu[2].hidden = FALSE;
        if (menu[0].selected != 0 && menu[1].selected != 0) {
            menu[2].selected = 0;
        }
    }
    if (challenges[frontendOptions_g.challengeIndex].allowedTeams & 2) {
        menu[3].hidden = FALSE;
        if (menu[0].selected != 0 && menu[1].selected != 0 && menu[2].selected != 0) {
            menu[3].selected = 0;
        }
    }
    if (challenges[frontendOptions_g.challengeIndex].allowedTeams & 16) {
        menu[4].hidden = FALSE;
        if (menu[0].selected != 0 && menu[1].selected != 0 && menu[2].selected != 0 && menu[3].selected != 0) {
            menu[4].selected = 0;
        }
    }
}

Gfx *RaceChallengeTeamMenu(Gfx *glistp) {
    if (!g_challengeTeamMenuInit) {
        InitChallengeTeamMenu(g_raceChallengeTeamMenu, g_raceChallenges);
        g_challengeTeamMenuInit = TRUE;
    }
    if (MenuControl(g_raceChallengeTeamMenu) == 1) {
        frontendOptions_g.ownShip[0] = frontendOptions_g.team = g_raceChallengeTeamMenu[g_menuItem].value;
        g_demoMode = 0;
        g_menuScreen = g_raceChallengeTeamMenu[g_menuItem].nextScreen;
        StartGame();
    }
    glistp = DrawMenu(g_raceChallengeTeamMenu, 130, 60, glistp, 1);
    return glistp;
}

Gfx *DrawChallengeCategoryBonus(ChallengeData *challenges, Gfx *glistp) {
    if (challenges == g_raceChallenges) {
        glistp = CentreText("WELL DONE!", 50, 40, 12, 0, glistp);
        glistp = CentreText("YOU HAVE QUALIFIED ON ALL", 50, 70, 8, 1, glistp);
        glistp = CentreText("THE RACE CHALLENGES.", 50, 80, 8, 1, glistp);
        glistp = CentreText("BONUS TRACK UNLOCKED:", 50, 160, 8, 1, glistp);
        glistp = CentreText("VELOCITAR", 50, 180, 12, 0, glistp);
    } else if (challenges == g_ttChallenges) {
        glistp = CentreText("WELL DONE!", 50, 40, 12, 0, glistp);
        glistp = CentreText("YOU HAVE QUALIFIED ON ALL", 50, 70, 8, 1, glistp);
        glistp = CentreText("THE TIME TRIAL CHALLENGES.", 50, 80, 8, 1, glistp);
        glistp = CentreText("BONUS TEAM UNLOCKED:", 50, 160, 8, 1, glistp);
        glistp = CentreText("PIRANHA II", 50, 180, 12, 0, glistp);
    } else if (challenges == g_weaponChallenges) {
        glistp = CentreText("WELL DONE!", 50, 40, 12, 0, glistp);
        glistp = CentreText("YOU HAVE QUALIFIED ON ALL", 50, 70, 8, 1, glistp);
        glistp = CentreText("THE WEAPON CHALLENGES.", 50, 80, 8, 1, glistp);
        glistp = CentreText("NEW WEAPON TECHNOLOGY", 50, 150, 8, 1, glistp);
        glistp = CentreText("UNLOCKED:", 50, 160, 8, 1, glistp);
        glistp = CentreText("CYCLONE", 50, 180, 12, 0, glistp);
        glistp = DrawFontText("ALL WEAPONS CAUSE 100% EXTRA DAMAGE", 65, 200, 8, glistp);
    } else if (challenges == g_superComboChallenges) {
        glistp = CentreText("WELL DONE!", 50, 40, 12, 0, glistp);
        glistp = CentreText("YOU HAVE QUALIFIED ON ALL", 50, 70, 8, 1, glistp);
        glistp = CentreText("THE SUPER COMBO", 50, 80, 8, 1, glistp);
        glistp = CentreText("CHALLENGES.", 50, 90, 8, 1, glistp);
        glistp = CentreText("CAN YOU GET A GOLD", 50, 160, 8, 1, glistp);
        glistp = CentreText("QUALIFICATION ON EVERY", 50, 170, 8, 1, glistp);
        glistp = CentreText("CHALLENGE...", 50, 180, 8, 1, glistp);
    }
    return glistp;
}

Gfx *DrawGoldChallengeBonus(ChallengeData *challenges, Gfx *glistp) {
    if (challenges == g_raceChallenges) {
        glistp = CentreText("WELL DONE!", 50, 40, 12, 0, glistp);
        glistp = CentreText("YOU HAVE GOLD QUALIFICATIONS", 50, 80, 8, 1, glistp);
        glistp = CentreText("ON THE FIRST SIX RACE", 50, 90, 8, 1, glistp);
        glistp = CentreText("CHALLENGES.", 50, 100, 8, 1, glistp);
        glistp = CentreText("BONUS RACE CHALLENGE", 50, 130, 8, 1, glistp);
        glistp = CentreText("UNLOCKED:", 50, 140, 8, 1, glistp);
        glistp = CentreText("GOLD CHALLENGE", 50, 170, 12, 0, glistp);
    } else if (challenges == g_ttChallenges) {
        glistp = CentreText("WELL DONE!", 50, 40, 12, 0, glistp);
        glistp = CentreText("YOU HAVE GOLD QUALIFICATIONS", 50, 80, 8, 1, glistp);
        glistp = CentreText("ON THE FIRST SIX TIME TRIAL", 50, 90, 8, 1, glistp);
        glistp = CentreText("CHALLENGES.", 50, 100, 8, 1, glistp);
        glistp = CentreText("BONUS TIME TRIAL CHALLENGE", 50, 130, 8, 1, glistp);
        glistp = CentreText("UNLOCKED:", 50, 140, 8, 1, glistp);
        glistp = CentreText("GOLD CHALLENGE", 50, 170, 12, 0, glistp);
    } else if (challenges == g_weaponChallenges) {
        glistp = CentreText("WELL DONE!", 50, 40, 12, 0, glistp);
        glistp = CentreText("YOU HAVE GOLD QUALIFICATIONS", 50, 80, 8, 1, glistp);
        glistp = CentreText("ON THE FIRST SIX WEAPON", 50, 90, 8, 1, glistp);
        glistp = CentreText("CHALLENGES.", 50, 100, 8, 1, glistp);
        glistp = CentreText("BONUS WEAPON CHALLENGE", 50, 130, 8, 1, glistp);
        glistp = CentreText("UNLOCKED:", 50, 140, 8, 1, glistp);
        glistp = CentreText("GOLD CHALLENGE", 50, 170, 12, 0, glistp);
    } else if (challenges == g_superComboChallenges) {
        glistp = CentreText("WELL DONE!", 50, 40, 12, 0, glistp);
        glistp = CentreText("YOU HAVE GOLD QUALIFICATIONS", 50, 80, 8, 1, glistp);
        glistp = CentreText("ON THE FIRST SIX SUPER", 50, 90, 8, 1, glistp);
        glistp = CentreText("COMBO CHALLENGES.", 50, 100, 8, 1, glistp);
        glistp = CentreText("BONUS SUPER COMBO", 50, 130, 8, 1, glistp);
        glistp = CentreText("CHALLENGE UNLOCKED:", 50, 140, 8, 1, glistp);
        glistp = CentreText("GOLD CHALLENGE", 50, 170, 12, 0, glistp);
    }
    return glistp;
}

Gfx *DrawSuperComboBonus(ChallengeData *challenges, Gfx *glistp) {
    glistp = CentreText("SUPER COMBO", 50, 30, 12, 0, glistp);
    glistp = CentreText("CHALLENGES OPENED!", 50, 50, 12, 0, glistp);
    glistp = CentreText("WELL DONE FSOOO PILOT", 50, 80, 8, 1, glistp);
    glistp = CentreText("YOU HAVE PROVED YOUR", 50, 90, 8, 1, glistp);
    glistp = CentreText("COMPETENCE.", 50, 100, 8, 1, glistp);
    glistp = CentreText("NOW IT GETS SERIOUS...", 50, 130, 8, 1, glistp);
    glistp = CentreText("SUPER COMBO COMBINES", 50, 160, 8, 1, glistp);
    glistp = CentreText("WEAPON AND RACE", 50, 170, 8, 1, glistp);
    glistp = CentreText("CHALLENGES", 50, 180, 8, 1, glistp);
    return glistp;
}

Gfx *DrawAllGoldBonus(ChallengeData *challenges, Gfx *glistp) {
    glistp = CentreText("ALL GOLD MEDALS", 50, 30, 12, 0, glistp);
    glistp = CentreText("AWARDED!", 50, 50, 12, 0, glistp);
    glistp = CentreText("YOU CAN NOW CLASS", 50, 80, 8, 1, glistp);
    glistp = CentreText("YOURSELF AMONGST THE FEW", 50, 90, 8, 1, glistp);
    glistp = CentreText("ELITE FSOOO PILOTS.", 50, 100, 8, 1, glistp);
    glistp = CentreText("THE WIPEOUT TEAM", 50, 130, 8, 1, glistp);
    glistp = CentreText("CONGRATULATES YOU ON YOUR", 50, 140, 8, 1, glistp);
    glistp = CentreText("SKILLS... BUT WILL THEY", 50, 150, 8, 1, glistp);
    glistp = CentreText("BE GOOD ENOUGH FOR THE", 50, 160, 8, 1, glistp);
    glistp = CentreText("NEXT EDITION OF", 50, 170, 8, 1, glistp);
    glistp = CentreText("WIPEOUT", 50, 180, 8, 1, glistp);
    return glistp;
}

void SwapLeagueStandings(LeagueStanding *standings, s32 idx) {
    s32 tmpPts;
    s32 tmpShip;

    tmpPts = standings[idx].points;
    tmpShip = standings[idx].shipIdx;
    standings[idx].points = standings[idx + 1].points;
    standings[idx].shipIdx = standings[idx + 1].shipIdx;
    standings[idx + 1].points = tmpPts;
    standings[idx + 1].shipIdx = tmpShip;
}

char *GetPilotName(s32 shipIdx, char *out) {
    s32 i;

    for (i = 0; i < frontendOptions_g.numPlayers; i++) {
        if (frontendOptions_g.ownShip[i] == shipIdx) {
            sprintf(out, "%s", frontendOptions_g.playerNames[i]);
            return out;
        }
    }
    if (frontendOptions_g.numPlayers == 3) {
        shipIdx = 1;
    }
    sprintf(out, "CPU%i", shipIdx);
    return out;
}

Gfx *LeagueStandingsMenu(Gfx *glistp) {
    char buf[256];
    char nameBuf[4];
    u8 count;
    u8 i;
    s32 j;
    s32 k;
    s32 col;

    sprintf(buf, "%d OF %d", (u8)(frontendOptions_g.leagueRaces - frontendOptions_g.leagueRacesLeft),
            frontendOptions_g.leagueRaces);
    count = 0;
    glistp = CentreText("RACES COMPLETE", 100, 75, 8, 1, glistp);
    glistp = CentreText(buf, 100, 85, 8, 1, glistp);

    for (i = 0; i < 15; i++) {
        g_leagueStandings[i].shipIdx = -1;
        g_leagueStandings[i].points = -1;
        if (ships[i].activeInLeague) {
            g_leagueResults[i].points = g_leagueResults[i].first * 5 + g_leagueResults[i].second * 3 +
                                        g_leagueResults[i].third + g_leagueResults[i].eliminations * 3;
            g_leagueStandings[count].shipIdx = i;
            g_leagueStandings[count].points = g_leagueResults[i].points;
            count++;
        }
    }

    for (j = 0; j < count; j++) {
        for (k = 0; k < count - 1; k++) {
            if (g_leagueStandings[k].points > g_leagueStandings[k + 1].points) {
                SwapLeagueStandings(g_leagueStandings, k);
            } else if (g_leagueStandings[k].points == g_leagueStandings[k + 1].points) {
                if (g_leagueResults[g_leagueStandings[k].shipIdx].first >
                    g_leagueResults[g_leagueStandings[k + 1].shipIdx].first) {
                    SwapLeagueStandings(g_leagueStandings, k);
                } else if (g_leagueResults[g_leagueStandings[k].shipIdx].first ==
                           g_leagueResults[g_leagueStandings[k + 1].shipIdx].first) {
                    if (g_leagueResults[g_leagueStandings[k].shipIdx].second >
                        g_leagueResults[g_leagueStandings[k + 1].shipIdx].second) {
                        SwapLeagueStandings(g_leagueStandings, k);
                    } else if (g_leagueResults[g_leagueStandings[k].shipIdx].second ==
                               g_leagueResults[g_leagueStandings[k + 1].shipIdx].second) {
                        if (g_leagueResults[g_leagueStandings[k].shipIdx].third >
                            g_leagueResults[g_leagueStandings[k + 1].shipIdx].third) {
                            SwapLeagueStandings(g_leagueStandings, k);
                        } else if (g_leagueResults[g_leagueStandings[k].shipIdx].third ==
                                   g_leagueResults[g_leagueStandings[k + 1].shipIdx].third) {
                            if (g_leagueResults[g_leagueStandings[k].shipIdx].eliminations >
                                g_leagueResults[g_leagueStandings[k + 1].shipIdx].eliminations) {
                                SwapLeagueStandings(g_leagueStandings, k);
                            }
                        }
                    }
                }
            }
        }
    }

    glistp = DrawMenuBox(7, 2, 60, 110, 224, 54, 8, glistp);
    g_leaguePilotCount = count;

    for (j = count - 1; j >= count - 4 && j >= 0; j--) {
        glistp = DrawFontText(GetPilotName(g_leagueStandings[j].shipIdx, nameBuf), 61, (count - j) * 9 + 120, 8,
                              glistp);
        col = 1;
        glistp = AddNumber(g_leagueResults[g_leagueStandings[j].shipIdx].first, 93 + col * 36, (count - j) * 9 + 120,
                           8, 1, glistp);
        col++;
        glistp = AddNumber(g_leagueResults[g_leagueStandings[j].shipIdx].second, 93 + col * 36, (count - j) * 9 + 120,
                           8, 1, glistp);
        col++;
        glistp = AddNumber(g_leagueResults[g_leagueStandings[j].shipIdx].third, 93 + col * 36, (count - j) * 9 + 120,
                           8, 1, glistp);
        col++;
        glistp = AddNumber(g_leagueResults[g_leagueStandings[j].shipIdx].eliminations, 93 + col * 36,
                           (count - j) * 9 + 120, 8, 1, glistp);
        col++;
        glistp = AddNumber(g_leagueResults[g_leagueStandings[j].shipIdx].points, 93 + col * 36, (count - j) * 9 + 120,
                           8, 1, glistp);
    }

    glistp = DrawMenu(g_leagueMenu, 130, 60, glistp, 1);
    return glistp;
}

Gfx *BonusCategoryScreen(Gfx *glistp) {
    s32 savedTrack;

    if (g_bonusMusicDelay != 0) {
        g_bonusMusicDelay--;
        if (g_bonusMusicDelay == 0) {
            savedTrack = frontendOptions_g.musicTrack;
            frontendOptions_g.musicTrack = 9;
            StopAudio();
            StartMusic();
            frontendOptions_g.musicTrack = savedTrack;
        }
    }

    if (MenuControl(g_bonusCategoryMenu) == 1) {
        g_menuScreen = g_bonusCategoryMenu[g_menuItem].nextScreen;
        StopAudio();
        StartMusic();
    }

    return DrawChallengeCategoryBonus(frontendOptions_g.challengeData, glistp);
}

Gfx *BonusCategoryCheck(Gfx *glistp) {
    if (CheckChallengeCategoryComplete(frontendOptions_g.challengeMenu, frontendOptions_g.challengeData)) {
        g_menuScreen = 29;
    } else {
        g_menuScreen = g_bonusMenu[0].nextScreen;
    }
    return glistp;
}

Gfx *BonusGoldScreen(Gfx *glistp) {
    s32 savedTrack;

    if (g_bonusMusicDelay != 0) {
        g_bonusMusicDelay--;
        if (g_bonusMusicDelay == 0) {
            savedTrack = frontendOptions_g.musicTrack;
            frontendOptions_g.musicTrack = 9;
            StopAudio();
            StartMusic();
            frontendOptions_g.musicTrack = savedTrack;
        }
    }

    if (MenuControl(g_bonusGoldMenu) == 1) {
        g_menuScreen = g_bonusGoldMenu[g_menuItem].nextScreen;
        StopAudio();
        StartMusic();
    }

    return DrawGoldChallengeBonus(frontendOptions_g.challengeData, glistp);
}

Gfx *BonusGoldCheck(Gfx *glistp) {
    if (CheckGoldChallengeUnlocked(frontendOptions_g.challengeMenu, frontendOptions_g.challengeData)) {
        g_menuScreen = 33;
    } else {
        g_menuScreen = g_bonusGoldCheckNext;
    }
    return glistp;
}

Gfx *BonusSuperComboScreen(Gfx *glistp) {
    s32 savedTrack;

    if (g_bonusMusicDelay != 0) {
        g_bonusMusicDelay--;
        if (g_bonusMusicDelay == 0) {
            savedTrack = frontendOptions_g.musicTrack;
            frontendOptions_g.musicTrack = 9;
            StopAudio();
            StartMusic();
            frontendOptions_g.musicTrack = savedTrack;
        }
    }

    if (MenuControl(g_bonusSuperComboMenu) == 1) {
        g_menuScreen = g_bonusSuperComboMenu[g_menuItem].nextScreen;
        StopAudio();
        StartMusic();
    }

    return DrawSuperComboBonus(frontendOptions_g.challengeData, glistp);
}

Gfx *BonusSuperComboCheck(Gfx *glistp) {
    if (CheckSuperComboUnlocked(frontendOptions_g.challengeMenu, frontendOptions_g.challengeData)) {
        g_menuScreen = 34;
    } else {
        g_bonusAllGoldMusicFlag = 1;
        g_menuScreen = g_bonusSuperComboCheckNext;
    }
    return glistp;
}

Gfx *BonusAllGoldScreen(Gfx *glistp) {
    s32 savedTrack;

    if (frontendOptions_g.allGoldAwarded && !frontendOptions_g.allGoldSeen) {
        if (g_bonusAllGoldMusicFlag && g_bonusMusicDelay != 0) {
            g_bonusMusicDelay--;
            if (g_bonusMusicDelay == 0) {
                savedTrack = frontendOptions_g.musicTrack;
                frontendOptions_g.musicTrack = 9;
                StopAudio();
                StartMusic();
                g_bonusAllGoldMusicFlag = 0;
                frontendOptions_g.musicTrack = savedTrack;
            }
        }
        if (MenuControl(g_bonusAllGoldMenu) == 1) {
            g_menuScreen = g_bonusAllGoldMenu[g_menuItem].nextScreen;
            frontendOptions_g.allGoldSeen = TRUE;
            StopAudio();
            StartMusic();
        }
        glistp = DrawAllGoldBonus(frontendOptions_g.challengeData, glistp);
    } else {
        g_menuScreen = g_bonusAllGoldMenu[0].nextScreen;
    }
    return glistp;
}

Gfx *WeaponChallengeTeamMenu(Gfx *glistp) {
    if (!g_challengeTeamMenuInit) {
        InitChallengeTeamMenu(g_weaponChallengeTeamMenu, g_weaponChallenges);
        g_challengeTeamMenuInit = TRUE;
    }
    if (MenuControl(g_weaponChallengeTeamMenu) == 1) {
        frontendOptions_g.ownShip[0] = frontendOptions_g.team = g_teamMenu[g_menuItem].value;
        g_demoMode = 0;
        g_menuScreen = g_weaponChallengeTeamMenu[g_menuItem].nextScreen;
        StartGame();
    }
    glistp = DrawMenu(g_weaponChallengeTeamMenu, 130, 60, glistp, 1);
    return glistp;
}

Gfx *SuperComboChallengeTeamMenu(Gfx *glistp) {
    if (!g_challengeTeamMenuInit) {
        InitChallengeTeamMenu(g_superComboChallengeTeamMenu, g_superComboChallenges);
        g_challengeTeamMenuInit = TRUE;
    }
    if (MenuControl(g_superComboChallengeTeamMenu) == 1) {
        frontendOptions_g.ownShip[0] = frontendOptions_g.team = g_teamMenu[g_menuItem].value;
        g_demoMode = 0;
        g_menuScreen = g_superComboChallengeTeamMenu[g_menuItem].nextScreen;
        StartGame();
    }
    glistp = DrawMenu(g_superComboChallengeTeamMenu, 130, 60, glistp, 1);
    return glistp;
}

Gfx *MenuScreenUnused(Gfx *glistp) {
    return glistp;
}

Gfx *ControllerPakMenu(Gfx *glistp) {
    s32 maxFiles;
    s32 filesUsed;
    s32 freeBytes;
    s32 err;
    s32 res;

    res = MenuControlMulti(g_loadSaveMenu, g_pakState);

    switch (g_pakState) {
        case 0:
            if (res == 2) {
                if (g_pakTouched) {
                    g_pakState = 19;
                } else {
                    SetMenuTransition(g_loadSaveMenu[g_menuItem].nextScreen);
                }
            } else if (res == 1) {
                g_pakConfirm = 0;
                g_pakState = 1;
            }
            break;

        case 1:
            if (g_menuItem == 0) {
                glistp = CentreText("LOAD INFORMATION", 105, 175, 8, 1, glistp);
            } else if (g_menuItem == 1) {
                glistp = CentreText("SAVE INFORMATION", 105, 175, 8, 1, glistp);
            }
            if (g_pakConfirm == 0) {
                glistp = DrawHudText("YES", 125, 195, 8, 1, glistp);
                glistp = DrawHudText("NO", 165, 195, 8, 0, glistp);
            } else {
                glistp = DrawHudText("YES", 125, 195, 8, 0, glistp);
                glistp = DrawHudText("NO", 165, 195, 8, 1, glistp);
            }

            if (g_menuButtonsPressed & L_JPAD) {
                if (g_pakConfirm == 0) {
                    PlayNote(26, 255, 15, 15, 256);
                }
                g_pakConfirm = 1;
            } else if (g_menuButtonsPressed & R_JPAD) {
                if (g_pakConfirm != 0) {
                    PlayNote(26, 255, 15, 15, 256);
                }
                g_pakConfirm = 0;
            }

            if (g_menuButtonsPressed & A_BUTTON) {
                if (g_pakConfirm != 0) {
                    if (g_menuItem == 1) {
                        g_pakState = 2;
                        g_pakConfirm = 0;
                        osContStartReadData(&g_siMsgQ);
                        osContGetReadData(g_contPads);
                    } else if (g_menuItem == 0) {
                        g_pakState = 2;
                        g_pakConfirm = 0;
                        osContStartReadData(&g_siMsgQ);
                        osContGetReadData(g_contPads);
                    }
                } else {
                    g_pakState = 0;
                    g_pakConfirm = 0;
                    osContStartReadData(&g_siMsgQ);
                    osContGetReadData(g_contPads);
                }
            }
            break;

        case 2:
            if (g_pakInitDelay == 0) {
                g_pakInitDelay = 10;
                g_pakRetries = 3;
            }
            g_pakInitDelay--;
            if (g_pakInitDelay == 0) {
                g_pfsErr = InitControllerPak();
                if (g_pfsErr == 0) {
                    g_pakState = 3;
                } else {
                    switch (g_pfsErr) {
                        case 1:
                            osContStartReadData(&g_siMsgQ);
                            osContGetReadData(g_contPads);
                            g_pakState = 4;
                            break;
                        case 2:
                            osContStartReadData(&g_siMsgQ);
                            osContGetReadData(g_contPads);
                            g_pakState = 5;
                            break;
                        case 11:
                            osContStartReadData(&g_siMsgQ);
                            osContGetReadData(g_contPads);
                            g_pakState = 17;
                            break;
                        case 10:
                            osContStartReadData(&g_siMsgQ);
                            osContGetReadData(g_contPads);
                            ReinitCont();
                            if (!g_rumblePresent) {
                                g_pakState = 21;
                            } else {
                                g_pakState = 17;
                            }
                            break;
                        case 4:
                            if (g_pakRetries > 0) {
                                g_pakState = 2;
                                g_pakInitDelay = 4;
                                g_pakRetries--;
                                break;
                            }
                        default:
                            osContStartReadData(&g_siMsgQ);
                            osContGetReadData(g_contPads);
                            g_pakState = 6;
                            break;
                    }
                }
            }
            break;

        case 3:
            g_pfsErr = osPfsFindFile(&g_motorPfs, g_pfsCompanyCode, g_pfsGameCode, g_pfsGameName, g_pfsExtName,
                                     &g_pfsFileNo);
            if (g_pfsErr == 0) {
                if (g_menuItem == 1 && !g_pakOverwriteConfirmed) {
                    osContStartReadData(&g_siMsgQ);
                    osContGetReadData(g_contPads);
                    g_pakState = 20;
                } else {
                    g_pakState = 8;
                    g_pakOverwriteConfirmed = FALSE;
                }
            } else if (g_menuItem == 0 && g_pfsErr == 5) {
                osContStartReadData(&g_siMsgQ);
                osContGetReadData(g_contPads);
                g_pakState = 13;
            } else {
                g_pakOverwriteConfirmed = FALSE;
                switch ((s32)g_pfsErr) {
                    case 1:
                        osContStartReadData(&g_siMsgQ);
                        osContGetReadData(g_contPads);
                        g_pakState = 4;
                        break;
                    case 2:
                        osContStartReadData(&g_siMsgQ);
                        osContGetReadData(g_contPads);
                        g_pakState = 5;
                        break;
                    case 5:
                        osContStartReadData(&g_siMsgQ);
                        osContGetReadData(g_contPads);
                        g_pakState = 7;
                        break;
                    default:
                        osContStartReadData(&g_siMsgQ);
                        osContGetReadData(g_contPads);
                        g_pakState = 6;
                        break;
                }
            }
            break;

        case 4:
            glistp = CentreText("CONTROLLER PAK", 105, 105, 8, 1, glistp);
            glistp = CentreText("NOT INSERTED", 105, 115, 8, 1, glistp);
            glistp = CentreText("PLEASE INSERT THE", 105, 135, 8, 1, glistp);
            glistp = CentreText("CONTROLLER PAK", 105, 145, 8, 1, glistp);
            glistp = CentreText("TO WHICH", 105, 155, 8, 1, glistp);
            if (g_menuItem == 0) {
                glistp = CentreText("YOU WISH TO LOAD", 105, 165, 8, 1, glistp);
            } else {
                glistp = CentreText("YOU WISH TO SAVE", 105, 165, 8, 1, glistp);
            }
            glistp = CentreText("DATA INTO CONTROLLER 1", 105, 175, 8, 1, glistp);
            if (g_pakConfirm == 0) {
                glistp = DrawHudText("RETRY", 100, 195, 8, 1, glistp);
                glistp = DrawHudText("CANCEL", 165, 195, 8, 0, glistp);
            } else {
                glistp = DrawHudText("RETRY", 100, 195, 8, 0, glistp);
                glistp = DrawHudText("CANCEL", 165, 195, 8, 1, glistp);
            }

            if (g_menuButtonsPressed & L_JPAD) {
                if (g_pakConfirm == 0) {
                    PlayNote(26, 255, 15, 15, 256);
                }
                g_pakConfirm = 1;
            } else if (g_menuButtonsPressed & R_JPAD) {
                if (g_pakConfirm != 0) {
                    PlayNote(26, 255, 15, 15, 256);
                }
                g_pakConfirm = 0;
            }

            if (g_menuButtonsPressed & A_BUTTON) {
                if (g_pakConfirm != 0) {
                    g_pakState = 2;
                    g_pakConfirm = 0;
                    osContStartReadData(&g_siMsgQ);
                    osContGetReadData(g_contPads);
                } else {
                    g_pakState = 26;
                    g_pakMsgTimer = 0;
                    g_pakConfirm = 0;
                    osContStartReadData(&g_siMsgQ);
                    osContGetReadData(g_contPads);
                }
            }
            break;

        case 5:
            glistp = CentreText("NEW CONTROLLER", 105, 125, 8, 1, glistp);
            glistp = CentreText("PAK INSERTED", 105, 135, 8, 1, glistp);
            glistp = CentreText("WOULD YOU LIKE", 105, 155, 8, 1, glistp);
            if (g_menuItem == 0) {
                glistp = CentreText("TO LOAD TO THIS", 105, 165, 8, 1, glistp);
            } else {
                glistp = CentreText("TO SAVE TO THIS", 105, 165, 8, 1, glistp);
            }
            glistp = CentreText("CONTROLLER PAK", 105, 175, 8, 1, glistp);
            if (g_pakConfirm == 0) {
                glistp = DrawHudText("YES", 125, 195, 8, 1, glistp);
                glistp = DrawHudText("NO", 165, 195, 8, 0, glistp);
            } else {
                glistp = DrawHudText("YES", 125, 195, 8, 0, glistp);
                glistp = DrawHudText("NO", 165, 195, 8, 1, glistp);
            }

            if (g_menuButtonsPressed & L_JPAD) {
                if (g_pakConfirm == 0) {
                    PlayNote(26, 255, 15, 15, 256);
                }
                g_pakConfirm = 1;
            } else if (g_menuButtonsPressed & R_JPAD) {
                if (g_pakConfirm != 0) {
                    PlayNote(26, 255, 15, 15, 256);
                }
                g_pakConfirm = 0;
            }

            if (g_menuButtonsPressed & A_BUTTON) {
                if (g_pakConfirm != 0) {
                    g_pakState = 2;
                    g_pakConfirm = 0;
                    osContStartReadData(&g_siMsgQ);
                    osContGetReadData(g_contPads);
                } else {
                    g_pakState = 0;
                    g_pakConfirm = 0;
                    osContStartReadData(&g_siMsgQ);
                    osContGetReadData(g_contPads);
                }
            }
            break;

        case 6:
            if (g_menuItem == 0) {
                glistp = CentreText("DATA LOAD FAILED", 105, 185, 8, 1, glistp);
            } else {
                glistp = CentreText("DATA SAVE FAILED", 105, 185, 8, 1, glistp);
            }
            g_pakMsgTimer++;
            if (g_pakMsgTimer > FRAME_RATE) {
                g_pakMsgTimer = 0;
                g_pakState = 0;
            }
            break;

        case 7:
            g_pfsErr = osPfsNumFiles(&g_motorPfs, &maxFiles, &filesUsed);
            if (g_pfsErr == 0) {
                if (filesUsed == maxFiles) {
                    osContStartReadData(&g_siMsgQ);
                    osContGetReadData(g_contPads);
                    g_pakState = 16;
                } else {
                    g_pakState = 14;
                }
            } else {
                switch (g_pfsErr) {
                    case 1:
                        osContStartReadData(&g_siMsgQ);
                        osContGetReadData(g_contPads);
                        g_pakState = 4;
                        break;
                    case 2:
                        osContStartReadData(&g_siMsgQ);
                        osContGetReadData(g_contPads);
                        g_pakState = 5;
                        break;
                    default:
                        osContStartReadData(&g_siMsgQ);
                        osContGetReadData(g_contPads);
                        g_pakState = 6;
                        break;
                }
            }
            break;

        case 8:
            if (g_menuItem == 0) {
                g_pfsErr = LoadControllerPakData(1, 1, 1, 1, 0);
            } else {
                g_pfsErr = SaveControllerPakData(1, 1, 1, 1, 0);
            }
            g_pakState = 9;
            break;

        case 9:
            if (g_menuItem == 0) {
                glistp = CentreText("READING SAVED DATA", 105, 155, 8, 1, glistp);
            } else {
                glistp = CentreText("WRITING DATA", 105, 155, 8, 1, glistp);
            }
            glistp = CentreText("DO NOT REMOVE", 105, 175, 8, 1, glistp);
            glistp = CentreText("CONTROLLER PAK", 105, 185, 8, 1, glistp);
            g_pakMsgTimer++;
            if (g_pakMsgTimer > FRAME_RATE) {
                g_pakMsgTimer = 0;
                if (g_pfsErr == 0) {
                    g_pakState = 10;
                } else {
                    g_pakState = 11;
                }
            }
            break;

        case 10:
            if (g_menuItem == 0) {
                glistp = CentreText("DATA LOADED", 105, 185, 8, 1, glistp);
            } else {
                glistp = CentreText("DATA SAVED", 105, 185, 8, 1, glistp);
            }
            g_pakMsgTimer++;
            if (g_pakMsgTimer > FRAME_RATE) {
                g_pakMsgTimer = 0;
                osContStartReadData(&g_siMsgQ);
                osContGetReadData(g_contPads);
                g_pakTouched = TRUE;
                g_pakState = 0;
            }
            break;

        case 11:
            if (g_pfsErr == 1) {
                osContStartReadData(&g_siMsgQ);
                osContGetReadData(g_contPads);
                g_pakState = 4;
            } else if (g_pfsErr == 2) {
                osContStartReadData(&g_siMsgQ);
                osContGetReadData(g_contPads);
                g_pakState = 5;
            } else {
                osContStartReadData(&g_siMsgQ);
                osContGetReadData(g_contPads);
                g_pakState = 6;
            }
            break;

        case 12:
            glistp = CentreText("NO GAME NOTES FREE", 105, 145, 8, 1, glistp);
            glistp = CentreText("ON CONTROLLER PAK", 105, 155, 8, 1, glistp);
            glistp = CentreText("SAVE FAILED", 105, 175, 8, 1, glistp);
            g_pakMsgTimer++;
            if (g_pakMsgTimer > FRAME_RATE) {
                g_pakMsgTimer = 0;
                g_pakState = 0;
            }
            break;

        case 13:
            glistp = CentreText("NO DATA ON", 105, 145, 8, 1, glistp);
            glistp = CentreText("CONTROLLER PAK", 105, 155, 8, 1, glistp);
            glistp = CentreText("LOAD FAILED", 105, 175, 8, 1, glistp);
            g_pakMsgTimer++;
            if (g_pakMsgTimer > FRAME_RATE) {
                g_pakMsgTimer = 0;
                g_pakState = 0;
            }
            break;

        case 14:
            g_pfsErr = osPfsFreeBlocks(&g_motorPfs, &freeBytes);
            if (g_pfsErr == 0) {
                if (freeBytes < g_pfsSaveSize) {
                    osContStartReadData(&g_siMsgQ);
                    osContGetReadData(g_contPads);
                    g_pakState = 16;
                } else {
                    g_pakState = 15;
                }
            } else {
                switch (g_pfsErr) {
                    case 1:
                        osContStartReadData(&g_siMsgQ);
                        osContGetReadData(g_contPads);
                        g_pakState = 4;
                        break;
                    case 2:
                        osContStartReadData(&g_siMsgQ);
                        osContGetReadData(g_contPads);
                        g_pakState = 5;
                        break;
                    default:
                        osContStartReadData(&g_siMsgQ);
                        osContGetReadData(g_contPads);
                        g_pakState = 6;
                        break;
                }
            }
            break;

        case 15:
            g_pfsErr = osPfsAllocateFile(&g_motorPfs, g_pfsCompanyCode, g_pfsGameCode, g_pfsGameName, g_pfsExtName,
                                         g_pfsSaveSize, &g_pfsFileNo);
            if (g_pfsErr == 0) {
                g_pakState = 8;
            } else {
                switch (g_pfsErr) {
                    case 1:
                        osContStartReadData(&g_siMsgQ);
                        osContGetReadData(g_contPads);
                        g_pakState = 4;
                        break;
                    case 2:
                        osContStartReadData(&g_siMsgQ);
                        osContGetReadData(g_contPads);
                        g_pakState = 5;
                        break;
                    default:
                        osContStartReadData(&g_siMsgQ);
                        osContGetReadData(g_contPads);
                        g_pakState = 6;
                        break;
                }
            }
            break;

        case 16:
            glistp = CentreText("NOT ENOUGH NOTES", 105, 125, 8, 1, glistp);
            glistp = CentreText("OR PAGES AVAILABLE.", 105, 135, 8, 1, glistp);
            glistp = CentreText("WIPEOUT64 REQUIRES", 105, 155, 8, 1, glistp);
            glistp = CentreText("1 NOTE AND", 105, 165, 8, 1, glistp);
            glistp = CentreText("SIX PAGES.", 105, 175, 8, 1, glistp);
            g_pakMsgTimer++;
            if (g_pakMsgTimer > (FRAME_RATE * 5) / 2) {
                g_pakMsgTimer = 0;
                g_pakState = 0;
            } else if (g_menuButtonsPressed & (A_BUTTON | B_BUTTON | START_BUTTON)) {
                g_pakMsgTimer = 0;
                g_pakState = 0;
                osContStartReadData(&g_siMsgQ);
                osContGetReadData(g_contPads);
            }
            break;

        case 17:
            g_pakState = 18;
            break;

        case 18:
            glistp = CentreText("A DEVICE OTHER THAN THE", 105, 115, 8, 1, glistp);
            glistp = CentreText("CONTROLLER PAK IS", 105, 125, 8, 1, glistp);
            glistp = CentreText("INSERTED IN CONTROLLER 1", 105, 135, 8, 1, glistp);
            glistp = CentreText("PLEASE REMOVE DEVICE", 105, 155, 8, 1, glistp);
            glistp = CentreText("AND INSERT CONTROLLER", 105, 165, 8, 1, glistp);
            glistp = CentreText("PAK INTO CONTROLLER 1", 105, 175, 8, 1, glistp);
            if (g_pakConfirm == 0) {
                glistp = DrawHudText("RETRY", 100, 195, 8, 1, glistp);
                glistp = DrawHudText("CANCEL", 165, 195, 8, 0, glistp);
            } else {
                glistp = DrawHudText("RETRY", 100, 195, 8, 0, glistp);
                glistp = DrawHudText("CANCEL", 165, 195, 8, 1, glistp);
            }

            if (g_menuButtonsPressed & L_JPAD) {
                if (g_pakConfirm == 0) {
                    PlayNote(26, 255, 15, 15, 256);
                }
                g_pakConfirm = 1;
            } else if (g_menuButtonsPressed & R_JPAD) {
                if (g_pakConfirm != 0) {
                    PlayNote(26, 255, 15, 15, 256);
                }
                g_pakConfirm = 0;
            }

            if (g_menuButtonsPressed & A_BUTTON) {
                if (g_pakConfirm != 0) {
                    g_pakState = 2;
                    g_pakConfirm = 0;
                    osContStartReadData(&g_siMsgQ);
                    osContGetReadData(g_contPads);
                } else {
                    g_pakState = 0;
                    g_pakConfirm = 0;
                    osContStartReadData(&g_siMsgQ);
                    osContGetReadData(g_contPads);
                }
            }
            break;

        case 19:
            glistp = CentreText("IF YOU WOULD LIKE TO", 105, 125, 8, 1, glistp);
            glistp = CentreText("INSERT THE RUMBLE", 145, 135, 8, 1, glistp);
            glistp = CentreText("PAK INTO CONTROLLER 1", 105, 145, 8, 1, glistp);
            glistp = CentreText("PLEASE DO SO NOW", 105, 155, 8, 1, glistp);
            glistp = CentreText("PRESS A TO CONTINUE", 105, 175, 8, 1, glistp);
            if (g_menuButtonsPressed & A_BUTTON) {
                SetMenuTransition(g_loadSaveMenu[g_menuItem].nextScreen);
                ReinitCont();
                g_pakTouched = FALSE;
                osContStartReadData(&g_siMsgQ);
                osContGetReadData(g_contPads);
                g_pakState = 0;
            }
            break;

        case 20:
            glistp = CentreText("YOU ARE ABOUT TO WRITE", 125, 125, 8, 1, glistp);
            glistp = CentreText("OVER AN EXISTING", 105, 135, 8, 1, glistp);
            glistp = CentreText("WIPEOUT64 SAVE", 105, 145, 8, 1, glistp);
            glistp = CentreText("DO YOU WANT TO", 105, 165, 8, 1, glistp);
            glistp = CentreText("CONTINUE", 105, 175, 8, 1, glistp);
            if (g_pakConfirm == 0) {
                glistp = DrawHudText("YES", 125, 195, 8, 1, glistp);
                glistp = DrawHudText("NO", 165, 195, 8, 0, glistp);
            } else {
                glistp = DrawHudText("YES", 125, 195, 8, 0, glistp);
                glistp = DrawHudText("NO", 165, 195, 8, 1, glistp);
            }

            if (g_menuButtonsPressed & L_JPAD) {
                if (g_pakConfirm == 0) {
                    PlayNote(26, 255, 15, 15, 256);
                }
                g_pakConfirm = 1;
            } else if (g_menuButtonsPressed & R_JPAD) {
                if (g_pakConfirm != 0) {
                    PlayNote(26, 255, 15, 15, 256);
                }
                g_pakConfirm = 0;
            }

            if (g_menuButtonsPressed & A_BUTTON) {
                if (g_pakConfirm != 0) {
                    g_pakState = 2;
                    g_pakConfirm = 0;
                    osContStartReadData(&g_siMsgQ);
                    osContGetReadData(g_contPads);
                    g_pakOverwriteConfirmed = TRUE;
                } else {
                    g_pakState = 0;
                    g_pakConfirm = 0;
                    g_pakOverwriteConfirmed = FALSE;
                    osContStartReadData(&g_siMsgQ);
                    osContGetReadData(g_contPads);
                }
            }
            break;

        case 21:
            glistp = CentreText("CONTROLLER PAK IS", 30, 125, 8, 1, glistp);
            glistp = CentreText("DAMAGED OR CORRUPTED", 30, 135, 8, 1, glistp);
            glistp = CentreText("OR MAY BE", 30, 145, 8, 1, glistp);
            glistp = CentreText("IMPROPERLY INSERTED.", 30, 155, 8, 1, glistp);
            if (g_pakConfirm == 0) {
                glistp = DrawHudText("REPAIR", 30, 175, 8, 1, glistp);
                glistp = DrawHudText("RETRY", 130, 175, 8, 1, glistp);
                glistp = DrawHudText("CONTINUE", 220, 175, 8, 0, glistp);
                if (g_menuItem == 1) {
                    glistp = DrawFontTextYellow("WITHOUT SAVING", 222, 185, 8, glistp);
                } else {
                    glistp = DrawFontTextYellow("WITHOUT LOADING", 220, 185, 8, glistp);
                }
            } else if (g_pakConfirm == 1) {
                glistp = DrawHudText("REPAIR", 30, 175, 8, 1, glistp);
                glistp = DrawHudText("RETRY", 130, 175, 8, 0, glistp);
                glistp = DrawHudText("CONTINUE", 220, 175, 8, 1, glistp);
                if (g_menuItem == 1) {
                    glistp = DrawFontText("WITHOUT SAVING", 222, 185, 8, glistp);
                } else {
                    glistp = DrawFontText("WITHOUT LOADING", 220, 185, 8, glistp);
                }
            } else {
                glistp = DrawHudText("REPAIR", 30, 175, 8, 0, glistp);
                glistp = DrawHudText("RETRY", 130, 175, 8, 1, glistp);
                glistp = DrawHudText("CONTINUE", 220, 175, 8, 1, glistp);
                if (g_menuItem == 1) {
                    glistp = DrawFontText("WITHOUT SAVING", 222, 185, 8, glistp);
                } else {
                    glistp = DrawFontText("WITHOUT LOADING", 220, 185, 8, glistp);
                }
            }

            if (g_menuButtonsPressed & L_JPAD) {
                if (g_pakConfirm == 0 || g_pakConfirm == 1) {
                    PlayNote(26, 255, 15, 15, 256);
                    g_pakConfirm++;
                }
            } else if (g_menuButtonsPressed & R_JPAD) {
                if (g_pakConfirm == 1 || g_pakConfirm == 2) {
                    PlayNote(26, 255, 15, 15, 256);
                    g_pakConfirm--;
                }
            }

            if (g_menuButtonsPressed & A_BUTTON) {
                if (g_pakConfirm == 0) {
                    g_pakState = 22;
                } else if (g_pakConfirm == 1) {
                    g_pakState = 23;
                } else {
                    g_pakState = 25;
                }
                osContStartReadData(&g_siMsgQ);
                osContGetReadData(g_contPads);
                g_pakConfirm = 0;
            }
            break;

        case 22:
            g_pakState = 6;
            break;

        case 23:
            g_pakState = 2;
            break;

        case 24:
            g_pfsRepairAttempts = 0;
            do {
                err = osPfsRepairId(&g_motorPfs);
                g_pfsRepairAttempts++;
            } while (err != 0 && g_pfsRepairAttempts < 4);
            g_pakState = 23;
            break;

        case 25:
            glistp = CentreText("ATTEMPTING TO REPAIR", 105, 125, 8, 1, glistp);
            glistp = CentreText("THE CONTROLLER PAK", 145, 135, 8, 1, glistp);
            glistp = CentreText("MAY CAUSE DATA TO", 105, 145, 8, 1, glistp);
            glistp = CentreText("BE LOST.", 105, 155, 8, 1, glistp);
            glistp = CentreText("CONTINUE?", 105, 175, 8, 1, glistp);
            if (g_pakConfirm == 0) {
                glistp = DrawHudText("YES", 125, 195, 8, 1, glistp);
                glistp = DrawHudText("NO", 165, 195, 8, 0, glistp);
            } else {
                glistp = DrawHudText("YES", 125, 195, 8, 0, glistp);
                glistp = DrawHudText("NO", 165, 195, 8, 1, glistp);
            }

            if (g_menuButtonsPressed & L_JPAD) {
                if (g_pakConfirm == 0) {
                    PlayNote(26, 255, 15, 15, 256);
                }
                g_pakConfirm = 1;
            } else if (g_menuButtonsPressed & R_JPAD) {
                if (g_pakConfirm != 0) {
                    PlayNote(26, 255, 15, 15, 256);
                }
                g_pakConfirm = 0;
            }

            if (g_menuButtonsPressed & A_BUTTON) {
                if (g_pakConfirm != 0) {
                    g_pakState = 24;
                    g_pakConfirm = 0;
                    osContStartReadData(&g_siMsgQ);
                    osContGetReadData(g_contPads);
                } else {
                    g_pakState = 21;
                    g_pakConfirm = 0;
                    osContStartReadData(&g_siMsgQ);
                    osContGetReadData(g_contPads);
                }
            }
            break;

        case 26:
            glistp = CentreText("YOU WILL BE UNABLE", 105, 125, 8, 1, glistp);
            glistp = CentreText("TO LOAD AND SAVE", 105, 135, 8, 1, glistp);
            glistp = CentreText("WIPEOUT64.", 105, 145, 8, 1, glistp);
            g_pakMsgTimer++;
            if (g_pakMsgTimer > (FRAME_RATE * 5) / 2) {
                g_pakMsgTimer = 0;
                g_pakState = 0;
            } else if (g_menuButtonsPressed & (A_BUTTON | B_BUTTON | START_BUTTON)) {
                g_pakMsgTimer = 0;
                g_pakState = 0;
                osContStartReadData(&g_siMsgQ);
                osContGetReadData(g_contPads);
            }
            break;
    }

    return DrawMenu(g_loadSaveMenu, 130, 60, glistp, 1);
}

Gfx *QuitLeagueMenu(Gfx *glistp) {
    s32 res;

    res = MenuControl(g_leagueMenu);
    if (g_quitLeaguePrompt == 0) {
        if (res == 1) {
            if (frontendOptions_g.leagueRacesLeft == 0) {
                g_menuScreen = 36;
            } else {
                frontendOptions_g.mode = 3;
                g_raceMode = 3;
                g_gameType = 3;
                g_menuScreen = 3;
            }
        } else if (res == 2) {
            if (frontendOptions_g.leagueRacesLeft == 0) {
                g_menuScreen = 1;
            } else {
                g_quitLeaguePrompt = 1;
                g_quitLeagueConfirm = 0;
            }
        }
    } else {
        glistp = CentreText("QUIT LEAGUE", 105, 185, 8, 1, glistp);
        if (g_quitLeagueConfirm == 0) {
            glistp = DrawHudText("YES", 125, 195, 8, 1, glistp);
            glistp = DrawHudText("NO", 165, 195, 8, 0, glistp);
        } else {
            glistp = DrawHudText("YES", 125, 195, 8, 0, glistp);
            glistp = DrawHudText("NO", 165, 195, 8, 1, glistp);
        }

        if (g_menuButtonsPressed & L_JPAD) {
            if (g_quitLeagueConfirm == 0) {
                PlayNote(26, 255, 15, 15, 256);
            }
            g_quitLeagueConfirm = 1;
        } else if (g_menuButtonsPressed & R_JPAD) {
            if (g_quitLeagueConfirm != 0) {
                PlayNote(26, 255, 15, 15, 256);
            }
            g_quitLeagueConfirm = 0;
        }

        if (g_menuButtonsPressed & A_BUTTON) {
            if (g_quitLeagueConfirm != 0) {
                SetMenuTransition(1);
                g_quitLeagueConfirm = 0;
            }
            g_quitLeaguePrompt = 0;
        }
    }

    return LeagueStandingsMenu(glistp);
}

#define tournamentTableStandingCount g_leaguePilotCount

Gfx *FinalStandingsMenu(Gfx *glistp) {
    char nameBuf[8];

    if (MenuControl(g_finalStandingsMenu) == 1) {
        SetMenuTransition(g_finalStandingsMenu[g_menuItem].nextScreen);
    }

    ASSERT(tournamentTableStandingCount >= 2, 5695);

    glistp = CentreTextAt(GetPilotName(g_leagueStandings[tournamentTableStandingCount - 1].shipIdx, nameBuf), 160, 80,
                          8, 1, glistp);
    glistp = CentreTextAt(GetPilotName(g_leagueStandings[tournamentTableStandingCount - 2].shipIdx, nameBuf), 80, 184,
                          8, 1, glistp);
    if (tournamentTableStandingCount >= 3) {
        glistp = CentreTextAt(GetPilotName(g_leagueStandings[tournamentTableStandingCount - 3].shipIdx, nameBuf), 250,
                              194, 8, 1, glistp);
    }

    return DrawMenu(g_finalStandingsMenu, 130, 60, glistp, 1);
}

uSprite *LoadFrontendSprites(s32 cols, s32 rows, u32 kind) {
    s32 startTex;
    void *timList;
    uSprite *sprites;
    s32 i;
    N64TextureStub *tex;

    startTex = TextureTableCount;
    switch (kind) {
        case 0:
            timList = LoadCompressedTextureSequence((u32)_feSpritesCmp0);
            break;
        case 1:
        case 9:
            timList = LoadCompressedTextureSequence((u32)_feSpritesCmp1);
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            timList = WADLoadCompressedTextureSequence(g_feWad);
            break;
        case 8:
            g_feWad->current = 11;
            timList = WADLoadCompressedTextureSequence(g_feWad);
            break;
        case 10:
            g_feWad->current = 12;
            timList = WADLoadCompressedTextureSequence(g_feWad);
            break;
        case 11:
            g_feWad->current = 13;
            timList = WADLoadCompressedTextureSequence(g_feWad);
            break;
    }

    LoadCmpFiles(timList);
    sprites = (uSprite *)DAlloc(g_heap, (cols * rows) * sizeof(uSprite), "frontend.c", 5754);
    ASSERT(sprites, 5755);

    for (i = 0; i < cols * rows; i++) {
        s32 bitSize;

        tex = g_textureTable[startTex + i];
        bitSize = (tex->type == 0) ? G_IM_SIZ_4b : G_IM_SIZ_8b;
        guSprite2DInit(&sprites[i], tex->pixels, tex->clut, tex->width, tex->width, tex->height, G_IM_FMT_CI,
                       bitSize, 0, 0);
    }

    return sprites;
}

Gfx *DrawMenuBox(s32 cols, s32 rows, s32 x, s32 y, s32 w, s32 h, u32 kind, Gfx *glistp) {
    uSprite *sprites;
    s32 idx;
    s32 c;
    s32 r;

    idx = 0;
    switch (kind) {
        case 0:
            sprites = g_boxSprites0;
            break;
        case 1:
        case 9:
            sprites = g_boxSprites1;
            break;
        case 2:
            sprites = g_boxSprites2;
            break;
        case 3:
            sprites = g_boxSprites3;
            break;
        case 4:
            sprites = g_boxSprites4;
            break;
        case 5:
            sprites = g_boxSprites5;
            break;
        case 6:
            sprites = g_boxSprites6;
            break;
        case 7:
            sprites = g_boxSprites7;
            break;
        case 8:
            sprites = g_boxSprites8;
            break;
        case 10:
            sprites = g_boxSprites10;
            break;
        case 11:
            sprites = g_boxSprites11;
            break;
    }

    gDPSetPrimColor(glistp++, 0, 0, 255, 255, 255, 255);

    for (c = 0; c < cols; c++) {
        for (r = rows - 1; r >= 0; r--) {
            if (kind != 9 || r < 2) {
                s32 px = x + c * (w / cols);
                s32 py = y + r * (h / rows);
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&sprites[idx]));
                gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
                gSPSprite2DDraw(glistp++, px << 2, py << 2);
            }
            idx++;
        }
    }

    return glistp;
}

void InitFrontendSprites(void) {
    Gfx *FrontendDList;

    FrontendDList = (Gfx *)DAlloc(g_heap, g_feDlistSize * sizeof(Gfx) + 32, "frontend.c", 5867);
    g_feDlistStart = FrontendDList;
    ASSERT(FrontendDList, 5868);

    TextureTableCount = 0;
    g_boxSprites0 = LoadFrontendSprites(10, 8, 0);
    g_boxSprites1 = LoadFrontendSprites(10, 8, 1);
    g_feWad = OpenWadFileNvc((u32)_feSpritesWad);
    g_boxSprites10 = LoadFrontendSprites(9, 2, 10);
    g_boxSprites11 = LoadFrontendSprites(10, 8, 11);
    CloseWadFileNvc(g_feWad);
}

void PackAudioSaveData(void) {
    g_saveBuf[0] = g_audioConfigMenu[1].value;
    g_saveBuf[1] = g_audioConfigMenu[2].value;
    g_saveBuf[2] = g_audioConfigMenu[0].value;
}

void PackGameAndScoresSaveData(void) {
    s32 i;
    s32 j;
    s32 offset;

    g_saveBuf[3] = g_gameConfigMenu[1].value;
    g_saveBuf[4] = g_gameConfigMenu[0].value;
    g_saveBuf[6] = g_gameConfigMenu[2].value;
    g_saveBuf[7] = g_gameConfigMenu[3].value;
    g_saveBuf[8] = g_gameConfigMenu[4].value;
    g_saveBuf[9] = g_gameConfigMenu[5].value;
    if (g_superComboUnlocked) {
        g_saveBuf[5] = g_gameConfigMenu[6].value;
    } else {
        g_saveBuf[5] = 0;
    }

    g_saveBuf[40] = g_hudState.nameEntered[0];
    g_saveBuf[41] = g_hudState.nameEntered[1];
    g_saveBuf[42] = g_hudState.nameEntered[2];
    g_saveBuf[43] = g_hudState.dispChar[0];
    g_saveBuf[45] = g_hudState.selectedChar;
    g_saveBuf[46] = g_hudState.alphabetIndex;
    g_saveBuf[47] = g_hudState.nameCursor;
    g_saveBuf[48] = g_hudState.nameEndX;
    g_saveBuf[50] = g_hudState.enteringName;
    g_saveBuf[51] = g_hudState.firstAlphabetIndex;
    g_saveBuf[52] = g_hudState.storeRecords;
    g_saveBuf[56] = g_hudState.pendingRecords;
    g_saveBuf[57] = g_hudState.displayedRecords;
    g_saveBuf[58] = g_hudState.highScore;
    g_saveBuf[59] = g_hudState.confirm;
    g_saveBuf[60] = g_hudState.lConfirm;

    offset = 61;
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 4; j++) {
            g_saveBuf[offset++] = g_hiScoreTable[i][j].name[0];
            g_saveBuf[offset++] = g_hiScoreTable[i][j].name[1];
            g_saveBuf[offset++] = g_hiScoreTable[i][j].name[2];
            *(s32 *)&g_saveBuf[offset] = *(s32 *)&g_hiScoreTable[i][j].time;
            offset += 4;
            g_saveBuf[offset++] = g_hiScoreTable[i][j].ship;
            g_saveBuf[offset++] = g_hiScoreTable[i][j].lapName[0];
            g_saveBuf[offset++] = g_hiScoreTable[i][j].lapName[1];
            g_saveBuf[offset++] = g_hiScoreTable[i][j].lapName[2];
            *(s32 *)&g_saveBuf[offset] = *(s32 *)&g_hiScoreTable[i][j].lapTime;
            offset += 4;
            *(s32 *)&g_saveBuf[offset] = *(s32 *)&g_hiScoreTable[i][j].eliminations;
            offset += 4;
            g_saveBuf[offset++] = g_hiScoreTable[i][j].lapShip;
        }
    }
}

void PackControllerSaveData(void) {
    g_saveBuf[10] = g_controllerConfigMenu[0].value;
}

void PackChallengeSaveData(void) {
    u8 i;

    for (i = 0; i < 7; i++) {
        g_saveBuf[11 + i] = g_raceChallenges[i].award;
    }
    for (i = 0; i < 7; i++) {
        g_saveBuf[18 + i] = g_ttChallenges[i].award;
    }
    for (i = 0; i < 7; i++) {
        g_saveBuf[25 + i] = g_weaponChallenges[i].award;
    }
    for (i = 0; i < 7; i++) {
        g_saveBuf[32 + i] = g_superComboChallenges[i].award;
    }
}

void UnpackAudioSaveData(void) {
    g_audioConfigMenu[1].value = g_saveBuf[0];
    g_audioConfigMenu[2].value = g_saveBuf[1];
    g_audioConfigMenu[0].value = g_saveBuf[2];
}

void UnpackGameAndScoresSaveData(void) {
    s32 i;
    s32 j;
    s32 offset;

    g_gameConfigMenu[1].value = g_saveBuf[3];
    g_gameConfigMenu[0].value = g_saveBuf[4];
    g_gameConfigMenu[6].value = g_saveBuf[5];
    g_gameConfigMenu[2].value = g_saveBuf[6];
    g_gameConfigMenu[3].value = g_saveBuf[7];
    g_gameConfigMenu[4].value = g_saveBuf[8];
    g_gameConfigMenu[5].value = g_saveBuf[9];

    g_hudState.nameEntered[0] = g_saveBuf[40];
    g_hudState.nameEntered[1] = g_saveBuf[41];
    g_hudState.nameEntered[2] = g_saveBuf[42];
    g_hudState.dispChar[0] = g_saveBuf[43];
    g_hudState.selectedChar = g_saveBuf[45];
    g_hudState.alphabetIndex = g_saveBuf[46];
    g_hudState.nameCursor = g_saveBuf[47];
    g_hudState.nameEndX = g_saveBuf[48];
    g_hudState.enteringName = g_saveBuf[50];
    g_hudState.firstAlphabetIndex = g_saveBuf[51];
    g_hudState.storeRecords = g_saveBuf[52];
    g_hiScoreLoaded = 1;
    g_hudState.pendingRecords = g_saveBuf[56];
    g_hudState.displayedRecords = g_saveBuf[57];
    g_hudState.highScore = g_saveBuf[58];
    g_hudState.confirm = g_saveBuf[59];
    g_hudState.lConfirm = g_saveBuf[60];

    offset = 61;
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 4; j++) {
            g_hiScoreTable[i][j].name[0] = g_saveBuf[offset++];
            g_hiScoreTable[i][j].name[1] = g_saveBuf[offset++];
            g_hiScoreTable[i][j].name[2] = g_saveBuf[offset++];
            *(s32 *)&g_hiScoreTable[i][j].time = *(s32 *)&g_saveBuf[offset];
            offset += 4;
            g_hiScoreTable[i][j].ship = g_saveBuf[offset++];
            g_hiScoreTable[i][j].lapName[0] = g_saveBuf[offset++];
            g_hiScoreTable[i][j].lapName[1] = g_saveBuf[offset++];
            g_hiScoreTable[i][j].lapName[2] = g_saveBuf[offset++];
            *(s32 *)&g_hiScoreTable[i][j].lapTime = *(s32 *)&g_saveBuf[offset];
            offset += 4;
            *(s32 *)&g_hiScoreTable[i][j].eliminations = *(s32 *)&g_saveBuf[offset];
            offset += 4;
            g_hiScoreTable[i][j].lapShip = g_saveBuf[offset++];
        }
    }
}

void UnpackControllerSaveData(void) {
    g_controllerConfigMenu[0].value = g_saveBuf[10];
}

void RefreshChallengeUnlocks(void) {
    u8 i;

    for (i = 0; i < 7; i++) {
        UnlockNextChallenge(g_raceChallengeMenu, g_raceChallenges, i);
    }
    for (i = 0; i < 7; i++) {
        UnlockNextChallenge(g_ttChallengeMenu, g_ttChallenges, i);
    }
    for (i = 0; i < 7; i++) {
        UnlockNextChallenge(g_weaponChallengeMenu, g_weaponChallenges, i);
    }
    for (i = 0; i < 7; i++) {
        UnlockNextChallenge(g_superComboChallengeMenu, g_superComboChallenges, i);
    }

    CheckGoldChallengeUnlocked(g_raceChallengeMenu, g_raceChallenges);
    CheckGoldChallengeUnlocked(g_ttChallengeMenu, g_ttChallenges);
    CheckGoldChallengeUnlocked(g_weaponChallengeMenu, g_weaponChallenges);
    CheckGoldChallengeUnlocked(g_superComboChallengeMenu, g_superComboChallenges);

    CheckChallengeCategoryComplete(NULL, g_raceChallenges);
    CheckChallengeCategoryComplete(NULL, g_ttChallenges);
    CheckChallengeCategoryComplete(NULL, g_weaponChallenges);
    CheckChallengeCategoryComplete(NULL, g_superComboChallenges);

    CheckSuperComboUnlocked(NULL, NULL);
    CheckAllGoldAwarded(NULL, NULL);
}

void UnpackChallengeSaveData(void) {
    u8 i;

    for (i = 0; i < 7; i++) {
        g_raceChallenges[i].award = g_saveBuf[11 + i];
    }
    for (i = 0; i < 7; i++) {
        g_ttChallenges[i].award = g_saveBuf[18 + i];
    }
    for (i = 0; i < 7; i++) {
        g_weaponChallenges[i].award = g_saveBuf[25 + i];
    }
    for (i = 0; i < 7; i++) {
        g_superComboChallenges[i].award = g_saveBuf[32 + i];
    }
    RefreshChallengeUnlocks();
}

u8 SaveControllerPakData(u8 savePad, u8 saveAudio, u8 saveGame, u8 saveChallenges, u8 unused) {
    if (savePad) {
        PackControllerSaveData();
    }
    if (saveAudio) {
        PackAudioSaveData();
    }
    if (saveGame) {
        PackGameAndScoresSaveData();
    }
    if (saveChallenges) {
        PackChallengeSaveData();
    }
    return osPfsReadWriteFile(&g_motorPfs, g_pfsFileNo, PFS_WRITE, 0, 0x560, g_saveBuf);
}

u8 LoadControllerPakData(u8 loadPad, u8 loadAudio, u8 loadGame, u8 loadChallenges, u8 unused) {
    s32 err;

    err = osPfsReadWriteFile(&g_motorPfs, g_pfsFileNo, PFS_READ, 0, 0x560, g_saveBuf);
    if (err == 0) {
        if (loadPad) {
            UnpackControllerSaveData();
        }
        if (loadAudio) {
            UnpackAudioSaveData();
        }
        if (loadGame) {
            UnpackGameAndScoresSaveData();
        }
        if (loadChallenges) {
            UnpackChallengeSaveData();
        }
    }
    return err;
}

u8 InitControllerPak(void) {
    s32 attempts;
    s32 err;

    attempts = 0;
    do {
        err = osPfsInit(&g_siMsgQ, &g_motorPfs, 0);
        attempts++;
    } while (err != 0 && attempts < 3);
    return err;
}

void StartupLogosAndAutoLoad(void) {
    OSTime now;

    if (g_logoTimerInit) {
        g_logoStartTime = osGetTime();
        g_logoTimerInit = 0;
    }

    if (g_pakAutoLoadState) {
        g_autoLoadErr = InitControllerPak();
        if (g_autoLoadErr == 0) {
            g_pakAutoLoadFind = 1;
            g_pakAutoLoadState = 0;
        } else {
            g_pakAutoLoadState = 0;
            g_pakAutoLoadDone = 1;
        }
    } else if (g_pakAutoLoadFind) {
        g_autoLoadErr = osPfsFindFile(&g_motorPfs, g_pfsCompanyCode, g_pfsGameCode, g_pfsGameName, g_pfsExtName,
                                      &g_pfsFileNo);
        if (g_autoLoadErr == 5) {
            g_pakAutoLoadFind = 0;
            g_pakAutoLoadDone = 1;
        } else if (g_autoLoadErr == 0) {
            LoadControllerPakData(1, 1, 1, 0, 0);
            g_pakAutoLoadFind = 0;
            g_pakAutoLoadDone = 1;
        } else {
            g_pakAutoLoadFind = 0;
            g_pakAutoLoadDone = 1;
        }
    } else if (g_pakAutoLoadDone) {
        osContStartReadData(&g_siMsgQ);
        osContGetReadData(g_contPads);
        CleanupStartupLogos();
        g_frontendLoaded = 0;
        g_startupActive = 0;
    } else {
        now = osGetTime();
        if (now < g_logoStartTime + 3 * g_oneSecond) {
            g_feDlistEnd = DrawMenuBox(10, 8, 0, 0, 320, 240, 1, g_feDlistEnd);
        } else if (now < g_logoStartTime + 6 * g_oneSecond) {
            g_feDlistEnd = DrawMenuBox(10, 8, 0, 0, 320, 240, 11, g_feDlistEnd);
        } else if (now < g_logoStartTime + 9 * g_oneSecond) {
            g_feDlistEnd = DrawMenuBox(9, 2, 16, 83, 288, 74, 10, g_feDlistEnd);
        } else if (now < g_logoStartTime + 12 * g_oneSecond) {
            g_feDlistEnd = DrawMenuBox(10, 8, 0, 0, 320, 240, 0, g_feDlistEnd);
        } else if (now >= g_logoStartTime + (g_oneSecond * 12010000ULL) / 1000000ULL) {
            if (g_rumblePresent) {
                g_pakAutoLoadDone = 1;
            } else {
                g_pakAutoLoadState = 1;
            }
        }
    }
}

void UnlockAllChallengesCheat(void) {
    u8 i;

    for (i = 0; i < 7; i++) {
        g_raceChallenges[i].award = 3;
        g_raceChallengeMenu[i].enabled = TRUE;
        g_raceChallengeMenu[i].hidden = FALSE;
    }
    g_velocitarUnlocked = TRUE;

    for (i = 0; i < 7; i++) {
        g_ttChallenges[i].award = 3;
        g_ttChallengeMenu[i].enabled = TRUE;
        g_ttChallengeMenu[i].hidden = FALSE;
    }
    g_piranhaUnlocked = TRUE;

    for (i = 0; i < 7; i++) {
        g_weaponChallenges[i].award = 3;
        g_weaponChallengeMenu[i].enabled = TRUE;
        g_weaponChallengeMenu[i].hidden = FALSE;
    }
    g_superComboUnlocked = TRUE;
    g_challengeMenu[3].hidden = FALSE;

    for (i = 0; i < 7; i++) {
        g_superComboChallenges[i].award = 3;
        g_superComboChallengeMenu[i].enabled = TRUE;
        g_superComboChallengeMenu[i].hidden = FALSE;
    }
    g_allChallengesCheated = TRUE;
}

u8 CheckControllerDetected(void) {
    if (!(g_contEnabled & 1)) {
        g_feDlistEnd = DrawMenuBox(10, 8, 0, 0, 320, 240, 9, g_feDlistEnd);
        g_feDlistEnd = CentreText("NO CONTROLLER DETECTED.", 50, 100, 8, 1, g_feDlistEnd);
        g_feDlistEnd = CentreText("PLEASE TURN OFF THE", 50, 120, 8, 1, g_feDlistEnd);
        g_feDlistEnd = CentreText("POWER INSERT A", 50, 130, 8, 1, g_feDlistEnd);
        g_feDlistEnd = CentreText("CONTROLLER AND TURN", 50, 140, 8, 1, g_feDlistEnd);
        g_feDlistEnd = CentreText("THE POWER ON.", 50, 150, 8, 1, g_feDlistEnd);
        return 0;
    }
    return 1;
}

u8 StartupWarningAndInit(void) {
    OSMesg dummyMsg;
    s32 i;
    s32 j;
    u8 ret;
    s16 texIdx;
    N64TextureStub *tex;

    ret = 1;
    g_startupWarningTimer--;
    if (g_startupWarningTimer != 0) {
        g_feDlistEnd = DrawMenuBox(10, 8, 0, 0, 320, 240, 9, g_feDlistEnd);
        if (g_startupWarningTimer < 8 * FRAME_RATE - 32) {
            if (GetBlinkState()) {
                g_feDlistEnd = CentreText("WARNING", 50, 70, 8, 0, g_feDlistEnd);
            }
        } else {
            g_feDlistEnd = CentreText("WARNING", 50, 70, 8, 0, g_feDlistEnd);
        }

        if (g_startupWarningPhase != 0) {
            g_feDlistEnd = CentreText("THERE IS NO CONTROLLER", 50, 90, 8, 1, g_feDlistEnd);
            g_feDlistEnd = CentreText("PAK ATTACHED OR THE ", 50, 100, 8, 1, g_feDlistEnd);
            g_feDlistEnd = CentreText("CONTROLLER PAK IS FULL", 50, 110, 8, 1, g_feDlistEnd);
            g_feDlistEnd = CentreText("OR THE CONTROLLER PAK", 50, 120, 8, 1, g_feDlistEnd);
            g_feDlistEnd = CentreText("IS CORRUPTED AND OR", 50, 130, 8, 1, g_feDlistEnd);
            g_feDlistEnd = CentreText("DAMAGED.", 50, 140, 8, 1, g_feDlistEnd);
            g_feDlistEnd = CentreText("YOU WILL BE UNABLE", 50, 170, 8, 1, g_feDlistEnd);
            g_feDlistEnd = CentreText("TO LOAD AND SAVE", 50, 180, 8, 1, g_feDlistEnd);
            g_feDlistEnd = CentreText("WIPEOUT64.", 50, 190, 8, 1, g_feDlistEnd);
        } else {
            g_feDlistEnd = CentreText("WIPEOUT64 SUPPORTS THE", 50, 90, 8, 1, g_feDlistEnd);
            g_feDlistEnd = CentreText("CONTROLLER PAK AND", 50, 100, 8, 1, g_feDlistEnd);
            g_feDlistEnd = CentreText("RUMBLE PAK.", 50, 110, 8, 1, g_feDlistEnd);
            g_feDlistEnd = CentreText("A PAK SHOULD NOT BE", 50, 130, 8, 1, g_feDlistEnd);
            g_feDlistEnd = CentreText("INSERTED OR REMOVED", 50, 140, 8, 1, g_feDlistEnd);
            g_feDlistEnd = CentreText("DURING GAMEPLAY.", 50, 150, 8, 1, g_feDlistEnd);
            g_feDlistEnd = CentreText("PLEASE WAIT UNTIL", 50, 170, 8, 1, g_feDlistEnd);
            g_feDlistEnd = CentreText("PROMPTED BEFORE", 50, 180, 8, 1, g_feDlistEnd);
            g_feDlistEnd = CentreText("INSERTING OR REMOVING", 50, 190, 8, 1, g_feDlistEnd);
            g_feDlistEnd = CentreText("A PAK.", 50, 200, 8, 1, g_feDlistEnd);
        }

        if (g_startupWarningTimer == 8 * FRAME_RATE - 2) {
            TextureTableCount = 600;
            g_heap = g_heapA = g_heapB = g_mainHeap;
            for (i = 0; i < 4; i++) {
                g_cameras[i].camPos = NewSkeleton();
                SetSkeletonPosition(g_cameras[i].camPos, 0, 0, 0);
                SetSkeletonDirection(g_cameras[i].camPos, 0, 0, 0);
                g_upperSkeletons[i] = NewSkeleton();
                SetSkeletonPosition(g_upperSkeletons[i], 0, 0, 0);
                SetSkeletonDirection(g_upperSkeletons[i], 0, 0, 0);
                g_cameras[i].camPos->super = g_upperSkeletons[i];
            }
            g_feSkeleton = NewSkeleton();
            g_feSkeleton->m[0][0] = 0x800;
            g_feSkeleton->m[1][1] = 0x800;
            g_feSkeleton->m[2][2] = 0x800;
            g_feSkeleton->super = g_cameras[0].camPos;
            InitPadSettings();
            for (j = 0; j < 15; j++) {
                g_shipDeathCount[j] = -1;
            }
            InitRaceResources();
            ScoreTabInit(g_raceOverlay);
            InitRenderList(g_renderList, 0, g_animTextureCount);
            CloseWadFileNvc(g_startupImage);
            g_startupData->current = 9;
            texIdx = TextureTableCount;
            LoadCmpFiles(WADLoadCompressedTextureSequence(g_startupData));
            tex = g_textureTable[texIdx];
            guSprite2DInit(&g_loaderSprite, (void *)OS_K0_TO_PHYSICAL(tex->pixels),
                           (void *)OS_K0_TO_PHYSICAL(tex->clut), tex->width, tex->width, tex->height, G_IM_FMT_CI,
                           tex->type != 0 ? G_IM_SIZ_8b : G_IM_SIZ_4b, 0, 0);
            CloseWadFileNvc(g_startupData);
            g_demoMode = 1;
            g_weaponLockFlag = 0;
            g_heap = g_heapA = g_heapB = g_tempHeap;
            InitInGamePlasmaPalette();
        }
    } else if (g_pakMissingAtBoot && g_startupWarningPhase == 0) {
        g_startupWarningTimer = 8 * FRAME_RATE - 3;
        g_startupWarningPhase = 1;
    } else {
        ret = 0;
    }

    osContStartReadData(&g_siMsgQ);
    osRecvMesg(&g_siMsgQ, &dummyMsg, OS_MESG_BLOCK);
    osContGetReadData(g_contPads);
    if (g_contPads[0].button & START_BUTTON) {
        if (g_startupWarningTimer < 8 * FRAME_RATE - 32) {
            if (g_pakMissingAtBoot && g_startupWarningPhase == 0) {
                g_startupWarningTimer = 8 * FRAME_RATE - 3;
                g_startupWarningPhase = 1;
                g_contPads[0].button &= ~START_BUTTON;
            } else {
                ret = 0;
            }
        }
        osContStartReadData(&g_siMsgQ);
        osRecvMesg(&g_siMsgQ, &dummyMsg, OS_MESG_BLOCK);
        osContGetReadData(g_contPads);
    }

    return ret;
}

void FrontendUpdate(void) {
    char unused[0x38]; /* unused: retail reserves 0x38 bytes of stack locals */
    N64TextureStub *dstTex;
    N64TextureStub *srcTex;
    s32 *initPtr;

    /* Empty-to-flow else arm stops cse1/cse2 from hoisting %hi(g_menuScreen) into $s5 across g_startupActive */
    if (g_menuScreen != 0) {
        osGetTime();
    } else {
        dstTex = NULL;
    }

    /* Unused pointer preserved from retail: places %hi(g_feInitialized) ($s4) in the g_startupActive branch delay slot */
    initPtr = &g_feInitialized;

    if (g_startupActive) {
        g_feDlistEnd = g_feDlistStart;
        UpdateBlink();
        if (g_startupLogosActive) {
            StartupLogosAndAutoLoad();
        } else if (g_pakManagerActive) {
            if (ControllerPakManagerMenu(&g_siMsgQ)) {
                g_startupWarningActive = 1;
                g_pakManagerActive = 0;
                g_pakManagerInit = 0;
            }
        } else if (g_startupWarningActive) {
            if (!StartupWarningAndInit()) {
                g_startupWarningActive = 0;
                StartupLogosAndAutoLoad();
                g_startupLogosActive = 1;
            }
        } else if (CheckControllerDetected()) {
            if (!g_rumblePresent) {
                if (!CheckBootControllerPak(&g_siMsgQ)) {
                    g_startupWarningActive = 1;
                } else if (ControllerPakManagerMenu(&g_siMsgQ)) {
                    g_startupWarningActive = 1;
                    g_pakManagerActive = 0;
                    g_pakManagerInit = 0;
                } else {
                    g_pakManagerActive = 1;
                }
            } else {
                g_startupWarningActive = 1;
                g_pakMissingAtBoot = 1;
            }
        }
    } else {
        if (g_feInitialized == 0) {
            frontendOptions_g.trackFile = (u32)_track1File;
            frontendOptions_g.trackNo = 1;
            frontendOptions_g.mode = 2;
            frontendOptions_g.teamClass = 0;
            frontendOptions_g.numPlayers = 1;
            frontendOptions_g.team = 3;
            g_gameType = 0;
            g_prevTrackNo = 1;
            g_prevTeamClass = 0;
            g_prevTeam = 3;
            ApplyGameConfig();
            SaveControllerConfig();
            g_savedMusicTrack = frontendOptions_g.musicTrack = g_audioConfigMenu[0].value;
            g_savedMusicVol = frontendOptions_g.musicVol = g_audioConfigMenu[1].value;
            SetMasterMusicVolume((frontendOptions_g.musicVol * 0x7FFF) / 116);
            g_savedSfxVol = frontendOptions_g.sfxVol = g_audioConfigMenu[2].value;
            SetMasterSfxVolume((frontendOptions_g.sfxVol * 0x7FFF) / 116);
            frontendOptions_g.allGoldAwarded = 0;
            frontendOptions_g.allGoldSeen = 0;
            g_feInitialized = 1;
        }

        if (g_bonusMusicDelay != 0 && g_menuScreen != 28 && g_menuScreen != 32 && g_menuScreen != 30 &&
            g_menuScreen != 31 && g_menuScreen != 29 && g_menuScreen != 33 && g_menuScreen != 34) {
            g_bonusMusicDelay--;
            if (g_bonusMusicDelay == 0) {
                StartMusic();
            }
        }

        UpdateBlink();
        UpdateCheatSequence(0);
        g_feDlistEnd = g_feDlistStart;
        UpdatePlasma();
        if (g_menuScreen != 0) {
            g_feDlistEnd = RenderPlasma(g_feDlistEnd);
        }
        if (cheatAllChallenges) {
            UnlockAllChallengesCheat();
        }

        switch (g_menuScreen) {
            case 0:
                g_feDlistEnd = TitleScreen(g_feDlistEnd);
                break;
            case 28:
                g_feDlistEnd = BonusCategoryCheck(g_feDlistEnd);
                break;
            case 29:
                g_feDlistEnd = BonusCategoryScreen(g_feDlistEnd);
                break;
            case 32:
                g_feDlistEnd = BonusGoldCheck(g_feDlistEnd);
                break;
            case 33:
                g_feDlistEnd = BonusGoldScreen(g_feDlistEnd);
                break;
            case 30:
                g_feDlistEnd = BonusSuperComboCheck(g_feDlistEnd);
                break;
            case 34:
                g_feDlistEnd = BonusSuperComboScreen(g_feDlistEnd);
                break;
            case 31:
                g_feDlistEnd = BonusAllGoldScreen(g_feDlistEnd);
                break;
            case 1:
                g_feDlistEnd = MainMenu(g_feDlistEnd);
                break;
            case 2:
                g_feDlistEnd = TeamSelectMenu(g_feDlistEnd);
                break;
            case 3:
                g_feDlistEnd = ClassSelectMenu(g_feDlistEnd);
                break;
            case 4:
                g_feDlistEnd = TrackSelectMenu(g_feDlistEnd);
                break;
            case 8:
                g_feDlistEnd = ChallengeSelectMenu(g_feDlistEnd);
                break;
            case 9:
                g_feDlistEnd = RaceChallengeMenu(g_feDlistEnd);
                break;
            case 10:
                g_feDlistEnd = TimeTrialChallengeMenu(g_feDlistEnd);
                break;
            case 11:
                g_feDlistEnd = WeaponChallengeMenu(g_feDlistEnd);
                break;
            case 12:
                g_feDlistEnd = SuperComboChallengeMenu(g_feDlistEnd);
                break;
            case 13:
                g_feDlistEnd = RaceChallengeBriefing(g_feDlistEnd);
                break;
            case 14:
                g_feDlistEnd = TimeTrialChallengeBriefing(g_feDlistEnd);
                break;
            case 15:
                g_feDlistEnd = WeaponChallengeBriefing(g_feDlistEnd);
                break;
            case 16:
                g_feDlistEnd = SuperComboChallengeBriefing(g_feDlistEnd);
                break;
            case 17:
                g_feDlistEnd = RaceChallengeTeamMenu(g_feDlistEnd);
                break;
            case 18:
                g_feDlistEnd = TimeTrialChallengeTeamMenu(g_feDlistEnd);
                break;
            case 19:
                g_feDlistEnd = WeaponChallengeTeamMenu(g_feDlistEnd);
                break;
            case 20:
                g_feDlistEnd = SuperComboChallengeTeamMenu(g_feDlistEnd);
                break;
            case 5:
                g_feDlistEnd = MultiplayerStartMenu(g_feDlistEnd);
                break;
            case 21:
                g_feDlistEnd = OptionsMenu(g_feDlistEnd);
                break;
            case 22:
                g_feDlistEnd = QuitLeagueMenu(g_feDlistEnd);
                break;
            case 36:
                g_feDlistEnd = FinalStandingsMenu(g_feDlistEnd);
                break;
            case 25:
                g_feDlistEnd = GameConfigMenu(g_feDlistEnd);
                break;
            case 27:
                g_feDlistEnd = TimeTrialRecordsMenu(g_feDlistEnd);
                break;
            case 26:
                g_feDlistEnd = RaceRecordsMenu(g_feDlistEnd);
                break;
            case 6:
                g_feDlistEnd = ControllerConfigMenu(g_feDlistEnd);
                break;
            case 7:
                g_feDlistEnd = AudioConfigMenu(g_feDlistEnd);
                break;
            case 35:
                g_feDlistEnd = CreditsMenu(g_feDlistEnd);
                break;
            case 24:
                g_feDlistEnd = ControllerPakMenu(g_feDlistEnd);
                break;
            case 23:
                g_feDlistEnd = MenuScreenUnused(g_feDlistEnd);
                break;
            case 38:
                break;
        }

        if (cheatfade > 0) {
            UpdateWeaponMessage(0);
            g_feDlistEnd = DrawScreenOverlay(0, 1, g_feDlistEnd, 0);
        }

        if (g_medalTexStart != -1) {
            dstTex = g_textureTable[g_medalTexStart];
            srcTex = g_textureTable[g_medalAnimFrame + g_medalTexStart + 1];
            if (dstTex->type == 0) {
                memcpy(dstTex->pixels, srcTex->pixels, (dstTex->width * dstTex->height) >> 1);
                memcpy(dstTex->clut, srcTex->clut, 32);
            } else {
                memcpy(dstTex->pixels, srcTex->pixels, dstTex->width * dstTex->height);
                memcpy(dstTex->clut, srcTex->clut, 512);
            }
            g_medalAnimTick++;
            if (!(g_medalAnimTick & 1)) {
                g_medalAnimFrame++;
                if (g_medalAnimFrame >= g_medalTexCount - 1) {
                    g_medalAnimFrame = 0;
                }
            }
        }
    }

    gSPEndDisplayList(g_feDlistEnd++);
}

void StartAttractDemo(void) {
    s32 i;
    s32 j;

    g_demoMode = 1;
    frontendOptions_g.numPlayers = 1;
    frontendOptions_g.teamClass = 2;

    for (i = 0; i < frontendOptions_g.numPlayers; i++) {
        frontendOptions_g.ownShip[i] = newRand() % 15;
        for (j = i - 1; j >= 0;) {
            if (frontendOptions_g.ownShip[i] == frontendOptions_g.ownShip[j]) {
                frontendOptions_g.ownShip[i]++;
                if (frontendOptions_g.ownShip[i] == 15) {
                    frontendOptions_g.ownShip[i] = 0;
                }
            } else {
                j--;
            }
        }
    }

    if (frontendOptions_g.trackNo == 6) {
        frontendOptions_g.trackNo = 1;
    } else if (frontendOptions_g.trackNo == 1) {
        frontendOptions_g.trackNo = 3;
    } else {
        frontendOptions_g.trackNo++;
    }

    frontendOptions_g.trackFile = g_trackMenu[frontendOptions_g.trackNo - 1].trackFile;

    if (frontendOptions_g.numPlayers == 1) {
        g_raceMode = 2;
        frontendOptions_g.unk14 = 1;
        g_gameType = 0;
    } else {
        g_raceMode = 3;
        frontendOptions_g.unk14 = 1;
        g_gameType = 3;
    }

    g_idleTimer = 0;
    frontendOptions_g.mode = g_raceMode;

    switch (frontendOptions_g.numPlayers) {
        case 1:
            frontendOptions_g.screenLayout[0] = 0;
            break;
        case 2:
            frontendOptions_g.screenLayout[0] = 3;
            frontendOptions_g.screenLayout[1] = 4;
            break;
        case 3:
            frontendOptions_g.screenLayout[0] = 5;
            frontendOptions_g.screenLayout[1] = 6;
            frontendOptions_g.screenLayout[2] = 7;
            break;
        case 4:
            frontendOptions_g.screenLayout[0] = 5;
            frontendOptions_g.screenLayout[1] = 6;
            frontendOptions_g.screenLayout[2] = 7;
            frontendOptions_g.screenLayout[3] = 8;
            break;
    }

    StartGame();
}

Gfx *TitleScreen(Gfx *glistp) {
    if (g_titleSeedLocked) {
        g_titleSeedCounter++;
    }

    glistp = DrawMenuBox(10, 8, 0, 0, 320, 240, 2, glistp);

    if (GetBlinkState()) {
#ifdef VERSION_EU
        glistp = CentreText("PRESS START", 157, 221, 8, 1, glistp);
#else
        glistp = CentreText("PRESS START", 157, 211, 8, 1, glistp);
#endif
    }

    if (g_firstContPad.button & START_BUTTON) {
        g_titleSeedLocked = TRUE;
        g_randSeed = g_titleSeedCounter;
        PlayNote(27, 255, 15, 15, 256);
        g_menuScreen = 1;
        g_menuItem = 0;
        g_titleIdleTimer = 15 * FRAME_RATE;
        frontendOptions_g.trackFile = (u32)_track1File;
        frontendOptions_g.numPlayers = 1;
        frontendOptions_g.mode = 2;
        frontendOptions_g.teamClass = 0;
        frontendOptions_g.trackNo = 1;
        frontendOptions_g.team = 3;
        g_gameType = 0;
        g_demoMode = 0;
        g_raceMode = 2;
        frontendOptions_g.unk14 = 1;
        frontendOptions_g.mode = 2;
    } else {
        g_titleIdleTimer--;
    }

    if (g_titleIdleTimer == 0) {
        g_titleIdleTimer = 15 * FRAME_RATE;
        g_menuItem = 0;
        StartAttractDemo();
    }

    return glistp;
}

u8 GetBlinkState(void) {
    u8 res = 0;

    if (g_blinkCounter >= 0) {
        res = 1;
    }
    return res;
}

void UpdateBlink(void) {
    g_blinkCounter--;
    if (g_blinkCounter == -15) {
        g_blinkCounter = 15;
    }
}

void LoadFrontendModels(void) {
    s16 startTex;
    s32 savedIdx;
    s16 i;
    ObjectStub *obj;
    WadFileStub *wad;

    startTex = TextureTableCount;
    LoadCmpFiles(WADLoadCompressedTextureSequence(g_feWad));
    InitRenderList(g_feRenderList, startTex, TextureTableCount - startTex);

    i = 0;
    obj = WADLoadPrm(g_feWad, g_feRenderList);
    g_feModels[0] = obj;
    while (obj != NULL) {
        obj->skeleton->super = g_cameras[0].camPos;
        g_feModels[i] = obj;
        obj = obj->next;
        i++;
    }

    savedIdx = g_feWad->current;
    g_feWad->current = 10;
    i = 29;
    obj = WADLoadPrm(g_feWad, g_feRenderList);
    g_feModels[29] = obj;
    while (obj != NULL) {
        obj->skeleton->super = g_cameras[0].camPos;
        g_feModels[i] = obj;
        obj = obj->next;
        i++;
    }

    g_feWad->current = savedIdx;
    wad = OpenWadFileNvc((u32)_feExtraModelsWad);
    wad->current = 21;
    i = 33;
    obj = WADLoadPrm(wad, g_feRenderList);
    g_feModels[33] = obj;
    while (obj != NULL) {
        obj->skeleton->super = g_cameras[0].camPos;
        g_feModels[i] = obj;
        obj = obj->next;
        i++;
    }
}

void CheckAllGoldInitial(void) {
    u8 i;

    for (i = 0; i < 7; i++) {
        if (g_raceChallenges[i].award != 3 || g_ttChallenges[i].award != 3 || g_weaponChallenges[i].award != 3 ||
            g_superComboChallenges[i].award != 3) {
            break;
        }
    }
    if (i == 7) {
        frontendOptions_g.allGoldAwarded = TRUE;
    }
}

void LoadSpinningMedal(void) {
    WadFileStub *wad;
    uSprite *spinningMedal;
    N64TextureStub *tex;
    s32 bitSize;

    wad = OpenWadFile((u32)_feMedalWad);
    wad->current = 10;
    g_medalTexStart = TextureTableCount;
    LoadCmpFiles(WADLoadCompressedTextureSequence(wad));
    g_medalTexCount = TextureTableCount - g_medalTexStart;
    CloseWadFile(wad);

    g_medalSprite = NULL;
    spinningMedal = (uSprite *)DAlloc(g_heap, sizeof(uSprite), "frontend.c", 7806);
    g_medalSprite = spinningMedal;
    ASSERT(spinningMedal, 7807);

    tex = g_textureTable[g_medalTexStart];
    bitSize = (tex->type == 0) ? G_IM_SIZ_4b : G_IM_SIZ_8b;
    guSprite2DInit(g_medalSprite, tex->pixels, tex->clut, tex->width, tex->width, tex->height, G_IM_FMT_CI,
                   bitSize, 0, 0);
}

void InitFrontend(void) {
    Gfx *FrontendDList;
    uSprite *frontEndSprites;
    s32 startTex;
    u32 i;
    N64TextureStub *tex;
    s32 bitSize;

    CheckAllGoldInitial();

    g_feDlistSize = 1400;
    FrontendDList = (Gfx *)DAlloc(g_heap, 1400 * sizeof(Gfx) + 32, "frontend.c", 7850);
    g_feDlistStart = FrontendDList;
    ASSERT(FrontendDList, 7851);

    TextureTableCount = 0;
    for (i = 0; i < 4; i++) {
        SetSkeletonPosition(g_cameras[i].camPos, 0, 0, 0);
        SetSkeletonDirection(g_cameras[i].camPos, 0, 0, 0);
        SetSkeletonPosition(g_upperSkeletons[i], 0, 0, 0);
        SetSkeletonDirection(g_upperSkeletons[i], 0, 0, 0);
    }

    g_feWad = OpenWadFileNvc((u32)_feSpritesWad);
    startTex = TextureTableCount;
    LoadCmpFiles(WADLoadCompressedTextureSequence(g_feWad));

    g_menuSprites = NULL;
    frontEndSprites = (uSprite *)DAlloc(g_heap, 48 * sizeof(uSprite), "frontend.c", 7874);
    g_menuSprites = frontEndSprites;
    ASSERT(frontEndSprites, 7875);

    for (i = 0; i < 48; i++) {
        if (i == 18) {
            g_font3TexStart = startTex + 18;
        }
        tex = g_textureTable[startTex + i];
        bitSize = (tex->type == 0) ? G_IM_SIZ_4b : G_IM_SIZ_8b;
        guSprite2DInit(&g_menuSprites[i], tex->pixels, tex->clut, tex->width, tex->width, tex->height, G_IM_FMT_CI,
                       bitSize, 0, 0);
    }

    InitFontSize(3);
    LoadFrontendModels();
    g_boxSprites3 = LoadFrontendSprites(10, 8, 3);
    g_boxSprites2 = LoadFrontendSprites(10, 8, 2);
    LoadAudioSliderSprites();
    g_boxSprites4 = LoadFrontendSprites(9, 3, 4);
    g_boxSprites5 = LoadFrontendSprites(9, 3, 5);
    g_boxSprites6 = LoadFrontendSprites(9, 3, 6);
    g_boxSprites7 = LoadFrontendSprites(9, 3, 7);
    g_boxSprites8 = LoadFrontendSprites(7, 2, 8);
    CloseWadFileNvc(g_feWad);
    LoadSpinningMedal();
    InitPlasma();

    for (i = 0; i < 16; i++) {
        g_cheatHistory[i] = 0;
    }

    g_randSeed = osGetTime();
    g_bonusMusicDelay = FRAME_RATE / 2;
}

void CleanupStartupLogos(void) {
    DInit(g_tempHeap, 0x184C00);
    g_challengeModelDl = NULL;
    g_challengeModelVtx = NULL;
    g_challengeMedalDl = NULL;
    g_challengeMedalVtx = NULL;
    g_podium1stDl = 0;
    g_podium1stVtx = 0;
    g_podium2ndDl = 0;
    g_podium2ndVtx = 0;
    g_podium3rdDl = 0;
    g_podium3rdVtx = 0;
    TextureTableCount = 0;
    g_heap = g_heapA = g_heapB = g_tempHeap;
}

Gfx *RenderMenu3DModel(MenuItem *menu, Gfx *glistp) {
    u8 i;
    s32 modelIdx;
    ObjectStub *obj;

    for (i = 0; menu[i].row != -1; i++) {
        if (menu[i].selected == 0) {
            g_menuItem = i;
            break;
        }
    }

    if ((menu[g_menuItem].hidden || !menu[g_menuItem].enabled) && menu != g_bonusMenu) {
        return glistp;
    }

    ResetRenderList(g_feRenderList);
    modelIdx = menu[g_menuItem].modelIdx;
    if (menu == g_bonusMenu) {
        if (frontendOptions_g.challengeData == g_raceChallenges) {
            modelIdx = 13;
        } else if (frontendOptions_g.challengeData == g_ttChallenges) {
            modelIdx = 21;
        } else if (frontendOptions_g.challengeData == g_weaponChallenges) {
            modelIdx = 26;
        } else {
            return glistp;
        }
    }

    if (modelIdx == 37) {
        return glistp;
    }

    obj = g_feModels[modelIdx];
    g_modelAngle += 20;
    if (g_modelAngle >= 4096) {
        g_modelAngle -= 4096;
    }
    SetSkeletonDirectionHPR(obj->skeleton, g_modelAngle, g_modelTransforms[modelIdx].pitch,
                            g_modelTransforms[modelIdx].roll);

    if (menu == g_bonusMenu) {
        if (frontendOptions_g.challengeData == g_raceChallenges) {
            SetSkeletonPosition(obj->skeleton, 0, 0, 1196);
        } else if (frontendOptions_g.challengeData == g_ttChallenges) {
            SetSkeletonPosition(obj->skeleton, 0, 0, 496);
        } else if (frontendOptions_g.challengeData == g_weaponChallenges) {
            SetSkeletonPosition(obj->skeleton, 0, -40, 2896);
        }
    } else if (modelIdx >= 20 && modelIdx < 25) {
        SetSkeletonPosition(obj->skeleton, -270, g_modelTransforms[modelIdx].y, g_modelTransforms[modelIdx].z);
    } else {
        SetSkeletonPosition(obj->skeleton, 0, g_modelTransforms[modelIdx].y, g_modelTransforms[modelIdx].z);
    }

    MatrixToN64Mtx(GetSkeletonMatrix(obj->skeleton), &g_mtxPool[g_mtxCount]);
    AddRenderGroup(g_feRenderList, obj->renderGroup, &g_mtxPool[g_mtxCount]);
    g_mtxCount++;
    return FlushRenderList(g_feRenderList, glistp);
}

Gfx *RenderMultiShipModel(MenuItem *menu, CameraStub *cam, s32 playerNum, Gfx *glistp) {
    u8 i;
    s32 slot;
    s32 modelIdx;
    ObjectStub *obj;

    for (i = 0; menu[i].row != -1; i++) {
        if (menu[i].selected == 0) {
            g_menuItem = i;
            break;
        }
    }

    if (menu[g_menuItem].hidden || !menu[g_menuItem].enabled) {
        return glistp;
    }

    ResetRenderList(g_feRenderList);
    if (playerNum == 0) {
        g_modelAngle += 20;
    }
    if (g_modelAngle >= 4096) {
        g_modelAngle -= 4096;
    }

    for (slot = 0; slot < 4; slot++) {
        if (g_multiSlots[slot].joined) {
            playerNum--;
            if (playerNum < 0) {
                break;
            }
        }
    }
    ASSERT(playerNum < 0, 8262);

    switch (g_multiSlots[slot].shipChoice) {
        case 0:
            modelIdx = 22;
            break;
        case 1:
            modelIdx = 24;
            break;
        case 2:
            modelIdx = 23;
            break;
        case 3:
            modelIdx = 20;
            break;
        case 4:
            modelIdx = 21;
            break;
    }

    obj = g_feModels[modelIdx];
    SetSkeletonDirectionHPR(obj->skeleton, g_modelAngle, 0, 0);
    SetSkeletonPosition(obj->skeleton, 0, 400, 796);
    obj->skeleton->super = cam->camPos;
    MatrixToN64Mtx(GetSkeletonMatrix(obj->skeleton), &g_mtxPool[g_mtxCount]);
    AddRenderGroup(g_feRenderList, obj->renderGroup, &g_mtxPool[g_mtxCount]);
    g_mtxCount++;
    return FlushRenderList(g_feRenderList, glistp);
}

Gfx *TimeTrialRecordsMenu(Gfx *glistp) {
    char timeBuf[8];
    s32 trackIdx;
    s32 halfWidth;
    s32 i;
    s32 y;

    gDPPipeSync(glistp++);
    gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);

    MenuControl(g_ttRecordsMenu);
    trackIdx = g_ttRecordsMenu[g_menuItem].value;
    glistp = CentreText(GetTrackName(trackIdx + 1), 160, 72, 8, 1, glistp);

    halfWidth = g_textEndX - 145;
    if ((g_ttRecordsMenu[g_menuItem].value < 5 && g_velocitarUnlocked == 0) ||
        (g_ttRecordsMenu[g_menuItem].value < 6 && (g_velocitarUnlocked != 0 || cheatTrack != 0))) {
        if (GetBlinkState()) {
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_arrowRightSprite));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, (halfWidth + 152) << 2, 295);
        }
    }
    if (g_ttRecordsMenu[g_menuItem].value != 0) {
        if (GetBlinkState()) {
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_arrowLeftSprite));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, (160 - halfWidth) << 2, 295);
        }
    }

    trackIdx += 8;
    glistp = CentreText("BEST LAP TIMES", 30, 94, 8, 0, glistp);

    for (i = 0; i < 4; i++) {
        s8 ship;

        glistp = DrawFontText(g_hiScoreTable[trackIdx][i].lapName, 117, 110 + i * 10, 8, glistp);
        TimeToString(g_hiScoreTable[trackIdx][i].lapTime, 1, timeBuf);
        glistp = DrawFontText(timeBuf, 150, 110 + i * 10, 8, glistp);

        ship = g_hiScoreTable[trackIdx][i].lapShip;
        switch ((s8)(ship / 3)) {
            case 0:
                glistp = DrawFontText("AURICOM", 200, 110 + i * 10, 8, glistp);
                break;
            case 1:
                glistp = DrawFontText("FEISAR", 200, 110 + i * 10, 8, glistp);
                break;
            case 2:
                glistp = DrawFontText("QIREX", 200, 110 + i * 10, 8, glistp);
                break;
            case 3:
                glistp = DrawFontText("PIRANHA II", 200, 110 + i * 10, 8, glistp);
                break;
            case 4:
                glistp = DrawFontText("AG SYSTEMS", 200, 110 + i * 10, 8, glistp);
                break;
            case 5:
                glistp = DrawFontText("WIPEOUT64", 200, 110 + i * 10, 8, glistp);
                break;
        }

        switch (i) {
            case 0:
                glistp = DrawFontText("VECTOR", 60, 110, 8, glistp);
                break;
            case 1:
                glistp = DrawFontText("VENOM", 60, 120, 8, glistp);
                break;
            case 2:
                glistp = DrawFontText("RAPIER", 60, 130, 8, glistp);
                break;
            case 3:
                glistp = DrawFontText("PHANTOM", 60, 140, 8, glistp);
                break;
        }
    }

    return DrawMenu(g_ttRecordsMenu, 130, 60, glistp, 1);
}

Gfx *RaceRecordsMenu(Gfx *glistp) {
    char timeBuf[8];
    s32 trackIdx;
    s32 halfWidth;
    s32 i;
    s32 raceY;
    s32 lapY;

    gDPPipeSync(glistp++);
    gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);

    MenuControl(g_raceRecordsMenu);
    trackIdx = g_raceRecordsMenu[g_menuItem].value;
    glistp = CentreText(GetTrackName(trackIdx + 1), 160, 72, 8, 1, glistp);

    halfWidth = g_textEndX - 145;
    if ((g_raceRecordsMenu[g_menuItem].value < 5 && g_velocitarUnlocked == 0) ||
        (g_raceRecordsMenu[g_menuItem].value < 6 && (g_velocitarUnlocked != 0 || cheatTrack != 0))) {
        if (GetBlinkState()) {
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_arrowRightSprite));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, (halfWidth + 152) << 2, 295);
        }
    }
    if (g_raceRecordsMenu[g_menuItem].value != 0) {
        if (GetBlinkState()) {
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_arrowLeftSprite));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, (160 - halfWidth) << 2, 295);
        }
    }

    glistp = CentreText("BEST RACE TIMES", 0, 87, 8, 0, glistp);
    glistp = CentreText("BEST LAP TIMES", 0, 144, 8, 0, glistp);

    for (i = 0; i < 4; i++) {
        s8 ship;

        glistp = DrawFontText(g_hiScoreTable[trackIdx][i].lapName, 117, 158 + i * 10, 8, glistp);
        glistp = DrawFontText(g_hiScoreTable[trackIdx][i].name, 117, 100 + i * 10, 8, glistp);
        TimeToString(g_hiScoreTable[trackIdx][i].lapTime, 1, timeBuf);
        glistp = DrawFontText(timeBuf, 150, 158 + i * 10, 8, glistp);
        TimeToString(g_hiScoreTable[trackIdx][i].time, 1, timeBuf);
        glistp = DrawFontText(timeBuf, 150, 100 + i * 10, 8, glistp);

        ship = g_hiScoreTable[trackIdx][i].lapShip;
        switch ((s8)(ship / 3)) {
            case 0:
                glistp = DrawFontText("AURICOM", 200, 158 + i * 10, 8, glistp);
                glistp = DrawFontText("AURICOM", 200, 100 + i * 10, 8, glistp);
                break;
            case 1:
                glistp = DrawFontText("FEISAR", 200, 158 + i * 10, 8, glistp);
                glistp = DrawFontText("FEISAR", 200, 100 + i * 10, 8, glistp);
                break;
            case 2:
                glistp = DrawFontText("QIREX", 200, 158 + i * 10, 8, glistp);
                glistp = DrawFontText("QIREX", 200, 100 + i * 10, 8, glistp);
                break;
            case 3:
                glistp = DrawFontText("PIRANHA II", 200, 158 + i * 10, 8, glistp);
                glistp = DrawFontText("PIRANHA II", 200, 100 + i * 10, 8, glistp);
                break;
            case 4:
                glistp = DrawFontText("AG SYSTEMS", 200, 158 + i * 10, 8, glistp);
                glistp = DrawFontText("AG SYSTEMS", 200, 100 + i * 10, 8, glistp);
                break;
            case 5:
                glistp = DrawFontText("WIPEOUT64", 200, 158 + i * 10, 8, glistp);
                glistp = DrawFontText("WIPEOUT64", 200, 100 + i * 10, 8, glistp);
                break;
        }

        switch (i) {
            case 0:
                glistp = DrawFontText("VECTOR", 60, 158, 8, glistp);
                glistp = DrawFontText("VECTOR", 60, 100, 8, glistp);
                break;
            case 1:
                glistp = DrawFontText("VENOM", 60, 168, 8, glistp);
                glistp = DrawFontText("VENOM", 60, 110, 8, glistp);
                break;
            case 2:
                glistp = DrawFontText("RAPIER", 60, 178, 8, glistp);
                glistp = DrawFontText("RAPIER", 60, 120, 8, glistp);
                break;
            case 3:
                glistp = DrawFontText("PHANTOM", 60, 188, 8, glistp);
                glistp = DrawFontText("PHANTOM", 60, 130, 8, glistp);
                break;
        }
    }

    return DrawMenu(g_raceRecordsMenu, 130, 60, glistp, 1);
}

Gfx *AudioConfigMenu(Gfx *glistp) {
    char artistBuf[32];
    s32 res;
    s32 endX;
    s32 halfWidth;
    s32 i;
    s32 x;
    char *artist;

    gDPPipeSync(glistp++);
    gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);

    res = MenuControl(g_audioConfigMenu);

    if (g_menuItem == 0) {
        glistp = CentreText("MUSIC TRACK", 0, 80, 8, 0, glistp);
        if (frontendOptions_g.musicTrack != g_audioConfigMenu[0].value) {
            frontendOptions_g.musicTrack = g_audioConfigMenu[0].value;
            StopAudio();
            StartMusic();
        }
        if (frontendOptions_g.musicTrack == 6) {
            glistp = CentreText("BANG ON", 160, 91, 8, 1, glistp);
        } else {
            glistp = CentreText(GetCdTrackName(frontendOptions_g.musicTrack), 160, 91, 8, 1, glistp);
        }
        endX = g_textEndX;
        halfWidth = endX - 147;
        if (GetBlinkState()) {
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_arrowRightSprite));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, (endX + 5) << 2, 372);
        }
        if (GetBlinkState()) {
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_arrowLeftSprite));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, (160 - halfWidth) << 2, 372);
        }
    } else {
        glistp = CentreText("MUSIC TRACK", 0, 80, 8, 1, glistp);
        frontendOptions_g.musicTrack = g_audioConfigMenu[0].value;
        if (frontendOptions_g.musicTrack == 6) {
            glistp = CentreText("BANG ON", 160, 91, 8, 1, glistp);
        } else {
            glistp = CentreText(GetCdTrackName(frontendOptions_g.musicTrack), 160, 91, 8, 1, glistp);
        }
    }

    /* Separate else arm prevents cse_skip_blocks from caching &frontendOptions_g in $s1 across musicTrack != 8 */
    if (frontendOptions_g.musicTrack != 8) {
        artist = GetCdTrackArtist(frontendOptions_g.currentMusicTrack);
        sprintf(artistBuf, "[%s]", artist);
        glistp = AddCentreText(artistBuf, 160, 102, 8, glistp);
    } else {
        artist = NULL;
    }

    if (g_menuItem == 1) {
        if (frontendOptions_g.musicVol != g_audioConfigMenu[1].value) {
            frontendOptions_g.musicVol = g_audioConfigMenu[1].value;
            if (frontendOptions_g.musicVol != 0) {
                SetMasterMusicVolume((frontendOptions_g.musicVol * 0x7FFF) / 116);
            } else {
                SetMasterMusicVolume(0);
            }
        }
        glistp = CentreText("MUSIC VOLUME", 0, 120, 8, 0, glistp);
    } else {
        glistp = CentreText("MUSIC VOLUME", 0, 120, 8, 1, glistp);
    }

    for (i = 0; i < 3; i++) {
        gDPPipeSync(glistp++);
        gDPSetPrimColor(glistp++, 0, 0, 255, 255, 255, 0);
        gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_volumeBarSprites[i]));
        gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
        gSPSprite2DDraw(glistp++, 372 + i * 180, 524);
    }
    x = frontendOptions_g.musicVol + 98;
    gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_volumeBarSprites[3]));
    gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
    gSPSprite2DDraw(glistp++, x << 2, 532);

    if (g_menuItem == 2) {
        if (g_audioConfigSfx == NULL) {
            g_audioConfigSfx = PlaySfx(2, 255, 128, 0, -1);
            if (g_audioConfigSfx != NULL) {
                SetSfxFreqOffset(g_audioConfigSfx, -4.0f);
            }
        }
        if (frontendOptions_g.sfxVol != g_audioConfigMenu[g_menuItem].value) {
            frontendOptions_g.sfxVol = g_audioConfigMenu[g_menuItem].value;
            if (frontendOptions_g.sfxVol != 0) {
                SetMasterSfxVolume((frontendOptions_g.sfxVol * 0x7FFF) / 116);
            } else {
                SetMasterSfxVolume(0);
            }
        }
        glistp = CentreText("SFX VOLUME", 0, 160, 8, 0, glistp);
    } else {
        StopSfx(&g_audioConfigSfx);
        glistp = CentreText("SFX VOLUME", 0, 160, 8, 1, glistp);
    }

    for (i = 0; i < 3; i++) {
        gDPPipeSync(glistp++);
        gDPSetPrimColor(glistp++, 0, 0, 255, 255, 255, 0);
        gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_volumeBarSprites[i]));
        gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
        gSPSprite2DDraw(glistp++, 372 + i * 180, 684);
    }
    x = frontendOptions_g.sfxVol + 98;
    gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_volumeBarSprites[3]));
    gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
    gSPSprite2DDraw(glistp++, x << 2, 692);

    /* Materializes constant 2 into $v1 in the delay slot of res != 1, keeping glistp += 1 at +0620 */
    if (res == 2) {
    }
    if (res == 1) {
        SetMenuTransition(g_audioConfigMenu[0].nextScreen);
        ApplyAudioConfig();
    } else if (res == 2) {
        SetMenuTransition(g_audioConfigMenu[0].nextScreen);
        RestoreAudioConfig();
    }

    return DrawMenu(g_audioConfigMenu, 130, 60, glistp, 1);
}

void LoadAudioSliderSprites(void) {
    s32 startTex;
    s32 i;

    startTex = TextureTableCount;
    LoadCmpFiles(WADLoadCompressedTextureSequence(g_feWad));

    for (i = 0; i < 4; i++) {
        guSprite2DInit(&g_volumeBarSprites[i], (void *)OS_K0_TO_PHYSICAL(g_textureTable[startTex]->pixels),
                       (void *)OS_K0_TO_PHYSICAL(g_textureTable[startTex]->clut), g_textureTable[startTex]->width,
                       g_textureTable[startTex]->width, g_textureTable[startTex++]->height, G_IM_FMT_CI, G_IM_SIZ_4b,
                       0, 0);
    }
}

void InitMultiplayerSlots(void) {
    s32 i;

    bzero(g_multiSlots, sizeof(g_multiSlots));
    memcpy(g_multiSlots[0].defaultName, "P 1", 4);
    memcpy(g_multiSlots[1].defaultName, "P 2", 4);
    memcpy(g_multiSlots[2].defaultName, "P 3", 4);
    memcpy(g_multiSlots[3].defaultName, "P 4", 4);

    for (i = 0; i < 4; i++) {
        g_multiSlots[i].screenLayout = -1;
        g_multiSlots[i].state = 1;
        g_multiSlots[i].padConfig = g_padConfigIndex[i];
        g_multiSlots[i].nameCursor = 3;
        StringCopy(g_multiSlots[i].nameEntered, g_multiSlots[i].defaultName);
        g_multiSlots[i].nameEndX = g_font12[15].width + (g_font12[28 + i].width + 8);
        g_multiSlots[i].alphabetIndex = 27;
        g_multiSlots[i].selectedChar = '\\';
    }
}

Gfx *SetupMultiViewport(s32 playerIdx, Dynamic *dp, u16 *perspNorm, Gfx *glistp) {
    if (g_multi3PlayerLayout == 1) {
        switch (playerIdx) {
            case 0:
                guPerspective(&dp->projection, perspNorm, 82.224f, 2.0f / 3.0f, 16.0f, (f32)g_farClip, 1.0f);
                guMtxCatL(&dp->projection, &g_flipYMtx, &dp->projection);
                guOrtho(&dp->ortho, -160.0f, 160.0f, -120.0f, 120.0f, 0.0f, 4000.0f, 1.0f);
                guMtxCatL(&dp->ortho, &g_flipYMtx, &dp->ortho);
                gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&dp->projection), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
                gSPPerspNormalize(glistp++, *perspNorm);
                gSPViewport(glistp++, &g_viewports[1]);
                break;
            case 1:
                guPerspective(&dp->projection, perspNorm, 82.224f, 4.0f / 3.0f, 16.0f, (f32)g_farClip, 1.0f);
                guMtxCatL(&dp->projection, &g_flipYMtx, &dp->projection);
                guOrtho(&dp->ortho, -160.0f, 160.0f, -120.0f, 120.0f, 0.0f, 4000.0f, 1.0f);
                guMtxCatL(&dp->ortho, &g_flipYMtx, &dp->ortho);
                gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&dp->projection), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
                gSPPerspNormalize(glistp++, *perspNorm);
                gSPViewport(glistp++, &g_viewports[6]);
                break;
            case 2:
                guPerspective(&dp->projection, perspNorm, 82.224f, 4.0f / 3.0f, 16.0f, (f32)g_farClip, 1.0f);
                guMtxCatL(&dp->projection, &g_flipYMtx, &dp->projection);
                guOrtho(&dp->ortho, -160.0f, 160.0f, -120.0f, 120.0f, 0.0f, 4000.0f, 1.0f);
                guMtxCatL(&dp->ortho, &g_flipYMtx, &dp->ortho);
                gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&dp->projection), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
                gSPPerspNormalize(glistp++, *perspNorm);
                gSPViewport(glistp++, &g_viewports[8]);
                break;
        }
    } else if (g_multi3PlayerLayout == 0) {
        switch (playerIdx) {
            case 0:
                guPerspective(&dp->projection, perspNorm, 82.224f, 8.0f / 3.0f, 16.0f, (f32)g_farClip, 1.0f);
                guMtxCatL(&dp->projection, &g_flipYMtx, &dp->projection);
                guOrtho(&dp->ortho, -160.0f, 160.0f, -120.0f, 120.0f, 0.0f, 4000.0f, 1.0f);
                guMtxCatL(&dp->ortho, &g_flipYMtx, &dp->ortho);
                gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&dp->projection), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
                gSPPerspNormalize(glistp++, *perspNorm);
                gSPViewport(glistp++, &g_viewports[3]);
                break;
            case 1:
                guPerspective(&dp->projection, perspNorm, 82.224f, 4.0f / 3.0f, 16.0f, (f32)g_farClip, 1.0f);
                guMtxCatL(&dp->projection, &g_flipYMtx, &dp->projection);
                guOrtho(&dp->ortho, -160.0f, 160.0f, -120.0f, 120.0f, 0.0f, 4000.0f, 1.0f);
                guMtxCatL(&dp->ortho, &g_flipYMtx, &dp->ortho);
                gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&dp->projection), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
                gSPPerspNormalize(glistp++, *perspNorm);
                gSPViewport(glistp++, &g_viewports[7]);
                break;
            case 2:
                guPerspective(&dp->projection, perspNorm, 82.224f, 4.0f / 3.0f, 16.0f, (f32)g_farClip, 1.0f);
                guMtxCatL(&dp->projection, &g_flipYMtx, &dp->projection);
                guOrtho(&dp->ortho, -160.0f, 160.0f, -120.0f, 120.0f, 0.0f, 4000.0f, 1.0f);
                guMtxCatL(&dp->ortho, &g_flipYMtx, &dp->ortho);
                gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(&dp->projection), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
                gSPPerspNormalize(glistp++, *perspNorm);
                gSPViewport(glistp++, &g_viewports[8]);
                break;
        }
    } else {
        switch (playerIdx) {
            case 0:
                gSPViewport(glistp++, &g_viewports[5]);
                break;
            case 1:
                gSPViewport(glistp++, &g_viewports[6]);
                break;
            case 2:
                gSPViewport(glistp++, &g_viewports[7]);
                break;
        }
    }
    return glistp;
}

void UpdateMultiScreenLayouts(void) {
    s32 slot;
    s32 i;

    for (i = 0; i < 4; i++) {
        g_multiSlots[i].screenLayout = -1;
    }

    switch (g_multiNumPlayers) {
        case 1:
            for (i = 0; i < 4; i++) {
                if (g_multiSlots[i].joined) {
                    g_multiSlots[i].screenLayout = 0;
                }
            }
            break;

        case 2:
            slot = 1;
            if (g_multiSplitVertical != 1) {
                slot = 3;
            }
            for (i = 0; i < 4; i++) {
                if (g_multiSlots[i].joined) {
                    g_multiSlots[i].screenLayout = slot++;
                }
            }
            break;

        case 3:
            slot = 1;
            for (i = 0; i < 4; i++) {
                if (g_multiSlots[i].joined) {
                    switch (g_multi3PlayerLayout) {
                        case 0:
                            if (slot == 1) {
                                g_multiSlots[i].screenLayout = 3;
                            } else if (slot == 2) {
                                g_multiSlots[i].screenLayout = 7;
                            } else {
                                g_multiSlots[i].screenLayout = 8;
                            }
                            break;
                        case 1:
                            if (slot == 1) {
                                g_multiSlots[i].screenLayout = 1;
                            } else if (slot == 2) {
                                g_multiSlots[i].screenLayout = 6;
                            } else {
                                g_multiSlots[i].screenLayout = 8;
                            }
                            break;
                        case 2:
                            if (slot == 1) {
                                g_multiSlots[i].screenLayout = 5;
                            } else if (slot == 2) {
                                g_multiSlots[i].screenLayout = 6;
                            } else {
                                g_multiSlots[i].screenLayout = 7;
                            }
                            break;
                    }
                    slot++;
                }
            }
            break;

        case 4:
            slot = 5;
            for (i = 0; i < 4; i++) {
                if (g_multiSlots[i].joined) {
                    g_multiSlots[i].screenLayout = slot++;
                }
            }
            break;
    }
}

void UpdateMultiPlayerInput(s32 playerIdx) {
    s32 maxShip;
    s32 firstJoined;
    s32 i;
    u16 dir;
    u16 stick;
    u16 pressed;

    maxShip = 3;
    if (g_piranhaUnlocked || cheatTeam) {
        maxShip = 4;
    }

    for (i = 0; i < 4; i++) {
        if (g_multiSlots[i].joined) {
            firstJoined = i;
            break;
        }
    }

    dir = g_padInfo[playerIdx].buttons;
    stick = g_padInfo[playerIdx].stickDirs;
    pressed = g_contButtonPressed[playerIdx];

    if (stick & 0x10) {
        dir |= 0x10;
    }
    if (stick & 0x40) {
        dir |= 0x40;
    }
    if (stick & 0x80) {
        dir |= 0x80;
    }
    if (stick & 0x20) {
        dir |= 0x20;
    }

    if ((pressed & 8) && playerIdx == firstJoined) {
        if (g_multiNumPlayers == 2) {
            g_multiSplitVertical = !g_multiSplitVertical;
            UpdateMultiScreenLayouts();
        }
    }

    switch (g_multiSlots[playerIdx].state) {
        case 1:
            if (g_multiSlots[playerIdx].ready) {
                if (pressed & B_BUTTON) {
                    g_multiSlots[playerIdx].ready = 0;
                    PlayNote(27, 255, 15, 15, 256);
                    g_multiReadyCount--;
                }
            } else if (dir & 0x10) {
                if (g_multiSlots[playerIdx].shipChoice > 0) {
                    g_multiSlots[playerIdx].shipChoice--;
                    PlayNote(26, 255, 15, 15, 256);
                }
            } else if (dir & 0x40) {
                if (g_multiSlots[playerIdx].shipChoice < maxShip) {
                    g_multiSlots[playerIdx].shipChoice++;
                    PlayNote(26, 255, 15, 15, 256);
                }
            } else if (pressed & A_BUTTON) {
                g_multiSlots[playerIdx].ready = 1;
                PlayNote(27, 255, 15, 15, 256);
                g_multiReadyCount++;
            } else if (pressed & B_BUTTON) {
                g_multiSlots[playerIdx].joined = 0;
                PlayNote(27, 255, 15, 15, 256);
                g_multiNumPlayers--;
                UpdateMultiScreenLayouts();
            } else if (pressed & 0x20) {
                g_multiSlots[playerIdx].state = 2;
                g_multiSlots[playerIdx].enteringName = 1;
                g_multiSlots[playerIdx].storeRecords = 1;
            } else if (pressed & 0x10) {
                g_multiSlots[playerIdx].state = 3;
                g_multiSlots[playerIdx].savedPadConfig = g_multiSlots[playerIdx].padConfig;
            }
            break;

        case 2:
            g_multiSlots[playerIdx].alphabetIndex = g_multiSlots[playerIdx].selectedChar - 'A';
            if ((dir & 0x20) && g_multiSlots[playerIdx].inputDelay == 0) {
                PlayNote(26, 255, 15, 15, 256);
                g_multiSlots[playerIdx].inputDelay = 5;
                g_multiSlots[playerIdx].alphabetIndex++;
                if (g_multiSlots[playerIdx].alphabetIndex >= 28) {
                    if (g_multiSlots[playerIdx].nameCursor != 3) {
                        g_multiSlots[playerIdx].alphabetIndex = 0;
                    } else {
                        g_multiSlots[playerIdx].alphabetIndex = 26;
                    }
                }
            }
            if ((dir & 0x80) && g_multiSlots[playerIdx].inputDelay == 0) {
                PlayNote(26, 255, 15, 15, 256);
                g_multiSlots[playerIdx].inputDelay = 5;
                g_multiSlots[playerIdx].alphabetIndex--;
                if (g_multiSlots[playerIdx].nameCursor != 3) {
                    g_multiSlots[playerIdx].firstAlphabetIndex = 0;
                } else {
                    g_multiSlots[playerIdx].firstAlphabetIndex = 26;
                }
                if (g_multiSlots[playerIdx].alphabetIndex < g_multiSlots[playerIdx].firstAlphabetIndex) {
                    g_multiSlots[playerIdx].alphabetIndex = 27;
                }
            }

            if (g_multiSlots[playerIdx].inputDelay > 0) {
                g_multiSlots[playerIdx].inputDelay--;
            }

            if ((pressed & A_BUTTON) && g_multiSlots[playerIdx].alphabetIndex == 26) {
                if (g_multiSlots[playerIdx].nameCursor > 0) {
                    PlayNote(27, 255, 15, 15, 256);
                    g_multiSlots[playerIdx].nameCursor--;
                    g_multiSlots[playerIdx].nameEndX -=
                        g_font12[g_multiSlots[playerIdx].nameEntered[g_multiSlots[playerIdx].nameCursor] - 'A'].width;
                    g_multiSlots[playerIdx].nameEntered[g_multiSlots[playerIdx].nameCursor] = 0;
                    g_multiSlots[playerIdx].alphabetIndex = 0;
                }
            } else {
                g_multiSlots[playerIdx].selectedChar = g_multiSlots[playerIdx].alphabetIndex + 'A';
                if ((pressed & A_BUTTON) && g_multiSlots[playerIdx].nameCursor != 4 &&
                    g_multiSlots[playerIdx].alphabetIndex != 26 && g_multiSlots[playerIdx].alphabetIndex != 27) {
                    PlayNote(27, 255, 15, 15, 256);
                    g_multiSlots[playerIdx].nameEntered[g_multiSlots[playerIdx].nameCursor] =
                        g_multiSlots[playerIdx].selectedChar;
                    g_multiSlots[playerIdx].nameEntered[g_multiSlots[playerIdx].nameCursor + 1] = 0;
                    g_multiSlots[playerIdx].nameCursor++;
                    g_multiSlots[playerIdx].nameEndX += g_font12[g_multiSlots[playerIdx].alphabetIndex].width;
                    if (g_multiSlots[playerIdx].nameCursor == 3) {
                        g_multiSlots[playerIdx].alphabetIndex = 27;
                        g_multiSlots[playerIdx].selectedChar = '\\';
                    }
                } else if ((pressed & A_BUTTON) && g_multiSlots[playerIdx].alphabetIndex == 27) {
                    PlayNote(27, 255, 15, 15, 256);
                    g_multiSlots[playerIdx].nameEntered[g_multiSlots[playerIdx].nameCursor] =
                        g_multiSlots[playerIdx].selectedChar;
                    g_multiSlots[playerIdx].nameEntered[g_multiSlots[playerIdx].nameCursor + 1] = 0;
                    g_multiSlots[playerIdx].enteringName = 0;
                    g_contButtonPressed[playerIdx] = 0;
                }
            }
            break;

        case 3:
            if (pressed & A_BUTTON) {
                g_multiSlots[playerIdx].state = 1;
            } else if (pressed & B_BUTTON) {
                g_multiSlots[playerIdx].state = 1;
                g_multiSlots[playerIdx].padConfig = g_multiSlots[playerIdx].savedPadConfig;
            } else if (dir & 0x80) {
                if (g_multiSlots[playerIdx].padConfig > 0) {
                    g_multiSlots[playerIdx].padConfig--;
                }
            } else if (dir & 0x20) {
                if (g_multiSlots[playerIdx].padConfig < 3) {
                    g_multiSlots[playerIdx].padConfig++;
                }
            }
            break;
    }
}

Gfx *DrawMultiPlayerSlot(s32 playerIdx, Gfx *glistp) {
    char buf[16];
    s32 layout;
    s32 nameX;
    s32 nameY;
    s32 centerX;
    s32 listY;
    s32 titleY;
    s32 waitY;
    s32 lineStep;
    s32 row;
    s32 arrowPad;
    s32 arrowYOff;
    s32 halfWidth;
    s32 colLeft;
    s32 colRight;

    layout = g_multiSlots[playerIdx].screenLayout;
    switch (layout) {
        case 0:
            nameX = 134;
            nameY = 130;
            centerX = 160;
            listY = 110;
            titleY = 80;
            waitY = 110;
            lineStep = 12;
            break;
        case 1:
            nameX = 50;
            nameY = 140;
            centerX = 80;
            listY = 125;
            titleY = 100;
            waitY = 110;
            lineStep = 7;
            break;
        case 2:
            nameX = 212;
            nameY = 140;
            centerX = 240;
            listY = 125;
            titleY = 100;
            waitY = 110;
            lineStep = 7;
            break;
        case 3:
            nameX = 135;
            nameY = 65;
            centerX = 160;
            listY = 50;
            titleY = 25;
            waitY = 55;
            lineStep = 7;
            break;
        case 4:
            nameX = 135;
            nameY = 180;
            centerX = 160;
            listY = 165;
            titleY = 140;
            waitY = 170;
            lineStep = 7;
            break;
        case 5:
            nameX = 50;
            nameY = 65;
            centerX = 80;
            listY = 50;
            titleY = 25;
            waitY = 55;
            lineStep = 7;
            break;
        case 6:
            nameX = 212;
            nameY = 65;
            centerX = 240;
            listY = 50;
            titleY = 25;
            waitY = 55;
            lineStep = 7;
            break;
        case 7:
            nameX = 50;
            nameY = 180;
            centerX = 80;
            listY = 165;
            titleY = 140;
            waitY = 170;
            lineStep = 7;
            break;
        case 8:
            nameX = 212;
            nameY = 180;
            centerX = 240;
            listY = 165;
            titleY = 140;
            waitY = 170;
            lineStep = 7;
            break;
    }

    UpdateMultiPlayerInput(playerIdx);

    if (g_multiSlots[playerIdx].ready) {
#ifndef VERSION_EU
        if (!GetBlinkState()) {
            return glistp;
        }
#endif
        if (layout == 0) {
            glistp = CentreTextAt("WAITING FOR OTHER", centerX, waitY, 8, 1, glistp);
            glistp = CentreTextAt("PLAYERS TO JOIN", centerX, waitY + lineStep, 8, 1, glistp);
        } else {
            gDPPipeSync(glistp++);
            gDPSetPrimColor(glistp++, 0, 0, 255, 255, 255, 0);
            gDPSetCombineMode(glistp++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);
            glistp = AddCentreText("WAITING FOR OTHER", centerX, waitY, 8, glistp);
            glistp = AddCentreText("PLAYERS TO JOIN", centerX, waitY + lineStep, 8, glistp);
        }
        return glistp;
    }

    sprintf(buf, "PLAYER %i", playerIdx + 1);
    if (layout == 0) {
        glistp = CentreTextAt(buf, centerX, titleY, 8, 1, glistp);
        row = 1;
        glistp = CentreTextAt(g_multiSlots[playerIdx].defaultName, centerX, titleY + lineStep, 8, 1, glistp);
    } else {
        gDPPipeSync(glistp++);
        gDPSetPrimColor(glistp++, 0, 0, 255, 255, 255, 0);
        gDPSetCombineMode(glistp++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);
        glistp = CentreTextAt(buf, centerX, titleY, 8, 1, glistp);
        row = 2;
        glistp = AddCentreText(g_multiSlots[playerIdx].defaultName, centerX, titleY + lineStep * 2, 8, glistp);
    }

    switch (g_multiSlots[playerIdx].state) {
        case 1:
            if (layout == 0) {
                row = 0;
                glistp = CentreTextAt("FEISAR", centerX, listY + lineStep * row, 8,
                                      g_multiSlots[playerIdx].shipChoice != row, glistp);
                row++;
                glistp = CentreTextAt("AG SYSTEMS", centerX, listY + lineStep * row, 8,
                                      g_multiSlots[playerIdx].shipChoice != row, glistp);
                row++;
                glistp = CentreTextAt("AURICOM", centerX, listY + lineStep * row, 8,
                                      g_multiSlots[playerIdx].shipChoice != row, glistp);
                row++;
                glistp = CentreTextAt("QIREX", centerX, listY + lineStep * row, 8,
                                      g_multiSlots[playerIdx].shipChoice != row, glistp);
                row++;
                if (g_piranhaUnlocked || cheatTeam) {
                    glistp = CentreTextAt("PIRANHA II", centerX, listY + lineStep * row, 8,
                                          g_multiSlots[playerIdx].shipChoice != row, glistp);
                } else {
                    glistp = CentreTextAt("PIRANHA II", centerX, listY + lineStep * row, 8, 2, glistp);
                }
            } else {
                row = 0;
                gDPPipeSync(glistp++);
                gDPSetPrimColor(glistp++, 0, 0, 255, 255, g_multiSlots[playerIdx].shipChoice == row ? 0 : 255, 0);
                gDPSetCombineMode(glistp++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);
                glistp = AddCentreText("FEISAR", centerX, listY + lineStep * row, 8, glistp);
                row++;

                gDPPipeSync(glistp++);
                gDPSetPrimColor(glistp++, 0, 0, 255, 255, g_multiSlots[playerIdx].shipChoice == row ? 0 : 255, 0);
                gDPSetCombineMode(glistp++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);
                glistp = AddCentreText("AG SYSTEMS", centerX, listY + lineStep * row, 8, glistp);
                row++;

                gDPPipeSync(glistp++);
                gDPSetPrimColor(glistp++, 0, 0, 255, 255, g_multiSlots[playerIdx].shipChoice == row ? 0 : 255, 0);
                gDPSetCombineMode(glistp++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);
                glistp = AddCentreText("AURICOM", centerX, listY + lineStep * row, 8, glistp);
                row++;

                gDPPipeSync(glistp++);
                gDPSetPrimColor(glistp++, 0, 0, 255, 255, g_multiSlots[playerIdx].shipChoice == row ? 0 : 255, 0);
                gDPSetCombineMode(glistp++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);
                glistp = AddCentreText("QIREX", centerX, listY + lineStep * row, 8, glistp);
                row++;

                if (g_piranhaUnlocked || cheatTeam) {
                    gDPPipeSync(glistp++);
                    gDPSetPrimColor(glistp++, 0, 0, 255, 255, g_multiSlots[playerIdx].shipChoice == row ? 0 : 255, 0);
                    gDPSetCombineMode(glistp++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);
                    glistp = AddCentreText("PIRANHA II", centerX, listY + lineStep * row, 8, glistp);
                } else {
                    gDPPipeSync(glistp++);
                    gDPSetPrimColor(glistp++, 0, 0, 50, 50, 50, 0);
                    gDPSetCombineMode(glistp++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);
                    glistp = AddCentreText("PIRANHA II", centerX, listY + lineStep * row, 8, glistp);
                }
            }
            break;

        case 2:
            if (g_multiSlots[playerIdx].nameCursor != 4 && g_multiSlots[playerIdx].enteringName) {
                g_multiSlots[playerIdx].dispChar[1] = 0;
                g_multiSlots[playerIdx].dispChar[0] = g_multiSlots[playerIdx].selectedChar;
                if (g_multiSlots[playerIdx].enteringName) {
                    if (g_multiFrameCounter == (g_multiFrameCounter / 3) * 3) {
                        if (g_multiSlots[playerIdx].alphabetIndex == 26 ||
                            g_multiSlots[playerIdx].alphabetIndex == 27) {
                            glistp = AddDelEnd(g_multiSlots[playerIdx].alphabetIndex - 26,
                                               g_multiSlots[playerIdx].nameEndX + nameX, nameY, 16, 1, glistp);
                        } else {
                            glistp = DrawHudText((char *)g_multiSlots[playerIdx].dispChar,
                                                 g_multiSlots[playerIdx].nameEndX + nameX, nameY, 12, 1, glistp);
                        }
                    } else {
                        if (g_multiSlots[playerIdx].alphabetIndex == 26 ||
                            g_multiSlots[playerIdx].alphabetIndex == 27) {
                            glistp = AddDelEnd(g_multiSlots[playerIdx].alphabetIndex - 26,
                                               g_multiSlots[playerIdx].nameEndX + nameX, nameY, 16, 0, glistp);
                        } else {
                            glistp = DrawHudText((char *)g_multiSlots[playerIdx].dispChar,
                                                 g_multiSlots[playerIdx].nameEndX + nameX, nameY, 12, 0, glistp);
                        }
                    }
                }
            } else if (g_multiSlots[playerIdx].storeRecords) {
                g_multiSlots[playerIdx].enteringName = 0;
                g_multiSlots[playerIdx].nameEntered[g_multiSlots[playerIdx].nameCursor] = 0;
                StringCopy(g_multiSlots[playerIdx].defaultName, g_multiSlots[playerIdx].nameEntered);
                g_multiSlots[playerIdx].storeRecords = 0;
                g_multiSlots[playerIdx].state = 1;
            }

            if (g_multiSlots[playerIdx].enteringName) {
                glistp = DrawHudText(g_multiSlots[playerIdx].nameEntered, nameX, nameY, 12, 1, glistp);
                if (layout == 0) {
                    glistp = CentreTextAt("ENTER YOUR NAME", centerX, listY, 8, 1, glistp);
                } else {
                    glistp = AddCentreText("ENTER YOUR NAME", centerX, listY, 8, glistp);
                }
            }
            break;

        case 3:
            sprintf(buf, "CONFIG %i", g_multiSlots[playerIdx].padConfig + 1);
            if (layout == 0) {
                glistp = CentreTextAt(buf, centerX, listY, 8, 1, glistp);
                row++;
                arrowPad = 15;
                arrowYOff = 2;
            } else {
                glistp = AddCentreText(buf, centerX, listY, 8, glistp);
                row++;
                arrowPad = 12;
                arrowYOff = 0;
            }

            halfWidth = (g_textEndX - centerX) + arrowPad;
            if (g_multiSlots[playerIdx].padConfig < 3 && GetBlinkState()) {
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_arrowRightSprite));
                gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
                gSPSprite2DDraw(glistp++, (centerX + halfWidth - 8) << 2, (listY + arrowYOff) << 2);
            }
            if (g_multiSlots[playerIdx].padConfig > 0 && GetBlinkState()) {
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_arrowLeftSprite));
                gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
                gSPSprite2DDraw(glistp++, (centerX - halfWidth) << 2, (listY + arrowYOff) << 2);
            }

            row += 2;
            if (layout == 0) {
                glistp = DrawFontText("THRUST", centerX - lineStep * 5, titleY + row * lineStep, 8, glistp);
                glistp = DrawFontText(GetButtonName(0x4000, g_multiSlots[playerIdx].padConfig), centerX + lineStep,
                                      titleY + row * lineStep, 8, glistp);
                row++;
                glistp = DrawFontText("FIRE", centerX - lineStep * 5, titleY + row * lineStep, 8, glistp);
                glistp = DrawFontText(GetButtonName(0x2000, g_multiSlots[playerIdx].padConfig), centerX + lineStep,
                                      titleY + row * lineStep, 8, glistp);
                row++;
                glistp = DrawFontText("DISCARD", centerX - lineStep * 5, titleY + row * lineStep, 8, glistp);
                glistp = DrawFontText(GetButtonName(0x8000, g_multiSlots[playerIdx].padConfig), centerX + lineStep,
                                      titleY + row * lineStep, 8, glistp);
                row++;
                glistp = DrawFontText("BRAKE LEFT", centerX - lineStep * 5, titleY + row * lineStep, 8, glistp);
                glistp = DrawFontText(GetButtonName(0x400, g_multiSlots[playerIdx].padConfig), centerX + lineStep,
                                      titleY + row * lineStep, 8, glistp);
                row++;
                glistp = DrawFontText("BRAKE RIGHT", centerX - lineStep * 5, titleY + row * lineStep, 8, glistp);
                glistp = DrawFontText(GetButtonName(0x800, g_multiSlots[playerIdx].padConfig), centerX + lineStep,
                                      titleY + row * lineStep, 8, glistp);
                row++;
                glistp = DrawFontText("VIEW", centerX - lineStep * 5, titleY + row * lineStep, 8, glistp);
                glistp = DrawFontText(GetButtonName(0x1000, g_multiSlots[playerIdx].padConfig), centerX + lineStep,
                                      titleY + row * lineStep, 8, glistp);
            } else {
                glistp = DrawFontText("THRUST", centerX - lineStep * 7, titleY + row * lineStep, 8, glistp);
                glistp = DrawFontText(GetButtonName(0x4000, g_multiSlots[playerIdx].padConfig), centerX + lineStep * 3,
                                      titleY + row * lineStep, 8, glistp);
                row++;
                glistp = DrawFontText("FIRE", centerX - lineStep * 7, titleY + row * lineStep, 8, glistp);
                glistp = DrawFontText(GetButtonName(0x2000, g_multiSlots[playerIdx].padConfig), centerX + lineStep * 3,
                                      titleY + row * lineStep, 8, glistp);
                row++;
                glistp = DrawFontText("DISCARD", centerX - lineStep * 7, titleY + row * lineStep, 8, glistp);
                glistp = DrawFontText(GetButtonName(0x8000, g_multiSlots[playerIdx].padConfig), centerX + lineStep * 3,
                                      titleY + row * lineStep, 8, glistp);
                row++;
                glistp = DrawFontText("BRAKE LEFT", centerX - lineStep * 7, titleY + row * lineStep, 8, glistp);
                glistp = DrawFontText(GetButtonName(0x400, g_multiSlots[playerIdx].padConfig), centerX + lineStep * 3,
                                      titleY + row * lineStep, 8, glistp);
                row++;
                glistp = DrawFontText("BRAKE RIGHT", centerX - lineStep * 7, titleY + row * lineStep, 8, glistp);
                glistp = DrawFontText(GetButtonName(0x800, g_multiSlots[playerIdx].padConfig), centerX + lineStep * 3,
                                      titleY + row * lineStep, 8, glistp);
                row++;
                glistp = DrawFontText("VIEW", centerX - lineStep * 7, titleY + row * lineStep, 8, glistp);
                glistp = DrawFontText(GetButtonName(0x1000, g_multiSlots[playerIdx].padConfig), centerX + lineStep * 3,
                                      titleY + row * lineStep, 8, glistp);
            }
            break;
    }

    return glistp;
}

s32 ShipChoiceToShipIdx(u32 choice) {
    s32 shipIdx;

    switch (choice) {
        case 0:
            shipIdx = 3;
            break;
        case 1:
            shipIdx = 12;
            break;
        case 2:
            shipIdx = 0;
            break;
        case 3:
            shipIdx = 6;
            break;
        case 4:
            shipIdx = 9;
            break;
        default:
            ASSERT(TRUE == FALSE, 10094);
            break;
    }
    return shipIdx;
}

Gfx *MultiplayerStartMenu(Gfx *glistp) {
    u8 sameTeam[4];
    u8 i;
    s32 p;
    s32 slot;
    u8 teamBase;
    u8 sameIdx;
    u8 done;

    done = 0;
    for (i = 0; i < 15; i++) {
        g_leagueResults[i].first = 0;
        g_leagueResults[i].second = 0;
        g_leagueResults[i].third = 0;
        g_leagueResults[i].eliminations = 0;
        g_leagueResults[i].points = 0;
    }

    if (g_multiSlotsInit) {
        InitMultiplayerSlots();
        g_multiSlotsInit = 0;
    }

    g_multiFrameCounter++;

    for (p = 0; p < 4; p++) {
        if ((g_contEnabled >> p) & 1) {
            if (!g_multiSlots[p].joined && (g_padInfo[p].buttons & 8)) {
                g_multiSlots[p].padConfig = g_padConfigIndex[p];
                g_multiSlots[p].joined = 1;
                g_multiNumPlayers++;
                g_padInfo[p].buttons &= ~8;
                PlayNote(27, 255, 15, 15, 256);
                UpdateMultiScreenLayouts();
            } else if ((g_contButtonPressed[p] & B_BUTTON) && g_multiNumPlayers == 0) {
                PlayNote(27, 255, 15, 15, 256);
                g_menuScreen = 4;
                return glistp;
            }
        }
    }

    if (g_multiNumPlayers == 0) {
        if (GetBlinkState()) {
            glistp = CentreText("PLAYERS PRESS START TO JOIN", 0, 120, 8, 1, glistp);
        }
        return glistp;
    }

    for (p = 0; p < 4; p++) {
        if (g_multiSlots[p].joined) {
            glistp = DrawMultiPlayerSlot(p, glistp);
        }
    }

    if (g_multiNumPlayers >= 2 && g_multiReadyCount == g_multiNumPlayers) {
        frontendOptions_g.numPlayers = g_multiNumPlayers;
        slot = 0;
        for (p = 0; p < 4; p++) {
            if (g_multiSlots[p].joined) {
                frontendOptions_g.ownShip[slot] = ShipChoiceToShipIdx(g_multiSlots[p].shipChoice);
                frontendOptions_g.screenLayout[slot] = g_multiSlots[p].screenLayout;
                frontendOptions_g.contPort[slot] = p;
                g_padConfigIndex[slot] = g_multiSlots[p].padConfig;
                if (slot == 0) {
                    g_controllerConfigMenu[0].value = g_multiSlots[0].padConfig;
                }
                StringCopy(frontendOptions_g.playerNames[slot], g_multiSlots[p].defaultName);
                slot++;
            }
        }

        for (i = 0; i < 4; i++) {
            sameTeam[i] = 0;
        }

        teamBase = 0;
        while (!done) {
            for (i = 0; i < frontendOptions_g.numPlayers; i++) {
                if (frontendOptions_g.ownShip[i] == teamBase) {
                    sameTeam[i] = 1;
                }
            }
            sameIdx = 0;
            for (i = 0; i < frontendOptions_g.numPlayers; i++) {
                if (sameTeam[i]) {
                    if (sameIdx == 3) {
                        if (teamBase == 12) {
                            g_fourPlayerSameTeam = 1;
                            frontendOptions_g.ownShip[i] = 3;
                        } else {
                            g_fourPlayerSameTeam = 1;
                            frontendOptions_g.ownShip[i] = 12;
                        }
                    } else {
                        frontendOptions_g.ownShip[i] = teamBase + sameIdx;
                    }
                    sameIdx++;
                }
            }
            for (i = 0; i < frontendOptions_g.numPlayers; i++) {
                sameTeam[i] = 0;
            }
            switch (teamBase) {
                case 0:
                    teamBase = 3;
                    break;
                case 3:
                    teamBase = 6;
                    break;
                case 6:
                    teamBase = 9;
                    break;
                case 9:
                    teamBase = 12;
                    break;
                case 12:
                    done = 1;
                    break;
            }
        }

        g_menuScreen = 1;
        g_multiNumPlayers = 0;
        g_multiReadyCount = 0;
        for (p = 0; p < 4; p++) {
            g_multiSlots[p].joined = 0;
            g_multiSlots[p].ready = 0;
            g_multiSlots[p].state = 1;
        }
        g_demoMode = 0;
        frontendOptions_g.leagueRacesLeft = frontendOptions_g.leagueRaces;
        StartGame();
    }

    return glistp;
}

Gfx *DrawChallengeInfo(MenuItem *menu, Gfx *glistp) {
    s32 spriteIdx;
    s32 weaponCount;
    u8 cls;

    glistp = CentreText(GetTrackName(frontendOptions_g.challengeData[frontendOptions_g.challengeIndex].trackNo), 0, 76,
                        8, 1, glistp);

    glistp = DrawFontText(GetClassName(frontendOptions_g.challengeData[frontendOptions_g.challengeIndex].teamClass), 222,
                          110, 8, glistp);

    switch (frontendOptions_g.challengeData[frontendOptions_g.challengeIndex].teamClass) {
        case 0:
            spriteIdx = 44;
            break;
        case 1:
            spriteIdx = 45;
            break;
        case 2:
            spriteIdx = 46;
            break;
        case 3:
            spriteIdx = 47;
            break;
    }

    gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_menuSprites[spriteIdx]));
    gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
    gSPSprite2DDraw(glistp++, 820, 421);

    gDPPipeSync(glistp++);
    gDPSetCombineMode(glistp++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetPrimColor(glistp++, 0, 0, 255, 145, 0, 255);
    gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(g_medalSprite));
    gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
    gSPSprite2DDraw(glistp++, 200, 455);

    if (frontendOptions_g.challengeIndex != 6) {
        gDPPipeSync(glistp++);
        gDPSetCombineMode(glistp++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColor(glistp++, 0, 0, 255, 255, 255, 255);
        gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(g_medalSprite));
        gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
        gSPSprite2DDraw(glistp++, 200, 555);

        gDPPipeSync(glistp++);
        gDPSetCombineMode(glistp++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        gDPSetPrimColor(glistp++, 0, 0, 155, 45, 0, 255);
        gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(g_medalSprite));
        gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
        gSPSprite2DDraw(glistp++, 200, 655);
    }

    spriteIdx = 24;
    switch (frontendOptions_g.challengeData[frontendOptions_g.challengeIndex].allowedTeams) {
        case 1:
            spriteIdx = 24;
            glistp = DrawFontText("FEISAR", 222, 135, 8, glistp);
            break;
        case 8:
            spriteIdx = 25;
            glistp = DrawFontText("AG SYSTEMS", 222, 135, 8, glistp);
            break;
        case 4:
            spriteIdx = 26;
            glistp = DrawFontText("AURICOM", 222, 135, 8, glistp);
            break;
        case 2:
            spriteIdx = 27;
            glistp = DrawFontText("QIREX", 222, 135, 8, glistp);
            break;
        case 16:
            spriteIdx = 28;
            glistp = DrawFontText("PIRANHA II", 222, 135, 8, glistp);
            break;
    }

    gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_menuSprites[spriteIdx]));
    gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
    gSPSprite2DDraw(glistp++, 820, 524);

    weaponCount = 0;
    if (frontendOptions_g.challengeData[frontendOptions_g.challengeIndex].weaponsMask != 0) {
        if ((frontendOptions_g.challengeData == g_weaponChallenges && frontendOptions_g.challengeIndex == 6) ||
            frontendOptions_g.challengeData == g_superComboChallenges) {
            g_weaponPulseAlpha -= 20;
            if (g_weaponPulseAlpha < 51) {
                g_weaponPulseAlpha = 200;
            }
            gDPPipeSync(glistp++);
            gDPSetPrimColor(glistp++, 0, 0, 255, 255, 255, g_weaponPulseAlpha);
            gDPSetCombineMode(glistp++, G_CC_DECALRGB_PRIMA, G_CC_DECALRGB_PRIMA);
        }

        if (frontendOptions_g.challengeData[frontendOptions_g.challengeIndex].weaponsMask & 0x80) {
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_menuSprites[29]));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, 820 + (weaponCount % 5) * 53, 648 + (weaponCount / 5) * 53);
            weaponCount++;
        }
        if (frontendOptions_g.challengeData[frontendOptions_g.challengeIndex].weaponsMask & 0x04) {
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_menuSprites[30]));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, 820 + (weaponCount % 5) * 53, 648 + (weaponCount / 5) * 53);
            weaponCount++;
        }
        if (frontendOptions_g.challengeData[frontendOptions_g.challengeIndex].weaponsMask & 0x400) {
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_menuSprites[31]));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, 820 + (weaponCount % 5) * 53, 648 + (weaponCount / 5) * 53);
            weaponCount++;
        }
        if (frontendOptions_g.challengeData[frontendOptions_g.challengeIndex].weaponsMask & 0x08) {
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_menuSprites[32]));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, 820 + (weaponCount % 5) * 53, 648 + (weaponCount / 5) * 53);
            weaponCount++;
        }
        if (frontendOptions_g.challengeData[frontendOptions_g.challengeIndex].weaponsMask & 0x01) {
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_menuSprites[33]));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, 820 + (weaponCount % 5) * 53, 648 + (weaponCount / 5) * 53);
            weaponCount++;
        }
        if (frontendOptions_g.challengeData[frontendOptions_g.challengeIndex].weaponsMask & 0x20) {
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_menuSprites[34]));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, 820 + (weaponCount % 5) * 53, 648 + (weaponCount / 5) * 53);
            weaponCount++;
        }
        if (frontendOptions_g.challengeData[frontendOptions_g.challengeIndex].weaponsMask & 0x02) {
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_menuSprites[35]));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, 820 + (weaponCount % 5) * 53, 648 + (weaponCount / 5) * 53);
            weaponCount++;
        }
        if (frontendOptions_g.challengeData[frontendOptions_g.challengeIndex].weaponsMask & 0x200) {
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_menuSprites[36]));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, 820 + (weaponCount % 5) * 53, 648 + (weaponCount / 5) * 53);
            weaponCount++;
        }
        if (frontendOptions_g.challengeData[frontendOptions_g.challengeIndex].weaponsMask & 0x10) {
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_menuSprites[37]));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, 820 + (weaponCount % 5) * 53, 648 + (weaponCount / 5) * 53);
            weaponCount++;
        }
        if (frontendOptions_g.challengeData[frontendOptions_g.challengeIndex].weaponsMask & 0x100) {
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_menuSprites[38]));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, 820 + (weaponCount % 5) * 53, 648 + (weaponCount / 5) * 53);
            weaponCount++;
        }
        if (frontendOptions_g.challengeData[frontendOptions_g.challengeIndex].weaponsMask & 0x40) {
            switch (frontendOptions_g.challengeData[frontendOptions_g.challengeIndex].allowedTeams) {
                case 1:
                    spriteIdx = 39;
                    break;
                case 8:
                    spriteIdx = 40;
                    break;
                case 4:
                    spriteIdx = 41;
                    break;
                case 2:
                    spriteIdx = 42;
                    break;
                case 16:
                    spriteIdx = 43;
                    break;
            }
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_menuSprites[spriteIdx]));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, 820 + (weaponCount % 5) * 53, 648 + (weaponCount / 5) * 53);
        }

        if ((frontendOptions_g.challengeData == g_weaponChallenges && frontendOptions_g.challengeIndex == 6) ||
            frontendOptions_g.challengeData == g_superComboChallenges) {
            gDPPipeSync(glistp++);
            gDPSetCombineMode(glistp++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        }
    } else {
        glistp = AddCentreText("NO WEAPONS", 240, 162, 8, glistp);
    }

    gDPPipeSync(glistp++);
    gDPSetPrimColor(glistp++, 0, 0, 255, 255, 0, 0);
    gDPSetCombineMode(glistp++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);
    glistp = AddCentreText("CLASS:", 240, 100, 8, glistp);
    glistp = AddCentreText("TARGET:", 100, 100, 8, glistp);
    glistp = AddCentreText("TEAM:", 240, 125, 8, glistp);
    glistp = AddCentreText("WEAPONS:", 240, 152, 8, glistp);

    gDPPipeSync(glistp++);
    gDPSetPrimColor(glistp++, 0, 0, 255, 255, 255, 0);
    gDPSetCombineMode(glistp++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);

    return DrawMenu(menu, 130, 60, glistp, 1);
}

Gfx *RaceChallengeBriefing(Gfx *glistp) {
    u32 teams;

    if (!g_challengeTeamMenuInit) {
        InitChallengeTeamMenu(g_raceChallengeTeamMenu, g_raceChallenges);
        g_challengeTeamMenuInit = TRUE;
    }

    gDPPipeSync(glistp++);
    gDPSetPrimColor(glistp++, 0, 0, 255, 255, 255, 0);
    gDPSetCombineMode(glistp++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);

    glistp = AddCentreText("FINISH 1ST", g_briefingTextX, g_briefingTextY, 8, glistp);
    if (frontendOptions_g.challengeIndex != 6) {
        glistp = AddCentreText("FINISH 2ND", g_briefingTextX, g_briefingTextY + g_briefingTextStep, 8, glistp);
        glistp = AddCentreText("FINISH 3RD", g_briefingTextX, g_briefingTextY + g_briefingTextStep * 2, 8, glistp);
    }

    glistp = DrawChallengeInfo(g_raceChallengeBriefingMenu, glistp);

    if (MenuControl(g_raceChallengeBriefingMenu) == 1) {
        teams = frontendOptions_g.challengeData[frontendOptions_g.challengeIndex].allowedTeams;
        if (teams & 1) {
            frontendOptions_g.team = 3;
        } else if (teams & 8) {
            frontendOptions_g.team = 12;
        } else if (teams & 4) {
            frontendOptions_g.team = 0;
        } else if (teams & 2) {
            frontendOptions_g.team = 6;
        } else if (teams & 16) {
            frontendOptions_g.team = 9;
        }
        g_demoMode = 0;
        frontendOptions_g.ownShip[0] = frontendOptions_g.team;
        g_menuScreen = g_raceChallengeTeamMenu[g_menuItem].nextScreen;
        StartGame();
    }

    return glistp;
}

Gfx *TimeTrialChallengeBriefing(Gfx *glistp) {
    char buf[8];
    u32 teams;

    if (!g_challengeTeamMenuInit) {
        InitChallengeTeamMenu(g_ttChallengeTeamMenu, g_ttChallenges);
        g_challengeTeamMenuInit = TRUE;
    }

    gDPPipeSync(glistp++);
    gDPSetPrimColor(glistp++, 0, 0, 255, 255, 255, 0);
    gDPSetCombineMode(glistp++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);

    TimeToString((s32)g_ttChallenges[frontendOptions_g.challengeIndex].goldTarget, 1, buf);
    glistp = AddCentreText(buf, g_briefingTextX, g_briefingTextY, 8, glistp);
    if (frontendOptions_g.challengeIndex != 6) {
        TimeToString((s32)g_ttChallenges[frontendOptions_g.challengeIndex].silverTarget, 1, buf);
        glistp = AddCentreText(buf, g_briefingTextX, g_briefingTextY + g_briefingTextStep, 8, glistp);
        TimeToString((s32)g_ttChallenges[frontendOptions_g.challengeIndex].bronzeTarget, 1, buf);
        glistp = AddCentreText(buf, g_briefingTextX, g_briefingTextY + g_briefingTextStep * 2, 8, glistp);
    }

    glistp = DrawChallengeInfo(g_ttChallengeBriefingMenu, glistp);

    if (MenuControl(g_ttChallengeBriefingMenu) == 1) {
        teams = frontendOptions_g.challengeData[frontendOptions_g.challengeIndex].allowedTeams;
        if (teams & 1) {
            frontendOptions_g.team = 3;
        } else if (teams & 8) {
            frontendOptions_g.team = 12;
        } else if (teams & 4) {
            frontendOptions_g.team = 0;
        } else if (teams & 2) {
            frontendOptions_g.team = 6;
        } else if (teams & 16) {
            frontendOptions_g.team = 9;
        }
        g_demoMode = 0;
        frontendOptions_g.ownShip[0] = frontendOptions_g.team;
        g_menuScreen = g_ttChallengeTeamMenu[g_menuItem].nextScreen;
        StartGame();
    }

    return glistp;
}

Gfx *WeaponChallengeBriefing(Gfx *glistp) {
    char buf[16];
    u32 teams;

    if (!g_challengeTeamMenuInit) {
        InitChallengeTeamMenu(g_weaponChallengeTeamMenu, g_weaponChallenges);
        g_challengeTeamMenuInit = TRUE;
    }

    gDPPipeSync(glistp++);
    gDPSetPrimColor(glistp++, 0, 0, 255, 255, 255, 0);
    gDPSetCombineMode(glistp++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);

    sprintf(buf, "DESTROY %i", (s32)g_weaponChallenges[frontendOptions_g.challengeIndex].goldTarget);
    glistp = AddCentreText(buf, g_briefingTextX, g_briefingTextY, 8, glistp);
    if (frontendOptions_g.challengeIndex != 6) {
        sprintf(buf, "DESTROY %i", (s32)g_weaponChallenges[frontendOptions_g.challengeIndex].silverTarget);
        glistp = AddCentreText(buf, g_briefingTextX, g_briefingTextY + g_briefingTextStep, 8, glistp);
        sprintf(buf, "DESTROY %i", (s32)g_weaponChallenges[frontendOptions_g.challengeIndex].bronzeTarget);
        glistp = AddCentreText(buf, g_briefingTextX, g_briefingTextY + g_briefingTextStep * 2, 8, glistp);
    }

    glistp = DrawChallengeInfo(g_weaponChallengeBriefingMenu, glistp);

    if (MenuControl(g_weaponChallengeBriefingMenu) == 1) {
        teams = frontendOptions_g.challengeData[frontendOptions_g.challengeIndex].allowedTeams;
        if (teams & 1) {
            frontendOptions_g.team = 3;
        } else if (teams & 8) {
            frontendOptions_g.team = 12;
        } else if (teams & 4) {
            frontendOptions_g.team = 0;
        } else if (teams & 2) {
            frontendOptions_g.team = 6;
        } else if (teams & 16) {
            frontendOptions_g.team = 9;
        }
        g_demoMode = 0;
        frontendOptions_g.ownShip[0] = frontendOptions_g.team;
        g_menuScreen = g_weaponChallengeTeamMenu[g_menuItem].nextScreen;
        StartGame();
    }

    return glistp;
}

Gfx *SuperComboChallengeBriefing(Gfx *glistp) {
    char buf[16];
    u32 teams;

    if (!g_challengeTeamMenuInit) {
        InitChallengeTeamMenu(g_superComboChallengeTeamMenu, g_superComboChallenges);
        g_challengeTeamMenuInit = TRUE;
    }

    gDPPipeSync(glistp++);
    gDPSetPrimColor(glistp++, 0, 0, 255, 255, 255, 0);
    gDPSetCombineMode(glistp++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);

    glistp = AddCentreText("FINISH 1ST", g_briefingTextX, g_briefingTextY - 4, 8, glistp);
    sprintf(buf, "DESTROY %i", (s32)g_superComboChallenges[frontendOptions_g.challengeIndex].goldKills);
    glistp = AddCentreText(buf, g_briefingTextX, g_briefingTextY + 4, 8, glistp);

    if (frontendOptions_g.challengeIndex != 6) {
        glistp = AddCentreText("FINISH 2ND", g_briefingTextX, g_briefingTextY + g_briefingTextStep - 4, 8, glistp);
        glistp = AddCentreText("FINISH 3RD", g_briefingTextX, g_briefingTextY + g_briefingTextStep * 2 - 4, 8, glistp);
        sprintf(buf, "DESTROY %i", (s32)g_superComboChallenges[frontendOptions_g.challengeIndex].silverKills);
        glistp = AddCentreText(buf, g_briefingTextX, g_briefingTextY + g_briefingTextStep + 4, 8, glistp);
        sprintf(buf, "DESTROY %i", (s32)g_superComboChallenges[frontendOptions_g.challengeIndex].bronzeKills);
        glistp = AddCentreText(buf, g_briefingTextX, g_briefingTextY + g_briefingTextStep * 2 + 4, 8, glistp);
    }

    glistp = DrawChallengeInfo(g_superComboChallengeBriefingMenu, glistp);

    if (MenuControl(g_superComboChallengeBriefingMenu) == 1) {
        teams = frontendOptions_g.challengeData[frontendOptions_g.challengeIndex].allowedTeams;
        if (teams & 1) {
            frontendOptions_g.team = 3;
        } else if (teams & 8) {
            frontendOptions_g.team = 12;
        } else if (teams & 4) {
            frontendOptions_g.team = 0;
        } else if (teams & 2) {
            frontendOptions_g.team = 6;
        } else if (teams & 16) {
            frontendOptions_g.team = 9;
        }
        g_demoMode = 0;
        frontendOptions_g.ownShip[0] = frontendOptions_g.team;
        g_menuScreen = g_superComboChallengeTeamMenu[g_menuItem].nextScreen;
        StartGame();
    }

    return glistp;
}

Gfx *ControllerConfigMenu(Gfx *glistp) {
    char buf[32];
    s32 res;
    s32 endX;

    res = MenuControl(g_controllerConfigMenu);
    g_padConfigIndex[0] = g_controllerConfigMenu[g_menuItem].value;

    gDPPipeSync(glistp++);
    gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);

    sprintf(buf, "CONFIG %i", g_padConfigIndex[0] + 1);
    glistp = CentreText(buf, 0, 190, 8, 1, glistp);

    if (g_ctrlAnimTimer == 0) {
        s32 leftOffset;

        endX = g_textEndX;
        leftOffset = endX - 145;
        if (g_padConfigIndex[0] < 3 && GetBlinkState()) {
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_arrowRightSprite));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, (endX + 7) << 2, 768);
        }
        if (g_padConfigIndex[0] > 0 && GetBlinkState()) {
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_arrowLeftSprite));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, (160 - leftOffset) << 2, 768);
        }
        glistp = DrawMenuBox(9, 3, 19, 61, 288, 144, g_padConfigIndex[0] + 4, glistp);
    }

    if (res == 1) {
        SetMenuTransition(g_controllerConfigMenu[0].nextScreen);
        SaveControllerConfig();
    } else if (res == 2) {
        SetMenuTransition(g_controllerConfigMenu[0].nextScreen);
        RestoreControllerConfig();
    }

    return DrawMenu(g_controllerConfigMenu, 130, 60, glistp, 1);
}

Gfx *RenderController3DModel(MenuItem *menu, Gfx *glistp) {
    ObjectStub *obj;

    ResetRenderList(g_feRenderList);
    obj = g_feModels[9];

    if (g_ctrlAnimTimer == 0) {
        SetSkeletonDirectionHPR(obj->skeleton, 0, 600, 0);
        SetSkeletonPosition(obj->skeleton, 0, 500, 4596);
    } else {
        if (g_ctrlAnimTimer > (g_ctrlAnimLength >> 1)) {
            if (g_ctrlAnimDir != 0) {
                SetSkeletonDirectionHPR(obj->skeleton, 0, 600,
                                        4096 - ((g_ctrlAnimLength - g_ctrlAnimTimer) * 2048) / (g_ctrlAnimLength >> 1));
            } else {
                SetSkeletonDirectionHPR(obj->skeleton, 0, 600,
                                        ((g_ctrlAnimLength - g_ctrlAnimTimer) * 2048) / (g_ctrlAnimLength >> 1));
            }
            SetSkeletonPosition(obj->skeleton, 0, 500,
                                ((g_ctrlAnimLength - g_ctrlAnimTimer) * 4404) / (g_ctrlAnimLength >> 1) + 4596);
        } else {
            if (g_ctrlAnimDir != 0) {
                SetSkeletonDirectionHPR(obj->skeleton, 0, 600, (g_ctrlAnimTimer * 2048) / (g_ctrlAnimLength >> 1));
            } else {
                SetSkeletonDirectionHPR(obj->skeleton, 0, 600,
                                        4096 - (g_ctrlAnimTimer * 2048) / (g_ctrlAnimLength >> 1));
            }
            SetSkeletonPosition(obj->skeleton, 0, 500,
                                (g_ctrlAnimTimer * 4404) / (g_ctrlAnimLength >> 1) + 4596);
        }
        g_ctrlAnimTimer--;
    }

    MatrixToN64Mtx(GetSkeletonMatrix(obj->skeleton), &g_mtxPool[g_mtxCount]);
    AddRenderGroup(g_feRenderList, obj->renderGroup, &g_mtxPool[g_mtxCount]);
    g_mtxCount++;
    return FlushRenderList(g_feRenderList, glistp);
}

Gfx *DrawShipStats(s8 shipIdx, Gfx *glistp) {
    u8 stats[5][5] = {
        { 6, 3, 7, 6, 7 },
        { 7, 5, 6, 3, 6 },
        { 5, 6, 5, 5, 5 },
        { 4, 7, 3, 7, 5 },
        { 10, 10, 10, 10, 10 },
    };
    s16 i;
    s32 w0, w1, w2, w3, w4;

    glistp = DrawFontText("THRUST", 155, 140, 8, glistp);
    glistp = DrawFontText("TOP SPEED", 155, 148, 8, glistp);
    glistp = DrawFontText("TURNING ABILITY", 155, 156, 8, glistp);
    glistp = DrawFontText("SHIELD ENERGY", 155, 164, 8, glistp);
    glistp = DrawFontText("AERODYNAMICS", 155, 172, 8, glistp);

    w0 = 10240 / stats[shipIdx][0];
    w1 = 10240 / stats[shipIdx][1];
    w2 = 10240 / stats[shipIdx][2];
    w3 = 10240 / stats[shipIdx][3];
    w4 = 10240 / stats[shipIdx][4];

    gDPPipeSync(glistp++);
    gDPSetPrimColor(glistp++, 0, 0, 85, 0, 40, 255);
    gSPTexture(glistp++, 0, 0, 0, G_TX_RENDERTILE, G_OFF);
    gDPSetCombineMode(glistp++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_loaderSprite));
    gSPSprite2DScaleFlip(glistp++, 1024, 9700, 0, 0);

    for (i = 0; i < 5; i++) {
        gSPSprite2DDraw(glistp++, 925, 559 + i * 32);
    }

    gDPPipeSync(glistp++);
    gDPSetPrimColor(glistp++, 0, 0, 24, 66, 11, 255);
    gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_loaderSprite));

    gSPSprite2DScaleFlip(glistp++, w0, 9700, 0, 0);
    gSPSprite2DDraw(glistp++, 925, 559);
    gSPSprite2DScaleFlip(glistp++, w1, 9700, 0, 0);
    gSPSprite2DDraw(glistp++, 925, 591);
    gSPSprite2DScaleFlip(glistp++, w2, 9700, 0, 0);
    gSPSprite2DDraw(glistp++, 925, 623);
    gSPSprite2DScaleFlip(glistp++, w3, 9700, 0, 0);
    gSPSprite2DDraw(glistp++, 925, 655);
    gSPSprite2DScaleFlip(glistp++, w4, 9700, 0, 0);
    gSPSprite2DDraw(glistp++, 925, 687);

    gSPTexture(glistp++, 0, 0, 0, G_TX_RENDERTILE, G_ON);
    return glistp;
}

u8 CheckBootControllerPak(OSMesgQueue *siMsgQ) {
    OSMesg dummyMsg;
    s32 maxFiles;
    s32 filesUsed;
    s32 freeBlocks;
    u8 attempts;

    attempts = 0;
    if (!(g_contStatus[0].status & CONT_CARD_ON)) {
        g_pakMissingAtBoot = 1;
        return 0;
    }

    do {
        attempts++;
        g_bootPakErr = InitControllerPak();
    } while (g_bootPakErr != 0 && attempts < 4);

    if (g_bootPakErr != 0) {
        g_pakMissingAtBoot = 1;
        return 0;
    }

    osContStartReadData(siMsgQ);
    osRecvMesg(siMsgQ, &dummyMsg, OS_MESG_BLOCK);
    osContGetReadData(g_contPads);
    if (osPfsNumFiles(&g_motorPfs, &maxFiles, &filesUsed) != 0) {
        g_pakMissingAtBoot = 1;
        return 0;
    }

    if (osPfsFreeBlocks(&g_motorPfs, &freeBlocks) != 0) {
        g_pakMissingAtBoot = 1;
        return 0;
    }

    if (maxFiles == filesUsed || (freeBlocks << 8) < g_pfsSaveSize) {
        g_pakMissingAtBoot = 1;
    }

    if (g_contPads[0].button & START_BUTTON) {
        return 1;
    }
    return 0;
}

void AppendN64FontCode(s32 code, char *out) {
    switch (code) {
        case 0x00:
            break;
        case 0x0F:
            StringCat(out, " ");
            break;
        case 0x10:
            StringCat(out, "0");
            break;
        case 0x11:
            StringCat(out, "1");
            break;
        case 0x12:
            StringCat(out, "2");
            break;
        case 0x13:
            StringCat(out, "3");
            break;
        case 0x14:
            StringCat(out, "4");
            break;
        case 0x15:
            StringCat(out, "5");
            break;
        case 0x16:
            StringCat(out, "6");
            break;
        case 0x17:
            StringCat(out, "7");
            break;
        case 0x18:
            StringCat(out, "8");
            break;
        case 0x19:
            StringCat(out, "9");
            break;
        case 0x1A:
            StringCat(out, "A");
            break;
        case 0x1B:
            StringCat(out, "B");
            break;
        case 0x1C:
            StringCat(out, "C");
            break;
        case 0x1D:
            StringCat(out, "D");
            break;
        case 0x1E:
            StringCat(out, "E");
            break;
        case 0x1F:
            StringCat(out, "F");
            break;
        case 0x20:
            StringCat(out, "G");
            break;
        case 0x21:
            StringCat(out, "H");
            break;
        case 0x22:
            StringCat(out, "I");
            break;
        case 0x23:
            StringCat(out, "J");
            break;
        case 0x24:
            StringCat(out, "K");
            break;
        case 0x25:
            StringCat(out, "L");
            break;
        case 0x26:
            StringCat(out, "M");
            break;
        case 0x27:
            StringCat(out, "N");
            break;
        case 0x28:
            StringCat(out, "O");
            break;
        case 0x29:
            StringCat(out, "P");
            break;
        case 0x2A:
            StringCat(out, "Q");
            break;
        case 0x2B:
            StringCat(out, "R");
            break;
        case 0x2C:
            StringCat(out, "S");
            break;
        case 0x2D:
            StringCat(out, "T");
            break;
        case 0x2E:
            StringCat(out, "U");
            break;
        case 0x2F:
            StringCat(out, "V");
            break;
        case 0x30:
            StringCat(out, "W");
            break;
        case 0x31:
            StringCat(out, "X");
            break;
        case 0x32:
            StringCat(out, "Y");
            break;
        case 0x33:
            StringCat(out, "Z");
            break;
        case 0x34:
            StringCat(out, "!");
            break;
        case 0x35:
            StringCat(out, "\"");
            break;
        case 0x36:
            StringCat(out, "#");
            break;
        case 0x37:
            StringCat(out, "'");
            break;
        case 0x38:
            StringCat(out, "*");
            break;
        case 0x39:
            StringCat(out, "+");
            break;
        case 0x3A:
            StringCat(out, ",");
            break;
        case 0x3B:
            StringCat(out, "-");
            break;
        case 0x3C:
            StringCat(out, ".");
            break;
        case 0x3D:
            StringCat(out, "/");
            break;
        case 0x3E:
            StringCat(out, ":");
            break;
        case 0x3F:
            StringCat(out, "=");
            break;
        case 0x40:
            StringCat(out, "?");
            break;
        case 0x41:
            StringCat(out, "@");
            break;
        case 0x94:
        default:
            StringCat(out, "-");
            break;
    }
}

void DrawPakProblemPrompt(void) {
    g_feDlistEnd = CentreText("THERE IS A PROBLEM WITH", 30, 110, 8, 1, g_feDlistEnd);
    g_feDlistEnd = CentreText("THE CONTROLLER PAK", 30, 120, 8, 1, g_feDlistEnd);
    g_feDlistEnd = CentreText("ON PORT 1.", 30, 130, 8, 1, g_feDlistEnd);
    g_feDlistEnd = DrawButtonPrompt(60, 200, 12, "", g_feDlistEnd);
    g_feDlistEnd = DrawFontText("ATTEMPT", 80, 199, 8, g_feDlistEnd);
    g_feDlistEnd = DrawFontText("REPAIR", 80, 207, 8, g_feDlistEnd);
    g_feDlistEnd = DrawButtonPrompt(250, 200, 13, "", g_feDlistEnd);
    g_feDlistEnd = DrawFontText("ATTEMPT", 201, 199, 8, g_feDlistEnd);
    g_feDlistEnd = DrawFontText("RECONNECT", 189, 207, 8, g_feDlistEnd);

    if (g_contPads[0].button & A_BUTTON) {
        osPfsRepairId(&g_motorPfs);
        g_bootPakErr = InitControllerPak();
    } else if (g_contPads[0].button & B_BUTTON) {
        g_bootPakErr = InitControllerPak();
    }
}

s32 ControllerPakManagerMenu(OSMesgQueue *siMsgQ) {
    char numBuf[8];
    char nameBuf[16];
    char extBuf[8];
    OSMesg dummyMsg;
    u16 pressed;
    u8 i;
    u8 j;

    if (g_pakMgrWarnActive) {
        if (g_pakMgrWarnTimer > 0) {
            g_pakMgrWarnTimer--;
            g_feDlistEnd = CentreText("THERE IS A PROBLEM WITH", 30, 110, 8, 1, g_feDlistEnd);
            g_feDlistEnd = CentreText("THE CONTROLLER PAK", 30, 120, 8, 1, g_feDlistEnd);
            g_feDlistEnd = CentreText("ON PORT 1.", 30, 130, 8, 1, g_feDlistEnd);
            g_feDlistEnd = CentreText("YOU WILL BE UNABLE", 30, 150, 8, 1, g_feDlistEnd);
            g_feDlistEnd = CentreText("TO LOAD AND SAVE", 30, 160, 8, 1, g_feDlistEnd);
            g_feDlistEnd = CentreText("WIPEOUT64.", 30, 170, 8, 1, g_feDlistEnd);
            g_feDlistEnd = DrawMenuBox(10, 8, 0, 0, 320, 240, 9, g_feDlistEnd);
            return 0;
        }
        return 1;
    }

    osContStartReadData(siMsgQ);
    osRecvMesg(siMsgQ, &dummyMsg, OS_MESG_BLOCK);
    osContGetReadData(g_contPads);

    if (g_contPads[0].stick_y < -30) {
        g_contPads[0].button |= D_JPAD;
    } else if (g_contPads[0].stick_y > 30) {
        g_contPads[0].button |= U_JPAD;
    }

    pressed = g_contPads[0].button & (g_contPads[0].button ^ g_pakMgrPrevButtons);
    g_pakMgrPrevButtons = g_contPads[0].button;
    g_contPads[0].button = pressed;

    if (pressed == 0) {
        g_pakManagerInit = 0;
    } else if ((pressed & START_BUTTON) && !g_pakManagerInit) {
        if (g_pakMgrErrorShown) {
            if (g_bootPakErr == 10) {
                g_pakMgrWarnActive = TRUE;
                return 0;
            }
            return 1;
        }
        return 1;
    }

    g_feDlistEnd = DrawMenuBox(10, 8, 0, 0, 320, 240, 9, g_feDlistEnd);

    if (g_bootPakErr == 10) {
        DrawPakProblemPrompt();
        g_pakMgrErrorShown = TRUE;
        return 0;
    }

    if (osPfsNumFiles(&g_motorPfs, &g_pakMgrMaxFiles, &g_pakMgrFilesUsed) != 0) {
        return 1;
    }

    if (g_pakMgrNeedRefresh) {
        g_pakMgrNeedRefresh = 0;
        for (i = 0; i < 16; i++) {
            g_pakMgrSlotStatus[i] = osPfsFileState(&g_motorPfs, i, &g_pakMgrSlotState[i]);
        }
        if (osPfsFreeBlocks(&g_motorPfs, &g_pakMgrFreeBlocks) != 0) {
            return 1;
        }
        if (g_pakMgrMaxFiles == g_pakMgrFilesUsed || (g_pakMgrFreeBlocks << 8) < g_pfsSaveSize) {
            g_pakMissingAtBoot = 1;
        } else {
            g_pakMissingAtBoot = 0;
        }
        g_pakMgrFreeBlocks >>= 8;
    }

    if (g_contPads[0].button & D_JPAD) {
        g_pakMgrCursor++;
        if (g_pakMgrCursor == 16) {
            g_pakMgrCursor = 0;
        } else if (g_pakMgrCursor == -1) {
            g_pakMgrCursor = 15;
        }
        g_pakMgrConfirmDelete = 0;
    } else if (g_contPads[0].button & U_JPAD) {
        g_pakMgrCursor--;
        if (g_pakMgrCursor == 16) {
            g_pakMgrCursor = 0;
        } else if (g_pakMgrCursor == -1) {
            g_pakMgrCursor = 15;
        }
        g_pakMgrConfirmDelete = 0;
    }

    g_feDlistEnd = DrawFontTextYellow("NOTE", 100, 65, 8, g_feDlistEnd);
    g_feDlistEnd = DrawFontTextYellow("PAGES", 230, 65, 8, g_feDlistEnd);
    sprintf(numBuf, "%ld", g_pakMgrFreeBlocks);
    g_feDlistEnd = DrawFontTextYellow(numBuf, 70, 190, 8, g_feDlistEnd);
    g_feDlistEnd = DrawFontTextYellow("PAGES FREE", 100, 190, 8, g_feDlistEnd);

    for (i = 0; i < 16; i++) {
        sprintf(numBuf, "%d", i + 1);
        if (i == g_pakMgrCursor) {
            g_feDlistEnd = DrawFontTextYellow(numBuf, 70, i * 7 + 75, 8, g_feDlistEnd);
        } else {
            g_feDlistEnd = DrawFontText(numBuf, 70, i * 7 + 75, 8, g_feDlistEnd);
        }

        if (g_pakMgrSlotStatus[i] == 0) {
            strcpy(nameBuf, "");
            for (j = 0; j < 16; j++) {
                AppendN64FontCode(g_pakMgrSlotState[i].game_name[j], nameBuf);
            }
            if (i == g_pakMgrCursor) {
                g_feDlistEnd = DrawFontTextYellow(nameBuf, 100, i * 7 + 75, 8, g_feDlistEnd);
            } else {
                g_feDlistEnd = DrawFontText(nameBuf, 100, i * 7 + 75, 8, g_feDlistEnd);
            }

            if (g_pakMgrSlotState[i].ext_name[0] != 0) {
                strcpy(extBuf, "");
                for (j = 0; j < 4; j++) {
                    AppendN64FontCode(g_pakMgrSlotState[i].ext_name[j], extBuf);
                }
                if (i == g_pakMgrCursor) {
                    g_feDlistEnd = DrawFontTextYellow(extBuf, 200, i * 7 + 75, 8, g_feDlistEnd);
                } else {
                    g_feDlistEnd = DrawFontText(extBuf, 200, i * 7 + 75, 8, g_feDlistEnd);
                }
            }

            sprintf(numBuf, "%ld", (g_pakMgrSlotState[i].file_size + 255) >> 8);
            if (i == g_pakMgrCursor) {
                g_feDlistEnd = DrawFontTextYellow(numBuf, 230, i * 7 + 75, 8, g_feDlistEnd);
            } else {
                g_feDlistEnd = DrawFontText(numBuf, 230, i * 7 + 75, 8, g_feDlistEnd);
            }
        } else {
            if (i == g_pakMgrCursor) {
                g_feDlistEnd = DrawFontTextYellow("[EMPTY]", 100, i * 7 + 75, 8, g_feDlistEnd);
            } else {
                g_feDlistEnd = DrawFontText("[EMPTY]", 100, i * 7 + 75, 8, g_feDlistEnd);
            }
            sprintf(numBuf, "%d", 0);
            if (i == g_pakMgrCursor) {
                g_feDlistEnd = DrawFontTextYellow(numBuf, 230, i * 7 + 75, 8, g_feDlistEnd);
            } else {
                g_feDlistEnd = DrawFontText(numBuf, 230, i * 7 + 75, 8, g_feDlistEnd);
            }
        }
    }

    if (g_pakMgrSlotStatus[g_pakMgrCursor] != 0) {
        g_feDlistEnd = DrawFontText("EMPTY GAME SLOT", 125, 205, 8, g_feDlistEnd);
        g_pakMgrConfirmDelete = 0;
    } else {
        switch (g_pakMgrConfirmDelete) {
            case 0:
                g_feDlistEnd = DrawButtonPrompt(135, 202, 12, "", g_feDlistEnd);
                g_feDlistEnd = DrawFontText("DELETE", 153, 205, 8, g_feDlistEnd);
                break;
            case 1:
                g_feDlistEnd = DrawButtonPrompt(120, 202, 13, "", g_feDlistEnd);
                g_feDlistEnd = DrawFontText("CONFIRM DELETE", 138, 205, 8, g_feDlistEnd);
                break;
        }

        if (g_pakMgrConfirmDelete == 0 && (g_contPads[0].button & A_BUTTON)) {
            g_pakMgrConfirmDelete = 1;
            return 0;
        }
        if (g_pakMgrConfirmDelete == 1 && (g_contPads[0].button & B_BUTTON)) {
            osPfsDeleteFile(&g_motorPfs, g_pakMgrSlotState[g_pakMgrCursor].company_code,
                            g_pakMgrSlotState[g_pakMgrCursor].game_code, g_pakMgrSlotState[g_pakMgrCursor].game_name,
                            g_pakMgrSlotState[g_pakMgrCursor].ext_name);
            g_pakMgrConfirmDelete = 0;
            g_pakMgrNeedRefresh = 1;
        }
    }

    return 0;
}

void InitCredits(void) {
    s32 i;
    s32 y;
    s32 count = 87;

    y = g_creditsBottomY;
    for (i = 0; i < count; i++) {
        g_creditsLines[i].y = y;
        y += g_creditsLineSpacing;
        y += g_creditsLines[i].extraSpacing * g_creditsLineSpacing;
    }
    g_creditsMusicInit = 1;
}

Gfx *CreditsMenu(Gfx *glistp) {
    s32 savedTrack;
    s32 anyVisible;
    s32 i;
    s32 count = 87;

    anyVisible = 0;
    if (g_creditsMusicInit) {
        savedTrack = frontendOptions_g.musicTrack;
        frontendOptions_g.musicTrack = 9;
        StopAudio();
        StartMusic();
        g_creditsMusicInit = 0;
        g_creditsFrameCount = 0;
        frontendOptions_g.musicTrack = savedTrack;
    }

    g_creditsFrameCount++;

    if (MenuControl(g_creditsMenu) == 2) {
        StopAudio();
        StartMusic();
    }

    if (g_menuTransition.timer == 0) {
        for (i = 0; i < count; i++) {
            if (!(g_creditsFrameCount & 1)) {
                g_creditsLines[i].y -= g_creditsScrollSpeed;
            }
            if (g_creditsLines[i].y >= g_creditsTopY && g_creditsLines[i].y <= g_creditsBottomY) {
                anyVisible = 1;
                if (g_creditsLines[i].flags & 4) {
                    glistp = AddCentreText(g_creditsLines[i].text, 160, g_creditsLines[i].y, 8, glistp);
                } else {
                    glistp = CentreText(g_creditsLines[i].text, 0, g_creditsLines[i].y, 8,
                                        (g_creditsLines[i].flags ^ 1) & 1, glistp);
                }
            }
        }

        if (!anyVisible) {
            glistp = CentreText("WIPEOUT 64", 0, 102, 8, 0, glistp);
            glistp = CentreText("COPYRIGHT PSYGNOSIS", 0, 119, 8, 1, glistp);
            glistp = CentreText("1998", 0, 129, 8, 1, glistp);
        }
    }

    return DrawMenu(g_creditsMenu, 130, 60, glistp, 1);
}

Gfx *RenderChallengeMedal3D(MenuItem *menu, Dynamic *dp, s32 isChallengeList, Gfx *glistp) {
    Gfx *dlStart;
    s32 startMtx;
    u8 i;
    s32 part;
    s32 modelIdx;
    ChallengeData *ch;
    ObjectStub *obj;
    ModelTransform *xf;
    Gfx *dl;

    ResetRenderList(g_feRenderList);
    g_modelAngle += 20;
    if (g_modelAngle >= 4096) {
        g_modelAngle -= 4096;
    }

    for (i = 0; menu[i].row != -1; i++) {
        if (menu[i].selected == 0) {
            g_menuItem = i;
            break;
        }
    }

    if (isChallengeList) {
        if (menu == g_raceChallengeMenu) {
            ch = &g_raceChallenges[g_menuItem];
        } else if (menu == g_ttChallengeMenu) {
            ch = &g_ttChallenges[g_menuItem];
        } else if (menu == g_weaponChallengeMenu) {
            ch = &g_weaponChallenges[g_menuItem];
        } else if (menu == g_superComboChallengeMenu) {
            ch = &g_superComboChallenges[g_menuItem];
        }

        if (ch->award != 0) {
            dl = g_medalDlBuf;
            dlStart = dl;
            startMtx = g_mtxCount;

            for (part = 0; part < 2; part++) {
                if (part == 0) {
                    modelIdx = 6;
                } else {
                    modelIdx = menu[g_menuItem].modelIdx;
                }
                obj = g_feModels[modelIdx];

                if (part == 0) {
                    if (g_challengeMedalDl != NULL) {
                        DFree(g_heap, g_challengeMedalDl);
                    }
                    if (g_challengeMedalVtx != NULL) {
                        DFree(g_heap, g_challengeMedalVtx);
                    }
                    BuildEnvMapDisplayList(obj, dp, modelIdx, ch->award, &g_challengeMedalVtx, (Gfx **)&g_challengeMedalDl);
                } else {
                    if (g_challengeModelDl != NULL) {
                        DFree(g_heap, g_challengeModelDl);
                    }
                    if (g_challengeModelVtx != NULL) {
                        DFree(g_heap, g_challengeModelVtx);
                    }
                    BuildEnvMapDisplayList(obj, dp, modelIdx, ch->award, &g_challengeModelVtx, (Gfx **)&g_challengeModelDl);
                }

                SetSkeletonDirectionHPR(obj->skeleton, g_modelAngle, g_modelTransforms[modelIdx].pitch,
                                        g_modelTransforms[modelIdx].roll);
                if (part == 0) {
                    SetSkeletonPosition(obj->skeleton, 0, 800, 4600);
                } else {
                    SetSkeletonPosition(obj->skeleton, 0, 1800, 4600);
                }
                MatrixToN64Mtx(GetSkeletonMatrix(obj->skeleton), &g_mtxPool[g_mtxCount]);
                gSPMatrix(dl++, OS_K0_TO_PHYSICAL(&g_mtxPool[g_mtxCount]),
                          G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
                g_mtxCount++;
                if (part == 0) {
                    gSPDisplayList(dl++, g_challengeMedalDl);
                } else {
                    gSPDisplayList(dl++, g_challengeModelDl);
                }
            }

            g_envMapCachedAward = ch->award;
            gSPEndDisplayList(dl++);
            AddRenderDisplayList(g_feRenderList, &dlStart, &g_mtxPool[startMtx]);
            glistp = FlushSkyRenderList(g_feRenderList, glistp);
        }
    } else {
        modelIdx = menu[g_menuItem].modelIdx;
        obj = g_feModels[modelIdx];
        if (g_challengeModelDl == NULL || g_challengeModelVtx == NULL || modelIdx != g_envMapCachedModel) {
            if (g_challengeModelDl != NULL) {
                DFree(g_heap, g_challengeModelDl);
            }
            if (g_challengeModelVtx != NULL) {
                DFree(g_heap, g_challengeModelVtx);
            }
            BuildEnvMapDisplayList(obj, dp, modelIdx, 3, &g_challengeModelVtx, (Gfx **)&g_challengeModelDl);
            g_envMapCachedModel = modelIdx;
        }
        SetSkeletonDirectionHPR(obj->skeleton, g_modelAngle, g_modelTransforms[modelIdx].pitch,
                                g_modelTransforms[modelIdx].roll);
        if (modelIdx >= 20 && modelIdx < 25) {
            if (menu == g_bonusMenu) {
                SetSkeletonPosition(obj->skeleton, 0, 0, 400);
            } else {
                SetSkeletonPosition(obj->skeleton, -270, g_modelTransforms[modelIdx].y, g_modelTransforms[modelIdx].z);
            }
        } else {
            SetSkeletonPosition(obj->skeleton, 0, g_modelTransforms[modelIdx].y, g_modelTransforms[modelIdx].z);
        }
        MatrixToN64Mtx(GetSkeletonMatrix(obj->skeleton), &g_mtxPool[g_mtxCount]);
        AddRenderDisplayList(g_feRenderList, (Gfx **)&g_challengeModelDl, &g_mtxPool[g_mtxCount]);
        g_mtxCount++;
        glistp = FlushSkyRenderList(g_feRenderList, glistp);
    }

    return glistp;
}

/*
 * Renders the 3D trophy ships on the final league standings screen.
 *
 * Compiler note (SN64 cc1n64.exe loop.c): scan_loop (0x48c618) initializes
 * threshold = (n_non_fixed_regs + 1) * 1 = 61 and move_movables (0x48dd88)
 * hoists a movable iff lifetime * savings * threshold >= insn_count, then
 * decrements threshold -= 3 (0x48e28f). In retail, %hi(tournamentTableStandingCount),
 * %hi(g_podium1stDl), and &g_podium1stVtx stay unhoisted while &jtbl stays hoisted.
 * Under #ifndef NON_MATCHING, two flow-eliminated loop-carried expressions lower
 * threshold by 6 before tournamentTableStandingCount and by 9 before place == 1
 * (emitting zero instructions) to reproduce retail's exact hoist set.
 */
Gfx *RenderFinalStandings3D(Dynamic *dp, Gfx *glistp) {
    Gfx *dlStart;
    s32 startMtx;
    Gfx *dl;
    s32 place;
    s32 shipIdx;
    ObjectStub *obj;
#ifndef NON_MATCHING
    s32 deadLoopA = 0;
    s32 deadLoopB = 0;
#endif

    g_modelAngle += 20;
    if (g_modelAngle >= 4096) {
        g_modelAngle -= 4096;
    }

    ResetRenderList(g_feRenderList);
    ASSERT(tournamentTableStandingCount >= 2, 12808);

    dl = g_medalDlBuf;
    dlStart = dl;
    startMtx = g_mtxCount;

    for (place = 1; place < 4; place++) {
#ifndef NON_MATCHING
        deadLoopA = deadLoopB + (s32)&g_feModels[0];
#endif
        if (place == 3 && tournamentTableStandingCount < 3) {
            continue;
        }
        shipIdx = g_leagueStandings[tournamentTableStandingCount - place].shipIdx;
        if (g_fourPlayerSameTeam && shipIdx == ownShip4) {
            shipIdx = ownShip3;
        }
        switch (shipIdx) {
            case 0:
            case 1:
            case 2:
                obj = shipShapes[3];
                break;
            case 3:
            case 4:
            case 5:
                obj = shipShapes[2];
                break;
            case 6:
            case 7:
            case 8:
                obj = shipShapes[0];
                break;
            case 9:
            case 10:
            case 11:
                obj = shipShapes[1];
                break;
            case 12:
            case 13:
            case 14:
                obj = shipShapes[4];
                break;
        }

#ifndef NON_MATCHING
        deadLoopB = deadLoopA + (s32)&g_modelTransforms[0] + (s32)&g_feModels[1];
#endif
        SetSkeletonDirectionHPR(obj->skeleton, g_modelAngle, 0, 0);

        if (place == 1) {
            if (g_podium1stDl == NULL || g_podium1stVtx == NULL) {
                if (g_podium1stDl != NULL) {
                    DFree(g_heap, g_podium1stDl);
                }
                if (g_podium1stVtx != NULL) {
                    DFree(g_heap, g_podium1stVtx);
                }
                BuildEnvMapDisplayList(obj, dp, 0, 3, &g_podium1stVtx, &g_podium1stDl);
            }
            SetSkeletonPosition(obj->skeleton, 0, 0, 400);
        } else if (place == 2) {
            if (g_podium2ndDl == NULL || g_podium2ndVtx == NULL) {
                if (g_podium2ndDl != NULL) {
                    DFree(g_heap, g_podium2ndDl);
                }
                if (g_podium2ndVtx != NULL) {
                    DFree(g_heap, g_podium2ndVtx);
                }
                BuildEnvMapDisplayList(obj, dp, 0, 2, &g_podium2ndVtx, &g_podium2ndDl);
            }
            SetSkeletonPosition(obj->skeleton, -380, 200, 650);
        } else if (place == 3) {
            if (g_podium3rdDl == NULL || g_podium3rdVtx == NULL) {
                if (g_podium3rdDl != NULL) {
                    DFree(g_heap, g_podium3rdDl);
                }
                if (g_podium3rdVtx != NULL) {
                    DFree(g_heap, g_podium3rdVtx);
                }
                BuildEnvMapDisplayList(obj, dp, 0, 1, &g_podium3rdVtx, &g_podium3rdDl);
            }
            SetSkeletonPosition(obj->skeleton, 560, 410, 900);
        }

        MatrixToN64Mtx(GetSkeletonMatrix(obj->skeleton), &g_mtxPool[g_mtxCount]);
        gSPMatrix(dl++, OS_K0_TO_PHYSICAL(&g_mtxPool[g_mtxCount]), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
        g_mtxCount++;

        if (place == 1) {
            gSPDisplayList(dl++, g_podium1stDl);
        } else if (place == 2) {
            gSPDisplayList(dl++, g_podium2ndDl);
        } else if (place == 3) {
            gSPDisplayList(dl++, g_podium3rdDl);
        }
    }

    gSPEndDisplayList(dl++);
    AddRenderDisplayList(g_feRenderList, &dlStart, &g_mtxPool[startMtx]);
    return FlushSkyRenderList(g_feRenderList, glistp);
}
