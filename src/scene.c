/*
 * Scene.c — PRM 3D model loader, instancing, and F3DEX display-list compiler.
 *
 * Parses Wipeout PRM object files (byte-swapping big-endian fields, scaling
 * normals, registering animated track props) and compiles PS1-style polygon
 * primitives into batched N64 F3DEX display lists and environment-mapped meshes.
 */
#include "scene.h"

#ident "$Revision: 1.17 $"

void SwapEndian32(void *ptr) {
    u8 *cvalue = (u8 *)ptr;
    u8 swapByte;

    swapByte = cvalue[0];
    cvalue[0] = cvalue[3];
    cvalue[3] = swapByte;
    swapByte = cvalue[1];
    cvalue[1] = cvalue[2];
    cvalue[2] = swapByte;
}

void SwapEndian16(void *ptr) {
    u8 *cvalue = (u8 *)ptr;
    u8 swapByte;

    swapByte = cvalue[0];
    cvalue[0] = cvalue[1];
    cvalue[1] = swapByte;
}

void SwapNibbles(u8 *bytePtr) {
    u8 val = *bytePtr;

    *bytePtr = ((val & 0xF) << 4) | (val >> 4);
}

void ScaleNormals(Object *object) {
    s32 i;
    s32 zeroFlag;
    f32 x;
    f32 y;
    f32 z;
    f32 len;
    f32 scale;

    zeroFlag = 0;
    for (i = 0; i < object->normalCount; i++) {
        x = ((SVECTOR *)object->normals)[i].vx;
        y = ((SVECTOR *)object->normals)[i].vy;
        z = ((SVECTOR *)object->normals)[i].vz;
        len = sqrtf(x * x + y * y + z * z);
        if (len != 0.0) {
            scale = 127.0 / (f64)len;
        } else {
            scale = 1.0f;
            if (!zeroFlag) {
                zeroFlag = 1;
            }
        }
        x *= scale;
        y *= scale;
        z *= scale;
        ((SVECTOR *)object->normals)[i].vx = (s16)x;
        ((SVECTOR *)object->normals)[i].vy = (s16)y;
        ((SVECTOR *)object->normals)[i].vz = (s16)z;
    }
}

Object *LoadPrmCommon(WADFILE *wad, u32 fileOffset, RenderList *renderList) {
    Object *object;
    Object *lastObject;
    Object *firstObject;
    Prm prm;
    char *start;
    char *finish;
    s32 length;
    s16 i;
    s32 untexturedCount;
    s32 texCount;
    RenderGroup *group;
    RenderGroupEntry *entry;
    s32 j;

    if (wad == NULL) {
        length = FileLength(fileOffset);
        if (length <= 0) {
            sprintf(errorString, "scene.c:LoadPrm: File %s not found\n", *(const char **)fileOffset);
            Error(errorString, Fatal);
        }

        start = (char *)DAlloc(g_heap, length + 32, "Scene.c", 258);
        finish = start + length;
        if (!start) {
            sprintf(errorString, "scene.c:LoadPrm(): Failed to allocate memory for file %s\n",
                    *(const char **)fileOffset);
            Error(errorString, Fatal);
        }

        if (LoadFile(fileOffset, start) != length) {
            sprintf(errorString, "scene.c:LoadPrm(): Failed to load file %s\n",
                    *(const char **)fileOffset);
            Error(errorString, Fatal);
        }
    } else {
        length = wad->sizes[wad->current];
        start = (char *)DAlloc(g_heap, length + 32, "Scene.c", 284);
        finish = start + length;
        ReadNextWadFile(wad, start);
    }

    prm.ptr = start;
    firstObject = prm.object;
    lastObject = NULL;

    while (prm.ptr < finish) {
        object = prm.object++;
        object->skeleton = prm.skeleton++;
        object->skeleton->relative.t[0] >>= 1;
        object->skeleton->relative.t[1] >>= 1;
        object->skeleton->relative.t[2] >>= 1;

        object->next = NULL;
        object->vertices = prm.svector;
        prm.svector += object->vertexCount;

        object->normals = prm.svector;
        prm.svector += object->normalCount;

        object->primitives = prm.primitive;

        untexturedCount = 0;
        for (i = 0; i < object->primitiveCount; i++) {
            switch (prm.f3->type) {
                case TypeF3:
                    untexturedCount++;
                    prm.f3++;
                    break;
                case TypeFT3:
                    prm.ft3++;
                    break;
                case TypeF4:
                    untexturedCount++;
                    prm.f4++;
                    break;
                case TypeFT4:
                    prm.ft4++;
                    break;
                case TypeG3:
                    untexturedCount++;
                    prm.g3++;
                    break;
                case TypeGT3:
                    prm.gt3++;
                    break;
                case TypeG4:
                    untexturedCount++;
                    prm.g4++;
                    break;
                case TypeGT4:
                    prm.gt4++;
                    break;
                case TypeLSF3:
                    untexturedCount++;
                    prm.lsf3++;
                    break;
                case TypeLSF4:
                    untexturedCount++;
                    prm.lsf4++;
                    break;
                case TypeLSFT3:
                    prm.lsft3++;
                    break;
                case TypeLSFT4:
                    prm.lsft4++;
                    break;
                case TypeLSG3:
                    untexturedCount++;
                    prm.lsg3++;
                    break;
                case TypeLSG4:
                    untexturedCount++;
                    prm.lsg4++;
                    break;
                case TypeLSGT3:
                    prm.lsgt3++;
                    break;
                case TypeLSGT4:
                    prm.lsgt4++;
                    break;
                case TypeTSPR:
                case TypeBSPR:
                    prm.spr->texture += (s16)renderList->textureStart;
                    prm.spr++;
                    break;
                case TypeSpline:
                    prm.spline++;
                    break;
                case TypeInfiniteLight:
                    prm.infiniteLight++;
                    break;
                case TypePointLight:
                    prm.pointLight++;
                    break;
                case TypeSpotLight:
                    prm.spotLight++;
                    break;
                default:
                    sprintf(errorString, "Bad primitive type \n");
                    Error(errorString, Fatal);
                    break;
            }
        }

        group = (RenderGroup *)DAlloc(g_heap, sizeof(RenderGroup), "Scene.c", 482);
        object->bspTree = group;
        if (!group) {
            return firstObject;
        }

        object->vtx = BuildObjectVertices(object);
        if (object->normalCount > 0) {
            ScaleNormals(object);
        }

        texCount = CountObjectTextures(object, renderList);
        InitObjectRenderGroup(object, texCount, untexturedCount, renderList);
        if (untexturedCount) {
            texCount++;
        }

        entry = group->entries;
        for (j = 0; j < texCount; j++, entry++) {
            BuildObjectDisplayList(object, (Vtx *)object->vtx, entry, renderList, 0);
        }

        switch (frontendOptions_g.trackNo) {
            case 1:
                if (CompareObjectName(object->name, "rot", 3)) {
                    RegisterTrack1RotObj(object);
                }
                break;
            case 2:
                if (DMemCmp(object->name, "fannybasher", 11) == 0) {
                    RegisterTrack2BasherObj(object);
                }
                if (DMemCmp(object->name, "fart", 4) == 0) {
                    RegisterTrack2FartObj(object);
                }
                break;
            case 3:
                if (DMemCmp(object->name, "C_CART_1", 8) == 0) {
                    RegisterTrack3CartObj(object, 0);
                } else if (DMemCmp(object->name, "C_CART_2", 8) == 0) {
                    RegisterTrack3CartObj(object, 1);
                } else if (DMemCmp(object->name, "M_CART_4", 8) == 0) {
                    RegisterTrack3CartObj(object, 2);
                } else if (DMemCmp(object->name, "V_STEAM", 7) == 0) {
                    RegisterTrack3SteamObj(object, 0);
                } else if (DMemCmp(object->name, "J_STEAM", 7) == 0) {
                    RegisterTrack3SteamObj(object, 1);
                }
                break;
            case 4:
                break;
            case 5:
                if (DMemCmp(object->name, "flame", 5) == 0) {
                    RegisterTrack5FlameObj(object);
                }
                break;
            case 6:
                if (DMemCmp(object->name, "claw1", 5) == 0) {
                    RegisterTrack6ClawObj(object, 0);
                } else if (DMemCmp(object->name, "claw2", 5) == 0) {
                    RegisterTrack6ClawObj(object, 1);
                } else if (DMemCmp(object->name, "J_STEAM", 7) == 0) {
                    RegisterTrack6SteamObj(object, 0);
                }
                break;
            case 7:
                break;
        }

        if (lastObject) {
            lastObject->next = object;
        }
        lastObject = object;
    }

    return firstObject;
}

