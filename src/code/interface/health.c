/**
 * @file interface/health.c
 * @author zel.
 * @brief Functions for the health drawer in the interface
 * @date 2021-12-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "global.h"
#include "custom/textures/interface_static/interface_static.h"

#define HEARTS_PER_ROW 10

s32 Health_GetRows() {
    return ((s32)((f32)((gSaveContext.healthCapacity / 0x10) - 1) / (f32)HEARTS_PER_ROW)) + 1;
}

u32 Health_IsCritical(void) {
    s32 criticalHearts;

    if (gSaveContext.healthCapacity <= 5 * 0x10) {
        criticalHearts = 1 * 0x10;
    } else if (gSaveContext.healthCapacity <= 10 * 0x10) {
        criticalHearts = 8 * 0x10;
    } else if (gSaveContext.healthCapacity <= 15 * 0x10) {
        criticalHearts = 2 * 0x10;
    } else {
        criticalHearts = 0x2C;
    }

    if ((criticalHearts >= gSaveContext.health) && (gSaveContext.health > 0)) {
        return true;
    } else {
        return false;
    }
}

static f32 sHeartBeatScale;

/*static*/ void Interface_UpdateHealth(GlobalContext* globalCtx) {
    InterfaceContext* interfaceCtx = &globalCtx->interfaceCtx;
    f32 speed = 0.1f * (R_UPDATE_RATE * (1.0f / 3.0f));

    if (interfaceCtx->unk_22C) {
        sHeartBeatScale -= speed;
        if (sHeartBeatScale <= 0.0f) {
            sHeartBeatScale = 0.0f;
            interfaceCtx->unk_22C = false;
            if (!Player_InCsMode(globalCtx) && (globalCtx->pauseCtx.state == 0) &&
                (globalCtx->pauseCtx.debugState == 0) && Health_IsCritical() && !Gameplay_InCsMode(globalCtx)) {
                func_80078884(NA_SE_SY_HITPOINT_ALARM);
            }
        }
    } else {
        sHeartBeatScale += speed;
        if (sHeartBeatScale >= 1.0f) {
            sHeartBeatScale = 1.0f;
            interfaceCtx->unk_22C = true;
        }
    }
}

/**
 * Sets a matrix for the beating heart
 */
static void Interface_SetBeatingHeartMtx(Mtx* matrix, f32 x, f32 y, f32 scale) {
    func_800D2CEC(matrix, 1.0f - (0.32f * scale), 1.0f - (0.32f * scale), 1.0f - (0.32f * scale), -130.0f + x,
                  94.5f - y, 0.0f);
}

