/*
 * hud.c — In-race HUD, speedometer, lap/checkpoint timers, and results screen.
 *
 * Renders the 1P-4P HUD (speedometer, shield/thrust bars, weapon icons, lap
 * times, target warnings, checkpoint banners) and the post-race summary. PAL
 * Europe adjusts 50 Hz timers and vertical coordinates for the 256-line display.
 */
#include "hud.h"

static const char rcsid[] = "$Revision: 1.17 $";

/* ============================================================================
 * HUD texture & speedometer initialisation
 * ============================================================================ */

void InitHud(void) {
    s32 start;

    g_startupData->current = 3;
    g_font16TexStart = TextureTableCount;
    LoadCmpFiles(WADLoadCompressedTextureSequence(g_startupData));
    g_startupData->current = 10;
    g_font12TexStart = TextureTableCount;
    LoadCmpFiles(WADLoadCompressedTextureSequence(g_startupData));
    g_startupData->current = 11;
    g_font8TexStart = TextureTableCount;
    LoadCmpFiles(WADLoadCompressedTextureSequence(g_startupData));
    start = TextureTableCount;
    g_startupData->current = 12;
    LoadCmpFiles(WADLoadCompressedTextureSequence(g_startupData));
    guSprite2DInit(&g_delIconSprite, (u8 *)g_textureTable[start]->texels - K0BASE,
                   (u8 *)g_textureTable[start]->palette - K0BASE, g_textureTable[start]->width,
                   g_textureTable[start]->width, g_textureTable[start]->height, G_IM_FMT_CI, G_IM_SIZ_4b, 0, 0);
    guSprite2DInit(&g_endIconSprite, (u8 *)g_textureTable[start + 1]->texels - K0BASE,
                   (u8 *)g_textureTable[start + 1]->palette - K0BASE, g_textureTable[start + 1]->width,
                   g_textureTable[start + 1]->width, g_textureTable[start + 1]->height, G_IM_FMT_CI, G_IM_SIZ_4b, 0, 0);
}

#define SPRITE_INIT(sprite, tex, image, palette, bitSize) \
    guSprite2DInit(sprite, image, palette, (tex)->width, (tex)->width, (tex)->height, G_IM_FMT_CI, bitSize, 0, 0)

void InitSpeedoSprites(uSprite *sprites, uSprite *mpSprites) {
    s32 bitSize;

    g_startupImage->current += 4;

    g_hudTempTex = g_textureTable[g_hudTexStart + 8];
    bitSize = g_hudTempTex->type != 0;
    SPRITE_INIT(&sprites[0], g_hudTempTex, (u8 *)g_hudTempTex->texels - K0BASE, (u8 *)g_hudTempTex->palette - K0BASE, bitSize);
    g_hudTempTex = g_textureTable[g_hudTexStart + 9];
    bitSize = g_hudTempTex->type != 0;
    SPRITE_INIT(&sprites[1], g_hudTempTex, (u8 *)g_hudTempTex->texels - K0BASE, (u8 *)g_hudTempTex->palette - K0BASE, bitSize);
    g_speedo1PThrustTex = g_textureTable[g_hudTexStart + 5];
    bitSize = g_speedo1PThrustTex->type != 0;
    SPRITE_INIT(&sprites[2], g_speedo1PThrustTex, g_speedo1PThrustTex->texels, g_speedo1PThrustTex->palette, bitSize);
    g_speedo1PShieldTex = g_textureTable[g_hudTexStart + 6];
    bitSize = g_speedo1PShieldTex->type != 0;
    SPRITE_INIT(&sprites[3], g_speedo1PShieldTex, g_speedo1PShieldTex->texels, g_speedo1PShieldTex->palette, bitSize);
    g_speedo1PWeaponTex = g_textureTable[g_hudTexStart + 7];
    bitSize = g_speedo1PWeaponTex->type != 0;
    SPRITE_INIT(&sprites[4], g_speedo1PWeaponTex, g_speedo1PWeaponTex->texels, g_speedo1PWeaponTex->palette, bitSize);

    g_hudTempTex = g_textureTable[g_hudTexStart + 3];
    bitSize = g_hudTempTex->type != 0;
    SPRITE_INIT(&mpSprites[0], g_hudTempTex, (u8 *)g_hudTempTex->texels - K0BASE, (u8 *)g_hudTempTex->palette - K0BASE, bitSize);
    g_hudTempTex = g_textureTable[g_hudTexStart + 4];
    bitSize = g_hudTempTex->type != 0;
    SPRITE_INIT(&mpSprites[1], g_hudTempTex, (u8 *)g_hudTempTex->texels - K0BASE, (u8 *)g_hudTempTex->palette - K0BASE, bitSize);

    g_speedoMPThrustTex = g_textureTable[g_hudTexStart + 0];
    bitSize = g_speedoMPThrustTex->type != 0;
    SPRITE_INIT(&mpSprites[2], g_speedoMPThrustTex, g_speedoMPThrustTex->texels, g_speedoMPThrustTex->palette, bitSize);
    SPRITE_INIT(&g_mpThrustSpriteP2, g_speedoMPThrustTex, g_speedoMPThrustTex->texels, g_speedoMPThrustTex->palette, bitSize);
    SPRITE_INIT(&g_mpThrustSpriteP3, g_speedoMPThrustTex, g_speedoMPThrustTex->texels, g_speedoMPThrustTex->palette, bitSize);
    SPRITE_INIT(&g_mpThrustSpriteP4, g_speedoMPThrustTex, g_speedoMPThrustTex->texels, g_speedoMPThrustTex->palette, bitSize);

    g_speedoMPShieldTex = g_textureTable[g_hudTexStart + 1];
    bitSize = g_speedoMPShieldTex->type != 0;
    SPRITE_INIT(&mpSprites[3], g_speedoMPShieldTex, g_speedoMPShieldTex->texels, g_speedoMPShieldTex->palette, bitSize);
    SPRITE_INIT(&g_mpShieldSpriteP2, g_speedoMPShieldTex, g_speedoMPShieldTex->texels, g_speedoMPShieldTex->palette, bitSize);
    SPRITE_INIT(&g_mpShieldSpriteP3, g_speedoMPShieldTex, g_speedoMPShieldTex->texels, g_speedoMPShieldTex->palette, bitSize);
    SPRITE_INIT(&g_mpShieldSpriteP4, g_speedoMPShieldTex, g_speedoMPShieldTex->texels, g_speedoMPShieldTex->palette, bitSize);

    g_speedoMPWeaponTex = g_textureTable[g_hudTexStart + 2];
    bitSize = g_speedoMPWeaponTex->type != 0;
    SPRITE_INIT(&mpSprites[4], g_speedoMPWeaponTex, g_speedoMPWeaponTex->texels, g_speedoMPWeaponTex->palette, bitSize);
    SPRITE_INIT(&g_mpWeaponSpriteP2, g_speedoMPWeaponTex, g_speedoMPWeaponTex->texels, g_speedoMPWeaponTex->palette, bitSize);
    SPRITE_INIT(&g_mpWeaponSpriteP3, g_speedoMPWeaponTex, g_speedoMPWeaponTex->texels, g_speedoMPWeaponTex->palette, bitSize);
    SPRITE_INIT(&g_mpWeaponSpriteP4, g_speedoMPWeaponTex, g_speedoMPWeaponTex->texels, g_speedoMPWeaponTex->palette, bitSize);
}

#define G_CC_DECALRGB_PRIMA 0, 0, 0, TEXEL0, TEXEL0, 0, PRIMITIVE, 0

/* PAL (50 Hz, 256-line framebuffer) shifts speedometer coordinates vertically/horizontally. */
#ifdef VERSION_EU
#define SPEEDO_1P_X         0x35C
#define SPEEDO_1P_Y         0x329
#define SPEEDO_LEFT_X       0x1BC
#define SPEEDO_RIGHT_X      0x415
#define SPEEDO_TOP_Y        0x167
#define SPEEDO_BOTTOM_Y     0x342
#else
#define SPEEDO_1P_X         0x35C
#define SPEEDO_1P_Y         0x2FD
#define SPEEDO_LEFT_X       0x1BC
#define SPEEDO_RIGHT_X      0x3FD
#define SPEEDO_TOP_Y        0x16F
#define SPEEDO_BOTTOM_Y     0x308
#endif

Gfx *FillSpeedo(uSprite *sprites1P, uSprite *spritesMP, ShipData *shipList, ConfigData *config,
                   s32 pauseState, s32 shipNo, u32 layout, Gfx *glistp) {
    s32 classSpeeds[4] = { 125, 167, 209, 250 };
    s32 unused[4] = { 990, 1370, 1800, 2100 };
    uSprite *sprites;
    uSprite *shieldSprite;
    N64Texture *tex;
    s32 thrustWidth;
    s32 speedWidth;
    s32 speed;
    f32 ratio;
    s32 x0, y0, x1, y1, x2, y2, x3, y3, x4, y4;

    sprites = spritesMP;
    if (layout == 0) {
        sprites = sprites1P;
    }

    switch (layout) {
        case 0:
            x0 = SPEEDO_1P_X; y0 = SPEEDO_1P_Y;
            x1 = SPEEDO_1P_X; y1 = SPEEDO_1P_Y;
            x3 = SPEEDO_1P_X; y3 = SPEEDO_1P_Y;
            x2 = SPEEDO_1P_X; y2 = SPEEDO_1P_Y;
            x4 = SPEEDO_1P_X; y4 = SPEEDO_1P_Y;
            break;
        case 1:
            x0 = SPEEDO_LEFT_X; y0 = SPEEDO_BOTTOM_Y;
            x1 = SPEEDO_LEFT_X; y1 = SPEEDO_BOTTOM_Y;
            x3 = SPEEDO_LEFT_X; y3 = SPEEDO_BOTTOM_Y;
            x2 = SPEEDO_LEFT_X; y2 = SPEEDO_BOTTOM_Y;
            x4 = SPEEDO_LEFT_X; y4 = SPEEDO_BOTTOM_Y;
            break;
        case 3:
            x0 = SPEEDO_RIGHT_X; y0 = SPEEDO_TOP_Y;
            x1 = SPEEDO_RIGHT_X; y1 = SPEEDO_TOP_Y;
            x3 = SPEEDO_RIGHT_X; y3 = SPEEDO_TOP_Y;
            x2 = SPEEDO_RIGHT_X; y2 = SPEEDO_TOP_Y;
            x4 = SPEEDO_RIGHT_X; y4 = SPEEDO_TOP_Y;
            break;
        case 5:
            x0 = SPEEDO_LEFT_X; y0 = SPEEDO_TOP_Y;
            x1 = SPEEDO_LEFT_X; y1 = SPEEDO_TOP_Y;
            x3 = SPEEDO_LEFT_X; y3 = SPEEDO_TOP_Y;
            x2 = SPEEDO_LEFT_X; y2 = SPEEDO_TOP_Y;
            x4 = SPEEDO_LEFT_X; y4 = SPEEDO_TOP_Y;
            break;
        case 6:
            x0 = SPEEDO_RIGHT_X; y0 = SPEEDO_TOP_Y;
            x1 = SPEEDO_RIGHT_X; y1 = SPEEDO_TOP_Y;
            x3 = SPEEDO_RIGHT_X; y3 = SPEEDO_TOP_Y;
            x2 = SPEEDO_RIGHT_X; y2 = SPEEDO_TOP_Y;
            x4 = SPEEDO_RIGHT_X; y4 = SPEEDO_TOP_Y;
            break;
        case 7:
            x0 = SPEEDO_LEFT_X; y0 = SPEEDO_BOTTOM_Y;
            x1 = SPEEDO_LEFT_X; y1 = SPEEDO_BOTTOM_Y;
            x3 = SPEEDO_LEFT_X; y3 = SPEEDO_BOTTOM_Y;
            x2 = SPEEDO_LEFT_X; y2 = SPEEDO_BOTTOM_Y;
            x4 = SPEEDO_LEFT_X; y4 = SPEEDO_BOTTOM_Y;
            break;
        case 2:
        case 4:
        case 8:
            x0 = SPEEDO_RIGHT_X; y0 = SPEEDO_BOTTOM_Y;
            x1 = SPEEDO_RIGHT_X; y1 = SPEEDO_BOTTOM_Y;
            x3 = SPEEDO_RIGHT_X; y3 = SPEEDO_BOTTOM_Y;
            x2 = SPEEDO_RIGHT_X; y2 = SPEEDO_BOTTOM_Y;
            x4 = SPEEDO_RIGHT_X; y4 = SPEEDO_BOTTOM_Y;
            break;
    }

    gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&sprites[0]));
    gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
    gSPSprite2DDraw(glistp++, x0, y0);
    gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&sprites[1]));
    gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
    gSPSprite2DDraw(glistp++, x1, y1);

    if (layout == 0) {
        tex = g_speedo1PWeaponTex;
    } else {
        tex = g_speedoMPWeaponTex;
    }
    thrustWidth = tex->width * shipList[shipNo].thrust / shipList[shipNo].maxThrust;
    if (thrustWidth >= tex->width) {
        thrustWidth = tex->width;
    }

    gDPPipeSync(glistp++);
    gDPSetPrimColor(glistp++, 0, 0, 255, 255, 255, 150);
    gDPSetCombineMode(glistp++, G_CC_DECALRGB_PRIMA, G_CC_DECALRGB_PRIMA);

    if (layout != 0) {
        if (shipNo == ownShip) {
            sprites[4].s.SubImageWidth = thrustWidth;
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&sprites[4]));
        } else if (shipNo == ownShip2) {
            g_mpWeaponSpriteP2.s.SubImageWidth = thrustWidth;
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_mpWeaponSpriteP2));
        } else if (shipNo == ownShip3) {
            g_mpWeaponSpriteP3.s.SubImageWidth = thrustWidth;
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_mpWeaponSpriteP3));
        } else if (shipNo == ownShip4) {
            g_mpWeaponSpriteP4.s.SubImageWidth = thrustWidth;
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_mpWeaponSpriteP4));
        }
    } else {
        sprites[4].s.SubImageWidth = thrustWidth;
        gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&sprites[4]));
    }
    gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
    gSPSprite2DDraw(glistp++, x2, y2);

    gDPPipeSync(glistp++);
    gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);

    if (CameraUpdate == UpdateCameraAttractInternal) {
        speed = shipList[shipNo].speed * 7;
    } else {
        speed = shipList[shipNo].speed;
    }
    if (layout == 0) {
        tex = g_speedo1PThrustTex;
    } else {
        tex = g_speedoMPThrustTex;
    }
    speedWidth = speed / classSpeeds[config->shipClass];
    if (speedWidth > tex->width) {
        speedWidth = tex->width;
    }

    if (layout != 0) {
        if (shipNo == ownShip) {
            sprites[2].s.SubImageWidth = speedWidth;
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&sprites[2]));
        } else if (shipNo == ownShip2) {
            g_mpThrustSpriteP2.s.SubImageWidth = speedWidth;
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_mpThrustSpriteP2));
        } else if (shipNo == ownShip3) {
            g_mpThrustSpriteP3.s.SubImageWidth = speedWidth;
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_mpThrustSpriteP3));
        } else if (shipNo == ownShip4) {
            g_mpThrustSpriteP4.s.SubImageWidth = speedWidth;
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_mpThrustSpriteP4));
        }
    } else {
        sprites[2].s.SubImageWidth = speedWidth;
        gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&sprites[2]));
    }
    gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
    gSPSprite2DDraw(glistp++, x3, y3);

    /* Dead texture selection left over before hardcoded 42/85px shield bar widths */
    if (layout == 0) {
        tex = g_speedo1PShieldTex;
    } else {
        tex = g_speedoMPShieldTex;
    }

    if (layout != 0) {
        if (shipNo == ownShip) {
            shieldSprite = &sprites[3];
        } else if (shipNo == ownShip2) {
            shieldSprite = &g_mpShieldSpriteP2;
        } else if (shipNo == ownShip3) {
            shieldSprite = &g_mpShieldSpriteP3;
        } else if (shipNo == ownShip4) {
            shieldSprite = &g_mpShieldSpriteP4;
        }
    } else {
        shieldSprite = &sprites[3];
    }

    ratio = GetDroneShield(shipNo) - shipDamage[shipNo];
    ratio /= GetDroneShield(shipNo);
    if (frontendOptions_g.numPlayers == 1) {
        shieldSprite->s.SubImageWidth = ratio * 85.0f;
    } else {
        shieldSprite->s.SubImageWidth = ratio * 42.0f;
    }

    if (pauseState != -1) {
        gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(shieldSprite));
        gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
        gSPSprite2DDraw(glistp++, x4, y4);
    } else if (GetDroneShield(shipNo) - shipDamage[shipNo] > GetDroneShield(shipNo) / 4) {
        gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(shieldSprite));
        gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
        gSPSprite2DDraw(glistp++, x4, y4);
    } else if (g_shieldFlashFrame % 8 < 5) {
        gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(shieldSprite));
        gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
        gSPSprite2DDraw(glistp++, x4, y4);
    }
    g_shieldFlashFrame++;
    return glistp;
}

/* ============================================================================
 * HUD time, text, number & sprite-font rendering
 *
 * PAL runs at 50 Hz (5 frames/tenth, 50 frames/sec); NTSC runs at 60 Hz (6/60).
 * MAX_TIMER_FRAMES caps the checkpoint countdown at 9m 59.9s (599.9s).
 * ============================================================================ */

#ifdef VERSION_EU
#define FRAMES_PER_TENTH    5
#define FRAMES_PER_SEC      50
#define MAX_TIMER_FRAMES    0x752F
#else
#define FRAMES_PER_TENTH    6
#define FRAMES_PER_SEC      60
#define MAX_TIMER_FRAMES    0x8C9F
#endif

