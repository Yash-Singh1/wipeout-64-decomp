#include "sfx.h"

static const char rcsid[] = "$Revision: 1.17 $";

void AdjustEngineSound(ShipData *playerShip, Skeleton *camPos) {
    char unused[64]; /* not referenced, but the retail frame still reserves it */
    s16 vol;
    s16 pitch;
    s16 brake;
    u8 allDestroyed;

    allDestroyed = 0;

    if (g_scrapeVol > 0) {
        g_scrapeVol -= 10;
    }
    if (g_scrapeVol < 0) {
        g_scrapeVol = 0;
    }
    MusHandleSetVolume(g_scrapeHandle, g_scrapeVol);

    if (ships[ownShip].combatAttr & SHIP_COMBAT_DESTROYED) {
        if (frontendOptions_g.numPlayers >= 2) {
            playerShip = &ships[ownShip2];
            if (playerShip->combatAttr & SHIP_COMBAT_DESTROYED) {
                if (frontendOptions_g.numPlayers >= 3) {
                    playerShip = &ships[ownShip3];
                    if (playerShip->combatAttr & SHIP_COMBAT_DESTROYED) {
                        if (frontendOptions_g.numPlayers == 4) {
                            playerShip = &ships[ownShip4];
                            if (playerShip->combatAttr & SHIP_COMBAT_DESTROYED) {
                                allDestroyed = 1;
                            }
                        } else {
                            allDestroyed = 1;
                        }
                    }
                } else {
                    allDestroyed = 1;
                }
            }
        } else {
            allDestroyed = 1;
        }
    }

    if (allDestroyed) {
        MusHandleSetVolume(g_engineHandle, 0);
        MusHandleSetVolume(g_turbHandle, 0);
    } else if (frontendOptions_g.numPlayers != 1) {
        MusHandleSetVolume(g_engineHandle, 128);
        MusHandleSetVolume(g_turbHandle, 128);
    } else {
        vol = ((s32)playerShip->speed << 12) >> 20;
        if (vol > 64) {
            vol = 64;
        }
        if (vol < 0) {
            vol = 0;
        }

        pitch = playerShip->speed >> 8;
        if (playerShip->attr & SHIP_ATTR_VIEW_IN) {
            vol += 100;
            if (vol > 256) {
                vol = 256;
            }
        } else {
            vol += 50;
            if (vol > 256) {
                vol = 256;
            }
        }

        MusHandleSetFreqOffset(g_engineHandle, ((f32)(pitch - 75) * 3.0f) / 150.0f);

        if (!(playerShip->combatAttr & SHIP_COMBAT_MUTE_ENGINE)) {
            MusHandleSetVolume(g_engineHandle, vol);
        }

        vol = (playerShip->r_brake_rot + playerShip->l_brake_rot) >> 1;
        if (vol > 100) {
            vol -= 28;
        }
        if (vol > 175) {
            vol = 175;
        }

        brake = playerShip->speed >> 8;
        if (brake > 128) {
            brake = 128;
        }

        vol += brake;
        if (vol > 256) {
            vol = 256;
        }

        if (!(playerShip->combatAttr & SHIP_COMBAT_MUTE_ENGINE)) {
            MusHandleSetVolume(g_turbHandle, vol);
        }
    }
}

void StartEngineSfx(void) {
    if (!g_scrapeHandle) {
        g_scrapeHandle = MusStartEffect2(SFX_SCRAPE, 0, 128, 0, -1);
        MusHandleSetFreqOffset(g_scrapeHandle, -1.0f / 24.0f);
    }
    if (!g_turbHandle) {
        g_turbHandle = MusStartEffect2(SFX_TURBULENCE, 0, 128, 0, -1);
    }
    if (!g_engineHandle) {
        g_engineHandle = MusStartEffect2(SFX_ENGINE, 0, 128, 0, -1);
    }
}

void StopEngineSfx(void) {
    if (g_scrapeHandle) {
        MusHandleStop(g_scrapeHandle, 0);
        g_scrapeHandle = 0;
    }
    if (g_turbHandle) {
        MusHandleStop(g_turbHandle, 0);
        g_turbHandle = 0;
    }
    if (g_engineHandle) {
        MusHandleStop(g_engineHandle, 0);
        g_engineHandle = 0;
    }
}

void SetSfxVolume(musHandle handle, s32 volume) {
    if (handle) {
        MusHandleSetVolume(handle, volume);
    }
}

void StopSfx(musHandle *handle) {
    if (*handle) {
        MusHandleStop(*handle, 0);
    }
    *handle = 0;
}

musHandle PlaySfx(s32 number, s32 volume, s32 pan, s32 restartflag, s32 priority) {
    return MusStartEffect2(number, volume, pan, restartflag, priority);
}

s32 SetSfxFreqOffset(musHandle handle, f32 offset) {
    return MusHandleSetFreqOffset(handle, offset);
}
