#include "plasma.h"

#ident "$Revision: 1.17 $"
#ident "$Revision: 7.27 $"
#ident "$Revision: 1.5 $"

void InitPlasmaTable1(void) {
    s32 y;
    s32 x;

    for (y = 0; y < 128; y++) {
        for (x = 0; x < 128; x++) {
            g_plasmaTable1[(y * 128) + x] =
                (u8)((sqrtf(((64 - y) * (64 - y) + 16.0) + (64 - x) * (64 - x)) - 4.0f) * 5.0f);
        }
    }
}

void InitPlasmaTable2(void) {
    s32 y;
    s32 x;
    f32 dist;

    for (y = 0; y < 128; y++) {
        for (x = 0; x < 128; x++) {
            dist = sqrtf(((64 - y) * (64 - y) + 16.0) + (64 - x) * (64 - x));
            g_plasmaTable2[(y * 128) + x] = (u8)((sinf((dist - 4.0f) / 9.5) + 1.0f) * 90.0f);
        }
    }
}

void InitPlasmaPalette(void) {
    s32 i;
    s32 r;
    s32 g;
    s32 b;

    i = 0;
    r = 0;
    b = 0;
    g = 0;

    for (; i < 32; ) {
        g_plasmaPalette[i] = ((r & 0x1F) << 11) + ((g & 0x1F) << 6) + ((b & 0x1F) << 1) + 1;
        i++;
        if ((i % 3) == 2) {
            b++;
        }
        if ((i % 5) == 2) {
            g++;
        }
    }

    for (i = 32; i < 64; ) {
        g_plasmaPalette[i] = ((r & 0x1F) << 11) + ((g & 0x1F) << 6) + ((b & 0x1F) << 1) + 1;
        i++;
        if ((i % 3) == 2) {
            g++;
        }
        if ((i % 2) == 1) {
            b++;
        }
    }

    for (i = 64; i < 96; ) {
        g_plasmaPalette[i] = ((r & 0x1F) << 11) + ((g & 0x1F) << 6) + ((b & 0x1F) << 1) + 1;
        i++;
        if ((i % 5) == 2) {
            b--;
        }
        if ((i % 2) == 1) {
            r++;
        }
        if ((i % 4) == 1) {
            g++;
        }
    }

    for (i = 96; i < 128; ) {
        g_plasmaPalette[i] = ((r & 0x1F) << 11) + ((g & 0x1F) << 6) + ((b & 0x1F) << 1) + 1;
        i++;
        if ((i % 5) == 2) {
            b++;
        }
        if ((i % 2) == 1) {
            r--;
        }
        if ((i % 4) == 1) {
            g--;
        }
    }

    for (i = 128; i < 160; ) {
        g_plasmaPalette[i] = ((r & 0x1F) << 11) + ((g & 0x1F) << 6) + ((b & 0x1F) << 1) + 1;
        i++;
        if ((i % 3) == 2) {
            g--;
        }
        if ((i % 2) == 1) {
            b--;
        }
    }

    for (i = 160; i < 192; ) {
        g_plasmaPalette[i] = ((r & 0x1F) << 11) + ((g & 0x1F) << 6) + ((b & 0x1F) << 1) + 1;
        i++;
        if ((i % 3) == 2) {
            b--;
        }
        if ((i % 5) == 2) {
            g--;
        }
    }

    for (i = 192; i < 224; ) {
        g_plasmaPalette[i] = ((r & 0x1F) << 11) + ((g & 0x1F) << 6) + ((b & 0x1F) << 1) + 1;
        i++;
        if ((i % 10) == 2) {
            g++;
        }
        if ((i % 5) == 1) {
            b++;
        }
    }

    for (i = 224; i < 256; ) {
        g_plasmaPalette[i] = ((r & 0x1F) << 11) + ((g & 0x1F) << 6) + ((b & 0x1F) << 1) + 1;
        i++;
        if ((i % 10) == 2) {
            g--;
        }
        if ((i % 5) == 1) {
            b--;
        }
    }
}