Gfx *AddTime(s32 time, s32 x, s32 y, s32 size, s32 colour, u8 flags, Gfx *glistp) {
    s32 digits[20];
    s32 minutes;
    s32 seconds;
    s32 tenths;
    s16 length;
    s32 i;
    s32 idx;
    u16 width;
    HudGlyph *font;
    HudGlyph *glyph;

    width = 0;
    length = 0;

    switch (size) {
        case 16:
            font = g_font16;
            break;
        case 12:
            font = g_font12;
            break;
        case 8:
            font = g_font8;
            break;
        case 3:
            font = g_font3;
            break;
    }

    if (colour != 1) {
        gDPPipeSync(glistp++);
        gDPSetPrimColor(glistp++, 0, 0, 255, 255, 0, 0);
        gDPSetCombineMode(glistp++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);
    } else {
        gDPPipeSync(glistp++);
        gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    }

    if (time < 0) {
        switch (size) {
            case 12:
                width = g_font12[39].width;
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_font12[39].sprite));
                break;
            case 8:
                width = g_font12[40].width;
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_font12[40].sprite));
                break;
            case 16:
                width = g_font16[12].width;
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_font16[12].sprite));
                break;
        }
        gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
        gSPSprite2DDraw(glistp++, x << 2, y << 2);
        x += width;
        time = -time;
    }

    tenths = (time / FRAMES_PER_TENTH) % 10;
    time = (time / FRAMES_PER_TENTH) / 10;
    seconds = time % 60;

    if (flags & 1) {
        minutes = time / 60;
        digits[0] = minutes % 10;
    } else {
        minutes = time / 60;
        digits[0] = minutes % 10;
        if (minutes > 9) {
            minutes /= 10;
            digits[1] = minutes % 10;
        } else {
            digits[1] = 0;
        }
        length = 1;
    }

    if (!(flags & 2)) {
        for (i = length; i >= 0; i--) {
            if (size == 16) {
                idx = digits[i];
            } else {
                idx = digits[i] + 26;
            }
            width = font[idx].width;
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&font[idx].sprite));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, x << 2, y << 2);
            x += width;
            g_spriteCount++;
        }
    }

    if (!(flags & 2)) {
        if (size == 16) {
            width = font[10].width;
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&font[10].sprite));
        } else {
            width = font[36].width;
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&font[36].sprite));
        }
        gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
        gSPSprite2DDraw(glistp++, x << 2, y << 2);
        x += width;
        g_spriteCount++;
    }

    digits[0] = seconds % 10;
    if (seconds > 9) {
        seconds /= 10;
        digits[1] = seconds % 10;
    } else {
        digits[1] = 0;
    }

    for (i = 1; i >= 0; i--) {
        if (size == 16) {
            idx = digits[i];
            width = font[idx].width;
        } else {
            idx = digits[i] + 26;
            width = font[idx].width;
        }
        glyph = &font[idx];
        gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&glyph->sprite));
        gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
        gSPSprite2DDraw(glistp++, x << 2, y << 2);
        x += width;
        g_spriteCount++;
    }

    if (size == 16) {
        width = font[11].width;
        gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&font[11].sprite));
    } else {
        width = font[37].width;
        gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&font[37].sprite));
    }
    gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
    gSPSprite2DDraw(glistp++, x << 2, y << 2);
    x += width;
    g_spriteCount++;

    font = g_font8;
    if (size != 8) {
        size = 8;
        y += 8;
    }

    if (size == 16) {
        glyph = &font[tenths];
        gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&glyph->sprite));
    } else {
        glyph = &font[tenths + 26];
        gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&glyph->sprite));
    }
    gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
    gSPSprite2DDraw(glistp++, x << 2, y << 2);
    g_spriteCount++;
    return glistp;
}

void TimeToString(s32 time, s32 flags, char *str) {
    s32 minutes;
    s32 seconds;
    s32 tenths;

    tenths = (time / FRAMES_PER_TENTH) % 10;
    time = (time / FRAMES_PER_TENTH) / 10;
    seconds = time % 60;
    minutes = time / 60;

    if (flags & 1) {
        sprintf(str, "%d:%.2d.%.1d", minutes, seconds, tenths);
    } else {
        sprintf(str, "%.2d:%.2d.%.1d", minutes, seconds, tenths);
    }
}

Gfx *AddText(char *text, s32 x, s32 y, s32 size, s32 colour, Gfx *glistp) {
    u8 letter;
    s16 glyph;
    u16 width;
    HudGlyph *font;

    switch (size) {
        case 16:
            font = g_font16;
            __assert("FALSE", "hud.c", 1396);
            break;
        case 12:
            font = g_font12;
            break;
        case 8:
            font = g_font8;
            break;
        case 3:
            font = g_font3;
            break;
    }

    if (colour == 0) {
        gDPPipeSync(glistp++);
        gDPSetPrimColor(glistp++, 0, 0, 255, 255, 0, 0);
        gDPSetCombineMode(glistp++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);
    } else if (colour == 2) {
        gDPPipeSync(glistp++);
        gDPSetPrimColor(glistp++, 0, 0, 25, 25, 25, 75);
        gDPSetCombineMode(glistp++, G_CC_DECALRGB_PRIMA, G_CC_DECALRGB_PRIMA);
    } else {
        gDPPipeSync(glistp++);
        gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    }

    while (1) {
        letter = *text++;
        if (letter != ' ') {
            if (letter == '\0') {
                g_textEndX = x;
                return glistp;
            }
            if (letter >= '0' && letter <= '9') {
                glyph = letter - '0' + 26;
                if (size == 3) {
                    glyph--;
                }
            } else {
                glyph = letter - 'A';
            }
            if (letter == ':') {
                x += 2;
                glyph = 36;
            }
            if (size == 12) {
                if (letter == '!') {
                    glyph = 38;
                } else if (letter == '-') {
                    glyph = 39;
                }
            }
            width = font[glyph].width;
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&font[glyph].sprite));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, x << 2, y << 2);
            x += width;
            g_spriteCount++;
        } else {
            x += 8;
        }
    }
}

Gfx *AddNumber(s32 num, s32 x, s32 y, s32 size, s32 colour, Gfx *glistp) {
    s32 digits[12];
    s32 length;
    s32 i;
    u16 width;
    HudGlyph *font;
    HudGlyph *glyph;

    width = 0;

    switch (size) {
        case 16:
            font = g_font16;
            break;
        case 12:
            font = g_font12;
            break;
        case 8:
            font = g_font8;
            break;
        case 3:
            font = g_font3;
            break;
    }

    if (colour != 1) {
        gDPPipeSync(glistp++);
        gDPSetPrimColor(glistp++, 0, 0, 255, 255, 0, 0);
        gDPSetCombineMode(glistp++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);
    } else {
        gDPPipeSync(glistp++);
        gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    }

    length = 0;
    if (num < 0) {
        switch (size) {
            case 12:
                width = g_font12[39].width;
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_font12[39].sprite));
                break;
            case 8:
                width = g_font12[40].width;
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_font12[40].sprite));
                break;
            case 16:
                width = g_font16[12].width;
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_font16[12].sprite));
                break;
        }
        gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
        gSPSprite2DDraw(glistp++, x << 2, y << 2);
        x += width;
        num = -num;
    }

    for (length = 0; length < 12; length++) {
        digits[length] = num % 10;
        num /= 10;
        if (num == 0) {
            break;
        }
    }

    for (i = length; i >= 0; i--) {
        s32 idx;
        if (size == 16) {
            idx = digits[i];
        } else {
            idx = digits[i] + 26;
        }
        width = font[idx].width;
        gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&font[idx].sprite));
        gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
        gSPSprite2DDraw(glistp++, x << 2, y << 2);
        x += width;
        g_spriteCount++;
    }

    g_textEndX = x;
    return glistp;
}

void NumberToString(s32 num, char *str) {
    s32 i;

    for (i = 0; i < 9; i++) {
        str[i] = (num % 10) + '0';
        num /= 10;
        if (num == 0) {
            break;
        }
    }
    str[i + 1] = '\0';
}

Gfx *AddJustifyNumber(s32 num, s32 x, s32 y, s32 size, s32 colour, Gfx *glistp) {
    s32 digits[12];
    s32 length;
    s32 i;
    s32 neg;
    u16 width;
    HudGlyph *font;

    width = 0;
    neg = 0;

    switch (size) {
        case 16:
            font = g_font16;
            break;
        case 12:
            font = g_font12;
            break;
        case 8:
            font = g_font8;
            break;
        case 3:
            font = g_font3;
            break;
    }

    if (colour != 1) {
        gDPPipeSync(glistp++);
        gDPSetPrimColor(glistp++, 0, 0, 255, 255, 0, 0);
        gDPSetCombineMode(glistp++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);
    } else {
        gDPPipeSync(glistp++);
        gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    }

    length = 0;
    if (num < 0) {
        switch (size) {
            case 12:
                width = g_font12[39].width;
                break;
            case 8:
                width = g_font12[40].width;
                break;
            case 16:
                width = g_font16[12].width;
                break;
        }
        neg = 1;
        x -= width;
        num = -num;
    }

    for (length = 0; length < 5; length++) {
        digits[length] = num % 10;
        num /= 10;
        if (num == 0) {
            break;
        }
    }

    for (i = length; i >= 0; i--) {
        s32 idx;
        if (size == 16) {
            idx = digits[i];
        } else {
            idx = digits[i] + 26;
        }
        width = font[idx].width;
        x -= width;
    }

    if (neg) {
        switch (size) {
            case 12:
                width = g_font12[39].width;
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_font12[39].sprite));
                break;
            case 8:
                width = g_font12[40].width;
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_font12[40].sprite));
                break;
            case 16:
                width = g_font16[12].width;
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_font16[12].sprite));
                break;
        }
        gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
        gSPSprite2DDraw(glistp++, x << 2, y << 2);
        x += width;
    }

    for (i = length; i >= 0; i--) {
        s32 idx;
        if (size == 16) {
            idx = digits[i];
        } else {
            idx = digits[i] + 26;
        }
        width = font[idx].width;
        gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&font[idx].sprite));
        gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
        gSPSprite2DDraw(glistp++, x << 2, y << 2);
        x += width;
        g_spriteCount++;
    }

    return glistp;
}

Gfx *AddDelEnd(u8 dir, s32 x, s32 y, s32 size, s32 colour, Gfx *glistp) {
    if (colour != 1) {
        gDPPipeSync(glistp++);
        gDPSetPrimColor(glistp++, 0, 0, 255, 255, 0, 0);
        gDPSetCombineMode(glistp++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);
    } else {
        gDPPipeSync(glistp++);
        gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    }

    if (dir != 0) {
        gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_endIconSprite));
    } else {
        gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_delIconSprite));
    }
    gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
    gSPSprite2DDraw(glistp++, x << 2, y << 2);
    return glistp;
}

void InitFontSize(u8 size) {
    s16 *table;
    s16 count;
    s16 i;
    s16 j;
    s16 entry;
    s16 row;
    s16 xPos;
    s16 yPos;
    s16 w;

    switch (size) {
        case 3:
            table = g_font3Widths;
            count = 32;
            break;
        case 8:
            table = g_font8Widths;
            count = 40;
            break;
        case 12:
            table = g_font12Widths;
            count = 42;
            break;
        case 16:
            table = g_font16Widths;
            count = 13;
            break;
    }

    for (i = 0; i < count; i++) {
        entry = table[i];
        row = entry / 100;
        yPos = row * size;
        xPos = 0;
        for (j = 0; j < i; j++) {
            if (table[j] / 100 == row) {
                xPos += table[j] % 100;
            }
        }
        switch (size) {
            case 3:
                g_font3[i].x = xPos;
                g_font3[i].y = 64;
                w = entry % 100;
                g_font3[i].width = w + 1;
                guSprite2DInit(&g_font3[i].sprite,
                               (u8 *)g_textureTable[g_font3TexStart + row]->texels - K0BASE,
                               (u8 *)g_textureTable[g_font3TexStart + row]->palette - K0BASE,
                               g_textureTable[g_font3TexStart + row]->width, w, 12,
                               G_IM_FMT_CI, G_IM_SIZ_4b, g_font3[i].x, 0);
                break;
            case 8:
                g_font8[i].x = xPos;
                g_font8[i].y = yPos + 100;
                w = entry % 100;
                g_font8[i].width = w;
                guSprite2DInit(&g_font8[i].sprite,
                               (u8 *)g_textureTable[g_font12TexStart]->texels - K0BASE,
                               (u8 *)g_textureTable[g_font12TexStart]->palette - K0BASE,
                               g_textureTable[g_font12TexStart]->width, w, size,
                               G_IM_FMT_CI, G_IM_SIZ_4b, xPos, yPos);
                break;
            case 12:
                g_font12[i].x = xPos;
                g_font12[i].y = 64;
                w = entry % 100;
                g_font12[i].width = w + 1;
                guSprite2DInit(&g_font12[i].sprite,
                               (u8 *)g_textureTable[g_font16TexStart + row]->texels - K0BASE,
                               (u8 *)g_textureTable[g_font16TexStart + row]->palette - K0BASE,
                               g_textureTable[g_font16TexStart + row]->width, w, 12,
                               G_IM_FMT_CI, G_IM_SIZ_4b, g_font12[i].x, 0);
                break;
            case 16:
                g_font16[i].x = xPos;
                g_font16[i].y = yPos;
                if (i == 12) {
                    g_font16[i].width = (entry % 100) + 1;
                } else {
                    g_font16[i].width = (entry % 100) - 1;
                }
                guSprite2DInit(&g_font16[i].sprite,
                               (u8 *)g_textureTable[g_font8TexStart]->texels - K0BASE,
                               (u8 *)g_textureTable[g_font8TexStart]->palette - K0BASE,
                               g_textureTable[g_font8TexStart]->width, (s16)(entry % 100), 16,
                               G_IM_FMT_CI, G_IM_SIZ_4b, xPos, 0);
                break;
        }
    }
}

Gfx *CentreText(char *text, s32 x, s32 y, s32 size, s32 colour, Gfx *glistp) {
    char *scan;
    u8 letter;
    s16 glyph;
    s16 totalWidth;
    u16 width;
    HudGlyph *font;

    totalWidth = 0;

    switch (size) {
        case 16:
            font = g_font16;
            __assert("FALSE", "hud.c", 2048);
            break;
        case 12:
            font = g_font12;
            break;
        case 8:
            font = g_font8;
            break;
        case 3:
            font = g_font3;
            break;
    }

    if (colour == 0) {
        gDPPipeSync(glistp++);
        gDPSetPrimColor(glistp++, 0, 0, 255, 255, 0, 0);
        gDPSetCombineMode(glistp++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);
    } else if (colour == 2) {
        gDPPipeSync(glistp++);
        gDPSetPrimColor(glistp++, 0, 0, 25, 25, 25, 75);
        gDPSetCombineMode(glistp++, G_CC_DECALRGB_PRIMA, G_CC_DECALRGB_PRIMA);
    } else {
        gDPPipeSync(glistp++);
        gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    }

    scan = text;
    while ((letter = *scan) != '\0') {
        if (letter >= '0' && letter <= '9') {
            glyph = letter - '0' + 26;
        } else {
            glyph = letter - 'A';
        }
        if (letter != ' ') {
            if (letter == '!') {
                totalWidth += g_font12[38].width;
            } else if (letter == '-') {
                totalWidth += g_font12[39].width;
            } else if (letter == '.') {
                totalWidth += font[37].width;
            } else if (letter == ':') {
                totalWidth += font[36].width;
            } else {
                totalWidth += font[glyph].width;
            }
        } else {
            totalWidth += 8;
        }
        scan++;
    }

    x = 160 - (totalWidth >> 1);
    while (1) {
        letter = *text++;
        if (letter != ' ') {
            if (letter == '\0') {
                g_textEndX = x;
                return glistp;
            }
            if (letter >= '0' && letter <= '9') {
                glyph = letter - '0' + 26;
            } else {
                glyph = letter - 'A';
            }
            if (letter == '!') {
                width = g_font12[38].width;
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_font12[38].sprite));
            } else if (letter == '-') {
                width = g_font12[39].width;
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_font12[39].sprite));
            } else if (letter == '.') {
                width = font[37].width;
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&font[37].sprite));
            } else if (letter == ':') {
                width = font[36].width;
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&font[36].sprite));
            } else {
                width = font[glyph].width;
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&font[glyph].sprite));
            }
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, x << 2, y << 2);
            x += width;
            g_spriteCount++;
        } else {
            x += 8;
        }
    }
}

Gfx *CentreTextAt(char *text, s32 x, s32 y, s32 size, s32 colour, Gfx *glistp) {
    char *scan;
    u8 letter;
    s16 glyph;
    s16 totalWidth;
    u16 width;
    HudGlyph *font;

    totalWidth = 0;

    switch (size) {
        case 16:
            font = g_font16;
            __assert("FALSE", "hud.c", 2249);
            break;
        case 12:
            font = g_font12;
            break;
        case 8:
            font = g_font8;
            break;
        case 3:
            font = g_font3;
            break;
    }

    if (colour == 0) {
        gDPPipeSync(glistp++);
        gDPSetPrimColor(glistp++, 0, 0, 255, 255, 0, 0);
        gDPSetCombineMode(glistp++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);
    } else if (colour == 2) {
        gDPPipeSync(glistp++);
        gDPSetPrimColor(glistp++, 0, 0, 25, 25, 25, 75);
        gDPSetCombineMode(glistp++, G_CC_DECALRGB_PRIMA, G_CC_DECALRGB_PRIMA);
    } else {
        gDPPipeSync(glistp++);
        gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    }

    scan = text;
    while ((letter = *scan) != '\0') {
        if (letter >= '0' && letter <= '9') {
            glyph = letter - '0' + 26;
        } else {
            glyph = letter - 'A';
        }
        if (letter != ' ') {
            if (letter == '!') {
                totalWidth += g_font12[38].width;
            } else if (letter == '-') {
                totalWidth += g_font12[39].width;
            } else if (letter == '.') {
                totalWidth += font[37].width;
            } else if (letter == ':') {
                totalWidth += font[36].width;
            } else {
                totalWidth += font[glyph].width;
            }
        } else {
            totalWidth += 8;
        }
        scan++;
    }

    x -= totalWidth >> 1;
    while (1) {
        letter = *text++;
        if (letter != ' ') {
            if (letter == '\0') {
                g_textEndX = x;
                return glistp;
            }
            if (letter >= '0' && letter <= '9') {
                glyph = letter - '0' + 26;
            } else {
                glyph = letter - 'A';
            }
            if (letter == '!') {
                width = g_font12[38].width;
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_font12[38].sprite));
            } else if (letter == '-') {
                width = g_font12[39].width;
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_font12[39].sprite));
            } else if (letter == '.') {
                width = font[37].width;
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&font[37].sprite));
            } else if (letter == ':') {
                width = font[36].width;
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&font[36].sprite));
            } else {
                width = font[glyph].width;
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&font[glyph].sprite));
            }
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, x << 2, y << 2);
            x += width;
            g_spriteCount++;
        } else {
            x += 8;
        }
    }
}

