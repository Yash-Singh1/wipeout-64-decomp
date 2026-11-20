#include "gfx2d.h"

static const char rcsid[] = "$Revision: 1.17 $";

Gfx *DrawSplitScreenBorders(Gfx *glist) {
    gSPTexture(glist++, 0, 0, 0, G_TX_RENDERTILE, G_OFF);
    gDPSetCombineMode(glist++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
    gDPSetPrimColor(glist++, 0, 0, 0, 0, 0, 255);
    gDPPipeSync(glist++);

    switch (frontendOptions_g.numPlayers) {
        case 2:
            if (frontendOptions_g.screenLayout[0] == 1) {
                gSPTextureRectangle(glist++, 160 << 2, 6 << 2, 161 << 2, 220 << 2, G_TX_RENDERTILE, 0, 0, 0, 0);
            } else {
                gSPTextureRectangle(glist++, 0 << 2, 118 << 2, 320 << 2, 121 << 2, G_TX_RENDERTILE, 0, 0, 0, 0);
            }
            break;
        case 3:
        case 4:
            gSPTextureRectangle(glist++, 160 << 2, 18 << 2, 161 << 2, 222 << 2, G_TX_RENDERTILE, 0, 0, 0, 0);
            gSPTextureRectangle(glist++, 0 << 2, 120 << 2, 320 << 2, 121 << 2, G_TX_RENDERTILE, 0, 0, 0, 0);
            break;
    }

    gDPPipeSync(glist++);
    gSPTexture(glist++, 0, 0, 0, G_TX_RENDERTILE, G_ON);
    gDPSetCombineMode(glist++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    return glist;
}

void Gfx2DRaceTask(Gfx *glistp, GFXInfo *i) {
    OSScTask *t;
    Gfx *glist2D;
    s32 player;
    s32 size;

    t = &i->task[0];
    i->flags &= ~1;
    t->list = g_gfxTaskTemplate;

    glist2D = glistp;
    gSPSegment(glist2D++, 0, 0);
    gSPSegment(glist2D++, 1, OS_K0_TO_PHYSICAL(g_staticSegment));
    gSPSegment(glist2D++, 2, OS_K0_TO_PHYSICAL(i->cfb));
    gSPDisplayList(glist2D++, g_rdpInitDL);
    gSPDisplayList(glist2D++, g_rspInitDL);
    gSPDisplayList(glist2D++, g_globalDL);
    glist2D = DrawSplitScreenBorders(glist2D);

    for (player = 1; player <= frontendOptions_g.numPlayers; player++) {
        if (ships[frontendOptions_g.ownShip[player - 1]].attr & 0x40) {
            glist2D = DrawShipEffect(g_shipEffectA, g_shipEffectB, &ships[frontendOptions_g.ownShip[player - 1]],
                                     frontendOptions_g.screenLayout[player - 1], glist2D);
        }
        glist2D = DrawPlayerHud(ships, g_hudTimers, g_hudLaps, g_raceOverlayMode, &g_raceOverlay,
                                g_hudState, player, frontendOptions_g.screenLayout[player - 1], glist2D);
    }

    if (frontendOptions_g.numPlayers == 3) {
        gDPPipeSync(glist2D++);
        gDPSetCombineMode(glist2D++, G_CC_DECALRGBA, G_CC_DECALRGBA);
        gDPSetPrimColor(glist2D++, 0, 0, 255, 255, 255, 255);
        gSPSprite2DBase(glist2D++, OS_K0_TO_PHYSICAL(&g_logoSprite));
        gSPSprite2DScaleFlip(glist2D++, 1 << 10, 1 << 10, 0, 0);
        gSPSprite2DDraw(glist2D++, 210 << 2, 130 << 2);
        glist2D = DrawLargeText("F5000", 190, 190, 12, 1, glist2D);
        glist2D = DrawText("ANTI-GRAVITY", 190, 204, 12, glist2D);
        glist2D = DrawText("RACING LEAGUE", 190, 212, 12, glist2D);
    }

    if (g_raceOverlay.unk12 || g_raceOverlay.unk14 || g_raceOverlay.unk18 || g_hudShowPause) {
        glist2D = DrawRaceOverlay(glist2D);
    }
    glist2D = DrawRaceMessages(glist2D);

    for (player = 0; player < frontendOptions_g.numPlayers; player++) {
        if (g_playerMessage[player]) {
            glist2D = DrawWeaponMessage(player, 0, glist2D, 0);
        }
    }

    gDPFullSync(glist2D++);
    gSPEndDisplayList(glist2D++);

    if (!(((u32)glist2D-(u32)&i->dp) < (GLIST_LEN * sizeof(Gfx)))) {
        __assert("((u32)glist2D-(u32)&i->dp) < (GLIST_LEN * sizeof(Gfx))", "gfx2d.c", 332);
    }

    size = (u32)glist2D - (u32)glistp;
    osWritebackDCache(glistp, size);

    t->list.t.ucode_boot = rspbootTextStart;
    t->list.t.flags = 0;
    t->list.t.data_ptr = (u64 *)glistp;
    t->list.t.ucode_boot_size = (u32)rspbootTextEnd - (u32)rspbootTextStart;
    t->list.t.ucode = gspS2DEX_fifoTextStart;
    t->list.t.ucode_data = gspS2DEX_fifoDataStart;
    t->list.t.output_buff = g_rdpFifo;
    t->list.t.output_buff_size = g_rdpFifo + RDP_FIFO_LEN;
    t->list.t.yield_data_ptr = g_yieldData;
    t->list.t.yield_data_size = YIELD_DATA_SIZE;
    t->list.t.data_size = size * sizeof(Gfx);
    t->list.t.dram_stack = g_dramStack;
    t->list.t.dram_stack_size = SP_DRAM_STACK_SIZE8;
    t->flags = OS_SC_NEEDS_RSP | OS_SC_NEEDS_RDP | OS_SC_LAST_TASK | OS_SC_SWAPBUFFER;
    t->msgQ = &g_schedMsgQ;
    t->next = NULL;
    t->msg = (OSMesg)&i->msg;
    t->framebuffer = i->cfb;
    t->totalTime = 0;
}

void Gfx2DMenuTask(Gfx *glistp, GFXInfo *i, s32 plasma) {
    OSScTask *t;
    Gfx *glist2D;
    s32 size;

    if (plasma == 1) {
        i->flags |= 1;
        t = &i->task[1];
    } else {
        t = &i->task[0];
    }
    t->list = g_gfxTaskTemplate;

    glist2D = glistp;
    gSPSegment(glist2D++, 0, 0);
    gSPSegment(glist2D++, 1, OS_K0_TO_PHYSICAL(g_staticSegment));
    gSPSegment(glist2D++, 2, OS_K0_TO_PHYSICAL(i->cfb));
    gSPDisplayList(glist2D++, g_rdpInitDL);
    gSPDisplayList(glist2D++, g_rspInitDL);
    gSPDisplayList(glist2D++, g_globalDL);

    if (plasma == 1) {
        glist2D = RenderPlasma(glist2D);
    } else {
        glist2D = RenderFrontend(glist2D);
    }

    gDPFullSync(glist2D++);
    gSPEndDisplayList(glist2D++);

    if (!(((u32)glist2D-(u32)&i->dp) < (GLIST_LEN * sizeof(Gfx)))) {
        __assert("((u32)glist2D-(u32)&i->dp) < (GLIST_LEN * sizeof(Gfx))", "gfx2d.c", 432);
    }

    size = (u32)glist2D - (u32)glistp;
    osWritebackDCache(glistp, size);

    t->list.t.ucode_boot = rspbootTextStart;
    t->list.t.flags = 0;
    t->list.t.data_ptr = (u64 *)glistp;
    t->list.t.ucode_boot_size = (u32)rspbootTextEnd - (u32)rspbootTextStart;
    t->list.t.ucode = gspS2DEX_fifoTextStart;
    t->list.t.ucode_data = gspS2DEX_fifoDataStart;
    t->list.t.output_buff = g_rdpFifo;
    t->list.t.output_buff_size = g_rdpFifo + RDP_FIFO_LEN;
    t->list.t.yield_data_ptr = g_yieldData;
    t->list.t.yield_data_size = YIELD_DATA_SIZE;
    t->list.t.data_size = size * sizeof(Gfx);
    t->list.t.dram_stack = g_dramStack;
    t->list.t.dram_stack_size = SP_DRAM_STACK_SIZE8;
    t->flags = OS_SC_NEEDS_RSP | OS_SC_NEEDS_RDP | OS_SC_LAST_TASK | OS_SC_SWAPBUFFER;
    t->msgQ = &g_schedMsgQ;
    t->next = NULL;
    t->msg = (OSMesg)&i->msg;
    t->framebuffer = i->cfb;
    t->totalTime = 0;
}

void Gfx2DInitDisplayList(Gfx *glistp, s32 mode) {
    Gfx *gp;

    guOrtho(&g_orthoMtx, -160.0f, 160.0f, -120.0f, 120.0f, 0.0f, 4000.0f, 1.0f);
    gp = glistp;
    gSPMatrix(gp++, OS_K0_TO_PHYSICAL(&g_orthoMtx), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    guRotate(&g_identityMtx, 0.0f, 1.0f, 1.0f, 1.0f);
    gSPMatrix(gp++, OS_K0_TO_PHYSICAL(&g_identityMtx), G_MTX_MODELVIEW | G_MTX_LOAD | G_MTX_NOPUSH);
    gDPPipeSync(gp++);

    if (mode == 4) {
        gDPSetCycleType(gp++, G_CYC_1CYCLE);
        gDPSetDepthImage(gp++, OS_K0_TO_PHYSICAL(g_zBuffer));
        gDPSetDepthSource(gp++, G_ZS_PRIM);
        gDPSetRenderMode(gp++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
        gSPClearGeometryMode(gp++, G_SHADE | G_SHADING_SMOOTH);
        gDPSetTextureFilter(gp++, G_TF_POINT);
        gDPSetCombineMode(gp++, G_CC_DECALRGBA, G_CC_DECALRGBA);
        gDPSetColorDither(gp++, G_CD_DISABLE);
        gDPPipeSync(gp++);
    }

    gSPEndDisplayList(gp++);
    osWritebackDCache(glistp, (u32)gp - (u32)glistp);
}
