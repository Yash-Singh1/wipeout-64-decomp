/*
 * Track.c — Track geometry loader (TRF/TRV/TRS), view lists, and track renderer.
 *
 * Loads track vertices, faces, sections, and visibility lists from track WADs
 * (Europe omits the legacy CheckVersion() check and shifts DAlloc lines by +1),
 * compiles per-section F3DEX display lists, and renders visible road segments.
 */
#include "track.h"

#ident "$Revision: 1.34 $"
#ident "$Revision: 1.5 $"
#ident "$Revision: 1.17 $"

#define ABS(a) ((a) < 0 ? -(a) : (a))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#ifndef VERSION_EU
void CheckVersion(void) {
    TrackSection *section;
    s16 i;

    section = track->sections;
    for (i = 0; i < track->sectionCount; i++) {
        if (section->version != 8) {
            Error("Convert track with track10", Fatal);
        }
        section++;
    }
}
#endif

void IndexToPointers(void) {
    TrackSection *section;
    s16 i;

    section = track->sections;
    for (i = 0; i < track->sectionCount; i++) {
        section->prevSection.ptr = track->sections + section->prevSection.index;
        section->nextSection.ptr = track->sections + section->nextSection.index;
        if (section->junction.index != -1) {
            section->junction.ptr = track->sections + section->junction.index;
        }
        section++;
    }
}

Vector *FindSectionSkeletonCentre(TrackSection *section) {
    Face *face;
    s32 i;
    s32 j;
    s32 minX, maxX;
    s32 minY, maxY;
    s32 minZ, maxZ;
    s32 midX, midY, midZ;
    s32 bestDist;
    s32 bestSection;
    s32 dist;

    face = track->faces + section->faceStart;
    maxX = minX = track->vertices[face->vertex[0]].vx;
    maxY = minY = track->vertices[face->vertex[0]].vy;
    maxZ = minZ = track->vertices[face->vertex[0]].vz;

    for (i = 0; i < section->faceCount; i++) {
        minX = MIN(track->vertices[face->vertex[0]].vx, minX);
        minX = MIN(track->vertices[face->vertex[1]].vx, minX);
        minX = MIN(track->vertices[face->vertex[2]].vx, minX);
        minX = MIN(track->vertices[face->vertex[3]].vx, minX);

        maxX = MAX(track->vertices[face->vertex[0]].vx, maxX);
        maxX = MAX(track->vertices[face->vertex[1]].vx, maxX);
        maxX = MAX(track->vertices[face->vertex[2]].vx, maxX);
        maxX = MAX(track->vertices[face->vertex[3]].vx, maxX);

        minY = MIN(track->vertices[face->vertex[0]].vy, minY);
        minY = MIN(track->vertices[face->vertex[1]].vy, minY);
        minY = MIN(track->vertices[face->vertex[2]].vy, minY);
        minY = MIN(track->vertices[face->vertex[3]].vy, minY);

        maxY = MAX(track->vertices[face->vertex[0]].vy, maxY);
        maxY = MAX(track->vertices[face->vertex[1]].vy, maxY);
        maxY = MAX(track->vertices[face->vertex[2]].vy, maxY);
        maxY = MAX(track->vertices[face->vertex[3]].vy, maxY);

        minZ = MIN(track->vertices[face->vertex[0]].vz, minZ);
        minZ = MIN(track->vertices[face->vertex[1]].vz, minZ);
        minZ = MIN(track->vertices[face->vertex[2]].vz, minZ);
        minZ = MIN(track->vertices[face->vertex[3]].vz, minZ);

        maxZ = MAX(track->vertices[face->vertex[0]].vz, maxZ);
        maxZ = MAX(track->vertices[face->vertex[1]].vz, maxZ);
        maxZ = MAX(track->vertices[face->vertex[2]].vz, maxZ);
        maxZ = MAX(track->vertices[face->vertex[3]].vz, maxZ);

        face++;
    }

    bestDist = 0;
    bestSection = -1;
    midX = (maxX + minX) >> 1;
    midY = (maxY + minY) >> 1;
    midZ = (maxZ + minZ) >> 1;

    for (j = 0; j < track->sectionCount; j++) {
        TrackSection *sec = track->sections + j;
        if (abs(maxX - sec->centre.vx) < 0x2000 &&
            abs(minX - sec->centre.vx) < 0x2000 &&
            abs(maxY - sec->centre.vy) < 0x2000 &&
            abs(minY - sec->centre.vy) < 0x2000 &&
            abs(maxZ - sec->centre.vz) < 0x2000 &&
            abs(minZ - sec->centre.vz) < 0x2000) {
            dist = SquareRoot0(
                (midX - sec->centre.vx) * (midX - sec->centre.vx) +
                (midY - sec->centre.vy) * (midY - sec->centre.vy) +
                (midZ - sec->centre.vz) * (midZ - sec->centre.vz));
            if (dist > bestDist) {
                bestDist = dist;
                bestSection = j;
            }
        }
    }

    if (bestSection != -1) {
        return &track->sections[bestSection].centre;
    } else {
        return NULL;
    }
}

