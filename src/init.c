/*
 * Init.c — Scene WAD loading, texture decompression, and F3DEX texture DLs.
 *
 * Loads per-track scene PRM models, decompresses TIM texture sequences,
 * converts 15-bit PS1 palettes to N64 RGBA5551, flips textures, and generates
 * per-texture F3DEX load display lists.
 */
#include "init.h"

s16 WADInitScene(Object **prm, s16 *prmCount, Skeleton *camPos, ConfigData *gameData, WADFILE *wad) {
    s16 sceneTextures;
    s16 scene2Textures;
    s16 skyTextures;
    s16 i;
    s16 j;
    Object *obj;
    s16 sky;
    TIMlist *timPtr;
    TIMlist *skyTimPtr;

    switch (frontendOptions_g.trackNo) {
        case 1:
            break;
        case 2:
            InitTrack2Anim();
            break;
        case 3:
            InitTrack3Anim();
            break;
        case 4:
            InitTrack4Anim();
            break;
        case 5:
            InitTrack5Anim();
            break;
        case 6:
            InitTrack6Anim();
            break;
        case 7:
            break;
    }

    sceneTextures = TextureTableCount;
    if (g_multiplayerFlag) {
        wad->current++;
        timPtr = WADLoadCompressedTextureSequence(wad);
    } else {
        timPtr = WADLoadCompressedTextureSequence(wad);
        wad->current++;
    }
    LoadCmpFiles(timPtr);
    InitRenderList(&g_sceneRenderList, sceneTextures, TextureTableCount - sceneTextures);

    scene2Textures = TextureTableCount;
    timPtr = WADLoadCompressedTextureSequence(wad);
    LoadCmpFiles(timPtr);
    InitRenderList(&g_scene2RenderList, scene2Textures, TextureTableCount - scene2Textures);

    skyTextures = TextureTableCount;
    skyTimPtr = WADLoadCompressedTextureSequence(wad);
    LoadCmpFiles(skyTimPtr);
    InitRenderList(&g_skyRenderList, skyTextures, 1);

    for (i = 0; i < 128; i++) {
        for (j = 0; j < 40; j++) {
            sharedPrimTable[i][j] = NULL;
        }
    }

    g_scenePrmIndex = *prmCount;
    if (g_multiplayerFlag) {
        wad->current++;
        prm[(*prmCount)++] = WADLoadPrm(wad, &g_sceneRenderList);
    } else {
        prm[(*prmCount)++] = WADLoadPrm(wad, &g_sceneRenderList);
        wad->current++;
    }

    g_scene2PrmIndex = *prmCount;
    prm[(*prmCount)++] = WADLoadPrm(wad, &g_scene2RenderList);

    for (i = 0; i < *prmCount; i++) {
        obj = prm[i];
        while (obj) {
            obj->skeleton->super = camPos;
            obj = obj->next;
        }
    }

    sky = *prmCount;
    prm[(*prmCount)++] = WADLoadPrm(wad, &g_skyRenderList);
    obj = prm[sky];
    obj->skeleton->super = camPos->super;
    {
        Skeleton *skel = prm[sky]->skeleton;
        skel->relative.m[2][2] = 0x800;
        skel->relative.m[1][1] = 0x800;
        skel->relative.m[0][0] = 0x800;
    }
    return sky;
}

void LoadCmpFiles(TIMlist *timPtr) {
    N64Texture *texture;
    s16 i;

    for (i = 0; i < numTex; i++) {
        texture = NewLoadTexture((Tim *)timPtr->memOffset[i], 1, 0);
        if (texture) {
            texture->index = i;
            g_textureTable[TextureTableCount++] = texture;
        } else {
            g_textureTable[TextureTableCount++] = NULL;
        }
    }
}

void FlipTexture(N64Texture *texture) {
    u8 rowBuf[256];
    s32 y;
    s32 x;
    u8 *src;
    u8 *dst;

    for (y = 0; y < texture->height / 2; y++) {
        src = (u8 *)texture->texels + (texture->height - 1 - y) * (texture->width / 2);
        dst = rowBuf;
        for (x = 0; x < texture->width / 2; x++) {
            *dst++ = *src++;
        }

        dst = (u8 *)texture->texels + (texture->height - 1 - y) * (texture->width / 2);
        src = (u8 *)texture->texels + y * (texture->width / 2);
        for (x = 0; x < texture->width / 2; x++) {
            *dst++ = *src++;
        }

        src = rowBuf;
        dst = (u8 *)texture->texels + y * (texture->width / 2);
        for (x = 0; x < texture->width / 2; x++) {
            *dst++ = *src++;
        }
    }
}

