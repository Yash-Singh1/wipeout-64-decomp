#include "pause.h"

/*
 * Track/artist pointer tables (g_cdTrackNames, g_cdTrackArtists) and the
 * $Revision: 1.17 $ + track/artist strings currently live in main_data and
 * the leading 0xB8 bytes of pause_rodata (US 0x4D50..0x4E08, EU 0x4D40..0x4DF8).
 * This file emits the 0xA7 bytes of function .rodata (US 0x4E08..0x4EAF,
 * EU 0x4DF8..0x4E9F) and zero .data/.bss while main_data remains unsplit.
 */

char *strcpy(char *dst, const char *src);

char *GetCdTrackName(s32 track) {
    return g_cdTrackNames[track];
}

char *GetCdTrackArtist(s32 track) {
    return g_cdTrackArtists[track];
}

Gfx *InGameReset(PauseState *pause, void *game, void *polys, u16 pad, Gfx *glistp) {
    char labels[4][16];
    s32 i;
    s32 y;
    u8 gap;

    y = 95;
    gap = 12;

    if ((pause->frameCount % 30) < 21) {
        glistp = CentreText("PAUSED", 107, 70, 12, 0, glistp);
    }

    if (pause->selection != PAUSE_AUDIO) {
        g_pauseBoxWidth = 0;
        g_pauseBoxHeight = 0;
        g_pauseBoxX = 160;
        g_pauseBoxY = 170;
        g_pauseOpenSteps = 0;
        g_pauseCloseSteps = 0;
        g_pauseBoxClosing = 0;
        g_pauseBoxOpen = 0;
        g_pauseBoxClosed = 0;
    }

    if (pause->stage == PAUSE_STAGE_REQUESTED || pause->stage == PAUSE_STAGE_NONE) {
        if (g_gameType == GAME_TYPE_LEAGUE) {
            strcpy(labels[0], "CONTINUE");
            strcpy(labels[1], "RESTART");
            strcpy(labels[2], "ABORT RACE");
            strcpy(labels[3], "QUIT LEAGUE");
        } else {
            strcpy(labels[0], "CONTINUE");
            strcpy(labels[1], "RESTART");
            strcpy(labels[2], "RETURN TO MENU");
        }

        if (g_gameType == GAME_TYPE_LEAGUE) {
            for (i = 0; i < 4; i++) {
                if (i == pause->selection) {
                    glistp = CentreText(labels[i], 0, y + (gap * i), 8, 0, glistp);
                } else {
                    glistp = CentreText(labels[i], 0, y + (gap * i), 8, 1, glistp);
                }
            }
        } else {
            for (i = 0; i < 3; i++) {
                if (i == pause->selection) {
                    glistp = CentreText(labels[i], 0, y + (gap * i), 8, 0, glistp);
                } else {
                    glistp = CentreText(labels[i], 0, y + (gap * i), 8, 1, glistp);
                }
            }
        }

        SetVolume(g_ambientVoice1, 0);
    }

    pause->frameCount++;
    g_pausePrevChoice = pause->selection;
    return glistp;
}

Gfx *InGameConfirm(u8 *confirmed, s32 mode, s16 pad, u8 *selection, s32 *frameCount, Gfx *glistp) {
    s32 highlight;

    highlight = 1;
    if (mode == 1) {
        glistp = CentreText("ARE YOU SURE YOU", 77, 95, 8, highlight, glistp);
        glistp = CentreText("WANT TO RESTART", 90, 105, 8, highlight, glistp);
    } else if (mode == 2) {
        if (g_gameType == GAME_TYPE_LEAGUE) {
            glistp = CentreText("ARE YOU SURE YOU", 77, 95, 8, highlight, glistp);
            glistp = CentreText("WANT TO ABORT RACE", 90, 105, 8, highlight, glistp);
        } else {
            glistp = CentreText("ARE YOU SURE YOU", 77, 95, 8, highlight, glistp);
            glistp = CentreText("WANT TO QUIT", 105, 105, 8, highlight, glistp);
        }
    } else if (g_gameType == GAME_TYPE_LEAGUE) {
        glistp = CentreText("ARE YOU SURE YOU", 77, 95, 8, highlight, glistp);
        glistp = CentreText("WANT TO QUIT LEAGUE", 105, 105, 8, highlight, glistp);
    }

    if (GetMenuKey(MENU_KEY_LEFT, pad) != 0) {
        if (*selection != 1) {
            *selection = 1;
            PlayNote(26, 255, 15, 15, 256);
        }
    } else if (GetMenuKey(MENU_KEY_RIGHT, pad) != 0) {
        if (*selection != 0) {
            *selection = 0;
            PlayNote(26, 255, 15, 15, 256);
        }
    }

    if (*selection != 0) {
        glistp = AddText("YES", 75, 120, 12, 0, glistp);
        glistp = AddText("NO", 205, 120, 12, 1, glistp);
    } else {
        glistp = AddText("YES", 75, 120, 12, 1, glistp);
        glistp = AddText("NO", 205, 120, 12, 0, glistp);
    }

    if (GetMenuKey(MENU_KEY_SELECT, pad) != 0) {
        *confirmed = *selection;
    }

    *frameCount += 1;
    return glistp;
}

Gfx *DeSelectScreen(void *sel, Gfx *glistp, s32 player) {
    return DrawScreenOverlay(player, 0, glistp, 1);
}
