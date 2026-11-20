#include "weapon.h"

#ident "$Revision: 1.17 $"
#ident "$Revision: 7.27 $"
#ident "$Revision: 1.5 $"

#ifdef VERSION_EU
#define SCALE_TIME(n)       (((n) * 5) / 6)
#define WEAPON_INIT_DELAY   33
#define SMARTBOMB_DELAY     20
#define AUTOPILOT_DURATION  100
#define SHIELD_DURATION     125
#define SUPER_MINE_DELAY    25
#else
#define SCALE_TIME(n)       (n)
#define WEAPON_INIT_DELAY   40
#define SMARTBOMB_DELAY     24
#define AUTOPILOT_DURATION  120
#define SHIELD_DURATION     150
#define SUPER_MINE_DELAY    30
#endif

Gfx *UpdateWeapons(ShipData *ships, WeaponShape *shapes, WeaponData *weapons, TrackCamera *camera, Gfx *gfx) {
    s32 i;
    s32 unused[2]; /* unused: retail reserves 8 bytes of stack locals */

    if (g_multiPlayer) {
        for (i = 0; i < 4; i++) {
            fadeActive[i] = 0;
            if (cheatfade > 0) {
                fadeActive[i] = 1;
            }
        }
    }
    ResetRenderList(&weaponRenderList);
    for (i = 0; i < NO_OF_WEAPONS; i++) {
        if (weapons[i].count > 0) {
            gfx = weapons[i].update(ships, shapes, &weapons[i], weapons, camera, gfx);
        }
    }
    if (g_multiPlayer) {
        for (i = 0; i < 4; i++) {
            if (fadeAmount[i] > 0) {
                fadeActive[i] = 1;
            }
            if (fadeActive[i]) {
                UpdateFadeScreen(i);
            }
        }
    }
    return FlushRenderList(&weaponRenderList, gfx);
}

void InsertWeapon(ShipData *ship, WeaponData *weapons, s32 owner, u8 type) {
    s32 i;
    s32 damage;
    s32 delay = 0;

    for (i = 0; i < NO_OF_WEAPONS; i++) {
        if (weapons[i].count == 0) {
            weaponInserted = 0;
            switch (type) {
            case 26:
            case 27:
            case 28:
            case 29:
                weapons[i].update = InitShipDebris;
                weapons[i].count = WEAPON_INIT_DELAY;
                weapons[i].owner = owner;
                weapons[i].type = type;
                return;
            case WEAPON_ROCKET:
                weapons[i].owner = owner;
                weapons[i].update = InitRocket;
                weapons[i].count = WEAPON_INIT_DELAY;
                if (IS_PLAYER(owner)) {
                    PlayNote(0x20, 0xA0, 0xF, 0xF, 0x100);
                }
                weapons[i].type = WEAPON_ROCKET;
                ship->weapon = 0;
                return;
            case 16:
                weapons[i].owner = owner;
                weapons[i].update = InitPlasma;
                weapons[i].count = WEAPON_INIT_DELAY;
                if (IS_PLAYER(owner)) {
                    PlayNote(0x20, 0xA0, 0xF, 0xF, 0x100);
                }
                ship->weapon = 0;
                return;
            case WEAPON_QUAKE:
                weapons[i].owner = owner;
                weapons[i].update = InitQuake;
                weapons[i].count = WEAPON_INIT_DELAY;
                if (IS_PLAYER(owner)) {
                    PlayNote(0x24, 0xFF, 0xF, 0xF, 0x100);
                }
                weapons[i].type = WEAPON_QUAKE;
                ship->weapon = 0;
                return;
            case WEAPON_ROCKET_LEFT:
                weapons[i].update = InitRocketLeft;
                weapons[i].count = WEAPON_INIT_DELAY;
                weapons[i].owner = owner;
                weapons[i].type = WEAPON_ROCKET;
                ship->weapon = 0;
                return;
            case WEAPON_ROCKET_RIGHT:
                weapons[i].update = InitRocketRight;
                weapons[i].count = WEAPON_INIT_DELAY;
                weapons[i].owner = owner;
                weapons[i].type = WEAPON_ROCKET;
                ship->weapon = 0;
                return;
            case WEAPON_MISSILE:
                weapons[i].owner = owner;
                weapons[i].update = InitMissile;
                weapons[i].count = WEAPON_INIT_DELAY;
                if (IS_PLAYER(owner)) {
                    PlayNote(0x20, 0xA0, 0xF, 0xF, 0x100);
                }
                weapons[i].type = WEAPON_MISSILE;
                ship->weapon = 0;
                return;
            case 18:
                weapons[i].owner = owner;
                weapons[i].update = InitMissile;
                weapons[i].count = WEAPON_INIT_DELAY;
                if (IS_PLAYER(owner)) {
                    PlayNote(0x20, 0xA0, 0xF, 0xF, 0x100);
                }
                weapons[i].type = 18;
                ship->weapon = 0;
                return;
            case WEAPON_MISSILE2:
                weapons[i].owner = owner;
                weapons[i].update = InitMissile;
                weapons[i].count = WEAPON_INIT_DELAY;
                if (IS_PLAYER(owner)) {
                    PlayNote(0x20, 0xA0, 0xF, 0xF, 0x100);
                }
                weapons[i].type = WEAPON_MISSILE;
                ship->weapon = 0;
                return;
            case 15:
                weapons[i].update = InitMinigun;
                weapons[i].count = WEAPON_INIT_DELAY;
                weapons[i].owner = owner;
                weapons[i].type = 15;
                ship->weapon = 0;
                return;
            case 20:
                weapons[i].update = InitMinigunBullet;
                weapons[i].count = WEAPON_INIT_DELAY;
                weapons[i].owner = owner;
                weapons[i].type = 20;
                return;
            case WEAPON_MINE:
                delay += 3;
                weapons[i].owner = owner;
                weapons[i].update = InitMine;
                if (!IS_PLAYER(weapons[i].owner)) {
                    weapons[i].count = delay + WEAPON_INIT_DELAY;
                } else {
                    weapons[i].count = delay;
                }
                weapons[i].type = WEAPON_MINE;
                if (delay >= 15) {
                    ship->weapon = 0;
                    return;
                }
                break;
            case 17:
                weapons[i].update = InitTrackTrap;
                weapons[i].owner = owner;
                weapons[i].count = WEAPON_INIT_DELAY;
                weapons[i].type = 17;
                ship->weapon = 0;
                if (IS_PLAYER(owner)) {
                    PlayNote(0x43, 0xA0, 0xF, 0xF, 0x100);
                }
                return;
            case 30:
                weapons[i].target = reboundTarget;
                weapons[i].update = InitMissile;
                weapons[i].owner = owner;
                weapons[i].count = 1;
                weapons[i].type = 21;
                return;
            case 21:
                weapons[i].update = UpdateReboundDelay;
                weapons[i].target = reboundTarget;
                weapons[i].owner = owner;
                weapons[i].count = SUPER_MINE_DELAY;
                weapons[i].type = 21;
                return;
            case WEAPON_SMART_BOMB:
                weapons[i].update = InitSmartBomb;
                weapons[i].count = SMARTBOMB_DELAY;
                weapons[i].owner = owner;
                weapons[i].type = WEAPON_SMART_BOMB;
                ship->weapon = 0;
                return;
            case WEAPON_ELEC_BOLT:
                weapons[i].update = InitElecBolt;
                weapons[i].count = WEAPON_INIT_DELAY;
                weapons[i].owner = owner;
                weapons[i].type = WEAPON_ELEC_BOLT;
                ship->weapon = 0;
                if (IS_PLAYER(owner)) {
                    PlayNote(0x13, 0xFF, 0xF, 0xF, 0x100);
                    PlayNote(0x20, 0xFF, 0xF, 0xF, 0xC0);
                }
                return;
            case WEAPON_AUTOPILOT:
                ship->weapon = 0;
                ship->autopilotCount = AUTOPILOT_DURATION;
                ship->update = UpdateDroneShip;
                humanToDroneSpeed(ships, owner);
                if (IS_PLAYER(owner)) {
                    PlayNote(0xC, 0x1E0, 0xF, 0xF, 0x100);
                }
                return;
            case WEAPON_ENERGY:
                damage = -150;
                if (D_80094F7E || cheatSuperCombo) {
                    if (gameType == 0 || gameType == 3) {
                        if (IS_PLAYER(owner) && frontendOptions_g.superCombo) {
                            ScaleChallengeDamage(&damage, 6);
                        }
                    } else if (gameType == 6) {
                        if ((frontendOptions_g.challengeType == 4 ||
                             (frontendOptions_g.challengeType == 3 && frontendOptions_g.challengeNo == 6)) &&
                            IS_PLAYER(owner)) {
                            ScaleChallengeDamage(&damage, 6);
                        }
                    }
                }
                shipDamage[owner] += damage;
                if (shipDamage[owner] < 0) {
                    shipDamage[owner] = 0;
                }
                ship->weapon = 0;
                if (IS_PLAYER(owner)) {
                    PlayNote(0x16, 0xFF, 0x1E, 0xF, 0x100);
                    PlayNote(0x16, 0xFF, 0, 0xF, 0x100);
                }
                return;
            case 19:
                weapons[i].update = InitCloak;
                weapons[i].count = SHIELD_DURATION;
                weapons[i].owner = owner;
                weapons[i].type = 19;
                ship->weapon = 0;
                return;
            case WEAPON_SHIELD:
                weapons[i].update = InitShield;
                weapons[i].count = SHIELD_DURATION;
                weapons[i].owner = owner;
                weapons[i].type = WEAPON_SHIELD;
                ship->weapon = 0;
                if (IS_PLAYER(owner) && shieldSfx == 0) {
                    shieldSfx = PlaySfx(2, 0, 0x80, 0, -1);
                    if (shieldSfx) {
                        SetSfxFreqOffset(shieldSfx, -4.0f);
                    }
                    shieldActive = 1;
                }
                return;
            case WEAPON_TURBO:
                ship->weapon = 0;
                ship->thrustCount = 13;
                ship->thrust = ship->maxThrust;
                if (IS_PLAYER(owner)) {
                    PlayNote(0x27, 0xC8, 0xF, 0xF, 0x100);
                }
                return;
            }
        }
    }
    if (weaponInserted) {
        weaponInserted = 0;
    }
}

void FireWeapon(ShipData *ship, WeaponData *weapons, s32 owner, u8 type) {
    if (IS_PLAYER(owner) && type == WEAPON_MISSILE && ship->rearFire && ship->rearTarget != -1) {
        ship->targetShip = ship->rearTarget;
    }
    InsertWeapon(ship, weapons, owner, type);
    if (owner == ownShip) {
        SetRumble(2, 0);
    }
    if (owner == ownShip2) {
        SetRumble(2, 1);
    }
    if (owner == ownShip3) {
        SetRumble(2, 2);
    }
    if (owner == ownShip4) {
        SetRumble(2, 3);
    }
    if (type == WEAPON_ROCKET) {
        InsertWeapon(ship, weapons, owner, WEAPON_ROCKET_LEFT);
        InsertWeapon(ship, weapons, owner, WEAPON_ROCKET_RIGHT);
    }
}

void FireSuperWeapon(ShipData *ship, WeaponData *weapons, s32 owner, u8 count) {
    s32 i;

    for (i = 0; i < count; i++) {
        InsertWeapon(ship, weapons, owner, 26);
    }
    for (i = 0; i < count; i++) {
        InsertWeapon(ship, weapons, owner, 27);
    }
    for (i = 0; i < count; i++) {
        InsertWeapon(ship, weapons, owner, 28);
    }
    for (i = 0; i < count; i++) {
        InsertWeapon(ship, weapons, owner, 29);
    }
}

void MissileLock(s32 shipNo, ShipData *ships, u8 weapon) {
    ShipData *s;
    s32 sec;
    s32 diff;
    s32 nearShip;

    s = &ships[shipNo];
    sec = g_unkAlloc9600[s->nearSecNo].distance;
    if (s->targetShip != -1) {
        diff = g_unkAlloc9600[ships[s->targetShip].nearSecNo].distance - sec;
        if (diff < 0) {
            diff += numTrackSections;
        }
        if (diff >= 40 || diff <= 0) {
            s->targetShip = -1;
        }
    }
    if (s->targetShip == -1) {
        nearShip = (s16)getShipInfrontLAP(ships, shipNo);
        if (nearShip != -1) {
            diff = g_unkAlloc9600[ships[nearShip].nearSecNo].distance - sec;
            if (diff < 0) {
                diff += numTrackSections;
            }
            if (diff < 36 && diff > 4) {
                missileLockTimer = 10;
                if (weapon != 15) {
                    PlayNote(0x19, 0x80, 0xF, 0xF, 0x100);
                }
                s->targetShip = nearShip;
            }
        }
    }
}

void RearMissileLock(s32 shipNo, ShipData *ships) {
    s32 nearShip;
    ShipData *s;
    s32 diff;
    s32 nearDistance;

    nearShip = (s16)getShipBehindLAP(ships, shipNo);
    s = &ships[shipNo];
    if (nearShip != -1) {
        nearDistance = g_unkAlloc9600[ships[nearShip].nearSecNo].distance;
        diff = g_unkAlloc9600[s->nearSecNo].distance - nearDistance;
        if (diff < 0) {
            diff += numTrackSections;
        }
        if (diff < 40 && diff > 0) {
            s->rearTarget = nearShip;
            if (shipNo == ownShip) {
                rearLockCycle[0] = (rearLockCycle[0] + 1) % 20;
            } else if (frontendOptions_g.numPlayers >= 2 && shipNo == ownShip2) {
                rearLockCycle[1] = (rearLockCycle[1] + 1) % 20;
            } else if (frontendOptions_g.numPlayers >= 3 && shipNo == ownShip3) {
                rearLockCycle[2] = (rearLockCycle[2] + 1) % 20;
            } else if (frontendOptions_g.numPlayers == 4 && shipNo == ownShip4) {
                rearLockCycle[3] = (rearLockCycle[3] + 1) % 20;
            }
        } else {
            s->rearTarget = -1;
            if (shipNo == ownShip) {
                rearLockCycle[0] = 0;
            } else if (frontendOptions_g.numPlayers >= 2 && shipNo == ownShip2) {
                rearLockCycle[1] = 0;
            } else if (frontendOptions_g.numPlayers >= 3 && shipNo == ownShip3) {
                rearLockCycle[2] = 0;
            } else if (frontendOptions_g.numPlayers == 4 && shipNo == ownShip4) {
                rearLockCycle[3] = 0;
            }
        }
    } else {
        s->rearTarget = nearShip;
        if (shipNo == ownShip) {
            rearLockCycle[0] = 0;
        } else if (frontendOptions_g.numPlayers >= 2 && shipNo == ownShip2) {
            rearLockCycle[1] = 0;
        } else if (frontendOptions_g.numPlayers >= 3 && shipNo == ownShip3) {
            rearLockCycle[2] = 0;
        } else if (frontendOptions_g.numPlayers == 4 && shipNo == ownShip4) {
            rearLockCycle[3] = 0;
        }
    }
}

void InitWeapons(WeaponShape *shapes, Skeleton *camPos, WeaponData *weapons) {
    WADFILE *wad;
    s16 textureStart;
    Object *obj;
    Object *o0;
    Object *o1;
    Object *o2;
    Object *o3;
    Object *o4;
    Object *o5;
    Object *o6;
    Object *o7;
    Object *o8;
    s32 i;

    wad = OpenWadFileNvc((u32)D_355F90);
    textureStart = g_unk8008A970;
    wad->current = 18;
    LoadCmpFiles(WADLoadCompressedTextureSequence(wad));
    InitRenderList(&weaponRenderList, textureStart, g_unk8008A970 - textureStart);
    shapes[0].shapePtr = WADLoadPrm(wad, &weaponRenderList);
    CloseWadFileNvc(wad);

    i = 0;
    obj = shapes[0].shapePtr;
    while (obj != NULL) {
        obj->skeleton->super = camPos;
        shapes[i++].shapePtr = obj;
        obj = obj->next;
    }

    o0 = shapes[5].shapePtr;
    o1 = shapes[7].shapePtr;
    o2 = shapes[6].shapePtr;
    o3 = shapes[1].shapePtr;
    o4 = shapes[8].shapePtr;
    o5 = shapes[2].shapePtr;
    o6 = shapes[0].shapePtr;
    o7 = shapes[4].shapePtr;
    o8 = shapes[3].shapePtr;

    shapes[0].shapePtr = o0;
    shapes[12].shapePtr = o1;
    shapes[32].shapePtr = o2;
    shapes[40].shapePtr = o3;
    shapes[36].shapePtr = o4;
    shapes[44].shapePtr = o5;
    shapes[48].shapePtr = o6;
    shapes[52].shapePtr = o7;
    shapes[56].shapePtr = o8;

    for (i = 1; i < 12; i++) {
        shapes[i].shapePtr = NewObjectInstance(shapes[0].shapePtr);
    }
    for (i = 13; i < 32; i++) {
        shapes[i].shapePtr = NewObjectInstance(shapes[12].shapePtr);
    }
    for (i = 33; i < 36; i++) {
        shapes[i].shapePtr = NewObjectInstance(shapes[32].shapePtr);
    }
    for (i = 41; i < 44; i++) {
        shapes[i].shapePtr = NewObjectInstance(shapes[40].shapePtr);
    }
    for (i = 37; i < 40; i++) {
        shapes[i].shapePtr = NewObjectInstance(shapes[36].shapePtr);
    }
    for (i = 45; i < 48; i++) {
        shapes[i].shapePtr = NewObjectInstance(shapes[44].shapePtr);
    }
    for (i = 49; i < 52; i++) {
        shapes[i].shapePtr = NewObjectInstance(shapes[48].shapePtr);
    }
    for (i = 53; i < 56; i++) {
        shapes[i].shapePtr = NewObjectInstance(shapes[52].shapePtr);
    }
    for (i = 57; i < 60; i++) {
        shapes[i].shapePtr = NewObjectInstance(shapes[56].shapePtr);
    }

    for (i = 0; i < NO_OF_SHAPES; i++) {
        shapes[i].used = 0;
        obj = shapes[i].shapePtr;
        while (obj != NULL) {
            obj->skeleton->super = camPos;
            obj = obj->next;
        }
    }
    for (i = 0; i < NO_OF_WEAPONS; i++) {
        weapons[i].count = 0;
        weapons[i].type = 0;
    }
    g_startupImage->current += 6;
}

void BumpUp(TrackSection *bump) {
    Face *face;

    if (bump != NULL && bump->bump < 8) {
        face = &track->faces[bump->faceStart];
        while (!(face->flags & TRACK_BASE)) {
            face++;
        }
        if (shipClassIndex >= 2) {
            track->vertices[face->vertex[0]].vy -= 400;
            track->vertices[face->vertex[1]].vy -= 400;
            track->vertices[(face + 1)->vertex[0]].vy -= 400;
        } else {
            track->vertices[face->vertex[0]].vy -= 200;
            track->vertices[face->vertex[1]].vy -= 200;
            track->vertices[(face + 1)->vertex[0]].vy -= 200;
        }
        bump->bump++;
    }
}

void BumpDown(TrackSection *bump) {
    Face *face;

    if (bump != NULL && bump->bump > 0) {
        face = &track->faces[bump->faceStart];
        while (!(face->flags & TRACK_BASE)) {
            face++;
        }
        if (shipClassIndex >= 2) {
            track->vertices[face->vertex[0]].vy += 400;
            track->vertices[face->vertex[1]].vy += 400;
            track->vertices[(face + 1)->vertex[0]].vy += 400;
        } else {
            track->vertices[face->vertex[0]].vy += 200;
            track->vertices[face->vertex[1]].vy += 200;
            track->vertices[(face + 1)->vertex[0]].vy += 200;
        }
        bump->bump--;
    }
}

void BumpFlatten(TrackSection *bump) {
    Face *face;
    Face *baseFace;

    if (bump != NULL && bump->bump > 0) {
        face = &track->faces[bump->faceStart];
        while (!(face->flags & TRACK_BASE)) {
            face++;
        }
        baseFace = face;
        while (bump->bump > 0) {
            face = baseFace;
            if (shipClassIndex >= 2) {
                track->vertices[face->vertex[0]].vy += 400;
                track->vertices[face->vertex[1]].vy += 400;
                face++;
                track->vertices[face->vertex[0]].vy += 400;
            } else {
                track->vertices[face->vertex[0]].vy += 200;
                track->vertices[face->vertex[1]].vy += 200;
                face++;
                track->vertices[face->vertex[0]].vy += 200;
            }
            bump->bump--;
        }
    }
}