N64Texture *NewLoadTexture(Tim *timPtr, s16 translucent, s32 scene) {
    Tim *tim;
    TimClut4 *tc4;
    TimClut8 *tc8;
    N64Texture *texture;
    struct {
        s16 x, y, w, h;
    } rect; /* unused, left over from PSX/XL; preserves 0x38 frame */
    s32 i;
    s32 n;
    u8 *p;
    u8 *src;
    u8 *dst;
    u16 *srcPal;
    u16 *dstPal;
    s32 x;
    s32 y;
    s32 newW;
    s32 newH;
    s32 step;
    s32 padBytes;

    tim = timPtr;
    SwapEndian32(&tim->flags);

    if (ClutType(tim) == Clut4Bit) {
        s16 w;
        s16 h;
        s32 width;

        tc4 = (TimClut4 *)tim;
        texture = DAlloc(g_heap, sizeof(N64Texture), "Init.c", 466);
        if (!texture) {
            Error("texture alloc", Fatal);
        }
        texture->type = CLUT4;
        texture->uScale = 0x8000;
        texture->vScale = 0x8000;
        texture->flags = tim->flags;
        texture->unk0A = -1;
        texture->clutSize = 16;
        texture->bpp = 1;

        SwapEndian16(&tc4->textureW);
        SwapEndian16(&tc4->textureH);

        texture->width = tc4->textureW * 4;
        texture->height = tc4->textureH;
        texture->texels = tc4 + 1;
        texture->palette = &tc4->clut;
        ConvertPalette(tc4->clut, 16);

        n = (texture->width * texture->height) / 2;
        p = (u8 *)texture->texels;
        for (i = 0; i < n; i++) {
            SwapNibbles(p++);
        }

        if (((u32)texture->texels & ~7) != (u32)texture->texels) {
            DFree(g_heap, texture);
            return NULL;
        }

        if (((u32)texture->palette & ~7) != (u32)texture->palette) {
            texture->palette = DAlloc(g_heap, 32, "Init.c", 506);
            if (!texture->palette) {
                Error("Palette alloc", Fatal);
            }
            srcPal = tc4->clut;
            dstPal = (u16 *)texture->palette;
            for (i = 0; i < 16; i++) {
                *dstPal++ = *srcPal++;
            }
        }

        width = texture->width;
        if (width != (width & ~15)) {
            src = (u8 *)texture->texels;
            dst = DAlloc(g_heap, (u32)(((width + 15) & ~15) * texture->height) / 2, "Init.c", 524);
            if (!dst) {
                Error("newTex", Fatal);
            }
            texture->texels = dst;
            for (y = 0; y < texture->height; y++) {
                for (x = 0; x < texture->width / 2; x++) {
                    *dst++ = *src++;
                }
                padBytes = (u32)(((texture->width + 15) & ~15) - (x * 2)) / 2;
                for (x = 0; x < padBytes; x++) {
                    *dst++ = 0;
                }
            }
            texture->width = (texture->width + 15) & ~15;
        }

        w = texture->width;
        h = texture->height;
        if (w * h > 4096) {
            newW = w;
            newH = h >> 1;
            step = 1;
            texture->vScale = 0x4000;
            if (newW * newH > 4096) {
                newW = w >> 1;
                step = 2;
                texture->uScale = 0x4000;
                if (newW * newH > 4096) {
                    DFree(g_heap, texture);
                    return NULL;
                }
            }

            src = (u8 *)texture->texels;
            dst = src;
            if (!dst) {
                Error("newTex", Fatal);
            }
            texture->texels = dst;
            for (y = 0; y < newH; y++) {
                for (x = 0; x < newW / 2; x++) {
                    *dst++ = *src;
                    src += step;
                }
                src += texture->width / 2;
            }
            texture->width = newW;
            texture->height = newH;
        }

        if (tim->flags & 0x20) {
            FlipTexture(texture);
        }
        BuildTextureDisplayList(texture);
    } else if (ClutType(tim) == Clut8Bit) {
        s16 w;
        s16 h;

        tc8 = (TimClut8 *)tim;
        texture = DAlloc(g_heap, sizeof(N64Texture), "Init.c", 640);
        if (!texture) {
            Error("Out of mem: texture", Fatal);
        }
        texture->type = CLUT8;
        texture->uScale = 0x8000;
        texture->vScale = 0x8000;
        texture->flags = tim->flags;
        texture->clutSize = 256;
        texture->bpp = 1;
        texture->unk0A = -1;

        SwapEndian16(&tc8->textureW);
        SwapEndian16(&tc8->textureH);

        texture->width = tc8->textureW * 2;
        texture->height = tc8->textureH;
        texture->texels = tc8 + 1;
        texture->palette = &tc8->clut;
        ConvertPalette(tc8->clut, 256);

        if (((u32)texture->texels & ~7) != (u32)texture->texels) {
            DFree(g_heap, texture);
            return NULL;
        }

        if (((u32)texture->palette & ~7) != (u32)texture->palette) {
            texture->palette = DAlloc(g_heap, 512, "Init.c", 684);
            if (!texture->palette) {
                Error("Out of mem: pal", Fatal);
            }
            srcPal = tc8->clut;
            dstPal = (u16 *)texture->palette;
            for (i = 0; i < 256; i++) {
                *dstPal++ = *srcPal++;
            }
        }

        w = texture->width;
        h = texture->height;
        if (w * h > 2048) {
            newW = w;
            newH = h >> 2;
            step = 1;
            texture->uScale = 0x4000;
            if (newW * newH > 2048) {
                newW = w >> 1;
                step = 2;
                texture->vScale = 0x4000;
                if (newW * newH > 2048) {
                    DFree(g_heap, texture);
                    return NULL;
                }
            }

            src = (u8 *)texture->texels;
            dst = src;
            if (!dst) {
                Error("Out of mem: newtex", Fatal);
            }
            texture->texels = dst;
            for (y = 0; y < newH; y++) {
                for (x = 0; x < newW; x++) {
                    *dst++ = *src;
                    src += step;
                }
                src += texture->width;
            }
            texture->width = newW;
            texture->height = newH;
        }

        if (tim->flags & 0x20) {
            FlipTexture(texture);
        }
        BuildTextureDisplayList(texture);
    } else {
        texture = NULL;
    }

    return texture;
}

