/**
 * @file interface/buttons.c
 * @author zel.
 * @brief Functions for the buttons in the interface
 * @date 2021-12-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "global.h"
#include "custom/textures/interface_static/interface_static.h"

/**
 * Draw a button background with a shadow
 */
static Gfx* Interface_DrawButtonBackground(Gfx* head, InterfaceButtonInfo* buttonInfo, u8 alpha) {
    // Draw Shadow
    gDPSetPrimColor(head++, 0, 0, 0, 0, 0, alpha / 2);
    head = Gfx_TextureLoadRGBA32(head, buttonInfo->buttonTex, 32, 32);
    head = Gfx_TextureRectangle(head, buttonInfo->x + 2, buttonInfo->y + 2, buttonInfo->sizeX, buttonInfo->sizeY,
                                DRAW_ANCHOR_C);

    // Draw Button
    gDPSetPrimColor(head++, 0, 0, 255, 255, 255, alpha);
    head =
        Gfx_TextureRectangle(head, buttonInfo->x, buttonInfo->y, buttonInfo->sizeX, buttonInfo->sizeY, DRAW_ANCHOR_C);

    return head;
}

/**
 * Draw an ammo label of a certain value at a certain position on the screen
 */
//! @TODO: Remove this and remake it!
static void Interface_DrawItemAmmoLabel(GlobalContext* globalCtx, s16 button, s16 x, s16 y, s16 alpha) {
    s16 item = gSaveContext.equips.buttonItems[button];
    s16 ammo;

    OPEN_DISPS(globalCtx->state.gfxCtx, "", 0);

    gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);

    if ((item == ITEM_STICK) || (item == ITEM_NUT) || (item == ITEM_BOMB) || (item == ITEM_BOW) ||
        ((item >= ITEM_BOW_ARROW_FIRE) && (item <= ITEM_BOW_ARROW_LIGHT)) || (item == ITEM_SLINGSHOT) ||
        (item == ITEM_BOMBCHU) || (item == ITEM_BEAN)) {

        if ((item >= ITEM_BOW_ARROW_FIRE) && (item <= ITEM_BOW_ARROW_LIGHT)) {
            item = ITEM_BOW;
        }

        ammo = AMMO(item);

        gDPPipeSync(OVERLAY_DISP++);

        if (((item == ITEM_BOW) && (AMMO(item) == CUR_CAPACITY(UPG_QUIVER))) ||
            ((item == ITEM_BOMB) && (AMMO(item) == CUR_CAPACITY(UPG_BOMB_BAG))) ||
            ((item == ITEM_SLINGSHOT) && (AMMO(item) == CUR_CAPACITY(UPG_BULLET_BAG))) ||
            ((item == ITEM_STICK) && (AMMO(item) == CUR_CAPACITY(UPG_STICKS))) ||
            ((item == ITEM_NUT) && (AMMO(item) == CUR_CAPACITY(UPG_NUTS))) ||
            ((item == ITEM_BOMBCHU) && (ammo == 50)) || ((item == ITEM_BEAN) && (ammo == 15))) {
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 120, 255, 0, alpha);
        }

        if (ammo == 0) {
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 100, 100, 100, alpha);
        }

        for (item = 0; ammo >= 10; item++) {
            ammo -= 10;
        }

        if (item != 0) {
            OVERLAY_DISP = Gfx_TextureRectangleIA8(OVERLAY_DISP, &gInterfaceAmmoDigits[item][0], 8, 8, x, y, 8, 8,
                                                   1 << 10, 1 << 10);
        }

        OVERLAY_DISP = Gfx_TextureRectangleIA8(OVERLAY_DISP, &gInterfaceAmmoDigits[ammo][0], 8, 8, x + 6, y, 8, 8,
                                               1 << 10, 1 << 10);
    }

    gDPPipeSync(OVERLAY_DISP++);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "", 0);
}