Object *WADLoadPrm(WADFILE *wad, RenderList *renderList) {
    return LoadPrmCommon(wad, 0, renderList);
}

Object *LoadPrm(u32 fileOffset, RenderList *renderList) {
    return LoadPrmCommon(NULL, fileOffset, renderList);
}

Vtx *BuildObjectVertices(Object *object) {
    Vtx *verts;
    Vtx *v;
    SVECTOR *src;
    s32 i;

    verts = (Vtx *)DAlloc(g_heap, object->vertexCount * sizeof(Vtx), "Scene.c", 643);
    if (!verts) {
        return NULL;
    }

    v = verts;
    src = (SVECTOR *)object->vertices;
    for (i = 0; i < object->vertexCount; i++, v++, src++) {
        v->v.ob[0] = src->vx >> 1;
        v->v.ob[1] = src->vy >> 1;
        v->v.ob[2] = src->vz >> 1;
        v->v.flag = 0;
        v->v.tc[0] = -1;
        v->v.tc[1] = -1;
        v->v.cn[0] = 0;
        v->v.cn[1] = 0;
        v->v.cn[2] = 0;
        v->v.cn[3] = 1;
    }

    return verts;
}

void BuildInstanceDisplayLists(Object *dst, Object *src, RenderList *renderList) {
    RenderGroup *dstGroup;
    RenderGroup *srcGroup;
    s32 count;
    s32 untextured;
    s32 i;
    s32 j;
    RenderGroupEntry *entry;

    untextured = 0;
    dst->bspTree = DAlloc(g_heap, sizeof(RenderGroup), "Scene.c", 676);

    srcGroup = (RenderGroup *)src->bspTree;
    count = srcGroup->count;
    dstGroup = (RenderGroup *)dst->bspTree;
    for (i = 0; i < count; i++) {
        if (srcGroup->entries[i].textureIndex == -1) {
            untextured++;
        }
    }

    if (untextured) {
        count--;
    }

    InitObjectRenderGroup(dst, count, untextured, renderList);

    if (untextured) {
        count++;
    }

    entry = dstGroup->entries;
    for (j = 0; j < count; j++, entry++) {
        BuildObjectDisplayList(dst, (Vtx *)dst->vtx, entry, renderList, 0);
    }
}

Object *NewInstanceWithDlist(Object *source, RenderList *renderList) {
    Object *copy;
    s32 i;

    copy = (Object *)DAlloc(g_heap, sizeof(Object), "Scene.c", 714);
    copy->vertexCount = source->vertexCount;
    copy->vertices = source->vertices;
    copy->normalCount = source->normalCount;
    copy->normals = source->normals;
    copy->primitiveCount = source->primitiveCount;
    copy->primitives = source->primitives;

    copy->skeleton = (Skeleton *)DAlloc(g_heap, sizeof(Skeleton), "Scene.c", 725);
    copy->skeleton->super = source->skeleton->super;
    copy->skeleton->sub = source->skeleton->sub;
    copy->skeleton->next = source->skeleton->next;

    copy->skeleton->relative.m[0][0] = source->skeleton->relative.m[0][0];
    copy->skeleton->relative.m[0][1] = source->skeleton->relative.m[0][1];
    copy->skeleton->relative.m[0][2] = source->skeleton->relative.m[0][2];
    copy->skeleton->relative.m[1][0] = source->skeleton->relative.m[1][0];
    copy->skeleton->relative.m[1][1] = source->skeleton->relative.m[1][1];
    copy->skeleton->relative.m[1][2] = source->skeleton->relative.m[1][2];
    copy->skeleton->relative.m[2][0] = source->skeleton->relative.m[2][0];
    copy->skeleton->relative.m[2][1] = source->skeleton->relative.m[2][1];
    copy->skeleton->relative.m[2][2] = source->skeleton->relative.m[2][2];

    copy->skeleton->relative.t[0] = source->skeleton->relative.t[0];
    copy->skeleton->relative.t[1] = source->skeleton->relative.t[1];
    copy->skeleton->relative.t[2] = source->skeleton->relative.t[2];

    copy->extent = source->extent;
    copy->flags = source->flags;
    copy->next = NULL;

    for (i = 0; i < 16; i++) {
        copy->name[i] = source->name[i];
    }

    copy->vtx = BuildObjectVertices(copy);
    BuildInstanceDisplayLists(copy, source, renderList);

    return copy;
}

