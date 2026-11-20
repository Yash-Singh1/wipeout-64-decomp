#ifndef WIPEOUT64_SHIPEFX_H
#define WIPEOUT64_SHIPEFX_H

#include "common.h"

/* Temporary symbol aliases until config/{us,eu}/symbol_addrs.txt are updated. */
#ifdef VERSION_EU
#define InitPadSettings          func_8002E620
#define SetBrake                 func_8002E6EC
#define SetView                  func_8002E794
#define SetFire                  func_8002E8C8
#define SetDitch                 func_8002EBB4
#define GetMenuKey               func_8002EC5C
#define UpdateCheatSequence      func_8002ECE8

#define PlayNote                 func_8001BCE8
#define UpdateCameraRaceExternal func_80021BEC
#define UpdateCameraRaceInternal func_80021F3C
#define FireWeapon               func_8003E9A8
#define GetCheatWeapon           func_8004B72C
#define SpawnCycloneCharge       func_8006A528
#define StopSfxLoop              func_8007200C
#define PlaySfxLoop              func_80072048
#define SetSfxPitch              func_8007206C

#define g_demoShip               D_80089FA4
#define cheatfade                D_80091252
#define cheatWeapon              D_80091254
#define cheatDamage              D_80091256
#define cheatTime                D_80091257
#define cheatSuperCombo          D_80091258
#define cheatTeam                D_80091259
#define cheatTrack               D_8009125A
#define g_cheatKeysPressed       D_8009125C
#define g_superWeaponActive1     D_800914A8
#define g_superWeaponActive2     D_800914AC
#define g_appState               D_80091730
#define g_frontendMenuState      D_8009193C
#define g_optionsMenu            D_80092314
#define g_trackMenu              D_80092654
#define g_teamMenu               D_80092854
#define g_velocitarUnlocked      D_800942C6
#define g_piranhaUnlocked        D_800943FA
#define g_chargeSfxHandle        D_800968BC
#define keyDefineThrustPad       D_800A9240
#define g_rawPadButtons          D_800A9304
#define CameraUpdate             D_800AE624
#define CameraUpdate2            D_800B33F8
#define CameraUpdate3            D_800B33FC
#define CameraUpdate4            D_800B3400
#define ownShip                  D_800B3414
#define ownShip2                 D_800B756C
#define ownShip3                 D_800B7570
#define ownShip4                 D_800B7574
#define keyDefineFire            D_800B7578
#define g_menuPadDown            D_800B75C8
#define keyDefineView            D_800B75E8
#define g_gameMode               D_800B7604
#define keyDefineDitch           D_800B761C
#define g_padButtons             D_800B7620
#define ships                    D_800B767C
#define keyDefineLBrake          D_800B76A0
#define keyDefineRBrake          D_800B76A4
#define g_prevCheatButtons       D_800B84A0
#define g_fireLock               D_800B84A4
#define g_weaponCharge           D_800B84A8
#define g_cheatHistory           D_800B84C8
#define g_superComboOption       D_800C5EBE
#else
#define InitPadSettings          func_80026F80
#define SetBrake                 func_8002704C
#define SetView                  func_800270F4
#define SetFire                  func_80027228
#define SetDitch                 func_80027514
#define GetMenuKey               func_800275BC
#define UpdateCheatSequence      func_80027648

#define PlayNote                 func_80014D38
#define UpdateCameraRaceExternal func_8001ACAC
#define UpdateCameraRaceInternal func_8001AF88
#define FireWeapon               func_800372B8
#define GetCheatWeapon           func_80043A08
#define SpawnCycloneCharge       func_800686F4
#define StopSfxLoop              func_8007006C
#define PlaySfxLoop              func_800700A8
#define SetSfxPitch              func_800700CC

