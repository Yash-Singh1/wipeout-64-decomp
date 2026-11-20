#include "shipefx.h"

#ifdef VERSION_EU
#define BRAKE_ROT_STEP   0x26
#define MAX_CHARGE_TIME  50
#define MAX_CHARGE_TIMEF 50.0f
#else
#define BRAKE_ROT_STEP   0x20
#define MAX_CHARGE_TIME  60
#define MAX_CHARGE_TIMEF 60.0f
#endif

void InitPadSettings(void) {
    g_padButtons.cross = 0x4000;
    g_padButtons.triangle = 0x1000;
    g_padButtons.circle = 0x2000;
    g_padButtons.square = 0x8000;
    g_padButtons.r2 = 0x0200;
    g_padButtons.l2 = 0x0100;
    g_padButtons.up = 0x0010;
    g_padButtons.down = 0x0040;
    g_padButtons.left = 0x0080;
    g_padButtons.right = 0x0020;

    keyDefineView[0] = 0x1000;
    keyDefineFire[0] = 0x2000;
    keyDefineDitch[0] = 0x8000;
    keyDefineLBrake[0] = 0x0200;
    keyDefineRBrake[0] = 0x0100;
    keyDefineThrustPad[0] = 0x4000;

    keyDefineView[1] = 0x1000;
    keyDefineFire[1] = 0x8FF1;
    keyDefineDitch[1] = 0x2000;
    keyDefineLBrake[1] = 0x0800;
    keyDefineRBrake[1] = 0x8FF2;
    keyDefineThrustPad[1] = 0x8FF0;
}

void SetBrake(ShipData *ship, ControlInput *control) {
    if (control->buttons & CONTROL_RIGHT_BRAKE) {
        ship->r_brake_rot += BRAKE_ROT_STEP;
    } else if (ship->r_brake_rot > 0) {
        ship->r_brake_rot -= BRAKE_ROT_STEP;
    }
    if (ship->r_brake_rot < 0) {
        ship->r_brake_rot = 0;
    }
    if (ship->r_brake_rot > 0x100) {
        ship->r_brake_rot = 0x100;
    }

    if (control->buttons & CONTROL_LEFT_BRAKE) {
        ship->l_brake_rot += BRAKE_ROT_STEP;
    } else if (ship->l_brake_rot > 0) {
        ship->l_brake_rot -= BRAKE_ROT_STEP;
    }
    if (ship->l_brake_rot < 0) {
        ship->l_brake_rot = 0;
    }
    if (ship->l_brake_rot > 0x100) {
        ship->l_brake_rot = 0x100;
    }
}

void SetView(ShipData *ship, ControlInput *control, s32 shipNo) {
    if (control->pressed & CONTROL_VIEW) {
        if (!(ship->attr & SHIP_ATTR_VIEW_IN)) {
            if (shipNo == ownShip) {
                CameraUpdate = UpdateCameraRaceInternal;
            } else if (shipNo == ownShip2) {
                CameraUpdate2 = UpdateCameraRaceInternal;
            } else if (shipNo == ownShip3) {
                CameraUpdate3 = UpdateCameraRaceInternal;
            } else if (shipNo == ownShip4) {
                CameraUpdate4 = UpdateCameraRaceInternal;
            }
            ship->attr |= SHIP_ATTR_VIEW_IN;
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
            ship->attr &= ~SHIP_ATTR_VIEW_IN;
        }
    }
}

