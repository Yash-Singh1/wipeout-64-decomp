#include "gfx.h"

static const char rcsid[] = "$Revision: 1.17 $";

void InitGfx(void) {
    u32 len;

    len = (u32)(_staticSegmentRomEnd - _staticSegmentRomStart);
    g_staticSegment = DAlloc(g_heap, len, "gfx.c", 90);
    DmaRead((u32)_staticSegmentRomStart, g_staticSegment, len);

    g_gfxInfo[0].taskMsg[0].type = SC_MSG_TASK0_DONE;
    g_gfxInfo[1].taskMsg[0].type = SC_MSG_TASK0_DONE;
    g_gfxInfo[0].taskMsg[5].type = SC_MSG_TASK5_DONE;
    g_gfxInfo[1].taskMsg[5].type = SC_MSG_TASK5_DONE;
    g_gfxInfo[0].taskMsg[2].type = SC_MSG_TASK2_DONE;
    g_gfxInfo[1].taskMsg[2].type = SC_MSG_TASK2_DONE;
    g_gfxInfo[0].taskMsg[3].type = SC_MSG_TASK3_DONE;
    g_gfxInfo[1].taskMsg[3].type = SC_MSG_TASK3_DONE;
    g_gfxInfo[0].taskMsg[1].type = SC_MSG_TASK1_DONE;
    g_gfxInfo[1].taskMsg[1].type = SC_MSG_TASK1_DONE;
    g_gfxInfo[0].taskMsg[4].type = SC_MSG_TASK4_DONE;
    g_gfxInfo[1].taskMsg[4].type = SC_MSG_TASK4_DONE;
    g_gfxInfo[0].taskMsg[6].type = SC_MSG_TASK6_DONE;
    g_gfxInfo[1].taskMsg[6].type = SC_MSG_TASK6_DONE;

    g_gfxInfo[0].doneMsg.type = OS_SC_DONE_MSG;
    g_gfxInfo[1].doneMsg.type = OS_SC_DONE_MSG;

    g_gfxInfo[0].cfb = g_cfb0;
    g_gfxInfo[1].cfb = g_cfb1;

    BuildInitDisplayList(g_initDls[0], 0);
    BuildInitDisplayList(g_initDls[1], 1);
    BuildInitDisplayList(g_initDls[2], 2);
    BuildInitDisplayList(g_initDls[3], 3);
    BuildInitDisplayList(g_initDls[4], 4);
    BuildInitDisplayList(g_initDls[5], 5);
}

void SubmitGfxTasks(s32 bufferIdx) {
    GFXInfo *info;

    g_gfxSubmitCount++;
    info = &g_gfxInfo[bufferIdx];
    g_lastFrameTime = osGetTime();
    osSendMesg(g_schedCmdQ, (OSMesg)&info->task[6], OS_MESG_BLOCK);
    if (info->mode == 1) {
        osSendMesg(g_schedCmdQ, (OSMesg)&info->task[4], OS_MESG_BLOCK);
        osSendMesg(g_schedCmdQ, (OSMesg)&info->task[1], OS_MESG_BLOCK);
        osSendMesg(g_schedCmdQ, (OSMesg)&info->task[0], OS_MESG_BLOCK);
        osSendMesg(g_schedCmdQ, (OSMesg)&info->task[2], OS_MESG_BLOCK);
        osSendMesg(g_schedCmdQ, (OSMesg)&info->task[5], OS_MESG_BLOCK);
        osSendMesg(g_schedCmdQ, (OSMesg)&info->task[7], OS_MESG_BLOCK);
        if (info->flags & 1) {
            osSendMesg(g_schedCmdQ, (OSMesg)&info->task[8], OS_MESG_BLOCK);
        }
    }
    g_gfxFrameCount++;
}