static void Interface_DrawItemButtons(GlobalContext* globalCtx) {
#define GET_ITEM_ICON(button) ((void*)((u8*)interfaceCtx->iconItemSegment + (button * 0x1000)))
    static InterfaceButtonInfo buttonInfo[] = {
        { gInterfaceBButtonTex, NULL, B_BUTTON_X, B_BUTTON_Y, 28, 28 },
        { gInterfaceCButtonTex, gInterfaceCLeftEmptyArrowTex, C_LEFT_BUTTON_X, C_LEFT_BUTTON_Y, 26, 26 },
        { gInterfaceCButtonTex, gInterfaceCDownEmptyArrowTex, C_DOWN_BUTTON_X, C_DOWN_BUTTON_Y, 26, 26 },
        { gInterfaceCButtonTex, gInterfaceCRightEmptyArrowTex, C_RIGHT_BUTTON_X, C_RIGHT_BUTTON_Y, 26, 26 },
    };
    InterfaceContext* interfaceCtx = &globalCtx->interfaceCtx;
    s32 button;

    OPEN_DISPS(globalCtx->state.gfxCtx, "", 0);

    // Clear attributes before drawing
    Gfx_ClearOverlayAttributes(globalCtx->state.gfxCtx);

    // Draw background
    for (button = 0; button < HUD_ITEM_BTN_MAX; button++) {
        u8 alpha = interfaceCtx->itemAlpha[button];

        if (alpha <= 0) {
            continue;
        }

        OVERLAY_DISP = Interface_DrawButtonBackground(OVERLAY_DISP, &buttonInfo[button], alpha);
    }

    // Draw item
    for (button = 0; button < HUD_ITEM_BTN_MAX; button++) {
        u8 alpha = interfaceCtx->itemAlpha[button];

        if (alpha <= 0) {
            continue;
        }

        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, alpha);

        if ((button != HUD_ITEM_BTN_B) && (gSaveContext.equips.buttonItems[button] > 0xF0)) {
            // Empty C button arrow
            OVERLAY_DISP = Gfx_TextureLoadRGBA32(OVERLAY_DISP, buttonInfo[button].emptyItemTex, 16, 16);
            OVERLAY_DISP =
                Gfx_TextureRectangle(OVERLAY_DISP, buttonInfo[button].x, buttonInfo[button].y, 16, 16, DRAW_ANCHOR_C);
        } else if ((button == HUD_ITEM_BTN_B) && interfaceCtx->unk_1FA) {
            // Action label on B
            gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                              PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
            OVERLAY_DISP =
                Gfx_TextureLoadIA4(OVERLAY_DISP, (void*)((u8*)interfaceCtx->doActionSegment + 0x480), 48, 16);
            OVERLAY_DISP =
                Gfx_TextureRectangle(OVERLAY_DISP, buttonInfo[button].x, buttonInfo[button].y, 48, 16, DRAW_ANCHOR_C);
        } else {
            // Item icon
            gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);
            OVERLAY_DISP = Gfx_TextureLoadRGBA32(OVERLAY_DISP, GET_ITEM_ICON(button), 32, 32);

            if (button != HUD_ITEM_BTN_B) {
                OVERLAY_DISP =
                    Gfx_TextureRectangle(OVERLAY_DISP, buttonInfo[button].x, buttonInfo[button].y,
                                         buttonInfo[button].sizeX - 2, buttonInfo[button].sizeX - 2, DRAW_ANCHOR_C);
                //! @TODO: Remove this and replace it!
                Interface_DrawItemAmmoLabel(globalCtx, button, buttonInfo[button].x + 1 - 12,
                                            buttonInfo[button].y + 17 - 12, alpha);
            } else {
                OVERLAY_DISP = Gfx_TextureRectangle(OVERLAY_DISP, buttonInfo[button].x, buttonInfo[button].y,
                                                    buttonInfo[button].sizeX, buttonInfo[button].sizeX, DRAW_ANCHOR_C);
                //! @TODO: Remove this and replace it!
                Interface_DrawItemAmmoLabel(globalCtx, button, buttonInfo[button].x + 1 - 14,
                                            buttonInfo[button].y + 17 - 14, alpha);
            }
        }
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "", 0);
}

static void Interface_DrawInventoryEquipEffects(GlobalContext* globalCtx) {
    PauseContext* pauseCtx = &globalCtx->pauseCtx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "", 0);

    // Clear attributes before drawing
    Gfx_ClearOverlayAttributes(globalCtx->state.gfxCtx);

    if (pauseCtx->inventoryEquipping > 0) {
        // Inventory Equip Effects
        gSPSegment(OVERLAY_DISP++, 0x08, pauseCtx->iconItemSegment);
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, 255);
        gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
        OVERLAY_DISP = Gfx_TextureLoadRGBA32(OVERLAY_DISP,
                                             (void*)(0x08000000 + pauseCtx->inventoryEquippingItem * 0x1000), 32, 32);
        OVERLAY_DISP = Gfx_TextureRectangle(OVERLAY_DISP, pauseCtx->equipAnimX, pauseCtx->equipAnimY,
                                            pauseCtx->equipAnimSize, pauseCtx->equipAnimSize, DRAW_ANCHOR_C);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "", 0);
}

static void Interface_DrawStartButton(GlobalContext* globalCtx) {
    static InterfaceButtonInfo startButtonInfo = {
        gInterfaceStartButtonTex, NULL, START_BUTTON_X, START_BUTTON_Y, 24, 24,
    };
    InterfaceContext* interfaceCtx = &globalCtx->interfaceCtx;
    PauseContext* pauseCtx = &globalCtx->pauseCtx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "", 0);

    if (interfaceCtx->startAlpha <= 0) {
        return;
    }

    // Clear attributes before drawing
    Gfx_ClearOverlayAttributes(globalCtx->state.gfxCtx);

    // Draw background
    OVERLAY_DISP = Interface_DrawButtonBackground(OVERLAY_DISP, &startButtonInfo, interfaceCtx->startAlpha);

    if (((pauseCtx->state < 8) || (pauseCtx->state >= 18)) &&
        ((globalCtx->pauseCtx.state != 0) || (globalCtx->pauseCtx.debugState != 0))) {
        // Action label
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->startAlpha);
        gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                          PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
        OVERLAY_DISP = Gfx_TextureLoadIA4(OVERLAY_DISP, (void*)((u8*)interfaceCtx->doActionSegment + 0x300), 48, 16);
        OVERLAY_DISP = Gfx_TextureRectangle(OVERLAY_DISP, startButtonInfo.x, startButtonInfo.y, 48, 16, DRAW_ANCHOR_C);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "", 0);
}

