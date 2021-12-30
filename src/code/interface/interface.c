/**
 * @file interface/health.c
 * @author zel.
 * @brief Functions for the interface
 * @date 2021-12-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "global.h"
#include "custom/textures/interface_static/interface_static.h"

#include "health.c"
#include "keys.c"
#include "rupees.c"
#include "magic_meter.c"
#include "buttons.c"
// #include "title_card.c"

void Interface_SetView(InterfaceContext* interfaceCtx) {
    SET_FULLSCREEN_VIEWPORT(&interfaceCtx->view);
    func_800AB2C4(&interfaceCtx->view);
}

void Interface_Draw(GlobalContext* globalCtx) {
    InterfaceContext* interfaceCtx = &globalCtx->interfaceCtx;
    PauseContext* pauseCtx = &globalCtx->pauseCtx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "", 0);

    gSPSegment(OVERLAY_DISP++, 0x02, interfaceCtx->parameterSegment);
    gSPSegment(OVERLAY_DISP++, 0x07, interfaceCtx->doActionSegment);
    gSPSegment(OVERLAY_DISP++, 0x08, interfaceCtx->iconItemSegment);
    gSPSegment(OVERLAY_DISP++, 0x0B, interfaceCtx->mapSegment);

    if (pauseCtx->debugState == 0) {
        Interface_DrawHealth(globalCtx);
        Interface_DrawDungeonKeys(globalCtx);
        Interface_DrawRupees(globalCtx);
        Interface_DrawMagicMeter(globalCtx);
        //! @TODO: Draw minimap/quickswap here
        if ((R_PAUSE_MENU_MODE != 2) && (R_PAUSE_MENU_MODE != 3)) {
            // Z Target
            func_8002C124(&globalCtx->actorCtx.targetCtx, globalCtx);
        }
        Interface_DrawItemButtons(globalCtx);
        Interface_DrawStartButton(globalCtx);
        Interface_DrawActionButton(globalCtx);
        Interface_DrawInventoryEquipEffects(globalCtx);
        //! @TODO: Draw horseback UI here
        //! @TODO: Draw timer here
    } else if (pauseCtx->debugState == 3) {
        FlagSet_Update(globalCtx);
    }

    if (interfaceCtx->unk_244 != 0) {
        gDPPipeSync(OVERLAY_DISP++);
        gSPDisplayList(OVERLAY_DISP++, gSetupFillRect);
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 0, 0, 0, interfaceCtx->unk_244);
        gDPFillRectangle(OVERLAY_DISP++, 0, 0, gScreenWidth - 1, gScreenHeight - 1);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "", 0);
}
