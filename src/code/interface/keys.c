/**
 * @file interface/keys.c
 * @author zel.
 * @brief Functions for the keys in the interface
 * @date 2021-12-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "global.h"
#include "custom/textures/interface_static/interface_static.h"

s32 Map_GetDungeonIndex() {
    //! @TODO: Refactor how the map indexes work, this function will return -1 for non-dungeons eventually
    return gSaveContext.mapIndex;
}

s32 Keys_GetBoss() {
    s32 mapIndex = Map_GetDungeonIndex();

    if (mapIndex >= 0) {
        return (gSaveContext.inventory.dungeonItems[mapIndex] & (1 << DUNGEON_KEY_BOSS));
    }
    return false;
}

s32 Keys_GetSmall() {
    s32 mapIndex = Map_GetDungeonIndex();

    if (mapIndex >= 0) {
        return gSaveContext.inventory.dungeonKeys[mapIndex];
    }
    return false;
}

static void Interface_DrawDungeonKeys(GlobalContext* globalCtx) {
    InterfaceContext* interfaceCtx = &globalCtx->interfaceCtx;
    s16 x = KEY_X, y = KEY_Y;
    s32 i;
    s32 smallKeys = Keys_GetSmall();

    OPEN_DISPS(globalCtx->state.gfxCtx, "", 0);

    if (interfaceCtx->magicAlpha <= 0) {
        return;
    }

    // Clear attributes before drawing
    Gfx_ClearOverlayAttributes(globalCtx->state.gfxCtx);

    // Set alpha
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->magicAlpha);

    // Draw the boss key
    if (Keys_GetBoss()) {
        OVERLAY_DISP = Gfx_TextureLoadRGBA32(OVERLAY_DISP, gInterfaceBossKeyTex, 16, 16);
        OVERLAY_DISP = Gfx_TextureRectangle(OVERLAY_DISP, x, y, 16, 16, DRAW_ANCHOR_UL);
        x -= 12;
    }

    // Draw the small keys
    if (smallKeys > 0) {
        OVERLAY_DISP = Gfx_TextureLoadIA8(OVERLAY_DISP, gInterfaceSmallKeyTex, 16, 16);

        for (i = 0; i < smallKeys; x -= 11, i++) {
            OVERLAY_DISP = Gfx_TextureRectangle(OVERLAY_DISP, x, y, 16, 16, DRAW_ANCHOR_UL);
        }
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "", 0);
}