Skeleton *NewTrackSkeleton(Vector *pos) {
    Skeleton *skel;

    skel = NewSkeleton();
    if (track->skeletonCount >= 200) {
        return NULL;
    }
    track->skeletons[track->skeletonCount++] = skel;
    skel->relative.t[0] = 0;
    skel->relative.t[1] = 0;
    skel->relative.t[2] = 0;
    return skel;
}

s32 CheckSectionInRange(TrackSection *section, Vector *centre) {
    Face *face;
    s32 i;
    s32 minX, maxX;
    s32 minY, maxY;
    s32 minZ, maxZ;

    face = track->faces + section->faceStart;
    maxX = minX = track->vertices[face->vertex[0]].vx;
    maxY = minY = track->vertices[face->vertex[0]].vy;
    maxZ = minZ = track->vertices[face->vertex[0]].vz;

    for (i = 0; i < section->faceCount; i++) {
        minX = MIN(track->vertices[face->vertex[0]].vx, minX);
        minX = MIN(track->vertices[face->vertex[1]].vx, minX);
        minX = MIN(track->vertices[face->vertex[2]].vx, minX);
        minX = MIN(track->vertices[face->vertex[3]].vx, minX);

        maxX = MAX(track->vertices[face->vertex[0]].vx, maxX);
        maxX = MAX(track->vertices[face->vertex[1]].vx, maxX);
        maxX = MAX(track->vertices[face->vertex[2]].vx, maxX);
        maxX = MAX(track->vertices[face->vertex[3]].vx, maxX);

        minY = MIN(track->vertices[face->vertex[0]].vy, minY);
        minY = MIN(track->vertices[face->vertex[1]].vy, minY);
        minY = MIN(track->vertices[face->vertex[2]].vy, minY);
        minY = MIN(track->vertices[face->vertex[3]].vy, minY);

        maxY = MAX(track->vertices[face->vertex[0]].vy, maxY);
        maxY = MAX(track->vertices[face->vertex[1]].vy, maxY);
        maxY = MAX(track->vertices[face->vertex[2]].vy, maxY);
        maxY = MAX(track->vertices[face->vertex[3]].vy, maxY);

        minZ = MIN(track->vertices[face->vertex[0]].vz, minZ);
        minZ = MIN(track->vertices[face->vertex[1]].vz, minZ);
        minZ = MIN(track->vertices[face->vertex[2]].vz, minZ);
        minZ = MIN(track->vertices[face->vertex[3]].vz, minZ);

        maxZ = MAX(track->vertices[face->vertex[0]].vz, maxZ);
        maxZ = MAX(track->vertices[face->vertex[1]].vz, maxZ);
        maxZ = MAX(track->vertices[face->vertex[2]].vz, maxZ);
        maxZ = MAX(track->vertices[face->vertex[3]].vz, maxZ);

        face++;
    }

    if (abs(maxX - centre->vx) < 0x2000 &&
        abs(minX - centre->vx) < 0x2000 &&
        abs(maxY - centre->vy) < 0x2000 &&
        abs(minY - centre->vy) < 0x2000 &&
        abs(maxZ - centre->vz) < 0x2000 &&
        abs(minZ - centre->vz) < 0x2000) {
        return 1;
    }
    return 0;
}

void BuildSectionVertexRuns(TrackSection *section, SectionVertexInfo *info) {
    s32 verts[256];
    Face *face;
    s32 i, j, k;
    s32 count;
    s32 run;
    s32 runIdx;
    s32 runStart;

    info->runs[0] = 0;
    info->runs[1] = 0;
    info->runs[2] = 0;
    info->runs[3] = 0;

    face = track->faces + section->faceStart;
    count = 0;
    runIdx = 0;
    for (i = 0; i < section->faceCount; i++, face++) {
        for (j = 0; j < 4; j++) {
            for (k = 0; k < count; k++) {
                if (face->vertex[j] == verts[k]) {
                    goto already_added;
                }
            }
            verts[count++] = face->vertex[j];
        already_added:;
        }
    }

    SortVertexList(verts, count);

    if (verts[count - 1] - verts[0] == count - 1) {
        info->totalVerts = count;
        info->runs[0] = count;
        info->runs[1] = verts[0];
        return;
    }

    for (k = 0; k < count; k++) {
    }

    info->totalVerts = count;
    run = 0;
    k = 0;
    while (k < count) {
        runStart = verts[k];
        while (verts[k] + 1 == verts[k + 1]) {
            k++;
        }
        info->runs[runIdx++] = (verts[k] - runStart) + 1;
        info->runs[runIdx++] = runStart;
        k++;
        run++;
        if (run >= 3) {
            while (1) {
            }
        }
    }
}

void ClearTrackVtxFlags(void) {
    s32 i;

    for (i = 0; i < track->vertexCount; i++) {
        track->vtxBuf[0][i].v.flag = 0;
        track->vtxBuf[1][i].v.flag = 0;
    }

    for (i = 0; i < track->vtxCount; i++) {
        track->vtxExtra[i].v.flag = 0;
    }
}

