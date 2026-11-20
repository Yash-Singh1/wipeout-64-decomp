/*
 * font.c — 6x6 S2DEX sprite font renderer, button prompts, and name tables.
 *
 * Renders small menu/HUD text, controller button icons, and right/center
 * aligned strings, and maps race class and track IDs to display names.
 */
#include "font.h"

char *GetClassName(s16 classIdx) {
    switch (classIdx) {
        case 0:
            return "VECTOR";
        case 1:
            return "VENOM";
        case 2:
            return "RAPIER";
        case 3:
            return "PHANTOM";
        default:
            return "SCREW UP";
    }
}

char *GetTrackName(s16 trackIdx) {
    switch (trackIdx) {
        case 1:
            return "KLIES BRIDGE";
        case 2:
            return "QORON IV";
        case 3:
            return "SOKANA";
        case 4:
            return "DYRONESS";
        case 5:
            return "MACHAON II";
        case 6:
            return "TERAFUMOS";
        case 7:
            return "VELOCITAR";
        default:
            return "SCREW UP";
    }
}

Gfx *DrawButtonPrompt(s16 x, s16 y, s16 type, char *text, Gfx *glistp) {
    if (type == 1) {
        type = 12;
    }

    gDPPipeSync(glistp++);
    gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);

    switch (type) {
        case 12:
            glistp = AddText(text, x + 22, y + 3, 8, glistp);
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_buttonASprite));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, x << 2, y << 2);
            break;

        case 13:
            glistp = AddText(text, x - 30, y + 3, 8, glistp);
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_buttonBSprite));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, x << 2, y << 2);
            break;

        case 16:
            glistp = AddText(text, x + 22, y + 4, 8, glistp);
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_menuSprites[20]));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, x << 2, y << 2);
            break;

        case 17:
            if (g_menuScreen == 22) {
                glistp = AddText(text, x - 5, y + 4, 8, glistp);
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_menuSprites[21]));
                gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
                gSPSprite2DDraw(glistp++, (x << 2) + 245, y << 2);
            } else {
                glistp = AddText(text, x - 30, y + 4, 8, glistp);
                gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_menuSprites[21]));
                gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
                gSPSprite2DDraw(glistp++, x << 2, y << 2);
            }
            break;

        case 14:
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_button14Sprite));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, x << 2, y << 2);
            break;

        case 15:
            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_button15Sprite));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, x << 2, y << 2);
            break;
    }

    return glistp;
}

Gfx *AddNumber(s32 num, s32 x, s32 y, s32 size, s32 colour, Gfx *glistp) {
    char str[16];
    s32 i;
    s32 j;
    s32 origNum;

    origNum = num;

    if (colour != 1) {
        gDPPipeSync(glistp++);
        gDPSetPrimColor(glistp++, 0, 0, 255, 255, 0, 0);
        gDPSetCombineMode(glistp++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);
    } else {
        gDPPipeSync(glistp++);
        gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    }

    if (num < 0) {
        x -= 6;
        num = -num;
    }

    for (i = 0; i < 5; i++) {
        num /= 10;
        if (num == 0) {
            break;
        }
    }

    for (j = i; j >= 0; j--) {
        x -= 6;
    }

    sprintf(str, "%i", origNum);
    return AddText(str, x, y, size, glistp);
}

Gfx *AddText(char *text, s32 x, s32 y, s32 size, Gfx *glistp) {
    u8 c;
    s32 glyph;

    size = FONT_CHAR_SIZE;

    gDPPipeSync(glistp++);
    gDPSetCombineMode(glistp++, G_CC_DECALRGBA, G_CC_DECALRGBA);

    while (1) {
        c = *text++;
        if (c != ' ') {
            if (c == '\0') {
                g_textEndX = x;
                return glistp;
            }

            glyph = c - 'A';
            if (c < 'A' || c > 'Z') {
                if (c >= '0' && c <= '9') {
                    glyph = c - '0' + 26;
                } else if (c == ':') {
                    glyph = 36;
                } else if (c == '.') {
                    glyph = 37;
                } else if (c == ',') {
                    glyph = 38;
                } else if (c == '[') {
                    glyph = 39;
                } else if (c == ']') {
                    glyph = 40;
                } else if (c == '/') {
                    glyph = 41;
                } else if (c == '\'') {
                    glyph = 42;
                } else if (c == '?') {
                    glyph = 43;
                } else if (c == '-') {
                    glyph = 44;
                } else if (c == '+') {
                    glyph = 45;
                } else if (c == '%') {
                    glyph = 46;
                } else if (c == '!') {
                    glyph = 47;
                } else if (c == '#') {
                    glyph = 48;
                } else if (c == '\\') {
                    glyph = 49;
                } else if (c == '`') {
                    glyph = 50;
                } else if (c == ' ') {
                    glyph = 51;
                } else if (c == '"') {
                    glyph = 52;
                } else if (c == '*') {
                    glyph = 53;
                } else if (c == '=') {
                    glyph = 54;
                } else if (c == '@') {
                    glyph = 55;
                }
            }

            if (glyph == 8) {
                x -= 2;
            }

            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_fontSprites[glyph]));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, x << 2, y << 2);
            g_spriteCount++;

            if (glyph == 8) {
                x += size - 2;
            } else {
                x += size;
            }
        } else {
            x += 3;
        }
    }
}

