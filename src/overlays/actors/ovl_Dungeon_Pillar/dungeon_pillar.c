/**
 * @file dungeon_pillar.c
 * @author zel.
 * @brief Rising and falling pillar used in dungeons
 * @date 2021-12-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "dungeon_pillar.h"
#include "custom/objects/object_earth_labyrinth/object_earth_labyrinth.h"
#include "custom/objects/object_snakes_remains/object_snakes_remains.h"

#define FLAGS 0x00000010

#define THIS ((DungeonPillar*)thisx)

typedef enum {
    PILLAR_WAIT_FOR_DEST_POS,
    PILLAR_MOVE_TO_DEST_POS,
    PILLAR_WAIT_FOR_INIT_POS,
    PILLAR_MOVE_TO_INIT_POS,
    PILLAR_WAIT_FOR_PLAYER
} DungeonPillarActionState;

void DungeonPillar_Init(Actor* thisx, GlobalContext* globalCtx);
void DungeonPillar_Destroy(Actor* thisx, GlobalContext* globalCtx);
void DungeonPillar_Update(Actor* thisx, GlobalContext* globalCtx);
void DungeonPillar_Draw(Actor* thisx, GlobalContext* globalCtx);

void DungeonPillar_WaitForObject(DungeonPillar* this, GlobalContext* globalCtx);
void DungeonPillar_AutoRiseFall(DungeonPillar* this, GlobalContext* globalCtx);
void DungeonPillar_PlayerRiseFall(DungeonPillar* this, GlobalContext* globalCtx);

#include "dungeon_pillar_collision.c"

extern CollisionHeader dungeon_pillar_collision_collisionHeader;

const ActorInit Dungeon_Pillar_InitVars = {
    ACTOR_DUNGEON_PILLAR,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(DungeonPillar),
    (ActorFunc)DungeonPillar_Init,
    (ActorFunc)DungeonPillar_Destroy,
    (ActorFunc)DungeonPillar_Update,
    NULL,
};

typedef struct {
    u16 object;
    Gfx* dList;
} DungeonPillarSkinInfo;

// { object file, displaylist }
static DungeonPillarSkinInfo sPillarSkins[] = {
    { OBJECT_DUN_1, gPillarDun1DL },
    { OBJECT_DUN_2, gPillarDun2DL },
    { OBJECT_DUN_3, gPillarDun3DL },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_F32(uncullZoneForward, 6000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 2000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 4000, ICHAIN_STOP),
};

void DungeonPillar_Init(Actor* thisx, GlobalContext* globalCtx) {
    DungeonPillar* this = THIS;
    CollisionHeader* colHeader;

    // Set the culling distances
    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);

    // Read the actor spawn params
    this->skin = DP_PARAMS_TO_SKIN(this->dyna.actor.params);
    this->initDir = DP_PARAMS_TO_INIT_DIR(this->dyna.actor.params);
    this->speed = DP_PARAMS_TO_SPEED(this->dyna.actor.params);
    this->reverseFlag = DP_PARAMS_TO_REVERSE_FLAG(this->dyna.actor.params);
    this->playerCheckFlag = DP_PARAMS_TO_PLAYER_CHECK_FLAG(this->dyna.actor.params);
    this->switchFlag = DP_PARAMS_TO_SWITCH_FLAG(this->dyna.actor.params);
    Actor_SetScale(&this->dyna.actor, 10.0f * DP_X_ROT_TO_SCALE(this->dyna.actor.world.rot.x));
    this->waitFrames = DP_X_ROT_TO_WAIT_FRAMES(this->dyna.actor.world.rot.x);
    this->height = DP_Z_ROT_TO_HEIGHT(this->dyna.actor.world.rot.z);

    // Clear the x and z rotations now that they have been read
    this->dyna.actor.world.rot.x = this->dyna.actor.world.rot.z = this->dyna.actor.shape.rot.x =
        this->dyna.actor.shape.rot.z = 0;

    // Find the initial target position
    this->targetY = this->dyna.actor.home.pos.y;
    if (this->initDir == 0) {
        this->targetY += this->height;
    } else {
        this->targetY -= this->height;
    }

    // Flip the pillar if the reverse flag is true
    if (this->reverseFlag) {
        this->dyna.actor.world.rot.z = BINANG_ROT180(this->dyna.actor.world.rot.z);
    }

    // Initialize the dyna flags
    DynaPolyActor_Init(&this->dyna, DPM_PLAYER);

    // Load the collision
    CollisionHeader_GetVirtual(&dungeon_pillar_collision_collisionHeader, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &this->dyna.actor, colHeader);

    // Set the object
    this->bankIndex = Object_GetIndex(&globalCtx->objectCtx, sPillarSkins[this->skin].object);

    // Set the initial action
    if (this->bankIndex < 0) {
        Actor_Kill(&this->dyna.actor);
    } else {
        this->actionFunc = DungeonPillar_WaitForObject;
    }

    this->timer = 0;
}

void DungeonPillar_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    DungeonPillar* this = THIS;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
}

void DungeonPillar_WaitForObject(DungeonPillar* this, GlobalContext* globalCtx) {
    // Wait until the object is loaded to begin drawing or updating
    if (Object_IsLoaded(&globalCtx->objectCtx, this->bankIndex)) {
        this->dyna.actor.objBankIndex = this->bankIndex;
        Actor_SetObjectDependency(globalCtx, &this->dyna.actor);

        // Set the action and draw
        this->dyna.actor.draw = (ActorFunc)DungeonPillar_Draw;
        if (!this->playerCheckFlag) {
            this->actionState = PILLAR_WAIT_FOR_DEST_POS;
            this->actionFunc = DungeonPillar_AutoRiseFall;
        } else {
            this->actionState = PILLAR_WAIT_FOR_PLAYER;
            this->actionFunc = DungeonPillar_PlayerRiseFall;
        }
    }
}

/**
 * Automatically rises and falls based on a timer
 */
