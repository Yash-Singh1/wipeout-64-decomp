/*
 * n64render.c — Texture-bucketed F3DEX display-list sorter and flusher.
 *
 * Groups geometry display lists by texture index to minimize TMEM texture
 * loads, tracks modelview matrix changes, and supports negated-matrix-pointer
 * combine-mode overrides for shaded/untextured passes.
 */
#include "n64render.h"

void ResetRenderList(RenderList *list) {
    u32 i;

    for (i = 0; i < list->bucketCount; i++) {
        list->buckets[i].next = NULL;
        list->buckets[i].mtx = NULL;
        list->buckets[i].dl = g_textureTable[i + list->textureStart]->dl;
    }

    list->untextured.next = NULL;
    list->untextured.mtx = NULL;
    g_renderNodePtr = g_renderNodePool;
}

void InitRenderList(RenderList *list, u32 textureStart, u32 bucketCount) {
    Gfx *dl;

    list->textureStart = textureStart;
    list->bucketCount = bucketCount;
    list->buckets = (RenderNode *)DAlloc(g_heap, bucketCount * sizeof(RenderNode), "n64render.c", 37);
    list->untextured.next = NULL;
    list->untextured.mtx = NULL;
    dl = (Gfx *)DAlloc(g_heap, RENDER_UNTEXT_DL_LEN * sizeof(Gfx), "n64render.c", 41);
    list->untextured.dl = dl;
    gSPTexture(dl, 0, 0, 0, G_TX_RENDERTILE, G_OFF);
    gSPEndDisplayList(dl + 1);
    ResetRenderList(list);
}

Gfx *FlushRenderList(RenderList *list, Gfx *glistp) {
    u32 i;
    RenderNode *node;
    Mtx *lastMtx;
    s32 shaded;

    lastMtx = NULL;
    shaded = 0;

    for (i = 0; i < list->bucketCount; i++) {
        node = &list->buckets[i];
        if (node->next != NULL) {
            if (node->dl != NULL) {
                gSPDisplayList(glistp++, node->dl);
            }
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
        }
    }

    node = &list->untextured;
    if (list->untextured.next != NULL) {
        if (node->dl != NULL) {
            gSPDisplayList(glistp++, node->dl);
        }
    }
    while (node->next != NULL) {
        node = node->next;
        if ((s32)node->mtx > 0) {
            node->mtx = (Mtx *)(-(s32)node->mtx);
            gDPPipeSync(glistp++);
            gDPSetCombineMode(glistp++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
            shaded = 1;
        }
        if (node->mtx != lastMtx) {
            gSPMatrix(glistp++, OS_K0_TO_PHYSICAL(node->mtx),
                      G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
            lastMtx = node->mtx;
        }
        gDma0p(glistp++, G_SPNOOP, list, i | 0x800000);
        gSPDisplayList(glistp++, node->dl);
        if (shaded) {
            shaded = 0;
            gDPPipeSync(glistp++);
            gDPSetCombineMode(glistp++, G_CC_MODULATEI_PRIM, G_CC_MODULATEI_PRIM);
        }
    }

    return glistp;
}

void AddRenderGroup(RenderList *list, RenderGroup *group, Mtx *mtx) {
    u32 i;
    RenderGroupEntry *entry;

    entry = group->entries;
    for (i = 0; i < group->count; i++) {
        if (entry->textureIndex == -1) {
            g_renderNodePtr->next = list->untextured.next;
            list->untextured.next = g_renderNodePtr;
            g_renderNodePtr->mtx = mtx;
            g_renderNodePtr->dl = entry->dl;
            g_renderNodePtr++;
            entry++;
        } else if (entry->textureIndex & RENDER_TEX_SKIP_FLAG) {
            entry++;
        } else {
            g_renderNodePtr->next = list->buckets[entry->textureIndex].next;
            list->buckets[entry->textureIndex].next = g_renderNodePtr;
            g_renderNodePtr->mtx = mtx;
            g_renderNodePtr->dl = entry->dl;
            g_renderNodePtr++;
            entry++;
        }
    }
}

void AddRenderDisplayList(RenderList *list, Gfx **dlPtr, Mtx *mtx) {
    g_renderNodePtr->next = list->buckets[0].next;
    list->buckets[0].next = g_renderNodePtr;
    g_renderNodePtr->mtx = mtx;
    g_renderNodePtr->dl = *dlPtr;
    g_renderNodePtr++;
}