/* ============================================================================
 * In-race player HUD, position ranking, post-race summary & checkpoint timer
 *
 * PAL adjusts HUD vertical/horizontal layout for the taller 256-line display.
 * ============================================================================ */

#ifdef VERSION_EU
#define HUD_DEMO_Y          0x0A
#define HUD_PRESS_START_Y   0xDD
#define HUD_LAP_REC_X       0x14
#define HUD_LAP_TIMES_Y_OFF 0
#define HUD_LAP_LBL_X       0x16
#define HUD_LAP_C_X         0x14
#define HUD_LAP_CUR_X       0x20
#define HUD_TARGET_X        0x15
#define HUD_DESTROY_NUM_X   0x44
#define HUD_OF_X            0xFD
#define HUD_OF_Y            0x1E
#define HUD_TOTAL_SHIPS_X   0x113
#define HUD_TOTAL_SHIPS_Y   0x16
#define HUD_MUSIC_Y         0xE6
#else
#define HUD_DEMO_Y          0x15
#define HUD_PRESS_START_Y   0xD3
#define HUD_LAP_REC_X       0x16
#define HUD_LAP_TIMES_Y_OFF 11
#define HUD_LAP_LBL_X       0x18
#define HUD_LAP_C_X         0x18
#define HUD_LAP_CUR_X       0x22
#define HUD_TARGET_X        0x16
#define HUD_DESTROY_NUM_X   0x45
#define HUD_OF_X            0xF7
#define HUD_OF_Y            0x27
#define HUD_TOTAL_SHIPS_X   0x10D
#define HUD_TOTAL_SHIPS_Y   0x1F
#define HUD_MUSIC_Y         0xD9
#endif