static void Interface_DrawActionButton(GlobalContext* globalCtx) {
    static Vtx actionButtonVtx[] = {
        VTX(-14, 14, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
        VTX(14, 14, 0, 1024, 0, 0xFF, 0xFF, 0xFF, 0xFF),
        VTX(-14, -14, 0, 0, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
        VTX(14, -14, 0, 1024, 1024, 0xFF, 0xFF, 0xFF, 0xFF),
    };
    static Vtx actionLabelVtx[] = {
        VTX(-24, 8, 0, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF),
        VTX(24, 8, 0, 1536, 0, 0xFF, 0xFF, 0xFF, 0xFF),
        VTX(-24, -8, 0, 0, 512, 0xFF, 0xFF, 0xFF, 0xFF),
        VTX(24, -8, 0, 1536, 512, 0xFF, 0xFF, 0xFF, 0xFF),
    };
    InterfaceContext* interfaceCtx = &globalCtx->interfaceCtx;
    s32 i;

    OPEN_DISPS(globalCtx->state.gfxCtx, "", 0);

    if (interfaceCtx->aAlpha <= 0) {
        return;
    }

    // Set RCP preset for drawing quadrangles
    func_80094A14(globalCtx->state.gfxCtx);

    gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 255);
    gSPClearGeometryMode(OVERLAY_DISP++, G_CULL_BOTH);
    gDPSetCombineMode(OVERLAY_DISP++, G_CC_MODULATERGBA_PRIM, G_CC_MODULATERGBA_PRIM);

    // Draw the button
    for (i = 0; i < 3; i++) {
        if (i != 0) {
            func_8008A8B8(globalCtx, A_BUTTON_Y, A_BUTTON_Y + 45, A_BUTTON_X, A_BUTTON_X + 45);
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->aAlpha);
        } else {
            // Button shadow
            func_8008A8B8(globalCtx, A_BUTTON_Y + 2, A_BUTTON_Y + 45 + 2, A_BUTTON_X + 2, A_BUTTON_X + 45 + 2);
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 0, 0, 0, interfaceCtx->aAlpha / 2);
        }

        Matrix_Translate(0.0f, 0.0f, -38.0f, MTXMODE_NEW);
        Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);

        if (i == 2) {
            // Flipping part of the button
            Matrix_RotateX(interfaceCtx->unk_1F4 / 10000.0f, MTXMODE_APPLY);
        }

        gSPMatrix(OVERLAY_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "", 0), G_MTX_MODELVIEW | G_MTX_LOAD);
        gSPVertex(OVERLAY_DISP++, actionButtonVtx, 4, 0);

        if (i == 0) {
            OVERLAY_DISP = Gfx_TextureLoadRGBA32(OVERLAY_DISP, gInterfaceAButtonBackgroundTex, 32, 32);
        } else if (i == 2) {
            OVERLAY_DISP = Gfx_TextureLoadRGBA32(OVERLAY_DISP, gInterfaceAButtonTex, 32, 32);
        }

        gSP1Quadrangle(OVERLAY_DISP++, 0, 2, 3, 1, 0);
        gDPPipeSync(OVERLAY_DISP++);
    }

    // Draw the action label
    func_8008A8B8(globalCtx, A_BUTTON_Y, A_BUTTON_Y + 45, A_BUTTON_X, A_BUTTON_X + 45);
    gSPSetGeometryMode(OVERLAY_DISP++, G_CULL_BACK);
    gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->aAlpha);
    gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 0);
    Matrix_Translate(0.0f, 0.0f, WREG(46 + gSaveContext.language) / 10.0f, MTXMODE_NEW);
    Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
    Matrix_RotateX(interfaceCtx->unk_1F4 / 10000.0f, MTXMODE_APPLY);
    gSPMatrix(OVERLAY_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "", 0), G_MTX_MODELVIEW | G_MTX_LOAD);
    gSPVertex(OVERLAY_DISP++, actionLabelVtx, 4, 0);

    // Load the action label texture
    OVERLAY_DISP =
        Gfx_TextureLoadIA4(OVERLAY_DISP,
                           (void*)((u8*)interfaceCtx->doActionSegment +
                                   (((interfaceCtx->unk_1EC < 2) || (interfaceCtx->unk_1EC == 3)) ? 0 : 0x180)),
                           48, 16);

    gSP1Quadrangle(OVERLAY_DISP++, 0, 2, 3, 1, 0);
    gDPPipeSync(OVERLAY_DISP++);

    // Reset view
    Interface_SetView(interfaceCtx);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "", 0);
}
