/*
 * pad.c — N64 controller input polling, control presets, and button labels.
 *
 * Translates raw OSContPad button/stick state into per-player PadInfo structs
 * using the 4 controller presets and maps action IDs to N64 button names.
 */
#include "pad.h"

void InitPad(void) {
    osCreateMesgQueue(&g_siMsgQ, g_siMsgs, 1);
    osSetEventMesg(OS_EVENT_SI, &g_siMsgQ, (OSMesg)1);
    InitControllers();
}

void ReadPads(void) {
    u16 i;
    u16 first;

    first = 1;
    osContGetReadData(g_contPads);

    for (i = 0; i < MAXCONTROLLERS; i++) {
        if ((g_contEnabled >> i) & 1) {
            OSContPad *cont = &g_contPads[i];
            PadInfo *info = &g_padInfo[i];

            if (first) {
                g_firstContPad.button = cont->button;
                g_firstContPad.stick_x = cont->stick_x;
                g_firstContPad.stick_y = cont->stick_y;
                g_firstContPad.errno = cont->errno;
                first = 0;
            }

            if (cont->errno == 0) {
                u16 pressed = cont->button ^ g_contButtonHeld[i];

                pressed &= cont->button;
                g_contButtonHeld[i] = cont->button;
                g_contButtonPressed[i] = pressed;

                info->opad = info->pad;
                info->ostickPad = info->stickPad;
                info->pad = 0;
                info->stickPad = 0;

                if (cont->button & U_JPAD) {
                    info->pad |= PAD_UP;
                }
                if (cont->button & D_JPAD) {
                    info->pad |= PAD_DOWN;
                }
                if (cont->button & L_JPAD) {
                    info->pad |= PAD_LEFT;
                }
                if (cont->button & R_JPAD) {
                    info->pad |= PAD_RIGHT;
                }
                if (cont->button & START_BUTTON) {
                    info->pad |= PAD_START;
                }

                if (cont->button & g_padConfigs[g_padConfigIndex[i]].view) {
                    info->pad |= PAD_VIEW;
                }
                if (cont->button & g_padConfigs[g_padConfigIndex[i]].fire) {
                    info->pad |= PAD_FIRE;
                }
                if (cont->button & g_padConfigs[g_padConfigIndex[i]].discard) {
                    info->pad |= PAD_DISCARD;
                }
                if (cont->button & g_padConfigs[g_padConfigIndex[i]].brakeLeft) {
                    info->pad |= PAD_R1;
                }
                if (cont->button & g_padConfigs[g_padConfigIndex[i]].brakeRight) {
                    info->pad |= PAD_L1;
                }
                if (cont->button & g_padConfigs[g_padConfigIndex[i]].thrust) {
                    info->pad |= PAD_THRUST;
                }

                info->stickX = cont->stick_x;
                info->stickY = cont->stick_y;
                info->xpad = (info->pad ^ info->opad) & info->pad;

                if (cont->stick_x > STICK_DIGITAL_THRESHOLD) {
                    info->stickPad |= PAD_RIGHT;
                }
                if (cont->stick_x < -STICK_DIGITAL_THRESHOLD) {
                    info->stickPad |= PAD_LEFT;
                }
                if (cont->stick_y > STICK_DIGITAL_THRESHOLD) {
                    info->stickPad |= PAD_UP;
                }
                if (cont->stick_y < -STICK_DIGITAL_THRESHOLD) {
                    info->stickPad |= PAD_DOWN;
                }

                info->xstickPad = (info->stickPad ^ info->ostickPad) & info->stickPad;
            }
        }
    }
}

char *GetButtonName(u16 action, u8 configIndex) {
    if (configIndex == 0) {
        switch (action) {
        case PAD_THRUST:
            return "A";
        case PAD_FIRE:
            return "B";
        case PAD_DISCARD:
            return "BOTTOM C";
        case PAD_L1:
            return "Z";
        case PAD_R1:
            return "R";
        case PAD_VIEW:
            return "TOP C";
        }
    } else if (configIndex == 1) {
        switch (action) {
        case PAD_THRUST:
            return "A";
        case PAD_FIRE:
            return "BOTTOM C";
        case PAD_DISCARD:
            return "B";
        case PAD_L1:
            return "Z";
        case PAD_R1:
            return "R";
        case PAD_VIEW:
            return "TOP C";
        }
    } else if (configIndex == 2) {
        switch (action) {
        case PAD_THRUST:
            return "A";
        case PAD_FIRE:
            return "B";
        case PAD_DISCARD:
            return "BOTTOM C";
        case PAD_L1:
            return "L";
        case PAD_R1:
            return "R";
        case PAD_VIEW:
            return "TOP C";
        }
    } else if (configIndex == 3) {
        switch (action) {
        case PAD_THRUST:
            return "A";
        case PAD_FIRE:
            return "BOTTOM C";
        case PAD_DISCARD:
            return "B";
        case PAD_L1:
            return "L";
        case PAD_R1:
            return "R";
        case PAD_VIEW:
            return "TOP C";
        }
    } else {
        return "UNKNOWN INDEX";
    }

    return "UNKNOWN ACTION";
}
