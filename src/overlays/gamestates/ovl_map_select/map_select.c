/**
 * @file map_select.c
 * @author zel.
 * @brief Map selection game state
 * @date 2021-12-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "global.h"
#include "alloca.h"
#include "custom/textures/map_select_static/map_select_static.h"

#include "area_list.c"

#define MAP_SELECT_TITLE_X 15
#define MAP_SELECT_TITLE_Y 27

#define MAP_SELECT_Z_LEFT_X 2
#define MAP_SELECT_Z_LEFT_Y 27

#define MAP_SELECT_R_RIGHT_X 35
#define MAP_SELECT_R_RIGHT_Y 27

#define MAP_SELECT_SCENE_LIST_X 3
#define MAP_SELECT_SCENE_LIST_Y 2

#define MAP_SELECT_SCENE_SETUP_X 3
#define MAP_SELECT_SCENE_SETUP_Y 23

#define MAP_SELECT_SPAWN_X 19
#define MAP_SELECT_SPAWN_Y 23

#define MAP_SELECT_3Q_LOGO_WIDTH 32
#define MAP_SELECT_3Q_LOGO_HEIGHT 32
#define MAP_SELECT_3Q_LOGO_X 275
#define MAP_SELECT_3Q_LOGO_Y 10

#define DIVIDER_LINE_Y 205

#define MAP_SELECT_SCENE_PRINT_MAX 20 // print only 20 scene names at a time

#define BUTTON_HOLD_WAIT 30

#define INIT_PRINTER(printer, x, y, r, g, b) \
    GfxPrint_SetPos(printer, x, y);          \
    GfxPrint_SetColor(printer, r, g, b, 255);

extern MapSelectArea gMapSelectSceneList[];

void MapSelect_SwitchToGameplay(MapSelectContext* this) {
    // Use the debug save (might want to remove this)
    if (gSaveContext.fileNum == 0xFF) {
        Sram_InitDebugSave();
        gSaveContext.unk_13F6 = gSaveContext.magic;
        gSaveContext.magic = 0;
        gSaveContext.unk_13F4 = 0;
        gSaveContext.magicLevel = gSaveContext.magic;
    }

    // Make sure buttons are enabled
    gSaveContext.buttonStatus[4] = BTN_ENABLED;
    gSaveContext.buttonStatus[3] = BTN_ENABLED;
    gSaveContext.buttonStatus[2] = BTN_ENABLED;
    gSaveContext.buttonStatus[1] = BTN_ENABLED;
    gSaveContext.buttonStatus[0] = BTN_ENABLED;

    // Setup save info
    gSaveContext.unk_13E7 = gSaveContext.unk_13E8 = gSaveContext.unk_13EA = gSaveContext.unk_13EC = 0;
    gSaveContext.nightFlag = false;
    gSaveContext.dayTime = gSaveContext.skyboxTime = 0x8001;
    gSaveContext.linkAge = 0;

    // Reset the music
    Audio_QueueSeqCmd(SEQ_PLAYER_BGM_MAIN << 24 | NA_BGM_STOP);

    // Set up the scene
    gSaveContext.cutsceneIndex = this->sceneSetup;
    gSaveContext.entranceIndex = gMapSelectSceneList[this->curScene].entrance;
    gSaveContext.respawnFlag = 0;
    gSaveContext.respawn[RESPAWN_MODE_DOWN].entranceIndex = -1;
    gSaveContext.seqId = 0xFF;
    gSaveContext.natureAmbienceId = 0xFF;
    gSaveContext.showTitleCard = true;
    gWeatherMode = 0;

    // Switch to gameplay
    this->state.running = false;
    SET_NEXT_GAMESTATE(&this->state, Gameplay_Init, GlobalContext);
}

void MapSelect_Update(MapSelectContext* this) {
    Input* controller1 = &this->state.input[0];

    if (this->state.running) {
        // Select scene
        if (CHECK_BTN_ALL(controller1->press.button, BTN_A) || CHECK_BTN_ALL(controller1->press.button, BTN_START)) {
            MapSelect_SwitchToGameplay(this);
            return;
        }

        // Increment and decrement spawn
        if (CHECK_BTN_ALL(controller1->press.button, BTN_CUP)) {
            this->sceneSpawn++;
        }
        if (CHECK_BTN_ALL(controller1->press.button, BTN_CDOWN)) {
            this->sceneSpawn--;
        }
        this->sceneSpawn = CLAMP(this->sceneSpawn, 0, 255);

        // Increment and decrement scene setup
        if (CHECK_BTN_ALL(controller1->press.button, BTN_CRIGHT)) {
            this->sceneSetup++;
        }
        if (CHECK_BTN_ALL(controller1->press.button, BTN_CLEFT)) {
            this->sceneSetup--;
        }
        this->sceneSetup = CLAMP(this->sceneSetup, 0, 255);

        // Scroll down
        if (CHECK_BTN_ALL(controller1->press.button, BTN_DDOWN)) {
            this->timer = BUTTON_HOLD_WAIT;
            goto LBL_INCREMENT_SCENE;
        } else if (CHECK_BTN_ALL(controller1->cur.button, BTN_DDOWN)) {
            if (this->timer != 0) {
                this->timer--;
            } else {
                this->timer = 0;
            LBL_INCREMENT_SCENE:
                Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                this->curScene++;
                if (this->curScene >= this->sceneCount) {
                    this->curScene = 0;
                }
            }
        }

        // Scroll up
        if (CHECK_BTN_ALL(controller1->press.button, BTN_DUP)) {
            this->timer = BUTTON_HOLD_WAIT;
            goto LBL_DECREMENT_SCENE;
        } else if (CHECK_BTN_ALL(controller1->cur.button, BTN_DUP)) {
            if (this->timer != 0) {
                this->timer--;
            } else {
                this->timer = 0;
            LBL_DECREMENT_SCENE:
                Audio_PlaySoundGeneral(NA_SE_SY_CURSOR, &D_801333D4, 4, &D_801333E0, &D_801333E0, &D_801333E8);
                this->curScene--;
                if (this->curScene < 0) {
                    this->curScene = this->sceneCount - 1;
                }
            }
        }

        // Scroll one page
        if (CHECK_BTN_ALL(controller1->press.button, BTN_Z)) {
            this->curScene += 20;
            if (this->curScene >= this->sceneCount) {
                this->curScene -= this->sceneCount + 1;
            }
        }

        // Save the current scene to restore if the map select is ever reopened
        dREG(80) = this->curScene;
    }
}

void MapSelect_PrintScenes(MapSelectContext* this, GfxPrint* printer) {
    char prefix[6];
    s32 printScene = this->curScene; // current scene being handled by the loop
    s32 i;

    for (i = 0; i < MAP_SELECT_SCENE_PRINT_MAX; i++, printScene++) {
        if (printScene >= this->sceneCount) {
            printScene = 0;
        }

        if (printScene == this->curScene) {
            INIT_PRINTER(printer, MAP_SELECT_SCENE_LIST_X, MAP_SELECT_SCENE_LIST_Y + i, 255, 255, 255);
        } else {
            INIT_PRINTER(printer, MAP_SELECT_SCENE_LIST_X, MAP_SELECT_SCENE_LIST_Y + i, 90, 90, 90);
        }

        if (gMapSelectSceneList[printScene].name != NULL) {
            // Set the number prefix for the scene
            if (printScene + 1 < 10) {
                sprintf(prefix, " %d:", printScene + 1);
            } else {
                sprintf(prefix, "%d:", printScene + 1);
            }

            // Print the name and combine it with the prefix
            GfxPrint_Printf(printer, "%s%s", prefix, gMapSelectSceneList[printScene].name);
        }
    };
}

void MapSelect_DrawMenu(MapSelectContext* this, GfxPrint* printer) {
    OPEN_DISPS(this->state.gfxCtx, "", 0);

    // Clear attributes before drawing
    func_80094140(this->state.gfxCtx);

    // Print "Map Select" title
    INIT_PRINTER(printer, MAP_SELECT_TITLE_X, MAP_SELECT_TITLE_Y, 255, 255, 255);
    GfxPrint_Printf(printer, "MAP SELECT");

    // Print Z left title
    // GfxPrint_SetPos(printer, MAP_SELECT_Z_LEFT_X, MAP_SELECT_Z_LEFT_Y);
    // GfxPrint_Printf(printer, "<Z");

    // Print R right title
    // GfxPrint_SetPos(printer, MAP_SELECT_R_RIGHT_X + 1, MAP_SELECT_R_RIGHT_Y);
    // GfxPrint_Printf(printer, "R>");

    // Print scene names
    MapSelect_PrintScenes(this, printer);

    // Print the scene setup
    INIT_PRINTER(printer, MAP_SELECT_SCENE_SETUP_X, MAP_SELECT_SCENE_SETUP_Y, 255, 255, 255);
    GfxPrint_Printf(printer, "Setup:%d", this->sceneSetup);

    // Print the spawn index
    INIT_PRINTER(printer, MAP_SELECT_SPAWN_X, MAP_SELECT_SPAWN_Y, 255, 255, 255);
    GfxPrint_Printf(printer, "Spawn:%d", this->sceneSpawn);

    // Draw the 3Q logo
    Gfx_ClearOverlayAttributes(this->state.gfxCtx);
    OVERLAY_DISP = Gfx_TextureRectangleI8(OVERLAY_DISP, gMapSelect3QLogo, MAP_SELECT_3Q_LOGO_WIDTH,
                                          MAP_SELECT_3Q_LOGO_HEIGHT, MAP_SELECT_3Q_LOGO_X, MAP_SELECT_3Q_LOGO_Y,
                                          MAP_SELECT_3Q_LOGO_WIDTH, MAP_SELECT_3Q_LOGO_HEIGHT, 1024, 1024);

    // Draw divider line
    gSPDisplayList(OVERLAY_DISP++, gSetupFillRect);
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, 90, 90, 90, 255);
    gDPFillRectangle(OVERLAY_DISP++, 0, DIVIDER_LINE_Y, 320, DIVIDER_LINE_Y + 1);

    CLOSE_DISPS(this->state.gfxCtx, "", 0);
}

void MapSelect_Draw(MapSelectContext* this) {
    GfxPrint* printer;

    OPEN_DISPS(this->state.gfxCtx, "", 0);

    // Fill the background black
    gSPSegment(POLY_OPA_DISP++, 0x00, NULL);
    func_80095248(this->state.gfxCtx, 0, 0, 0);
    SET_FULLSCREEN_VIEWPORT(&this->view);
    func_800AAA50(&this->view, 0xF);

    // Set segment 1 to the static segment
    gSPSegment(POLY_OPA_DISP++, 1, this->staticSegment);

    // Initialize the printer
    printer = alloca(sizeof(GfxPrint));
    GfxPrint_Init(printer);
    GfxPrint_Open(printer, POLY_OPA_DISP);

    if (this->state.running) {
        MapSelect_DrawMenu(this, printer);
    }

    // Close the printer
    POLY_OPA_DISP = GfxPrint_Close(printer);
    GfxPrint_Destroy(printer);

    CLOSE_DISPS(this->state.gfxCtx, "", 0);
}

void MapSelect_Main(GameState* thisx) {
    MapSelectContext* this = (MapSelectContext*)thisx;

    MapSelect_Update(this);
    MapSelect_Draw(this);
}

void MapSelect_Destroy(GameState* thisx) {
}

void MapSelect_Init(GameState* thisx) {
    MapSelectContext* this = (MapSelectContext*)thisx;
    u32 size;

    // Set main/destroy
    this->state.main = MapSelect_Main;
    this->state.destroy = MapSelect_Destroy;

    // DMA copy the static segment
    size = (u32)_map_select_staticSegmentRomEnd - (u32)_map_select_staticSegmentRomStart;
    this->staticSegment = GameState_Alloc(&this->state, size, "", 0);

    if (this->staticSegment != NULL) {
        DmaMgr_SendRequest1(this->staticSegment, (u32)_map_select_staticSegmentRomStart, size, "", 0);
    }

    // Initiliaze the view
    View_Init(&this->view, this->state.gfxCtx);
    this->view.flags = 2 | 8;

    // Initialize variables
    this->sceneCount = ARRAY_COUNT(gMapSelectSceneList);

    if ((dREG(80) >= 0) && (dREG(80) < this->sceneCount)) {
        this->curScene = dREG(80);
    } else {
        this->curScene = 0;
    }

    this->timer = BUTTON_HOLD_WAIT;
    this->sceneSetup = this->sceneSpawn = 0;

    R_UPDATE_RATE = 1;
}
