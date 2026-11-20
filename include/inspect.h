#ifndef WIPEOUT64_INSPECT_H
#define WIPEOUT64_INSPECT_H

#include "common.h"
#include "dheap.h"
#include "error.h"
#include "files.h"

#ifndef Fatal
#define Fatal 2
#endif

/*
 * Temporary US/EU symbol-address aliases for unnamed symbols not yet added to
 * config/{us,eu}/symbol_addrs.txt and config/common_syms.ld.
 */
#define _aiShipsFile            D_3E5460
#define _aiDamageFile           D_3E5700

#ifdef VERSION_EU
#define InspectStub_8006F540    func_800714E0
#define InspectStub_8006F548    func_800714E8
#define InspectStub_8006F550    func_800714F0
#define InspectStub_8006F558    func_800714F8
#define InspectStub_8006F560    func_80071500
#define InspectStub_8006F568    func_80071508
#define InspectStub_8006F570    func_80071510
#define InspectLoadAiDamage     func_80071518
#define InspectLoadAiShips      func_800717A4
#define InspectStub_8006FC38    func_80071BD8
#define InspectStub_8006FC40    func_80071BE0

#ifndef errorString
#define errorString             D_800AE520
#endif
#define g_baseDamage            D_800914CC
#define g_weaponDamage          D_800914D0
#define g_shipMaxShield         D_80089FD0
#define g_aiShipMaxShield       D_80089FE4
#define g_classDamageMod        D_80091510
#define g_classShieldMod        D_80091520

#define g_spreadScaleVector     D_8008A0F8
#define g_spreadScaleVenom      D_8008A118
#define g_spreadScaleRapier     D_8008A138
#define g_spreadScalePhantom    D_8008A158
#define g_aiMaxThrustVector     D_8008A298
#define g_aiMaxThrustVenom      D_8008A310
#define g_aiMaxThrustRapier     D_8008A388
#define g_aiMaxThrustPhantom    D_8008A400
#define g_aiThrustMagVector     D_8008A2D4
#define g_aiThrustMagVenom      D_8008A34C
#define g_aiThrustMagRapier     D_8008A3C4
#define g_aiThrustMagPhantom    D_8008A43C
#define g_aiCatchupMaxThrust    D_80091158
#define g_aiCatchupThrustMag    D_80091168
#define g_aiBoostMaxThrust      D_80091178
#define g_aiBoostThrustMag      D_80091188
#else
#define InspectStub_8006F540    func_8006F540
#define InspectStub_8006F548    func_8006F548
#define InspectStub_8006F550    func_8006F550
#define InspectStub_8006F558    func_8006F558
#define InspectStub_8006F560    func_8006F560
#define InspectStub_8006F568    func_8006F568
#define InspectStub_8006F570    func_8006F570
#define InspectLoadAiDamage     func_8006F578
#define InspectLoadAiShips      func_8006F804
#define InspectStub_8006FC38    func_8006FC38
#define InspectStub_8006FC40    func_8006FC40

#ifndef errorString
#define errorString             D_800A0F40
#endif
#define g_baseDamage            D_80091E9C
#define g_weaponDamage          D_80091EA0
#define g_shipMaxShield         D_8008A9A0
#define g_aiShipMaxShield       D_8008A9B4
#define g_classDamageMod        D_80091EE0
#define g_classShieldMod        D_80091EF0

#define g_spreadScaleVector     D_8008AAC8
#define g_spreadScaleVenom      D_8008AAE8
#define g_spreadScaleRapier     D_8008AB08
#define g_spreadScalePhantom    D_8008AB28
#define g_aiMaxThrustVector     D_8008AC68
#define g_aiMaxThrustVenom      D_8008ACE0
#define g_aiMaxThrustRapier     D_8008AD58
#define g_aiMaxThrustPhantom    D_8008ADD0
#define g_aiThrustMagVector     D_8008ACA4
#define g_aiThrustMagVenom      D_8008AD1C
#define g_aiThrustMagRapier     D_8008AD94
#define g_aiThrustMagPhantom    D_8008AE0C
#define g_aiCatchupMaxThrust    D_80091B28
#define g_aiCatchupThrustMag    D_80091B38
#define g_aiBoostMaxThrust      D_80091B48
#define g_aiBoostThrustMag      D_80091B58
#endif