void ConvertPalette(u16 *pal, s32 count) {
    u8 *p;
    u8 tmp;
    s32 i;
    u16 color;
    u16 r;
    u16 g;
    u16 b;
    u16 a;

    for (i = 0; i < count; i++) {
        p = (u8 *)pal;
        tmp = p[0];
        p[0] = p[1];
        p[1] = tmp;

        color = *pal;
        a = 1;
        r = color & 0x1F;
        g = (color & 0x3E0) >> 5;
        b = (color & 0x7C00) >> 10;
        if (r == 0 && g == 0 && b == 0) {
            a = 0;
        }
        *pal++ = a | ((r << 11) | (g << 6) | (b << 1));
    }
}

TIMlist *LoadCompressedTextureSequence(u32 fileOffset) {
    s16 texNum;
    char *fileBuffer;
    s32 fileSize;
    s32 timSize;
    s32 timTotalSize;

    fileSize = FileLength(fileOffset);
    if (fileSize <= 0) {
        sprintf(errorString, "Tim decompress: file %s not found\n", *(const char **)fileOffset);
        Error(errorString, Fatal);
    }
    fileBuffer = DAlloc(g_heap, fileSize + 32, "Init.c", 837);
    if (!fileBuffer) {
        Error("Decompress Load: Failed to allocate memory for track vertices", Fatal);
    }
    if (LoadFile(fileOffset, fileBuffer) != fileSize) {
        sprintf(errorString, "Decompress(): Failed to load file %s\n", *(const char **)fileOffset);
        Error(errorString, Fatal);
    }
    SwapEndian32(fileBuffer);
    numTex = ((CompressedTextureSequence *)fileBuffer)->textureCount;
    timTotalSize = 0;
    for (texNum = 0; texNum < numTex; texNum++) {
        timList.memOffset[texNum] = timTotalSize;
        SwapEndian32(&((CompressedTextureSequence *)fileBuffer)->textureSize[texNum]);
        timSize = ((CompressedTextureSequence *)fileBuffer)->textureSize[texNum];
        timTotalSize += timSize;
    }
    timList.memBase = DAlloc(g_heap, timTotalSize, "Init.c", 866);
    g_texMemTotal += timTotalSize;
    if (!timList.memBase) {
        Error("Out of mem: memBase", Fatal);
    }
    for (texNum = 0; texNum < numTex; texNum++) {
        timList.memOffset[texNum] += (s32)timList.memBase;
    }
    ExpandData((u8 *)&((CompressedTextureSequence *)fileBuffer)->textureSize[numTex], (u8 *)timList.memBase);
    DFree(g_heap, fileBuffer);
    return &timList;
}