void InitPlasmaPaletteGold(void) {
    s32 i;
    s32 r;
    s32 g;
    s32 b;

    i = 0;
    r = 0;
    b = 0;
    g = 0;

    for (; i < 32; ) {
        g_plasmaPalette[i] = ((((u32)(u8)r * 31) / 255) << 11) + ((((u32)(u8)g * 31) / 255) << 6) + ((((u32)(u8)b * 31) / 255) << 1) + 1;
        i++;
        g = (s32)(i * 1.5f);
        b = (s32)(i * 2.75f);
    }

    for (i = 32; i < 64; ) {
        g_plasmaPalette[i] = ((((u32)(u8)r * 31) / 255) << 11) + ((((u32)(u8)g * 31) / 255) << 6) + ((((u32)(u8)b * 31) / 255) << 1) + 1;
        i++;
        r = (s32)((i - 32) * 5.21f + 0.0f);
        g = (s32)((i - 32) * 2.56f + 48.0f);
        b = (s32)(88.0f - (i - 32) * 2.65f);
    }

    for (i = 64; i < 96; ) {
        g_plasmaPalette[i] = ((((u32)(u8)r * 31) / 255) << 11) + ((((u32)(u8)g * 31) / 255) << 6) + ((((u32)(u8)b * 31) / 255) << 1) + 1;
        i++;
        r = (s32)((i - 64) * 2.43f + 167.0f);
        g = (s32)((i - 64) * 3.37f + 130.0f);
        b = (s32)((i - 64) * 1.84f + 3.0f);
    }

    for (i = 96; i < 128; ) {
        g_plasmaPalette[i] = ((((u32)(u8)r * 31) / 255) << 11) + ((((u32)(u8)g * 31) / 255) << 6) + ((((u32)(u8)b * 31) / 255) << 1) + 1;
        i++;
        r = (s32)(245.0f - (i - 96) * 2.43f);
        g = (s32)(238.0f - (i - 96) * 3.37f);
        b = (s32)(62.0f - (i - 96) * 1.84f);
    }

    for (i = 128; i < 160; ) {
        g_plasmaPalette[i] = ((((u32)(u8)r * 31) / 255) << 11) + ((((u32)(u8)g * 31) / 255) << 6) + ((((u32)(u8)b * 31) / 255) << 1) + 1;
        i++;
        r = (s32)(167.0f - (i - 128) * 5.21f);
        g = (s32)(130.0f - (i - 128) * 2.56f);
        b = (s32)((i - 128) * 2.65f + 3.0f);
    }

    for (i = 160; i < 192; ) {
        g_plasmaPalette[i] = ((((u32)(u8)r * 31) / 255) << 11) + ((((u32)(u8)g * 31) / 255) << 6) + ((((u32)(u8)b * 31) / 255) << 1) + 1;
        i++;
        r = 0;
        g = (s32)(48.0f - (i - 160) * 1.5f);
        b = (s32)(88.0f - (i - 160) * 2.75f);
    }

    for (i = 192; i < 224; ) {
        g_plasmaPalette[i] = ((((u32)(u8)r * 31) / 255) << 11) + ((((u32)(u8)g * 31) / 255) << 6) + ((((u32)(u8)b * 31) / 255) << 1) + 1;
        i++;
        g = (s32)((i - 192) * 1.5f + 0.0f);
        b = (s32)((i - 192) * 2.75f + 0.0f);
    }

    for (i = 224; i < 256; ) {
        g_plasmaPalette[i] = ((((u32)(u8)r * 31) / 255) << 11) + ((((u32)(u8)g * 31) / 255) << 6) + ((((u32)(u8)b * 31) / 255) << 1) + 1;
        i++;
        g = (s32)(48.0f - (i - 224) * 1.5f);
        b = (s32)(88.0f - (i - 224) * 2.75f);
    }
}

void DrawPlasma(s32 x1, s32 y1, s32 x2, s32 y2, s32 x3, s32 y3, s32 x4, s32 y4, s32 colorShift) {
    s32 y;
    s32 x;
    s32 offset;

    for (y = 0, offset = 0; y < 64; y++, offset += 64) {
        for (x = 0; x < 64; x++) {
            g_plasmaTexture[offset + x] =
                g_plasmaTable1[((y + y1) * 128) + x + x1] + colorShift +
                g_plasmaTable2[((y + y2) * 128) + x + x2] +
                g_plasmaTable2[((y + y3) * 128) + x + x3] +
                g_plasmaTable2[((y + y4) * 128) + x + x4];
        }
    }
}

void InitPlasma(void) {
    g_plasmaTexture = DAlloc(g_heap, 0x1000, "plasma.c", 356);
    g_plasmaPalette = DAlloc(g_heap, 0x200, "plasma.c", 357);
    g_plasmaTable1 = DAlloc(g_heap, 0x4000, "plasma.c", 358);
    g_plasmaTable2 = DAlloc(g_heap, 0x4000, "plasma.c", 359);

    InitPlasmaTable1();
    InitPlasmaTable2();
    CheckGoldMedalsUnlocked(frontendOptions_g.unk60, frontendOptions_g.unk64);
    if (frontendOptions_g.goldUnlocked != 0) {
        InitPlasmaPaletteGold();
    } else {
        InitPlasmaPalette();
    }
    guSprite2DInit(&g_plasmaSprite, g_plasmaTexture, g_plasmaPalette, 64, 64, 64, G_IM_FMT_CI, G_IM_SIZ_8b, 0, 0);
}