void FlattenTrack(void) {
    s32 i;
    TrackSection *section = track->sections;

    for (i = 0; i < track->sectionCount; i++, section++) {
        BumpFlatten(section);
    }
}

void InitialiseBumps(void) {
    TrackSection *section = track->sections;
    s32 i;
    s32 sectionNo;

    for (i = 0; i < track->sectionCount; i++, section++) {
        section->bump = 0;
        sectionNo = section->secNo;
        if ((gameData.trackNo == 2 &&
             ((sectionNo > 102 && sectionNo < 158) ||
              (sectionNo > 190 && sectionNo < 221) ||
              (sectionNo > 269 && sectionNo < 386))) ||
            (gameData.trackNo == 1 &&
             ((sectionNo >= 0 && sectionNo < 73) ||
              (sectionNo > 86 && sectionNo < 104) ||
              (sectionNo > 184 && sectionNo < 226) ||
              (sectionNo > 133 && sectionNo < 173))) ||
            (gameData.trackNo == 8 &&
             ((sectionNo > 34 && sectionNo < 65) ||
              (sectionNo > 152 && sectionNo < 171) ||
              (sectionNo > 250 && sectionNo < 267) ||
              (sectionNo > 282 && sectionNo < 295))) ||
            (gameData.trackNo == 13 &&
             ((sectionNo > 124 && sectionNo < 216) ||
              (sectionNo > 260 && sectionNo < 281) ||
              (sectionNo > 298 && sectionNo < 343) ||
              (sectionNo > 412 && sectionNo < 432))) ||
            (gameData.trackNo == 17 &&
             ((sectionNo > 14 && sectionNo < 32) ||
              (sectionNo > 73 && sectionNo < 106) ||
              (sectionNo > 129 && sectionNo < 168) ||
              (sectionNo > 229 && sectionNo < 274))) ||
            (gameData.trackNo == 6 &&
             ((sectionNo > 18 && sectionNo < 25) ||
              (sectionNo > 194 && sectionNo < 268) ||
              (sectionNo > 357 && sectionNo < 428))) ||
            (gameData.trackNo == 20 &&
             ((sectionNo > 212 && sectionNo < 223) ||
              (sectionNo > 230 && sectionNo < 257) ||
              (sectionNo > 303 && sectionNo < 334))) ||
            (gameData.trackNo == 7 &&
             ((sectionNo > 26 && sectionNo < 38) ||
              (sectionNo > 94 && sectionNo < 103) ||
              (sectionNo > 108 && sectionNo < 143) ||
              (sectionNo > 170 && sectionNo < 231) ||
              (sectionNo > 323 && sectionNo < 341) ||
              (sectionNo > 73 && sectionNo < 84)))) {
            section->snow = 1;
        } else {
            section->snow = 0;
        }
    }
}

/* Ship hit eligibility check shared across weapon update loops (handles 2-4P versus filtering). */
#define CAN_HIT_SHIP(i, ship, weapon)                                                                     \
    (!((ship)->combatAttr & 0x8000) &&                                                                    \
     (((i) != (weapon)->owner && gameType != 3) ||                                                        \
      (gameType == 3 && frontendOptions_g.numPlayers == 2 && ((i) == ownShip || (i) == ownShip2) &&       \
       (i) != (weapon)->owner) ||                                                                         \
      (gameType == 3 && frontendOptions_g.numPlayers == 3 &&                                              \
       ((i) == ownShip || (i) == ownShip2 || (i) == ownShip3) && (i) != (weapon)->owner) ||               \
      (gameType == 3 && frontendOptions_g.numPlayers == 4 &&                                              \
       ((i) == ownShip || (i) == ownShip2 || (i) == ownShip3 || (i) == ownShip4) &&                       \
       (i) != (weapon)->owner) ||                                                                         \
      (gameType == 3 && frontendOptions_g.numPlayers == 2 && (i) != (weapon)->owner) ||                   \
      (gameType == 3 && frontendOptions_g.numPlayers == 3 &&                                              \
       ((i) == ownShip || (i) == ownShip2 || (i) == ownShip3 || (ship)->unkF4 != 0) &&                    \
       (i) != (weapon)->owner)))

/* PAL (50 Hz) vs NTSC (60 Hz) weapon timers and acceleration scaling.
 * PAL_SCALE_ACCEL intentionally expands to bare statements (no do/while(0)) so GCC 2.7.2 schedules across it identically. */
#ifdef VERSION_EU
#define PAL_SCALE_ACCEL(w)                       \
    (w)->accel.vx = ((w)->accel.vx * 60) / 50;   \
    (w)->accel.vy = ((w)->accel.vy * 60) / 50;   \
    (w)->accel.vz = ((w)->accel.vz * 60) / 50
#define MISSILE_LIFE        150
#define MINIGUN_LIFE        125
#define BULLET_LIFE         25
#define QUAKE_LIFE          60
#define PLASMA_LIFE         150
#define PLASMA_CHARGE_HIGH  50
#define PLASMA_CHARGE_MID   25
#define ROCKET_LIFE         140
#define ELEC_BOLT_LIFE      100
#define MINE_LIFE           350
#define TRACK_TRAP_LIFE     200
#define SMART_BOMB_LIFE     20
#define DEATH_DELAY_1P      50
#define DEATH_DELAY_MP      5
#else
#define PAL_SCALE_ACCEL(w)
#define MISSILE_LIFE        200
#define MINIGUN_LIFE        150
#define BULLET_LIFE         30
#define QUAKE_LIFE          70
#define PLASMA_LIFE         180
#define PLASMA_CHARGE_HIGH  60
#define PLASMA_CHARGE_MID   30
#define ROCKET_LIFE         200
#define ELEC_BOLT_LIFE      140
#define MINE_LIFE           450
#define TRACK_TRAP_LIFE     240
#define SMART_BOMB_LIFE     24
#define DEATH_DELAY_1P      60
#define DEATH_DELAY_MP      6
#endif

Gfx *UpdateQuake(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons,
                 TrackCamera *camera, Gfx *gfx) {
    TrackSection *nextSection;
    TrackSection *section;
    Face *facePtr;
    Face *nextFace;
    VECTOR unused0; /* unused: retail reserves 0x10 bytes before facePos */
    VECTOR facePos;
    Vector nextCentre;
    VECTOR unused1[4]; /* unused: retail reserves 0x40-byte stack area */
    s32 i;
    s32 j;
    s32 shipNo;
    s32 bumpCount;
    s32 sectionCount;
    s32 playerShip;
    s32 playerIdx;
    TrackSection *bumpSection;
    TrackSection *junction;

    if (g_multiPlayer) {
        weapon->section = FindNearestSection(weapon->section, weapon->pos);
        nextSection = weapon->section->nextSection.ptr;
        facePtr = track->faces + weapon->section->faceStart;
        nextFace = track->faces + nextSection->faceStart;
        while (!(facePtr->flags & TRACK_BASE)) {
            facePtr++;
        }
        while (!(nextFace->flags & TRACK_BASE)) {
            nextFace++;
        }
        GetFaceVertex(weapon->section, facePtr, 0, &facePos);

        section = weapon->section;
        for (shipNo = 0; shipNo < NO_OF_SHIPS; shipNo++) {
            if (section == ships[shipNo].nearTrkSect && shipNo != weapon->owner &&
                !(ships[shipNo].combatAttr & 0x8000)) {
                if (!IS_PLAYER(shipNo)) {
                    ships[shipNo].speed >>= 2;
                }
                ships[shipNo].vpivot.vx >>= 2;
                ships[shipNo].vpivot.vy >>= 2;
                ships[shipNo].vpivot.vz >>= 2;
                if (shipNo == ownShip) {
                    SetShake(8, 0);
                    SetRumble(15, 0);
                }
                if (shipNo == ownShip2) {
                    SetShake(8, 1);
                    SetRumble(15, 1);
                }
                if (shipNo == ownShip3) {
                    SetShake(8, 2);
                    SetRumble(15, 2);
                }
                if (shipNo == ownShip4) {
                    SetShake(8, 3);
                    SetRumble(15, 3);
                }
                PlayNote(0xA, 0xFF, 0xF, 0xF, 0x100);
                AddExplode(ships[shipNo].ppivot.vx + (newRand() & 0x3FF) - 0x200,
                           ships[shipNo].ppivot.vy - (newRand() & 0x1FF),
                           ships[shipNo].ppivot.vz + (newRand() & 0x3FF) - 0x200, -1);
                DamageCollide(12, shipNo, weapon);
            }
        }

        for (bumpCount = 0; bumpCount < weapon->bumpStep; bumpCount++) {
            BumpUp(section);
        }

        section = weapon->section;
        for (sectionCount = 0; sectionCount < 20; sectionCount++) {
            junction = section->junction.ptr;
            if (junction != (TrackSection *)-1) {
                bumpSection = junction;
                if (junction->flags & 8) {
                    for (j = 0; j < 10; j++) {
                        BumpDown(bumpSection);
                        bumpSection = bumpSection->prevSection.ptr;
                    }
                }
            }
            section = section->prevSection.ptr;
            BumpDown(section);
        }

        nextCentre.vx = nextSection->centre.vx;
        nextCentre.vy = nextSection->centre.vy;
        nextCentre.vz = nextSection->centre.vz;
        weapon->count--;

        playerShip = ownShip;
        for (playerIdx = 1; playerIdx <= frontendOptions_g.numPlayers; playerIdx++) {
            if (ships[playerShip].nearTrkSect->bump > 0 && playerShip == weapon->owner) {
                BumpFlatten(ships[playerShip].nearTrkSect);
                BumpFlatten(ships[playerShip].nearTrkSect->nextSection.ptr);
                BumpFlatten(ships[playerShip].nearTrkSect->prevSection.ptr);
            }
            if (playerIdx == 1) {
                playerShip = ownShip2;
            } else if (playerIdx == 2) {
                playerShip = ownShip3;
            } else if (playerIdx == 3) {
                playerShip = ownShip4;
            }
        }

        if (shipClassIndex >= 2 || (weapon->count & 1)) {
            weapon->pos.vx = nextSection->centre.vx;
            weapon->pos.vy = nextSection->centre.vy;
            weapon->pos.vz = nextSection->centre.vz;
            if (ships[weapon->owner].nearTrkSect == nextSection ||
                ships[weapon->owner].nearTrkSect == nextSection->prevSection.ptr ||
                ships[weapon->owner].nearTrkSect == nextSection->prevSection.ptr->prevSection.ptr) {
                weapon->pos.vx = nextSection->nextSection.ptr->centre.vx;
                weapon->pos.vy = nextSection->nextSection.ptr->centre.vy;
                weapon->pos.vz = nextSection->nextSection.ptr->centre.vz;
            }
            if (weapon->bumpStep < 8 && weapon->count > 8) {
                weapon->bumpStep += 2;
            }
            if (weapon->bumpStep > 0 && weapon->count <= 8) {
                weapon->bumpStep--;
            }
        }

        if (weapon->count == 0) {
            section = weapon->section;
            for (i = 0; i < 20; i++) {
                junction = section->junction.ptr;
                if (junction != (TrackSection *)-1) {
                    bumpSection = junction;
                    if (junction->flags & 8) {
                        for (j = 0; j < 20; j++) {
                            BumpFlatten(bumpSection);
                            bumpSection = bumpSection->prevSection.ptr;
                        }
                    }
                }
                BumpFlatten(section);
                section = section->prevSection.ptr;
            }
        }
    }
    return gfx;
}

Gfx *HitReboundMissile(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons,
                       TrackCamera *camera, s32 shipNo, Gfx *gfx) {
    if (IS_PLAYER(shipNo)) {
        if (shipNo == ownShip) {
            SetShake(20, 0);
            SetRumble(8, 0);
        }
        if (shipNo == ownShip2) {
            SetShake(20, 1);
            SetRumble(8, 1);
        }
        if (shipNo == ownShip3) {
            SetShake(20, 2);
            SetRumble(8, 2);
        }
        if (shipNo == ownShip4) {
            SetShake(20, 3);
            SetRumble(8, 3);
        }
    }
    DamageCollide(18, shipNo, weapon);
    weapon->count = -1;
    weapon->target = shipNo;
    shapes[weapon->shapeNo].used = 0;
    PlayNote(0x45, 0xFF, 0xF, 0xF, 0x100);
    reboundTarget = weapon->owner;
    InsertWeapon(ships, weapons, shipNo, 21);
    return gfx;
}

Gfx *UpdateReboundDelay(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons,
                        TrackCamera *camera, Gfx *gfx) {
    if (weapon->count >= 2) {
        if (g_multiPlayer) {
            weapon->count--;
        }
    } else {
        weapon->count = 0;
        reboundTarget = weapon->target;
        InsertWeapon(ships, weapons, weapon->owner, 30);
    }
    return gfx;
}

Gfx *UpdateMissile(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons,
                   TrackCamera *camera, Gfx *gfx) {
    VECTOR dist;
    VECTOR targetVector;
    VECTOR unusedVec; /* unused: retail reserves 0x10-byte stack slot */
    VECTOR facePoint;
    VECTOR nextPoint;
    VECTOR path;
    VECTOR offset;
    s32 horizDist;
    s32 vhdg;
    s32 vpitch;
    Face *facePtr;
    Face *nextFace;
    TrackSection *nextSection;
    s32 mag;
    s32 alpha;
    s32 height;
    s32 i;
    ShipData *ship;

    if (weapon->target == -1) {
        return UpdateRocket(ships, shapes, weapon, weapons, camera, gfx);
    }
    if (g_multiPlayer) {
        targetVector.vx = (ships[weapon->target].ppivot.vx - weapon->pos.vx) >> 3;
        targetVector.vy = (ships[weapon->target].ppivot.vy - weapon->pos.vy) >> 3;
        targetVector.vz = (ships[weapon->target].ppivot.vz - weapon->pos.vz) >> 3;
        horizDist = SquareRoot0(targetVector.vx * targetVector.vx + targetVector.vz * targetVector.vz);
        vhdg = -ratan2(targetVector.vx, targetVector.vz) - weapon->hdg;
        vpitch = -ratan2(targetVector.vy, horizDist) - weapon->pitch;
        if (g_multiPlayer) {
            weapon->section = FindNearestSection(weapon->section, weapon->pos);
            if (vhdg > 2048) {
                vhdg -= 4096;
            } else if (vhdg < -2048) {
                vhdg += 4096;
            }
            if (vpitch > 2048) {
                vpitch -= 4096;
            } else if (vpitch < -2048) {
                vpitch += 4096;
            }
            weapon->hdg += vhdg >> 1;
            weapon->pitch += vpitch >> 1;
            weapon->hdg = ang(weapon->hdg);
            weapon->pitch = ang(weapon->pitch);

            weapon->accel.vx = (-rsin(weapon->hdg) * rcos(weapon->pitch)) >> 16;
            weapon->accel.vy = -rsin(weapon->pitch) >> 4;
            weapon->accel.vz = (rcos(weapon->hdg) * rcos(weapon->pitch)) >> 16;
            PAL_SCALE_ACCEL(weapon);
            weapon->velocity.vx += weapon->accel.vx;
            weapon->velocity.vy += weapon->accel.vy;
            weapon->velocity.vz += weapon->accel.vz;
            if (shipClassIndex >= 2) {
                weapon->velocity.vx -= weapon->velocity.vx / 6;
                weapon->velocity.vy -= weapon->velocity.vy / 6;
                weapon->velocity.vz -= weapon->velocity.vz / 6;
            } else {
                weapon->velocity.vx -= weapon->velocity.vx >> 2;
                weapon->velocity.vy -= weapon->velocity.vy >> 2;
                weapon->velocity.vz -= weapon->velocity.vz >> 2;
            }
            weapon->pos.vx += weapon->velocity.vx;
            weapon->pos.vy += weapon->velocity.vy;
            weapon->pos.vz += weapon->velocity.vz;

            facePtr = track->faces + weapon->section->faceStart;
            while (!(facePtr->flags & TRACK_BASE)) {
                facePtr++;
            }
            nextSection = weapon->section->nextSection.ptr;
            nextFace = track->faces + nextSection->faceStart;
            while (!(nextFace->flags & TRACK_BASE)) {
                nextFace++;
            }
            GetFaceVertex(weapon->section, facePtr, 0, &facePoint);
            GetFaceVertex(nextSection, nextFace, 0, &nextPoint);
            path.vx = nextPoint.vx - facePoint.vx;
            path.vy = nextPoint.vy - facePoint.vy;
            path.vz = nextPoint.vz - facePoint.vz;
            offset.vx = weapon->pos.vx - facePoint.vx;
            offset.vz = weapon->pos.vz - facePoint.vz;
            mag = SquareRoot0(path.vx * path.vx + path.vz * path.vz);
            alpha = (((path.vx * offset.vx + path.vz * offset.vz) / mag) << 16) / mag;
            height = nextPoint.vy - facePoint.vy;
            height = (height * alpha) >> 16;
            if (weapon->pos.vy < (facePoint.vy + height) - 300) {
                weapon->pos.vy = (facePoint.vy + height) - 300;
            }

            weapon->count--;
            if (weapon->count == 0) {
                shapes[weapon->shapeNo].used = 0;
            }
            SetSkeletonPosition(shapes[weapon->shapeNo].shapePtr->skeleton,
                                weapon->pos.vx >> 1, weapon->pos.vy >> 1, weapon->pos.vz >> 1);
            SetSkeletonDirectionHPR(shapes[weapon->shapeNo].shapePtr->skeleton, weapon->hdg, weapon->pitch, 0);
        }
    }

    shapes[weapon->shapeNo].shapePtr->skeleton->update = 1;
    shapes[weapon->shapeNo].shapePtr->skeleton->super = camera->camPos;
    MatrixToN64Mtx(GetSkeletonMatrix(shapes[weapon->shapeNo].shapePtr->skeleton), (N64Mtx *)&g_mtxPool[g_mtxIndex]);
    AddRenderGroup(&weaponRenderList, shapes[weapon->shapeNo].shapePtr->group, &g_mtxPool[g_mtxIndex]);
    g_mtxIndex++;

    if (g_multiPlayer) {
        for (i = 0; i < NO_OF_SHIPS; i++) {
            ship = &ships[i];
            if (CAN_HIT_SHIP(i, ship, weapon)) {
                dist.vx = (ship->ppivot.vx - weapon->pos.vx) >> 3;
                dist.vy = (ship->ppivot.vy - weapon->pos.vy) >> 3;
                dist.vz = (ship->ppivot.vz - weapon->pos.vz) >> 3;
                if (GetMagnitude(dist) < 100 && !(ship->attr & 0x4000) && !(ship->combatAttr & 0x100)) {
                    if (weapon->type == 18) {
                        return HitReboundMissile(ships, shapes, weapon, weapons, camera, i, gfx);
                    } else if (weapon->type == 21) {
                        DamageCollide(21, i, weapon);
                        weapon->count = -1;
                        weapon->target = i;
                        shapes[weapon->shapeNo].used = 0;
                        PlayNote(0x16, 0xFF, 0xF, 0xF, 0x100);
                    } else {
                        if (!IS_PLAYER(i)) {
                            FlipShip(ship);
                            ship->speed >>= 5;
                            ship->vroll += 5000;
                            ship->ahdg = (newRand() - 16384) >> 1;
                        } else {
                            ship->vpivot.vx -= (ship->vpivot.vx * 3) >> 2;
                            ship->vpivot.vy -= (ship->vpivot.vy * 3) >> 2;
                            ship->vpivot.vz -= (ship->vpivot.vz * 3) >> 2;
                            ship->vroll += newRand() % 250;
                            ship->ahdg = (newRand() - 16384) >> 1;
                            if (i == ownShip) {
                                SetShake(20, 0);
                                SetRumble(8, 0);
                            }
                            if (i == ownShip2) {
                                SetShake(20, 1);
                                SetRumble(8, 1);
                            }
                            if (i == ownShip3) {
                                SetShake(20, 2);
                                SetRumble(8, 2);
                            }
                            if (i == ownShip4) {
                                SetShake(20, 3);
                                SetRumble(8, 3);
                            }
                        }
                        DamageCollide(2, i, weapon);
                        weapon->count = -1;
                        weapon->target = i;
                        shapes[weapon->shapeNo].used = 0;
                        PlayNote(0xA, 0xFF, 0xF, 0xF, 0x100);
                    }
                    return gfx;
                }
            }
        }

        facePtr = track->faces + weapon->section->faceStart;
        for (i = 0; i < weapon->section->faceCount; i++) {
            GetFaceVertex(weapon->section, facePtr, 0, &facePoint);
            if (PlaneLineCollision(facePoint, weapon->pos, facePtr->normal) <= 0) {
                if (!(facePtr->flags & TRACK_BASE)) {
                    weapon->count = -2;
                    shapes[weapon->shapeNo].used = 0;
                    PlayNote(0xB, 0xA0, 0xF, 0xF, 0x100);
                    return gfx;
                }
            }
            facePtr++;
        }
        Weapon2Mine(weapon, weapons, shapes, 0);
    }
    return gfx;
}