void BuildInitDisplayList(Gfx *dl, s32 type) {
    Gfx *glistp;

    glistp = dl;
    switch (type) {
    case 0:
        gDPPipeSync(glistp++);
        gDPSetDepthImage(glistp++, OS_K0_TO_PHYSICAL(g_zBuffer));
        gDPSetCycleType(glistp++, G_CYC_1CYCLE);
        gDPSetRenderMode(glistp++, G_RM_ZB_OPA_SURF, G_RM_ZB_OPA_SURF2);
        gDPSetDepthSource(glistp++, G_ZS_PIXEL);
        gDPSetTexturePersp(glistp++, G_TP_PERSP);
        gSPSetGeometryMode(glistp++, G_ZBUFFER | G_SHADING_SMOOTH | G_CULL_FRONT);
        gDPSetTextureFilter(glistp++, G_TF_BILERP);
        gDPSetCombineMode(glistp++, G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA);
        gDPSetColorDither(glistp++, G_CD_MAGICSQ);
        break;
    case 1:
        gDPPipeSync(glistp++);
        gDPSetDepthImage(glistp++, OS_K0_TO_PHYSICAL(g_zBuffer));
        gDPSetCycleType(glistp++, G_CYC_1CYCLE);
#ifdef VERSION_EU
        gDPSetRenderMode(glistp++, G_RM_ZB_OPA_SURF, G_RM_ZB_OPA_SURF2);
#else
        gDPSetRenderMode(glistp++, (G_RM_ZB_OPA_SURF) & ~Z_CMP, (G_RM_ZB_OPA_SURF2) & ~Z_CMP);
#endif
        gDPSetTexturePersp(glistp++, G_TP_PERSP);
        gDPSetDepthSource(glistp++, G_ZS_PIXEL);
        gSPSetGeometryMode(glistp++, G_ZBUFFER | G_CULL_BACK | G_CLIPPING);
        gDPSetTextureFilter(glistp++, G_TF_BILERP);
        gDPSetCombineMode(glistp++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEI_PRIM);
        gDPSetColorDither(glistp++, G_CD_MAGICSQ);
        break;
    case 2:
        gDPPipeSync(glistp++);
        gDPSetDepthImage(glistp++, OS_K0_TO_PHYSICAL(g_zBuffer));
        gDPSetCycleType(glistp++, G_CYC_1CYCLE);
        gDPSetRenderMode(glistp++, G_RM_TEX_EDGE | Z_CMP | Z_UPD, G_RM_TEX_EDGE2 | Z_CMP | Z_UPD);
        gDPSetDepthSource(glistp++, G_ZS_PIXEL);
        gDPSetTexturePersp(glistp++, G_TP_PERSP);
        gSPClearGeometryMode(glistp++, G_CULL_BACK);
        gSPSetGeometryMode(glistp++, G_ZBUFFER | G_SHADING_SMOOTH | G_CULL_FRONT | G_CLIPPING);
        gDPSetTextureFilter(glistp++, G_TF_BILERP);
        gDPSetCombineMode(glistp++, G_CC_MODULATEIDECALA, G_CC_MODULATEIDECALA);
        gDPSetColorDither(glistp++, G_CD_MAGICSQ);
        break;
    case 3:
        gDPPipeSync(glistp++);
        gDPSetCycleType(glistp++, G_CYC_1CYCLE);
        gDPSetRenderMode(glistp++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
        gSPClearGeometryMode(glistp++, G_SHADING_SMOOTH);
        gSPSetGeometryMode(glistp++, G_CLIPPING);
        gDPSetTextureFilter(glistp++, G_TF_BILERP);
        gDPSetColorDither(glistp++, G_CD_MAGICSQ);
        break;
    case 5:
        gDPPipeSync(glistp++);
        gDPSetCycleType(glistp++, G_CYC_1CYCLE);
        gDPSetRenderMode(glistp++, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF2);
        gSPSetGeometryMode(glistp++, G_ZBUFFER | G_SHADING_SMOOTH | G_CLIPPING);
        gDPSetTextureFilter(glistp++, G_TF_BILERP);
        gDPSetCombineMode(glistp++, G_CC_SHADE, G_CC_SHADE);
        gDPSetColorDither(glistp++, G_CD_MAGICSQ);
        gSPTexture(glistp++, 0, 0, 0, 0, G_OFF);
        break;
    default:
        BuildGfx2DInitDisplayList(dl, type);
        return;
    }
    gSPEndDisplayList(glistp++);
    osWritebackDCache(dl, (s32)glistp - (s32)dl);
}