void UpdatePlasma(void) {
    s32 x1;
    s32 y1;
    s32 x2;
    s32 y2;
    s32 x3;
    s32 y3;
    s32 x4;
    s32 y4;

    g_plasmaPhase0 += 0.85f / 60.0f;
    g_plasmaPhase1 -= 1.0f / 60.0f;
    g_plasmaPhase2 += 3.0f / 60.0f;
    g_plasmaPhase3 -= 2.0f / 60.0f;
    g_plasmaPhase4 += 4.0f / 60.0f;
    g_plasmaPhase5 -= 1.5f / 60.0f;
    g_plasmaPhase6 += 3.5f / 60.0f;
    g_plasmaPhase7 -= 0.5f / 60.0f;

    x2 = (s32)(sinf(g_plasmaPhase0) * 32.0f + 32.0f);
    y2 = (s32)(cosf(g_plasmaPhase1) * 32.0f + 32.0f);
    x1 = (s32)(cosf(g_plasmaPhase2) * 32.0f + 32.0f);
    y1 = (s32)(sinf(g_plasmaPhase3) * 32.0f + 32.0f);
    x3 = (s32)(cosf(g_plasmaPhase4) * 32.0f + 32.0f);
    y3 = (s32)(sinf(g_plasmaPhase5) * 32.0f + 32.0f);
    x4 = (s32)(cosf(g_plasmaPhase6) * 32.0f + 32.0f);
    y4 = (s32)(sinf(g_plasmaPhase7) * 32.0f + 32.0f);

    g_plasmaColorShift += 5;

    DrawPlasma(x1, y1, x2, y2, x3, y3, x4, y4, g_plasmaColorShift);
}

Gfx *RenderPlasma(Gfx *glistp) {
    gDPPipeSync(glistp++);
    gDPSetRenderMode(glistp++, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF2);
    gSPSetGeometryMode(glistp++, G_ZBUFFER);
    gDPSetPrimDepth(glistp++, -1, 0);

    glistp = DrawFrontendBackdrop(10, 8, 0, 0, 320, 240, 3, glistp);

    gDPPipeSync(glistp++);
    gDPSetPrimColor(glistp++, 0, 0, 255, 255, 255, 80);
    gDPSetCombineMode(glistp++, G_CC_PLASMA, G_CC_PLASMA);
    gDPSetTextureFilter(glistp++, G_TF_BILERP);
    gDPSetPrimDepth(glistp++, -3, 0);
    gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_plasmaSprite));
    gSPSprite2DScaleFlip(glistp++, 200, 270, 0, 0);
    gSPSprite2DDraw(glistp++, 0, 0);
    gDPSetPrimDepth(glistp++, -5, 0);
    gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_plasmaSprite));
    gSPSprite2DScaleFlip(glistp++, 200, 270, 1, 1);
    gSPSprite2DDraw(glistp++, 0, 0);
    gDPSetRenderMode(glistp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gSPClearGeometryMode(glistp++, G_ZBUFFER);
    gDPSetTextureFilter(glistp++, G_TF_POINT);

    return glistp;
}