typedef struct InspectAiDamageData {
    /* 0x00 */ s32 weaponDamage[16];
    /* 0x40 */ s32 baseDamage;
    /* 0x44 */ s32 shipMaxShield[5];
    /* 0x58 */ s32 aiShipMaxShield[5];
    /* 0x6C */ s32 classDamageMod[4];
    /* 0x7C */ s32 classShieldMod[4];
} InspectAiDamageData; /* size = 0x8C */

typedef struct InspectAiShipData {
    /* 0x000 */ s32 spreadScaleVector[8];
    /* 0x020 */ s32 spreadScaleVenom[8];
    /* 0x040 */ s32 spreadScaleRapier[8];
    /* 0x060 */ s32 spreadScalePhantom[8];
    /* 0x080 */ s32 aiMaxThrustVector[15];
    /* 0x0BC */ s32 aiMaxThrustVenom[15];
    /* 0x0F8 */ s32 aiMaxThrustRapier[15];
    /* 0x134 */ s32 aiMaxThrustPhantom[15];
    /* 0x170 */ s32 aiThrustMagVector[15];
    /* 0x1AC */ s32 aiThrustMagVenom[15];
    /* 0x1E8 */ s32 aiThrustMagRapier[15];
    /* 0x224 */ s32 aiThrustMagPhantom[15];
    /* 0x260 */ s32 aiCatchupMaxThrust[4];
    /* 0x270 */ s32 aiCatchupThrustMag[4];
    /* 0x280 */ s32 aiBoostMaxThrust[4];
    /* 0x290 */ s32 aiBoostThrustMag[4];
} InspectAiShipData; /* size = 0x2A0 */

extern u8 _aiShipsFile[];
extern u8 _aiDamageFile[];

extern char errorString[256];

extern s32 g_baseDamage;
extern s32 g_weaponDamage[16];
extern s32 g_shipMaxShield[5];
extern s32 g_aiShipMaxShield[5];
extern s32 g_classDamageMod[4];
extern s32 g_classShieldMod[4];

extern s32 g_spreadScaleVector[8];
extern s32 g_spreadScaleVenom[8];
extern s32 g_spreadScaleRapier[8];
extern s32 g_spreadScalePhantom[8];
extern s32 g_aiMaxThrustVector[15];
extern s32 g_aiMaxThrustVenom[15];
extern s32 g_aiMaxThrustRapier[15];
extern s32 g_aiMaxThrustPhantom[15];
extern s32 g_aiThrustMagVector[15];
extern s32 g_aiThrustMagVenom[15];
extern s32 g_aiThrustMagRapier[15];
extern s32 g_aiThrustMagPhantom[15];
extern s32 g_aiCatchupMaxThrust[4];
extern s32 g_aiCatchupThrustMag[4];
extern s32 g_aiBoostMaxThrust[4];
extern s32 g_aiBoostThrustMag[4];

int sprintf(char *dst, const char *fmt, ...);
void *memcpy(void *dst, const void *src, size_t len);

void InspectStub_8006F540(void);
void InspectStub_8006F548(void);
void InspectStub_8006F550(void);
void InspectStub_8006F558(void);
void InspectStub_8006F560(void);
void InspectStub_8006F568(void);
void InspectStub_8006F570(void);
void InspectLoadAiDamage(void);
void InspectLoadAiShips(void);
void InspectStub_8006FC38(void);
void InspectStub_8006FC40(void);

#endif /* WIPEOUT64_INSPECT_H */
