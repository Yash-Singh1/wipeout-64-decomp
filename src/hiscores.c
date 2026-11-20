#include "hiscores.h"

#ident "$Revision: 1.17 $"
#ident "$Revision: 7.27 $"
#ident "$Revision: 1.5 $"

void ScoreTabInit(ConfigData *config) {
    char names[8][4][4] = {
        { "DIG", "JEF", "MRK", "DAN" },
        { "JAN", "JON", "D D", "BEN" },
        { "ASH", "STE", "HOS", "NEV" },
        { "ROB", "AJS", "TOM", "SEB" },
        { "ONE", "ARM", "LAS", "ROB" },
        { "HAR", "RY",  "ART", "HUR" },
        { "S H", "ARS", "MSP", "CW"  },
        { "",    "",    "POL", "CJR" },
    };
    s32 raceTimes[8][4] = {
        { 102, 131, 127, 137 },
        { 141, 193, 188, 206 },
        { 156, 209, 197, 216 },
        { 108, 147, 147, 162 },
        { 147, 200, 199, 224 },
        { 139, 185, 186, 213 },
        { 113, 161, 158, 171 },
        {   0,   0, 192, 220 },
    };
    s32 lapTimes[8][4] = {
        { 50, 43, 31, 26 },
        { 70, 63, 46, 40 },
        { 77, 69, 48, 42 },
        { 53, 48, 36, 31 },
        { 72, 66, 49, 44 },
        { 68, 61, 45, 42 },
        { 55, 53, 39, 33 },
        {  0,  0, 47, 43 },
    };
    s32 track;
    s32 trackSlot;
    s32 cls;

    if (config->initMagic != HISCORE_MAGIC) {
        for (track = 0; track < 16; track++) {
            if (track >= 8) {
                trackSlot = track - 8;
            } else {
                trackSlot = track;
            }
            for (cls = 0; cls < 4; cls++) {
                StringCopy(g_hiScoreTable[track][cls].name, names[trackSlot][cls]);
                g_hiScoreTable[track][cls].time = raceTimes[trackSlot][cls] * HISCORE_FRAME_RATE;
                StringCopy(g_hiScoreTable[track][cls].lapName, names[trackSlot][cls]);
                g_hiScoreTable[track][cls].lapTime = lapTimes[trackSlot][cls] * HISCORE_FRAME_RATE;
                g_hiScoreTable[track][cls].eliminations = cls;
                g_hiScoreTable[track][cls].lapShip = 15;
                g_hiScoreTable[track][cls].ship = 15;
            }
        }
    }
}

void InitScoreTabData(HiScoreData *score) {
    if (g_firstHiScoreInit != 0) {
        if (g_hiScoreLoaded == 0) {
            score->dispChar[0] = 0;
            score->nameEntered[0] = 0;
            score->selectedChar = 'A';
            score->alphabetIndex = 0;
            score->nameCursor = 0;
            score->nameEndX = 0;
            g_firstHiScoreInit = 0;
        }
    }
    score->enteringName = 1;
    score->storeRecords = 1;
    score->cursorBlinkFrames = 0;
    score->firstAlphabetIndex = 0;
    score->finishDelayFrames = 0;
    score->inputDelayFrames = 0;
    score->totalTime = 0;
    score->pendingRecords = 0;
    score->displayedRecords = 0;
    score->highScore = 0;
    score->confirm = 2;
    score->lConfirm = 0;
}