Object *NewInstance(Object *source) {
    Object *copy;
    s32 i;

    copy = (Object *)DAlloc(g_heap, sizeof(Object), "Scene.c", 774);
    copy->vertexCount = source->vertexCount;
    copy->vertices = source->vertices;
    copy->normalCount = source->normalCount;
    copy->normals = source->normals;
    copy->primitiveCount = source->primitiveCount;
    copy->primitives = source->primitives;
    copy->vtx = source->vtx;
    copy->bspTree = source->bspTree;

    copy->skeleton = (Skeleton *)DAlloc(g_heap, sizeof(Skeleton), "Scene.c", 789);
    copy->skeleton->super = source->skeleton->super;
    copy->skeleton->sub = source->skeleton->sub;
    copy->skeleton->next = source->skeleton->next;

    copy->skeleton->relative.m[0][0] = source->skeleton->relative.m[0][0];
    copy->skeleton->relative.m[0][1] = source->skeleton->relative.m[0][1];
    copy->skeleton->relative.m[0][2] = source->skeleton->relative.m[0][2];
    copy->skeleton->relative.m[1][0] = source->skeleton->relative.m[1][0];
    copy->skeleton->relative.m[1][1] = source->skeleton->relative.m[1][1];
    copy->skeleton->relative.m[1][2] = source->skeleton->relative.m[1][2];
    copy->skeleton->relative.m[2][0] = source->skeleton->relative.m[2][0];
    copy->skeleton->relative.m[2][1] = source->skeleton->relative.m[2][1];
    copy->skeleton->relative.m[2][2] = source->skeleton->relative.m[2][2];

    copy->skeleton->relative.t[0] = source->skeleton->relative.t[0];
    copy->skeleton->relative.t[1] = source->skeleton->relative.t[1];
    copy->skeleton->relative.t[2] = source->skeleton->relative.t[2];

    copy->extent = source->extent;
    copy->flags = source->flags;
    copy->next = NULL;

    for (i = 0; i < 16; i++) {
        copy->name[i] = source->name[i];
    }

    return copy;
}

Gfx *ModifyVertexST(Gfx *dl, s32 vtx, s32 u, s32 v, s32 texIdx) {
    N64Texture *tex;

    tex = g_textureTable[texIdx];
    u <<= 6;
    v <<= 6;
    u = (u * tex->uScale) >> 16;
    v = (v * tex->vScale) >> 16;
    u <<= 16;
    gSPModifyVertex(dl++, vtx, G_MWO_POINT_ST, u | v);
    return dl;
}

s32 CountObjectTextures(Object *object, RenderList *renderList) {
    u32 tex;
    s32 i;
    s32 count;
    Primitive *poly;

    count = 0;
    for (tex = 0; tex < renderList->bucketCount; tex++) {
        poly = (Primitive *)object->primitives;
        for (i = 0; i < object->primitiveCount; i++) {
            if (PolyUsesTexture(poly, tex)) {
                count++;
                break;
            }
            poly = AdvancePoly(poly);
        }
    }

    for (tex = 0; (s32)tex < g_animTextureCount; tex++) {
        poly = (Primitive *)object->primitives;
        for (i = 0; i < object->primitiveCount; i++) {
            if (PolyUsesTexture(poly, tex | RENDER_TEX_SKIP_FLAG)) {
                count++;
                break;
            }
            poly = AdvancePoly(poly);
        }
    }

    return count;
}

void InitObjectRenderGroup(Object *object, s32 texCount, s32 hasUntextured, RenderList *renderList) {
    RenderGroup *group;
    RenderGroupEntry *entry;
    u32 tex;
    s32 i;
    Primitive *poly;

    group = (RenderGroup *)object->bspTree;
    if (hasUntextured) {
        texCount++;
    }

    group->entries = (RenderGroupEntry *)DAlloc(g_heap, texCount * sizeof(RenderGroupEntry), "Scene.c", 907);
    group->count = texCount;
    entry = group->entries;

    if (hasUntextured) {
        entry->textureIndex = -1;
        entry++;
    }

    for (tex = 0; tex < renderList->bucketCount; tex++) {
        poly = (Primitive *)object->primitives;
        for (i = 0; i < object->primitiveCount; i++) {
            if (PolyUsesTexture(poly, tex)) {
                entry->textureIndex = tex;
                entry++;
                break;
            }
            poly = AdvancePoly(poly);
        }
    }

    for (tex = 0; (s32)tex < g_animTextureCount; tex++) {
        poly = (Primitive *)object->primitives;
        for (i = 0; i < object->primitiveCount; i++) {
            if (PolyUsesTexture(poly, tex | RENDER_TEX_SKIP_FLAG)) {
                entry->textureIndex = tex | RENDER_TEX_SKIP_FLAG;
                entry++;
                break;
            }
            poly = AdvancePoly(poly);
        }
    }
}

s32 CompareObjectName(const char *s1, const char *s2, s32 len) {
    while (len-- > 0) {
        if (*s1++ != *s2++) {
            return 0;
        }
    }
    return 1;
}

#define SET_VERTEX_UV(coordExpr, uVal, vVal, vtxSlot)                                                 \
    if (verts[(coordExpr)].v.tc[0] == -1) {                                                           \
        verts[(coordExpr)].v.tc[0] = (uVal) << 6;                                                     \
        verts[(coordExpr)].v.tc[1] = (vVal) << 6;                                                     \
        vtxState[(coordExpr)].s = (uVal) << 6;                                                        \
        vtxState[(coordExpr)].t = (vVal) << 6;                                                        \
    } else if (vtxState[(coordExpr)].s != ((uVal) << 6) ||                                            \
               vtxState[(coordExpr)].t != ((vVal) << 6)) {                                            \
        vtxState[(coordExpr)].s = (uVal) << 6;                                                        \
        vtxState[(coordExpr)].t = (vVal) << 6;                                                        \
        dl = ModifyVertexST(dl, vtxIdx[(vtxSlot)], (uVal), (vVal),                                    \
                            texture + renderList->textureStart);                                      \
    }