Gfx *InitMissile(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons,
                 TrackCamera *camera, Gfx *gfx) {
    s32 i;
    Face *facePtr;

    if (!IS_PLAYER(weapon->owner)) {
        if (weapon->count > 1) {
            if (g_multiPlayer) {
                weapon->count--;
            }
            return gfx;
        }
        weapon->count = MISSILE_LIFE;
    } else {
        weapon->count = MISSILE_LIFE;
    }
    weapon->section = ships[weapon->owner].nearTrkSect;
    for (i = 0; i < 4; i++) {
        if (shapes[i + 32].used == 0) {
            weapon->shapeNo = i + 32;
            shapes[i + 32].used = 1;
            if (weapon->type != 21) {
                weapon->target = ships[weapon->owner].targetShip;
            } else {
                weapon->target = reboundTarget;
            }
            facePtr = track->faces + ships[weapon->owner].nearTrkSect->faceStart;
            while (!(facePtr->flags & TRACK_BASE)) {
                facePtr++;
            }
            GetFireVector(&weapon->accel, ships[weapon->owner].unitVecNose, facePtr,
                          ships[weapon->owner].ppivot, ships[weapon->owner].nearTrkSect);
            PAL_SCALE_ACCEL(weapon);
            weapon->velocity.vx = ships[weapon->owner].vpivot.vx >> 6;
            weapon->velocity.vy = ships[weapon->owner].vpivot.vy >> 6;
            weapon->velocity.vz = ships[weapon->owner].vpivot.vz >> 6;
            weapon->pos.vx = ships[weapon->owner].ppivot.vx;
            weapon->pos.vy = ships[weapon->owner].ppivot.vy;
            weapon->pos.vz = ships[weapon->owner].ppivot.vz;
            SetSkeletonDirectionHPR(shapes[weapon->shapeNo].shapePtr->skeleton,
                                    ships[weapon->owner].hdg, ships[weapon->owner].pitch, ships[weapon->owner].roll);
            weapon->hdg = ships[weapon->owner].hdg;
            weapon->pitch = ships[weapon->owner].pitch;
            weapon->update = UpdateMissile;
            ships[weapon->owner].targetShip = -1;
            return gfx;
        }
    }
    weapon->count = 0;
    return gfx;
}

Gfx *UpdateMinigunBullet(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons,
                         TrackCamera *camera, Gfx *gfx) {
    VECTOR dist;
    VECTOR targetVector;
    VECTOR unusedVec; /* unused: retail reserves 0x10-byte stack slot */
    VECTOR facePoint;
    VECTOR zeroVel;
    s32 horizDist;
    s32 vpitch;
    s32 i;
    s32 prevHits;
    ShipData *ship;
    Face *facePtr;

    if (g_multiPlayer) {
        if (weapon->target != -1) {
            targetVector.vx = (ships[weapon->target].ppivot.vx - weapon->pos.vx) >> 3;
            targetVector.vy = (ships[weapon->target].ppivot.vy - weapon->pos.vy) >> 3;
            targetVector.vz = (ships[weapon->target].ppivot.vz - weapon->pos.vz) >> 3;
            horizDist = SquareRoot0(targetVector.vx * targetVector.vx + targetVector.vz * targetVector.vz);
            vpitch = -ratan2(targetVector.vy, horizDist) - weapon->pitch;
            if (vpitch > 2048) {
                vpitch -= 4096;
            } else if (vpitch < -2048) {
                vpitch += 4096;
            }
            weapon->pitch += vpitch >> 1;
            weapon->pitch = ang(weapon->pitch);

            weapon->accel.vx = (-rsin(weapon->hdg) * rcos(weapon->pitch)) >> 16;
            weapon->accel.vy = -rsin(weapon->pitch) >> 4;
            weapon->accel.vz = (rcos(weapon->hdg) * rcos(weapon->pitch)) >> 16;
            PAL_SCALE_ACCEL(weapon);
            weapon->velocity.vx += weapon->accel.vx;
            weapon->velocity.vy += weapon->accel.vy;
            weapon->velocity.vz += weapon->accel.vz;
            weapon->velocity.vx -= weapon->velocity.vx >> 2;
            weapon->velocity.vy -= weapon->velocity.vy >> 2;
            weapon->velocity.vz -= weapon->velocity.vz >> 2;
        } else {
            weapon->velocity.vx += weapon->accel.vx;
            weapon->velocity.vy += weapon->accel.vy;
            weapon->velocity.vz += weapon->accel.vz;
            weapon->velocity.vx -= weapon->velocity.vx >> 5;
            weapon->velocity.vy -= weapon->velocity.vy >> 5;
            weapon->velocity.vz -= weapon->velocity.vz >> 5;
        }

        weapon->pos.vx += weapon->velocity.vx;
        weapon->pos.vy += weapon->velocity.vy;
        weapon->pos.vz += weapon->velocity.vz;
        weapon->count--;

        for (i = 0; i < NO_OF_SHIPS; i++) {
            ship = &ships[i];
            if (CAN_HIT_SHIP(i, ship, weapon)) {
                dist.vx = (ship->ppivot.vx - weapon->pos.vx) >> 3;
                dist.vy = (ship->ppivot.vy - weapon->pos.vy) >> 3;
                dist.vz = (ship->ppivot.vz - weapon->pos.vz) >> 3;
                if (GetMagnitude(dist) < 100) {
                    if (!(ship->attr & 0x4000) && !(ship->combatAttr & 0x100)) {
                        prevHits = ships[weapon->owner].unkF0;
                        DamageCollide(15, i, weapon);
                        if (i == ownShip) {
                            SetRumble(8, 0);
                            SetShake(20, 0);
                        } else if (i == ownShip2) {
                            SetRumble(8, 1);
                            SetShake(20, 1);
                        } else if (i == ownShip3) {
                            SetRumble(8, 2);
                            SetShake(20, 2);
                        } else if (i == ownShip4) {
                            SetRumble(8, 3);
                            SetShake(20, 3);
                        }
                        if (prevHits < ships[weapon->owner].unkF0) {
                            PlayNote(0xA, 0xFF, 0xF, 0xF, 0x100);
                            if (i == weapon->target) {
                                ships[weapon->owner].targetShip = -1;
                            }
                        }
                    }
                    weapon->count = -1;
                    shapes[weapon->shapeNo].used = 0;
                    weapon->pos.vx -= weapon->velocity.vx;
                    weapon->pos.vy -= weapon->velocity.vy;
                    weapon->pos.vz -= weapon->velocity.vz;
                    AddRandomParticles(particleSlots, 8, &weapon->pos, &weapon->velocity);
                    weapon->target = i;
                    return gfx;
                }
            }
        }

        weapon->section = FindNearestSection(weapon->section, weapon->pos);
        facePtr = track->faces + weapon->section->faceStart;
        while (!(facePtr->flags & TRACK_BASE)) {
            facePtr++;
        }
        for (i = 0; i < weapon->section->faceCount; i++) {
            GetFaceVertex(weapon->section, facePtr, 0, &facePoint);
            if (PlaneLineCollision(facePoint, weapon->pos, facePtr->normal) <= 0) {
                weapon->count = -2;
                PlayNote(0xB, 0xA0, 0xF, 0xF, 0x100);
                weapon->pos.vx -= weapon->velocity.vx;
                weapon->pos.vy -= weapon->velocity.vy;
                weapon->pos.vz -= weapon->velocity.vz;
                zeroVel.vx = 0;
                zeroVel.vy = 0;
                zeroVel.vz = 0;
                AddRandomParticles(particleSlots, 8, &weapon->pos, &zeroVel);
                return gfx;
            }
            facePtr++;
        }
        Weapon2Mine(weapon, weapons, shapes, 0);
    }
    return gfx;
}

Gfx *UpdateMinigun(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons,
                   TrackCamera *camera, Gfx *gfx) {
    if (g_multiPlayer) {
        weapon->count--;
        if (weapon->count != 0) {
            InsertWeapon(ships, weapons, weapon->owner, 20);
        } else {
            ships[weapon->owner].targetShip = -1;
            if (weapon->owner == ownShip) {
                g_fireLock[0] = 0;
            } else if (weapon->owner == ownShip2) {
                g_fireLock[1] = 0;
            } else if (weapon->owner == ownShip3) {
                g_fireLock[2] = 0;
            } else if (weapon->owner == ownShip4) {
                g_fireLock[3] = 0;
            }
        }
    }
    return gfx;
}

Gfx *InitMinigun(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons,
                 TrackCamera *camera, Gfx *gfx) {
    if (!IS_PLAYER(weapon->owner)) {
        if (weapon->count > 1) {
            if (g_multiPlayer) {
                weapon->count--;
            }
            return gfx;
        }
        weapon->count = MINIGUN_LIFE;
    } else {
        weapon->count = MINIGUN_LIFE;
        if (weapon->owner == ownShip) {
            g_fireLock[0] = 1;
        } else if (weapon->owner == ownShip2) {
            g_fireLock[1] = 1;
        } else if (weapon->owner == ownShip3) {
            g_fireLock[2] = 1;
        } else if (weapon->owner == ownShip4) {
            g_fireLock[3] = 1;
        }
    }
    weapon->update = UpdateMinigun;
    return gfx;
}

Gfx *InitMinigunBullet(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons,
                       TrackCamera *camera, Gfx *gfx) {
    Face *facePtr;
    s32 slot;

    if (!IS_PLAYER(weapon->owner)) {
        if (weapon->count > 1) {
            if (g_multiPlayer) {
                weapon->count--;
            }
            return gfx;
        }
        weapon->count = BULLET_LIFE;
    } else {
        weapon->count = BULLET_LIFE;
    }

    minigunFrame++;
    if (minigunFrame & 1) {
        weapon->count = 0;
        return gfx;
    }

    superWeaponFlag = weapon->owner;
    weapon->section = ships[weapon->owner].nearTrkSect;
    facePtr = track->faces + ships[weapon->owner].nearTrkSect->faceStart;
    while (!(facePtr->flags & TRACK_BASE)) {
        facePtr++;
    }
    weapon->target = ships[weapon->owner].targetShip;
    weapon->accel.vx = ships[weapon->owner].unitVecNose.vx >> 6;
    weapon->accel.vy = ships[weapon->owner].unitVecNose.vy >> 6;
    weapon->accel.vz = ships[weapon->owner].unitVecNose.vz >> 6;
    PAL_SCALE_ACCEL(weapon);
    weapon->velocity.vx = ships[weapon->owner].vpivot.vx >> 6;
    weapon->velocity.vy = ships[weapon->owner].vpivot.vy >> 6;
    weapon->velocity.vz = ships[weapon->owner].vpivot.vz >> 6;
    weapon->velocity.vx += weapon->accel.vx * 8;
    weapon->velocity.vy += weapon->accel.vy * 8;
    weapon->velocity.vz += weapon->accel.vz * 8;
    weapon->pos.vx = ships[weapon->owner].ppivot.vx;
    weapon->pos.vy = ships[weapon->owner].ppivot.vy;
    weapon->pos.vz = ships[weapon->owner].ppivot.vz;
    weapon->hdg = ships[weapon->owner].hdg;
    weapon->pitch = ships[weapon->owner].pitch;
    weapon->update = UpdateMinigunBullet;

    slot = nextParticleSlot;
    nextParticleSlot = (nextParticleSlot + 1) % 256;
    particleSlots[slot].pos.vx = weapon->pos.vx;
    particleSlots[slot].pos.vy = weapon->pos.vy;
    particleSlots[slot].pos.vz = weapon->pos.vz;
    particleSlots[slot].vel.vx = weapon->velocity.vx;
    particleSlots[slot].vel.vy = weapon->velocity.vy;
    particleSlots[slot].vel.vz = weapon->velocity.vz;
    particleSlots[slot].life = 15;
    if (!(minigunFrame & 7)) {
        particleSlots[slot].type = 2;
    } else {
        particleSlots[slot].type = 3;
    }
    particleSlots[slot].flags = 0;

    if (IS_PLAYER(weapon->owner)) {
        SetSfxFreqOffset(PlaySfx(0xA, 0xFF, 0x80, 1, -1), 1.0f);
    }
    return gfx;
}

Gfx *UpdateMine(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons,
                TrackCamera *camera, Gfx *gfx) {
    VECTOR dist;
    s32 i;
    ShipData *ship;

    if (g_multiPlayer) {
        weapon->count--;
        if (weapon->count == 0) {
            shapes[weapon->shapeNo].used = 0;
            return gfx;
        }
        weapon->pos.vx += sar(rsin((s16)(weapon->shapeNo * 400 + weapon->count * 40)), 9);
        weapon->hdg += 128;
        SetSkeletonDirectionHPR(shapes[weapon->shapeNo].shapePtr->skeleton, weapon->hdg, 0, 0);
        SetSkeletonPosition(shapes[weapon->shapeNo].shapePtr->skeleton,
                            weapon->pos.vx >> 1, weapon->pos.vy >> 1, weapon->pos.vz >> 1);
    }

    shapes[weapon->shapeNo].shapePtr->skeleton->update = 1;
    shapes[weapon->shapeNo].shapePtr->skeleton->super = camera->camPos;
    MatrixToN64Mtx(GetSkeletonMatrix(shapes[weapon->shapeNo].shapePtr->skeleton), (N64Mtx *)&g_mtxPool[g_mtxIndex]);
    AddRenderGroup(&weaponRenderList, shapes[weapon->shapeNo].shapePtr->group, &g_mtxPool[g_mtxIndex]);
    g_mtxIndex++;

    if (g_multiPlayer) {
        for (i = 0; i < NO_OF_SHIPS; i++) {
            ship = &ships[i];
            if (CAN_HIT_SHIP(i, ship, weapon)) {
                dist.vx = (ship->ppivot.vx - weapon->pos.vx) >> 2;
                dist.vy = (ship->ppivot.vy - weapon->pos.vy) >> 2;
                dist.vz = (ship->ppivot.vz - weapon->pos.vz) >> 2;
                if (GetMagnitude(dist) < 100) {
                    if (!(ship->attr & 0x4000) && !(ship->combatAttr & 0x100)) {
                        weapon->target = i;
                        weapon->count = -1;
                        if (!IS_PLAYER(i)) {
                            ship->speed >>= 3;
                        } else {
                            ship->vpivot.vx -= ship->vpivot.vx >> 3;
                            ship->vpivot.vy -= ship->vpivot.vy >> 3;
                            ship->vpivot.vz -= ship->vpivot.vz >> 3;
                            if (i == ownShip) {
                                SetRumble(5, 0);
                                SetShake(20, 0);
                            }
                            if (i == ownShip2) {
                                SetRumble(5, 1);
                                SetShake(20, 1);
                            }
                            if (i == ownShip3) {
                                SetRumble(5, 2);
                                SetShake(20, 2);
                            }
                            if (i == ownShip4) {
                                SetRumble(5, 3);
                                SetShake(20, 3);
                            }
                        }
                    }
                    DamageCollide(3, i, weapon);
                    weapon->count = -1;
                    shapes[weapon->shapeNo].used = 0;
                    PlayNote(0xA, 0xFF, 0xF, 0xF, 0x100);
                    return gfx;
                }
            }
        }
    }
    return gfx;
}

Gfx *InitQuake(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons,
               TrackCamera *camera, Gfx *gfx) {
    Face *facePtr;
    u8 unused[0x18]; /* unused: retail reserves 0x18 bytes of stack locals */

    if (!IS_PLAYER(weapon->owner)) {
        if (weapon->count > 1) {
            if (g_multiPlayer) {
                weapon->count--;
            }
            return gfx;
        }
        weapon->count = QUAKE_LIFE;
    } else {
        weapon->count = QUAKE_LIFE;
    }

    weapon->section = ships[weapon->owner].nearTrkSect;
    facePtr = track->faces + ships[weapon->owner].nearTrkSect->faceStart;
    while (!(facePtr->flags & TRACK_BASE)) {
        facePtr++;
    }
    GetFireVector(&weapon->accel, ships[weapon->owner].unitVecNose, facePtr,
                  ships[weapon->owner].ppivot, ships[weapon->owner].nearTrkSect);
    PAL_SCALE_ACCEL(weapon);
    weapon->velocity.vx = ships[weapon->owner].vpivot.vx >> 6;
    weapon->velocity.vy = ships[weapon->owner].vpivot.vy >> 6;
    weapon->velocity.vz = ships[weapon->owner].vpivot.vz >> 6;
    weapon->bumpStep = 0;
    weapon->pos.vx = ships[weapon->owner].nearTrkSect->nextSection.ptr->nextSection.ptr->centre.vx;
    weapon->pos.vy = ships[weapon->owner].nearTrkSect->nextSection.ptr->nextSection.ptr->centre.vy;
    weapon->pos.vz = ships[weapon->owner].nearTrkSect->nextSection.ptr->nextSection.ptr->centre.vz;
    weapon->update = UpdateQuake;
    return gfx;
}

Gfx *InitPlasma(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons,
                TrackCamera *camera, Gfx *gfx) {
    Face *facePtr;
    VECTOR unused; /* unused: retail reserves 0x10-byte stack slot */

    if (!IS_PLAYER(weapon->owner)) {
        if (weapon->count > 1) {
            if (g_multiPlayer) {
                weapon->count--;
            }
            return gfx;
        }
        weapon->count = PLASMA_LIFE;
    } else {
        weapon->count = PLASMA_LIFE;
    }

    weapon->section = ships[weapon->owner].nearTrkSect;
    if (g_weaponCharge[weapon->owner] >= PLASMA_CHARGE_HIGH) {
        weapon->type = 23;
    } else if (g_weaponCharge[weapon->owner] >= PLASMA_CHARGE_MID) {
        weapon->type = 24;
    } else {
        weapon->type = 25;
    }
    g_weaponCharge[weapon->owner] = 0;

    facePtr = track->faces + ships[weapon->owner].nearTrkSect->faceStart;
    while (!(facePtr->flags & TRACK_BASE)) {
        facePtr++;
    }
    GetFireVector(&weapon->accel, ships[weapon->owner].unitVecNose, facePtr,
                  ships[weapon->owner].ppivot, ships[weapon->owner].nearTrkSect);
    PAL_SCALE_ACCEL(weapon);
    weapon->bumpStep = -1;
    weapon->velocity.vx = weapon->accel.vx * 8;
    weapon->velocity.vy = weapon->accel.vy * 8;
    weapon->velocity.vz = weapon->accel.vz * 8;
    weapon->pos.vx = ships[weapon->owner].ppivot.vx;
    weapon->pos.vy = ships[weapon->owner].ppivot.vy;
    weapon->pos.vz = ships[weapon->owner].ppivot.vz;
    weapon->update = UpdatePlasma;
    return gfx;
}