void CheckForHiScore(ConfigData *config, HiScoreData *score, ShipData *ships, s32 shipNum, u8 lap) {
    s32 trackNum;
    s32 i;
    s8 lapNum;

    if (g_gameType == GAME_TYPE_TIME_TRIAL ||
        (g_gameType == GAME_TYPE_CHALLENGE && frontendOptions_g.challengeMedal == 2)) {
        trackNum = config->trackNum + 8;
    } else {
        trackNum = config->trackNum;
    }

    score->totalTime = 0;

    if (lap == 1) {
        lapNum = ships[shipNum].lap;
        if (lapNum == 2 || !score->pendingRecords) {
            if (ships[shipNum].lapTimes[lapNum - 2] < g_hiScoreTable[trackNum][config->shipClass].lapTime) {
                score->lapTime = ships[ownShip].lapTimes[ships[ownShip].lap - 2];
                score->lapPilot = ownShip;
                score->pendingRecords |= HISCORE_LAP_RECORD;
                score->displayedRecords = score->pendingRecords;
                if (ships[ownShip].lap < g_classLaps[config->shipClass] + 1) {
                    if (g_gameType != GAME_TYPE_CHALLENGE || frontendOptions_g.challengeMedal != 2) {
                        PlayNote(59, 510, 15, 15, 256);
                    }
                }
            }
        } else if (ships[shipNum].lapTimes[lapNum - 2] < score->lapTime) {
            score->lapTime = ships[ownShip].lapTimes[ships[ownShip].lap - 2];
            score->lapPilot = ownShip;
            g_myBestLap = score->lapTime;
            score->pendingRecords |= HISCORE_LAP_RECORD;
            score->displayedRecords = score->pendingRecords;
            if (ships[ownShip].lap < g_classLaps[config->shipClass] + 1) {
                if (g_gameType != GAME_TYPE_CHALLENGE || frontendOptions_g.challengeMedal != 2) {
                    PlayNote(59, 510, 15, 15, 256);
                }
            }
        }
    } else {
        for (i = 0; i < g_classLaps[config->shipClass]; i++) {
            score->totalTime += g_savedLapTimes[i][0];
        }
        if (score->totalTime < g_hiScoreTable[trackNum][config->shipClass].time) {
            score->raceTime = score->totalTime;
            g_myBestRace = score->raceTime;
            score->racePilot = ownShip;
            score->raceEliminations = ships[shipNum].eliminations;
            score->pendingRecords |= HISCORE_RACE_RECORD;
            score->displayedRecords = score->pendingRecords;
        }
    }
}

Gfx *DisplayScoreTab(ConfigData *config, HiScoreData *score, u16 pad, Gfx *glistp) {
    s32 track;
    s32 cls;

    cls = config->shipClass;

    if (g_gameType == GAME_TYPE_TIME_TRIAL ||
        (g_gameType == GAME_TYPE_CHALLENGE && frontendOptions_g.challengeMedal == 2)) {
        track = config->trackNum + 8;
    } else {
        track = config->trackNum;
    }

    if (g_oldRaceTime == -1) {
        g_oldRaceTime = g_hiScoreTable[track][cls].time;
        TimeToString(g_oldRaceTime, 1, g_oldRaceTimeStr);
    }
    if (g_oldLapTime == -1) {
        g_oldLapTime = g_hiScoreTable[track][cls].lapTime;
        TimeToString(g_oldLapTime, 1, g_oldLapTimeStr);
    }

    if (score->pendingRecords != 0) {
        glistp = EnterHighScore(config, score, pad, glistp);
    }

    switch (frontendOptions_g.trackNo) {
        case 1:
            glistp = CentreText("KLIES BRIDGE", 40, 50, 8, 1, glistp);
            break;
        case 2:
            glistp = CentreText("QORON IV", 40, 50, 8, 1, glistp);
            break;
        case 3:
            glistp = CentreText("SOKANA", 40, 50, 8, 1, glistp);
            break;
        case 4:
            glistp = CentreText("DYRONESS", 40, 50, 8, 1, glistp);
            break;
        case 5:
            glistp = CentreText("MACHAON II", 40, 50, 8, 1, glistp);
            break;
        case 6:
            glistp = CentreText("TERAFUMOS", 40, 50, 8, 1, glistp);
            break;
        case 7:
            glistp = CentreText("VELOCITAR", 40, 50, 8, 1, glistp);
            break;
    }

    if (g_gameType == GAME_TYPE_TIME_TRIAL ||
        (g_gameType == GAME_TYPE_CHALLENGE && frontendOptions_g.challengeMedal == 2)) {
        if (score->displayedRecords & HISCORE_LAP_RECORD) {
            if ((g_scoreTabFrames % 30) < 21) {
                glistp = CentreText("NEW LAP RECORD", 40, 70, 8, 0, glistp);
            }
            if (g_nameStored != 0) {
                glistp = AddText(g_hiScoreTable[track][cls].lapName, 85, 84, 8, 1, glistp);
            }
            glistp = DrawFontText("OLD RECORD", 130, 97, 8, glistp);
            glistp = DrawFontText(g_oldLapTimeStr, 195, 97, 8, glistp);
            glistp = AddTime(score->lapTime, 180, 84, 8, 1, 1, glistp);
        } else {
            glistp = CentreText("LAP RECORD", 40, 70, 8, 1, glistp);
            glistp = AddText(g_hiScoreTable[track][cls].lapName, 85, 84, 8, 1, glistp);
            glistp = AddTime(g_oldLapTime, 180, 84, 8, 1, 1, glistp);
        }
    } else {
        if (score->displayedRecords & HISCORE_RACE_RECORD) {
            if ((g_scoreTabFrames % 30) < 21) {
                glistp = CentreText("NEW RACE RECORD", 40, 70, 8, 0, glistp);
            }
            if (g_nameStored != 0) {
                glistp = AddText(g_hiScoreTable[track][cls].name, 85, 84, 8, 1, glistp);
            }
            glistp = DrawFontText("OLD RECORD", 130, 97, 8, glistp);
            glistp = DrawFontText(g_oldRaceTimeStr, 195, 97, 8, glistp);
            glistp = AddTime(score->raceTime, 180, 84, 8, 1, 1, glistp);
        } else {
            glistp = CentreText("RACE RECORD", 40, 70, 8, 0, glistp);
            glistp = AddText(g_hiScoreTable[track][cls].name, 85, 84, 8, 1, glistp);
            glistp = AddTime(g_oldRaceTime, 180, 84, 8, 1, 1, glistp);
        }

        if (score->displayedRecords & HISCORE_LAP_RECORD) {
            if ((g_scoreTabFrames % 30) < 21) {
                glistp = CentreText("NEW LAP RECORD", 40, 125, 8, 0, glistp);
            }
            if (g_nameStored != 0) {
                glistp = AddText(g_hiScoreTable[track][cls].lapName, 85, 139, 8, 1, glistp);
            }
            glistp = DrawFontText("OLD RECORD", 130, 152, 8, glistp);
            glistp = DrawFontText(g_oldLapTimeStr, 195, 152, 8, glistp);
            glistp = AddTime(score->lapTime, 180, 139, 8, 1, 1, glistp);
        } else {
            glistp = CentreText("LAP RECORD", 40, 125, 8, 1, glistp);
            glistp = AddText(g_hiScoreTable[track][cls].lapName, 85, 139, 8, 1, glistp);
            glistp = AddTime(g_oldLapTime, 180, 139, 8, 1, 1, glistp);
        }
    }

    g_scoreTabFrames++;
    return glistp;
}