#define SET_GOURAUD_COLOURS(primPtr, numVerts, modAlpha)                                              \
    for (j = 0; j < (numVerts); j++) {                                                                \
        if (verts[(primPtr)->coords[j]].v.cn[3] == 1) {                                               \
            verts[(primPtr)->coords[j]].v.cn[0] = (primPtr)->colour[j].r;                             \
            verts[(primPtr)->coords[j]].v.cn[1] = (primPtr)->colour[j].g;                             \
            verts[(primPtr)->coords[j]].v.cn[2] = (primPtr)->colour[j].b;                             \
            verts[(primPtr)->coords[j]].v.cn[3] = 0x7F;                                               \
            vtxState[(primPtr)->coords[j]].r = (primPtr)->colour[j].r;                                \
            vtxState[(primPtr)->coords[j]].g = (primPtr)->colour[j].g;                                \
            vtxState[(primPtr)->coords[j]].b = (primPtr)->colour[j].b;                                \
        } else if (vtxState[(primPtr)->coords[j]].r != (primPtr)->colour[j].r ||                      \
                   vtxState[(primPtr)->coords[j]].g != (primPtr)->colour[j].g ||                      \
                   vtxState[(primPtr)->coords[j]].b != (primPtr)->colour[j].b) {                      \
            vtxState[(primPtr)->coords[j]].r = (primPtr)->colour[j].r;                                \
            vtxState[(primPtr)->coords[j]].g = (primPtr)->colour[j].g;                                \
            vtxState[(primPtr)->coords[j]].b = (primPtr)->colour[j].b;                                \
            col = ((primPtr)->colour[j].r << 24) | ((primPtr)->colour[j].g << 16) |                   \
                  ((primPtr)->colour[j].b << 8) | (modAlpha);                                         \
            gSPModifyVertex(dl++, vtxIdx[j], G_MWO_POINT_RGBA, col);                                  \
        }                                                                                             \
    }

#define SET_LIT_NORMALS(primPtr, numVerts)                                                            \
    for (j = 0; j < (numVerts); j++) {                                                                \
        if (verts[(primPtr)->coords[j]].v.cn[3] == 1) {                                               \
            verts[(primPtr)->coords[j]].n.n[0] =                                                      \
                ((SVECTOR *)object->normals)[(primPtr)->normals[j]].vx;                               \
            verts[(primPtr)->coords[j]].n.n[1] =                                                      \
                ((SVECTOR *)object->normals)[(primPtr)->normals[j]].vy;                               \
            verts[(primPtr)->coords[j]].n.n[2] =                                                      \
                ((SVECTOR *)object->normals)[(primPtr)->normals[j]].vz;                               \
            verts[(primPtr)->coords[j]].n.a = 0x7F;                                                   \
        }                                                                                             \
    }

#define COMPUTE_QUAD_UVS(primPtr)                                                                     \
    if ((primPtr)->flag & TileUVMask) {                                                               \
        uScale = (primPtr)->pad1 >> 8;                                                                \
        vScale = (primPtr)->pad1 & 0xFF;                                                              \
    } else {                                                                                          \
        uScale = vScale = 1;                                                                          \
    }                                                                                                 \
    u0 = (primPtr)->u0 * uScale;                                                                      \
    v0 = (primPtr)->v0 * vScale;                                                                      \
    u1 = (primPtr)->u1 * uScale;                                                                      \
    v1 = (primPtr)->v1 * vScale;                                                                      \
    u2 = (primPtr)->u2 * uScale;                                                                      \
    v2 = (primPtr)->v2 * vScale;                                                                      \
    u3 = (primPtr)->u3 * uScale;                                                                      \
    v3 = (primPtr)->v3 * vScale

#define COMPUTE_TRI_UVS(primPtr)                                                                      \
    if ((primPtr)->flag & TileUVMask) {                                                               \
        uScale = (primPtr)->pad1 >> 8;                                                                \
        vScale = (primPtr)->pad1 & 0xFF;                                                              \
    } else {                                                                                          \
        uScale = vScale = 1;                                                                          \
    }                                                                                                 \
    u0 = (primPtr)->u0 * uScale;                                                                      \
    v0 = (primPtr)->v0 * vScale;                                                                      \
    u1 = (primPtr)->u1 * uScale;                                                                      \
    v1 = (primPtr)->v1 * vScale;                                                                      \
    u2 = (primPtr)->u2 * uScale;                                                                      \
    v2 = (primPtr)->v2 * vScale