Gfx *UpdatePlasma(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons,
                  TrackCamera *camera, Gfx *gfx) {
    VECTOR dist;
    VECTOR facePoint;
    VECTOR nextPoint;
    VECTOR path;
    VECTOR offset;
    VECTOR v1;
    VECTOR v2;
    VECTOR unusedVec; /* unused: retail reserves 0x10-byte stack slot */
    u8 rgb[3];
    TrackSection *nextSection;
    Face *facePtr;
    Face *nextFace;
    s32 alpha;
    s32 height;
    s32 dot;
    s32 i;
    ShipData *ship;

    if (g_multiPlayer) {
        weapon->section = FindNearestSection(weapon->section, weapon->pos);
        nextSection = weapon->section->nextSection.ptr;
        facePtr = track->faces + weapon->section->faceStart;
        nextFace = track->faces + nextSection->faceStart;
        while (!(facePtr->flags & TRACK_BASE)) {
            facePtr++;
        }
        while (!(nextFace->flags & TRACK_BASE)) {
            nextFace++;
        }

        weapon->velocity.vx += weapon->accel.vx;
        weapon->velocity.vy += weapon->accel.vy;
        weapon->velocity.vz += weapon->accel.vz;
        weapon->velocity.vx -= weapon->velocity.vx >> 6;
        weapon->velocity.vy -= weapon->velocity.vy >> 6;
        weapon->velocity.vz -= weapon->velocity.vz >> 6;
        weapon->pos.vx += weapon->velocity.vx;
        weapon->pos.vz += weapon->velocity.vz;

        GetFaceVertex(weapon->section, facePtr, 0, &facePoint);
        GetFaceVertex(nextSection, nextFace, 0, &nextPoint);
        path.vx = nextPoint.vx - facePoint.vx;
        path.vy = nextPoint.vy - facePoint.vy;
        path.vz = nextPoint.vz - facePoint.vz;
        offset.vx = weapon->pos.vx - facePoint.vx;
        offset.vz = weapon->pos.vz - facePoint.vz;
        height = SquareRoot0(path.vx * path.vx + path.vz * path.vz);
        dot = path.vx * offset.vx + path.vz * offset.vz;
        alpha = ((dot / height) << 16) / height;
        height = nextPoint.vy - facePoint.vy;
        height = (height * alpha) >> 16;
        weapon->pos.vy = facePoint.vy + height - 300;
        ratan2(path.vy, SquareRoot0(path.vx * path.vx + path.vz * path.vz));

        v1.vx = facePoint.vx;
        v1.vy = facePoint.vy;
        v1.vz = facePoint.vz;
        facePtr++;
        GetFaceVertex(weapon->section, facePtr, 0, &v2);
        path.vx = v2.vx - v1.vx;
        path.vz = v2.vz - v1.vz;
        offset.vx = weapon->pos.vx - v1.vx;
        offset.vz = weapon->pos.vz - v1.vz;
        height = SquareRoot0(path.vx * path.vx + path.vz * path.vz);
        dot = path.vx * offset.vx + path.vz * offset.vz;
        alpha = ((dot / height) << 16) / height;
        height = v2.vy - v1.vy;
        alpha = (height * alpha) >> 16;
        height = alpha - (height >> 1);
        weapon->pos.vy += height;
        weapon->pitch = 0;
        weapon->hdg += 128;
        weapon->count--;

        for (i = 0; i < NO_OF_SHIPS; i++) {
            ship = &ships[i];
            if (CAN_HIT_SHIP(i, ship, weapon)) {
                dist.vx = (ship->ppivot.vx - weapon->pos.vx) >> 2;
                dist.vy = (ship->ppivot.vy - weapon->pos.vy) >> 2;
                dist.vz = (ship->ppivot.vz - weapon->pos.vz) >> 2;
                if (GetMagnitude(dist) < 100) {
                    if (!(ship->attr & 0x4000) && !(ship->combatAttr & 0x100)) {
                        if (!IS_PLAYER(i)) {
                            FlipShip(ship);
                            ship->speed >>= 5;
                            ship->vroll += newRand() % 250;
                            ship->ahdg = (newRand() - 16384) >> 1;
                        } else {
                            ship->vpivot.vx -= (ship->vpivot.vx * 3) >> 2;
                            ship->vpivot.vy -= (ship->vpivot.vy * 3) >> 2;
                            ship->vpivot.vz -= (ship->vpivot.vz * 3) >> 2;
                            ship->vroll += newRand() % 250;
                            ship->ahdg = (newRand() - 16384) >> 1;
                            if (i == ownShip) {
                                SetRumble(8, 0);
                                SetShake(20, 0);
                            }
                            if (i == ownShip2) {
                                SetRumble(8, 1);
                                SetShake(20, 1);
                            }
                            if (i == ownShip3) {
                                SetRumble(8, 2);
                                SetShake(20, 2);
                            }
                            if (i == ownShip4) {
                                SetRumble(8, 3);
                                SetShake(20, 3);
                            }
                        }
                    }
                    rgb[2] = 0xFF;
                    rgb[1] = 0xFF;
                    rgb[0] = 0xFF;
                    AddCycloneFlash(weapon->pos.vx, weapon->pos.vy, weapon->pos.vz, 12, rgb);
                    AddExplode(weapon->pos.vx, weapon->pos.vy, weapon->pos.vz, -1);
                    DamageCollide(weapon->type, i, weapon);
                    weapon->bumpStep = i;
                    if (IS_PLAYER(i)) {
                        PlayNote(0xA, 0xFF, 0xF, 0xF, 0x100);
                    } else {
                        PlayNote(0xA, 0xFF, 0xF, 0xF, 0x100);
                    }
                    return gfx;
                }
            }
        }

        facePtr = track->faces + weapon->section->faceStart;
        if (!(weapon->section->flags & 1)) {
            for (i = 0; i < weapon->section->faceCount; i++) {
                GetFaceVertex(weapon->section, facePtr, 0, &facePoint);
                if (PlaneLineCollision(facePoint, weapon->pos, facePtr->normal) <= 0 &&
                    !(facePtr->flags & TRACK_BASE)) {
                    weapon->count = -2;
                    PlayNote(0xB, 0xA0, 0xF, 0xF, 0x100);
                    return gfx;
                }
                facePtr++;
            }
        }
        Weapon2Mine(weapon, weapons, shapes, 1);
    }
    AddCycloneTrail(&weapon->pos, &weapon->velocity, 10, weapon->type);
    return gfx;
}

Gfx *InitRocket(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons,
                TrackCamera *camera, Gfx *gfx) {
    s32 i;
    Face *facePtr;
    VECTOR unused; /* unused: retail reserves 0x10-byte stack slot */

    if (!IS_PLAYER(weapon->owner)) {
        if (weapon->count > 1) {
            if (g_multiPlayer) {
                weapon->count--;
            }
            return gfx;
        }
        weapon->count = ROCKET_LIFE;
    } else {
        weapon->count = ROCKET_LIFE;
    }

    weapon->section = ships[weapon->owner].nearTrkSect;
    for (i = 0; i < 12; i++) {
        if (shapes[i].used == 0) {
            weapon->shapeNo = i;
            shapes[i].used = 1;
            facePtr = track->faces + ships[weapon->owner].nearTrkSect->faceStart;
            while (!(facePtr->flags & TRACK_BASE)) {
                facePtr++;
            }
            GetFireVector(&weapon->accel, ships[weapon->owner].unitVecNose, facePtr,
                          ships[weapon->owner].ppivot, ships[weapon->owner].nearTrkSect);
            PAL_SCALE_ACCEL(weapon);
            weapon->bumpStep = 0;
            weapon->velocity.vx = weapon->accel.vx * 8;
            weapon->velocity.vy = weapon->accel.vy * 8;
            weapon->velocity.vz = weapon->accel.vz * 8;
            weapon->pos.vx = ships[weapon->owner].ppivot.vx;
            weapon->pos.vy = ships[weapon->owner].ppivot.vy;
            weapon->pos.vz = ships[weapon->owner].ppivot.vz;
            SetSkeletonDirectionHPR(shapes[weapon->shapeNo].shapePtr->skeleton,
                                    ships[weapon->owner].hdg, ships[weapon->owner].pitch, ships[weapon->owner].roll);
            weapon->update = UpdateRocket;
            return gfx;
        }
    }
    weapon->count = 0;
    return gfx;
}

void SetDebrisPrimColour(Gfx *dl, u8 r, u8 g, u8 b, s32 a) {
    while (dl->dma.cmd != G_ENDDL && (s8)dl->dma.cmd != (s8)G_SETPRIMCOLOR) {
        dl++;
    }
    if (dl->dma.cmd != G_ENDDL) {
        gDPSetPrimColor(dl, 0, 0, r, g, b, a);
    }
}

Gfx *UpdateShipDebris(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons,
                      TrackCamera *camera, Gfx *gfx) {
    if (g_multiPlayer) {
        weapon->count--;
        if (weapon->count == 0) {
            weapon->count = -1;
            return gfx;
        }
        weapon->velocity.vx += weapon->accel.vx;
        weapon->velocity.vy += weapon->accel.vy;
        weapon->velocity.vz += weapon->accel.vz;
        weapon->accel.vx -= weapon->accel.vx >> 2;
        weapon->accel.vy -= weapon->accel.vy >> 2;
        weapon->accel.vz -= weapon->accel.vz >> 2;
        weapon->pos.vx += weapon->velocity.vx;
        weapon->pos.vy += weapon->velocity.vy;
        weapon->pos.vz += weapon->velocity.vz;
        weapon->pitch += weapon->pos.pad;
        weapon->hdg += weapon->velocity.pad;
        SetSkeletonDirectionHPR(shapes[weapon->shapeNo].shapePtr->skeleton, weapon->hdg, weapon->pitch, 0);
        SetSkeletonPosition(shapes[weapon->shapeNo].shapePtr->skeleton,
                            weapon->pos.vx >> 1, weapon->pos.vy >> 1, weapon->pos.vz >> 1);
    }

    shapes[weapon->shapeNo].shapePtr->skeleton->update = 1;
    shapes[weapon->shapeNo].shapePtr->skeleton->super = camera->camPos;
    MatrixToN64Mtx(GetSkeletonMatrix(shapes[weapon->shapeNo].shapePtr->skeleton), (N64Mtx *)&g_mtxPool[g_mtxIndex]);
    AddRenderGroup(&weaponRenderList, shapes[weapon->shapeNo].shapePtr->group, &g_mtxPool[g_mtxIndex]);
    g_mtxIndex++;
    return gfx;
}

Gfx *InitShipDebris(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons,
                    TrackCamera *camera, Gfx *gfx) {
    u8 rgb[3];
    s32 startShape;
    s32 count;
    s32 i;
    u32 j;
    s32 k;
    s16 team;
    s16 primCount;
    DebrisPrim *prim;

    weapon->count = g_debrisLife;
    switch (weapon->type) {
    case 26:
        count = 4;
        startShape = 44;
        break;
    case 27:
        count = 4;
        startShape = 48;
        break;
    case 28:
        count = 4;
        startShape = 52;
        break;
    case 29:
        count = 4;
        startShape = 56;
        break;
    }

    for (i = 0; i < count; i++) {
        if (shapes[startShape + i].used == 0) {
            weapon->shapeNo = startShape + i;
            shapes[startShape + i].used = 1;
            weapon->pos.vx = ships[weapon->owner].ppivot.vx;
            weapon->pos.vy = ships[weapon->owner].ppivot.vy;
            weapon->pos.vz = ships[weapon->owner].ppivot.vz;
            weapon->velocity.vx = (newRand() % (g_debrisSpeed * 2)) - g_debrisSpeed;
            weapon->velocity.vy = newRand() % (g_debrisSpeed * 2);
            weapon->velocity.vz = (newRand() % (g_debrisSpeed * 2)) - g_debrisSpeed;
            weapon->accel.vx = 0;
            weapon->accel.vy = g_debrisGravity;
            weapon->accel.vz = 0;
            weapon->pos.pad = newRand() % 256;
            weapon->velocity.pad = newRand() % 256;
            weapon->pitch = 0;
            weapon->hdg = 0;

            if (g_unk800923F9 && weapon->owner == ownShip4) {
                team = GetShipTeam(ownShip3);
            } else {
                team = GetShipTeam(weapon->owner);
            }
            switch (team) {
            case 16:
                switch (weapon->type) {
                case 26:
                    rgb[0] = 0;
                    rgb[1] = 0;
                    rgb[2] = 0x78;
                    break;
                case 27:
                    rgb[0] = 0;
                    rgb[1] = 0;
                    rgb[2] = 0x78;
                    break;
                case 28:
                    rgb[0] = 0x60;
                    rgb[1] = 0;
                    rgb[2] = 0;
                    break;
                case 29:
                    rgb[0] = 0;
                    rgb[1] = 0;
                    rgb[2] = 0;
                    break;
                }
                break;
            case 15:
                switch (weapon->type) {
                case 26:
                    rgb[0] = 0;
                    rgb[1] = 0;
                    rgb[2] = 0x60;
                    break;
                case 27:
                    rgb[0] = 0;
                    rgb[1] = 0;
                    rgb[2] = 0x60;
                    break;
                case 28:
                    rgb[0] = 0xC8;
                    rgb[1] = 0xA0;
                    rgb[2] = 8;
                    break;
                case 29:
                    rgb[0] = 0;
                    rgb[1] = 0;
                    rgb[2] = 0;
                    break;
                }
                break;
            case 17:
                switch (weapon->type) {
                case 26:
                    rgb[0] = 0x40;
                    rgb[1] = 0;
                    rgb[2] = 0x48;
                    break;
                case 27:
                    rgb[0] = 0x40;
                    rgb[1] = 0;
                    rgb[2] = 0x48;
                    break;
                case 28:
                    rgb[0] = 0;
                    rgb[1] = 0;
                    rgb[2] = 0x40;
                    break;
                case 29:
                    rgb[0] = 0;
                    rgb[1] = 0;
                    rgb[2] = 0;
                    break;
                }
                break;
            case 19:
                switch (weapon->type) {
                case 26:
                    rgb[0] = 0x68;
                    rgb[1] = 0;
                    rgb[2] = 0;
                    break;
                case 27:
                    rgb[0] = 0x68;
                    rgb[1] = 0;
                    rgb[2] = 0;
                    break;
                case 28:
                    rgb[0] = 0x68;
                    rgb[1] = 0x38;
                    rgb[2] = 0;
                    break;
                case 29:
                    rgb[0] = 0;
                    rgb[1] = 0;
                    rgb[2] = 0;
                    break;
                }
                break;
            case 18:
                switch (weapon->type) {
                case 26:
                    rgb[0] = 0x58;
                    rgb[1] = 0;
                    rgb[2] = 0;
                    break;
                case 27:
                    rgb[0] = 0x58;
                    rgb[1] = 0;
                    rgb[2] = 0;
                    break;
                case 28:
                    rgb[0] = 0xB8;
                    rgb[1] = 0xB0;
                    rgb[2] = 0xA8;
                    break;
                case 29:
                    rgb[0] = 0;
                    rgb[1] = 0;
                    rgb[2] = 0xA0;
                    break;
                }
                break;
            }

            prim = shapes[weapon->shapeNo].shapePtr->primitives;
            primCount = shapes[weapon->shapeNo].shapePtr->primitiveCount;
            for (k = 0; k < primCount; k++) {
                switch (prim->type) {
                case 1:
                    for (j = 0; j < shapes[weapon->shapeNo].shapePtr->group->count; j++) {
                        SetDebrisPrimColour(shapes[weapon->shapeNo].shapePtr->group->entries[j].dl,
                                            rgb[0], rgb[1], rgb[2], 0xFF);
                    }
                    break;
                case 4:
                    for (j = 0; j < shapes[weapon->shapeNo].shapePtr->group->count; j++) {
                        SetDebrisPrimColour(shapes[weapon->shapeNo].shapePtr->group->entries[j].dl,
                                            rgb[0], rgb[1], rgb[2], 0xFF);
                    }
                    break;
                }
            }
            weapon->update = UpdateShipDebris;
            return gfx;
        }
    }
    weapon->count = 0;
    return gfx;
}

Gfx *InitRocketLeft(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons,
                    TrackCamera *camera, Gfx *gfx) {
    s32 i;
    Face *facePtr;
    s32 unused[2]; /* unused: retail reserves 8 bytes before offset */
    VECTOR offset;

    if (!IS_PLAYER(weapon->owner)) {
        if (weapon->count > 1) {
            if (g_multiPlayer) {
                weapon->count--;
            }
            return gfx;
        }
        weapon->count = ROCKET_LIFE;
    } else {
        weapon->count = ROCKET_LIFE;
    }

    weapon->section = ships[weapon->owner].nearTrkSect;
    for (i = 0; i < 12; i++) {
        if (shapes[i].used == 0) {
            weapon->shapeNo = i;
            shapes[i].used = 1;
            facePtr = track->faces + ships[weapon->owner].nearTrkSect->faceStart;
            while (!(facePtr->flags & TRACK_BASE)) {
                facePtr++;
            }
            GetFireVector(&weapon->accel, ships[weapon->owner].unitVecNose, facePtr,
                          ships[weapon->owner].ppivot, ships[weapon->owner].nearTrkSect);
            PAL_SCALE_ACCEL(weapon);
            weapon->velocity.vx = weapon->accel.vx * 8;
            weapon->velocity.vy = weapon->accel.vy * 8;
            weapon->velocity.vz = weapon->accel.vz * 8;
            weapon->velocity.vx += ships[weapon->owner].unitVecWing.vx >> 6;
            weapon->velocity.vy += ships[weapon->owner].unitVecWing.vy >> 6;
            weapon->velocity.vz += ships[weapon->owner].unitVecWing.vz >> 6;
            offset.vx = ships[weapon->owner].unitVecWing.vx >> 4;
            offset.vy = ships[weapon->owner].unitVecWing.vy >> 4;
            offset.vz = ships[weapon->owner].unitVecWing.vz >> 4;
            offset.vx -= ships[weapon->owner].unitVecNose.vx >> 4;
            offset.vy -= ships[weapon->owner].unitVecNose.vy >> 4;
            offset.vz -= ships[weapon->owner].unitVecNose.vz >> 4;
            weapon->pos.vx = ships[weapon->owner].ppivot.vx + offset.vx;
            weapon->pos.vy = ships[weapon->owner].ppivot.vy + offset.vy;
            weapon->pos.vz = ships[weapon->owner].ppivot.vz + offset.vz;
            SetSkeletonDirectionHPR(shapes[weapon->shapeNo].shapePtr->skeleton,
                                    ships[weapon->owner].hdg, ships[weapon->owner].pitch, ships[weapon->owner].roll);
            weapon->update = UpdateRocket;
            return gfx;
        }
    }
    weapon->count = 0;
    return gfx;
}

Gfx *InitRocketRight(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons,
                     TrackCamera *camera, Gfx *gfx) {
    s32 i;
    Face *facePtr;
    s32 unused[2]; /* unused: retail reserves 8 bytes before offset */
    VECTOR offset;

    if (!IS_PLAYER(weapon->owner)) {
        if (weapon->count > 1) {
            if (g_multiPlayer) {
                weapon->count--;
            }
            return gfx;
        }
        weapon->count = ROCKET_LIFE;
    } else {
        weapon->count = ROCKET_LIFE;
    }

    weapon->section = ships[weapon->owner].nearTrkSect;
    for (i = 0; i < 12; i++) {
        if (shapes[i].used == 0) {
            weapon->shapeNo = i;
            shapes[i].used = 1;
            facePtr = track->faces + ships[weapon->owner].nearTrkSect->faceStart;
            while (!(facePtr->flags & TRACK_BASE)) {
                facePtr++;
            }
            GetFireVector(&weapon->accel, ships[weapon->owner].unitVecNose, facePtr,
                          ships[weapon->owner].ppivot, ships[weapon->owner].nearTrkSect);
            PAL_SCALE_ACCEL(weapon);
            weapon->velocity.vx = weapon->accel.vx * 8;
            weapon->velocity.vy = weapon->accel.vy * 8;
            weapon->velocity.vz = weapon->accel.vz * 8;
            weapon->velocity.vx -= ships[weapon->owner].unitVecWing.vx >> 6;
            weapon->velocity.vy -= ships[weapon->owner].unitVecWing.vy >> 6;
            weapon->velocity.vz -= ships[weapon->owner].unitVecWing.vz >> 6;
            offset.vx = -ships[weapon->owner].unitVecWing.vx >> 4;
            offset.vy = -ships[weapon->owner].unitVecWing.vy >> 4;
            offset.vz = -ships[weapon->owner].unitVecWing.vz >> 4;
            offset.vx -= ships[weapon->owner].unitVecNose.vx >> 4;
            offset.vy -= ships[weapon->owner].unitVecNose.vy >> 4;
            offset.vz -= ships[weapon->owner].unitVecNose.vz >> 4;
            weapon->pos.vx = ships[weapon->owner].ppivot.vx + offset.vx;
            weapon->pos.vy = ships[weapon->owner].ppivot.vy + offset.vy;
            weapon->pos.vz = ships[weapon->owner].ppivot.vz + offset.vz;
            SetSkeletonDirectionHPR(shapes[weapon->shapeNo].shapePtr->skeleton,
                                    ships[weapon->owner].hdg, ships[weapon->owner].pitch, ships[weapon->owner].roll);
            weapon->update = UpdateRocket;
            return gfx;
        }
    }
    weapon->count = 0;
    return gfx;
}