Gfx *AddTextYellow(char *text, s32 x, s32 y, s32 size, Gfx *glistp) {
    u8 c;
    s32 glyph;

    size = FONT_CHAR_SIZE;

    gDPPipeSync(glistp++);
    gDPSetPrimColor(glistp++, 0, 0, 255, 255, 0, 0);
    gDPSetCombineMode(glistp++, G_CC_MODULATEIDECALA_PRIM, G_CC_MODULATEIDECALA_PRIM);

    while (1) {
        c = *text++;
        if (c != ' ') {
            if (c == '\0') {
                g_textEndX = x;
                return glistp;
            }

            glyph = c - 'A';
            if (c < 'A' || c > 'Z') {
                if (c >= '0' && c <= '9') {
                    glyph = c - '0' + 26;
                } else if (c == ':') {
                    glyph = 36;
                } else if (c == '.') {
                    glyph = 37;
                } else if (c == ',') {
                    glyph = 38;
                } else if (c == '[') {
                    glyph = 39;
                } else if (c == ']') {
                    glyph = 40;
                } else if (c == '/') {
                    glyph = 41;
                } else if (c == '\'') {
                    glyph = 42;
                } else if (c == '?') {
                    glyph = 43;
                } else if (c == '-') {
                    glyph = 44;
                } else if (c == '+') {
                    glyph = 45;
                } else if (c == '%') {
                    glyph = 46;
                } else if (c == '!') {
                    glyph = 47;
                } else if (c == '#') {
                    glyph = 48;
                } else if (c == '\\') {
                    glyph = 49;
                } else if (c == '`') {
                    glyph = 50;
                } else if (c == ' ') {
                    glyph = 51;
                } else if (c == '"') {
                    glyph = 52;
                } else if (c == '*') {
                    glyph = 53;
                } else if (c == '=') {
                    glyph = 54;
                } else if (c == '@') {
                    glyph = 55;
                }
            }

            if (glyph == 8) {
                x -= 2;
            }

            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_fontSprites[glyph]));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, x << 2, y << 2);
            g_spriteCount++;

            if (glyph == 8) {
                x += size - 2;
            } else {
                x += size;
            }
        } else {
            x += 3;
        }
    }
}

Gfx *AddCentreText(char *text, s32 x, s32 y, s32 size, Gfx *glistp) {
    u8 c;
    s32 glyph;
    char *p;
    s16 len;

    size = FONT_CHAR_SIZE;
    len = 0;
    p = text;

    while ((c = *p) != '\0') {
        if (c == ' ') {
            len += 3;
        } else if (c != 'I') {
            len += size;
        } else {
            len += 3;
        }
        p++;
    }

    x -= len >> 1;

    while (1) {
        c = *text++;
        if (c != ' ') {
            if (c == '\0') {
                g_textEndX = x;
                return glistp;
            }

            glyph = c - 'A';
            if (c < 'A' || c > 'Z') {
                if (c >= '0' && c <= '9') {
                    glyph = c - '0' + 26;
                } else if (c == ':') {
                    glyph = 36;
                } else if (c == '.') {
                    glyph = 37;
                } else if (c == ',') {
                    glyph = 38;
                } else if (c == '[') {
                    glyph = 39;
                } else if (c == ']') {
                    glyph = 40;
                } else if (c == '/') {
                    glyph = 41;
                } else if (c == '\'') {
                    glyph = 42;
                } else if (c == '?') {
                    glyph = 43;
                } else if (c == '-') {
                    glyph = 44;
                } else if (c == '+') {
                    glyph = 45;
                } else if (c == '%') {
                    glyph = 46;
                } else if (c == '!') {
                    glyph = 47;
                } else if (c == '#') {
                    glyph = 48;
                } else if (c == '\\') {
                    glyph = 49;
                } else if (c == '`') {
                    glyph = 50;
                } else if (c == ' ') {
                    glyph = 51;
                } else if (c == '"') {
                    glyph = 52;
                } else if (c == '*') {
                    glyph = 53;
                } else if (c == '=') {
                    glyph = 54;
                } else if (c == '@') {
                    glyph = 55;
                }
            }

            if (glyph == 8) {
                x -= 2;
            }

            gSPSprite2DBase(glistp++, OS_K0_TO_PHYSICAL(&g_fontSprites[glyph]));
            gSPSprite2DScaleFlip(glistp++, 1 << 10, 1 << 10, 0, 0);
            gSPSprite2DDraw(glistp++, x << 2, y << 2);
            g_spriteCount++;

            if (glyph == 8) {
                x += size - 2;
            } else {
                x += size;
            }
        } else {
            x += 3;
        }
    }
}

