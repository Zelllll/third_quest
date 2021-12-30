/**
 * @file interface/rupees.c
 * @author zel.
 * @brief Functions for the rupees in the interface
 * @date 2021-12-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "global.h"
#include "custom/textures/interface_static/interface_static.h"

static void Interface_DrawRupees(GlobalContext* globalCtx) {
    static s16 walletDigits[] = { 2, 3, 3, 4 };
    static Color_RGB8 rupeeColors[] = {
        { 0, 255, 0 },
        { 128, 128, 255 },
        { 255, 64, 64 },
        { 170, 0, 25 },
    };
    InterfaceContext* interfaceCtx = &globalCtx->interfaceCtx;
    s16 digits = walletDigits[CUR_UPG_VALUE(UPG_WALLET)];

    OPEN_DISPS(globalCtx->state.gfxCtx, "", 0);

    if (interfaceCtx->magicAlpha <= 0) {
        return;
    }

    // Clear attributes before drawing
    Gfx_ClearOverlayAttributes(globalCtx->state.gfxCtx);

    // Set the color and alpha of the shadow
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 0, 0, 0, interfaceCtx->magicAlpha);

    // Set combiner for the text
    gDPSetCombineLERP(OVERLAY_DISP++, 0, 0, 0, PRIMITIVE, TEXEL0, 0, PRIMITIVE, 0, 0, 0, 0, PRIMITIVE, TEXEL0, 0,
                      PRIMITIVE, 0);

    // Draw the text shadow
    OVERLAY_DISP = Gfx_DrawDigits(OVERLAY_DISP, gSaveContext.rupees, RUPEE_DIGIT_X + 1, RUPEE_DIGIT_Y + 1, digits, true,
                                  DIGIT_DRAW_TYPE_COUNTER);

    // Set the color and alpha of the text
    if (gSaveContext.rupees == CUR_CAPACITY(UPG_WALLET)) {
        // Max capacity
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 120, 255, 0, interfaceCtx->magicAlpha);
    } else if (gSaveContext.rupees > 0) {
        // Normal
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->magicAlpha);
    } else {
        // None
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 100, 100, 100, interfaceCtx->magicAlpha);
    }

    // Draw the text
    OVERLAY_DISP = Gfx_DrawDigits(OVERLAY_DISP, gSaveContext.rupees, RUPEE_DIGIT_X, RUPEE_DIGIT_Y, digits, true,
                                  DIGIT_DRAW_TYPE_COUNTER);

    // Clear attributes before drawing
    Gfx_ClearOverlayAttributes(globalCtx->state.gfxCtx);

    // Set the color of the icon based on the upgrade
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, rupeeColors[CUR_UPG_VALUE(UPG_WALLET)].r,
                    rupeeColors[CUR_UPG_VALUE(UPG_WALLET)].g, rupeeColors[CUR_UPG_VALUE(UPG_WALLET)].b,
                    interfaceCtx->magicAlpha);

    // Draw the rupee icon
    OVERLAY_DISP = Gfx_TextureRectangleIA8(OVERLAY_DISP, gInterfaceRupeeTex, 16, 16, RUPEE_DIGIT_X - 16 - (8 * digits),
                                           RUPEE_DIGIT_Y, 16, 16, 1 << 10, 1 << 10);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "", 0);
}