Gfx *UpdateElecBolt(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons,
                    TrackCamera *camera, Gfx *gfx) {
    VECTOR dist;
    VECTOR targetVector;
    VECTOR unusedVec; /* unused: retail reserves 0x10-byte stack slot */
    VECTOR facePoint;
    s32 horizDist;
    s32 vhdg;
    s32 vpitch;
    s32 alpha;
    s32 i;
    Face *facePtr;
    ShipData *ship;

    if (g_multiPlayer) {
        if (weapon->target != -1) {
            targetVector.vx = (ships[weapon->target].ppivot.vx - weapon->pos.vx) >> 3;
            targetVector.vy = (ships[weapon->target].ppivot.vy - weapon->pos.vy) >> 3;
            targetVector.vz = (ships[weapon->target].ppivot.vz - weapon->pos.vz) >> 3;
            horizDist = SquareRoot0(targetVector.vx * targetVector.vx + targetVector.vz * targetVector.vz);
            vhdg = -ratan2(targetVector.vx, targetVector.vz) - weapon->hdg;
            vpitch = -ratan2(targetVector.vy, horizDist) - weapon->pitch;
        } else {
            vhdg = 0;
            vpitch = 0;
        }
        if (vhdg > 2048) {
            vhdg -= 4096;
        } else if (vhdg < -2048) {
            vhdg += 4096;
        }
        if (vpitch > 2048) {
            vpitch -= 4096;
        } else if (vpitch < -2048) {
            vpitch += 4096;
        }
        weapon->hdg += vhdg >> 2;
        weapon->pitch += vpitch >> 2;
        weapon->hdg = ang(weapon->hdg);
        weapon->pitch = ang(weapon->pitch);

        weapon->accel.vx = (-rsin(weapon->hdg) * rcos(weapon->pitch)) >> 16;
        weapon->accel.vy = -rsin(weapon->pitch) >> 4;
        weapon->accel.vz = (rcos(weapon->hdg) * rcos(weapon->pitch)) >> 16;
        PAL_SCALE_ACCEL(weapon);
        weapon->velocity.vx += weapon->accel.vx;
        weapon->velocity.vy += weapon->accel.vy;
        weapon->velocity.vz += weapon->accel.vz;
        weapon->velocity.vx -= weapon->velocity.vx >> 2;
        weapon->velocity.vy -= weapon->velocity.vy >> 2;
        weapon->velocity.vz -= weapon->velocity.vz >> 2;
        weapon->pos.vx += weapon->velocity.vx;
        weapon->pos.vy += weapon->velocity.vy;
        weapon->pos.vz += weapon->velocity.vz;

        weapon->section = FindNearestSection(weapon->section, weapon->pos);
        facePtr = track->faces + weapon->section->faceStart;
        while (!(facePtr->flags & TRACK_BASE)) {
            facePtr++;
        }
        GetFaceVertex(weapon->section, facePtr, 0, &facePoint);
        alpha = PlaneLineCollision(facePoint, weapon->pos, facePtr->normal);
        if (alpha < 2000) {
            s32 diff = 200 - alpha;
            weapon->pos.vx += (diff * facePtr->normal.vx) >> 13;
            weapon->pos.vy += (diff * facePtr->normal.vy) >> 13;
            weapon->pos.vz += (diff * facePtr->normal.vz) >> 13;
        }
        weapon->count--;
        if (weapon->count == 0) {
            shapes[weapon->shapeNo].used = 0;
        }
        SetSkeletonPosition(shapes[weapon->shapeNo].shapePtr->skeleton,
                            weapon->pos.vx >> 1, weapon->pos.vy >> 1, weapon->pos.vz >> 1);
        SetSkeletonDirectionHPR(shapes[weapon->shapeNo].shapePtr->skeleton, weapon->hdg, weapon->pitch, 0);
    }

    shapes[weapon->shapeNo].shapePtr->skeleton->update = 1;
    shapes[weapon->shapeNo].shapePtr->skeleton->super = camera->camPos;
    MatrixToN64Mtx(GetSkeletonMatrix(shapes[weapon->shapeNo].shapePtr->skeleton), (N64Mtx *)&g_mtxPool[g_mtxIndex]);
    AddRenderGroup(&weaponRenderList, shapes[weapon->shapeNo].shapePtr->group, &g_mtxPool[g_mtxIndex]);
    g_mtxIndex++;

    if (g_multiPlayer) {
        for (i = 0; i < NO_OF_SHIPS; i++) {
            ship = &ships[i];
            if (CAN_HIT_SHIP(i, ship, weapon)) {
                dist.vx = (ship->ppivot.vx - weapon->pos.vx) >> 3;
                dist.vy = (ship->ppivot.vy - weapon->pos.vy) >> 3;
                dist.vz = (ship->ppivot.vz - weapon->pos.vz) >> 3;
                if (GetMagnitude(dist) < 100) {
                    if (!(ship->attr & 0x4000) && !(ship->combatAttr & 0x100)) {
                        ship->attr |= 0x8000;
                        ship->electroCount = ELEC_BOLT_LIFE;
                    }
                    DamageCollide(5, i, weapon);
                    if (i == ownShip) {
                        SetShake(ELEC_BOLT_LIFE, 0);
                        SetRumble(ELEC_BOLT_LIFE, 0);
                    }
                    if (i == ownShip2) {
                        SetShake(ELEC_BOLT_LIFE, 1);
                        SetRumble(ELEC_BOLT_LIFE, 1);
                    }
                    if (i == ownShip3) {
                        SetShake(ELEC_BOLT_LIFE, 2);
                        SetRumble(ELEC_BOLT_LIFE, 2);
                    }
                    if (i == ownShip4) {
                        SetShake(ELEC_BOLT_LIFE, 3);
                        SetRumble(ELEC_BOLT_LIFE, 3);
                    }
                    AddRandomParticles(particleSlots, 64, &weapon->pos, &weapon->velocity);
                    weapon->count = -1;
                    weapon->target = i;
                    shapes[weapon->shapeNo].used = 0;
                    return gfx;
                }
            }
        }

        facePtr = track->faces + weapon->section->faceStart;
        for (i = 0; i < weapon->section->faceCount; i++) {
            GetFaceVertex(weapon->section, facePtr, 0, &facePoint);
            if (PlaneLineCollision(facePoint, weapon->pos, facePtr->normal) <= 0) {
                weapon->count = -2;
                shapes[weapon->shapeNo].used = 0;
                PlayNote(0xB, 0xA0, 0xF, 0xF, 0x100);
                return gfx;
            }
            facePtr++;
        }
        Weapon2Mine(weapon, weapons, shapes, 0);
    }
    return gfx;
}

Gfx *InitElecBolt(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons,
                  TrackCamera *camera, Gfx *gfx) {
    s32 i;

    if (!IS_PLAYER(weapon->owner)) {
        if (weapon->count > 1) {
            if (g_multiPlayer) {
                weapon->count--;
            }
            return gfx;
        }
        weapon->count = ELEC_BOLT_LIFE;
    } else {
        weapon->count = ELEC_BOLT_LIFE;
    }

    weapon->section = ships[weapon->owner].nearTrkSect;
    for (i = 0; i < 4; i++) {
        if (shapes[i + 36].used == 0) {
            weapon->shapeNo = i + 36;
            shapes[i + 36].used = 1;
            weapon->target = ships[weapon->owner].targetShip;
            weapon->accel.vx = ships[weapon->owner].unitVecNose.vx >> 8;
            weapon->accel.vy = ships[weapon->owner].unitVecNose.vy >> 8;
            weapon->accel.vz = ships[weapon->owner].unitVecNose.vz >> 8;
            PAL_SCALE_ACCEL(weapon);
            weapon->velocity.vx = ships[weapon->owner].vpivot.vx >> 6;
            weapon->velocity.vy = ships[weapon->owner].vpivot.vy >> 6;
            weapon->velocity.vz = ships[weapon->owner].vpivot.vz >> 6;
            weapon->pos.vx = ships[weapon->owner].ppivot.vx;
            weapon->pos.vy = ships[weapon->owner].ppivot.vy;
            weapon->pos.vz = ships[weapon->owner].ppivot.vz;
            SetSkeletonDirectionHPR(shapes[weapon->shapeNo].shapePtr->skeleton,
                                    ships[weapon->owner].hdg, ships[weapon->owner].pitch, ships[weapon->owner].roll);
            weapon->hdg = ships[weapon->owner].hdg;
            weapon->pitch = ships[weapon->owner].pitch;
            weapon->update = UpdateElecBolt;
            ships[weapon->owner].targetShip = -1;
            return gfx;
        }
    }
    weapon->count = 0;
    return gfx;
}

Gfx *InitMine(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons,
              TrackCamera *camera, Gfx *gfx) {
    s32 i;

    if (weapon->count == 1) {
        weapon->section = ships[weapon->owner].nearTrkSect;
        for (i = 0; i < 20; i++) {
            if (shapes[i + 12].used == 0) {
                weapon->shapeNo = i + 12;
                shapes[i + 12].used = 1;
                weapon->pos.vx = ships[weapon->owner].ppivot.vx;
                weapon->pos.vy = ships[weapon->owner].ppivot.vy;
                weapon->pos.vz = ships[weapon->owner].ppivot.vz;
                weapon->hdg = 0;
                if (IS_PLAYER(weapon->owner)) {
                    PlayNote(0x1D, 0xC8, 0xF, 0xF, 0x100);
                }
                weapon->update = UpdateMine;
                weapon->count = MINE_LIFE;
                return gfx;
            }
        }
        weapon->count = 0;
    } else if (weapon->count > 0 && g_multiPlayer) {
        weapon->count--;
    }
    return gfx;
}

Gfx *InitTrackTrap(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons,
                   TrackCamera *camera, Gfx *gfx) {
    s32 i;
    Face *facePtr;

    if (!IS_PLAYER(weapon->owner)) {
        if (weapon->count > 1) {
            if (g_multiPlayer) {
                weapon->count--;
            }
            return gfx;
        }
        weapon->count = TRACK_TRAP_LIFE;
    } else {
        weapon->count = TRACK_TRAP_LIFE;
    }

    weapon->section = ships[weapon->owner].nearTrkSect;
    for (i = 0; i < 10; i++) {
        if (weapon->section->nextSection.ptr != NULL) {
            weapon->section = weapon->section->nextSection.ptr;
        }
    }
    weapon->accel.pad = weapon->section->faceStart;
    facePtr = track->faces + weapon->section->faceStart;
    while (!(facePtr->flags & TRACK_BASE)) {
        facePtr++;
        weapon->accel.pad++;
    }

    if (ships[weapon->owner].attr & 0x20) {
        weapon->velocity.pad = 1;
    } else {
        weapon->velocity.pad = 0;
        weapon->accel.pad++;
    }
    weapon->update = UpdateTrackTrap;
    InitTrackTrapFace(weapon, &ships[weapon->owner]);
    return gfx;
}

Gfx *UpdateTrackTrap(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons,
                     TrackCamera *camera, Gfx *gfx) {
    s32 i;
    ShipData *ship;
    Face *facePtr;

    if (g_multiPlayer) {
        weapon->count--;
        /* Unused pointer preserved from retail: CSE reuses track->faces and index*20 before the branch */
        facePtr = track->faces + weapon->accel.pad;
        if (weapon->count == 0) {
            track->faces[weapon->accel.pad].red = track->colCopy[weapon->accel.pad].red;
            track->faces[weapon->accel.pad].green = track->colCopy[weapon->accel.pad].green;
            track->faces[weapon->accel.pad].blue = track->colCopy[weapon->accel.pad].blue;
            return gfx;
        }
        for (i = 0; i < NO_OF_SHIPS; i++) {
            ship = &ships[i];
            if (CAN_HIT_SHIP(i, ship, weapon) && ship->nearTrkSect == weapon->section) {
                if ((weapon->velocity.pad == 1 && (ship->attr & 0x20)) ||
                    (weapon->velocity.pad == 0 && !(ship->attr & 0x20))) {
                    DamageCollide(17, i, weapon);
                    weapon->count = -1;
                    weapon->target = i;
                    FlipShip(ship);
                    ship->speed >>= 5;
                    ship->vroll += newRand() % 250;
                    ship->ahdg = (newRand() - 16384) >> 1;
                    D_800AD820 = 0;
                    PlayNote(0x41, 0xA0, 0xF, 0xF, 0x100);
                    if (i == ownShip) {
                        SetRumble(8, 0);
                        SetShake(20, 0);
                    } else if (i == ownShip2) {
                        SetRumble(8, 1);
                        SetShake(20, 1);
                    } else if (i == ownShip3) {
                        SetRumble(8, 2);
                        SetShake(20, 2);
                    } else if (i == ownShip4) {
                        SetRumble(8, 3);
                        SetShake(20, 3);
                    }
                    return gfx;
                }
            }
        }
    }
    return gfx;
}

Gfx *InitSmartBomb(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons,
                   TrackCamera *camera, Gfx *gfx) {
    s32 i;

    weapon->section = ships[weapon->owner].nearTrkSect;
    for (i = 0; i < 20; i++) {
        if (shapes[i + 12].used == 0) {
            weapon->shapeNo = i + 12;
            shapes[i + 12].used = 1;
            weapon->pos.vx = ships[weapon->owner].ppivot.vx;
            weapon->pos.vy = ships[weapon->owner].ppivot.vy;
            weapon->pos.vz = ships[weapon->owner].ppivot.vz;
            weapon->velocity.vx = ships[weapon->owner].vpivot.vx >> 5;
            weapon->velocity.vy = ships[weapon->owner].vpivot.vy >> 5;
            weapon->velocity.vz = ships[weapon->owner].vpivot.vz >> 5;
            weapon->hdg = 0;
            if (IS_PLAYER(weapon->owner)) {
                PlayNote(0x1D, 0xC8, 0xF, 0xF, 0x100);
            }
            weapon->update = UpdateSmartBomb;
            weapon->count = SMART_BOMB_LIFE;
            return gfx;
        }
    }
    weapon->count = 0;
    return gfx;
}

void KillMines(WeaponData *weapon, WeaponData *weapons, WeaponShape *shapes) {
    s32 i;

    AddExplode(weapon->pos.vx, weapon->pos.vy, weapon->pos.vz, -1);
    for (i = 0; i < NO_OF_WEAPONS; i++) {
        if (weapons[i].type == WEAPON_MINE && weapons[i].count > 0) {
            weapons[i].count = 0;
            weapon->count = 0;
            AddExplode(weapons[i].pos.vx, weapons[i].pos.vy, weapons[i].pos.vz, -1);
            shapes[weapons[i].shapeNo].used = 0;
            shapes[weapon->shapeNo].used = 0;
            PlayNote(0xB, 0xA0, 0xF, 0xF, 0x100);
        }
    }
}

Gfx *UpdateSmartBomb(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons,
                     TrackCamera *camera, Gfx *gfx) {
    VECTOR dist;
    s32 i;
    ShipData *ship;

    if (g_multiPlayer) {
        weapon->count--;
        if (weapon->count == 0) {
            if (weapon->owner == ownShip) {
                fadeAmount[0] = 255;
            } else if (weapon->owner == ownShip2) {
                fadeAmount[1] = 255;
            } else if (weapon->owner == ownShip3) {
                fadeAmount[2] = 255;
            } else if (weapon->owner == ownShip4) {
                fadeAmount[3] = 255;
            }
            PlayNote(0xA, 0xFF, 0xF, 0xF, 0x100);
            KillMines(weapon, weapons, shapes);
            for (i = 0; i < NO_OF_SHIPS; i++) {
                ship = &ships[i];
                dist.vx = (ship->ppivot.vx - weapon->pos.vx) >> 3;
                dist.vy = (ship->ppivot.vy - weapon->pos.vy) >> 3;
                dist.vz = (ship->ppivot.vz - weapon->pos.vz) >> 3;
                if (GetMagnitude(dist) < 2900 && i != weapon->owner && !(ship->combatAttr & 0x8000)) {
                    DamageCollide(14, i, weapon);
                    if (i == ownShip) {
                        fadeAmount[0] = 255;
                    } else if (i == ownShip2) {
                        fadeAmount[1] = 255;
                    } else if (i == ownShip3) {
                        fadeAmount[2] = 255;
                    } else if (i == ownShip4) {
                        fadeAmount[3] = 255;
                    }
                    if (!(ship->attr & 0x4000) && !(ship->combatAttr & 0x100)) {
                        if (!IS_PLAYER(i)) {
                            ship->speed >>= 1;
                            ship->vroll += 5000;
                            ship->ahdg = (newRand() - 16384) >> 1;
                        } else {
                            ship->vpivot.vx -= (ship->vpivot.vx * 3) >> 1;
                            ship->vpivot.vy -= (ship->vpivot.vy * 3) >> 1;
                            ship->vpivot.vz -= (ship->vpivot.vz * 3) >> 1;
                            ship->vroll += newRand() % 250;
                            ship->ahdg = (newRand() - 16384) >> 3;
                            if (i == ownShip) {
                                SetRumble(8, 0);
                                SetShake(20, 0);
                            } else if (i == ownShip2) {
                                SetRumble(8, 1);
                                SetShake(20, 1);
                            } else if (i == ownShip3) {
                                SetRumble(8, 2);
                                SetShake(20, 2);
                            } else if (i == ownShip4) {
                                SetRumble(8, 3);
                                SetShake(20, 3);
                            }
                        }
                        AddExplode(ship->ppivot.vx + (newRand() & 0x3FF) - 0x200,
                                   ship->ppivot.vy - (newRand() & 0x1FF),
                                   ship->ppivot.vz + (newRand() & 0x3FF) - 0x200, -1);
                        AddExplode(ship->ppivot.vx + (newRand() & 0x3FF) - 0x200,
                                   ship->ppivot.vy - (newRand() & 0x1FF),
                                   ship->ppivot.vz + (newRand() & 0x3FF) - 0x200, -1);
                        PlayNote(0xA, 0xFF, 0xF, 0xF, 0x100);
                    }
                }
            }
            weapon->count = 0;
            shapes[weapon->shapeNo].used = 0;
            return gfx;
        }
        weapon->pos.vy -= 10;
        weapon->velocity.vx -= weapon->velocity.vx >> 2;
        weapon->velocity.vy -= weapon->velocity.vy >> 2;
        weapon->velocity.vz -= weapon->velocity.vz >> 2;
        weapon->pos.vx += weapon->velocity.vx;
        weapon->pos.vy += weapon->velocity.vy;
        weapon->pos.vz += weapon->velocity.vz;
        weapon->hdg += 256;
        SetSkeletonDirectionHPR(shapes[weapon->shapeNo].shapePtr->skeleton, weapon->hdg, 0, 0);
        SetSkeletonPosition(shapes[weapon->shapeNo].shapePtr->skeleton,
                            weapon->pos.vx >> 1, weapon->pos.vy >> 1, weapon->pos.vz >> 1);
    }

    shapes[weapon->shapeNo].shapePtr->skeleton->update = 1;
    shapes[weapon->shapeNo].shapePtr->skeleton->super = camera->camPos;
    MatrixToN64Mtx(GetSkeletonMatrix(shapes[weapon->shapeNo].shapePtr->skeleton), (N64Mtx *)&g_mtxPool[g_mtxIndex]);
    AddRenderGroup(&weaponRenderList, shapes[weapon->shapeNo].shapePtr->group, &g_mtxPool[g_mtxIndex]);
    g_mtxIndex++;
    return gfx;
}

Gfx *InitShield(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons,
                TrackCamera *camera, Gfx *gfx) {
    s32 i;

    shieldGreen[weapon->owner] = 0;
    shieldRed[weapon->owner] = 0;
    weapon->section = ships[weapon->owner].nearTrkSect;
    for (i = 0; i < 4; i++) {
        if (shapes[i + 40].used == 0) {
            weapon->shapeNo = i + 40;
            shapes[i + 40].used = 1;
            weapon->pos.vx = ships[weapon->owner].ppivot.vx;
            weapon->pos.vy = ships[weapon->owner].ppivot.vy;
            weapon->pos.vz = ships[weapon->owner].ppivot.vz;
            SetSkeletonDirectionHPR(shapes[weapon->shapeNo].shapePtr->skeleton,
                                    ships[weapon->owner].hdg, ships[weapon->owner].pitch, ships[weapon->owner].roll);
            weapon->update = UpdateShield;
            ships[weapon->owner].attr |= 0x4000;
            return gfx;
        }
    }
    weapon->count = 0;
    return gfx;
}