Gfx *AddRightText(char *text, s32 x, s32 y, s32 size, Gfx *glistp) {
    u8 c;
    char *p;
    s16 len;

    size = FONT_CHAR_SIZE;
    len = 0;
    p = text;

    while ((c = *p) != '\0') {
        if (c == ' ') {
            len += 3;
        } else if (c != 'I') {
            len += size;
        } else {
            len += 3;
        }
        p++;
    }

    x -= len;
    return AddText(text, x, y, size, glistp);
}

void InitFontUnused(void) {
}

void LoadFontStartupTextures(void) {
    void *texTable;

    g_fontStartTex = TextureTableCount;
    texTable = WADLoadCompressedTextureSequence(g_startupImage);
    RegisterTextureSequence(texTable);
}

void InitFont(void) {
    WADFILE *wad;
    s16 texIdx;
    s32 i;
    s32 charSize;
    s32 bitSize;
    N64Texture *tex;

    charSize = FONT_CHAR_SIZE;
    LoadFontStartupTextures();
    g_startupImage->current++;
    wad = OpenWadFileNvc((u32)_startupDataFile);
    wad->current = 22;
    texIdx = TextureTableCount;
    RegisterTextureSequence(WADLoadCompressedTextureSequence(wad));
    g_fontTexture = g_textureTable[texIdx];

    for (i = 0; i < FONT_NUM_GLYPHS; i++) {
        guSprite2DInit(&g_fontSprites[i],
                       (void *)OS_K0_TO_PHYSICAL(g_fontTexture->texels),
                       (void *)OS_K0_TO_PHYSICAL(g_fontTexture->palette),
                       g_fontTexture->width,
                       FONT_CHAR_SIZE, FONT_CHAR_SIZE,
                       G_IM_FMT_CI, G_IM_SIZ_4b,
                       (i % FONT_GLYPHS_PER_ROW) * charSize,
                       (i / FONT_GLYPHS_PER_ROW) * charSize);
    }

    CloseWadFileNvc(wad);

    g_hudTexStart = TextureTableCount;
    RegisterTextureSequence(LoadCompressedTextureSequence((u32)_hudTexturesFile));

    tex = g_textureTable[g_hudTexStart + 19];
    g_arrowLeftTex = tex;
    bitSize = (tex->type == 0) ? G_IM_SIZ_4b : G_IM_SIZ_8b;
    guSprite2DInit(&g_arrowLeftSprite,
                   (void *)OS_K0_TO_PHYSICAL(tex->texels),
                   (void *)OS_K0_TO_PHYSICAL(tex->palette),
                   tex->width, tex->width, tex->height,
                   G_IM_FMT_CI, bitSize,
                   0, 0);

    tex = g_textureTable[g_hudTexStart + 20];
    g_arrowRightTex = tex;
    bitSize = (tex->type == 0) ? G_IM_SIZ_4b : G_IM_SIZ_8b;
    guSprite2DInit(&g_arrowRightSprite,
                   (void *)OS_K0_TO_PHYSICAL(tex->texels),
                   (void *)OS_K0_TO_PHYSICAL(tex->palette),
                   tex->width, tex->width, tex->height,
                   G_IM_FMT_CI, bitSize,
                   0, 0);

    tex = g_textureTable[g_hudTexStart + 21];
    g_buttonATex = tex;
    bitSize = (tex->type == 0) ? G_IM_SIZ_4b : G_IM_SIZ_8b;
    guSprite2DInit(&g_buttonASprite,
                   (void *)OS_K0_TO_PHYSICAL(tex->texels),
                   (void *)OS_K0_TO_PHYSICAL(tex->palette),
                   tex->width, tex->width, tex->height,
                   G_IM_FMT_CI, bitSize,
                   0, 0);

    tex = g_textureTable[g_hudTexStart + 22];
    g_buttonBTex = tex;
    bitSize = (tex->type == 0) ? G_IM_SIZ_4b : G_IM_SIZ_8b;
    guSprite2DInit(&g_buttonBSprite,
                   (void *)OS_K0_TO_PHYSICAL(tex->texels),
                   (void *)OS_K0_TO_PHYSICAL(tex->palette),
                   tex->width, tex->width, tex->height,
                   G_IM_FMT_CI, bitSize,
                   0, 0);

    tex = g_textureTable[g_hudTexStart + 23];
    g_button15Tex = tex;
    bitSize = (tex->type == 0) ? G_IM_SIZ_4b : G_IM_SIZ_8b;
    guSprite2DInit(&g_button15Sprite,
                   (void *)OS_K0_TO_PHYSICAL(tex->texels),
                   (void *)OS_K0_TO_PHYSICAL(tex->palette),
                   tex->width, tex->width, tex->height,
                   G_IM_FMT_CI, bitSize,
                   0, 0);

    tex = g_textureTable[g_hudTexStart + 24];
    g_button14Tex = tex;
    bitSize = (tex->type == 0) ? G_IM_SIZ_4b : G_IM_SIZ_8b;
    guSprite2DInit(&g_button14Sprite,
                   (void *)OS_K0_TO_PHYSICAL(tex->texels),
                   (void *)OS_K0_TO_PHYSICAL(tex->palette),
                   tex->width, tex->width, tex->height,
                   G_IM_FMT_CI, bitSize,
                   0, 0);
}