void SetFire(ShipData *ship, WeaponData *weapons, s32 shipNo, ControlInput *control) {
    if (ship->weapon == WEAPON_CYCLONE) {
        if ((g_weaponCharge[shipNo] == 0 && (control->pressed & CONTROL_FIRE)) ||
            (g_weaponCharge[shipNo] > 0 && (control->buttons & CONTROL_FIRE))) {
            if (!(ship->attr & SHIP_ATTR_SHIELDED)) {
                if (g_chargeSfxHandle == 0) {
                    g_chargeSfxHandle = PlaySfxLoop(0x11, 0xFF, 0x80, 0, -1);
                } else {
                    SetSfxPitch(g_chargeSfxHandle, (f32)g_weaponCharge[shipNo] * 8.0f / MAX_CHARGE_TIMEF);
                }
                g_weaponCharge[shipNo]++;
                if (g_weaponCharge[shipNo] > MAX_CHARGE_TIME) {
                    g_weaponCharge[shipNo] = MAX_CHARGE_TIME;
                }
                SpawnCycloneCharge(shipNo, g_weaponCharge[shipNo]);
            } else if (g_weaponCharge[shipNo] == 0) {
                PlayNote(0x1A, 0xFF, 0xF, 0xF, 0x100);
            }
        } else if (g_weaponCharge[shipNo] > 0) {
            if (g_chargeSfxHandle != 0) {
                StopSfxLoop(&g_chargeSfxHandle);
            }
            PlayNote(0x20, 0xA0, 0xF, 0xF, 0x100);
            FireWeapon(ship, weapons, shipNo, WEAPON_CYCLONE);
            if (cheatWeapon != 0) {
                GetCheatWeapon(ships, shipNo);
            }
        }
    } else if ((control->pressed & CONTROL_FIRE) && (ship->weapon != WEAPON_NONE)) {
        if ((shipNo == ownShip && g_fireLock[0] == 0) ||
            (shipNo == ownShip2 && g_fireLock[1] == 0) ||
            (shipNo == ownShip3 && g_fireLock[2] == 0) ||
            (shipNo == ownShip4 && g_fireLock[3] == 0) ||
            (shipNo == g_demoShip && g_gameMode == 1)) {
            if (control->rawButtons & CONTROL_DOWN) {
                ship->rearFire = 1;
            } else {
                ship->rearFire = 0;
            }
            if (!(ship->attr & SHIP_ATTR_SHIELDED) && !(ship->combatAttr & SHIP_COMBAT_NO_FIRE)) {
                FireWeapon(ship, weapons, shipNo, ship->weapon);
                if (cheatWeapon != 0) {
                    GetCheatWeapon(ships, shipNo);
                }
            } else {
                PlayNote(0x1A, 0xFF, 0xF, 0xF, 0x100);
            }
        }
    }
}

void SetDitch(ShipData *ship, WeaponData *weapons, s32 shipNo, ControlInput *control) {
    if (control->pressed & CONTROL_DITCH) {
        if (ship->weapon != WEAPON_NONE) {
            PlayNote(0x10, 0xFF, 0xF, 0xF, 0x100);
            if (ship->weapon == WEAPON_SUPER_ALT) {
                g_superWeaponActive1 = 0;
            } else if (ship->weapon == WEAPON_CYCLONE) {
                g_superWeaponActive2 = 0;
            }
            ship->weapon = WEAPON_NONE;
        }
        if (cheatWeapon != 0) {
            GetCheatWeapon(ships, ownShip);
        }
    }
}

s16 GetMenuKey(s16 action, u16 keysPressed) {
    if ((g_menuPadDown & CHEAT_MODIFIERS) == CHEAT_MODIFIERS) {
        return 0;
    }
    switch (action) {
        case MENU_KEY_UP:
            return keysPressed & CONTROL_UP;
        case MENU_KEY_DOWN:
            return keysPressed & CONTROL_DOWN;
        case MENU_KEY_LEFT:
            return keysPressed & CONTROL_LEFT;
        case MENU_KEY_RIGHT:
            return keysPressed & CONTROL_RIGHT;
        case MENU_KEY_BACK:
            return keysPressed & CONTROL_FIRE;
        case MENU_KEY_SELECT:
            return keysPressed & CONTROL_THRUST;
        case MENU_KEY_START:
            return keysPressed & CONTROL_START;
    }
    return 0;
}