Gfx *DrawPlayerHud(ShipData *shipList, uSprite *sprites1P, uSprite *spritesMP, s32 pauseChoice,
                   ConfigData *config, ScoreData *score, s32 player, u32 layout, Gfx *glistp) {
    char unused[16];
    char musicBuf[80];
#ifdef VERSION_EU
    s16 lapTimesY[4] = { 204, 192, 180, 168 };
#else
    s16 lapTimesY[4] = { 180, 170, 160, 150 };
#endif
    char lapLabelBuf[8];
    char autopilotBuf[8];
    char lapOfBuf[272];
    char timeBuf[8];
    char targetBuf[16];
    s32 *timer;
    s16 curLap;
    s32 shipNo;
    s32 timeSpriteX, timeSpriteY;
    s32 posSpriteX, posSpriteY;
    s32 posNumX, posNumY;
    s32 timerX, timerY;
    s32 lockX, lockY;
    s32 wrongWayX, wrongWayY;
    s32 autopilotX, autopilotY;
    s32 disengagingX, disengagingY;
    s32 apCountX, apCountY;
    s32 elimNumX, elimNumY;
    s32 elimIconX, elimIconY;
    s32 perfectLapX, perfectLapY;
    s32 mpLapX, mpLapY;
    s32 mpLapOfX, mpLapOfY;
    s32 dangerY, dangerX;
    s32 critShieldX, critShieldY;
    s32 showGhostTime;
    s32 lapRecTime;
    s16 distDelta;
    s16 i;
    s16 j;
    s16 l;
    s16 m;
    u8 k;
    s16 pos;
    s32 activeShips;
    s16 apCount;
    s32 trackIdx;
    u8 targetLevel;
    char *trackName;

    showGhostTime = 0;
    shipNo = frontendOptions_g.ownShips[player - 1];

    switch (layout) {
#ifdef VERSION_EU
        case 0:
            timeSpriteX = 0x50;   timeSpriteY = 0x29;
            posSpriteX = 0x38A;   posSpriteY = 0x29;
            posNumX = 0xFB;       posNumY = 0x16;
            timerX = 0x14;        timerY = 0x16;
            lockX = 0x217;        lockY = 0xE6;
            wrongWayX = 0x68;     wrongWayY = 0x54;
            autopilotX = 0x72;    autopilotY = 0x6D;
            disengagingX = 0x67;  disengagingY = 0x79;
            apCountX = 0x9B;      apCountY = 0x79;
            elimNumX = 0x121;     elimNumY = 0x4A;
            elimIconX = 0x122;    elimIconY = 0x46;
            perfectLapX = 0x6A;   perfectLapY = 0x54;
            dangerX = 0x0F;       dangerY = 0x5E;
            critShieldX = 0x2D;   critShieldY = 0x8A;
            break;
        case 1:
            timeSpriteX = 0x4C;   timeSpriteY = 0x4D;
            posSpriteX = 0x224;   posSpriteY = 0x4D;
            posNumX = 0x9C;       posNumY = 0x05;
            timerX = 0x0F;        timerY = 0x05;
            lockX = 0xFD;         lockY = 0x53;
            wrongWayX = 0x40;     wrongWayY = 0x64;
            autopilotX = 0x3D;    autopilotY = 0x6E;
            disengagingX = 0x38;  disengagingY = 0x76;
            apCountX = 0x52;      apCountY = 0x76;
            elimNumX = 0x8E;      elimNumY = 0x3C;
            elimIconX = 0x8F;     elimIconY = 0x38;
            perfectLapX = 0x32;   perfectLapY = 0x6E;
            mpLapX = 0x0F;        mpLapY = 0xDE;
            mpLapOfX = 0x0F;      mpLapOfY = 0xE5;
            dangerX = 0x45;       dangerY = 0x75;
            critShieldX = 0x1F;   critShieldY = 0x8A;
            break;
        case 2:
            timeSpriteX = 0x290;  timeSpriteY = 0x4D;
            posSpriteX = 0x478;   posSpriteY = 0x4D;
            posNumX = 0x131;      posNumY = 0x05;
            timerX = 0xA5;        timerY = 0x05;
            lockX = 0x355;        lockY = 0x53;
            wrongWayX = 0xD7;     wrongWayY = 0x64;
            autopilotX = 0xD3;    autopilotY = 0x6E;
            disengagingX = 0xCE;  disengagingY = 0x76;
            apCountX = 0xE8;      apCountY = 0x76;
            elimNumX = 0x124;     elimNumY = 0x3C;
            elimIconX = 0x125;    elimIconY = 0x38;
            perfectLapX = 0x32;   perfectLapY = 0x6E;
            mpLapX = 0xA5;        mpLapY = 0xDE;
            mpLapOfX = 0xA5;      mpLapOfY = 0xE5;
            dangerX = 0xE0;       dangerY = 0x75;
            critShieldX = 0xB4;   critShieldY = 0x8A;
            break;
        case 3:
            timeSpriteX = 0x3C;   timeSpriteY = 0x11;
            posSpriteX = 0x478;   posSpriteY = 0x4D;
            posNumX = 0x131;      posNumY = 0x05;
            timerX = 0x0F;        timerY = 0x05;
            lockX = 0x20F;        lockY = 0x53;
            wrongWayX = 0x85;     wrongWayY = 0x2B;
            autopilotX = 0x85;    autopilotY = 0x3A;
            disengagingX = 0x80;  disengagingY = 0x41;
            apCountX = 0x9B;      apCountY = 0x41;
            elimNumX = 0x124;     elimNumY = 0x22;
            elimIconX = 0x125;    elimIconY = 0x1E;
            perfectLapX = 0x7B;   perfectLapY = 0x35;
            mpLapX = 0x0F;        mpLapY = 0x67;
            mpLapOfX = 0x0F;      mpLapOfY = 0x6D;
            dangerX = 0x90;       dangerY = 0x33;
            critShieldX = 0x66;   critShieldY = 0x49;
            break;
        case 4:
            timeSpriteX = 0x3C;   timeSpriteY = 0x1EC;
            posSpriteX = 0x478;   posSpriteY = 0x1EC;
            posNumX = 0x131;      posNumY = 0x7D;
            timerX = 0x0F;        timerY = 0x7D;
            lockX = 0x20F;        lockY = 0x22C;
            wrongWayX = 0x85;     wrongWayY = 0xA3;
            autopilotX = 0x85;    autopilotY = 0xA6;
            disengagingX = 0x80;  disengagingY = 0xAD;
            apCountX = 0x9B;      apCountY = 0xAD;
            elimNumX = 0x124;     elimNumY = 0x9A;
            elimIconX = 0x125;    elimIconY = 0x96;
            perfectLapX = 0x7B;   perfectLapY = 0x9E;
            mpLapX = 0x0F;        mpLapY = 0xDE;
            mpLapOfX = 0x0F;      mpLapOfY = 0xE5;
            dangerX = 0x90;       dangerY = 0xA5;
            critShieldX = 0x66;   critShieldY = 0xBC;
            break;
        case 5:
            timeSpriteX = 0x4C;   timeSpriteY = 0x4D;
            posSpriteX = 0x224;   posSpriteY = 0x4D;
            posNumX = 0x9C;       posNumY = 0x05;
            timerX = 0x0F;        timerY = 0x05;
            lockX = 0xFD;         lockY = 0x53;
            wrongWayX = 0x40;     wrongWayY = 0x2B;
            autopilotX = 0x3D;    autopilotY = 0x3A;
            disengagingX = 0x38;  disengagingY = 0x41;
            apCountX = 0x52;      apCountY = 0x41;
            elimNumX = 0x8E;      elimNumY = 0x22;
            elimIconX = 0x8F;     elimIconY = 0x1E;
            perfectLapX = 0x3E;   perfectLapY = 0x38;
            mpLapX = 0x0F;        mpLapY = 0x67;
            mpLapOfX = 0x0F;      mpLapOfY = 0x6D;
            dangerX = 0x45;       dangerY = 0x33;
            critShieldX = 0x20;   critShieldY = 0x49;
            break;
        case 6:
            timeSpriteX = 0x290;  timeSpriteY = 0x4D;
            posSpriteX = 0x478;   posSpriteY = 0x4D;
            posNumX = 0x131;      posNumY = 0x05;
            timerX = 0xA5;        timerY = 0x05;
            lockX = 0x355;        lockY = 0x53;
            wrongWayX = 0xD7;     wrongWayY = 0x2B;
            autopilotX = 0xD3;    autopilotY = 0x3A;
            disengagingX = 0xCE;  disengagingY = 0x41;
            apCountX = 0xE8;      apCountY = 0x41;
            elimNumX = 0x124;     elimNumY = 0x22;
            elimIconX = 0x125;    elimIconY = 0x1E;
            perfectLapX = 0xD5;   perfectLapY = 0x38;
            mpLapX = 0xA5;        mpLapY = 0x67;
            mpLapOfX = 0xA5;      mpLapOfY = 0x6D;
            dangerX = 0xDB;       dangerY = 0x33;
            critShieldX = 0xB2;   critShieldY = 0x49;
            break;
        case 7:
            timeSpriteX = 0x4C;   timeSpriteY = 0x1EC;
            posSpriteX = 0x224;   posSpriteY = 0x1EC;
            posNumX = 0x9C;       posNumY = 0x7D;
            timerX = 0x0F;        timerY = 0x7D;
            lockX = 0xFD;         lockY = 0x22C;
            wrongWayX = 0x40;     wrongWayY = 0xA3;
            autopilotX = 0x3D;    autopilotY = 0xA6;
            disengagingX = 0x38;  disengagingY = 0xAD;
            apCountX = 0x52;      apCountY = 0xAD;
            elimNumX = 0x8E;      elimNumY = 0x9A;
            elimIconX = 0x8F;     elimIconY = 0x96;
            perfectLapX = 0x3E;   perfectLapY = 0x9F;
            mpLapX = 0x0F;        mpLapY = 0xDE;
            mpLapOfX = 0x0F;      mpLapOfY = 0xE5;
            dangerX = 0x45;       dangerY = 0xA5;
            critShieldX = 0x20;   critShieldY = 0xBC;
            break;
        case 8:
            timeSpriteX = 0x290;  timeSpriteY = 0x1EC;
            posSpriteX = 0x478;   posSpriteY = 0x1EC;
            posNumX = 0x131;      posNumY = 0x7D;
            timerX = 0xA5;        timerY = 0x7D;
            lockX = 0x355;        lockY = 0x22C;
            wrongWayX = 0xD7;     wrongWayY = 0xA3;
            autopilotX = 0xD3;    autopilotY = 0xA6;
            disengagingX = 0xCE;  disengagingY = 0xAD;
            apCountX = 0xE8;      apCountY = 0xAD;
            elimNumX = 0x124;     elimNumY = 0x9A;
            elimIconX = 0x125;    elimIconY = 0x96;
            perfectLapX = 0xD5;   perfectLapY = 0x9F;
            mpLapX = 0xA5;        mpLapY = 0xDE;
            mpLapOfX = 0xA5;      mpLapOfY = 0xE5;
            dangerX = 0xDB;       dangerY = 0xA5;
            critShieldX = 0xB2;   critShieldY = 0xBC;
            break;
#else
        case 0:
            timeSpriteX = 0x4C;   timeSpriteY = 0x4C;
            posSpriteX = 0x37A;   posSpriteY = 0x4C;
            posNumX = 0xF5;       posNumY = 0x20;
            timerX = 0x14;        timerY = 0x1F;
            lockX = 0x217;        lockY = 0x10E;
            wrongWayX = 0x6A;     wrongWayY = 0x54;
            autopilotX = 0x72;    autopilotY = 0x6D;
            disengagingX = 0x67;  disengagingY = 0x79;
            apCountX = 0x9B;      apCountY = 0x79;
            elimNumX = 0x119;     elimNumY = 0x4D;
            elimIconX = 0x11A;    elimIconY = 0x48;
            perfectLapX = 0x6A;   perfectLapY = 0x54;
            dangerX = 0x0F;       dangerY = 0x40;
            critShieldX = 0x2D;   critShieldY = 0x80;
            break;
        case 1:
            timeSpriteX = 0x50;   timeSpriteY = 0x46;
            posSpriteX = 0x21C;   posSpriteY = 0x46;
            posNumX = 0x9B;       posNumY = 0x15;
            timerX = 0x16;        timerY = 0x15;
            lockX = 0xFC;         lockY = 0x93;
            wrongWayX = 0x40;     wrongWayY = 0x60;
            autopilotX = 0x3E;    autopilotY = 0x6E;
            disengagingX = 0x39;  disengagingY = 0x78;
            apCountX = 0x53;      apCountY = 0x78;
            elimNumX = 0x8D;      elimNumY = 0x40;
            elimIconX = 0x8E;     elimIconY = 0x3C;
            perfectLapX = 0x32;   perfectLapY = 0x6E;
            mpLapX = 0x16;        mpLapY = 0xCF;
            mpLapOfX = 0x15;      mpLapOfY = 0xD6;
            dangerX = 0x45;       dangerY = 0x58;
            critShieldX = 0x1F;   critShieldY = 0x6C;
            break;
        case 2:
            timeSpriteX = 0x294;  timeSpriteY = 0x46;
            posSpriteX = 0x488;   posSpriteY = 0x46;
            posNumX = 0x12B;      posNumY = 0x15;
            timerX = 0xA6;        timerY = 0x15;
            lockX = 0x354;        lockY = 0x93;
            wrongWayX = 0xD7;     wrongWayY = 0x60;
            autopilotX = 0xD4;    autopilotY = 0x6E;
            disengagingX = 0xD0;  disengagingY = 0x78;
            apCountX = 0xE9;      apCountY = 0x78;
            elimNumX = 0x11E;     elimNumY = 0x40;
            elimIconX = 0x11F;    elimIconY = 0x3C;
            perfectLapX = 0x32;   perfectLapY = 0x6E;
            mpLapX = 0xA6;        mpLapY = 0xCF;
            mpLapOfX = 0xA5;      mpLapOfY = 0xD6;
            dangerX = 0xE0;       dangerY = 0x58;
            critShieldX = 0xB4;   critShieldY = 0x6C;
            break;
        case 3:
            timeSpriteX = 0x54;   timeSpriteY = 0x41;
            posSpriteX = 0x45C;   posSpriteY = 0x41;
            posNumX = 0x12B;      posNumY = 0x15;
            timerX = 0x16;        timerY = 0x15;
            lockX = 0x20E;        lockY = 0x93;
            wrongWayX = 0x85;     wrongWayY = 0x3B;
            autopilotX = 0x86;    autopilotY = 0x44;
            disengagingX = 0x81;  disengagingY = 0x4C;
            apCountX = 0x9C;      apCountY = 0x4C;
            elimNumX = 0x11E;     elimNumY = 0x2E;
            elimIconX = 0x11F;    elimIconY = 0x2A;
            perfectLapX = 0x7B;   perfectLapY = 0x35;
            mpLapX = 0x16;        mpLapY = 0x68;
            mpLapOfX = 0x15;      mpLapOfY = 0x6F;
            dangerX = 0x90;       dangerY = 0x2F;
            critShieldX = 0x66;   critShieldY = 0x53;
            break;
        case 4:
            timeSpriteX = 0x54;   timeSpriteY = 0x1EE;
            posSpriteX = 0x45C;   posSpriteY = 0x1EE;
            posNumX = 0x12B;      posNumY = 0x7D;
            timerX = 0x16;        timerY = 0x7D;
            lockX = 0x20E;        lockY = 0x233;
            wrongWayX = 0x85;     wrongWayY = 0xA4;
            autopilotX = 0x86;    autopilotY = 0xA6;
            disengagingX = 0x81;  disengagingY = 0xAE;
            apCountX = 0x9C;      apCountY = 0xAE;
            elimNumX = 0x11E;     elimNumY = 0x99;
            elimIconX = 0x11F;    elimIconY = 0x95;
            perfectLapX = 0x7B;   perfectLapY = 0x9E;
            mpLapX = 0x16;        mpLapY = 0xCF;
            mpLapOfX = 0x15;      mpLapOfY = 0xD6;
            dangerX = 0x90;       dangerY = 0x98;
            critShieldX = 0x66;   critShieldY = 0xBC;
            break;
        case 5:
            timeSpriteX = 0x54;   timeSpriteY = 0x41;
            posSpriteX = 0x21C;   posSpriteY = 0x41;
            posNumX = 0x9B;       posNumY = 0x15;
            timerX = 0x16;        timerY = 0x15;
            lockX = 0xFC;         lockY = 0x93;
            wrongWayX = 0x40;     wrongWayY = 0x3B;
            autopilotX = 0x3E;    autopilotY = 0x3E;
            disengagingX = 0x3A;  disengagingY = 0x46;
            apCountX = 0x53;      apCountY = 0x46;
            elimNumX = 0x8D;      elimNumY = 0x2E;
            elimIconX = 0x8E;     elimIconY = 0x2A;
            perfectLapX = 0x3E;   perfectLapY = 0x38;
            mpLapX = 0x16;        mpLapY = 0x68;
            mpLapOfX = 0x15;      mpLapOfY = 0x6F;
            dangerX = 0x45;       dangerY = 0x2F;
            critShieldX = 0x20;   critShieldY = 0x53;
            break;
        case 6:
            timeSpriteX = 0x294;  timeSpriteY = 0x41;
            posSpriteX = 0x45C;   posSpriteY = 0x41;
            posNumX = 0x12B;      posNumY = 0x15;
            timerX = 0xA6;        timerY = 0x15;
            lockX = 0x354;        lockY = 0x93;
            wrongWayX = 0xD7;     wrongWayY = 0x3B;
            autopilotX = 0xD4;    autopilotY = 0x3E;
            disengagingX = 0xCF;  disengagingY = 0x46;
            apCountX = 0xE9;      apCountY = 0x46;
            elimNumX = 0x11E;     elimNumY = 0x2E;
            elimIconX = 0x11F;    elimIconY = 0x2A;
            perfectLapX = 0xD5;   perfectLapY = 0x38;
            mpLapX = 0xA6;        mpLapY = 0x68;
            mpLapOfX = 0xA5;      mpLapOfY = 0x6F;
            dangerX = 0xDB;       dangerY = 0x2F;
            critShieldX = 0xB2;   critShieldY = 0x53;
            break;
        case 7:
            timeSpriteX = 0x54;   timeSpriteY = 0x1EE;
            posSpriteX = 0x21C;   posSpriteY = 0x1EE;
            posNumX = 0x9B;       posNumY = 0x7D;
            timerX = 0x16;        timerY = 0x7D;
            lockX = 0xFC;         lockY = 0x233;
            wrongWayX = 0x40;     wrongWayY = 0xA4;
            autopilotX = 0x3E;    autopilotY = 0xAD;
            disengagingX = 0x39;  disengagingY = 0xB6;
            apCountX = 0x53;      apCountY = 0xB6;
            elimNumX = 0x8D;      elimNumY = 0x99;
            elimIconX = 0x8E;     elimIconY = 0x95;
            perfectLapX = 0x3E;   perfectLapY = 0x9F;
            mpLapX = 0x16;        mpLapY = 0xCF;
            mpLapOfX = 0x15;      mpLapOfY = 0xD6;
            dangerX = 0x45;       dangerY = 0x98;
            critShieldX = 0x20;   critShieldY = 0xA8;
            break;
        case 8:
            timeSpriteX = 0x294;  timeSpriteY = 0x1EE;
            posSpriteX = 0x45C;   posSpriteY = 0x1EE;
            posNumX = 0x12B;      posNumY = 0x7D;
            timerX = 0xA6;        timerY = 0x7D;
            lockX = 0x354;        lockY = 0x233;
            wrongWayX = 0xD7;     wrongWayY = 0xA4;
            autopilotX = 0xD4;    autopilotY = 0xAD;
            disengagingX = 0xCF;  disengagingY = 0xB6;
            apCountX = 0xE9;      apCountY = 0xB6;
            elimNumX = 0x11E;     elimNumY = 0x99;
            elimIconX = 0x11F;    elimIconY = 0x95;
            perfectLapX = 0xD5;   perfectLapY = 0x9F;
            mpLapX = 0xA6;        mpLapY = 0xCF;
            mpLapOfX = 0xA5;      mpLapOfY = 0xD6;
            dangerX = 0xDB;       dangerY = 0x98;
            critShieldX = 0xB2;   critShieldY = 0xA8;
            break;
#endif
    }

    if (g_attractMode == 1) {
        UpdateBlink();
        glistp = CentreText("DEMO MODE", 0x3C, HUD_DEMO_Y, 8, 0, glistp);
        if (GetBlinkState()) {
            glistp = CentreText("PRESS START", 0x9D, HUD_PRESS_START_Y, 8, 1, glistp);
        }
    }

    if (shipList[shipNo].attr & 0x40) {
        if (g_gameType == 0 || g_gameType == 1 || g_gameType == 6) {
            glistp = DrawFontText("LAP RECORD", HUD_LAP_REC_X, 0x46, 8, glistp);
            if (score->gotHiScore & 1) {
                lapRecTime = score->bestLapTime;
                if (lapRecTime != g_prevLapRecord) {
                    g_lapRecFlashTimer = FRAMES_PER_SEC * 2;
                }
                if (g_lapRecFlashTimer > 0) {
                    if ((g_lapRecFlashTimer % 30) < 21) {
                        glistp = AddTime(lapRecTime, HUD_LAP_REC_X, 0x4D, 8, 1, 1, glistp);
                    }
                    if (pauseChoice == -1) {
                        g_lapRecFlashTimer--;
                    }
                } else {
                    glistp = AddTime(lapRecTime, HUD_LAP_REC_X, 0x4D, 8, 1, 1, glistp);
                }
            } else {
                trackIdx = config->trackNum;
                if (g_gameType == 1 || (g_gameType == 6 && frontendOptions_g.challengeMode == 2)) {
                    trackIdx += 8;
                }
                lapRecTime = g_hiScoreTable[trackIdx][config->shipClass].lapTime;
                glistp = AddTime(lapRecTime, HUD_LAP_REC_X, 0x4D, 8, 1, 1, glistp);
            }
            g_prevLapRecord = lapRecTime;
        }

        if ((shipList[shipNo].attr & 0x40) && g_gameType == 1) {
            glistp = DrawFontText("SESSION BEST", HUD_LAP_REC_X, 0x5A, 8, glistp);
            if (g_ghostLapTime0 != 0) {
                if (g_ghostBestLapTime != g_ghostLapTime0) {
                    g_sessionBestFlashTimer = FRAMES_PER_SEC * 2;
                }
                if (g_sessionBestFlashTimer > 0) {
                    if ((g_sessionBestFlashTimer % 30) < 21) {
                        glistp = AddTime(g_ghostBestLapTime, HUD_LAP_REC_X, 0x61, 8, 1, 1, glistp);
                    }
                    if (pauseChoice == -1) {
                        g_sessionBestFlashTimer--;
                    }
                } else {
                    glistp = AddTime(g_ghostBestLapTime, HUD_LAP_REC_X, 0x61, 8, 1, 1, glistp);
                }
                g_ghostLapTime0 = g_ghostBestLapTime;
            }
        }
    }

    if (frontendOptions_g.numPlayers >= 2) {
        switch (player) {
            case 2:
                timer = &checkTimer2;
                break;
            case 3:
                timer = &checkTimer3;
                break;
            case 4:
                timer = &checkTimer4;
                break;
            default:
                timer = &checkTimer;
                break;
        }
    } else {
        timer = &checkTimer;
    }

    if (score->displayScore != 1 && g_raceEndState == 0 &&
        (!(shipList[shipNo].combatAttr & 0x8000) || !(shipList[shipNo].combatAttr & 4))) {
        if (shipList[shipNo].update == UpdatePlayerShip ||
            shipList[shipNo].update == UpdateRescueDroid ||
            (shipList[shipNo].update == UpdateDroneShip && shipList[shipNo].autopilotCount > 0)) {
            glistp = AlterCheckPoint(g_checkPoints, shipList, pauseData, config, player, shipNo, layout, glistp);
        }
    }

    if (shipNo == ownShip ||
        (frontendOptions_g.numPlayers >= 2 && shipNo == ownShip2) ||
        (frontendOptions_g.numPlayers >= 3 && shipNo == ownShip3) ||
        (frontendOptions_g.numPlayers == 4 && shipNo == ownShip4)) {
        for (i = 0; i < 15; i++) {
            if (!(shipList[i].combatAttr & 0x8000) &&
                shipList[i].lap < g_classLaps[config->shipClass] + 2) {
                distDelta = shipList[i].trackDistance - ((RaceLineEntry *)raceLines)[shipList[i].nearTrkSect->secNo].dist;
                if (distDelta > (numTrackSections >> 1)) {
                    if (shipList[i].reverseLap == 0) {
                        shipList[i].lap++;
                        if (g_gameType == 1 && i == ownShip) {
                            g_ttLapCount++;
                            if (g_ghostLoaded != 0) {
                                g_showSplitDelta = g_ttLapCount > 0;
                            } else {
                                g_showSplitDelta = g_ttLapCount >= 2;
                            }
                        }
                        for (j = 0; j < g_classLaps[config->shipClass]; j++) {
                            shipList[i].lapTimes[j] = (shipList[i].lapTimes[j] / FRAMES_PER_TENTH) * FRAMES_PER_TENTH;
                        }
                        if (i == ownShip && g_gameType == 1) {
                            if (shipList[i].lap >= 2) {
                                if (g_ghostLapTime0 == 0) {
                                    g_ghostBestLapTime = shipList[i].lapTimes[shipList[i].lap - 2];
                                    StoreGhostReplay(config, score, &shipList[i]);
                                    g_ghostLapTime0 = 1;
                                } else if (shipList[i].lapTimes[shipList[i].lap - 2] < g_ghostLapTime0) {
                                    g_ghostBestLapTime = shipList[i].lapTimes[shipList[i].lap - 2];
                                    StoreGhostReplay(config, score, &shipList[i]);
                                }
                                g_splitLapDelta = shipList[ownShip].lapTimes[shipList[ownShip].lap - 2] - g_ghostLapTime0;
                            }
                            InitGhostShip(resetData.config, resetData.ships, 1);
                        }
                        if (g_gameType != 3 && i == ownShip && shipList[i].lap >= 2 &&
                            shipList[i].lap <= g_classLaps[config->shipClass] + 1) {
                            CheckForHiScore(config, score, shipList, i, 1);
                        }
                        if (i == ownShip && g_gameType == 1 && shipList[i].lap == 3) {
                            shipList[i].lap = 2;
                            shipList[i].lapTimes[0] = shipList[i].lapTimes[1];
                            shipList[i].lapTimes[1] = 0;
                            for (l = 0; l < 6; l++) {
                                checkPointReached[l][0] = 1;
                                checkPointReached[l][1] = 1;
                                checkPointReached[l][2] = 1;
                                checkPointReached[l][3] = 1;
                            }
                        }
                    } else {
                        shipList[i].reverseLap--;
                    }
                    shipList[i].combatAttr |= 0x4000;
                    if ((i == ownShip || i == g_demoShip) &&
                        (g_gameType == 1 || (g_gameType == 6 && frontendOptions_g.challengeMode == 2))) {
                        shipList[i].weapon = 9;
                    }
                    if (i == ownShip) {
                        if (g_perfectLapFlag[0] != 0) {
                            g_perfectLapTimer[0] = 0x78;
                        }
                        g_perfectLapFlag[0] = 1;
                    } else if (i == ownShip2) {
                        if (g_perfectLapFlag[1] != 0) {
                            g_perfectLapTimer[1] = 0x78;
                        }
                        g_perfectLapFlag[1] = 1;
                    } else if (i == ownShip3) {
                        if (g_perfectLapFlag[2] != 0) {
                            g_perfectLapTimer[2] = 0x78;
                        }
                        g_perfectLapFlag[2] = 1;
                    } else if (i == ownShip4) {
                        if (g_perfectLapFlag[3] != 0) {
                            g_perfectLapTimer[3] = 0x78;
                        }
                        g_perfectLapFlag[3] = 1;
                    }
                } else if (distDelta < -(numTrackSections >> 1)) {
                    if (i == ownShip) {
                        g_perfectLapFlag[0] = 0;
                    } else if (i == ownShip2) {
                        g_perfectLapFlag[1] = 0;
                    } else if (i == ownShip3) {
                        g_perfectLapFlag[2] = 0;
                    } else if (i == ownShip4) {
                        g_perfectLapFlag[3] = 0;
                    }
                    shipList[i].reverseLap++;
                }
                curLap = shipList[i].lap - shipList[i].reverseLap;
                if (curLap < 0) {
                    curLap = 0;
                }
                shipList[i].distance = curLap * numTrackSections + ((RaceLineEntry *)raceLines)[shipList[i].nearTrkSect->secNo].dist;
                shipList[i].trackDistance = ((RaceLineEntry *)raceLines)[shipList[i].nearTrkSect->secNo].dist;
            }
        }
    }

    if ((g_shipDeathCount[shipNo] & 7) < 4 && g_shipDeathCount[shipNo] >= 16 && !(shipList[shipNo].combatAttr & 4)) {
        if (frontendOptions_g.numPlayers == 1) {
            glistp = CentreText("DANGER", dangerX, dangerY, 12, 1, glistp);
            glistp = CentreText("CRITICAL SHIELD FAILURE", critShieldX, critShieldY, 8, 0, glistp);
            if (!(g_shipDeathCount[shipNo] & 7)) {
                PlayNote(9, 0xFF, 15, 15, 0x100);
            }
        } else {
            glistp = DrawFontText("DANGER", dangerX, dangerY, 8, glistp);
            glistp = DrawFontText("CRITICAL SHIELD FAILURE", critShieldX, critShieldY, 8, glistp);
        }
    }

    if (g_podiumLocked[0] == -1) {
        g_podiumShips[0] = -1;
    }
    if (g_podiumLocked[1] == -1) {
        g_podiumShips[1] = -1;
    }
    if (g_podiumLocked[2] == -1) {
        g_podiumShips[2] = -1;
    }

    for (m = 0; m < 15; m++) {
        if (shipList[m].activeInLeague != 0 && !(shipList[m].combatAttr & 0x8000) && shipList[m].finishPosition == 0) {
            pos = GetShipPosition(shipList, m);
            if (pos == 1 && g_podiumLocked[0] == -1) {
                g_podiumShips[0] = m;
            } else if (pos == 2 && g_podiumLocked[1] == -1) {
                g_podiumShips[1] = m;
            } else if (pos == 3 && g_podiumLocked[2] == -1) {
                g_podiumShips[2] = m;
            }
            if (shipList[m].lap > g_classLaps[config->shipClass]) {
                shipList[m].finishPosition = pos;
                if (pos == 1) {
                    g_podiumLocked[0] = 1;
                } else if (pos == 2) {
                    g_podiumLocked[1] = 1;
                } else if (pos == 3) {
                    g_podiumLocked[2] = 1;
                }
            }
        }
    }

    pos = GetShipPosition(shipList, shipNo);
    if (shipNo == ownShip) {
        playerPosition = pos;
    } else if (frontendOptions_g.numPlayers >= 2 && shipNo == ownShip2) {
        player2Position = pos;
    } else if (frontendOptions_g.numPlayers >= 3 && shipNo == ownShip3) {
        player3Position = pos;
    } else if (frontendOptions_g.numPlayers == 4 && shipNo == ownShip4) {
        player4Position = pos;
    }

    if (g_multiplayerFlag != 0) {
        if (shipList[shipNo].attr & 0x40) {
            if (shipList[shipNo].update != UpdatePlayerShipStart) {
                curLap = shipList[shipNo].lap;
                if (curLap == 0) {
                    curLap = 1;
                }
                if (pauseChoice == -1 && shipList[shipNo].lap < g_classLaps[config->shipClass] + 1) {
                    shipList[shipNo].lapTimes[curLap - 1] += 2;
                }
            }
        } else {
            goto skip_hud_draw;
        }
    } else if (shipList[shipNo].combatAttr & 0x4000) {
        curLap = shipList[shipNo].lap;
        if (curLap == 0) {
            curLap = 1;
        }
        if (pauseChoice == -1 && shipList[shipNo].lap < g_classLaps[config->shipClass] + 1) {
            shipList[shipNo].lapTimes[curLap - 1] += 2;
        }
    }

    if (shipList[shipNo].attr & 0x40) {
        if (layout == 0) {
            if (shipList[shipNo].combatAttr & 0x4000) {
                if (g_gameType == 1 || (g_gameType == 6 && frontendOptions_g.challengeMode == 2)) {
                    glistp = DrawFontText("LAP TIMES", HUD_LAP_REC_X, lapTimesY[0] + HUD_LAP_TIMES_Y_OFF, 8, glistp);
                    if (shipList[shipNo].lap == 2) {
                        for (i = 1; i < 3; i++) {
                            sprintf(lapLabelBuf, "%c", (i == 1) ? 'P' : 'C');
                            glistp = DrawFontText(lapLabelBuf, HUD_LAP_LBL_X, lapTimesY[0] + (i * 10 + HUD_LAP_TIMES_Y_OFF), 8, glistp);
                        }
                    } else {
                        glistp = DrawFontText("C", HUD_LAP_C_X, lapTimesY[0] + HUD_LAP_TIMES_Y_OFF + 10, 8, glistp);
                    }
                } else {
                    glistp = DrawFontText("LAP TIMES", HUD_LAP_REC_X, lapTimesY[config->shipClass] + HUD_LAP_TIMES_Y_OFF, 8, glistp);
                    for (i = 1; i <= g_classLaps[config->shipClass]; i++) {
                        sprintf(lapLabelBuf, "%d", i);
                        glistp = DrawFontText(lapLabelBuf, HUD_LAP_LBL_X, lapTimesY[config->shipClass] + (i * 10 + HUD_LAP_TIMES_Y_OFF), 8, glistp);
                    }
                }

                for (i = 1; i <= shipList[shipNo].lap; i++) {
                    if (i == curLap) {
                        if (g_gameType == 1 || (g_gameType == 6 && frontendOptions_g.challengeMode == 2)) {
                            glistp = AddTime(shipList[shipNo].lapTimes[i - 1], HUD_LAP_CUR_X,
                                                   lapTimesY[0] + i * 10 + (HUD_LAP_TIMES_Y_OFF - 1), 8, 0, 1, glistp);
                        } else {
                            glistp = AddTime(shipList[shipNo].lapTimes[i - 1], HUD_LAP_CUR_X,
                                                   lapTimesY[config->shipClass] + i * 10 + (HUD_LAP_TIMES_Y_OFF - 1), 8, 0, 1, glistp);
                        }
                    } else {
                        if (g_gameType == 1 || (g_gameType == 6 && frontendOptions_g.challengeMode == 2)) {
                            TimeToString(shipList[shipNo].lapTimes[i - 1], 1, timeBuf);
                            glistp = DrawFontText(timeBuf, 0x24, lapTimesY[0] + (i * 10 + HUD_LAP_TIMES_Y_OFF), 8, glistp);
                        } else {
                            TimeToString(shipList[shipNo].lapTimes[i - 1], 1, timeBuf);
                            glistp = DrawFontText(timeBuf, 0x24, lapTimesY[config->shipClass] + (i * 10 + HUD_LAP_TIMES_Y_OFF), 8, glistp);
                        }
                    }
                }
            }
        }

        apCount = shipList[shipNo].autopilotCount;
        if (apCount != 0) {
            if (layout == 0) {
                glistp = AddText("AUTOPILOT", autopilotX, autopilotY, 8, 0, glistp);
            } else {
                glistp = DrawFontTextYellow("AUTOPILOT", autopilotX, autopilotY, 8, glistp);
            }
            if (apCount < (FRAMES_PER_SEC / 2)) {
                if (layout == 0) {
                    glistp = AddText("DISENGAGING", disengagingX, disengagingY, 8, 0, glistp);
                } else {
                    glistp = DrawFontTextYellow("DISENGAGING", disengagingX, disengagingY, 8, glistp);
                }
            } else if (layout == 0) {
                glistp = AddNumber(apCount / (FRAMES_PER_SEC / 2), apCountX, apCountY, 12, 1, glistp);
            } else {
                sprintf(autopilotBuf, "%d", apCount / (FRAMES_PER_SEC / 2));
                glistp = DrawFontText(autopilotBuf, apCountX, apCountY, 12, glistp);
            }
        }

        if (shipList[shipNo].rearTarget != -1 && rearLockCycle[player - 1] < 13 &&
            shipList[shipNo].weapon == 2) {
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_rearLockSprite));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, lockX, lockY);
        }

        if (g_perfectLapTimer[player - 1] != 0) {
            if (layout == 0) {
                glistp = AddText("PERFECT LAP", perfectLapX, perfectLapY, 8, 0, glistp);
            }
            g_perfectLapTimer[player - 1]--;
        }

        if (g_gameType != 1 && (g_gameType != 6 || frontendOptions_g.challengeMode != 2)) {
            activeShips = 0;
            for (k = 0; k < 15; k++) {
                if (!(shipList[k].combatAttr & 0x8000)) {
                    activeShips++;
                }
            }
            if (layout == 0) {
                glistp = AddJustifyNumber(pos, posNumX, posNumY, 16, 1, glistp);
            } else {
                glistp = AddJustifyNumber(pos, posNumX, posNumY, 8, 1, glistp);
            }
            if (g_gameType != 3) {
                glistp = AddText("OF", HUD_OF_X, HUD_OF_Y, 8, 0, glistp);
                glistp = AddNumber(activeShips, HUD_TOTAL_SHIPS_X, HUD_TOTAL_SHIPS_Y, 16, 1, glistp);
            }
        }

        if (frontendOptions_g.numPlayers >= 2) {
            glistp = DrawFontText("LAP", mpLapX, mpLapY, 8, glistp);
            sprintf(lapOfBuf, "%i OF %i", shipList[shipNo].lap, g_classLaps[config->shipClass]);
            glistp = DrawFontText(lapOfBuf, mpLapOfX, mpLapOfY, 8, glistp);
        }

        gDPPipeSync(glistp++);
        gDPSetPrimColor(glistp++, 0, 0, 255, 255, 0, 0);
        gDPSetCombineMode(glistp++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);

        if (g_gameType != 1 && (g_gameType != 6 || frontendOptions_g.challengeMode != 2) && layout == 0) {
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_plusSprite));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, timeSpriteX, timeSpriteY);
        }
        if (g_gameType != 1 && (g_gameType != 6 || frontendOptions_g.challengeMode != 2) && layout == 0) {
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_minusSprite));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, posSpriteX, posSpriteY);
        }

        if (g_gameType == 6) {
            targetLevel = frontendOptions_g.challengeClass[frontendOptions_g.challengeIndex].award + 1;
            if (frontendOptions_g.challengeIndex == 6) {
                if (frontendOptions_g.challengeClass[frontendOptions_g.challengeIndex].award != 3) {
                    glistp = DrawFontText("GOLD TARGET", HUD_TARGET_X, 0x5A, 8, glistp);
                    if (frontendOptions_g.challengeMode == 1) {
                        glistp = DrawFontText("FINISH 1ST", HUD_TARGET_X, 0x61, 8, glistp);
                    }
                    if (frontendOptions_g.challengeMode == 2) {
                        TimeToString((s32)frontendOptions_g.challengeClass[frontendOptions_g.challengeIndex].goldTarget1, 1, targetBuf);
                        glistp = DrawFontText(targetBuf, HUD_TARGET_X, 0x61, 8, glistp);
                    }
                    if (frontendOptions_g.challengeMode == 3) {
                        glistp = DrawFontText("DESTROY", HUD_TARGET_X, 0x61, 8, glistp);
                        NumberToString((s32)frontendOptions_g.challengeClass[frontendOptions_g.challengeIndex].goldTarget1, targetBuf);
                        glistp = DrawFontText(targetBuf, HUD_DESTROY_NUM_X, 0x61, 8, glistp);
                    }
                    if (frontendOptions_g.challengeMode == 4) {
                        glistp = DrawFontText("FINISH 1ST", HUD_TARGET_X, 0x61, 8, glistp);
                        glistp = DrawFontText("DESTROY", HUD_TARGET_X, 0x6B, 8, glistp);
                        NumberToString((s32)frontendOptions_g.challengeClass[frontendOptions_g.challengeIndex].goldTarget2, targetBuf);
                        glistp = DrawFontText(targetBuf, HUD_DESTROY_NUM_X, 0x6B, 8, glistp);
                    }
                }
            } else if (targetLevel < 4) {
                if (targetLevel == 3) {
                    glistp = DrawFontText("GOLD TARGET", HUD_TARGET_X, 0x5A, 8, glistp);
                    if (frontendOptions_g.challengeMode == 1) {
                        glistp = DrawFontText("FINISH 1ST", HUD_TARGET_X, 0x61, 8, glistp);
                    }
                    if (frontendOptions_g.challengeMode == 2) {
                        TimeToString((s32)frontendOptions_g.challengeClass[frontendOptions_g.challengeIndex].goldTarget1, 1, targetBuf);
                        glistp = DrawFontText(targetBuf, HUD_TARGET_X, 0x61, 8, glistp);
                    }
                    if (frontendOptions_g.challengeMode == 3) {
                        glistp = DrawFontText("DESTROY", HUD_TARGET_X, 0x61, 8, glistp);
                        NumberToString((s32)frontendOptions_g.challengeClass[frontendOptions_g.challengeIndex].goldTarget1, targetBuf);
                        glistp = DrawFontText(targetBuf, HUD_DESTROY_NUM_X, 0x61, 8, glistp);
                    }
                    if (frontendOptions_g.challengeMode == 4) {
                        glistp = DrawFontText("FINISH 1ST", HUD_TARGET_X, 0x61, 8, glistp);
                        glistp = DrawFontText("DESTROY", HUD_TARGET_X, 0x6B, 8, glistp);
                        NumberToString((s32)frontendOptions_g.challengeClass[frontendOptions_g.challengeIndex].goldTarget2, targetBuf);
                        glistp = DrawFontText(targetBuf, HUD_DESTROY_NUM_X, 0x6B, 8, glistp);
                    }
                }
                if (targetLevel == 2) {
                    glistp = DrawFontText("SILVER TARGET", HUD_TARGET_X, 0x5A, 8, glistp);
                    if (frontendOptions_g.challengeMode == 1) {
                        glistp = DrawFontText("FINISH 2ND", HUD_TARGET_X, 0x61, 8, glistp);
                    }
                    if (frontendOptions_g.challengeMode == 2) {
                        TimeToString((s32)frontendOptions_g.challengeClass[frontendOptions_g.challengeIndex].silverTarget1, 1, targetBuf);
                        glistp = DrawFontText(targetBuf, HUD_TARGET_X, 0x61, 8, glistp);
                    }
                    if (frontendOptions_g.challengeMode == 3) {
                        glistp = DrawFontText("DESTROY", HUD_TARGET_X, 0x61, 8, glistp);
                        NumberToString((s32)frontendOptions_g.challengeClass[frontendOptions_g.challengeIndex].silverTarget1, targetBuf);
                        glistp = DrawFontText(targetBuf, HUD_DESTROY_NUM_X, 0x61, 8, glistp);
                    }
                    if (frontendOptions_g.challengeMode == 4) {
                        glistp = DrawFontText("FINISH 2ND", HUD_TARGET_X, 0x61, 8, glistp);
                        glistp = DrawFontText("DESTROY", HUD_TARGET_X, 0x6B, 8, glistp);
                        NumberToString((s32)frontendOptions_g.challengeClass[frontendOptions_g.challengeIndex].silverTarget2, targetBuf);
                        glistp = DrawFontText(targetBuf, HUD_DESTROY_NUM_X, 0x6B, 8, glistp);
                    }
                }
                if (targetLevel == 1) {
                    glistp = DrawFontText("BRONZE TARGET", HUD_TARGET_X, 0x5A, 8, glistp);
                    if (frontendOptions_g.challengeMode == 1) {
                        glistp = DrawFontText("FINISH 3RD", HUD_TARGET_X, 0x61, 8, glistp);
                    }
                    if (frontendOptions_g.challengeMode == 2) {
                        TimeToString((s32)frontendOptions_g.challengeClass[frontendOptions_g.challengeIndex].bronzeTarget1, 1, targetBuf);
                        glistp = DrawFontText(targetBuf, HUD_TARGET_X, 0x61, 8, glistp);
                    }
                    if (frontendOptions_g.challengeMode == 3) {
                        glistp = DrawFontText("DESTROY", HUD_TARGET_X, 0x61, 8, glistp);
                        NumberToString((s32)frontendOptions_g.challengeClass[frontendOptions_g.challengeIndex].bronzeTarget1, targetBuf);
                        glistp = DrawFontText(targetBuf, HUD_DESTROY_NUM_X, 0x61, 8, glistp);
                    }
                    if (frontendOptions_g.challengeMode == 4) {
                        glistp = DrawFontText("FINISH 3RD", HUD_TARGET_X, 0x61, 8, glistp);
                        glistp = DrawFontText("DESTROY", HUD_TARGET_X, 0x6B, 8, glistp);
                        NumberToString((s32)frontendOptions_g.challengeClass[frontendOptions_g.challengeIndex].bronzeTarget2, targetBuf);
                        glistp = DrawFontText(targetBuf, HUD_DESTROY_NUM_X, 0x6B, 8, glistp);
                    }
                }
            }
        }

        if (g_gameType != 1 && (g_gameType != 6 || frontendOptions_g.challengeMode != 2)) {
            if (attractMode == 2) {
                glistp = DrawButtonPrompt(elimIconX, elimIconY, 14, NULL, glistp);
                glistp = AddJustifyNumber(shipList[shipNo].eliminations, elimNumX, elimNumY, 8, 1, glistp);
            } else {
                glistp = DrawButtonPrompt(elimIconX, elimIconY, 15, NULL, glistp);
                glistp = DrawFontNumber(shipList[shipNo].eliminations, elimNumX, elimNumY, 8, 1, glistp);
            }
        }

        if (pauseChoice == -1 && apCount == 0 && shipDamage[shipNo] < GetDroneShield(shipNo) &&
            !(shipList[shipNo].attr & 8)) {
            if (layout == 0) {
                glistp = AddText("WRONG WAY", wrongWayX, wrongWayY, 8, 0, glistp);
            } else {
                glistp = DrawFontTextYellow("WRONG WAY", wrongWayX, wrongWayY, 8, glistp);
            }
        }

        if ((shipList[shipNo].attr & 0x40) && shipList[shipNo].lap >= 0) {
            if (layout == 0) {
                if (g_gameType == 1) {
                    if (g_ghostEnabled != 0 && g_showSplitDelta != 0) {
                        if (*timer != g_prevSplitDelta) {
                            g_splitFlashTimer = FRAMES_PER_SEC;
                        }
                        if (g_splitFlashTimer > 0) {
                            if (pauseChoice == -1) {
                                g_splitFlashTimer--;
                            }
                            if ((g_splitFlashTimer % 30) < 21) {
                                showGhostTime = 1;
                            }
                        } else {
                            showGhostTime = 1;
                        }
                        if (showGhostTime != 0) {
                            if (*timer < 0) {
                                if (*timer < -FRAMES_PER_TENTH) {
                                    glistp = AddTime(*timer, timerX, timerY, 16, 0, 1, glistp);
                                } else {
                                    glistp = AddTime(0, timerX, timerY, 16, 0, 1, glistp);
                                }
                                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_plusSprite));
                                gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
                                gSPSprite2DDraw(glistp++, timeSpriteX, timeSpriteY);
                            } else {
                                if (*timer > FRAMES_PER_TENTH) {
                                    glistp = AddTime(*timer, timerX, timerY, 16, 1, 1, glistp);
                                } else {
                                    glistp = AddTime(0, timerX, timerY, 16, 1, 1, glistp);
                                }
                                gDPPipeSync(glistp++);
                                gDPSetPrimColor(glistp++, 0, 0, 255, 255, 0, 0);
                                gDPSetCombineMode(glistp++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);
                                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_plusSprite));
                                gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
                                gSPSprite2DDraw(glistp++, timeSpriteX, timeSpriteY);
                            }
                        }
                        g_prevSplitDelta = *timer;
                    }
                } else if (g_gameType != 6 || frontendOptions_g.challengeMode != 2) {
                    if (*timer <= 0) {
                        if ((g_hudFrameCount % 30) < 21) {
                            glistp = AddTime(*timer, timerX, timerY, 16, 0, 1, glistp);
                        }
                    } else {
                        glistp = AddTime(*timer, timerX, timerY, 16, 1, 1, glistp);
                    }
                }
            } else if (frontendOptions_g.multiTimer != 0 && g_multiplayerFlag != 0) {
                if (*timer <= 0) {
                    if ((g_hudFrameCount % 30) < 21) {
                        TimeToString(*timer, 1, timeBuf);
                        glistp = DrawFontText(timeBuf, timerX, timerY, 16, glistp);
                    }
                } else {
                    gDPPipeSync(glistp++);
                    gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
                    TimeToString(*timer, 1, timeBuf);
                    glistp = DrawFontText(timeBuf, timerX, timerY, 16, glistp);
                }
            }
        }

        gDPPipeSync(glistp++);
        gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    }

