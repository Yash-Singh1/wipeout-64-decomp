/*
 * loader.c — Loading screen S2DEX 2D task and Rumble Pak insertion prompt.
 *
 * Renders the F5000 Anti-Gravity Racing League loading screen, prompts for
 * Rumble Pak insertion when not detected, and drives the 3D background task
 * during loads (GLIST_LEN 0x880 USA vs 0x900 Europe).
 */
#include "loader.h"

#ident "$Revision: 1.17 $"
#ident "$Revision: 7.27 $"
#ident "$Revision: 1.5 $"

void CreateLoader2DTask(Gfx *glistStart, GFXInfo *i) {
    OSScTask *t;
    Gfx *glist2D;

    t = &i->task2D;
    i->flags &= ~1;
    t->list = g_s2dexTaskTemplate;

    glist2D = glistStart;
    gSPSegment(glist2D++, 0, 0x0);
    gSPSegment(glist2D++, STATIC_SEGMENT, OS_K0_TO_PHYSICAL(g_staticSegment));
    gSPSegment(glist2D++, CFB_SEGMENT, OS_K0_TO_PHYSICAL(i->cfb));
    gSPDisplayList(glist2D++, g_rdpInit2DDL);
    gSPDisplayList(glist2D++, g_rspInit2DDL);
    gSPDisplayList(glist2D++, g_dl2DInit);
    gDPPipeSync(glist2D++);
    gDPSetCombineMode(glist2D++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetPrimColor(glist2D++, 0, 0, 255, 255, 255, 255);
    gSPSprite2DBase(glist2D++, OS_K0_TO_PHYSICAL(&g_loaderSprite));
    gSPSprite2DScaleFlip(glist2D++, 1 << 10, 1 << 10, 0, 0);
    gSPSprite2DDraw(glist2D++, 140 << 2, 70 << 2);

    glist2D = DrawHudText("F5000", 120, 130, 12, 1, glist2D);
    glist2D = DrawFontText("ANTI-GRAVITY", 120, 144, 12, glist2D);
    glist2D = DrawFontText("RACING LEAGUE", 120, 152, 12, glist2D);

    if (g_gameState == 1 && g_rumblePakFound != 1 && g_loadDone) {
        glist2D = DrawFontTextYellow("THIS GAME IS RUMBLE PAK COMPATIBLE.", 70, 180, 8, glist2D);
        glist2D = DrawFontTextYellow("PLEASE INSERT YOUR RUMBLE PAK NOW", 70, 187, 8, glist2D);
        glist2D = DrawFontTextYellow("IF YOU WISH TO PLAY WITH IT.", 70, 195, 8, glist2D);
        glist2D = DrawFontTextButton(125, 210, 12, "CONTINUE", glist2D);
    } else {
        glist2D = DrawHudTextCentered("PLEASE WAIT", 160, 183, 12, 1, glist2D);
    }

    gDPFullSync(glist2D++);
    gSPEndDisplayList(glist2D++);

    if (!(((u32)glist2D - (u32)&i->dp) < (GLIST_LEN * sizeof(Gfx)))) {
        __assert("((u32)glist2D-(u32)&i->dp) < (GLIST_LEN * sizeof(Gfx))", "loader.c", 145);
    }

    osWritebackDCache(glistStart, (u32)glist2D - (u32)glistStart);

    t->list.t.ucode_boot = (u64 *)rspbootTextStart;
    t->list.t.ucode_boot_size = (u32)rspbootTextEnd - (u32)rspbootTextStart;
    t->list.t.ucode = (u64 *)gspS2DEX_fifoTextStart;
    t->list.t.ucode_data = (u64 *)gspS2DEX_fifoDataStart;
    t->list.t.output_buff = g_rdpOutputBuf;
    t->list.t.output_buff_size = (u64 *)((u32)g_rdpOutputBuf + RDP_OUTPUT_LEN);
    t->list.t.yield_data_ptr = g_gfxYieldBuf;
    t->list.t.yield_data_size = OS_YIELD_DATA_SIZE;
    t->list.t.flags = 0;
    t->list.t.data_ptr = (u64 *)glistStart;
    t->list.t.data_size = ((u32)glist2D - (u32)glistStart) * sizeof(Gfx);
    t->list.t.dram_stack = g_dramStack;
    t->list.t.dram_stack_size = SP_DRAM_STACK_SIZE8;

    t->next = NULL;
    t->flags = OS_SC_NEEDS_RSP | OS_SC_NEEDS_RDP | OS_SC_LAST_TASK | OS_SC_SWAPBUFFER;
    t->msgQ = &g_schedMsgQ;
    t->msg = (OSMesg)&i->doneMsg;
    t->framebuffer = i->cfb;
    t->totalTime = 0;
}

void CreateLoaderGfxTask(s32 frameIdx) {
    GFXInfo *i;
    Gfx *glistp;
    Gfx *taskStart;
    u16 perspNorm;
    s32 delay;

    i = &g_gfxInfo[frameIdx];
    glistp = i->dp.glist;
    i->active = 1;

    {
        Gfx *start = glistp;
        gDPFullSync(glistp++);
        gSPEndDisplayList(glistp++);
        SetupGfxTask(i, 6, start, glistp);
    }

    taskStart = glistp;
    g_mtxCount = 0;
    g_mtxPool = i->dp.mtxPool;

    gSPSegment(glistp++, 0, 0x0);
    gSPSegment(glistp++, STATIC_SEGMENT, OS_K0_TO_PHYSICAL(g_staticSegment));
    gSPSegment(glistp++, CFB_SEGMENT, OS_K0_TO_PHYSICAL(i->cfb));

    guPerspective(&i->dp.projection, &perspNorm, 82.224f, 4.0f / 3.0f, 16.0f, (f32)g_farClip, 1.0f);
    guMtxCatL(&i->dp.projection, &g_flipYMtx, &i->dp.projection);

    gSPDisplayList(glistp++, g_rdpInitDL);
    gSPViewport(glistp++, &g_vpFullScreen);
    gSPDisplayList(glistp++, g_rspInitDL);
    gSPDisplayList(glistp++, g_clearZBufferDL);
    gSPDisplayList(glistp++, g_clearCfbDL);
    gDPFullSync(glistp++);
    gSPEndDisplayList(glistp++);
    SetupGfxTask(i, 4, taskStart, glistp);

    guOrtho(&i->dp.ortho, -160.0f, 160.0f, -120.0f, 120.0f, 0.0f, 4000.0f, 1.0f);
    guMtxCatL(&i->dp.ortho, &g_flipYMtx, &i->dp.ortho);

    {
        Gfx *start = glistp;
        gDPFullSync(glistp++);
        gSPEndDisplayList(glistp++);
        SetupGfxTask(i, 3, start, glistp);
    }

    {
        Gfx *start = glistp;
        gDPFullSync(glistp++);
        gSPEndDisplayList(glistp++);
        SetupGfxTask(i, 1, start, glistp);
    }

    {
        Gfx *start = glistp;
        gDPFullSync(glistp++);
        gSPEndDisplayList(glistp++);
        SetupGfxTask(i, 0, start, glistp);
    }

    {
        Gfx *start = glistp;
        gDPFullSync(glistp++);
        gSPEndDisplayList(glistp++);
        SetupGfxTask(i, 2, start, glistp);
    }

    {
        Gfx *start = glistp;
        gDPFullSync(glistp++);
        gSPEndDisplayList(glistp++);
        SetupGfxTask(i, 5, start, glistp);
    }

    gDPFullSync(glistp++);
    gSPEndDisplayList(glistp++);

    i->glist2D = glistp;
    CreateLoader2DTask(glistp, i);

    if (g_frameDelayFlag) {
        for (delay = 0; delay < 416; delay++) {
        }
    }

    osWritebackDCache(&i->dp, (u32)glistp - (u32)&i->dp);
}
