/**
 * @file interface/title_card.c
 * @author zel.
 * @brief Functions for the title cards when entering a new area
 * @date 2021-12-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#if 0
#include "global.h"
#include "custom/textures/interface_static/interface_static.h"

/**
 * Draw the area title card
 */
// @TODO: probably redo how titleCtx works and the title update function that this relies on
void Interface_DrawAreaTitleCard(GlobalContext* globalCtx, TitleCardContext* titleCtx) {
    InterfaceContext* interfaceCtx = &globalCtx->interfaceCtx;
    f32 percent = (f32)titleCtx->alpha / 255.0f;
    f32 innerGradientWidth, innerGradientPosX;
    f32 borderGradientWidth, borderGradientPosX;
    f32 scaleFactor = 1.35f;

    OPEN_DISPS(globalCtx->state.gfxCtx, "", 0);

    // Clear attributes before drawing
    Gfx_ClearOverlayAttributes(globalCtx->state.gfxCtx);

    if (titleCtx->alpha != 0) {
        // Set the color and alpha of the inner gradient
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, AREA_TITLE_BEGUNA_R, AREA_TITLE_BEGUNA_G, AREA_TITLE_BEGUNA_B,
                        (u8)(percent * AREA_TITLE_ALPHA));

        // Calculate the width and x position of the inner gradient animation
        innerGradientWidth =
            CLAMP_MAX((f32)AREA_TITLE_WIDTH_MIN +
                          (scaleFactor * percent * ((f32)AREA_TITLE_GRAD_TILE_WIDTH - AREA_TITLE_WIDTH_MIN)),
                      (f32)AREA_TITLE_GRAD_TILE_WIDTH);
        innerGradientPosX = (f32)AREA_TITLE_X_CENTER - (innerGradientWidth / 2.0f);

        // Draw the inner gradient
        OVERLAY_DISP = Gfx_TextureRectangleIA16(OVERLAY_DISP, gInterfaceTitleCardGradientTex, AREA_TITLE_GRAD_WIDTH,
                                                AREA_TITLE_GRAD_HEIGHT, innerGradientPosX, AREA_TITLE_Y,
                                                AREA_TITLE_GRAD_TILE_WIDTH, AREA_TITLE_GRAD_TILE_HEIGHT,
                                                (u16)(qu510((f32)AREA_TITLE_GRAD_WIDTH / innerGradientWidth)),
                                                (u16)(qu510((f32)AREA_TITLE_GRAD_HEIGHT / AREA_TITLE_GRAD_TILE_HEIGHT)));

        // Set the color and alpha of the border
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 200, 200, 200, (u8)(percent * AREA_TITLE_ALPHA));

        // Calculate the width and x position of the border gradient animation
        if (percent >= 0.40f) {
            // Calculate the width and x position of the border gradient animation
            scaleFactor = 1.80f;
            borderGradientWidth = CLAMP_MAX(scaleFactor * (percent - 0.30f) * (f32)AREA_TITLE_BORDER_TILE_WIDTH,
                                            AREA_TITLE_BORDER_TILE_WIDTH);
            borderGradientPosX = (f32)AREA_TITLE_X_CENTER - (borderGradientWidth / 2.0f);

            // Upper border
            gSPTextureRectangle(OVERLAY_DISP++, borderGradientPosX * 4, AREA_TITLE_BORDER_UPPER_Y * 4,
                                (borderGradientPosX + borderGradientWidth) * 4,
                                (AREA_TITLE_BORDER_UPPER_Y + AREA_TITLE_BORDER_TILE_HEIGHT) * 4, G_TX_RENDERTILE, 0, 0,
                                (u16)(qu510((f32)AREA_TITLE_GRAD_WIDTH / borderGradientWidth)),
                                (u16)(qu510((f32)AREA_TITLE_GRAD_HEIGHT / AREA_TITLE_BORDER_TILE_HEIGHT)));

            // Lower border
            gSPTextureRectangle(OVERLAY_DISP++, borderGradientPosX * 4, AREA_TITLE_BORDER_LOWER_Y * 4,
                                (borderGradientPosX + borderGradientWidth) * 4,
                                (AREA_TITLE_BORDER_LOWER_Y + AREA_TITLE_BORDER_TILE_HEIGHT) * 4, G_TX_RENDERTILE, 0, 0,
                                (u16)(qu510((f32)AREA_TITLE_GRAD_WIDTH / borderGradientWidth)),
                                (u16)(qu510((f32)AREA_TITLE_GRAD_HEIGHT / AREA_TITLE_BORDER_TILE_HEIGHT)));
        }
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "", 0);
}

#endif