void InitVertices(WADFILE *wad) {
    s32 length;
    s32 i, j;
    s32 extraVtxCount;
    Vector *centre;
    Skeleton *skel;

    length = wad->sizes[wad->current];
    track->vertexCount = (u32)length / sizeof(VECTOR);
    track->vertices = (VECTOR *)DAlloc(g_heap, length + 32, "Track.c", TRACK_LINE(677));
    if (!track->vertices) {
        Error("Track.c:InitVertices: Failed to allocate memory for track vertices", Fatal);
    }

    ReadNextWadFile(wad, track->vertices);

    g_trackVtxAllocCount = track->vertexCount + 0x600;
    track->vtxBuf[0] = (Vtx *)DAlloc(g_heap, g_trackVtxAllocCount * sizeof(Vtx), "Track.c", TRACK_LINE(688));
    track->vtxBuf[1] = (Vtx *)DAlloc(g_heap, g_trackVtxAllocCount * sizeof(Vtx), "Track.c", TRACK_LINE(689));

    for (i = 0; i < track->vertexCount; i++) {
        track->vtxBuf[0][i].v.flag = 0xFFFF;
        track->vtxBuf[1][i].v.flag = 0xFFFF;
    }

    for (i = 0; i < track->sectionCount; i++) {
        if (track->sectionSkeletons[i] == NULL) {
            centre = FindSectionSkeletonCentre(&track->sections[i]);
            skel = NewTrackSkeleton(centre);
            for (j = i; j < track->sectionCount; j++) {
                if (track->sectionSkeletons[j] == NULL) {
                    if (CheckSectionInRange(&track->sections[j], centre)) {
                        track->sectionSkeletons[j] = skel;
                    }
                }
            }
        }
    }

    extraVtxCount = 0;
    for (i = 0; i < track->vertexCount; i++) {
        track->vtxBuf[0][i].v.ob[0] = track->vertices[i].vx >> 1;
        track->vtxBuf[0][i].v.ob[1] = track->vertices[i].vy >> 1;
        track->vtxBuf[0][i].v.ob[2] = track->vertices[i].vz >> 1;

        track->vtxBuf[1][i].v.ob[0] = track->vertices[i].vx >> 1;
        track->vtxBuf[1][i].v.ob[1] = track->vertices[i].vy >> 1;
        track->vtxBuf[1][i].v.ob[2] = track->vertices[i].vz >> 1;
    }

    g_sectionVertexInfo = (SectionVertexInfo *)DAlloc(
        g_heap, track->sectionCount * sizeof(SectionVertexInfo), "Track.c", TRACK_LINE(742));
    for (i = 0; i < track->sectionCount; i++) {
        BuildSectionVertexRuns(&track->sections[i], &g_sectionVertexInfo[i]);
    }

    for (i = 0; i < track->skeletonCount; i++) {
        track->skeletons[i]->super = cameras[0].camPos;
    }

    for (i = 0; i < track->faceCount; i++) {
        track->faces[i].red = GammaCorrectColor(track->faces[i].red);
        track->faces[i].green = GammaCorrectColor(track->faces[i].green);
        track->faces[i].blue = GammaCorrectColor(track->faces[i].blue);
    }

    track->vtxCount = extraVtxCount;
    ClearTrackVtxFlags();
}

void InitFaces(WADFILE *wad) {
    u32 length;
    s16 i;

    length = wad->sizes[wad->current];
    track->faces = (Face *)DAlloc(g_heap, length + 32, "Track.c", TRACK_LINE(778));
    if (!track->faces) {
        Error("Track.c:InitFaces: Failed to allocate memory for track faces", Fatal);
    }

    ReadNextWadFile(wad, track->faces);
    track->faceCount = length / sizeof(Face);

    track->colCopy = (Colors *)DAlloc(g_heap, track->faceCount * sizeof(Colors), "Track.c", TRACK_LINE(792));
    if (!track->faces) {
        Error("Track.c:InitFaces: Failed to allocate memory for copy of colors", Fatal);
    }

    for (i = 0; i < track->faceCount; i++) {
        track->colCopy[i].red = track->faces[i].red;
        track->colCopy[i].green = track->faces[i].green;
        track->colCopy[i].blue = track->faces[i].blue;
    }
}

void SortVertexList(s32 *list, s32 count) {
    s32 i, j;
    s32 temp;

    for (i = 0; i < count; i++) {
        for (j = 0; j < count - 1; j++) {
            if (list[j] > list[j + 1]) {
                temp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = temp;
            }
        }
    }
}

void InitSections(WADFILE *wad) {
    u32 length;
    s32 i;
    TrackSection *section;

    length = wad->sizes[wad->current];
    track->sections = (TrackSection *)DAlloc(g_heap, length + 32, "Track.c", TRACK_LINE(1341));
    if (!track->sections) {
        Error("Track.c:InitSections: Failed to allocate memory for track sections", Fatal);
    }

    ReadNextWadFile(wad, track->sections);
    track->sectionCount = length / sizeof(TrackSection);

    track->sectionSkeletons = (Skeleton **)DAlloc(
        g_heap, track->sectionCount * sizeof(Skeleton *), "Track.c", TRACK_LINE(1350));
    for (i = 0; i < track->sectionCount; i++) {
        track->sectionSkeletons[i] = NULL;
    }

    track->skeletonCount = 0;
    section = track->sections;
    for (i = 0; i < track->sectionCount; section++) {
        if (section->flags & TrackPitStart) {
            g_pitStartSections[gameData.actTrackNum] = i;
        }
        i++;
    }

    IndexToPointers();
#ifndef VERSION_EU
    CheckVersion();
#endif
}