skip_hud_draw:
    for (i = 0; i < 15; i++) {
        if (i != ownShip && i != ownShip2 && i != ownShip3 && i != ownShip4) {
            if (shipList[i].lap > g_classLaps[config->shipClass]) {
                if (shipList[i].attr & 0x40) {
                    shipList[i].attr &= ~0x40;
                }
            }
        }
    }

    if ((shipList[shipNo].lap > g_classLaps[config->shipClass] ||
         (g_gameType == 6 && frontendOptions_g.challengeMode == 2 && shipList[shipNo].lap >= 2)) &&
        pauseChoice == -1 && (shipList[shipNo].attr & 0x40)) {
        shipList[shipNo].update = UpdateDroneShip;
        shipList[shipNo].attr = 0;
        shipList[shipNo].updateCount = 0;
        shipList[shipNo].autopilotCount = 0;
        shipList[shipNo].attr &= ~0x40;
        humanToDroneSpeed(shipList, shipNo);
        if (!(shipList[shipNo].attr & 0x40)) {
            if (shipNo == ownShip) {
                config->raceInfo[0] = 1;
                CameraUpdate = UpdateCameraAttractMode;
                g_finishPosition[player - 1] = playerPosition;
                copyInTempLapTimes(shipList, 0);
            }
            if (shipNo == ownShip2) {
                config->raceInfo[1] = 1;
                CameraUpdate2 = UpdateCameraAttractMode;
                g_finishPosition[player - 1] = player2Position;
                copyInTempLapTimes(shipList, 1);
            }
            if (shipNo == ownShip3) {
                config->raceInfo[2] = 1;
                CameraUpdate3 = UpdateCameraAttractMode;
                g_finishPosition[player - 1] = player3Position;
                copyInTempLapTimes(shipList, 2);
            }
            if (shipNo == ownShip4) {
                config->raceInfo[3] = 1;
                CameraUpdate4 = UpdateCameraAttractMode;
                g_finishPosition[player - 1] = player4Position;
                copyInTempLapTimes(shipList, 3);
            }
            pauseRequests[player - 1] = 1;
            switch (frontendOptions_g.numPlayers) {
                case 1:
                    if (pauseRequests[0] != 0) {
                        attractDelayFrames = 1;
                    }
                    break;
                case 2:
                    if (pauseRequests[0] != 0 && pauseRequests[1] != 0) {
                        attractDelayFrames = 1;
                    }
                    break;
                case 3:
                    if (pauseRequests[0] != 0 && pauseRequests[1] != 0 && pauseRequests[2] != 0) {
                        attractDelayFrames = 1;
                    }
                    break;
                case 4:
                    if (pauseRequests[0] != 0 && pauseRequests[1] != 0 && pauseRequests[2] != 0 && pauseRequests[3] != 0) {
                        attractDelayFrames = 1;
                    }
                    break;
            }
            g_playerFinished[player - 1] = 1;
            StopRumbleAll(player - 1);
        }
    }

    if (shipList[shipNo].attr & 0x40) {
        if (shipList[shipNo].attr & 0x400) {
            shipList[shipNo].speed = 0;
        }
        glistp = FillSpeedo(sprites1P, spritesMP, shipList, config, pauseChoice, shipNo, layout, glistp);
    }

    if (frontendOptions_g.numPlayers == 1 && g_attractMode != 1 && g_trackNameScroll >= -479) {
        g_trackNameFade = g_trackNameScroll;
        if (g_trackNameFade < 0 && g_trackNameFade > -200) {
            g_trackNameFade = 0;
        }
        if (g_trackNameFade <= -200) {
            g_trackNameFade += 200;
        }
        if (g_trackNameFade < 0) {
            g_trackNameFade = -g_trackNameFade;
        }
        if (pauseData == -1) {
            g_trackNameScroll -= 6;
        }
        trackName = GetCdTrackName(frontendOptions_g.musicTrack);
        sprintf(musicBuf, "MUSIC TRACK: %d   %s [%s]", frontendOptions_g.musicTrack + 1,
                trackName, GetCdTrackArtist(frontendOptions_g.musicTrack));
        gDPPipeSync(glistp++);
        gDPSetPrimColor(glistp++, 0, 0, 255, 255, 255, ((255 - g_trackNameFade) < 0) ? 0 : (255 - g_trackNameFade));
        gDPSetCombineMode(glistp++, G_CC_DECALRGB_PRIMA, G_CC_DECALRGB_PRIMA);
        glistp = AddCentreText(musicBuf, 160, HUD_MUSIC_Y, 1, glistp);
        gDPPipeSync(glistp++);
        gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    }

    g_hudFrameCount++;
    return glistp;
}