static void Interface_DrawHealth(GlobalContext* globalCtx) {
    static Vtx beatingHeartVtx[] = {
        VTX(-8, 8, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
        VTX(8, 8, 0, 512, 0, 0xFF, 0xFF, 0xFF, 0xFF),
        VTX(-8, -8, 0, 0, 512, 0xFF, 0xFF, 0xFF, 0xFF),
        VTX(8, -8, 0, 512, 512, 0xFF, 0xFF, 0xFF, 0xFF),
    };

    static void* heartFractionTextures[] = {
        gInterfaceHeartQuarterTex,      gInterfaceHeartQuarterTex,      gInterfaceHeartQuarterTex,
        gInterfaceHeartQuarterTex,      gInterfaceHeartQuarterTex,      gInterfaceHeartHalfTex,
        gInterfaceHeartHalfTex,         gInterfaceHeartHalfTex,         gInterfaceHeartHalfTex,
        gInterfaceHeartHalfTex,         gInterfaceHeartThreeQuarterTex, gInterfaceHeartThreeQuarterTex,
        gInterfaceHeartThreeQuarterTex, gInterfaceHeartThreeQuarterTex, gInterfaceHeartThreeQuarterTex,
    };

    InterfaceContext* interfaceCtx = &globalCtx->interfaceCtx;

    // Pointer to the current texture in tmem
    void *curTex, *nextTex;

    // Current fractional heart
    s32 curHeartFraction = gSaveContext.health % 0x10;

    // Total heart capacity
    s16 totalHeartCount = gSaveContext.healthCapacity / 0x10;

    // Amount of hearts that are currently filled
    s16 curHeartCount = (gSaveContext.health + 0xF) / 0x10;

    // Current upper left x and y position to draw the hearts
    s16 x = HEARTS_X, y = HEARTS_Y;

    // Upper left x and y position to draw the beating heart at
    f32 beatingHeartX, beatingHeartY;

    // Tile size of the hearts
    register s16 tileSzX = 10, tileSzY = 10;

    // Space between each heart
    register s16 stepX = tileSzX, stepY = tileSzY + 1;

    // Matrix for scaling and positioning the beating heart
    Mtx* beatingHeartMtx;

    s32 i;

    OPEN_DISPS(globalCtx->state.gfxCtx, "", 0);

    if (interfaceCtx->healthAlpha <= 0) {
        return;
    }

    // Clear attributes before drawing
    Gfx_ClearOverlayAttributes(globalCtx->state.gfxCtx);

    // Set up the viewport for the beating heart
    Interface_SetView(interfaceCtx);

    // Set combine mode
    gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

    // If we have more than 1 heart, or the only heart remaining isn't a fraction, load the full heart texture
    if (totalHeartCount > 1 || curHeartFraction == 0) {
        OVERLAY_DISP = Gfx_TextureLoadIA8(OVERLAY_DISP, gInterfaceHeartFullTex, 16, 16);
        curTex = gInterfaceHeartFullTex;
    }

    // Draw all the normal hearts
    for (i = 1; i <= totalHeartCount; i++) {
        if (i != curHeartCount) {
            // Check if this is the first empty heart, in which case we load the empty heart texture for the remaining
            // iterations
            if (i == curHeartCount + 1) {
                OVERLAY_DISP = Gfx_TextureLoadIA8(OVERLAY_DISP, gInterfaceHeartEmptyTex, 16, 16);
                curTex = gInterfaceHeartEmptyTex;
            }

            // Draw shadow
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 0, 0, 0, interfaceCtx->healthAlpha);
            gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 255);
            OVERLAY_DISP = Gfx_TextureRectangle(OVERLAY_DISP, 1.0f + x, 1.0f + y, tileSzX, tileSzY, DRAW_ANCHOR_UL);

            // Draw heart
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->healthAlpha);
            if (i < curHeartCount) {
                gDPSetEnvColor(OVERLAY_DISP++, 255, 0, 0, 255);
            }
            OVERLAY_DISP = Gfx_TextureRectangle(OVERLAY_DISP, x, y, tileSzX, tileSzY, DRAW_ANCHOR_UL);
        } else {
            // Store the position that the beating heart needs to render at, but don't yet so the other hearts don't
            // overlap.
            beatingHeartX = x - HEARTS_X;
            beatingHeartY = y - HEARTS_Y;
        }

        // Adjust the x and y coords for the next hearts
        if ((i % HEARTS_PER_ROW) == 0) {
            x = HEARTS_X;
            y += stepY;
        } else {
            x += stepX;
        }
    }

    // Draw the beating heart
    beatingHeartMtx = Graph_Alloc(globalCtx->state.gfxCtx, sizeof(Mtx) * 2);

    // Set RCP preset for drawing quadrangles and the combine mode
    func_80094A14(globalCtx->state.gfxCtx);
    gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

    // If the fraction is not full prepare to load the empty heart background, if not choose the full heart texture
    // since a background is not needed
    nextTex = (curHeartFraction != 0) ? gInterfaceHeartEmptyTex : gInterfaceHeartFullTex;

    // Load the required texture
    if ((u32)curTex != (u32)nextTex) {
        gDPLoadTextureBlock(OVERLAY_DISP++, nextTex, G_IM_FMT_IA, G_IM_SIZ_8b, 16, 16, 0, G_TX_NOMIRROR | G_TX_WRAP,
                            G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);
        curTex = nextTex;
    }

    // Set the matrix for the shadow
    Interface_SetBeatingHeartMtx(beatingHeartMtx, beatingHeartX + 1.0f, beatingHeartY + 1.0f, sHeartBeatScale);

    // Draw shadow
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 0, 0, 0, interfaceCtx->healthAlpha);
    gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 255);
    gSPMatrix(OVERLAY_DISP++, beatingHeartMtx, G_MTX_MODELVIEW | G_MTX_LOAD);
    gSPVertex(OVERLAY_DISP++, beatingHeartVtx, 4, 0);
    gSP1Quadrangle(OVERLAY_DISP++, 0, 2, 3, 1, 0);

    // Set the matrix for the heart
    beatingHeartMtx++;
    Interface_SetBeatingHeartMtx(beatingHeartMtx, beatingHeartX, beatingHeartY, sHeartBeatScale);

    // Draw the heart background is the heart if the fraction is not full
    if (curHeartFraction != 0) {
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->healthAlpha);
        gSPMatrix(OVERLAY_DISP++, beatingHeartMtx, G_MTX_MODELVIEW | G_MTX_LOAD);
        gSPVertex(OVERLAY_DISP++, beatingHeartVtx, 4, 0);
        gSP1Quadrangle(OVERLAY_DISP++, 0, 2, 3, 1, 0);

        // Load the fraction texture
        OVERLAY_DISP = Gfx_TextureLoadIA8(OVERLAY_DISP, heartFractionTextures[curHeartFraction - 1], 16, 16);
        // curTex = heartFractionTextures[curHeartFraction - 1];
    }

    // Finish drawing the heart
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->healthAlpha);
    gDPSetEnvColor(OVERLAY_DISP++, 255, 0, 0, 255);
    gSPMatrix(OVERLAY_DISP++, beatingHeartMtx, G_MTX_MODELVIEW | G_MTX_LOAD);
    gSPVertex(OVERLAY_DISP++, beatingHeartVtx, 4, 0);
    gSP1Quadrangle(OVERLAY_DISP++, 0, 2, 3, 1, 0);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "", 0);
}