TIMlist *WADLoadCompressedTextureSequence(WADFILE *wad) {
    s16 texNum;
    char *fileBuffer;
    s32 fileSize;
    s32 timSize;
    s32 timTotalSize;

    fileSize = wad->sizes[wad->current];
    fileBuffer = DAlloc(g_heap, fileSize + 32, "Init.c", 913);
    if (!fileBuffer) {
        Error("Decompress Load: Failed to allocate memory for track vertices", Fatal);
    }
    ReadNextWadFile(wad, fileBuffer);
    SwapEndian32(fileBuffer);
    numTex = ((CompressedTextureSequence *)fileBuffer)->textureCount;
    timTotalSize = 0;
    for (texNum = 0; texNum < numTex; texNum++) {
        wadTimList.memOffset[texNum] = timTotalSize;
        SwapEndian32(&((CompressedTextureSequence *)fileBuffer)->textureSize[texNum]);
        timSize = ((CompressedTextureSequence *)fileBuffer)->textureSize[texNum];
        timTotalSize += timSize;
    }
    wadTimList.memBase = DAlloc(g_heap, timTotalSize + 32, "Init.c", 940);
    g_texMemTotal += timTotalSize + 32;
    if (!wadTimList.memBase) {
        Error("Out of mem: memBase", Fatal);
    }
    for (texNum = 0; texNum < numTex; texNum++) {
        wadTimList.memOffset[texNum] += (s32)wadTimList.memBase;
    }
    ExpandData((u8 *)&((CompressedTextureSequence *)fileBuffer)->textureSize[numTex], (u8 *)wadTimList.memBase);
    DFree(g_heap, fileBuffer);
    return &wadTimList;
}

void GetTextureMasks(N64Texture *texture, s32 *maskS, s32 *maskT) {
    s32 bitCount;
    s32 mask;
    s32 size;

    bitCount = 0;
    mask = 0;
    size = texture->width >> 1;
    while (size > 0) {
        mask++;
        if (size & 1) {
            bitCount++;
        }
        size >>= 1;
    }
    if (bitCount >= 2) {
        mask = 0;
    }
    *maskS = mask;

    bitCount = 0;
    mask = 0;
    size = texture->height >> 1;
    while (size > 0) {
        mask++;
        if (size & 1) {
            bitCount++;
        }
        size >>= 1;
    }
    if (bitCount >= 2) {
        mask = 0;
    }
    *maskT = mask;
}

void BuildTextureDisplayList(N64Texture *texture) {
    Gfx *dl;
    s32 maskS;
    s32 maskT;
    s32 cms;
    s32 cmt;
    s32 flags;

    maskS = 0;
    maskT = 0;
    if (texture->flags & 0x70) {
        GetTextureMasks(texture, &maskS, &maskT);
    }

    flags = texture->flags;
    if (flags & 0x10) {
        cms = (maskS != 0) ? G_TX_MIRROR : G_TX_WRAP;
    } else {
        cms = G_TX_WRAP;
    }
    if (flags & 0x20) {
        cmt = (maskT != 0) ? G_TX_MIRROR : G_TX_WRAP;
    } else {
        cmt = G_TX_WRAP;
    }

    dl = texture->dl;
    if (texture->type == CLUT4) {
        gDPLoadTLUT_pal16(dl++, 0, texture->palette);
        gDPLoadTextureBlock_4b(dl++, texture->texels, G_IM_FMT_CI, texture->width, texture->height,
                               0, cms, cmt, maskS, maskT, G_TX_NOLOD, G_TX_NOLOD);
        gDPSetTextureLUT(dl++, G_TT_RGBA16);
        gSPTexture(dl++, texture->uScale, texture->vScale, 0, G_TX_RENDERTILE, G_ON);
        gDPLoadSync(dl++);
        gSPEndDisplayList(dl++);
    } else {
        gDPLoadTLUT_pal256(dl++, texture->palette);
        gDPLoadTextureBlock(dl++, texture->texels, G_IM_FMT_CI, G_IM_SIZ_8b, texture->width, texture->height,
                            0, cms, cmt, maskS, maskT, G_TX_NOLOD, G_TX_NOLOD);
        gSPTexture(dl++, texture->uScale, texture->vScale, 0, G_TX_RENDERTILE, G_ON);
        gDPSetTextureLUT(dl++, G_TT_RGBA16);
        gDPLoadSync(dl++);
        gSPEndDisplayList(dl++);
    }
}
