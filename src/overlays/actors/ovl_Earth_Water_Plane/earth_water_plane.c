/**
 * @file earth_water_plane.c
 * @author zel.
 * @brief Water plane used in the Earth Labyrinth
 * @date 2021-12-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "earth_water_plane.h"
#include "custom/objects/object_earth_labyrinth/object_earth_labyrinth.h"

#define FLAGS 0x00000010

#define THIS ((EarthWaterPlane*)thisx)

#define WATER_SWITCH_FLAG ((this->actor.params & 0xFF00) >> 8)
#define WATER_DRAW_ENABLED (this->actor.params & 0x00FF)
#define WATER_ENTRANCE_Z_BOUNDS -236 // 840
#define WATER_POS_ENTRANCE -1158     // 300
#define WATER_POS_CENTRAL -3713      // -2250
#define WATER_POS_RAISED -3180       // -1720
#define WATER_POS_HIDDEN -32000

void EarthWaterPlane_Init(Actor* thisx, GlobalContext* globalCtx);
void EarthWaterPlane_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EarthWaterPlane_Update(Actor* thisx, GlobalContext* globalCtx);
void EarthWaterPlane_Draw(Actor* thisx, GlobalContext* globalCtx);

const ActorInit Earth_Water_Plane_InitVars = {
    ACTOR_EARTH_WATER_PLANE,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_EARTH_LABYRINTH,
    sizeof(EarthWaterPlane),
    (ActorFunc)EarthWaterPlane_Init,
    (ActorFunc)EarthWaterPlane_Destroy,
    (ActorFunc)EarthWaterPlane_Update,
    (ActorFunc)EarthWaterPlane_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 12000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 8000, ICHAIN_STOP),
};

void EarthWaterPlane_SetWaterboxSurface(GlobalContext* globalCtx, s16 ySurface) {
    WaterBox* waterBox = &globalCtx->colCtx.colHeader->waterBoxes[0];
    waterBox->ySurface = ySurface;
}

s32 EarthWaterPlane_FindNewPosition(EarthWaterPlane* this, GlobalContext* globalCtx, s16 room) {
    Player* player = GET_PLAYER(globalCtx);

    if (Flags_GetSwitch(globalCtx, this->switchFlag)) {
        return WATER_POS_RAISED;
    }

    if (room == 0) {
        // Entrance room
        if ((s32)player->actor.world.pos.z < WATER_ENTRANCE_Z_BOUNDS) {
            // When the player is descending the stairs
            return WATER_POS_HIDDEN;
        }
        return WATER_POS_ENTRANCE;
    } else if (room == 3) {
        // Central room
        return WATER_POS_CENTRAL;
    }

    return WATER_POS_HIDDEN;
}

void EarthWaterPlane_UpdateWaterPos(EarthWaterPlane* this, GlobalContext* globalCtx) {
    s32 newYSurface;

    // Update the initial position of the water
    newYSurface = EarthWaterPlane_FindNewPosition(this, globalCtx, this->actor.room);
    EarthWaterPlane_SetWaterboxSurface(globalCtx, newYSurface);

    // Don't update the y position in the entrance room since the water plane lowers when the player is on the steps,
    // but the actor should not.
    if (!((this->actor.room == 0) && (newYSurface == WATER_POS_HIDDEN))) {
        this->actor.world.pos.y = newYSurface;
    }
}

void EarthWaterPlane_Init(Actor* thisx, GlobalContext* globalCtx) {
    EarthWaterPlane* this = THIS;
    s32 newYSurface;

    // Set the culling distances
    Actor_ProcessInitChain(&this->actor, sInitChain);

    // Set the scale of the water
    Actor_SetScale(&this->actor, 10.0f);

    // Get the switch flag from the params
    this->switchFlag = WATER_SWITCH_FLAG;

    // Disable drawing for rooms where the water does not need to be rendered
    if (!(WATER_DRAW_ENABLED)) {
        this->actor.draw = NULL;
    }

    // Update the initial position of the water
    EarthWaterPlane_UpdateWaterPos(this, globalCtx);
}

void EarthWaterPlane_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EarthWaterPlane* this = THIS;
}

void EarthWaterPlane_Update(Actor* thisx, GlobalContext* globalCtx) {
    EarthWaterPlane* this = THIS;

    if (this->actor.room == 0 || this->actor.room == 3) {
        EarthWaterPlane_UpdateWaterPos(this, globalCtx);
    }
}

void EarthWaterPlane_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EarthWaterPlane* this = THIS;
    s32 gameplayFrames = globalCtx->state.frames;

    OPEN_DISPS(globalCtx->state.gfxCtx, "", 0);

    func_80093D84(globalCtx->state.gfxCtx);

    gSPSegment(POLY_XLU_DISP++, 0x08,
               Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, -gameplayFrames, gameplayFrames, 32, 32, 1, 0,
                                -gameplayFrames, 32, 32));

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "", 0),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    gSPDisplayList(POLY_XLU_DISP++, gEarthLabyrinthWaterDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "", 0);
}