Gfx *UpdateShield(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons,
                  TrackCamera *camera, Gfx *gfx) {
    s32 i;
    s16 primCount;
    s16 fadeOut;
    s16 alpha;
    s16 c0, c1, c2, c3;
    VECTOR unused[4]; /* unused: retail reserves 0x40-byte stack area before spilled locals */
    ShieldPoly poly;
    s16 *coords;

    if (g_multiPlayer) {
        if (shieldSfx == 0 && IS_PLAYER(weapon->owner)) {
            shieldSfx = PlaySfx(2, 0, 0x80, 0, -1);
            if (shieldSfx) {
                SetSfxFreqOffset(shieldSfx, -4.0f);
            }
            shieldActive = 1;
        }
        if (shieldGreen[weapon->owner] > 0) {
            shieldGreen[weapon->owner] -= 10;
        }
        if (shieldRed[weapon->owner] > 0) {
            shieldRed[weapon->owner] -= 10;
        }
        if (shieldGreen[weapon->owner] < 0) {
            shieldGreen[weapon->owner] = 0;
        }
        if (shieldRed[weapon->owner] < 0) {
            shieldRed[weapon->owner] = 0;
        }

        if (ships[weapon->owner].attr & 4) {
            weapon->pos.vx = ships[weapon->owner].ppivot.vx + (ships[weapon->owner].unitVecNose.vx >> 6);
            weapon->pos.vy = ships[weapon->owner].ppivot.vy + (ships[weapon->owner].unitVecNose.vy >> 6);
            weapon->pos.vz = ships[weapon->owner].ppivot.vz + (ships[weapon->owner].unitVecNose.vz >> 6);
        } else {
            weapon->pos.vx = ships[weapon->owner].ppivot.vx;
            weapon->pos.vy = ships[weapon->owner].ppivot.vy;
            weapon->pos.vz = ships[weapon->owner].ppivot.vz;
        }

        fadeOut = 0;
        weapon->count--;
        if (weapon->count < 20) {
            fadeOut = (20 - weapon->count) << 1;
        }
        if (weapon->count == 0 || (ships[weapon->owner].attr & 0x400)) {
            weapon->count = 0;
            if (weapon->owner == ownShip) {
                fadeBlue[0] = 0;
                fadeGreen[0] = 0;
                fadeRed[0] = 0;
            } else if (weapon->owner == ownShip2) {
                fadeBlue[1] = 0;
                fadeGreen[1] = 0;
                fadeRed[1] = 0;
            } else if (weapon->owner == ownShip3) {
                fadeBlue[2] = 0;
                fadeGreen[2] = 0;
                fadeRed[2] = 0;
            } else if (weapon->owner == ownShip4) {
                fadeBlue[3] = 0;
                fadeGreen[3] = 0;
                fadeRed[3] = 0;
            }
            shapes[weapon->shapeNo].used = 0;
            ships[weapon->owner].attr &= ~0x4000;
            if (IS_PLAYER(weapon->owner)) {
                StopSfx(&shieldSfx);
            }
            if (gameType == 3) {
                shieldActive = 0;
            }
        }

        if ((ships[weapon->owner].attr & 4) && IS_PLAYER(weapon->owner)) {
            c0 = (rsin(weapon->count << 7) >> 7) + 64;
            c0 -= fadeOut;
            if (c0 < 0) {
                c0 = 0;
            }
            if (weapon->owner == ownShip) {
                fadeRed[0] = shieldRed[weapon->owner];
                fadeGreen[0] = shieldGreen[weapon->owner];
                fadeBlue[0] = c0;
                fadeActive[0] = 1;
            } else if (weapon->owner == ownShip2) {
                fadeRed[1] = shieldRed[weapon->owner];
                fadeGreen[1] = shieldGreen[weapon->owner];
                fadeBlue[1] = c0;
                fadeActive[1] = 1;
            } else if (weapon->owner == ownShip3) {
                fadeRed[2] = shieldRed[weapon->owner];
                fadeGreen[2] = shieldGreen[weapon->owner];
                fadeBlue[2] = c0;
                fadeActive[2] = 1;
            } else if (weapon->owner == ownShip4) {
                fadeRed[3] = shieldRed[weapon->owner];
                fadeGreen[3] = shieldGreen[weapon->owner];
                fadeBlue[3] = c0;
                fadeActive[3] = 1;
            }
        } else {
            SetSkeletonPosition(shapes[weapon->shapeNo].shapePtr->skeleton,
                                weapon->pos.vx >> 1, weapon->pos.vy >> 1, weapon->pos.vz >> 1);
            SetSkeletonDirectionHPR(shapes[weapon->shapeNo].shapePtr->skeleton,
                                    ships[weapon->owner].hdg, ships[weapon->owner].pitch, ships[weapon->owner].roll);
            poly.prim = shapes[weapon->shapeNo].shapePtr->primitives;
            primCount = shapes[weapon->shapeNo].shapePtr->primitiveCount;
            if (weapon->count < 33) {
                alpha = weapon->count << 2;
                if (alpha < 0) {
                    alpha = 0;
                }
            } else {
                alpha = 128;
            }
            for (i = 0; i < primCount; i++) {
                switch (poly.prim->type) {
                case 6:
                    /* coords holds &coords[0] in $s2 for indices 1..2 while coords[0] uses -0x18($fp) */
                    coords = poly.gt3->coords;
                    c0 = (rsin(weapon->count * poly.gt3->coords[0]) >> 5) + 128;
                    c1 = (rsin(weapon->count * coords[1]) >> 5) + 128;
                    c2 = (rsin(weapon->count * coords[2]) >> 5) + 128;
                    if (c0 > 255) {
                        c0 = 255;
                    }
                    if (c1 > 255) {
                        c1 = 255;
                    }
                    if (c2 > 255) {
                        c2 = 255;
                    }
                    c0 -= fadeOut;
                    c1 -= fadeOut;
                    c2 -= fadeOut;
                    if (c0 < 0) {
                        c0 = 0;
                    }
                    if (c1 < 0) {
                        c1 = 0;
                    }
                    if (c2 < 0) {
                        c2 = 0;
                    }
                    shapes[weapon->shapeNo].shapePtr->vtx[poly.gt3->coords[0]].v.cn[3] = alpha;
                    shapes[weapon->shapeNo].shapePtr->vtx[coords[1]].v.cn[3] = alpha;
                    shapes[weapon->shapeNo].shapePtr->vtx[coords[2]].v.cn[3] = alpha;
                    shapes[weapon->shapeNo].shapePtr->vtx[poly.gt3->coords[0]].v.cn[2] = (u8)c0 >> 1;
                    shapes[weapon->shapeNo].shapePtr->vtx[coords[1]].v.cn[2] = (u8)c1 >> 1;
                    shapes[weapon->shapeNo].shapePtr->vtx[coords[2]].v.cn[2] = (u8)c2 >> 1;
                    shapes[weapon->shapeNo].shapePtr->vtx[poly.gt3->coords[0]].v.cn[1] = shieldGreen[weapon->owner];
                    shapes[weapon->shapeNo].shapePtr->vtx[coords[1]].v.cn[1] = shieldGreen[weapon->owner];
                    shapes[weapon->shapeNo].shapePtr->vtx[coords[2]].v.cn[1] = shieldGreen[weapon->owner];
                    shapes[weapon->shapeNo].shapePtr->vtx[poly.gt3->coords[0]].v.cn[0] = shieldRed[weapon->owner];
                    shapes[weapon->shapeNo].shapePtr->vtx[coords[1]].v.cn[0] = shieldRed[weapon->owner];
                    shapes[weapon->shapeNo].shapePtr->vtx[coords[2]].v.cn[0] = shieldRed[weapon->owner];
                    poly.gt3++;
                    break;
                case 7:
                    coords = poly.g4->coords;
                    c0 = (rsin(weapon->count * poly.g4->coords[0]) >> 5) + 128;
                    c1 = (rsin(weapon->count * coords[1]) >> 5) + 128;
                    c2 = (rsin(weapon->count * coords[2]) >> 5) + 128;
                    c3 = (rsin(weapon->count * coords[3]) >> 5) + 128;
                    if (c0 > 255) {
                        c0 = 255;
                    }
                    if (c1 > 255) {
                        c1 = 255;
                    }
                    if (c2 > 255) {
                        c2 = 255;
                    }
                    if (c3 > 255) {
                        c2 = 255; /* retail bug: clamps c2 instead of c3 */
                    }
                    c0 -= fadeOut;
                    c1 -= fadeOut;
                    c2 -= fadeOut;
                    c3 -= fadeOut;
                    if (c0 < 0) {
                        c0 = 0;
                    }
                    if (c1 < 0) {
                        c1 = 0;
                    }
                    if (c2 < 0) {
                        c2 = 0;
                    }
                    if (c3 < 0) {
                        c3 = 0;
                    }
                    shapes[weapon->shapeNo].shapePtr->vtx[poly.g4->coords[0]].v.cn[3] = alpha;
                    shapes[weapon->shapeNo].shapePtr->vtx[coords[1]].v.cn[3] = alpha;
                    shapes[weapon->shapeNo].shapePtr->vtx[coords[2]].v.cn[3] = alpha;
                    shapes[weapon->shapeNo].shapePtr->vtx[coords[3]].v.cn[3] = alpha;
                    shapes[weapon->shapeNo].shapePtr->vtx[poly.g4->coords[0]].v.cn[2] = (u8)c0 >> 1;
                    shapes[weapon->shapeNo].shapePtr->vtx[coords[1]].v.cn[2] = (u8)c1 >> 1;
                    shapes[weapon->shapeNo].shapePtr->vtx[coords[3]].v.cn[2] = (u8)c2 >> 1;
                    shapes[weapon->shapeNo].shapePtr->vtx[coords[2]].v.cn[2] = (u8)c3 >> 1;
                    shapes[weapon->shapeNo].shapePtr->vtx[poly.g4->coords[0]].v.cn[1] = shieldGreen[weapon->owner];
                    shapes[weapon->shapeNo].shapePtr->vtx[coords[1]].v.cn[1] = shieldGreen[weapon->owner];
                    shapes[weapon->shapeNo].shapePtr->vtx[coords[3]].v.cn[1] = shieldGreen[weapon->owner];
                    shapes[weapon->shapeNo].shapePtr->vtx[coords[2]].v.cn[1] = shieldGreen[weapon->owner];
                    shapes[weapon->shapeNo].shapePtr->vtx[poly.g4->coords[0]].v.cn[0] = shieldRed[weapon->owner];
                    shapes[weapon->shapeNo].shapePtr->vtx[coords[1]].v.cn[0] = shieldRed[weapon->owner];
                    shapes[weapon->shapeNo].shapePtr->vtx[coords[3]].v.cn[0] = shieldRed[weapon->owner];
                    shapes[weapon->shapeNo].shapePtr->vtx[coords[2]].v.cn[0] = shieldRed[weapon->owner];
                    poly.g4 += 1;
                    break;
                }
            }
        }

        if (IS_PLAYER(weapon->owner)) {
            if ((weapon->owner == ownShip && (ships[ownShip].attr & 4)) ||
                (weapon->owner == ownShip2 && (ships[weapon->owner].attr & 4)) ||
                (weapon->owner == ownShip3 && (ships[weapon->owner].attr & 4)) ||
                (weapon->owner == ownShip4 && (ships[weapon->owner].attr & 4))) {
                if (shieldSfx) {
                    SetSfxVolume(shieldSfx, 100);
                }
            } else {
                if (shieldSfx) {
                    SetSfxVolume(shieldSfx, alpha << 1);
                }
            }
        }
    }

    shapes[weapon->shapeNo].shapePtr->skeleton->update = 1;
    shapes[weapon->shapeNo].shapePtr->skeleton->super = camera->camPos;
    MatrixToN64Mtx(GetSkeletonMatrix(shapes[weapon->shapeNo].shapePtr->skeleton), (N64Mtx *)&g_mtxPool[g_mtxIndex]);
    AddRenderGroup(&weaponRenderList, shapes[weapon->shapeNo].shapePtr->group, &g_mtxPool[g_mtxIndex]);
    gDPSetRenderMode(gfx++, G_RM_ZB_OPA_SURF, G_RM_ZB_OPA_SURF2);
    g_mtxIndex++;
    return gfx;
}

Gfx *InitCloak(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons,
               TrackCamera *camera, Gfx *gfx) {
    weapon->update = UpdateCloak;
    ships[weapon->owner].combatAttr |= 0x100;
    if (g_engineHandle) {
        SetSfxVolume(g_engineHandle, 50);
    }
    if (g_turbHandle) {
        SetSfxVolume(g_turbHandle, 50);
    }
    return gfx;
}

Gfx *UpdateCloak(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons,
                 TrackCamera *camera, Gfx *gfx) {
    VECTOR dist;
    VECTOR unused[3]; /* unused: retail reserves 0x30-byte stack area */
    s32 i;
    ShipData *ship;

    if (g_multiPlayer) {
        weapon->count--;
        if (weapon->count == 0) {
            ships[weapon->owner].combatAttr &= ~0x100;
        }
        for (i = 0; i < NO_OF_SHIPS; i++) {
            ship = &ships[i];
            if (CAN_HIT_SHIP(i, ship, weapon)) {
                dist.vx = (ship->ppivot.vx - ships[weapon->owner].ppivot.vx) >> 2;
                dist.vy = (ship->ppivot.vy - ships[weapon->owner].ppivot.vy) >> 2;
                dist.vz = (ship->ppivot.vz - ships[weapon->owner].ppivot.vz) >> 2;
                if (GetMagnitude(dist) < 100) {
                    if (weapon->count == 1) {
                        if (ship->attr & 0x4000) {
                            ships[weapon->owner].attr &= ~0x4000;
                            DamageCollide(22, weapon->owner, weapon);
                            weapon->count = -1;
                            weapon->target = weapon->owner;
                        } else {
                            DamageCollide(22, i, weapon);
                            weapon->count = -1;
                            weapon->target = i;
                            ships[weapon->owner].combatAttr &= ~0x100;
                        }
                    } else {
                        if (ship->stallCount == 0) {
                            AddCloakHitEffect(ship->ppivot.vx, ship->ppivot.vy, ship->ppivot.vz, 4);
                            ship->stallCount = 15;
                        }
                        DamageCollide(19, i, weapon);
                        PlayNote(0xA, 0xFF, 0xF, 0xF, 0x100);
                        if (i == ownShip) {
                            SetRumble(8, 0);
                            SetShake(20, 0);
                        } else if (i == ownShip2) {
                            SetRumble(8, 1);
                            SetShake(20, 1);
                        } else if (i == ownShip3) {
                            SetRumble(8, 2);
                            SetShake(20, 2);
                        } else if (i == ownShip4) {
                            SetRumble(8, 3);
                            SetShake(20, 3);
                        }
                    }
                    return gfx;
                }
            }
        }
    }
    return gfx;
}

void CycleWeaponGrids(ShipData *ships, WeaponGridZone *grids, s32 noGrids) {
    s32 i;
    s32 j;
    Face *facePtr;
    SVector rgb;

    for (i = 0; i < noGrids; i++) {
        facePtr = track->faces + grids[i].section->faceStart;
        while (!(facePtr->flags & TRACK_BASE)) {
            facePtr++;
        }
        for (j = 0; j < NO_OF_SHIPS; j++) {
            if (!(ships[j].combatAttr & 0x8000)) {
                if ((facePtr->flags & 2) && (ships[j].attr & 0x20) &&
                    grids[i].section == ships[j].nearTrkSect && grids[i].count >= 0) {
                    grids[i].count = -10;
                    if (IS_PLAYER(j)) {
                        if (ships[j].weapon == 0) {
                            PlayNote(0x22, 0xC0, 0xF, 0xF, 0x100);
                            if (cheatWeapon) {
                                GetCheatWeapon(ships, j);
                            } else {
                                GetNewWeapon(ships, j);
                            }
                        }
                    } else {
                        GetNewDroneWeapon(ships, j);
                    }
                }
                facePtr++;
                if ((facePtr->flags & 8) && !(ships[j].attr & 0x20) &&
                    grids[i].section == ships[j].nearTrkSect && grids[i].count >= 0) {
                    grids[i].count = -10;
                    if (IS_PLAYER(j)) {
                        if (ships[j].weapon == 0) {
                            PlayNote(0x22, 0xC0, 0xF, 0xF, 0x100);
                            if (cheatWeapon) {
                                GetCheatWeapon(ships, j);
                            } else {
                                GetNewWeapon(ships, j);
                            }
                        }
                    } else {
                        GetNewDroneWeapon(ships, j);
                    }
                }
                facePtr--;
            }
        }

        if (grids[i].count >= 0) {
            rgb.vx = (rsin(grids[i].angle) >> 5) + 128;
            rgb.vy = (rcos(grids[i].angle) >> 5) + 128;
            rgb.vz = (rsin(-grids[i].angle) >> 5) + 128;
            if (rgb.vx > 255) {
                rgb.vx = 255;
            }
            if (rgb.vy > 255) {
                rgb.vy = 255;
            }
            if (rgb.vz > 255) {
                rgb.vz = 255;
            }
            grids[i].r = rgb.vx;
            grids[i].g = rgb.vy;
            grids[i].b = rgb.vz;
        } else {
            grids[i].r = 128;
            grids[i].g = 128;
            grids[i].b = 128;
        }
        if (grids[i].count < 0) {
            grids[i].count++;
        }
        grids[i].angle += 32;
        if (grids[i].attr & 2) {
            facePtr->red = grids[i].r;
            facePtr->green = grids[i].g;
            facePtr->blue = grids[i].b;
        }
        facePtr++;
        if (grids[i].attr & 8) {
            facePtr->red = grids[i].r;
            facePtr->green = grids[i].g;
            facePtr->blue = grids[i].b;
        }
    }
}

void GetFireVector(VECTOR *result, VECTOR target, Face *facePtr, VECTOR shipPos, TrackSection *section) {
    VECTOR facePoint;
    s32 alpha1;
    s32 alpha2;

    GetFaceVertex(section, facePtr, 0, &facePoint);
    target.vx = shipPos.vx + (target.vx >> 6);
    target.vy = shipPos.vy + (target.vy >> 6);
    target.vz = shipPos.vz + (target.vz >> 6);
    alpha1 = PlaneLineCollision(facePoint, target, facePtr->normal);
    alpha2 = PlaneLineCollision(facePoint, shipPos, facePtr->normal);
    alpha1 = (alpha1 * 31) >> 5;
    alpha1 -= alpha2;
    result->vx = target.vx - ((alpha1 * facePtr->normal.vx) >> 12);
    result->vy = target.vy - ((alpha1 * facePtr->normal.vy) >> 12);
    result->vz = target.vz - ((alpha1 * facePtr->normal.vz) >> 12);
    result->vx -= shipPos.vx;
    result->vy -= shipPos.vy;
    result->vz -= shipPos.vz;
}

void Weapon2Mine(WeaponData *weapon, WeaponData *weapons, WeaponShape *shapes, s32 flag) {
    VECTOR dist;
    s32 i;

    for (i = 0; i < NO_OF_WEAPONS; i++) {
        if (weapons[i].type == WEAPON_MINE && weapons[i].count > 0) {
            dist.vx = (weapons[i].pos.vx - weapon->pos.vx) >> 2;
            dist.vy = (weapons[i].pos.vy - weapon->pos.vy) >> 2;
            dist.vz = (weapons[i].pos.vz - weapon->pos.vz) >> 2;
            if (GetMagnitude(dist) < 30) {
                weapons[i].count = 0;
                shapes[weapons[i].shapeNo].used = 0;
                if (flag == 0) {
                    weapon->count = -2;
                    shapes[weapon->shapeNo].used = 0;
                }
                PlayNote(0xB, 0xA0, 0xF, 0xF, 0x100);
                return;
            }
        }
    }
}

s16 GetShipTeam(s32 shipNo) {
    s16 weapon;

    if (g_unk800923F9 && shipNo == ownShip4) {
        shipNo = ownShip3;
    }
    switch (shipNo / 3) {
    case 0:
        weapon = 16;
        break;
    case 1:
        weapon = 15;
        break;
    case 2:
        weapon = 17;
        break;
    case 3:
        weapon = 19;
        break;
    case 4:
        weapon = 18;
        break;
    }
    return weapon;
}