Gfx *EnterHighScore(ConfigData *config, HiScoreData *score, u16 pad, Gfx *glistp) {
    u16 curPad;
    u16 stickPad;
    s16 selectPressed;

    stickPad = g_padInfo[frontendOptions_g.playerController[0]].stickPad;
    curPad = g_padInfo[frontendOptions_g.playerController[0]].pad;
    if (stickPad & PAD_UP) {
        curPad |= PAD_UP;
    }
    if (stickPad & PAD_DOWN) {
        curPad |= PAD_DOWN;
    }
    if (stickPad & PAD_LEFT) {
        curPad |= PAD_LEFT;
    }
    if (stickPad & PAD_RIGHT) {
        curPad |= PAD_RIGHT;
    }

    if (score->nameCursor != 4 && score->enteringName != 0) {
        score->dispChar[0] = score->selectedChar;
        score->dispChar[1] = 0;

        if (score->enteringName != 0) {
            if ((score->cursorBlinkFrames % 3) == 0) {
                if (score->alphabetIndex == ALPHA_DELETE || score->alphabetIndex == ALPHA_END) {
                    glistp = AddDelEnd(score->alphabetIndex - ALPHA_DELETE, score->nameEndX + 140, 200, 16, 1, glistp);
                } else {
                    glistp = AddText((char *)score->dispChar, score->nameEndX + 140, 200, 12, 1, glistp);
                }
            } else {
                if (score->alphabetIndex == ALPHA_DELETE || score->alphabetIndex == ALPHA_END) {
                    glistp = AddDelEnd(score->alphabetIndex - ALPHA_DELETE, score->nameEndX + 140, 200, 16, 0, glistp);
                } else {
                    glistp = AddText((char *)score->dispChar, score->nameEndX + 140, 200, 12, 0, glistp);
                }
            }
        }

        score->alphabetIndex = score->selectedChar - 'A';

        if ((curPad & PAD_RIGHT) && score->inputDelayFrames == 0) {
            PlayNote(26, 255, 15, 15, 256);
            score->inputDelayFrames = 5;
            score->alphabetIndex++;
            if (score->alphabetIndex > ALPHA_END) {
                if (score->nameCursor != 3) {
                    score->alphabetIndex = 0;
                } else {
                    score->alphabetIndex = ALPHA_DELETE;
                }
            }
        }

        if ((curPad & PAD_LEFT) && score->inputDelayFrames == 0) {
            PlayNote(26, 255, 15, 15, 256);
            score->inputDelayFrames = 5;
            score->alphabetIndex--;
            if (score->nameCursor != 3) {
                score->firstAlphabetIndex = 0;
            } else {
                score->firstAlphabetIndex = ALPHA_DELETE;
            }
            if (score->alphabetIndex < score->firstAlphabetIndex) {
                score->alphabetIndex = ALPHA_END;
            }
        }

        if (score->inputDelayFrames > 0) {
            score->inputDelayFrames--;
        }

        selectPressed = GetMenuKey(MENU_KEY_SELECT, pad);
        if (selectPressed && score->alphabetIndex == ALPHA_DELETE) {
            if (score->nameCursor > 0) {
                PlayNote(27, 255, 15, 15, 256);
                score->nameCursor--;
                score->nameEndX -= g_font12[score->nameEntered[score->nameCursor] - 'A'].width;
                score->nameEntered[score->nameCursor] = 0;
                score->alphabetIndex = 0;
            }
        } else {
            score->selectedChar = score->alphabetIndex + 'A';
            if (selectPressed && score->nameCursor != 4 && score->alphabetIndex != ALPHA_DELETE &&
                score->alphabetIndex != ALPHA_END) {
                PlayNote(27, 255, 15, 15, 256);
                score->nameEntered[score->nameCursor] = score->selectedChar;
                score->nameEntered[score->nameCursor + 1] = 0;
                score->nameCursor++;
                score->nameEndX += g_font12[score->alphabetIndex].width;
                if (score->nameCursor == 3) {
                    score->alphabetIndex = ALPHA_END;
                    score->selectedChar = 'A' + ALPHA_END;
                }
            } else if (selectPressed && score->alphabetIndex == ALPHA_END) {
                PlayNote(27, 255, 15, 15, 256);
                score->nameEntered[score->nameCursor] = score->selectedChar;
                score->nameEntered[score->nameCursor + 1] = 0;
                score->enteringName = 0;
            }
        }
    } else if (score->storeRecords != 0) {
        s16 scoreTrack;

        score->enteringName = 0;
        score->nameEntered[score->nameCursor] = 0;

        if (g_gameType == GAME_TYPE_TIME_TRIAL ||
            (g_gameType == GAME_TYPE_CHALLENGE && frontendOptions_g.challengeMedal == 2)) {
            scoreTrack = config->trackNum + 8;
        } else {
            scoreTrack = config->trackNum;
        }

        if (score->pendingRecords & HISCORE_RACE_RECORD) {
            StringCopy(g_hiScoreTable[scoreTrack][config->shipClass].name, (char *)score->nameEntered);
            g_hiScoreTable[scoreTrack][config->shipClass].time = score->raceTime;
            g_hiScoreTable[scoreTrack][config->shipClass].ship = score->racePilot;
            g_hiScoreTable[scoreTrack][config->shipClass].eliminations = score->raceEliminations;
        }

        if (score->pendingRecords & HISCORE_LAP_RECORD) {
            StringCopy(g_hiScoreTable[scoreTrack][config->shipClass].lapName, (char *)score->nameEntered);
            g_hiScoreTable[scoreTrack][config->shipClass].lapTime = score->lapTime;
            g_hiScoreTable[scoreTrack][config->shipClass].lapShip = score->lapPilot;
        }

        score->storeRecords = 0;
        g_nameStored = 1;
    }

    glistp = CentreText("NEW RECORD", 107, 30, 12, 0, glistp);
    if (score->enteringName != 0) {
        glistp = AddText((char *)score->nameEntered, 140, 200, 12, 1, glistp);
        glistp = CentreText("ENTER YOUR NAME", 40, 180, 8, 1, glistp);
    }

    score->cursorBlinkFrames++;

    if (score->enteringName == 0) {
        if (score->finishDelayFrames > 2) {
            if (GetMenuKey(MENU_KEY_SELECT, pad)) {
                score->highScore = 0;
                score->pendingRecords = 0;
                score->finishDelayFrames = 0;
                score->totalTime = 0;
                config->complete = 1;
                g_raceCompleteFlag = 1;
                g_oldRaceTime = -1;
                g_oldLapTime = -1;
                config->unk1E = 0;
                score->displayedRecords = 0;
                g_nameStored = 0;
                g_enteringHiScore = 0;
                return glistp;
            }
        }
        score->finishDelayFrames++;
    }

    return glistp;
}
