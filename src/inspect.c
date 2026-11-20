#include "inspect.h"

#ident "$Revision: 1.17 $"
#ident "$Revision: 7.27 $"
#ident "$Revision: 1.5 $"

void InspectStub_8006F540(void) {
}

void InspectStub_8006F548(void) {
}

void InspectStub_8006F550(void) {
}

void InspectStub_8006F558(void) {
}

void InspectStub_8006F560(void) {
}

void InspectStub_8006F568(void) {
}

void InspectStub_8006F570(void) {
}

void InspectLoadAiDamage(void) {
    InspectAiDamageData data;
    s32 length;
    void *fileBuf;
    s32 i;

    length = FileLength((u32)_aiDamageFile);
    fileBuf = DAlloc(g_heap, length + 32, "inspect.c", 744);
    if (!fileBuf) {
        sprintf(errorString, "inspect.c : InspectLoadAiDamage (): Failed to allocate memory for file \n");
        Error(errorString, Fatal);
    }

    if (LoadFile((u32)_aiDamageFile, fileBuf) != length) {
        sprintf(errorString, "inspect.c : InspectLoadAiDamage (): Failed to load file \n");
        Error(errorString, Fatal);
    }

    memcpy(&data, fileBuf, sizeof(InspectAiDamageData));
    DFree(g_heap, fileBuf);

    for (i = 0; i < 16; i++) {
        g_weaponDamage[i] = data.weaponDamage[i];
    }

    g_baseDamage = data.baseDamage;

    for (i = 0; i < 5; i++) {
        g_shipMaxShield[i] = data.shipMaxShield[i];
    }

    for (i = 0; i < 5; i++) {
        g_aiShipMaxShield[i] = data.aiShipMaxShield[i];
    }

    for (i = 0; i < 4; i++) {
        g_classDamageMod[i] = data.classDamageMod[i];
    }

    for (i = 0; i < 4; i++) {
        g_classShieldMod[i] = data.classShieldMod[i];
    }
}

void InspectLoadAiShips(void) {
    InspectAiShipData data;
    s32 length;
    void *fileBuf;
    s32 i;

    length = FileLength((u32)_aiShipsFile);
    fileBuf = DAlloc(g_heap, length + 32, "inspect.c", 795);
    if (!fileBuf) {
        sprintf(errorString, "inspect.c : InspectLoadAiShips (): Failed to allocate memory for file \n");
        Error(errorString, Fatal);
    }

    if (LoadFile((u32)_aiShipsFile, fileBuf) != length) {
        sprintf(errorString, "inspect.c : InspectLoadAiShips (): Failed to load file \n");
        Error(errorString, Fatal);
    }

    memcpy(&data, fileBuf, sizeof(InspectAiShipData));
    DFree(g_heap, fileBuf);

    for (i = 0; i < 8; i++) {
        g_spreadScaleVector[i] = data.spreadScaleVector[i];
    }
    for (i = 0; i < 8; i++) {
        g_spreadScaleVenom[i] = data.spreadScaleVenom[i];
    }
    for (i = 0; i < 8; i++) {
        g_spreadScaleRapier[i] = data.spreadScaleRapier[i];
    }
    for (i = 0; i < 8; i++) {
        g_spreadScalePhantom[i] = data.spreadScalePhantom[i];
    }

    for (i = 0; i < 15; i++) {
        g_aiMaxThrustVector[i] = data.aiMaxThrustVector[i];
    }
    for (i = 0; i < 15; i++) {
        g_aiMaxThrustVenom[i] = data.aiMaxThrustVenom[i];
    }
    for (i = 0; i < 15; i++) {
        g_aiMaxThrustRapier[i] = data.aiMaxThrustRapier[i];
    }
    for (i = 0; i < 15; i++) {
        g_aiMaxThrustPhantom[i] = data.aiMaxThrustPhantom[i];
    }

    for (i = 0; i < 15; i++) {
        g_aiThrustMagVector[i] = data.aiThrustMagVector[i];
    }
    for (i = 0; i < 15; i++) {
        g_aiThrustMagVenom[i] = data.aiThrustMagVenom[i];
    }
    for (i = 0; i < 15; i++) {
        g_aiThrustMagRapier[i] = data.aiThrustMagRapier[i];
    }
    for (i = 0; i < 15; i++) {
        g_aiThrustMagPhantom[i] = data.aiThrustMagPhantom[i];
    }

    for (i = 0; i < 4; i++) {
        g_aiCatchupMaxThrust[i] = data.aiCatchupMaxThrust[i];
    }
    for (i = 0; i < 4; i++) {
        g_aiCatchupThrustMag[i] = data.aiCatchupThrustMag[i];
    }
    for (i = 0; i < 4; i++) {
        g_aiBoostMaxThrust[i] = data.aiBoostMaxThrust[i];
    }
    for (i = 0; i < 4; i++) {
        g_aiBoostThrustMag[i] = data.aiBoostThrustMag[i];
    }
}

void InspectStub_8006FC38(void) {
}

void InspectStub_8006FC40(void) {
}