void GetNewWeapon(ShipData *shipList, s32 shipNo) {
    s16 tier = 0;
    u8 alive = 0;
    u8 found = 0;
    s16 weapon = 0;
    s16 pos;
    s16 idx;
    u8 i;
    s16 roll;
    ShipData *ship;

    if ((shipNo != ownShip || pauseRequests[0] == 0) &&
        (frontendOptions_g.numPlayers < 2 || shipNo != ownShip2 || pauseRequests[1] == 0) &&
        (frontendOptions_g.numPlayers < 3 || shipNo != ownShip3 || pauseRequests[2] == 0) &&
        (frontendOptions_g.numPlayers != 4 || shipNo != ownShip4 || pauseRequests[3] == 0) &&
        D_800AA088 != 1) {
        ship = &shipList[shipNo];
        if (shipNo == ownShip) {
            pos = playerPosition;
        } else if (shipNo == ownShip2) {
            pos = player2Position;
        } else if (shipNo == ownShip3) {
            pos = player3Position;
        } else if (shipNo == ownShip4) {
            pos = player4Position;
        }

        if (ship->weapon == 0) {
            if (gameType == 6 && frontendOptions_g.challengeType != 1) {
                if (frontendOptions_g.challengeTable[frontendOptions_g.challengeNo].weaponMask != 0) {
                    if (frontendOptions_g.challengeType == 4 &&
                        (frontendOptions_g.challengeNo == 1 || frontendOptions_g.challengeNo == 2 ||
                         frontendOptions_g.challengeNo == 4)) {
                        weapon = GetShipTeam(shipNo);
                    } else if (shipDamage[shipNo] >= (GetDroneShield(shipNo) / 4) * 3 &&
                               randWep() % 2 == 1 && pos != 1 &&
                               (frontendOptions_g.challengeTable[frontendOptions_g.challengeNo].weaponMask & 0x400)) {
                        weapon = 6;
                    } else {
                        idx = randWep() % 10;
                        while (1) {
                            do {
                                switch (idx) {
                                case 0:
                                    if (frontendOptions_g.challengeTable[frontendOptions_g.challengeNo].weaponMask & 1) {
                                        weapon = 2;
                                        found = 1;
                                    } else {
                                        idx++;
                                    }
                                    break;
                                case 1:
                                    if (frontendOptions_g.challengeTable[frontendOptions_g.challengeNo].weaponMask & 2) {
                                        weapon = 1;
                                        found = 1;
                                    } else {
                                        idx++;
                                    }
                                    break;
                                case 2:
                                    if (frontendOptions_g.challengeTable[frontendOptions_g.challengeNo].weaponMask & 4) {
                                        weapon = 5;
                                        found = 1;
                                    } else {
                                        idx++;
                                    }
                                    break;
                                case 3:
                                    if (frontendOptions_g.challengeTable[frontendOptions_g.challengeNo].weaponMask & 8) {
                                        weapon = 3;
                                        found = 1;
                                    } else {
                                        idx++;
                                    }
                                    break;
                                case 4:
                                    if (frontendOptions_g.challengeTable[frontendOptions_g.challengeNo].weaponMask & 0x10) {
                                        weapon = 14;
                                        found = 1;
                                    } else {
                                        idx++;
                                    }
                                    break;
                                case 5:
                                    if (frontendOptions_g.challengeTable[frontendOptions_g.challengeNo].weaponMask & 0x20) {
                                        weapon = 12;
                                        found = 1;
                                    } else {
                                        idx++;
                                    }
                                    break;
                                case 6:
                                    if (frontendOptions_g.challengeTable[frontendOptions_g.challengeNo].weaponMask & 0x40) {
                                        weapon = GetShipTeam(shipNo);
                                        found = 1;
                                    } else {
                                        idx++;
                                    }
                                    break;
                                case 7:
                                    if (frontendOptions_g.challengeTable[frontendOptions_g.challengeNo].weaponMask & 0x80) {
                                        weapon = 7;
                                        found = 1;
                                    } else {
                                        idx++;
                                    }
                                    break;
                                case 8:
                                    if (frontendOptions_g.challengeTable[frontendOptions_g.challengeNo].weaponMask & 0x100) {
                                        weapon = 9;
                                        found = 1;
                                    } else {
                                        idx++;
                                    }
                                    break;
                                case 9:
                                    if (frontendOptions_g.challengeTable[frontendOptions_g.challengeNo].weaponMask & 0x200) {
                                        weapon = 8;
                                        found = 1;
                                    } else {
                                        idx = 0;
                                    }
                                    break;
                                }
                            } while (!found);
                            if (weapon == ship->lastWeapon) {
                                if (idx == 9) {
                                    idx = 0;
                                } else {
                                    idx++;
                                }
                                found = 0;
                            } else {
                                break;
                            }
                        }
                    }
                }
            } else {
                if (shipDamage[shipNo] >= (GetDroneShield(shipNo) / 4) * 3 &&
                    randWep() % 2 == 1 && pos != 1) {
                    weapon = 6;
                } else {
                    for (i = 0; i < NO_OF_SHIPS; i++) {
                        if (!(ships[i].combatAttr & 0x8000)) {
                            alive++;
                        }
                    }
                    if (attractMode == 2 ||
                        (attractMode == 3 && frontendOptions_g.numPlayers == 2 && frontendOptions_g.aiShipsEnabled == 1) ||
                        (gameType == 6 && frontendOptions_g.challengeType == 1)) {
                        if ((u8)alive >= 7) {
                            if (pos < 4) {
                                tier = 0;
                            } else if (pos > (u8)alive - 3) {
                                tier = 2;
                            } else {
                                tier = 1;
                            }
                        } else if (pos <= (u8)alive / 2) {
                            tier = 0;
                        } else {
                            tier = 2;
                        }
                    } else if (attractMode == 3) {
                        switch ((u8)alive) {
                        case 4:
                            if (pos == 1) {
                                tier = 0;
                            } else if (pos == 2 || pos == 3) {
                                tier = 1;
                            } else {
                                tier = 2;
                            }
                            break;
                        case 3:
                            if (pos == 1) {
                                tier = 0;
                            } else if (pos == 2) {
                                tier = 1;
                            } else {
                                tier = 2;
                            }
                            break;
                        case 2:
                            if (pos == 1) {
                                tier = 0;
                            } else if (pos == 2) {
                                tier = 2;
                            }
                            break;
                        default:
                            tier = 2;
                            break;
                        }
                    }

                    while (1) {
                        roll = randWep() % 100;
                        if (attractMode == 2) {
                            roll -= g_weaponChance1P[tier][0];
                            if (roll < 0) {
                                weapon = 2;
                            } else if ((roll -= g_weaponChance1P[tier][1]) < 0) {
                                weapon = 1;
                            } else if ((roll -= g_weaponChance1P[tier][2]) < 0) {
                                weapon = 5;
                            } else if ((roll -= g_weaponChance1P[tier][3]) < 0) {
                                weapon = 3;
                            } else if ((roll -= g_weaponChance1P[tier][4]) < 0) {
                                weapon = 14;
                            } else if ((roll -= g_weaponChance1P[tier][5]) < 0) {
                                weapon = 12;
                            } else if ((roll -= g_weaponChance1P[tier][6]) < 0) {
                                weapon = GetShipTeam(shipNo);
                            } else if ((roll -= g_weaponChance1P[tier][7]) < 0) {
                                weapon = 7;
                            } else if ((roll -= g_weaponChance1P[tier][8]) < 0) {
                                weapon = 9;
                            } else if ((roll -= g_weaponChance1P[tier][9]) < 0) {
                                weapon = 8;
                            } else {
                                weapon = 8;
                            }
                        } else if (attractMode == 3) {
                            roll -= g_weaponChanceMP[tier][0];
                            if (roll < 0) {
                                weapon = 2;
                            } else if ((roll -= g_weaponChanceMP[tier][1]) < 0) {
                                weapon = 1;
                            } else if ((roll -= g_weaponChanceMP[tier][2]) < 0) {
                                weapon = 5;
                            } else if ((roll -= g_weaponChanceMP[tier][3]) < 0) {
                                weapon = 3;
                            } else if ((roll -= g_weaponChanceMP[tier][4]) < 0) {
                                weapon = 14;
                            } else if ((roll -= g_weaponChanceMP[tier][5]) < 0) {
                                weapon = 12;
                            } else if ((roll -= g_weaponChanceMP[tier][6]) < 0) {
                                weapon = GetShipTeam(shipNo);
                                if (weapon == 15 && frontendOptions_g.numPlayers == 4) {
                                    weapon = 2;
                                } else if (weapon == 17) {
                                    if (g_superWeaponActive1) {
                                        weapon = ship->lastWeapon;
                                    } else if (ship->lastWeapon != 17) {
                                        g_superWeaponActive1 = 1;
                                    }
                                } else if (weapon == 16) {
                                    if (g_superWeaponActive2) {
                                        weapon = ship->lastWeapon;
                                    } else if (ship->lastWeapon != 16) {
                                        g_superWeaponActive2 = 1;
                                    }
                                }
                            } else if ((roll -= g_weaponChanceMP[tier][7]) < 0) {
                                weapon = 7;
                            } else if ((roll -= g_weaponChanceMP[tier][8]) < 0) {
                                weapon = 9;
                            } else if ((roll -= g_weaponChanceMP[tier][9]) < 0) {
                                weapon = 8;
                            } else {
                                weapon = 0;
                            }
                        }
                        if (attractMode == 3) {
                            if (weapon == 8) {
                                if (shieldActive == 0 && ship->lastWeapon != 8) {
                                    shieldActive = 1;
                                    break;
                                }
                            } else if (weapon != ship->lastWeapon) {
                                break;
                            }
                        } else if (weapon != ship->lastWeapon) {
                            break;
                        }
                    }
                }
            }

            ship->weapon = weapon;
            ship->lastWeapon = weapon;
            if ((weapon == 15 || weapon == 16 || weapon == 17 || weapon == 19 || weapon == 18) &&
                !(gameType == 6 && frontendOptions_g.challengeType == 4 &&
                  (frontendOptions_g.challengeNo == 1 || frontendOptions_g.challengeNo == 2 ||
                   frontendOptions_g.challengeNo == 4))) {
                PlayNote(0x3C, 0xFF, 0xF, 0xF, 0x100);
            }
        }
    }
}

void GetNewDroneWeapon(ShipData *shipList, s32 shipNo) {
    s32 alive = 0;
    s16 tier;
    s16 pos;
    u8 i;
    s16 roll;
    s16 weapon;
    ShipData *ship;

    if ((shipNo != ownShip || pauseRequests[0] == 0) &&
        (frontendOptions_g.numPlayers < 2 || shipNo != ownShip2 || pauseRequests[1] == 0) &&
        (frontendOptions_g.numPlayers < 3 || shipNo != ownShip3 || pauseRequests[2] == 0) &&
        (frontendOptions_g.numPlayers != 4 || shipNo != ownShip4 || pauseRequests[3] == 0) &&
        D_800AA088 != 1) {
        ship = &shipList[shipNo];
        if (ship->numToSpeed > 0) {
            ship->weapon = 0;
            ship->lastWeapon = 0;
            return;
        }
        if (!(ships[shipNo].attr & 0x4000)) {
            pos = GetShipPosition(ships, shipNo);
            for (i = 0; i < NO_OF_SHIPS; i++) {
                if (!(ships[i].combatAttr & 0x8000)) {
                    alive++;
                }
            }
            if ((u8)alive >= 7) {
                if (pos < 4) {
                    tier = 0;
                } else if (pos > (u8)alive - 3) {
                    tier = 2;
                } else {
                    tier = 1;
                }
            } else if (pos <= (u8)alive / 2) {
                tier = 0;
            } else {
                tier = 2;
            }

            roll = randWep() % 100;
            if (shipClassIndex == 0) {
                if ((roll -= g_droneWeaponChance0[tier][0]) < 0) {
                    weapon = 2;
                } else if ((roll -= g_droneWeaponChance0[tier][1]) < 0) {
                    weapon = 1;
                } else if ((roll -= g_droneWeaponChance0[tier][2]) < 0) {
                    weapon = (frontendOptions_g.numPlayers == 1) ? 5 : 0;
                } else if ((roll -= g_droneWeaponChance0[tier][3]) < 0) {
                    weapon = 3;
                } else if ((roll -= g_droneWeaponChance0[tier][4]) < 0) {
                    weapon = 0;
                } else if ((roll -= g_droneWeaponChance0[tier][5]) < 0) {
                    weapon = 12;
                } else if ((roll -= g_droneWeaponChance0[tier][6]) < 0) {
                    weapon = 0;
                } else if ((roll -= g_droneWeaponChance0[tier][7]) < 0) {
                    weapon = 0;
                } else if ((roll -= g_droneWeaponChance0[tier][8]) < 0) {
                    weapon = 9;
                } else if ((roll -= g_droneWeaponChance0[tier][9]) < 0) {
                    weapon = 8;
                } else {
                    weapon = 0;
                }
            } else if (shipClassIndex == 1) {
                if ((roll -= g_droneWeaponChance1[tier][0]) < 0) {
                    weapon = 2;
                } else if ((roll -= g_droneWeaponChance1[tier][1]) < 0) {
                    weapon = 1;
                } else if ((roll -= g_droneWeaponChance1[tier][2]) < 0) {
                    weapon = (frontendOptions_g.numPlayers == 1) ? 5 : 0;
                } else if ((roll -= g_droneWeaponChance1[tier][3]) < 0) {
                    weapon = 3;
                } else if ((roll -= g_droneWeaponChance1[tier][4]) < 0) {
                    weapon = 0;
                } else if ((roll -= g_droneWeaponChance1[tier][5]) < 0) {
                    weapon = 12;
                } else if ((roll -= g_droneWeaponChance1[tier][6]) < 0) {
                    weapon = 0;
                } else if ((roll -= g_droneWeaponChance1[tier][7]) < 0) {
                    weapon = 0;
                } else if ((roll -= g_droneWeaponChance1[tier][8]) < 0) {
                    weapon = 9;
                } else if ((roll -= g_droneWeaponChance1[tier][9]) < 0) {
                    weapon = 8;
                } else {
                    weapon = 0;
                }
            } else if (shipClassIndex == 2) {
                if ((roll -= g_droneWeaponChance2[tier][0]) < 0) {
                    weapon = 2;
                } else if ((roll -= g_droneWeaponChance2[tier][1]) < 0) {
                    weapon = 1;
                } else if ((roll -= g_droneWeaponChance2[tier][2]) < 0) {
                    weapon = (frontendOptions_g.numPlayers == 1) ? 5 : 0;
                } else if ((roll -= g_droneWeaponChance2[tier][3]) < 0) {
                    weapon = 3;
                } else if ((roll -= g_droneWeaponChance2[tier][4]) < 0) {
                    weapon = 0;
                } else if ((roll -= g_droneWeaponChance2[tier][5]) < 0) {
                    weapon = 12;
                } else if ((roll -= g_droneWeaponChance2[tier][6]) < 0) {
                    weapon = 0;
                } else if ((roll -= g_droneWeaponChance2[tier][7]) < 0) {
                    weapon = 0;
                } else if ((roll -= g_droneWeaponChance2[tier][8]) < 0) {
                    weapon = 9;
                } else if ((roll -= g_droneWeaponChance2[tier][9]) < 0) {
                    weapon = 8;
                } else {
                    weapon = 0;
                }
            } else if (shipClassIndex == 3) {
                if ((roll -= g_droneWeaponChance3[tier][0]) < 0) {
                    weapon = 2;
                } else if ((roll -= g_droneWeaponChance3[tier][1]) < 0) {
                    weapon = 1;
                } else if ((roll -= g_droneWeaponChance3[tier][2]) < 0) {
                    weapon = (frontendOptions_g.numPlayers == 1) ? 5 : 0;
                } else if ((roll -= g_droneWeaponChance3[tier][3]) < 0) {
                    weapon = 3;
                } else if ((roll -= g_droneWeaponChance3[tier][4]) < 0) {
                    weapon = 0;
                } else if ((roll -= g_droneWeaponChance3[tier][5]) < 0) {
                    weapon = 12;
                } else if ((roll -= g_droneWeaponChance3[tier][6]) < 0) {
                    weapon = 0;
                } else if ((roll -= g_droneWeaponChance3[tier][7]) < 0) {
                    weapon = 0;
                } else if ((roll -= g_droneWeaponChance3[tier][8]) < 0) {
                    weapon = 9;
                } else if ((roll -= g_droneWeaponChance3[tier][9]) < 0) {
                    weapon = 8;
                } else {
                    weapon = 0;
                }
            }
#ifdef VERSION_EU
            /* PAL revision: strip electric bolt in multiplayer and normalize weapons in challenge 1-0. */
            if (frontendOptions_g.numPlayers >= 2 && weapon == 5) {
                weapon = 0;
            }
            if (gameType == 6 && frontendOptions_g.challengeType == 1 && frontendOptions_g.challengeNo == 0 &&
                (weapon == 2 || weapon == 15 || weapon == 7 || weapon == 5)) {
                weapon = 1;
            }
#endif
            ship->weapon = weapon;
            ship->lastWeapon = weapon;
        }
    }
}

Gfx *UpdateRocket(ShipData *ships, WeaponShape *shapes, WeaponData *weapon, WeaponData *weapons,
                  TrackCamera *camera, Gfx *gfx) {
    VECTOR dist;
    VECTOR facePoint;
    VECTOR nextPoint;
    VECTOR path;
    VECTOR offset;
    VECTOR v1;
    VECTOR v2;
    VECTOR unusedVec; /* unused: retail reserves 0x10-byte stack slot */
    TrackSection *nextSection;
    Face *facePtr;
    Face *nextFace;
    s32 alpha;
    s32 height;
    s32 i;
    ShipData *ship;

    if (g_multiPlayer) {
        weapon->section = FindNearestSection(weapon->section, weapon->pos);
        nextSection = weapon->section->nextSection.ptr;
        facePtr = track->faces + weapon->section->faceStart;
        nextFace = track->faces + nextSection->faceStart;
        while (!(facePtr->flags & TRACK_BASE)) {
            facePtr++;
        }
        while (!(nextFace->flags & TRACK_BASE)) {
            nextFace++;
        }

        weapon->velocity.vx += weapon->accel.vx;
        weapon->velocity.vy += weapon->accel.vy;
        weapon->velocity.vz += weapon->accel.vz;
        weapon->velocity.vx -= weapon->velocity.vx >> 5;
        weapon->velocity.vy -= weapon->velocity.vy >> 5;
        weapon->velocity.vz -= weapon->velocity.vz >> 5;
        weapon->pos.vx += weapon->velocity.vx;
        weapon->pos.vz += weapon->velocity.vz;

        GetFaceVertex(weapon->section, facePtr, 0, &facePoint);
        GetFaceVertex(nextSection, nextFace, 0, &nextPoint);
        path.vx = nextPoint.vx - facePoint.vx;
        path.vy = nextPoint.vy - facePoint.vy;
        path.vz = nextPoint.vz - facePoint.vz;
        offset.vx = weapon->pos.vx - facePoint.vx;
        offset.vz = weapon->pos.vz - facePoint.vz;
        alpha = SquareRoot0(path.vx * path.vx + path.vz * path.vz);
        alpha = (((path.vx * offset.vx + path.vz * offset.vz) / alpha) << 16) / alpha;
        height = nextPoint.vy - facePoint.vy;
        height = (height * alpha) >> 16;
        weapon->pos.vy = facePoint.vy + height - 300;
        ratan2(path.vy, SquareRoot0(path.vx * path.vx + path.vz * path.vz));

        v1.vx = facePoint.vx;
        v1.vy = facePoint.vy;
        v1.vz = facePoint.vz;
        facePtr++;
        GetFaceVertex(weapon->section, facePtr, 0, &v2);
        path.vx = v2.vx - v1.vx;
        path.vz = v2.vz - v1.vz;
        offset.vx = weapon->pos.vx - v1.vx;
        offset.vz = weapon->pos.vz - v1.vz;
        alpha = SquareRoot0(path.vx * path.vx + path.vz * path.vz);
        alpha = (((path.vx * offset.vx + path.vz * offset.vz) / alpha) << 16) / alpha;
        height = v2.vy - v1.vy;
        alpha = (height * alpha) >> 16;
        height = alpha - (height >> 1);
        weapon->pos.vy += height;
        weapon->pitch = 0;
        weapon->count--;
        if (weapon->count == 0) {
            shapes[weapon->shapeNo].used = 0;
        }
        SetSkeletonPosition(shapes[weapon->shapeNo].shapePtr->skeleton,
                            weapon->pos.vx >> 1, weapon->pos.vy >> 1, weapon->pos.vz >> 1);
    }

    shapes[weapon->shapeNo].shapePtr->skeleton->update = 1;
    shapes[weapon->shapeNo].shapePtr->skeleton->super = camera->camPos;
    MatrixToN64Mtx(GetSkeletonMatrix(shapes[weapon->shapeNo].shapePtr->skeleton), (N64Mtx *)&g_mtxPool[g_mtxIndex]);
    AddRenderGroup(&weaponRenderList, shapes[weapon->shapeNo].shapePtr->group, &g_mtxPool[g_mtxIndex]);
    g_mtxIndex++;

    if (g_multiPlayer) {
        for (i = 0; i < NO_OF_SHIPS; i++) {
            ship = &ships[i];
            if (CAN_HIT_SHIP(i, ship, weapon)) {
                dist.vx = (ship->ppivot.vx - weapon->pos.vx) >> 2;
                dist.vy = (ship->ppivot.vy - weapon->pos.vy) >> 2;
                dist.vz = (ship->ppivot.vz - weapon->pos.vz) >> 2;
                if (GetMagnitude(dist) < 100) {
                    if (!(ship->attr & 0x4000) && !(ship->combatAttr & 0x100)) {
                        if (!IS_PLAYER(i)) {
                            FlipShip(ship);
                            ship->speed >>= 5;
                            ship->vroll += newRand() % 250;
                            ship->ahdg = (newRand() - 16384) >> 1;
                        } else {
                            ship->vpivot.vx -= (ship->vpivot.vx * 3) >> 2;
                            ship->vpivot.vy -= (ship->vpivot.vy * 3) >> 2;
                            ship->vpivot.vz -= (ship->vpivot.vz * 3) >> 2;
                            ship->vroll += newRand() % 250;
                            ship->ahdg = (newRand() - 16384) >> 1;
                            if (i == ownShip) {
                                SetRumble(8, 0);
                                SetShake(20, 0);
                            }
                            if (i == ownShip2) {
                                SetRumble(8, 1);
                                SetShake(20, 1);
                            }
                            if (i == ownShip3) {
                                SetRumble(8, 2);
                                SetShake(20, 2);
                            }
                            if (i == ownShip4) {
                                SetRumble(8, 3);
                                SetShake(20, 3);
                            }
                        }
                    }
                    DamageCollide(1, i, weapon);
                    weapon->count = -1;
                    weapon->target = i;
                    shapes[weapon->shapeNo].used = 0;
                    if (IS_PLAYER(i)) {
                        PlayNote(0xA, 0xFF, 0xF, 0xF, 0x100);
                    } else {
                        PlayNote(0xA, 0xFF, 0xF, 0xF, 0x100);
                    }
                    return gfx;
                }
            }
        }

        facePtr = track->faces + weapon->section->faceStart;
        if (!(weapon->section->flags & 1)) {
            for (i = 0; i < weapon->section->faceCount; i++) {
                GetFaceVertex(weapon->section, facePtr, 0, &facePoint);
                if (PlaneLineCollision(facePoint, weapon->pos, facePtr->normal) <= 0 &&
                    !(facePtr->flags & TRACK_BASE)) {
                    weapon->count = -2;
                    shapes[weapon->shapeNo].used = 0;
                    PlayNote(0xB, 0xA0, 0xF, 0xF, 0x100);
                    return gfx;
                }
                facePtr++;
            }
        }
        Weapon2Mine(weapon, weapons, shapes, 0);
    }
    return gfx;
}