void InitTrackDisplayLists(void) {
    s32 maxSections;

    if (frontendOptions_g.numPlayers == 3) {
        maxSections = frontendOptions_g.numPlayers * 40;
    } else if (frontendOptions_g.numPlayers == 4) {
        maxSections = frontendOptions_g.numPlayers * 40;
    } else {
        maxSections = frontendOptions_g.numPlayers * 55;
    }

    g_trackDListSize = maxSections * 58 * sizeof(Gfx);
    g_trackDLists[0] = (Gfx *)DAlloc(g_heap, g_trackDListSize, "Track.c", TRACK_LINE(1403));
    g_trackDLists[1] = (Gfx *)DAlloc(g_heap, g_trackDListSize, "Track.c", TRACK_LINE(1404));
}

void InitViewList(WADFILE *wad) {
    TrackSection *section;
    s32 length;
    s32 total;
    s16 i, j;

    length = wad->sizes[wad->current];
    track->viewList = (s16 *)DAlloc(g_heap, length + 32, "Track.c", TRACK_LINE(1419));
    if (!track->viewList) {
        Error("Track.c:InitViewList: Failed to allocate memory for view list", Fatal);
    }

    ReadNextWadFile(wad, track->viewList);

    section = track->sections;
    total = 0;
    for (i = 0; i < track->sectionCount; i++) {
        for (j = 0; j < 3; j++) {
            section->northViewSection[j] = track->viewList + total;
            total += section->northViewCount[j];
            section->southViewSection[j] = track->viewList + total;
            total += section->southViewCount[j];
            section->eastViewSection[j] = track->viewList + total;
            total += section->eastViewCount[j];
            section->westViewSection[j] = track->viewList + total;
            total += section->westViewCount[j];
            section->allViewSection[j] = track->viewList + total;
            total += section->allViewCount[j];
        }
        section++;
    }

    if (total * 2 != length) {
        Error("track.c::InitViewList():problem with view list", Fatal);
    }
}

TrackSection *NextIndexSection(TrackSection *section) {
    section++;
    if (section > track->sections + track->sectionCount - 1) {
        section = track->sections;
    }
    return section;
}

void DrawTrackSection(TrackSection *section, Skeleton *camPos, s16 count) {
    TrackSection *curSec;
    s32 secLoop;
    s32 secIdx;
    Mtx *mtx;
    s32 firstRunStart;
    s32 ox, oy, oz;
    s32 runIdx;
    s32 vtxDone;
    s32 vtxBase;
    s32 runCount;
    s32 runStart;
    s32 i;
    s32 dst;
    Face *face;
    s32 texIdx;
    s32 vertIdx[4];
    s32 j;
    s32 vtx;
    Gfx *dlStart;
    u8 lastTex;

    curSec = section;
    secIdx = ((u32)curSec - (u32)track->sections) / sizeof(TrackSection);
    dlStart = g_trackDListPtr;

    ox = -camPos->relative.t[0];
    oy = -camPos->relative.t[1];
    oz = -camPos->relative.t[2];
    SetSkeletonPosition(camPos, 0, 0, 0);

    lastTex = 0xFF;
    track->sectionSkeletons[0]->update = 1;
    MatrixToN64Mtx(GetSkeletonMatrix(camPos), (N64Mtx *)&g_mtxPool[g_mtxIndex]);
    {
        s32 mtxIdx = g_mtxIndex;
        g_mtxIndex++;
        mtx = &g_mtxPool[mtxIdx];
    }

    for (secLoop = 0; secLoop < count; secLoop++) {
        runIdx = 0;
        vtxDone = 0;
        vtxBase = 0;
        firstRunStart = g_sectionVertexInfo[secIdx].runs[1];
        while (vtxDone < g_sectionVertexInfo[secIdx].totalVerts) {
            runCount = g_sectionVertexInfo[secIdx].runs[runIdx++];
            runStart = g_sectionVertexInfo[secIdx].runs[runIdx++];
            dst = g_trackVtxCursor;
            for (i = runStart; i < runStart + runCount; i++, dst++) {
                track->vtxBuf[g_trackBufIdx][dst].v.ob[0] = (track->vertices[i].vx >> 1) - ox;
                track->vtxBuf[g_trackBufIdx][dst].v.ob[1] = (track->vertices[i].vy >> 1) - oy;
                track->vtxBuf[g_trackBufIdx][dst].v.ob[2] = (track->vertices[i].vz >> 1) - oz;
            }
            vtxDone += runCount;
            gSPVertex(g_trackDListPtr++,
                      OS_K0_TO_PHYSICAL(&track->vtxBuf[g_trackBufIdx][g_trackVtxCursor]),
                      runCount, vtxBase);
            vtxBase += runCount;
            g_trackVtxCursor += runCount;
        }

        face = track->faces + curSec->faceStart;
        for (i = 0; i < curSec->faceCount; i++, face++) {
            texIdx = face->texture + g_trackRenderList.textureStart;
            if (face->texture != lastTex) {
                gSPDisplayList(g_trackDListPtr++, g_textureTable[texIdx]->dl);
                lastTex = face->texture;
            }

            if (g_textureTable[texIdx] != NULL) {
                for (j = 0; j < 4; j++) {
                    vtx = face->vertex[j];
                    if (vtx < g_sectionVertexInfo[secIdx].runs[3] ||
                        g_sectionVertexInfo[secIdx].runs[2] == 0) {
                        vertIdx[j] = vtx - firstRunStart;
                    } else {
                        vertIdx[j] = (vtx - g_sectionVertexInfo[secIdx].runs[3]) +
                                     g_sectionVertexInfo[secIdx].runs[0];
                    }

                    if (face->flags & FLIP) {
                        g_trackDListPtr = ModifyVertexUV(
                            g_trackDListPtr, vertIdx[j],
                            ((~((j - 1) & 3) & 2) >> 1) * (g_textureTable[texIdx]->width - 1),
                            ((j & 2) >> 1) * (g_textureTable[texIdx]->height - 1),
                            texIdx);
                    } else {
                        g_trackDListPtr = ModifyVertexUV(
                            g_trackDListPtr, vertIdx[j],
                            (((j - 1) & 2) >> 1) * (g_textureTable[texIdx]->width - 1),
                            ((j & 2) >> 1) * (g_textureTable[texIdx]->height - 1),
                            texIdx);
                    }
                }
            }

            if (g_highlightFace != -1 && curSec->faceStart + i == g_highlightFace) {
                gDPSetPrimColor(g_trackDListPtr++, 0, 0, 0xFF, face->green, face->blue, 0xFF);
            } else {
                gDPSetPrimColor(g_trackDListPtr++, 0, 0, face->red, face->green, face->blue, 0xFF);
            }

            gDPPipeSync(g_trackDListPtr++);
            gSP2Triangles(g_trackDListPtr++,
                          vertIdx[0], vertIdx[1], vertIdx[2], 0,
                          vertIdx[2], vertIdx[3], vertIdx[0], 0);
            g_trackTriCount += 2;
        }

        curSec++;
        secIdx++;
    }

    SetSkeletonPosition(camPos, -ox, -oy, -oz);
    gSPEndDisplayList(g_trackDListPtr++);
    AddRenderDisplayList(&g_trackRenderList, &dlStart, mtx);
}