s16 GetShipPosition(ShipData *shipList, s32 shipNo) {
    s32 i;
    s32 j;
    s32 pos;
    s32 dist;
    s32 finished;
    s32 angle1;
    s32 angle2;
    s32 proj1;
    s32 proj2;
    VECTOR shipVec;
    VECTOR otherVec;
    VECTOR trackVec;

    for (i = 0; i < 3; i++) {
        if (g_podiumLocked[i] != -1 && g_podiumShips[i] == shipNo) {
            return i + 1;
        }
    }

    pos = 1;
    if (g_podiumLocked[0] != -1) {
        if (g_podiumLocked[1] == -1) {
            pos = 2;
        } else if (g_podiumLocked[2] == -1) {
            pos = 3;
        } else {
            pos = 4;
        }
    }

    dist = shipList[shipNo].distance;
    shipVec.vx = shipList[shipNo].ppivot.vx - shipList[shipNo].nearTrkSect->centre.vx;
    shipVec.vy = shipList[shipNo].ppivot.vy - shipList[shipNo].nearTrkSect->centre.vy;
    shipVec.vz = shipList[shipNo].ppivot.vz - shipList[shipNo].nearTrkSect->centre.vz;
    trackVec.vx = shipList[shipNo].nearTrkSect->nextSection.ptr->centre.vx - shipList[shipNo].nearTrkSect->centre.vx;
    trackVec.vy = shipList[shipNo].nearTrkSect->nextSection.ptr->centre.vy - shipList[shipNo].nearTrkSect->centre.vy;
    trackVec.vz = shipList[shipNo].nearTrkSect->nextSection.ptr->centre.vz - shipList[shipNo].nearTrkSect->centre.vz;

    for (i = 0; i < 15; i++) {
        if (i != shipNo && !(shipList[i].combatAttr & 0x8000)) {
            finished = 0;
            for (j = 0; j < 3; j++) {
                if (g_podiumLocked[j] != -1 && g_podiumShips[j] == i) {
                    finished = 1;
                }
            }
            if (!finished) {
                if (dist == shipList[i].distance) {
                    otherVec.vx = shipList[i].ppivot.vx - shipList[i].nearTrkSect->centre.vx;
                    otherVec.vy = shipList[i].ppivot.vy - shipList[i].nearTrkSect->centre.vy;
                    otherVec.vz = shipList[i].ppivot.vz - shipList[i].nearTrkSect->centre.vz;
                    angle1 = GetAng(shipVec, trackVec);
                    angle2 = GetAng(otherVec, trackVec);
                    proj1 = (rcos((s16)(angle1 >> 3)) * GetMagnitude(shipVec)) >> 12;
                    proj2 = (rcos((s16)(angle2 >> 3)) * GetMagnitude(otherVec)) >> 12;
                    if (proj2 >= proj1) {
                        pos++;
                    }
                } else if (dist < shipList[i].distance) {
                    pos++;
                }
            }
        }
    }
    return pos;
}

void copyInTempLapTimes(ShipData *shipList, s32 player) {
    s32 i;
    s32 shipNo;

    if (player == 0) {
        shipNo = ownShip;
    } else if (player == 1) {
        shipNo = ownShip2;
    } else if (player == 2) {
        shipNo = ownShip3;
    } else if (player == 3) {
        shipNo = ownShip4;
    }

    for (i = 0; i < 5; i++) {
        g_savedLapTimes[i][player] = (shipList[shipNo].lapTimes[i] / FRAMES_PER_TENTH) * FRAMES_PER_TENTH;
    }
}

