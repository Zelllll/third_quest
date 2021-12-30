/**
 * @file interface/magic_meter.c
 * @author zel.
 * @brief Functions for the magic meter in the interface
 * @date 2021-12-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "global.h"
#include "custom/textures/interface_static/interface_static.h"

static Color_RGB8 sMagicFillColors[] = {
    { 0, 192, 0 },
    { 192, 192, 0 },
};

static Color_RGB8 sCurMagicFillColor = { 0, 192, 0 };

// @TODO: Refactor all of this!
void Interface_UpdateMagicMeter(GlobalContext* globalCtx) {
    InterfaceContext* interfaceCtx = &globalCtx->interfaceCtx;
    static s16 sMagicBorderIndexes[] = { 0, 1, 1, 0 };
    static s16 sMagicBorderRatio = 2;
    static s16 sMagicBorderStep = 1;
    MessageContext* msgCtx = &globalCtx->msgCtx;
    s16 borderChangeR;
    s16 borderChangeG;
    s16 borderChangeB;
    s16 temp;

    switch (gSaveContext.unk_13F0) {
        case 8:
            temp = gSaveContext.magicLevel * 0x30;
            if (gSaveContext.unk_13F4 != temp) {
                if (gSaveContext.unk_13F4 < temp) {
                    gSaveContext.unk_13F4 += 8;
                    if (gSaveContext.unk_13F4 > temp) {
                        gSaveContext.unk_13F4 = temp;
                    }
                } else {
                    gSaveContext.unk_13F4 -= 8;
                    if (gSaveContext.unk_13F4 <= temp) {
                        gSaveContext.unk_13F4 = temp;
                    }
                }
            } else {
                gSaveContext.unk_13F0 = 9;
            }
            break;

        case 9:
            gSaveContext.magic += 4;

            if (gSaveContext.gameMode == 0 && gSaveContext.sceneSetupIndex < 4) {
                Audio_PlaySoundGeneral(NA_SE_SY_GAUGE_UP - SFX_FLAG, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                       &D_801333E8);
            }

            if (gSaveContext.magic >= gSaveContext.unk_13F6) {
                gSaveContext.magic = gSaveContext.unk_13F6;
                gSaveContext.unk_13F0 = gSaveContext.unk_13F2;
                gSaveContext.unk_13F2 = 0;
            }
            break;

        case 1:
            sMagicBorderRatio = 2;
            gSaveContext.unk_13F0 = 2;
            break;

        case 2:
            gSaveContext.magic -= 2;
            if (gSaveContext.magic <= 0) {
                gSaveContext.magic = 0;
                gSaveContext.unk_13F0 = 3;
                sCurMagicFillColor.r = 0;
                sCurMagicFillColor.g = 192;
                sCurMagicFillColor.b = 0;
            } else if (gSaveContext.magic == gSaveContext.unk_13F8) {
                gSaveContext.unk_13F0 = 3;
                sCurMagicFillColor.r = 0;
                sCurMagicFillColor.g = 192;
                sCurMagicFillColor.b = 0;
            }
        case 3:
        case 4:
        case 6:
            temp = sMagicBorderIndexes[sMagicBorderStep];
            borderChangeR = ABS(sCurMagicFillColor.r - sMagicFillColors[temp].r) / sMagicBorderRatio;
            borderChangeG = ABS(sCurMagicFillColor.g - sMagicFillColors[temp].g) / sMagicBorderRatio;
            borderChangeB = ABS(sCurMagicFillColor.b - sMagicFillColors[temp].b) / sMagicBorderRatio;

            if (sCurMagicFillColor.r >= sMagicFillColors[temp].r) {
                sCurMagicFillColor.r -= borderChangeR;
            } else {
                sCurMagicFillColor.r += borderChangeR;
            }

            if (sCurMagicFillColor.g >= sMagicFillColors[temp].g) {
                sCurMagicFillColor.g -= borderChangeG;
            } else {
                sCurMagicFillColor.g += borderChangeG;
            }

            if (sCurMagicFillColor.b >= sMagicFillColors[temp].b) {
                sCurMagicFillColor.b -= borderChangeB;
            } else {
                sCurMagicFillColor.b += borderChangeB;
            }

            sMagicBorderRatio--;
            if (sMagicBorderRatio == 0) {
                sCurMagicFillColor.r = sMagicFillColors[temp].r;
                sCurMagicFillColor.g = sMagicFillColors[temp].g;
                sCurMagicFillColor.b = sMagicFillColors[temp].b;
                sMagicBorderRatio = YREG(40 + sMagicBorderStep);
                sMagicBorderStep++;
                if (sMagicBorderStep >= 4) {
                    sMagicBorderStep = 0;
                }
            }
            break;

        case 5:
            sCurMagicFillColor.r = 0;
            sCurMagicFillColor.g = 192;
            sCurMagicFillColor.b = 0;
            gSaveContext.unk_13F0 = 0;
            break;

        case 7:
            if ((globalCtx->pauseCtx.state == 0) && (globalCtx->pauseCtx.debugState == 0) && (msgCtx->msgMode == 0) &&
                (globalCtx->gameOverCtx.state == GAMEOVER_INACTIVE) && (globalCtx->sceneLoadFlag == 0) &&
                (globalCtx->transitionMode == 0) && !Gameplay_InCsMode(globalCtx)) {
                if ((gSaveContext.magic == 0) || ((func_8008F2F8(globalCtx) >= 2) && (func_8008F2F8(globalCtx) < 5)) ||
                    ((gSaveContext.equips.buttonItems[1] != ITEM_LENS) &&
                     (gSaveContext.equips.buttonItems[2] != ITEM_LENS) &&
                     (gSaveContext.equips.buttonItems[3] != ITEM_LENS)) ||
                    (globalCtx->actorCtx.unk_03 == 0)) {
                    globalCtx->actorCtx.unk_03 = 0;
                    Audio_PlaySoundGeneral(NA_SE_SY_GLASSMODE_OFF, &D_801333D4, 4, &D_801333E0, &D_801333E0,
                                           &D_801333E8);
                    gSaveContext.unk_13F0 = 0;
                    sCurMagicFillColor.r = 0;
                    sCurMagicFillColor.g = 192;
                    sCurMagicFillColor.b = 0;
                    break;
                }

                interfaceCtx->unk_230--;
                if (interfaceCtx->unk_230 == 0) {
                    gSaveContext.magic--;
                    interfaceCtx->unk_230 = 80;
                }
            }

            temp = sMagicBorderIndexes[sMagicBorderStep];
            borderChangeR = ABS(sCurMagicFillColor.r - sMagicFillColors[temp].r) / sMagicBorderRatio;
            borderChangeG = ABS(sCurMagicFillColor.g - sMagicFillColors[temp].g) / sMagicBorderRatio;
            borderChangeB = ABS(sCurMagicFillColor.b - sMagicFillColors[temp].b) / sMagicBorderRatio;

            if (sCurMagicFillColor.r >= sMagicFillColors[temp].r) {
                sCurMagicFillColor.r -= borderChangeR;
            } else {
                sCurMagicFillColor.r += borderChangeR;
            }

            if (sCurMagicFillColor.g >= sMagicFillColors[temp].g) {
                sCurMagicFillColor.g -= borderChangeG;
            } else {
                sCurMagicFillColor.g += borderChangeG;
            }

            if (sCurMagicFillColor.b >= sMagicFillColors[temp].b) {
                sCurMagicFillColor.b -= borderChangeB;
            } else {
                sCurMagicFillColor.b += borderChangeB;
            }

            sMagicBorderRatio--;
            if (sMagicBorderRatio == 0) {
                sCurMagicFillColor.r = sMagicFillColors[temp].r;
                sCurMagicFillColor.g = sMagicFillColors[temp].g;
                sCurMagicFillColor.b = sMagicFillColors[temp].b;
                sMagicBorderRatio = YREG(40 + sMagicBorderStep);
                sMagicBorderStep++;
                if (sMagicBorderStep >= 4) {
                    sMagicBorderStep = 0;
                }
            }
            break;

        case 10:
            gSaveContext.magic += 4;
            Audio_PlaySoundGeneral(NA_SE_SY_GAUGE_UP - SFX_FLAG, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
            if (gSaveContext.magic >= gSaveContext.unk_13F8) {
                gSaveContext.magic = gSaveContext.unk_13F8;
                gSaveContext.unk_13F0 = gSaveContext.unk_13F2;
                gSaveContext.unk_13F2 = 0;
            }
            break;

        default:
            gSaveContext.unk_13F0 = 0;
            break;
    }
}

static void Interface_DrawMagicMeter(GlobalContext* globalCtx) {
    InterfaceContext* interfaceCtx = &globalCtx->interfaceCtx;

    if (interfaceCtx->magicAlpha <= 0) {
        return;
    }

    if (gSaveContext.magicLevel != 0) {
        s32 rowAdd = Health_GetRows() - 1;
        s16 centerWidth;
        s16 x;

        OPEN_DISPS(globalCtx->state.gfxCtx, "", 0);

        // Clear attributes before drawing
        Gfx_ClearOverlayAttributes(globalCtx->state.gfxCtx);
        gSPDisplayList(OVERLAY_DISP++, gSetupFillRect);

        if (gSaveContext.magic < gSaveContext.unk_13F6) {
            // Set the color and alpha for the empty fill
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 0, 0, 0, (s32)((f32)interfaceCtx->magicAlpha / 2.0f));

            // Draw the empty fill
            gDPFillRectangle(OVERLAY_DISP++, MAGIC_METER_X + 3 + gSaveContext.magic, MAGIC_METER_Y(rowAdd) + 1,
                             MAGIC_METER_X + 3 + gSaveContext.unk_13F6, MAGIC_METER_Y(rowAdd) + 8);
        }

        if (gSaveContext.magic > 0) {
            // Set the color and alpha for the magic fill
            gDPSetPrimColor(OVERLAY_DISP++, 0, 0, sCurMagicFillColor.r, sCurMagicFillColor.g, sCurMagicFillColor.b,
                            interfaceCtx->magicAlpha);

            // Draw the magic fill
            gDPFillRectangle(OVERLAY_DISP++, MAGIC_METER_X + 3, MAGIC_METER_Y(rowAdd) + 1,
                             MAGIC_METER_X + 3 + gSaveContext.magic, MAGIC_METER_Y(rowAdd) + 8);
        }

        // Clear attributes before drawing
        Gfx_ClearOverlayAttributes(globalCtx->state.gfxCtx);

        // Set the color and alpha for the border
        gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 255, 255, 255, interfaceCtx->magicAlpha);

        // Draw the left part of the border
        x = MAGIC_METER_X;

        OVERLAY_DISP = Gfx_TextureRectangleIA8(OVERLAY_DISP, gInterfaceMagicMeterLeftTex, 8, 11, x,
                                               MAGIC_METER_Y(rowAdd), 8, 11, 1 << 10, 1 << 10);

        // Draw the center of the border
        x += 8;
        centerWidth = gSaveContext.unk_13F6 - 8;

        OVERLAY_DISP =
            Gfx_TextureRectangleIA8(OVERLAY_DISP, gInterfaceMagicMeterCenterTex, 8, 11, x, MAGIC_METER_Y(rowAdd),
                                    centerWidth, 11, (u16)(qu510((f32)8 / centerWidth)), 1 << 10);

        // Draw the right part of the border
        x += centerWidth;

        OVERLAY_DISP = Gfx_TextureRectangleIA8(OVERLAY_DISP, gInterfaceMagicMeterRightTex, 8, 11, x,
                                               MAGIC_METER_Y(rowAdd), 8, 11, 1 << 10, 1 << 10);

        CLOSE_DISPS(globalCtx->state.gfxCtx, "", 0);
    }
}