s32 DrawTrackSections(TrackSection *section, Skeleton *camPos, s32 count, s16 dir,
                      s32 angle, TrackCamera *camera, s32 skipJunction) {
    TrackSection *cur;
    TrackSection *juncSec;
    s32 vertIdx[4];
    s32 unused16[4];
    TrackSection *secList[48];
    Gfx *dlStart;
    Mtx *mtx;
    s32 firstRunStart;
    s32 sx, sy, sz;
    u8 lastTex;
    s32 junctionAdjusted;
    s32 hasJunction;
    s32 noSkip;
    s32 i;
    s32 secLoop;
    u8 reverseMain;
    u8 pitHit;
    s32 juncCount;
    u8 juncBack;
    u8 juncFwd;
    Face *face;
    TrackSection *checkSec;
    s32 secIdx;
    s32 runIdx;
    s32 vtxDone;
    s32 vtxBase;
    s32 runCount;
    s32 runStart;
    s32 dst;
    s32 faceIdx;
    s32 texIdx;
    s32 j;
    s32 vtx;

    lastTex = 0xFF;
    juncSec = NULL;
    cur = section;
    junctionAdjusted = 0;

    hasJunction = 0;
    noSkip = (skipJunction == 0);
    if (cur->junction.ptr != NULL && cur->junction.index != -1) {
        hasJunction = 1;
    } else if (cur->prevSection.ptr->junction.ptr != NULL && cur->prevSection.ptr->junction.index != -1) {
        hasJunction = 1;
    } else if (cur->nextSection.ptr->junction.ptr != NULL && cur->nextSection.ptr->junction.index != -1) {
        hasJunction = 1;
    }

    if (noSkip & hasJunction) {
        if (dir == 2) {
            cur = cur->nextSection.ptr;
            cur = cur->nextSection.ptr;
        } else if (dir == 1) {
            cur = cur->prevSection.ptr;
            cur = cur->prevSection.ptr;
        }
        if (frontendOptions_g.numPlayers < 3) {
            count++;
        }
        count++;
        junctionAdjusted = 1;
    }

    dlStart = g_trackDListPtr;
    sx = camPos->relative.t[0];
    sy = camPos->relative.t[1];
    sz = camPos->relative.t[2];
    SetSkeletonPosition(camPos, 0, 0, 0);

    track->sectionSkeletons[0]->update = 1;
    MatrixToN64Mtx(GetSkeletonMatrix(camPos), (N64Mtx *)&g_mtxPool[g_mtxIndex]);
    {
        s32 mtxIdx = g_mtxIndex;
        g_mtxIndex++;
        mtx = &g_mtxPool[mtxIdx];
    }

    juncCount = 0;
    reverseMain = 0;
    pitHit = 0;
    juncBack = 0;
    juncFwd = 0;
    i = 0;
    while (i < count) {
        secList[i] = cur;
        i++;
        if (juncSec == NULL) {
            if (cur->junction.ptr != NULL && cur->junction.index != -1) {
                juncSec = cur->junction.ptr;
                {
                    Face *juncFace = track->faces + section->faceStart;
                    if (juncFace[0].flags & PIT_LANE) {
                        pitHit = 1;
                    } else if (juncFace[1].flags & PIT_LANE) {
                        pitHit = 1;
                    }
                }
                if (!pitHit) {
                    if (juncSec->nextSection.ptr == cur || juncSec->nextSection.ptr == cur->nextSection.ptr) {
                        juncBack = 1;
                    }
                    if (juncSec->prevSection.ptr == cur) {
                        juncFwd = 1;
                    }
                } else {
                    if (dir == 2) {
                        checkSec = juncSec->prevSection.ptr;
                    } else {
                        checkSec = juncSec->nextSection.ptr;
                    }
                    {
                        Face *checkFace = track->faces + checkSec->faceStart;
                        reverseMain = 1;
                        if ((checkFace[0].flags & PIT_LANE) || (checkFace[1].flags & PIT_LANE)) {
                            reverseMain = 0;
                        }
                    }
                }
            }
        }

        if (juncSec != NULL && juncCount < 10 && i < count) {
            secList[i] = juncSec;
            i++;
            juncCount++;
            if (juncBack || (!juncFwd && dir == 2)) {
                juncSec = juncSec->prevSection.ptr;
            } else {
                juncSec = juncSec->nextSection.ptr;
            }
        }

        if (reverseMain) {
            if (dir == 2) {
                cur = cur->nextSection.ptr;
            } else {
                cur = cur->prevSection.ptr;
            }
        } else {
            if (dir == 1) {
                cur = cur->nextSection.ptr;
            } else {
                cur = cur->prevSection.ptr;
            }
        }
    }

    secLoop = count;
    while (secLoop-- > 0) {
        cur = secList[secLoop];
        secIdx = ((u32)cur - (u32)track->sections) / sizeof(TrackSection);
        runIdx = 0;
        vtxDone = 0;
        vtxBase = 0;
        firstRunStart = g_sectionVertexInfo[secIdx].runs[1];
        while (vtxDone < g_sectionVertexInfo[secIdx].totalVerts) {
            runCount = g_sectionVertexInfo[secIdx].runs[runIdx++];
            runStart = g_sectionVertexInfo[secIdx].runs[runIdx++];
            dst = g_trackVtxCursor;
            for (faceIdx = runStart; faceIdx < runStart + runCount; faceIdx++, dst++) {
                track->vtxBuf[g_trackBufIdx][dst].v.ob[0] = (track->vertices[faceIdx].vx >> 1) + sx;
                track->vtxBuf[g_trackBufIdx][dst].v.ob[1] = (track->vertices[faceIdx].vy >> 1) + sy;
                track->vtxBuf[g_trackBufIdx][dst].v.ob[2] = (track->vertices[faceIdx].vz >> 1) + sz;
            }
            vtxDone += runCount;
            gSPVertex(g_trackDListPtr++,
                      OS_K0_TO_PHYSICAL(&track->vtxBuf[g_trackBufIdx][g_trackVtxCursor]),
                      runCount, vtxBase);
            vtxBase += runCount;
            g_trackVtxCursor += runCount;
        }

        face = track->faces + cur->faceStart;
        for (faceIdx = 0; faceIdx < cur->faceCount; faceIdx++, face++) {
            texIdx = face->texture + g_trackRenderList.textureStart;
            if (face->texture != lastTex) {
                gSPDisplayList(g_trackDListPtr++, g_textureTable[texIdx]->dl);
                lastTex = face->texture;
            }

            if (g_textureTable[texIdx] != NULL) {
                for (j = 0; j < 4; j++) {
                    vtx = face->vertex[j];
                    if (vtx < g_sectionVertexInfo[secIdx].runs[3] ||
                        g_sectionVertexInfo[secIdx].runs[2] == 0) {
                        vertIdx[j] = vtx - firstRunStart;
                    } else {
                        vertIdx[j] = (vtx - g_sectionVertexInfo[secIdx].runs[3]) +
                                     g_sectionVertexInfo[secIdx].runs[0];
                    }

                    if (face->flags & FLIP) {
                        g_trackDListPtr = ModifyVertexUV(
                            g_trackDListPtr, vertIdx[j],
                            ((~((j - 1) & 3) & 2) >> 1) * (g_textureTable[texIdx]->width - 1),
                            ((j & 2) >> 1) * (g_textureTable[texIdx]->height - 1),
                            texIdx);
                    } else {
                        g_trackDListPtr = ModifyVertexUV(
                            g_trackDListPtr, vertIdx[j],
                            (((j - 1) & 2) >> 1) * (g_textureTable[texIdx]->width - 1),
                            ((j & 2) >> 1) * (g_textureTable[texIdx]->height - 1),
                            texIdx);
                    }
                }
            }

            if (g_highlightFace != -1 && cur->faceStart + faceIdx == g_highlightFace) {
                gDPSetPrimColor(g_trackDListPtr++, 0, 0, 0xFF, face->green, face->blue, 0xFF);
            } else {
                gDPSetPrimColor(g_trackDListPtr++, 0, 0, face->red, face->green, face->blue, 0xFF);
            }

            gDPPipeSync(g_trackDListPtr++);
            gSP2Triangles(g_trackDListPtr++,
                          vertIdx[0], vertIdx[1], vertIdx[2], 0,
                          vertIdx[2], vertIdx[3], vertIdx[0], 0);
            g_trackTriCount += 2;
        }
    }

    SetSkeletonPosition(camPos, sx, sy, sz);
    gSPEndDisplayList(g_trackDListPtr++);
    AddRenderDisplayList(&g_trackRenderList, &dlStart, mtx);
    return junctionAdjusted;
}