void BuildObjectDisplayList(Object *object, Vtx *verts, RenderGroupEntry *rData, RenderList *renderList,
                            s32 preload) {
    s32 cache[32];
    s32 vtxIdx[4];
    s32 texture;
    s32 vCount;
    s32 numCoords;
    s32 i;
    s32 cacheCount;
    s32 size;
    s16 u0, u1, u2, u3;
    s16 v0, v1, v2, v3;
    s32 lastCull;
    s32 pCount;
    s32 j;
    s32 k;
    u64 combineMode;
    s32 cull;
    u32 col;
    s16 uScale;
    s16 vScale;
    VtxState *vtxState;
    Gfx *dl;
    Prm prm;

    combineMode = 0;
    lastCull = -1;
    size = 0;
    preload = 0;

    texture = rData->textureIndex;
    vtxState = (VtxState *)DAlloc(g_heap, object->vertexCount * sizeof(VtxState), "Scene.c", 991);
    for (i = 0; i < object->vertexCount; i++) {
        vtxState[i].s = -1;
        vtxState[i].t = -1;
    }

    rData->dl = (Gfx *)DAlloc(g_heap, size * sizeof(Gfx), "Scene.c", 998);

    vCount = CountTextureVertices(object, texture);
    if (!vCount) {
        __assert("vCount", "Scene.c", 1003);
    }

    pCount = CountTexturePolys(object, texture);
    if (!pCount) {
        __assert("pCount", "Scene.c", 1007);
    }

    if (vCount >= 32) {
        size = (pCount * 16) + 1;
    } else {
        size = (vCount * 3 + pCount * 4) + 1;
    }

    size += 20;
    if (!size) {
        __assert("size", "Scene.c", 1040);
    }

    rData->dl = (Gfx *)DAlloc(g_heap, size * sizeof(Gfx), "Scene.c", 1041);
    if (!rData->dl) {
        __assert("rData->dlist", "Scene.c", 1042);
    }

    dl = rData->dl;
    if (vCount > g_maxTextureVerts) {
        g_maxTextureVerts = vCount;
    }

    for (i = 0; i < 32; i++) {
        cache[i] = -1;
    }
    cacheCount = 0;

    prm.primitive = (Primitive *)object->primitives;
    for (i = 0; i < object->primitiveCount; i++) {
        if (PolyUsesTexture(prm.primitive, texture)) {
            switch (prm.f3->type) {
                case TypeF4:
                case TypeFT4:
                case TypeG4:
                case TypeGT4:
                case TypeLSF4:
                case TypeLSFT4:
                case TypeLSG4:
                case TypeLSGT4:
                    numCoords = 4;
                    break;
                case TypeF3:
                case TypeFT3:
                case TypeG3:
                case TypeGT3:
                case TypeLSF3:
                case TypeLSFT3:
                case TypeLSG3:
                case TypeLSGT3:
                    numCoords = 3;
                    break;
                case TypeTSPR:
                case TypeBSPR:
                    numCoords = 0;
                    break;
            }

            if (preload) {
                for (j = 0; j < numCoords; j++) {
                    vtxIdx[j] = prm.f3->coords[j];
                }
            } else if (vCount >= 32) {
                for (j = 0; j < numCoords; j++) {
                    gSPVertex(dl++, OS_K0_TO_PHYSICAL(&verts[prm.f3->coords[j]]), 1, j);
                    vtxIdx[j] = j;
                }
            } else {
                for (j = 0; j < numCoords; j++) {
                    for (k = 0; k < cacheCount; k++) {
                        if (cache[k] == prm.f3->coords[j]) {
                            vtxIdx[j] = k;
                            break;
                        }
                    }
                    if (k >= cacheCount) {
                        cache[cacheCount] = prm.f3->coords[j];
                        gSPVertex(dl++, OS_K0_TO_PHYSICAL(&verts[prm.f3->coords[j]]), 1, cacheCount);
                        vtxIdx[j] = cacheCount;
                        cacheCount++;
                    }
                }
            }

            if (frontendOptions_g.trackNo == 1) {
                if (CompareObjectName(object->name, "8x", 2) ||
                    CompareObjectName(object->name, "6x", 2) ||
                    CompareObjectName(object->name, "4x", 2) ||
                    CompareObjectName(object->name, "2x", 2) ||
                    CompareObjectName(object->name, "grid", 4) ||
                    CompareObjectName(object->name, "bill", 4) ||
                    CompareObjectName(object->name, "rock", 4) ||
                    CompareObjectName(object->name, "cave_ent", 8) ||
                    CompareObjectName(object->name, "caveback", 8)) {
                    if (prm.ft4->v0 < 4) {
                        prm.ft4->v0 = 4;
                    }
                    if (prm.ft4->v1 < 4) {
                        prm.ft4->v1 = 4;
                    }
                    if (prm.ft4->v2 < 4) {
                        prm.ft4->v2 = 4;
                    }
                    if (prm.ft4->v3 < 4) {
                        prm.ft4->v3 = 4;
                    }
                    if (prm.ft4->u0 < 4) {
                        prm.ft4->u0 = 4;
                    }
                    if (prm.ft4->u1 < 4) {
                        prm.ft4->u1 = 4;
                    }
                    if (prm.ft4->u2 < 4) {
                        prm.ft4->u2 = 4;
                    }
                    if (prm.ft4->u3 < 4) {
                        prm.ft4->u3 = 4;
                    }
                }
            }

            if (CompareObjectName(object->name, "shield", 6)) {
                gDPSetRenderMode(dl++, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF2);
            }

            cull = prm.f3->flag & 1;
            if (cull != lastCull) {
                if (cull) {
                    gSPSetGeometryMode(dl++, G_CULL_FRONT);
                } else {
                    gSPClearGeometryMode(dl++, G_CULL_FRONT);
                }
                lastCull = cull;
            }

            switch (prm.f3->type) {
                case TypeF4:
                    gDPPipeSync(dl++);
                    gDPSetPrimColor(dl++, 0, 0, prm.f4->colour.r, prm.f4->colour.g, prm.f4->colour.b,
                                    0xFF);
                    if (combineMode != 1) {
                        gDPSetCombineMode(dl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
                        combineMode = 1;
                    }
                    gSP2Triangles(dl++, vtxIdx[0], vtxIdx[1], vtxIdx[2], 0, vtxIdx[2], vtxIdx[1],
                                  vtxIdx[3], 0);
                    break;

                case TypeFT4:
                    gDPPipeSync(dl++);
                    gDPSetPrimColor(dl++, 0, 0, prm.ft4->colour.r, prm.ft4->colour.g, prm.ft4->colour.b,
                                    0x5F);
                    if (combineMode != 2) {
                        gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                        combineMode = 2;
                    }
                    COMPUTE_QUAD_UVS(prm.ft4);
                    SET_VERTEX_UV(prm.ft4->coords[0], u0, v0, 0);
                    SET_VERTEX_UV(prm.ft4->coords[1], u1, v1, 1);
                    SET_VERTEX_UV(prm.ft4->coords[2], u2, v2, 2);
                    SET_VERTEX_UV(prm.ft4->coords[3], u3, v3, 3);
                    gSP2Triangles(dl++, vtxIdx[0], vtxIdx[1], vtxIdx[2], 0, vtxIdx[2], vtxIdx[1],
                                  vtxIdx[3], 0);
                    break;

                case TypeG4:
                    gDPPipeSync(dl++);
                    SET_GOURAUD_COLOURS(prm.g4, 4, 0x7F);
                    if (combineMode != 3) {
                        gDPSetCombineMode(dl++, G_CC_SHADE, G_CC_SHADE);
                        combineMode = 3;
                    }
                    gSP2Triangles(dl++, vtxIdx[0], vtxIdx[1], vtxIdx[2], 0, vtxIdx[2], vtxIdx[1],
                                  vtxIdx[3], 0);
                    break;

                case TypeGT4:
                    gDPPipeSync(dl++);
                    SET_GOURAUD_COLOURS(prm.gt4, 4, 0x7F);
                    if (combineMode != 4) {
                        gDPSetCombineMode(dl++, G_CC_MODULATEIA, G_CC_MODULATEIA);
                        combineMode = 4;
                    }
                    COMPUTE_QUAD_UVS(prm.gt4);
                    SET_VERTEX_UV(prm.gt4->coords[0], u0, v0, 0);
                    SET_VERTEX_UV(prm.gt4->coords[1], u1, v1, 1);
                    SET_VERTEX_UV(prm.gt4->coords[2], u2, v2, 2);
                    SET_VERTEX_UV(prm.gt4->coords[3], u3, v3, 3);
                    gSP2Triangles(dl++, vtxIdx[0], vtxIdx[1], vtxIdx[2], 0, vtxIdx[2], vtxIdx[1],
                                  vtxIdx[3], 0);
                    break;

                case TypeF3:
                    gDPPipeSync(dl++);
                    gDPSetPrimColor(dl++, 0, 0, prm.f3->colour.r, prm.f3->colour.g, prm.f3->colour.b,
                                    0x7F);
                    if (combineMode != 1) {
                        gDPSetCombineMode(dl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
                        combineMode = 1;
                    }
                    gSP1Triangle(dl++, vtxIdx[0], vtxIdx[1], vtxIdx[2], 0);
                    break;

                case TypeFT3:
                    gDPPipeSync(dl++);
                    gDPSetPrimColor(dl++, 0, 0, prm.ft3->colour.r, prm.ft3->colour.g, prm.ft3->colour.b,
                                    0x5F);
                    if (combineMode != 2) {
                        gDPSetCombineMode(dl++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
                        combineMode = 2;
                    }
                    COMPUTE_TRI_UVS(prm.ft3);
                    SET_VERTEX_UV(prm.ft3->coords[0], u0, v0, 0);
                    SET_VERTEX_UV(prm.ft3->coords[1], u1, v1, 1);
                    SET_VERTEX_UV(prm.ft3->coords[2], u2, v2, 2);
                    gSP1Triangle(dl++, vtxIdx[0], vtxIdx[1], vtxIdx[2], 0);
                    break;

                case TypeG3:
                    gDPPipeSync(dl++);
                    SET_GOURAUD_COLOURS(prm.g3, 3, 0xFF);
                    if (combineMode != 3) {
                        gDPSetCombineMode(dl++, G_CC_SHADE, G_CC_SHADE);
                        combineMode = 3;
                    }
                    gSP1Triangle(dl++, vtxIdx[0], vtxIdx[1], vtxIdx[2], 0);
                    break;

                case TypeGT3:
                    gDPPipeSync(dl++);
                    SET_GOURAUD_COLOURS(prm.gt3, 3, 0xFF);
                    if (combineMode != 4) {
                        gDPSetCombineMode(dl++, G_CC_MODULATEIA, G_CC_MODULATEIA);
                        combineMode = 4;
                    }
                    COMPUTE_TRI_UVS(prm.gt3);
                    SET_VERTEX_UV(prm.gt3->coords[0], u0, v0, 0);
                    SET_VERTEX_UV(prm.gt3->coords[1], u1, v1, 1);
                    SET_VERTEX_UV(prm.gt3->coords[2], u2, v2, 2);
                    gSP1Triangle(dl++, vtxIdx[0], vtxIdx[1], vtxIdx[2], 0);
                    break;

                case TypeLSG4:
                    gDPPipeSync(dl++);
                    SET_GOURAUD_COLOURS(prm.lsg4, 4, 0x7F);
                    if (combineMode != 3) {
                        gDPSetCombineMode(dl++, G_CC_SHADE, G_CC_SHADE);
                        combineMode = 3;
                    }
                    gSP2Triangles(dl++, vtxIdx[0], vtxIdx[1], vtxIdx[2], 0, vtxIdx[2], vtxIdx[1],
                                  vtxIdx[3], 0);
                    break;

                case TypeLSGT4:
                    gDPPipeSync(dl++);
                    SET_LIT_NORMALS(prm.lsgt4, 4);
                    if (combineMode != 4) {
                        gDPSetCombineMode(dl++, G_CC_MODULATEIA, G_CC_MODULATEIA);
                        combineMode = 4;
                    }
                    COMPUTE_QUAD_UVS(prm.lsgt4);
                    SET_VERTEX_UV(prm.lsgt4->coords[0], u0, v0, 0);
                    SET_VERTEX_UV(prm.lsgt4->coords[1], u1, v1, 1);
                    SET_VERTEX_UV(prm.lsgt4->coords[2], u2, v2, 2);
                    SET_VERTEX_UV(prm.lsgt4->coords[3], u3, v3, 3);
                    gSP2Triangles(dl++, vtxIdx[0], vtxIdx[1], vtxIdx[2], 0, vtxIdx[2], vtxIdx[1],
                                  vtxIdx[3], 0);
                    break;

                case TypeLSG3:
                    gDPPipeSync(dl++);
                    SET_GOURAUD_COLOURS(prm.lsg3, 3, 0x7F);
                    if (combineMode != 3) {
                        gDPSetCombineMode(dl++, G_CC_SHADE, G_CC_SHADE);
                        combineMode = 3;
                    }
                    gSP1Triangle(dl++, vtxIdx[0], vtxIdx[1], vtxIdx[2], 0);
                    break;

                case TypeLSGT3:
                    gDPPipeSync(dl++);
                    SET_LIT_NORMALS(prm.lsgt3, 3);
                    if (combineMode != 4) {
                        gDPSetCombineMode(dl++, G_CC_MODULATEIA, G_CC_MODULATEIA);
                        combineMode = 4;
                    }
                    COMPUTE_TRI_UVS(prm.lsgt3);
                    SET_VERTEX_UV(prm.lsgt3->coords[0], u0, v0, 0);
                    SET_VERTEX_UV(prm.lsgt3->coords[1], u1, v1, 1);
                    SET_VERTEX_UV(prm.lsgt3->coords[2], u2, v2, 2);
                    gSP1Triangle(dl++, vtxIdx[0], vtxIdx[1], vtxIdx[2], 0);
                    break;
            }
        }

        prm.primitive = AdvancePoly(prm.primitive);
    }

    gSPEndDisplayList(dl++);
    DFree(g_heap, vtxState);

    if ((dl - rData->dl) > size) {
        while (1) {
            __asm__("break 0x407");
        }
    }

    DCheckHeap(g_heapA);
}

s32 PolyUsesTexture(Primitive *poly, s32 texture) {
    Prm prm;

    prm.primitive = poly;
    switch (prm.f3->type) {
        case TypeFT3:
            if (prm.ft3->texture == texture) {
                return 1;
            }
            break;
        case TypeFT4:
            if (prm.ft4->texture == texture) {
                return 1;
            }
            break;
        case TypeGT3:
            if (prm.gt3->texture == texture) {
                return 1;
            }
            break;
        case TypeGT4:
            if (prm.gt4->texture == texture) {
                return 1;
            }
            break;
        case TypeLSFT3:
            if (prm.lsft3->texture == texture) {
                return 1;
            }
            break;
        case TypeLSFT4:
            if (prm.lsft4->texture == texture) {
                return 1;
            }
            break;
        case TypeLSGT3:
            if (prm.lsgt3->texture == texture) {
                return 1;
            }
            break;
        case TypeLSGT4:
            if (prm.lsgt4->texture == texture) {
                return 1;
            }
            break;
        case TypeF3:
        case TypeF4:
        case TypeG3:
        case TypeG4:
        case TypeLSF3:
        case TypeLSF4:
        case TypeLSG3:
        case TypeLSG4:
            if (texture == -1) {
                return 1;
            }
            break;
    }

    return 0;
}

Primitive *AdvancePoly(Primitive *poly) {
    Prm prm;

    prm.primitive = poly;
    switch (prm.f3->type) {
        case TypeF3:
            prm.f3++;
            break;
        case TypeFT3:
            prm.ft3++;
            break;
        case TypeF4:
            prm.f4++;
            break;
        case TypeFT4:
            prm.ft4++;
            break;
        case TypeG3:
            prm.g3++;
            break;
        case TypeGT3:
            prm.gt3++;
            break;
        case TypeG4:
            prm.g4++;
            break;
        case TypeGT4:
            prm.gt4++;
            break;
        case TypeLSF3:
            prm.lsf3++;
            break;
        case TypeLSF4:
            prm.lsf4++;
            break;
        case TypeLSFT3:
            prm.lsft3++;
            break;
        case TypeLSFT4:
            prm.lsft4++;
            break;
        case TypeLSG3:
            prm.lsg3++;
            break;
        case TypeLSG4:
            prm.lsg4++;
            break;
        case TypeLSGT3:
            prm.lsgt3++;
            break;
        case TypeLSGT4:
            prm.lsgt4++;
            break;
        case TypeSpline:
            prm.spline++;
            break;
        case TypePointLight:
            prm.pointLight++;
            break;
        case TypeSpotLight:
            prm.spotLight++;
            break;
        case TypeInfiniteLight:
            prm.infiniteLight++;
            break;
        case TypeTSPR:
        case TypeBSPR:
            prm.spr++;
            break;
        default:
            sprintf(errorString, "Bad primitive type %x in AdvancePoly \n", prm.f3->type);
            Error(errorString, Fatal);
            break;
    }

    return prm.primitive;
}

s32 CountTextureVertices(Object *object, s32 texture) {
    s16 verts[256];
    s32 i;
    s32 j;
    s32 k;
    s32 count;
    s32 numCoords;
    s16 *coords;
    Prm prm;

    for (i = 0; i < 256; i++) {
        verts[i] = -1;
    }

    prm.primitive = (Primitive *)object->primitives;
    count = 0;

    for (i = 0; i < object->primitiveCount; i++) {
        switch (prm.f3->type) {
            case TypeF3:
            case TypeG3:
            case TypeLSF3:
            case TypeLSG3:
                if (texture != -1) {
                    goto next;
                }
                numCoords = 3;
                coords = prm.f3->coords;
                break;
            case TypeF4:
            case TypeG4:
            case TypeLSF4:
            case TypeLSG4:
                if (texture != -1) {
                    goto next;
                }
                coords = prm.f4->coords;
                numCoords = 4;
                break;
            case TypeFT3:
            case TypeGT3:
                if (prm.ft3->texture != texture) {
                    goto next;
                }
                coords = prm.ft3->coords;
                numCoords = 3;
                break;
            case TypeFT4:
            case TypeGT4:
                if (prm.ft4->texture != texture) {
                    goto next;
                }
                numCoords = 4;
                coords = prm.ft4->coords;
                break;
            case TypeLSFT3:
                if (prm.lsft3->texture != texture) {
                    goto next;
                }
                coords = prm.lsft3->coords;
                numCoords = 3;
                break;
            case TypeLSFT4:
                if (prm.lsft4->texture != texture) {
                    goto next;
                }
                numCoords = 4;
                coords = prm.lsft4->coords;
                break;
            case TypeLSGT3:
                if (prm.lsgt3->texture != texture) {
                    goto next;
                }
                coords = prm.lsgt3->coords;
                numCoords = 3;
                break;
            case TypeLSGT4:
                if (prm.lsgt4->texture != texture) {
                    goto next;
                }
                numCoords = 4;
                coords = prm.lsgt4->coords;
                break;
            default:
                goto next;
        }

        for (j = 0; j < numCoords; j++) {
            for (k = 0; k < count; k++) {
                if (verts[k] == coords[j]) {
                    goto found;
                }
            }
            verts[count++] = coords[j];
            if (count >= 256) {
                return 256;
            }
        found:;
        }
    next:
        prm.primitive = AdvancePoly(prm.primitive);
    }

    return count;
}

s32 CountTexturePolys(Object *object, s32 texture) {
    s32 i;
    s32 count;
    Prm prm;

    prm.primitive = (Primitive *)object->primitives;
    count = 0;

    for (i = 0; i < object->primitiveCount; i++) {
        switch (prm.f3->type) {
            case TypeF3:
            case TypeF4:
            case TypeG3:
            case TypeG4:
            case TypeLSF3:
            case TypeLSF4:
            case TypeLSG3:
            case TypeLSG4:
                if (texture == -1) {
                    count++;
                }
                break;
            case TypeFT3:
            case TypeGT3:
                if (prm.ft3->texture == texture) {
                    count++;
                }
                break;
            case TypeFT4:
            case TypeGT4:
                if (prm.ft4->texture == texture) {
                    count++;
                }
                break;
            case TypeLSFT3:
                if (prm.lsft3->texture == texture) {
                    count++;
                }
                break;
            case TypeLSFT4:
                if (prm.lsft4->texture == texture) {
                    count++;
                }
                break;
            case TypeLSGT3:
                if (prm.lsgt3->texture == texture) {
                    count++;
                }
                break;
            case TypeLSGT4:
                if (prm.lsgt4->texture == texture) {
                    count++;
                }
                break;
        }

        prm.primitive = AdvancePoly(prm.primitive);
    }

    return count;
}

void NormaliseVector3f(Vec3f *v) {
    f32 len;

    len = sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
    if (len != 0.0) {
        len = 1.0 / (f64)len;
        v->x *= len;
        v->y *= len;
        v->z *= len;
    } else {
        v->x = 0.0f;
        v->y = 0.0f;
        v->z = 0.0f;
    }
}

void CalcFaceNormal(s32 i0, s32 i1, s32 i2, Vtx *verts, Vec3f *out) {
    Vec3f v0;
    Vec3f v1;
    Vec3f v2;
    Vec3f e1;
    Vec3f e2;
    Vec3f n;

    v0.x = verts[i0].v.ob[0];
    v0.y = verts[i0].v.ob[1];
    v0.z = verts[i0].v.ob[2];

    v1.x = verts[i1].v.ob[0];
    v1.y = verts[i1].v.ob[1];
    v1.z = verts[i1].v.ob[2];

    v2.x = verts[i2].v.ob[0];
    v2.y = verts[i2].v.ob[1];
    v2.z = verts[i2].v.ob[2];

    e1.x = v1.x - v0.x;
    e1.y = v1.y - v0.y;
    e1.z = v1.z - v0.z;

    e2.x = v2.x - v0.x;
    e2.y = v2.y - v0.y;
    e2.z = v2.z - v0.z;

    n.x = e1.y * e2.z - e1.z * e2.y;
    n.y = e1.z * e2.x - e1.x * e2.z;
    n.z = e1.x * e2.y - e1.y * e2.x;

    NormaliseVector3f(&n);

    out->x = -n.x;
    out->y = -n.y;
    out->z = -n.z;
}

void CalcVertexNormals(Object *object, Vtx *verts) {
    Vec3f faceNormals[512];
    Vec3f avg;
    Prm prm;
    s32 i;
    s32 j;
    s32 k;
    s32 numCoords;
    s32 count;
    f32 nx;
    f32 ny;
    f32 nz;

    prm.primitive = (Primitive *)object->primitives;
    for (i = 0; i < object->primitiveCount; i++) {
        CalcFaceNormal(prm.f3->coords[0], prm.f3->coords[1], prm.f3->coords[2], verts, &faceNormals[i]);
        prm.primitive = AdvancePoly(prm.primitive);
    }

    for (i = 0; i < object->vertexCount; i++) {
        nz = 0.0f;
        ny = 0.0f;
        nx = 0.0f;
        count = 0;

        prm.primitive = (Primitive *)object->primitives;
        for (j = 0; j < object->primitiveCount; j++) {
            switch (prm.f3->type) {
                case TypeF4:
                case TypeFT4:
                case TypeG4:
                case TypeGT4:
                    numCoords = 4;
                    break;
                case TypeF3:
                case TypeFT3:
                case TypeG3:
                case TypeGT3:
                    numCoords = 3;
                    break;
                default:
                    numCoords = 0;
                    break;
            }

            for (k = 0; k < numCoords; k++) {
                if (prm.f3->coords[k] == i) {
                    nx += faceNormals[j].x;
                    ny += faceNormals[j].y;
                    nz += faceNormals[j].z;
                    count++;
                    break;
                }
            }

            prm.primitive = AdvancePoly(prm.primitive);
        }

        if (count == 0) {
            count = 1;
        }
        avg.x = nx / count;
        avg.y = ny / count;
        avg.z = nz / count;
        NormaliseVector3f(&avg);

        verts[i].n.n[0] = (s8)(avg.x * 127.0);
        verts[i].n.n[1] = (s8)(avg.y * 127.0);
        verts[i].n.n[2] = (s8)(avg.z * 127.0);
        verts[i].n.a = 0xFF;
    }
}

void BuildEnvMapDisplayList(Object *object, s32 a1, s32 a2, s32 lightType, Vtx **vtxOut, Gfx **dlOut) {
    s32 vtxIdx[4];
    s32 pCount;
    Vtx *verts;
    s32 vCount;
    s32 dlSize;
    Gfx *dl;
    s32 i;
    s32 j;
    s32 numCoords;
    Prm prm;

    vCount = object->vertexCount;
    pCount = object->primitiveCount;
    dlSize = pCount * 5 + 64;

    *vtxOut = (Vtx *)DAlloc(g_heap, vCount * sizeof(Vtx), "Scene.c", 2558);
    verts = *vtxOut;

    dl = (Gfx *)DAlloc(g_heap, dlSize * sizeof(Gfx), "Scene.c", 2561);
    *dlOut = dl;

    for (i = 0; i < vCount; i++) {
        verts[i].v.ob[0] = ((Vtx *)object->vtx)[i].v.ob[0];
        verts[i].v.ob[1] = ((Vtx *)object->vtx)[i].v.ob[1];
        verts[i].v.ob[2] = ((Vtx *)object->vtx)[i].v.ob[2];
    }

    CalcVertexNormals(object, verts);

    gDPSetCycleType(dl++, G_CYC_1CYCLE);
    gSPTexture(dl++, 0x07C0, 0x07C0, 0, G_TX_RENDERTILE, G_ON);
    gDPLoadTextureBlock(dl++, g_envMapTex, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0,
                        G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR,
                        5, 5, G_TX_NOLOD, G_TX_NOLOD);
    gSPClearGeometryMode(dl++, G_CLIPPING | G_CULL_BOTH);
    gDPSetTextureFilter(dl++, G_TF_BILERP);
    gDPSetCombineMode(dl++, G_CC_MODULATERGB, G_CC_MODULATERGB);
    gSPSetGeometryMode(dl++, G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR);

    switch (lightType) {
        case 1:
            gSPSetLights1(dl++, g_bronzeLight);
            break;
        case 2:
            gSPSetLights1(dl++, g_silverLight);
            break;
        case 3:
            gSPSetLights1(dl++, g_goldLight);
            break;
    }

    gDPPipeSync(dl++);
    gDPSetRenderMode(dl++, G_RM_ZB_OPA_SURF, G_RM_ZB_OPA_SURF2);
    gSPClearGeometryMode(dl++, G_CULL_BOTH);
    gSPSetGeometryMode(dl++, G_CLIPPING | G_SHADING_SMOOTH | G_SHADE | G_ZBUFFER);
    gDPPipeSync(dl++);

    prm.primitive = (Primitive *)object->primitives;
    for (i = 0; i < pCount; i++) {
        switch (prm.f3->type) {
            case TypeF4:
            case TypeFT4:
            case TypeG4:
            case TypeGT4:
                numCoords = 4;
                break;
            case TypeF3:
            case TypeFT3:
            case TypeG3:
            case TypeGT3:
                numCoords = 3;
                break;
            default:
                numCoords = 0;
                break;
        }

        for (j = 0; j < numCoords; j++) {
            gSPVertex(dl++, OS_K0_TO_PHYSICAL(&verts[prm.f3->coords[j]]), 1, j);
            vtxIdx[j] = j;
        }

        switch (prm.f3->type) {
            case TypeF4:
            case TypeFT4:
            case TypeG4:
            case TypeGT4:
                gSP2Triangles(dl++, vtxIdx[0], vtxIdx[1], vtxIdx[2], 0, vtxIdx[2], vtxIdx[1], vtxIdx[3], 0);
                break;
            case TypeF3:
            case TypeFT3:
            case TypeG3:
            case TypeGT3:
                gSP1Triangle(dl++, vtxIdx[0], vtxIdx[1], vtxIdx[2], 0);
                break;
        }

        prm.primitive = AdvancePoly(prm.primitive);
    }

    gSPClearGeometryMode(dl++, G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR);
    gSPSetGeometryMode(dl++, G_CLIPPING | G_ZBUFFER);
    gSPEndDisplayList(dl++);
}