#define g_demoShip               D_8008A974
#define cheatfade                D_80091C22
#define cheatWeapon              D_80091C24
#define cheatDamage              D_80091C26
#define cheatTime                D_80091C27
#define cheatSuperCombo          D_80091C28
#define cheatTeam                D_80091C29
#define cheatTrack               D_80091C2A
#define g_cheatKeysPressed       D_80091C2C
#define g_superWeaponActive1     D_80091E78
#define g_superWeaponActive2     D_80091E7C
#define g_appState               D_80092180
#define g_frontendMenuState      D_8009238C
#define g_optionsMenu            D_80092D64
#define g_trackMenu              D_800930A4
#define g_teamMenu               D_800932A4
#define g_velocitarUnlocked      D_80094D16
#define g_piranhaUnlocked        D_80094E4A
#define g_chargeSfxHandle        D_8009730C
#define keyDefineThrustPad       D_8009BC60
#define g_rawPadButtons          D_8009BD24
#define CameraUpdate             D_800A1044
#define CameraUpdate2            D_800A5E18
#define CameraUpdate3            D_800A5E1C
#define CameraUpdate4            D_800A5E20
#define ownShip                  D_800A5E2C
#define ownShip2                 D_800A9F8C
#define ownShip3                 D_800A9F90
#define ownShip4                 D_800A9F94
#define keyDefineFire            D_800A9F98
#define g_menuPadDown            D_800A9FE8
#define keyDefineView            D_800AA008
#define g_gameMode               D_800AA024
#define keyDefineDitch           D_800AA040
#define g_padButtons             D_800AA048
#define ships                    D_800AA0A4
#define keyDefineLBrake          D_800AA0CC
#define keyDefineRBrake          D_800AA0D0
#define g_prevCheatButtons       D_800AAED0
#define g_fireLock               D_800AAED4
#define g_weaponCharge           D_800AAED8
#define g_cheatHistory           D_800AAEF8
#define g_superComboOption       D_800C5B7E
#endif

/* ControlInput button bits. */
#define CONTROL_START            0x0008
#define CONTROL_UP               0x0010
#define CONTROL_RIGHT            0x0020
#define CONTROL_DOWN             0x0040
#define CONTROL_LEFT             0x0080
#define CONTROL_LEFT_BRAKE       0x0400
#define CONTROL_RIGHT_BRAKE      0x0800
#define CONTROL_VIEW             0x1000
#define CONTROL_FIRE             0x2000
#define CONTROL_THRUST           0x4000
#define CONTROL_DITCH            0x8000

/* N64 cheat modifier buttons (Z | L | R) and C-button chords. */
#define CHEAT_MODIFIERS          0x2030
#define CHEAT_C_RIGHT            (CHEAT_MODIFIERS | 0x0001)
#define CHEAT_C_LEFT             (CHEAT_MODIFIERS | 0x0002)
#define CHEAT_C_DOWN             (CHEAT_MODIFIERS | 0x0004)
#define CHEAT_C_UP               (CHEAT_MODIFIERS | 0x0008)

/* Ship attribute / weapon flags. */
#define SHIP_ATTR_VIEW_IN        0x0004
#define SHIP_ATTR_SHIELDED       0x4000
#define SHIP_COMBAT_NO_FIRE      0x0100

#define WEAPON_NONE              0x00
#define WEAPON_CYCLONE           0x10
#define WEAPON_SUPER_ALT         0x11

/* Menu key queries for GetMenuKey. */
#define MENU_KEY_UP              0
#define MENU_KEY_DOWN            1
#define MENU_KEY_LEFT            2
#define MENU_KEY_RIGHT           3
#define MENU_KEY_BACK            4
#define MENU_KEY_SELECT          5
#define MENU_KEY_START           6

typedef struct PadButtons {
    /* 0x00 */ u16 cross;
    /* 0x02 */ u16 triangle;
    /* 0x04 */ u16 circle;
    /* 0x06 */ u16 square;
    /* 0x08 */ u16 r2;
    /* 0x0A */ u16 l2;
    /* 0x0C */ u16 up;
    /* 0x0E */ u16 down;
    /* 0x10 */ u16 left;
    /* 0x12 */ u16 right;
} PadButtons;

typedef struct ControlInput {
    /* 0x00 */ u16 head;
    /* 0x02 */ u16 buttons;
    /* 0x04 */ u16 pressed;
    /* 0x06 */ u16 previousButtons;
    /* 0x08 */ u8 unk08[4];
    /* 0x0C */ u16 rawButtons;
} ControlInput;