void DungeonPillar_AutoRiseFall(DungeonPillar* this, GlobalContext* globalCtx) {
    // If switch flag is set, activate the pillar!
    if (Flags_GetSwitch(globalCtx, this->switchFlag)) {
        switch (this->actionState) {
            // Wait to move to the destination
            case PILLAR_WAIT_FOR_DEST_POS:
                this->timer++;

                if (this->timer >= this->waitFrames) {
                    this->timer = 0;
                    this->actionState = PILLAR_MOVE_TO_DEST_POS;
                }
                break;

            // Move to the destination
            case PILLAR_MOVE_TO_DEST_POS:
                // Move Y position towards the destination
                Math_StepToF(&this->dyna.actor.world.pos.y, this->targetY, this->speed);

                if ((s32)this->dyna.actor.world.pos.y == (s32)this->targetY) {
                    // Play the pillar stopping sound
                    Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_PILLAR_MOVE_STOP);

                    this->actionState = PILLAR_WAIT_FOR_INIT_POS;
                }
                break;

            // Wait to return to the initial position
            case PILLAR_WAIT_FOR_INIT_POS:
                this->timer++;

                if (this->timer >= this->waitFrames) {
                    this->timer = 0;
                    this->actionState = PILLAR_MOVE_TO_INIT_POS;
                }
                break;

            // Return to the initial position
            case PILLAR_MOVE_TO_INIT_POS:
                // Move Y position towards the initial position
                Math_StepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, this->speed);

                if ((s32)this->dyna.actor.world.pos.y == (s32)this->dyna.actor.home.pos.y) {
                    // Play pillar stopping sound
                    Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_PILLAR_MOVE_STOP);

                    this->actionState = PILLAR_WAIT_FOR_DEST_POS;
                }
                break;
        }
    }
}

/**
 * Rises/falls when the player steps on the pillar, rather than doing so based on a timer
 */
void DungeonPillar_PlayerRiseFall(DungeonPillar* this, GlobalContext* globalCtx) {
    s32 playerOnPillar = func_8004356C(&this->dyna);

    // If switch flag is set, activate the pillar!
    if (Flags_GetSwitch(globalCtx, this->switchFlag)) {
        switch (this->actionState) {
            // Wait for the player to stand on the pillar
            case PILLAR_WAIT_FOR_PLAYER:
                if (playerOnPillar) {
                    this->actionState = PILLAR_MOVE_TO_DEST_POS;
                }
                break;

            // Move to the destination
            case PILLAR_MOVE_TO_DEST_POS:
                // Move Y position towards the destination
                Math_StepToF(&this->dyna.actor.world.pos.y, this->targetY, this->speed);

                if ((s32)this->dyna.actor.world.pos.y == (s32)this->targetY) {
                    // Play the pillar stopping sound
                    Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_PILLAR_MOVE_STOP);
                    this->actionState = PILLAR_WAIT_FOR_INIT_POS;
                }
                break;

            // Wait to return to the initial position
            case PILLAR_WAIT_FOR_INIT_POS:
                if (!playerOnPillar) {
                    this->actionState = PILLAR_MOVE_TO_INIT_POS;
                }
                break;

            // Return to the initial position
            case PILLAR_MOVE_TO_INIT_POS:
                // Move Y position towards the initial position
                Math_StepToF(&this->dyna.actor.world.pos.y, this->dyna.actor.home.pos.y, this->speed);

                if ((s32)this->dyna.actor.world.pos.y == (s32)this->dyna.actor.home.pos.y) {
                    // Play the pillar stopping sound
                    Audio_PlayActorSound2(&this->dyna.actor, NA_SE_EV_PILLAR_MOVE_STOP);
                    this->actionState = PILLAR_WAIT_FOR_DEST_POS;
                }
                break;

            // Wait to move to the destination
            case PILLAR_WAIT_FOR_DEST_POS:
                if (!playerOnPillar) {
                    this->actionState = PILLAR_WAIT_FOR_PLAYER;
                }
                break;
        }
    }
}

void DungeonPillar_Update(Actor* thisx, GlobalContext* globalCtx) {
    DungeonPillar* this = THIS;

    this->actionFunc(this, globalCtx);
}

void DungeonPillar_Draw(Actor* thisx, GlobalContext* globalCtx) {
    DungeonPillar* this = THIS;

    // Draw pillar
    Gfx_DrawDListOpa(globalCtx, sPillarSkins[this->skin].dList);
}