void UpdateCheatSequence(void) {
    s32 i;
    u16 prev;

    prev = g_prevCheatButtons;
    g_prevCheatButtons = g_rawPadButtons;
    g_cheatKeysPressed = g_rawPadButtons & (g_rawPadButtons ^ prev);

    if (g_cheatKeysPressed != 0) {
        for (i = 15; i > 0; i--) {
            g_cheatHistory[i] = g_cheatHistory[i - 1];
        }
        g_cheatHistory[0] = g_cheatKeysPressed;
        if ((g_rawPadButtons & CHEAT_MODIFIERS) == CHEAT_MODIFIERS) {
            g_cheatHistory[0] = g_cheatKeysPressed | CHEAT_MODIFIERS;
        }

        if (g_appState == 1 && g_gameMode == 0) {
            if (g_cheatHistory[6] == CHEAT_C_DOWN && g_cheatHistory[5] == g_cheatHistory[6] &&
                g_cheatHistory[4] == CHEAT_C_LEFT && g_cheatHistory[3] == g_cheatHistory[4] &&
                g_cheatHistory[2] == CHEAT_C_RIGHT && g_cheatHistory[1] == g_cheatHistory[2] &&
                g_cheatHistory[0] == CHEAT_C_UP) {
                cheatWeapon ^= 1;
                if (cheatWeapon != 0) {
                    GetCheatWeapon(ships, ownShip);
                }
                cheatfade = 0xFF;
            }
            if (g_cheatHistory[7] == CHEAT_C_UP && g_cheatHistory[6] == CHEAT_C_DOWN &&
                g_cheatHistory[5] == CHEAT_C_LEFT && g_cheatHistory[4] == CHEAT_C_RIGHT &&
                g_cheatHistory[3] == g_cheatHistory[7] && g_cheatHistory[2] == g_cheatHistory[6] &&
                g_cheatHistory[1] == g_cheatHistory[5] && g_cheatHistory[0] == g_cheatHistory[4]) {
                cheatDamage ^= 1;
                cheatfade = 0xFF;
            }
            if (g_cheatHistory[7] == CHEAT_C_UP && g_cheatHistory[6] == CHEAT_C_RIGHT &&
                g_cheatHistory[5] == CHEAT_C_LEFT && g_cheatHistory[4] == CHEAT_C_DOWN &&
                g_cheatHistory[3] == g_cheatHistory[7] && g_cheatHistory[2] == g_cheatHistory[6] &&
                g_cheatHistory[1] == g_cheatHistory[5] && g_cheatHistory[0] == g_cheatHistory[4]) {
                cheatTime ^= 1;
                cheatfade = 0xFF;
            }
        } else if (g_appState == 4 && g_frontendMenuState == 1) {
            if (g_cheatHistory[4] == CHEAT_C_LEFT && g_cheatHistory[3] == CHEAT_C_RIGHT &&
                g_cheatHistory[2] == CHEAT_C_UP && g_cheatHistory[1] == g_cheatHistory[3] &&
                g_cheatHistory[0] == g_cheatHistory[4]) {
                cheatTrack ^= 1;
                g_trackMenu[6].enabled ^= 1;
                if (g_velocitarUnlocked != 0) {
                    g_trackMenu[6].enabled = 1;
                }
                if (g_trackMenu[6].enabled == 0) {
                    if (g_trackMenu[6].unselected == 0) {
                        g_trackMenu[6].unselected = 1;
                        g_trackMenu[5].unselected = 0;
                    }
                }
                cheatfade = 0xFF;
            }
            if (g_cheatHistory[6] == CHEAT_C_DOWN && g_cheatHistory[5] == g_cheatHistory[6] &&
                g_cheatHistory[4] == g_cheatHistory[5] && g_cheatHistory[3] == g_cheatHistory[4] &&
                g_cheatHistory[2] == CHEAT_C_RIGHT && g_cheatHistory[1] == CHEAT_C_UP &&
                g_cheatHistory[0] == CHEAT_C_LEFT) {
                cheatTeam ^= 1;
                g_teamMenu[4].enabled ^= 1;
                if (g_piranhaUnlocked != 0) {
                    g_teamMenu[4].enabled = 1;
                }
                if (g_teamMenu[4].enabled == 0) {
                    if (g_teamMenu[4].unselected == 0) {
                        g_teamMenu[4].unselected = 1;
                        g_teamMenu[3].unselected = 0;
                    }
                }
                cheatfade = 0xFF;
            }
            if (g_cheatHistory[6] == CHEAT_C_LEFT && g_cheatHistory[5] == CHEAT_C_RIGHT &&
                g_cheatHistory[4] == CHEAT_C_DOWN && g_cheatHistory[3] == g_cheatHistory[6] &&
                g_cheatHistory[2] == g_cheatHistory[5] && g_cheatHistory[1] == g_cheatHistory[4] &&
                g_cheatHistory[0] == CHEAT_C_UP) {
                cheatSuperCombo ^= 1;
                cheatfade = 0xFF;
                if (cheatSuperCombo != 0) {
                    g_optionsMenu[6].value = 1;
                } else {
                    g_optionsMenu[6].value = 0;
                }
                g_superComboOption = g_optionsMenu[6].value;
                if (g_optionsMenu[6].enabled == 0) {
                    if (g_optionsMenu[6].unselected == 0) {
                        g_optionsMenu[6].unselected = 1;
                        g_optionsMenu[5].unselected = 0;
                    }
                }
            }
        }
    }
}