typedef struct ShipData {
    /* 0x000 */ s16 highResTableIndex;
    /* 0x002 */ s16 lowResTableIndex;
    /* 0x004 */ u8 unk004[0x0C];
    /* 0x010 */ s32 attr;
    /* 0x014 */ u8 unk014[0x80];
    /* 0x094 */ s16 r_brake_rot;
    /* 0x096 */ s16 l_brake_rot;
    /* 0x098 */ u8 unk098[0x18];
    /* 0x0B0 */ u16 combatAttr;
    /* 0x0B2 */ u8 unk0B2[0x0C];
    /* 0x0BE */ u8 weapon;
    /* 0x0BF */ u8 unk0BF;
    /* 0x0C0 */ u8 rearFire;
    /* 0x0C1 */ u8 unk0C1[0x47];
} ShipData;

typedef struct WeaponData WeaponData;

typedef struct FrontendMenuItem {
    /* 0x00 */ s32 id;
    /* 0x04 */ char name[0x20];
    /* 0x24 */ u8 unselected;
    /* 0x25 */ u8 enabled;
    /* 0x26 */ u8 unk26[0x0A];
    /* 0x30 */ u8 value;
    /* 0x31 */ u8 unk31[0x0F];
} FrontendMenuItem;

typedef void (*CameraUpdateFn)(void);

extern PadButtons g_padButtons;
extern u16 keyDefineView[2];
extern u16 keyDefineFire[2];
extern u16 keyDefineDitch[2];
extern u16 keyDefineLBrake[2];
extern u16 keyDefineRBrake[2];
extern u16 keyDefineThrustPad[2];

extern s32 ownShip;
extern s32 ownShip2;
extern s32 ownShip3;
extern s32 ownShip4;
extern CameraUpdateFn CameraUpdate;
extern CameraUpdateFn CameraUpdate2;
extern CameraUpdateFn CameraUpdate3;
extern CameraUpdateFn CameraUpdate4;

extern ShipData *ships;
extern s16 g_weaponCharge[16];
extern s32 g_chargeSfxHandle;
extern u8 g_fireLock[4];
extern s32 g_demoShip;
extern s32 g_gameMode;
extern s32 g_superWeaponActive1;
extern s32 g_superWeaponActive2;

extern u16 g_menuPadDown;
extern u16 g_rawPadButtons;
extern u16 g_prevCheatButtons;
extern u16 g_cheatKeysPressed;
extern u16 g_cheatHistory[16];

extern s32 g_appState;
extern s32 g_frontendMenuState;
extern s16 cheatfade;
extern u8 cheatWeapon;
extern u8 cheatDamage;
extern u8 cheatTime;
extern u8 cheatSuperCombo;
extern u8 cheatTeam;
extern u8 cheatTrack;

extern FrontendMenuItem g_optionsMenu[];
extern FrontendMenuItem g_trackMenu[];
extern FrontendMenuItem g_teamMenu[];
extern u8 g_velocitarUnlocked;
extern u8 g_piranhaUnlocked;
extern u8 g_superComboOption;

void PlayNote(s32 id, s32 vol, s32 a2, s32 a3, s32 a4);
void UpdateCameraRaceExternal(void);
void UpdateCameraRaceInternal(void);
void FireWeapon(ShipData *ship, WeaponData *weapons, s32 shipNo, s32 weaponType);
void GetCheatWeapon(ShipData *shipList, s32 shipNo);
void SpawnCycloneCharge(s32 shipNo, s16 charge);
void StopSfxLoop(s32 *handle);
s32 PlaySfxLoop(s32 id, s32 vol, s32 pan, s32 a3, s32 loop);
void SetSfxPitch(s32 handle, f32 pitch);

void InitPadSettings(void);
void SetBrake(ShipData *ship, ControlInput *control);
void SetView(ShipData *ship, ControlInput *control, s32 shipNo);
void SetFire(ShipData *ship, WeaponData *weapons, s32 shipNo, ControlInput *control);
void SetDitch(ShipData *ship, WeaponData *weapons, s32 shipNo, ControlInput *control);
s16 GetMenuKey(s16 action, u16 keysPressed);
void UpdateCheatSequence(void);

#endif /* WIPEOUT64_SHIPEFX_H */