void InitInGamePlasmaPalette(void) {
    s32 i;
    s32 r;
    s32 g;
    s32 b;

    i = 0;
    r = 0;
    b = 0;
    g = 0;

    for (; i < 32; ) {
        g_inGamePlasmaPalette[i] = ((r & 0x1F) << 11) + ((g & 0x1F) << 6) + ((b & 0x1F) << 1) + 1;
        i++;
        if ((i % 3) == 2) {
            r++;
        }
        if ((i % 5) == 2) {
            g++;
        }
    }

    for (i = 32; i < 64; ) {
        g_inGamePlasmaPalette[i] = ((r & 0x1F) << 11) + ((g & 0x1F) << 6) + ((b & 0x1F) << 1) + 1;
        i++;
        if ((i % 3) == 2) {
            g++;
        }
        if ((i % 2) == 1) {
            r++;
        }
    }

    for (i = 64; i < 96; ) {
        g_inGamePlasmaPalette[i] = ((r & 0x1F) << 11) + ((g & 0x1F) << 6) + ((b & 0x1F) << 1) + 1;
        i++;
        if ((i % 5) == 2) {
            r--;
        }
        if ((i % 2) == 1) {
            b++;
        }
        if ((i % 4) == 1) {
            g++;
        }
    }

    for (i = 96; i < 128; ) {
        g_inGamePlasmaPalette[i] = ((r & 0x1F) << 11) + ((g & 0x1F) << 6) + ((b & 0x1F) << 1) + 1;
        i++;
        if ((i % 5) == 2) {
            r++;
        }
        if ((i % 2) == 1) {
            b--;
        }
        if ((i % 4) == 1) {
            g--;
        }
    }

    for (i = 128; i < 160; ) {
        g_inGamePlasmaPalette[i] = ((r & 0x1F) << 11) + ((g & 0x1F) << 6) + ((b & 0x1F) << 1) + 1;
        i++;
        if ((i % 3) == 2) {
            g--;
        }
        if ((i % 2) == 1) {
            r--;
        }
    }

    for (i = 160; i < 192; ) {
        g_inGamePlasmaPalette[i] = ((r & 0x1F) << 11) + ((g & 0x1F) << 6) + ((b & 0x1F) << 1) + 1;
        i++;
        if ((i % 3) == 2) {
            r--;
        }
        if ((i % 5) == 2) {
            g--;
        }
    }

    for (i = 192; i < 224; ) {
        g_inGamePlasmaPalette[i] = ((r & 0x1F) << 11) + ((g & 0x1F) << 6) + ((b & 0x1F) << 1) + 1;
        i++;
        if ((i % 10) == 2) {
            g++;
        }
        if ((i % 5) == 1) {
            r++;
        }
    }

    for (i = 224; i < 256; ) {
        g_inGamePlasmaPalette[i] = ((r & 0x1F) << 11) + ((g & 0x1F) << 6) + ((b & 0x1F) << 1) + 1;
        i++;
        if ((i % 10) == 2) {
            g--;
        }
        if ((i % 5) == 1) {
            r--;
        }
    }
}

void UpdateInGamePlasma(s32 mode) {
    u8 xWave[64];
    u8 yWave[32];
    s16 saved1;
    s16 saved2;
    s16 saved3;
    s16 delta;
    s32 i;
    s32 j;
    s16 v1;
    s16 v2;
    s16 v3;
    s16 v4;
    s16 v5;
    u8 yVal;
    u8 *dst;

    saved1 = g_inGamePlasmaPhase1;
    saved2 = g_inGamePlasmaPhase2;
    saved3 = g_inGamePlasmaPhase3;

    for (i = 0; i < 64; i++) {
        v1 = rsin(g_inGamePlasmaPhase1) >> 4;
        v2 = rsin(g_inGamePlasmaPhase2) >> 5;
        v3 = rsin(g_inGamePlasmaPhase3) >> 6;
        xWave[i] = v1 + v2 + v3;
        if (mode == 0) {
            g_inGamePlasmaPhase1 += 0x14;
            g_inGamePlasmaPhase2 += 0x40;
            g_inGamePlasmaPhase3 += 0x60;
        } else {
            g_inGamePlasmaPhase1 += 0x14;
            g_inGamePlasmaPhase2 += 0x20;
            g_inGamePlasmaPhase3 += 0;
        }
    }

    if (mode == 0) {
        g_inGamePlasmaPhase1 = saved1 - 0x60;
        g_inGamePlasmaPhase2 = saved2 + 0x20;
        g_inGamePlasmaPhase3 = saved3 + 0x18;
    } else {
        delta = rsin(g_inGamePlasmaAngle) >> 6;
        g_inGamePlasmaPhase1 = saved1 + delta;
        g_inGamePlasmaPhase2 = saved2;
        g_inGamePlasmaPhase3 = saved3;
        g_inGamePlasmaAngle += 0x20;
    }

    saved1 = g_inGamePlasmaPhase4;
    saved2 = g_inGamePlasmaPhase5;

    for (i = 0; i < 32; i++) {
        v4 = rsin(g_inGamePlasmaPhase4) >> 4;
        v5 = rsin(g_inGamePlasmaPhase5) >> 5;
        yWave[i] = v4 + v5;
        if (mode == 0) {
            g_inGamePlasmaPhase4 += 0x1C;
            g_inGamePlasmaPhase5 += 0x2E;
        } else {
            g_inGamePlasmaPhase4 += 0x1C;
            g_inGamePlasmaPhase5 += 0x22;
        }
    }

    if (mode == 0) {
        g_inGamePlasmaPhase4 = saved1 - 0x34;
        g_inGamePlasmaPhase5 = saved2 + 0x4E;
    } else {
        g_inGamePlasmaPhase4 = saved1 + (delta >> 1);
        g_inGamePlasmaPhase5 = saved2;
    }

    dst = g_inGamePlasmaTexture;
    for (j = 0; j < 32; j++) {
        yVal = yWave[j] - 0x80;
        for (i = 0; i < 64; i++) {
            *dst++ = yVal + xWave[i];
        }
    }
}