Gfx *DisplayRaceInfo(ConfigData *config, ScoreData *score, ShipData *shipList, Gfx *glistp, s32 player) {
    char titleBuf[40];
    char suffixBuf[16];
    char numBuf[8];
    s32 totalTime;
    s32 playerShip;
    s32 lapLabelX, lapLabelY;
    s32 lapNumX, lapNumY;
    s32 lapTimeX, lapTimeY;
    s32 titleX, titleY;
    s32 posNumX, posNumY;
    s32 statsX, statsY;
    s32 raceLabelX, raceLabelY;
    s32 raceTimeX, raceTimeY;
    s32 elimLabelX, elimLabelY;
    s32 elimNumX, elimNumY;
    s32 suffixX, suffixY;
    s32 badLuckX, badLuckY;
    s32 destroyedX, destroyedY;
    s32 outOfTimeX, outOfTimeY;
    s32 layout;
    s32 i;
    s16 lap;
    s16 lap2;
    u8 allEliminated;

    totalTime = 0;
    allEliminated = 1;
    layout = frontendOptions_g.screenLayout[player];

    switch (layout) {
        case 0:
            lapLabelX = 0x67;   lapLabelY = 0x69;
            lapNumX = 0x8F;     lapNumY = 0x69;
            lapTimeX = 0xA3;    lapTimeY = 0x69;
            titleX = 0x5D;      titleY = 0x1E;
            posNumX = 0x94;     posNumY = 0x35;
            statsX = 0x5D;      statsY = 0x55;
            raceLabelX = 0x67;  raceLabelY = 0xAB;
            raceTimeX = 0xA3;   raceTimeY = 0xAB;
            elimLabelX = 0x53;  elimLabelY = 0xC3;
            elimNumX = 0xD5;    elimNumY = 0xC3;
            suffixX = 0xA3;     suffixY = 0x35;
            badLuckX = 0x5D;    badLuckY = 0x1E;
            destroyedX = 0x5D;  destroyedY = 0x1E;
            outOfTimeX = 0x5D;  outOfTimeY = 0x1E;
            break;
#ifdef VERSION_EU
        case 1:
            lapLabelX = 0x36;   lapLabelY = 0x6C;
            lapNumX = 0x49;     lapNumY = 0x6C;
            lapTimeX = 0x55;    lapTimeY = 0x6C;
            titleX = 0x2D;      titleY = 0x46;
            posNumX = 0x57;     posNumY = 0x4F;
            statsX = 0x30;      statsY = 0x62;
            raceLabelX = 0x36;  raceLabelY = 0x93;
            raceTimeX = 0x55;   raceTimeY = 0x93;
            elimLabelX = 0x32;  elimLabelY = 0x9D;
            elimNumX = 0x76;    elimNumY = 0x9D;
            suffixX = 0x58;     suffixY = 0x4F;
            badLuckX = 0x41;    badLuckY = 0x46;
            destroyedX = 0x31;  destroyedY = 0x51;
            outOfTimeX = 0x3B;  outOfTimeY = 0x51;
            break;
        case 2:
            lapLabelX = 0xCC;   lapLabelY = 0x6C;
            lapNumX = 0xDF;     lapNumY = 0x6C;
            lapTimeX = 0xEB;    lapTimeY = 0x6C;
            titleX = 0xC2;      titleY = 0x46;
            posNumX = 0xEC;     posNumY = 0x4F;
            statsX = 0xC5;      statsY = 0x62;
            raceLabelX = 0xCC;  raceLabelY = 0x93;
            raceTimeX = 0xEB;   raceTimeY = 0x93;
            elimLabelX = 0xC7;  elimLabelY = 0x9D;
            elimNumX = 0x10B;   elimNumY = 0x9D;
            suffixX = 0xED;     suffixY = 0x4F;
            badLuckX = 0xD6;    badLuckY = 0x46;
            destroyedX = 0xC7;  destroyedY = 0x51;
            outOfTimeX = 0xD1;  outOfTimeY = 0x51;
            break;
        case 3:
            lapLabelX = 0x80;   lapLabelY = 0x2D;
            lapNumX = 0x93;     lapNumY = 0x2D;
            lapTimeX = 0x9F;    lapTimeY = 0x2D;
            titleX = 0x55;      titleY = 0x07;
            posNumX = 0x9F;     posNumY = 0x13;
            statsX = 0x7A;      statsY = 0x23;
            raceLabelX = 0x80;  raceLabelY = 0x55;
            raceTimeX = 0x9F;   raceTimeY = 0x55;
            elimLabelX = 0x7B;  elimLabelY = 0x5F;
            elimNumX = 0xBF;    elimNumY = 0x5F;
            suffixX = 0xA1;     suffixY = 0x13;
            badLuckX = 0x77;    badLuckY = 0x07;
            destroyedX = 0x7B;  destroyedY = 0x14;
            outOfTimeX = 0x85;  outOfTimeY = 0x14;
            break;
        case 4:
            lapLabelX = 0x80;   lapLabelY = 0xA5;
            lapNumX = 0x93;     lapNumY = 0xA5;
            lapTimeX = 0x9F;    lapTimeY = 0xA5;
            titleX = 0x55;      titleY = 0x7F;
            posNumX = 0x9F;     posNumY = 0x8B;
            statsX = 0x7A;      statsY = 0x9B;
            raceLabelX = 0x80;  raceLabelY = 0xCD;
            raceTimeX = 0x9F;   raceTimeY = 0xCD;
            elimLabelX = 0x7B;  elimLabelY = 0xD7;
            elimNumX = 0xBF;    elimNumY = 0xD7;
            suffixX = 0xA1;     suffixY = 0x8B;
            badLuckX = 0x77;    badLuckY = 0x7F;
            destroyedX = 0x7B;  destroyedY = 0x8C;
            outOfTimeX = 0x85;  outOfTimeY = 0x8C;
            break;
        case 5:
            lapLabelX = 0x36;   lapLabelY = 0x2D;
            lapNumX = 0x49;     lapNumY = 0x2D;
            lapTimeX = 0x55;    lapTimeY = 0x2D;
            titleX = 0x2D;      titleY = 0x07;
            posNumX = 0x57;     posNumY = 0x13;
            statsX = 0x30;      statsY = 0x23;
            raceLabelX = 0x36;  raceLabelY = 0x55;
            raceTimeX = 0x55;   raceTimeY = 0x55;
            elimLabelX = 0x32;  elimLabelY = 0x5F;
            elimNumX = 0x76;    elimNumY = 0x5F;
            suffixX = 0x59;     suffixY = 0x13;
            badLuckX = 0x41;    badLuckY = 0x07;
            destroyedX = 0x31;  destroyedY = 0x14;
            outOfTimeX = 0x3B;  outOfTimeY = 0x14;
            break;
        case 6:
            lapLabelX = 0xCC;   lapLabelY = 0x2D;
            lapNumX = 0xDF;     lapNumY = 0x2D;
            lapTimeX = 0xEB;    lapTimeY = 0x2D;
            titleX = 0xC2;      titleY = 0x07;
            posNumX = 0xEC;     posNumY = 0x13;
            statsX = 0xC5;      statsY = 0x23;
            raceLabelX = 0xCC;  raceLabelY = 0x55;
            raceTimeX = 0xEB;   raceTimeY = 0x55;
            elimLabelX = 0xC7;  elimLabelY = 0x5F;
            elimNumX = 0x10B;   elimNumY = 0x5F;
            suffixX = 0xEE;     suffixY = 0x13;
            badLuckX = 0xD6;    badLuckY = 0x07;
            destroyedX = 0xC7;  destroyedY = 0x14;
            outOfTimeX = 0xD1;  outOfTimeY = 0x14;
            break;
        case 7:
            lapLabelX = 0x36;   lapLabelY = 0xA5;
            lapNumX = 0x49;     lapNumY = 0xA5;
            lapTimeX = 0x55;    lapTimeY = 0xA5;
            titleX = 0x2D;      titleY = 0x7F;
            posNumX = 0x57;     posNumY = 0x8B;
            statsX = 0x30;      statsY = 0x9B;
            raceLabelX = 0x36;  raceLabelY = 0xCD;
            raceTimeX = 0x55;   raceTimeY = 0xCD;
            elimLabelX = 0x32;  elimLabelY = 0xD7;
            elimNumX = 0x76;    elimNumY = 0xD7;
            suffixX = 0x59;     suffixY = 0x8B;
            badLuckX = 0x40;    badLuckY = 0x7F;
            destroyedX = 0x31;  destroyedY = 0x8C;
            outOfTimeX = 0x3B;  outOfTimeY = 0x8C;
            break;
        case 8:
            lapLabelX = 0xCC;   lapLabelY = 0xA5;
            lapNumX = 0xDF;     lapNumY = 0xA5;
            lapTimeX = 0xEB;    lapTimeY = 0xA5;
            titleX = 0xC2;      titleY = 0x7F;
            posNumX = 0xEC;     posNumY = 0x8B;
            statsX = 0xC5;      statsY = 0x9B;
            raceLabelX = 0xCC;  raceLabelY = 0xCD;
            raceTimeX = 0xEB;   raceTimeY = 0xCD;
            elimLabelX = 0xC7;  elimLabelY = 0xD7;
            elimNumX = 0x10B;   elimNumY = 0xD7;
            suffixX = 0xEE;     suffixY = 0x8B;
            badLuckX = 0xD6;    badLuckY = 0x7F;
            destroyedX = 0xC7;  destroyedY = 0x8C;
            outOfTimeX = 0xD1;  outOfTimeY = 0x8C;
            break;
#else
        case 1:
            lapLabelX = 0x36;   lapLabelY = 0x6C;
            lapNumX = 0x49;     lapNumY = 0x6C;
            lapTimeX = 0x55;    lapTimeY = 0x6C;
            titleX = 0x2D;      titleY = 0x44;
            posNumX = 0x57;     posNumY = 0x4F;
            statsX = 0x30;      statsY = 0x62;
            raceLabelX = 0x36;  raceLabelY = 0x94;
            raceTimeX = 0x55;   raceTimeY = 0x94;
            elimLabelX = 0x32;  elimLabelY = 0x9E;
            elimNumX = 0x76;    elimNumY = 0x9E;
            suffixX = 0x58;     suffixY = 0x4F;
            badLuckX = 0x41;    badLuckY = 0x44;
            destroyedX = 0x31;  destroyedY = 0x4E;
            outOfTimeX = 0x3B;  outOfTimeY = 0x4E;
            break;
        case 2:
            lapLabelX = 0xCC;   lapLabelY = 0x6C;
            lapNumX = 0xDF;     lapNumY = 0x6C;
            lapTimeX = 0xEB;    lapTimeY = 0x6C;
            titleX = 0xC2;      titleY = 0x44;
            posNumX = 0xEC;     posNumY = 0x4F;
            statsX = 0xC5;      statsY = 0x62;
            raceLabelX = 0xCC;  raceLabelY = 0x94;
            raceTimeX = 0xEB;   raceTimeY = 0x94;
            elimLabelX = 0xC7;  elimLabelY = 0x9E;
            elimNumX = 0x10B;   elimNumY = 0x9E;
            suffixX = 0xED;     suffixY = 0x4F;
            badLuckX = 0xD6;    badLuckY = 0x44;
            destroyedX = 0xC7;  destroyedY = 0x4E;
            outOfTimeX = 0xD1;  outOfTimeY = 0x4E;
            break;
        case 3:
            lapLabelX = 0x80;   lapLabelY = 0x3A;
            lapNumX = 0x93;     lapNumY = 0x3A;
            lapTimeX = 0x9F;    lapTimeY = 0x3A;
            titleX = 0x55;      titleY = 0x14;
            posNumX = 0xA0;     posNumY = 0x21;
            statsX = 0x7A;      statsY = 0x30;
            raceLabelX = 0x80;  raceLabelY = 0x62;
            raceTimeX = 0x9F;   raceTimeY = 0x62;
            elimLabelX = 0x7B;  elimLabelY = 0x6C;
            elimNumX = 0xBF;    elimNumY = 0x6C;
            suffixX = 0xA1;     suffixY = 0x21;
            badLuckX = 0x77;    badLuckY = 0x14;
            destroyedX = 0x7B;  destroyedY = 0x21;
            outOfTimeX = 0x85;  outOfTimeY = 0x21;
            break;
        case 4:
            lapLabelX = 0x80;   lapLabelY = 0xA2;
            lapNumX = 0x93;     lapNumY = 0xA2;
            lapTimeX = 0x9F;    lapTimeY = 0xA2;
            titleX = 0x55;      titleY = 0x7F;
            posNumX = 0xA0;     posNumY = 0x8B;
            statsX = 0x7A;      statsY = 0x98;
            raceLabelX = 0x80;  raceLabelY = 0xCA;
            raceTimeX = 0x9F;   raceTimeY = 0xCA;
            elimLabelX = 0x7B;  elimLabelY = 0xD4;
            elimNumX = 0xBF;    elimNumY = 0xD4;
            suffixX = 0xA1;     suffixY = 0x8B;
            badLuckX = 0x77;    badLuckY = 0x7F;
            destroyedX = 0x7B;  destroyedY = 0x8C;
            outOfTimeX = 0x85;  outOfTimeY = 0x8C;
            break;
        case 5:
            lapLabelX = 0x36;   lapLabelY = 0x3A;
            lapNumX = 0x49;     lapNumY = 0x3A;
            lapTimeX = 0x55;    lapTimeY = 0x3A;
            titleX = 0x2D;      titleY = 0x18;
            posNumX = 0x58;     posNumY = 0x21;
            statsX = 0x30;      statsY = 0x30;
            raceLabelX = 0x36;  raceLabelY = 0x62;
            raceTimeX = 0x55;   raceTimeY = 0x62;
            elimLabelX = 0x32;  elimLabelY = 0x6C;
            elimNumX = 0x76;    elimNumY = 0x6C;
            suffixX = 0x59;     suffixY = 0x21;
            badLuckX = 0x41;    badLuckY = 0x18;
            destroyedX = 0x31;  destroyedY = 0x21;
            outOfTimeX = 0x3B;  outOfTimeY = 0x21;
            break;
        case 6:
            lapLabelX = 0xCC;   lapLabelY = 0x3A;
            lapNumX = 0xDF;     lapNumY = 0x3A;
            lapTimeX = 0xEB;    lapTimeY = 0x3A;
            titleX = 0xC2;      titleY = 0x18;
            posNumX = 0xED;     posNumY = 0x21;
            statsX = 0xC5;      statsY = 0x30;
            raceLabelX = 0xCC;  raceLabelY = 0x62;
            raceTimeX = 0xEB;   raceTimeY = 0x62;
            elimLabelX = 0xC7;  elimLabelY = 0x6C;
            elimNumX = 0x10B;   elimNumY = 0x6C;
            suffixX = 0xEE;     suffixY = 0x21;
            badLuckX = 0xD6;    badLuckY = 0x18;
            destroyedX = 0xC7;  destroyedY = 0x21;
            outOfTimeX = 0xD1;  outOfTimeY = 0x21;
            break;
        case 7:
            lapLabelX = 0x36;   lapLabelY = 0xA2;
            lapNumX = 0x49;     lapNumY = 0xA2;
            lapTimeX = 0x55;    lapTimeY = 0xA2;
            titleX = 0x2D;      titleY = 0x7F;
            posNumX = 0x58;     posNumY = 0x88;
            statsX = 0x30;      statsY = 0x98;
            raceLabelX = 0x36;  raceLabelY = 0xCA;
            raceTimeX = 0x55;   raceTimeY = 0xCA;
            elimLabelX = 0x32;  elimLabelY = 0xD4;
            elimNumX = 0x76;    elimNumY = 0xD4;
            suffixX = 0x59;     suffixY = 0x88;
            badLuckX = 0x41;    badLuckY = 0x7F;
            destroyedX = 0x31;  destroyedY = 0x8C;
            outOfTimeX = 0x3B;  outOfTimeY = 0x8C;
            break;
        case 8:
            lapLabelX = 0xCC;   lapLabelY = 0xA2;
            lapNumX = 0xDF;     lapNumY = 0xA2;
            lapTimeX = 0xEB;    lapTimeY = 0xA2;
            titleX = 0xC2;      titleY = 0x7F;
            posNumX = 0xED;     posNumY = 0x88;
            statsX = 0xC5;      statsY = 0x98;
            raceLabelX = 0xCC;  raceLabelY = 0xCA;
            raceTimeX = 0xEB;   raceTimeY = 0xCA;
            elimLabelX = 0xC7;  elimLabelY = 0xD4;
            elimNumX = 0x10B;   elimNumY = 0xD4;
            suffixX = 0xEE;     suffixY = 0x88;
            badLuckX = 0xD6;    badLuckY = 0x7F;
            destroyedX = 0xC7;  destroyedY = 0x8C;
            outOfTimeX = 0xD1;  outOfTimeY = 0x8C;
            break;
#endif
    }

    if (g_hudFlashState >= 16) {
        g_hudFlashState = 0;
    }

    for (i = 0; i < frontendOptions_g.numPlayers; i++) {
        if (g_playerFinished[i] != 2) {
            allEliminated = 0;
        }
    }

    if (frontendOptions_g.numPlayers == 1) {
        if (allEliminated) {
            if (shipList[ownShip].combatAttr & 4) {
                glistp = CentreText("BAD LUCK", badLuckX, badLuckY, 12, 0, glistp);
                glistp = CentreText("YOU RAN OUT OF TIME", 15, 60, 8, 0, glistp);
                if (g_timeoutSfxPending != 0) {
                    PlayNote(0x38, 0xFF, 15, 15, 0x100);
                }
                g_timeoutSfxPending = 0;
            } else if (g_shipDeathCount[ownShip] < 8) {
                glistp = CentreText("BAD LUCK", badLuckX, badLuckY, 12, 0, glistp);
                glistp = CentreText("YOUR SHIP WAS DESTROYED", 45, 60, 8, 0, glistp);
            }
            score->totalTime = 0;
            if (shipList[ownShip].lap >= 2) {
                glistp = AddText("RACE STATISTICS", statsX, statsY, 8, 0, glistp);
                glistp = AddText("ELIMINATIONS", elimLabelX, elimLabelY, 8, 0, glistp);
                glistp = AddNumber(shipList[ownShip].eliminations, elimNumX, elimNumY, 8, 1, glistp);
                for (lap = 0; lap < shipList[ownShip].lap - 1; lap++) {
                    glistp = AddText("LAP", lapLabelX, lapLabelY + lap * 12, 8, 0, glistp);
                    glistp = AddNumber(lap + 1, lapNumX, lapNumY + lap * 12, 8, 0, glistp);
                    glistp = AddTime(g_savedLapTimes[lap][0], lapTimeX, lapTimeY + lap * 12, 8, 1, 1, glistp);
                    score->totalTime += g_savedLapTimes[lap][0];
                }
            }
        } else if (g_playerFinished[0] != 2) {
            if (g_gameType == 6) {
                if (frontendOptions_g.challengeMode == 1) {
                    if (frontendOptions_g.challengeIndex == 6) {
                        if ((f32)g_finishPosition[0] == g_raceChallenges[frontendOptions_g.challengeIndex].goldTarget1) {
                            strcpy(titleBuf, "CONGRATULATIONS!");
                            g_challengeMedalLevel = 3;
                            if (g_raceChallenges[frontendOptions_g.challengeIndex].award < 3) {
                                g_raceChallenges[frontendOptions_g.challengeIndex].award = 3;
                            }
                        } else {
                            strcpy(titleBuf, "BAD LUCK");
                        }
                    } else if ((f32)g_finishPosition[0] <= g_raceChallenges[frontendOptions_g.challengeIndex].bronzeTarget1) {
                        strcpy(titleBuf, "CONGRATULATIONS!");
                        if ((f32)g_finishPosition[0] == g_raceChallenges[frontendOptions_g.challengeIndex].bronzeTarget1) {
                            g_challengeMedalLevel = 1;
                            if (g_raceChallenges[frontendOptions_g.challengeIndex].award < 1) {
                                g_raceChallenges[frontendOptions_g.challengeIndex].award = 1;
                            }
                        } else if ((f32)g_finishPosition[0] == g_raceChallenges[frontendOptions_g.challengeIndex].silverTarget1) {
                            g_challengeMedalLevel = 2;
                            if (g_raceChallenges[frontendOptions_g.challengeIndex].award < 2) {
                                g_raceChallenges[frontendOptions_g.challengeIndex].award = 2;
                            }
                        } else if ((f32)g_finishPosition[0] == g_raceChallenges[frontendOptions_g.challengeIndex].goldTarget1) {
                            g_challengeMedalLevel = 3;
                            if (g_raceChallenges[frontendOptions_g.challengeIndex].award < 3) {
                                g_raceChallenges[frontendOptions_g.challengeIndex].award = 3;
                            }
                        }
                    } else {
                        strcpy(titleBuf, "BAD LUCK");
                    }
                } else if (frontendOptions_g.challengeMode == 2) {
                    if (frontendOptions_g.challengeIndex == 6) {
                        if ((f32)g_savedLapTimes[0][0] <= g_ttChallenges[frontendOptions_g.challengeIndex].goldTarget1) {
                            g_challengeMedalLevel = 3;
                            if (g_ttChallenges[frontendOptions_g.challengeIndex].award < 3) {
                                g_ttChallenges[frontendOptions_g.challengeIndex].award = 3;
                            }
                            strcpy(titleBuf, "CONGRATULATIONS!");
                        } else {
                            strcpy(titleBuf, "BAD LUCK");
                        }
                    } else if ((f32)g_savedLapTimes[0][0] <= g_ttChallenges[frontendOptions_g.challengeIndex].bronzeTarget1) {
                        strcpy(titleBuf, "CONGRATULATIONS!");
                        if ((f32)g_savedLapTimes[0][0] <= g_ttChallenges[frontendOptions_g.challengeIndex].bronzeTarget1) {
                            g_challengeMedalLevel = 1;
                            if (g_ttChallenges[frontendOptions_g.challengeIndex].award < 1) {
                                g_ttChallenges[frontendOptions_g.challengeIndex].award = 1;
                            }
                        }
                        if ((f32)g_savedLapTimes[0][0] <= g_ttChallenges[frontendOptions_g.challengeIndex].silverTarget1) {
                            g_challengeMedalLevel = 2;
                            if (g_ttChallenges[frontendOptions_g.challengeIndex].award < 2) {
                                g_ttChallenges[frontendOptions_g.challengeIndex].award = 2;
                            }
                        }
                        if ((f32)g_savedLapTimes[0][0] <= g_ttChallenges[frontendOptions_g.challengeIndex].goldTarget1) {
                            g_challengeMedalLevel = 3;
                            if (g_ttChallenges[frontendOptions_g.challengeIndex].award < 3) {
                                g_ttChallenges[frontendOptions_g.challengeIndex].award = 3;
                            }
                        }
                    } else {
                        strcpy(titleBuf, "BAD LUCK");
                    }
                } else if (frontendOptions_g.challengeMode == 3) {
                    if (frontendOptions_g.challengeIndex == 6) {
                        if ((f32)shipList[ownShip].eliminations >= g_weaponChallenges[frontendOptions_g.challengeIndex].goldTarget1) {
                            g_challengeMedalLevel = 3;
                            if (g_weaponChallenges[frontendOptions_g.challengeIndex].award < 3) {
                                g_weaponChallenges[frontendOptions_g.challengeIndex].award = 3;
                            }
                            strcpy(titleBuf, "CONGRATULATIONS!");
                        } else {
                            strcpy(titleBuf, "BAD LUCK");
                        }
                    } else if ((f32)shipList[ownShip].eliminations >= g_weaponChallenges[frontendOptions_g.challengeIndex].bronzeTarget1) {
                        strcpy(titleBuf, "CONGRATULATIONS!");
                        if ((f32)shipList[ownShip].eliminations >= g_weaponChallenges[frontendOptions_g.challengeIndex].bronzeTarget1) {
                            g_challengeMedalLevel = 1;
                            if (g_weaponChallenges[frontendOptions_g.challengeIndex].award < 1) {
                                g_weaponChallenges[frontendOptions_g.challengeIndex].award = 1;
                            }
                        }
                        if ((f32)shipList[ownShip].eliminations >= g_weaponChallenges[frontendOptions_g.challengeIndex].silverTarget1) {
                            g_challengeMedalLevel = 2;
                            if (g_weaponChallenges[frontendOptions_g.challengeIndex].award < 2) {
                                g_weaponChallenges[frontendOptions_g.challengeIndex].award = 2;
                            }
                        }
                        if ((f32)shipList[ownShip].eliminations >= g_weaponChallenges[frontendOptions_g.challengeIndex].goldTarget1) {
                            g_challengeMedalLevel = 3;
                            if (g_weaponChallenges[frontendOptions_g.challengeIndex].award < 3) {
                                g_weaponChallenges[frontendOptions_g.challengeIndex].award = 3;
                            }
                        }
                    } else {
                        strcpy(titleBuf, "BAD LUCK");
                    }
                } else if (frontendOptions_g.challengeMode == 4) {
                    if (frontendOptions_g.challengeIndex == 6) {
                        if ((f32)g_finishPosition[0] <= g_superComboChallenges[frontendOptions_g.challengeIndex].goldTarget1 &&
                            (f32)shipList[ownShip].eliminations >= g_superComboChallenges[frontendOptions_g.challengeIndex].goldTarget2) {
                            g_challengeMedalLevel = 3;
                            if (g_superComboChallenges[frontendOptions_g.challengeIndex].award < 3) {
                                g_superComboChallenges[frontendOptions_g.challengeIndex].award = 3;
                            }
                            strcpy(titleBuf, "CONGRATULATIONS!");
                        } else {
                            strcpy(titleBuf, "BAD LUCK");
                        }
                    } else if ((f32)g_finishPosition[0] <= g_superComboChallenges[frontendOptions_g.challengeIndex].bronzeTarget1 &&
                               (f32)shipList[ownShip].eliminations >= g_superComboChallenges[frontendOptions_g.challengeIndex].bronzeTarget2) {
                        strcpy(titleBuf, "CONGRATULATIONS!");
                        if ((f32)g_finishPosition[0] <= g_superComboChallenges[frontendOptions_g.challengeIndex].bronzeTarget1 &&
                            (f32)shipList[ownShip].eliminations >= g_superComboChallenges[frontendOptions_g.challengeIndex].bronzeTarget2) {
                            g_challengeMedalLevel = 1;
                            if (g_superComboChallenges[frontendOptions_g.challengeIndex].award < 1) {
                                g_superComboChallenges[frontendOptions_g.challengeIndex].award = 1;
                            }
                        }
                        if ((f32)g_finishPosition[0] <= g_superComboChallenges[frontendOptions_g.challengeIndex].silverTarget1 &&
                            (f32)shipList[ownShip].eliminations >= g_superComboChallenges[frontendOptions_g.challengeIndex].silverTarget2) {
                            g_challengeMedalLevel = 2;
                            if (g_superComboChallenges[frontendOptions_g.challengeIndex].award < 2) {
                                g_superComboChallenges[frontendOptions_g.challengeIndex].award = 2;
                            }
                        }
                        if ((f32)g_finishPosition[0] <= g_superComboChallenges[frontendOptions_g.challengeIndex].goldTarget1 &&
                            (f32)shipList[ownShip].eliminations >= g_superComboChallenges[frontendOptions_g.challengeIndex].goldTarget2) {
                            g_challengeMedalLevel = 3;
                            if (g_superComboChallenges[frontendOptions_g.challengeIndex].award < 3) {
                                g_superComboChallenges[frontendOptions_g.challengeIndex].award = 3;
                            }
                        }
                    } else {
                        strcpy(titleBuf, "BAD LUCK");
                    }
                }
            }

            if (g_gameType != 1 && g_gameType != 3) {
                if (g_gameType != 6) {
                    if (g_finishPosition[0] < 4) {
                        strcpy(titleBuf, "CONGRATULATIONS!");
                    } else {
                        strcpy(titleBuf, "BAD LUCK");
                    }
                }

                if (g_gameType == 6) {
                    glistp = CentreText(titleBuf, 0x5D, 0x14, 12, 0, glistp);
                } else {
                    glistp = CentreText(titleBuf, titleX, titleY, 12, 0, glistp);
                }

                if (g_gameType == 6) {
                    if (frontendOptions_g.challengeMode == 1) {
                        if (frontendOptions_g.challengeIndex == 6) {
                            glistp = CentreText("GOLD RACE CHALLENGE", 0x49, 0x28, 8, 1, glistp);
                        } else {
                            glistp = AddText("RACE CHALLENGE ", 0x49, 0x28, 8, 1, glistp);
                            glistp = AddNumber(frontendOptions_g.challengeIndex + 1, 0xF0, 0x28, 8, 1, glistp);
                        }
                    } else if (frontendOptions_g.challengeMode == 2) {
                        if (frontendOptions_g.challengeIndex == 6) {
                            glistp = CentreText("GOLD TIME TRIAL CHALLENGE", 0x49, 0x28, 8, 1, glistp);
                        } else {
                            glistp = AddText("TIME TRIAL CHALLENGE ", 0x31, 0x28, 8, 1, glistp);
                            glistp = AddNumber(frontendOptions_g.challengeIndex + 1, 0x109, 0x28, 8, 1, glistp);
                        }
                    } else if (frontendOptions_g.challengeMode == 3) {
                        if (frontendOptions_g.challengeIndex == 6) {
                            glistp = CentreText("GOLD WEAPON CHALLENGE", 0x49, 0x28, 8, 1, glistp);
                        } else {
                            strcpy(titleBuf, "WEAPON CHALLENGE ");
                            glistp = AddText(titleBuf, 0x3A, 0x28, 8, 1, glistp);
                            glistp = AddNumber(frontendOptions_g.challengeIndex + 1, 0xFE, 0x28, 8, 1, glistp);
                        }
                    } else if (frontendOptions_g.challengeMode == 4) {
                        if (frontendOptions_g.challengeIndex == 6) {
                            glistp = CentreText("GOLD SUPER COMBO CHALLENGE", 0x49, 0x28, 8, 1, glistp);
                        } else {
                            strcpy(titleBuf, "SUPER COMBO CHALLENGE ");
                            glistp = AddText(titleBuf, 0x21, 0x28, 8, 1, glistp);
                            glistp = AddNumber(frontendOptions_g.challengeIndex + 1, 0x118, 0x28, 8, 1, glistp);
                        }
                    }

                    if (g_gameType == 6 && frontendOptions_g.challengeMode == 2) {
                        glistp = AddText("AWARD", 0x82, 0x5F, 8, 0, glistp);
                    } else {
                        glistp = AddText("AWARD", 0xCB, 0x5F, 8, 0, glistp);
                    }

                    if (g_challengeMedalLevel == 3) {
                        if (g_gameType == 6 && frontendOptions_g.challengeMode == 2) {
                            glistp = DrawFontText("GOLD", 0x96, 0xAF, 8, glistp);
                        } else {
                            glistp = DrawFontText("GOLD", 0xE1, 0xAF, 8, glistp);
                        }
                        if (g_resultSfxPending != 0) {
                            PlayNote(0x34, 0xFF, 15, 15, 0x100);
                        }
                    } else if (g_challengeMedalLevel == 2) {
                        if (g_gameType == 6 && frontendOptions_g.challengeMode == 2) {
                            glistp = DrawFontText("SILVER", 0x92, 0xAF, 8, glistp);
                        } else {
                            glistp = DrawFontText("SILVER", 0xDD, 0xAF, 8, glistp);
                        }
                        if (g_resultSfxPending != 0) {
                            PlayNote(0x35, 0xFF, 15, 15, 0x100);
                        }
                    } else if (g_challengeMedalLevel == 1) {
                        if (g_gameType == 6 && frontendOptions_g.challengeMode == 2) {
                            glistp = DrawFontText("BRONZE", 0x90, 0xAF, 8, glistp);
                        } else {
                            glistp = DrawFontText("BRONZE", 0xDB, 0xAF, 8, glistp);
                        }
                        if (g_resultSfxPending != 0) {
                            PlayNote(0x36, 0xFF, 15, 15, 0x100);
                        }
                    } else {
                        if (g_gameType == 6 && frontendOptions_g.challengeMode == 2) {
                            glistp = DrawFontText("NO AWARD", 0x8C, 0xAF, 8, glistp);
                        } else {
                            glistp = DrawFontText("NO AWARD", 0xD7, 0xAF, 8, glistp);
                        }
                        if (g_resultSfxPending != 0) {
                            PlayNote(0x37, 0xFF, 15, 15, 0x100);
                        }
                    }
                }

                switch (g_finishPosition[0]) {
                    case 1:
                        strcpy(suffixBuf, "ST");
                        if (g_resultSfxPending != 0 && g_gameType != 6) {
                            PlayNote(0x31, 0xFF, 15, 15, 0x100);
                        }
                        break;
                    case 2:
                        strcpy(suffixBuf, "ND");
                        if (g_resultSfxPending != 0 && g_gameType != 6) {
                            PlayNote(0x32, 0xFF, 15, 15, 0x100);
                        }
                        break;
                    case 3:
                        strcpy(suffixBuf, "RD");
                        if (g_resultSfxPending != 0 && g_gameType != 6) {
                            PlayNote(0x33, 0xFF, 15, 15, 0x100);
                        }
                        break;
                    default:
                        strcpy(suffixBuf, "TH");
                        if (g_resultSfxPending != 0 && g_gameType != 6) {
                            PlayNote(0x2F, 0xFF, 15, 15, 0x100);
                        }
                        break;
                }

                if (g_gameType == 6 && frontendOptions_g.challengeMode == 2) {
                    glistp = AddTime(g_savedLapTimes[0][0], 0x7B, 0x35, 16, 1, 1, glistp);
                } else if (g_gameType == 6 && frontendOptions_g.challengeMode == 3) {
                    glistp = DrawButtonPrompt(0x8F, 0x35, 14, NULL, glistp);
                    glistp = AddNumber(shipList[ownShip].eliminations, 0xA3, 0x37, 12, 1, glistp);
                } else if (g_gameType == 6 && frontendOptions_g.challengeMode == 4) {
                    glistp = DrawButtonPrompt(0xAD, 0x35, 14, NULL, glistp);
                    glistp = AddNumber(shipList[ownShip].eliminations, 0xC1, 0x37, 12, 1, glistp);
                    glistp = AddNumber(g_finishPosition[0], 0x76, 0x37, 12, 1, glistp);
                    glistp = AddText(suffixBuf, g_textEndX + 2, 0x37, 8, 1, glistp);
                } else {
                    glistp = AddNumber(g_finishPosition[0], posNumX, posNumY, 12, 1, glistp);
                    glistp = AddText(suffixBuf, g_textEndX + 2, suffixY, 8, 1, glistp);
                }

                if (g_gameType == 6) {
                    if (frontendOptions_g.challengeMode != 2) {
                        glistp = AddText("ELIMINATIONS", 0x21, 0xC3, 8, 0, glistp);
                        glistp = AddNumber(shipList[ownShip].eliminations, 0xA3, 0xC3, 8, 1, glistp);
                    }
                } else {
                    glistp = AddText("ELIMINATIONS", elimLabelX, elimLabelY, 8, 0, glistp);
                    glistp = AddNumber(shipList[ownShip].eliminations, elimNumX, elimNumY, 8, 1, glistp);
                }
            }
            if (g_gameType == 3 && g_resultSfxPending != 0) {
                PlayNote(0x2F, 0xFF, 15, 15, 0x100);
            }

            if (g_gameType == 6) {
                if (frontendOptions_g.challengeMode != 2) {
                    glistp = AddText("RACE STATISTICS", 0x21, 0x55, 8, 0, glistp);
                }
            } else {
                glistp = CentreText("RACE STATISTICS", statsX, statsY, 8, 0, glistp);
            }

            score->totalTime = 0;
            for (lap = 0; lap < g_classLaps[config->shipClass]; lap++) {
                if (g_gameType == 6) {
                    if (frontendOptions_g.challengeMode != 2) {
                        glistp = AddText("LAP", 0x21, lap * 12 + 0x69, 8, 0, glistp);
                        if (frontendOptions_g.challengeMode != 2) {
                            glistp = AddNumber(lap + 1, 0x49, lap * 12 + 0x69, 8, 0, glistp);
                        }
                        glistp = AddTime(g_savedLapTimes[lap][0], 0x5D, lap * 12 + 0x69, 8, 1, 1, glistp);
                    }
                } else {
                    glistp = AddText("LAP", lapLabelX, lapLabelY + lap * 12, 8, 0, glistp);
                    glistp = AddNumber(lap + 1, lapNumX, lapNumY + lap * 12, 8, 0, glistp);
                    glistp = AddTime(g_savedLapTimes[lap][0], lapTimeX, lapTimeY + lap * 12, 8, 1, 1, glistp);
                }
                score->totalTime += g_savedLapTimes[lap][0];
            }

            if ((g_gameType == 0 || g_gameType == 4) && g_finishPosition[0] < 4) {
                InitFontUnused(0x46, 0, 0xB4, g_raceInfoOverlayArg, 0, g_raceInfoFontCtx);
            }

            if (g_gameType == 6 && frontendOptions_g.challengeMode != 2) {
                glistp = AddText("RACE", 0x21, 0xAB, 8, 0, glistp);
            } else if (g_gameType != 6) {
                glistp = AddText("RACE", raceLabelX, raceLabelY, 8, 0, glistp);
            }

            if (g_gameType == 6 && frontendOptions_g.challengeMode != 2) {
                glistp = AddTime(score->totalTime, 0x5D, 0xAB, 8, 1, 1, glistp);
            } else if (g_gameType != 6) {
                if (g_gameType == 0 || g_gameType == 4 || g_gameType == 1) {
                    glistp = AddTime(score->totalTime, raceTimeX, raceTimeY, 8, 1, 1, glistp);
                } else {
                    for (lap2 = 0; lap2 < g_classLaps[config->shipClass]; lap2++) {
                        totalTime += g_savedLapTimes[lap2][0];
                    }
                    glistp = AddTime(totalTime, 0x80, 0xB4, 8, 1, 1, glistp);
                }
            }
        }
    }

    if (player == 0) {
        playerShip = ownShip;
    } else if (player == 1) {
        playerShip = ownShip2;
    } else if (player == 2) {
        playerShip = ownShip3;
    } else if (player == 3) {
        playerShip = ownShip4;
    }

    if (frontendOptions_g.numPlayers >= 2) {
        if (g_playerFinished[player] == 2) {
            if (shipList[playerShip].combatAttr & 4) {
                if (frontendOptions_g.numPlayers == 1) {
                    glistp = AddText("BAD LUCK", badLuckX, badLuckY, 8, 0, glistp);
                } else if (layout == 3 || layout == 4) {
                    glistp = AddText("BAD LUCK", badLuckX, badLuckY, 8, 0, glistp);
                } else {
                    glistp = DrawFontTextYellow("BAD LUCK", badLuckX, badLuckY, 8, glistp);
                }
                glistp = DrawFontText("OUT OF TIME", outOfTimeX, outOfTimeY, 8, glistp);
            } else if (g_shipDeathCount[playerShip] < 8) {
                if (frontendOptions_g.numPlayers == 1) {
                    glistp = AddText("BAD LUCK", badLuckX, badLuckY, 8, 0, glistp);
                } else if (layout == 3 || layout == 4) {
                    glistp = AddText("BAD LUCK", badLuckX, badLuckY, 8, 0, glistp);
                } else {
                    glistp = DrawFontTextYellow("BAD LUCK", badLuckX, badLuckY, 8, glistp);
                }
                glistp = DrawFontText("SHIP DESTROYED", destroyedX, destroyedY, 8, glistp);
            }
            if (shipList[playerShip].lap >= 2) {
                glistp = DrawFontTextYellow("ELIMINATIONS", elimLabelX, elimLabelY, 8, glistp);
                NumberToString(shipList[playerShip].eliminations, numBuf);
                glistp = DrawFontText(numBuf, elimNumX, elimNumY, 8, glistp);
                glistp = DrawFontTextYellow("RACE STATISTICS", statsX, statsY, 8, glistp);
                for (lap = 0; lap < shipList[playerShip].lap - 1; lap++) {
                    glistp = DrawFontTextYellow("LAP", lapLabelX, lapLabelY + lap * 8, 8, glistp);
                    NumberToString(lap + 1, numBuf);
                    glistp = DrawFontTextYellow(numBuf, lapNumX, lapNumY + lap * 8, 8, glistp);
                    TimeToString(g_savedLapTimes[lap][player], 1, numBuf);
                    glistp = DrawFontText(numBuf, lapTimeX, lapTimeY + lap * 8, 8, glistp);
                }
            }
        } else {
            glistp = DrawFontTextYellow("RACE", raceLabelX, raceLabelY, 8, glistp);
            totalTime = 0;
            for (lap2 = 0; lap2 < g_classLaps[config->shipClass]; lap2++) {
                totalTime += g_savedLapTimes[lap2][player];
            }
            TimeToString(totalTime, 1, numBuf);
            glistp = DrawFontText(numBuf, raceTimeX, raceTimeY, 8, glistp);

            if (g_finishPosition[player] == 1) {
                if (frontendOptions_g.numPlayers < 3) {
                    if (layout == 1 || layout == 2) {
                        glistp = DrawFontTextYellow("CONGRATULATIONS", titleX, titleY, 8, glistp);
                    } else {
                        glistp = AddText("CONGRATULATIONS", titleX, titleY, 8, 0, glistp);
                    }
                } else {
                    glistp = DrawFontTextYellow("CONGRATULATIONS", titleX, titleY, 8, glistp);
                }
            } else {
                if (frontendOptions_g.numPlayers < 3) {
                    if (layout == 1 || layout == 2) {
                        glistp = DrawFontTextYellow("BAD LUCK", badLuckX, badLuckY, 8, glistp);
                    } else {
                        glistp = AddText("BAD LUCK", badLuckX, badLuckY, 8, 0, glistp);
                    }
                } else {
                    glistp = DrawFontTextYellow("BAD LUCK", badLuckX, badLuckY, 8, glistp);
                }
            }

            glistp = AddJustifyNumber(g_finishPosition[player], posNumX, posNumY, 8, 1, glistp);
            switch (g_finishPosition[player]) {
                case 1:
                    strcpy(suffixBuf, "ST");
                    break;
                case 2:
                    strcpy(suffixBuf, "ND");
                    break;
                case 3:
                    strcpy(suffixBuf, "RD");
                    break;
                default:
                    strcpy(suffixBuf, "TH");
                    break;
            }
            glistp = DrawFontText(suffixBuf, suffixX, suffixY, 8, glistp);
            glistp = DrawFontTextYellow("ELIMINATIONS", elimLabelX, elimLabelY, 8, glistp);
            NumberToString(shipList[playerShip].eliminations, numBuf);
            glistp = DrawFontText(numBuf, elimNumX, elimNumY, 8, glistp);
            glistp = DrawFontTextYellow("RACE STATISTICS", statsX, statsY, 8, glistp);
            for (lap = 0; lap < g_classLaps[config->shipClass]; lap++) {
                glistp = DrawFontTextYellow("LAP", lapLabelX, lapLabelY + lap * 8, 8, glistp);
                NumberToString(lap + 1, numBuf);
                glistp = DrawFontTextYellow(numBuf, lapNumX, lapNumY + lap * 8, 8, glistp);
                TimeToString(g_savedLapTimes[lap][player], 1, numBuf);
                glistp = DrawFontText(numBuf, lapTimeX, lapTimeY + lap * 8, 8, glistp);
            }
        }
    }

    g_raceInfoFrameCount++;
    return glistp;
}