Gfx *DrawNewTrack(TrackCamera *camera, s32 playerIdx, Gfx *glistp) {
    s32 angle;
    s32 drawDist;
    s32 drewBack;
    s32 pauseReq;
    ShipData *ship;
    s16 dir;
    s16 i;
    s16 count;
    s16 loCount = 0;
    s16 medCount = 0;
    s16 hiCount = 0;
    f32 fStep;
    s32 fwd, back;
    s32 used;

    drewBack = 0;

    switch (playerIdx) {
        case 0:
            pauseReq = pauseRequests[0];
            ship = &ships[ownShip];
            break;
        case 1:
            pauseReq = pauseRequests[1];
            ship = &ships[ownShip2];
            break;
        case 2:
            pauseReq = pauseRequests[2];
            ship = &ships[ownShip3];
            break;
        case 3:
            pauseReq = pauseRequests[3];
            ship = &ships[ownShip4];
            break;
    }

    g_camSecIdx = ((u32)camera->section - (u32)track->sections) / sizeof(TrackSection);

    switch (frontendOptions_g.numPlayers) {
        case 1:
            if (frontendOptions_g.trackNo == 7) {
                drawDist = 20;
            } else {
                drawDist = 22;
            }
            break;
        case 2:
            drawDist = 18;
            break;
        case 3:
            drawDist = 14;
#ifndef VERSION_EU
            if (g_playerFinished[playerIdx]) {
                drawDist = 4;
            }
#endif
            break;
        case 4:
#ifdef VERSION_EU
            drawDist = 12;
#else
            drawDist = 10;
            if (g_playerFinished[playerIdx]) {
                drawDist = 4;
            }
#endif
            break;
    }

    g_trackTriCount = 0;
    ResetRenderList(&g_trackRenderList);
    dir = TrackDirection(camera, &angle);

    if (dir == 1) {
        if (frontendOptions_g.numPlayers >= 3) {
            DrawTrackSections(camera->section, camera->camPos, drawDist, 1, angle, camera, 0);
        } else {
            if (frontendOptions_g.trackNo == 5 &&
                ((camera->section->secNo >= 166 && camera->section->secNo <= 190) ||
                 (camera->section->secNo >= 72 && camera->section->secNo <= 91) ||
                 (camera->section->secNo >= 266 && camera->section->secNo <= 310))) {
                DrawTrackSections(camera->section, camera->camPos, 15, 1, angle, camera, 0);
            } else if (frontendOptions_g.trackNo == 5 &&
                       (camera->section->secNo >= 338 && camera->section->secNo <= 350)) {
                DrawTrackSections(camera->section, camera->camPos, 12, 1, angle, camera, 0);
            } else if (frontendOptions_g.trackNo == 5 &&
                       (camera->section->secNo >= 390 && camera->section->secNo <= 405)) {
                DrawTrackSections(camera->section, camera->camPos, 15, 1, angle, camera, 0);
            } else if (frontendOptions_g.trackNo == 4 &&
                       (camera->section->secNo >= 229 && camera->section->secNo <= 250)) {
                DrawTrackSections(camera->section, camera->camPos, 15, dir, angle, camera, 0);
            } else if ((frontendOptions_g.trackNo == 4 &&
                        (camera->section->secNo >= 251 && camera->section->secNo <= 277)) ||
                       (frontendOptions_g.trackNo == 1 &&
                        ((camera->section->secNo >= 200 && camera->section->secNo <= 253) ||
                         (camera->section->secNo >= 99 && camera->section->secNo <= 134)))) {
                DrawTrackSections(camera->section, camera->camPos, 18, dir, angle, camera, 0);
            } else {
                drewBack = DrawTrackSections(camera->section, camera->camPos, drawDist - 2, dir, angle, camera, 0);
            }

            if (!drewBack) {
                DrawTrackSections(camera->section, camera->camPos, 2, 2, angle, camera, 1);
            }
        }

        if ((g_attractMode == 1 || (ship->combatAttr & 0x8000) || pauseReq) && !drewBack) {
            DrawTrackSections(camera->section, camera->camPos, 2, 2, angle, camera, 1);
        }
    } else if (dir == 2) {
#ifdef VERSION_EU
        if (frontendOptions_g.numPlayers < 3 &&
            (CameraUpdate == UpdateCameraRaceStart || CameraUpdate2 == UpdateCameraRaceStart)) {
#else
        if (g_raceStartCamera && frontendOptions_g.numPlayers < 3) {
#endif
            for (i = 0; i < camera->section->southViewCount[0]; i += 2) {
                DrawTrackSection(track->sections + camera->section->southViewSection[0][i],
                                 camera->camPos, camera->section->southViewSection[0][i + 1]);
                hiCount += camera->section->southViewSection[0][i + 1];
            }
            for (i = 0; i < camera->section->southViewCount[1]; i += 2) {
                DrawTrackSection(track->sections + camera->section->southViewSection[1][i],
                                 camera->camPos, camera->section->southViewSection[1][i + 1]);
                medCount += camera->section->southViewSection[1][i + 1];
            }
            for (i = 0; i < camera->section->southViewCount[2]; i += 2) {
                count = camera->section->southViewSection[2][i + 1];
#ifdef VERSION_EU
                if (frontendOptions_g.trackNo == 5) {
                    if (count > 22) {
                        count = 22;
                    }
                } else {
                    if (count > 20) {
                        count = 20;
                    }
                }
#else
                if (count > 100) {
                    count = 100;
                }
#endif
                DrawTrackSection(track->sections + camera->section->southViewSection[2][i],
                                 camera->camPos, count);
                loCount += camera->section->southViewSection[2][i + 1];
            }
        } else {
            if (frontendOptions_g.numPlayers == 4) {
                DrawTrackSections(camera->section, camera->camPos, drawDist, dir, angle, camera, 0);
            } else {
                drewBack = DrawTrackSections(camera->section, camera->camPos, drawDist - 2, dir, angle, camera, 0);
                if (!drewBack) {
                    DrawTrackSections(camera->section, camera->camPos, 2, 1, angle, camera, 1);
                }
            }

            if ((g_attractMode == 1 || (ship->combatAttr & 0x8000) || pauseReq) && !drewBack) {
                DrawTrackSections(camera->section, camera->camPos, 2, 1, angle, camera, 1);
            }
        }
    } else if (dir == 8 || dir == 4) {
        fwd = drawDist;
        fStep = 1000 / fwd;
        if (angle >= 0x200 && angle < 0x600) {
            angle = (s32)((f32)(angle - 0x200) / fStep);
            fwd -= angle;
            back = angle;
        } else {
            angle = (s32)((f32)(-(angle + 0x200)) / fStep);
            fwd -= angle;
            back = angle;
        }
        if (frontendOptions_g.numPlayers == 4) {
            while (fwd < 2) {
                fwd++;
                back--;
            }
            while (back < 2) {
                back++;
                fwd--;
            }
        } else {
            while (fwd < 8) {
                fwd++;
                back--;
            }
            while (back < 8) {
                back++;
                fwd--;
            }
        }

        DrawTrackSections(camera->section, camera->camPos, fwd, 1, angle, camera, back < fwd);
        DrawTrackSections(camera->section, camera->camPos, back, 2, angle, camera, back < fwd);
    }

    used = (s32)g_trackDListPtr - (s32)g_trackDLists[g_trackBufIdx];
    g_trackDListUsed = used;
    g_trackDListPct = (used * 100) / g_trackDListSize;
    return FlushTrackRenderList(&g_trackRenderList, glistp);
}

Gfx *FlushTrackRenderList(RenderList *list, Gfx *glistp) {
    u32 i;
    RenderNode *node;
    Mtx *lastMtx;
    s32 shaded;

    lastMtx = NULL;
    shaded = 0;

    node = &list->buckets[0];
    while (node->next != NULL) {
        node = node->next;
        if ((s32)node->mtx > 0) {
            node->mtx = (Mtx *)(-(s32)node->mtx);
            gDPPipeSync(glistp++);
            gDPSetCombineMode(glistp++, G_CC_SHADE, G_CC_SHADE);
            shaded = 1;
        }
        if (node->mtx != lastMtx) {
            gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(node->mtx),
                      G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
            lastMtx = node->mtx;
        }
        gDma0p(glistp++, G_SPNOOP, list, i);
        gSPDisplayList(glistp++, node->dl);
        if (shaded) {
            shaded = 0;
            gDPPipeSync(glistp++);
            gDPSetCombineMode(glistp++, G_CC_MODULATEI_PRIM, G_CC_MODULATEI_PRIM);
        }
    }

    return glistp;
}

void GetFaceVertex(TrackSection *section, Face *face, s32 vertex, Vector *out) {
    out->vx = track->vertices[face->vertex[vertex]].vx;
    out->vy = track->vertices[face->vertex[vertex]].vy;
    out->vz = track->vertices[face->vertex[vertex]].vz;
}

void RGBToHSV(Colors *rgb, Colors *hsv) {
    f32 g = rgb->green;
    f32 r = rgb->red;
    f32 b = rgb->blue;
    f32 maxVal;
    f32 minVal;
    f32 delta;
    f32 rc, gc, bc;
    f32 h, s, v;

    maxVal = MAX(r, g);
    maxVal = (maxVal < b) ? b : maxVal;
    minVal = (r < maxVal) ? b : maxVal;

    v = maxVal;
    delta = maxVal - minVal;
    if (maxVal != 0.0f) {
        s = delta / maxVal;
    } else {
        s = 0.0f;
    }

    if (s == 0.0) {
        h = -1.0f;
    } else {
        rc = (maxVal - r) / delta;
        gc = (maxVal - g) / delta;
        bc = (maxVal - b) / delta;
        if (r == maxVal) {
            h = bc - gc;
        } else if (g == maxVal) {
            h = 2.0 + rc - bc;
        } else if (b == maxVal) {
            h = 4.0 + gc - rc;
        }
        h *= 60.0;
        if (h < 0.0) {
            h += 360.0;
        }
    }

    hsv->red = (u8)h;
    hsv->green = (u8)s;
    hsv->blue = (u8)v;
}