void GetWeaponDamage(s32 *damage, s32 kind) {
    s32 idx = 0;

    switch (kind) {
    case 1:
        idx = 1;
        break;
    case 12:
        idx = 5;
        break;
    case 14:
        idx = 4;
        break;
    case 2:
        idx = 0;
        break;
    case 3:
        idx = 3;
        break;
    case 15:
        idx = 6;
        break;
    case 5:
        idx = 2;
        break;
    case 18:
        idx = 7;
        break;
    case 19:
        idx = 8;
        break;
    case 23:
        idx = 9;
        break;
    case 24:
        idx = 10;
        break;
    case 25:
        idx = 11;
        break;
    case 17:
        if (g_raceClass == 0) {
            idx = 12;
        }
        if (g_raceClass == 1) {
            idx = 13;
        }
        if (g_raceClass == 2) {
            idx = 14;
        }
        if (g_raceClass == 3) {
            idx = 15;
        }
        break;
    }
    *damage = g_weaponDamageTable[idx];
}

void ScaleChallengeDamage(s32 *damage, s32 kind) {
    switch (kind) {
    case 1:
    case 2:
    case 3:
    case 5:
    case 12:
    case 14:
    case 15:
    case 17:
    case 18:
    case 19:
    case 22:
    case 23:
    case 24:
    case 25:
        *damage *= 2;
        break;
    }
}

void DamageCollide(s32 damageType, s32 shipNo, WeaponData *weapon) {
    s32 damage;

    if (ships[shipNo].unkFC == 0 &&
        (shipNo != ownShip || pauseRequests[0] == 0) &&
        (frontendOptions_g.numPlayers < 2 || shipNo != ownShip2 || pauseRequests[1] == 0) &&
        (frontendOptions_g.numPlayers < 3 || shipNo != ownShip3 || pauseRequests[2] == 0) &&
        (frontendOptions_g.numPlayers != 4 || shipNo != ownShip4 || pauseRequests[3] == 0) &&
        D_800AA088 != 1) {
        switch (damageType) {
        case 0x80:
            damage = -10;
            break;
        case 21:
            damage = -50;
            break;
        case 0x81:
        case 0x82:
            damage = 10;
            break;
        case 22:
            damage = 10000;
            break;
        case 1:
        case 2:
        case 3:
        case 5:
        case 12:
        case 14:
        case 15:
        case 17:
        case 18:
        case 19:
        case 23:
        case 24:
        case 25:
            GetWeaponDamage(&damage, damageType);
            break;
        }

        if (damageType == 0x80) {
            damage = g_pitHealTable[g_raceClass];
            if ((frontendOptions_g.numPlayers == 1 && shipNo != ownShip) ||
                (frontendOptions_g.numPlayers == 2 && shipNo != ownShip && shipNo != ownShip2) ||
                (frontendOptions_g.numPlayers == 3 &&
                 shipNo != ownShip && shipNo != ownShip2 && shipNo != ownShip3) ||
                (frontendOptions_g.numPlayers == 4 &&
                 shipNo != ownShip && shipNo != ownShip2 && shipNo != ownShip3 && shipNo != ownShip4)) {
                f32 scale = (f32)g_pitHealScale[g_raceClass];
                scale = (f32)(scale / 100.0);
                damage = (f32)damage * scale;
            }
        }

        if (shipNo == ownShip && damageType != 0x80) {
            if (gameType != 1 || shipNo != g_demoShip) {
                g_perfectLap[0] = 0;
            }
        } else if (shipNo == ownShip2 && damageType != 0x80) {
            if (gameType != 1 || shipNo != g_demoShip) {
                g_perfectLap[1] = 0;
            }
        } else if (shipNo == ownShip3 && damageType != 0x80) {
            if (gameType != 1 || shipNo != g_demoShip) {
                g_perfectLap[1] = 0;
            }
        } else if (shipNo == ownShip4 && damageType != 0x80) {
            if (gameType != 1 || shipNo != g_demoShip) {
                g_perfectLap[1] = 0;
            }
        }

        if ((ships[shipNo].attr & 0x4000) || (ships[shipNo].combatAttr & 0x100) ||
            gameType == 1 || (gameType == 6 && frontendOptions_g.challengeType == 2)) {
            if (damage > 0) {
                damage = 0;
            }
        }

        if (D_80094F7E || cheatSuperCombo) {
            if (gameType == 0 || gameType == 3) {
                if (weapon != NULL && IS_PLAYER(weapon->owner) && frontendOptions_g.superCombo) {
                    ScaleChallengeDamage(&damage, damageType);
                }
            } else if (gameType == 6) {
                if ((frontendOptions_g.challengeType == 4 ||
                     (frontendOptions_g.challengeType == 3 && frontendOptions_g.challengeNo == 6)) &&
                    shipNo != ownShip) {
                    ScaleChallengeDamage(&damage, damageType);
                }
            }
        }

        if (damageType == 18) {
            s32 maxShield = GetDroneShield(shipNo);
            shipDamage[shipNo] = (shipDamage[shipNo] + 9 >= maxShield) ? (maxShield + 1) : (maxShield - 9);
        } else {
            shipDamage[shipNo] += damage;
        }
        if (shipDamage[shipNo] < 0) {
            shipDamage[shipNo] = 0;
        }
        if (cheatDamage && shipNo == ownShip) {
            shipDamage[shipNo] = 0;
        }

        if (shipDamage[shipNo] >= GetDroneShield(shipNo)) {
            if (shipDeathCount[shipNo] == -1) {
                shipDamage[shipNo] = GetDroneShield(shipNo);
                GetDroneShield(shipNo);
                if (IS_PLAYER(shipNo)) {
                    if (g_weaponCharge[shipNo] != 0) {
                        if (g_chargeSfxHandle) {
                            StopSfx(&g_chargeSfxHandle);
                        }
                        g_superWeaponActive2 = 0;
                        g_weaponCharge[shipNo] = 0;
                    }
                    if (D_800AA088 != 1) {
                        if (frontendOptions_g.numPlayers == 1) {
                            shipDeathCount[shipNo] = DEATH_DELAY_1P;
                        } else {
                            shipDeathCount[shipNo] = DEATH_DELAY_MP;
                        }
                        if (shipNo == ownShip) {
                            CameraUpdate = UpdateCameraAttractDestroyed;
                            ships[shipNo].attr &= ~4;
                        } else if (shipNo == ownShip2) {
                            CameraUpdate2 = UpdateCameraAttractDestroyed;
                            ships[shipNo].attr &= ~4;
                        }
                        if (shipNo == ownShip3) {
                            CameraUpdate3 = UpdateCameraAttractDestroyed;
                            ships[shipNo].attr &= ~4;
                        }
                        if (shipNo == ownShip4) {
                            CameraUpdate4 = UpdateCameraAttractDestroyed;
                            ships[shipNo].attr &= ~4;
                        }
                    }
                } else {
                    shipDeathCount[shipNo] = DEATH_DELAY_MP;
                }
                if (weapon == NULL) {
                    if (damageType == 0x82) {
                        ships[g_lastRammingShip].unkF0++;
                    }
                } else {
                    ships[weapon->owner].unkF0++;
                }
            }
        }

        if (damageType != 0x80 && damageType != 0x81) {
            shieldGreen[shipNo] = 255;
            shieldRed[shipNo] = 255;
        }
        if (damageType == 0x81) {
            shieldRed[shipNo] = 255;
        }

        if (shipNo == ownShip) {
            if (previousPlayerDamage < (GetDroneShield(shipNo) / 4) * 3 &&
                shipDamage[ownShip] >= (GetDroneShield(shipNo) / 4) * 3) {
                PlayNote(0x14, 0x1FE, 0xF, 0xF, 0x100);
            }
            previousPlayerDamage = shipDamage[ownShip];
        }
        if (shipNo == ownShip2) {
            if (previousPlayerDamage2 < (GetDroneShield(shipNo) / 4) * 3 &&
                shipDamage[ownShip2] >= (GetDroneShield(shipNo) / 4) * 3) {
                PlayNote(0x14, 0x1FE, 0xF, 0xF, 0x100);
            }
            previousPlayerDamage2 = shipDamage[ownShip2];
        }
        if (shipNo == ownShip3) {
            if (previousPlayerDamage3 < (GetDroneShield(shipNo) / 4) * 3 &&
                shipDamage[ownShip3] >= (GetDroneShield(shipNo) / 4) * 3) {
                PlayNote(0x14, 0x1FE, 0xF, 0xF, 0x100);
            }
            previousPlayerDamage3 = shipDamage[ownShip3];
        }
        if (shipNo == ownShip4) {
            if (previousPlayerDamage4 < (GetDroneShield(shipNo) / 4) * 3 &&
                shipDamage[ownShip4] >= (GetDroneShield(shipNo) / 4) * 3) {
                PlayNote(0x14, 0x1FE, 0xF, 0xF, 0x100);
            }
            previousPlayerDamage4 = shipDamage[ownShip4];
        }
    }
}

void CheckPitShips(void) {
    s32 i;
    s32 j;
    s32 inPit = 0;
    TrackSection *section;
    Face *face;
    Face *nextFace;
    Face *prevFace;
    s32 nextPit;
    s32 prevPit;

    for (i = 0; i < NO_OF_SHIPS; i++) {
        if (!(ships[i].combatAttr & 0x8000)) {
            section = ships[i].nearTrkSect;
            nextFace = track->faces + section->nextSection.ptr->faceStart;
            nextPit = nextFace->flags & 0x40;
            if (!nextPit) {
                nextPit = (nextFace + 1)->flags & 0x40;
            }
            prevFace = track->faces + section->prevSection.ptr->faceStart;
            prevPit = prevFace->flags & 0x40;
            if (!prevPit) {
                prevPit = (prevFace + 1)->flags & 0x40;
            }
            face = track->faces + section->faceStart;
            if (prevPit && nextPit && (face->flags & 0x40)) {
                DamageCollide(0x80, i, NULL);
                if (IS_PLAYER(i)) {
                    inPit = 1;
                    if (g_pitSfxHandle == 0 && pauseData == -1 && frontendOptions_g.numPlayers == 1) {
                        g_pitSfxHandle = PlaySfx(0x25, 0x81, 0x80, 0, -1);
                    }
                }
            } else {
                face++;
                if (prevPit && nextPit && (face->flags & 0x40)) {
                    DamageCollide(0x80, i, NULL);
                    for (j = 0; j < 8; j++) {
                        AddPitSpark(&ships[i].ppivot, 2);
                    }
                    if (IS_PLAYER(i)) {
                        inPit = 1;
                        if (g_pitSfxHandle == 0 && pauseData == -1 && frontendOptions_g.numPlayers == 1) {
                            g_pitSfxHandle = PlaySfx(0x25, 0x81, 0x80, 0, -1);
                        }
                    }
                } else {
                    if (frontendOptions_g.numPlayers == 1 && i == ownShip && g_pitSfxHandle != 0) {
                        StopSfx(&g_pitSfxHandle);
                    }
                }
            }
        } else {
            if (frontendOptions_g.numPlayers == 1 && i == ownShip && g_pitSfxHandle != 0) {
                StopSfx(&g_pitSfxHandle);
            }
        }
    }

    if (frontendOptions_g.numPlayers != 1) {
        if (inPit) {
            if (g_pitSfxHandle == 0) {
                g_pitSfxHandle = PlaySfx(0x25, 0x81, 0x80, 0, -1);
            }
        } else if (g_pitSfxHandle != 0) {
            StopSfx(&g_pitSfxHandle);
        }
    }
}

void UpdateFadeScreen(s32 player) {
    g_fadeColour[player].r =
        (fadeRed[player] + fadeAmount[player] < 256) ? (u8)(fadeRed[player] + fadeAmount[player]) : 255;
    g_fadeColour[player].g =
        (fadeGreen[player] + fadeAmount[player] + cheatfade + g_pitFade < 256)
            ? (u8)(fadeGreen[player] + fadeAmount[player] + cheatfade + g_pitFade)
            : 255;
    g_fadeColour[player].b =
        (fadeBlue[player] + fadeAmount[player] + g_pitFade < 256)
            ? (u8)(fadeBlue[player] + fadeAmount[player] + g_pitFade)
            : 255;

    if (fadeAmount[player] > 0) {
        fadeAmount[player] -= 20;
    }
    if (fadeAmount[player] < 0) {
        fadeAmount[player] = 0;
    }
    if (cheatfade > 0) {
        cheatfade -= 10;
    }
    if (cheatfade < 0) {
        cheatfade = 0;
    }
    if (g_pitFade > 0) {
        g_pitFade -= 25;
    }
    if (g_pitFade < 50) {
        g_pitFade = 0;
    }
}

Gfx *DrawScreenOverlay(s32 player, s32 fullScreen, Gfx *gfx, u8 isDim) {
    s32 layout;
    s32 posX, posY;
    s32 scaleX, scaleY;

    layout = frontendOptions_g.screenLayout[player];
    if (fullScreen) {
        layout = 0;
    }
    switch (layout) {
#ifdef VERSION_EU
    /* PAL 320x288 split-screen overlay coordinates and Sprite2D scale factors (vs NTSC 320x240 below). */
    case 0:
        posX = 0;
        posY = 0;
        scaleX = 0xC8;
        scaleY = 0xC8;
        break;
    case 1:
        posX = 0;
        posY = 0;
        scaleX = 0x19E;
        scaleY = 0xC8;
        break;
    case 2:
        posX = 0x284;
        posY = 0;
        scaleX = 0x19E;
        scaleY = 0xC8;
        break;
    case 3:
        posX = 0;
        posY = 0;
        scaleX = 0xC8;
        scaleY = 0x1C2;
        break;
    case 4:
        posX = 0;
        posY = 0x1E0;
        scaleX = 0xC8;
        scaleY = 0x1C2;
        break;
    case 5:
        posX = 0;
        posY = 0;
        scaleX = 0x19E;
        scaleY = 0x1C2;
        break;
    case 6:
        posX = 0x284;
        posY = 0;
        scaleX = 0x19E;
        scaleY = 0x1C2;
        break;
    case 7:
        posX = 0;
        posY = 0x1E0;
        scaleX = 0x19E;
        scaleY = 0x1C2;
        break;
    case 8:
        posX = 0x284;
        posY = 0x1E0;
        scaleX = 0x19E;
        scaleY = 0x1C2;
        break;
#else
    case 0:
        posX = 0x28;
        posY = 0x3C;
        scaleX = 0xD2;
        scaleY = 0xFA;
        break;
    case 1:
        posX = 0x28;
        posY = 0x3C;
        scaleX = 0x1BC;
        scaleY = 0xFA;
        break;
    case 2:
        posX = 0x284;
        posY = 0x3C;
        scaleX = 0x1BC;
        scaleY = 0xFA;
        break;
    case 3:
        posX = 0x28;
        posY = 0x3C;
        scaleX = 0xD2;
        scaleY = 0x1FE;
        break;
    case 4:
        posX = 0x28;
        posY = 0x1E0;
        scaleX = 0xD2;
        scaleY = 0x1FE;
        break;
    case 5:
        posX = 0x28;
        posY = 0x3C;
        scaleX = 0x1B8;
        scaleY = 0x202;
        break;
    case 6:
        posX = 0x284;
        posY = 0x3C;
        scaleX = 0x1B8;
        scaleY = 0x202;
        break;
    case 7:
        posX = 0x28;
        posY = 0x1DE;
        scaleX = 0x1B8;
        scaleY = 0x202;
        break;
    case 8:
        posX = 0x284;
        posY = 0x1DE;
        scaleX = 0x1B8;
        scaleY = 0x202;
        break;
#endif
    }

    gDPPipeSync(gfx++);
    if (isDim) {
        if (fullScreen) {
            gDPSetPrimColor(gfx++, 0, 0, 6, 6, 6, 90);
        } else {
            gDPSetPrimColor(gfx++, 0, 0, 6, 6, 6, 180);
        }
    } else {
        gDPSetPrimColor(gfx++, 0, 0, g_fadeColour[player].r, g_fadeColour[player].g, g_fadeColour[player].b, 100);
    }
    gSPTexture(gfx++, 0, 0, 0, 0, G_OFF);
    gDPSetCombineMode(gfx++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gSPSprite2DBase(gfx++, OS_K0_TO_PHYSICAL(&g_logoSprite));
    if (fullScreen) {
        gSPSprite2DScaleFlip(gfx++, 0xC8, 0xC8, 0, 0);
        gSPSprite2DDraw(gfx++, 0, 0);
    } else {
        gSPSprite2DScaleFlip(gfx++, scaleX, scaleY, 0, 0);
        gSPSprite2DDraw(gfx++, posX, posY);
    }
    gSPTexture(gfx++, 0, 0, 0, 0, G_ON);
    return gfx;
}

void FlipShip(ShipData *ship) {
    Face *facePtr;
    SVector normal;

    facePtr = track->faces + ship->nearTrkSect->faceStart;
    while (!(facePtr->flags & TRACK_BASE)) {
        facePtr++;
    }
    facePtr++;
    normal.vx = (facePtr->normal.vx * 2) >> 1;
    normal.vy = (facePtr->normal.vy * 2) >> 1;
    normal.vz = (facePtr->normal.vz * 2) >> 1;
    ship->vpivot.vx += normal.vx * 2;
    ship->vpivot.vy += normal.vy * 2;
    ship->vpivot.vz += normal.vz * 2;
}

void GetCheatWeapon(ShipData *ships, s32 shipNo) {
    ShipData *ship = &ships[shipNo];
    s16 weapon;

    if (frontendOptions_g.unk58 && ship->weapon == 0) {
        do {
            switch (newRand() & 7) {
            case 0:
                weapon = 2;
                break;
            case 1:
                weapon = 1;
                break;
            case 2:
                weapon = 5;
                break;
            case 3:
                weapon = 3;
                break;
            case 4:
                weapon = 14;
                break;
            case 5:
                weapon = 12;
                break;
            case 6:
                weapon = GetShipTeam(shipNo);
                break;
            default:
                weapon = 2;
                break;
            }
        } while (weapon == ship->lastWeapon);
        ship->weapon = weapon;
        ship->lastWeapon = weapon;
    }
}

s32 randWep(void) {
    seedWep = (seedWep >> 1) ^ (seedWep * 8);
    return seedWep & 0x7FFF;
}