Gfx *AlterCheckPoint(CheckPoint *checkPoints, ShipData *shipList, s32 pauseState, ConfigData *config,
                   s32 player, s32 shipNo, u32 layout, Gfx *glistp) {
    s32 x;
    s32 y;
    s32 numCheckPoints;
    s16 i;
    s32 *timer;
    Face *face;
    s32 totalSecs;
    s32 secs;
    s32 mins;
    s32 sfxHandle;
    OSTime now;

    switch (layout) {
        case 0:
            x = 0x5D;
            y = 0x5F;
            break;
        case 1:
        case 2:
            x = 0x32;
            y = 0x6E;
            break;
        case 3:
            x = 0x71;
            y = 0x42;
            break;
        case 4:
            x = 0x71;
            y = 0xB7;
            break;
        case 5:
            x = 0x33;
            y = 0x42;
            break;
        case 6:
            x = 0xCB;
            y = 0x42;
            break;
        case 7:
            x = 0x33;
            y = 0xAD;
            break;
        case 8:
            x = 0xCB;
            y = 0xAD;
            break;
    }

    if (frontendOptions_g.numPlayers >= 2) {
        switch (player) {
            case 2:
                timer = &checkTimer2;
                break;
            case 3:
                timer = &checkTimer3;
                break;
            case 4:
                timer = &checkTimer4;
                break;
            default:
                timer = &checkTimer;
                break;
        }
    } else {
        timer = &checkTimer;
    }

    switch (frontendOptions_g.trackNo) {
        case 1:
            numCheckPoints = 2;
            break;
        case 2:
            numCheckPoints = 2;
            break;
        case 3:
            numCheckPoints = 3;
            break;
        case 4:
            numCheckPoints = 2;
            break;
        case 5:
            numCheckPoints = 3;
            break;
        case 6:
            numCheckPoints = 3;
            break;
        case 7:
            numCheckPoints = 3;
            break;
    }

    face = (Face *)track->faces + shipList[shipNo].nearTrkSect->faceStart;
    while (!(face->flags & 1)) {
        face++;
    }

    if (pauseState == -1 && g_raceModeFlag == 0 && shipList[shipNo].lap < g_classLaps[config->shipClass] + 1 &&
        shipList[shipNo].lap >= 0) {
        if (g_gameType != 1 && *timer > 0) {
            if (g_gameType != 6 || frontendOptions_g.challengeMode != 2) {
                *timer -= 2;
                totalSecs = *timer / FRAMES_PER_SEC;
                mins = totalSecs / 60;
                secs = totalSecs % 60;
                if (secs < 11) {
                    if ((*timer / FRAMES_PER_TENTH) == totalSecs * 10 && mins == 0) {
                        s32 rem = 10 - totalSecs;
                        if ((frontendOptions_g.multiTimer != 0 || g_multiplayerFlag == 0) &&
                            g_shipDeathCount[shipNo] == pauseState) {
                            now = osGetTime();
                            if (now - g_lastCheckSfxTime > 100000) {
                                sfxHandle = MusStartEffect(13);
                                SetSfxFreqOffset(sfxHandle, (f32)rem * 0.5f);
                                MusHandleSetVolume(sfxHandle, 0x100);
                                g_lastCheckSfxTime = now;
                            }
                        }
                    }
                }
                if (g_infiniteTimeFlag != 0) {
                    *timer = MAX_TIMER_FRAMES;
                }
            }
        } else if (g_multiplayerFlag == 0) {
            if (g_gameType != 1 && noCheckPoints == 0 && !(shipList[shipNo].combatAttr & 0x8000)) {
                if (g_gameType != 6 || frontendOptions_g.challengeMode != 2) {
                    if (cheatTime == 0) {
                        shipList[shipNo].combatAttr |= 8;
                        if (shipList[shipNo].autopilotCount > 0) {
                            shipList[shipNo].autopilotCount = 0;
                            shipList[shipNo].update = UpdatePlayerShip;
                            shipList[shipNo].vhdg = 0;
                        }
                    }
                }
            }
        } else if (frontendOptions_g.multiTimer != 0) {
            shipList[shipNo].combatAttr |= 8;
            if (shipList[shipNo].autopilotCount > 0) {
                shipList[shipNo].autopilotCount = 0;
                shipList[shipNo].update = UpdatePlayerShip;
                shipList[shipNo].vhdg = 0;
            }
        }
    }

    if (shipList[shipNo].lap > 0) {
        for (i = 0; i < numCheckPoints; i++) {
            if (checkPoints[i].section == shipList[shipNo].nearTrkSect->secNo ||
                checkPoints[i].section == shipList[shipNo].nearTrkSect->secNo - 1 ||
                checkPoints[i].section == shipList[shipNo].nearTrkSect->secNo - 2) {
                if (checkPointReached[i][player - 1] < shipList[shipNo].lap &&
                    ((shipList[shipNo].attr & 8) ||
                     shipList[shipNo].nearTrkSect->junction.index != -1 ||
                     shipList[shipNo].nearTrkSect->nextSection.ptr->junction.index !=
                         shipList[shipNo].nearTrkSect->junction.index)) {
                    if (g_gameType != 1) {
                        if (g_gameType != 6 || frontendOptions_g.challengeMode != 2) {
                            *timer += checkPoints[i].time * FRAMES_PER_SEC;
                            g_checkPointBannerTimer[player - 1] = FRAMES_PER_SEC;
                            shipList[shipNo].combatAttr &= ~8;
                        }
                    } else {
                        if (checkPoints[i].section != startLinePos[frontendOptions_g.trackNo]) {
                            *timer = shipList[ownShip].lapTimes[shipList[ownShip].lap - 1] -
                                     g_ghostReplay.lapTimes[i];
                            g_showSplitDelta = 1;
                            g_prevSplitDelta = 0;
                        } else {
                            *timer = g_splitLapDelta;
                            g_prevSplitDelta = 0;
                            if (g_ghostLoaded != 0) {
                                g_showSplitDelta = g_ttLapCount > 0;
                            } else {
                                g_showSplitDelta = g_ttLapCount >= 2;
                            }
                        }
                        g_ghostLapTimes[i] = shipList[ownShip].lapTimes[shipList[ownShip].lap - 1];
                    }
                    checkPointReached[i][player - 1] = shipList[shipNo].lap;
                    PlayNote(13, 320, 15, 15, 0x100);
                }
                break;
            }
        }

        if (pauseState == -1 && (shipList[shipNo].attr & 0x40)) {
            if (shipList[shipNo].lap > 0 && g_checkPointBannerTimer[player - 1] > 0) {
                if ((g_checkPointFlashFrame % 30) < 21 && layout == 0) {
                    glistp = AddText("EXTENDED TIME", x, y, 8, 0, glistp);
                }
                g_checkPointBannerTimer[player - 1]--;
            }
            g_checkPointFlashFrame++;
        }
    }

    return glistp;
}

void DisplayContinues(void) {
    s16 i;

    for (i = 0; i < g_continuesLeft; i++) {
    }
}
