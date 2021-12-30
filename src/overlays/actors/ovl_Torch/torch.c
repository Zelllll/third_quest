/**
 * @file torch.c
 * @author zel.
 * @brief Torch
 * @date 2021-12-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "torch.h"
#include "overlays/actors/ovl_En_Arrow/z_en_arrow.h"
#include "custom/objects/gameplay_keep/torch_mesh.h"

#define FLAGS 0x00000410

#define THIS ((Torch*)thisx)

void Torch_Init(Actor* thisx, GlobalContext* globalCtx);
void Torch_Destroy(Actor* thisx, GlobalContext* globalCtx);
void Torch_Update(Actor* thisx, GlobalContext* globalCtx);
void Torch_Draw(Actor* thisx, GlobalContext* globalCtx);

const ActorInit Torch_InitVars = {
    ACTOR_TORCH,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(Torch),
    (ActorFunc)Torch_Init,
    (ActorFunc)Torch_Destroy,
    (ActorFunc)Torch_Update,
    (ActorFunc)Torch_Draw,
};

static ColliderCylinderInit sCylInitStand = {
    {
        COLTYPE_METAL,
        AT_NONE,
        AC_ON | AC_HARD | AC_TYPE_PLAYER,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK2,
        { 0x00100000, 0x00, 0x00 },
        { 0xEE01FFFF, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON | BUMP_HOOKABLE,
        OCELEM_ON,
    },
    { 12, 45, 0, { 0, 0, 0 } },
};

static ColliderCylinderInit sCylInitFlame = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_ON | AC_TYPE_PLAYER,
        OC1_NONE,
        OC2_NONE,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK2,
        { 0x00000000, 0x00, 0x00 },
        { 0x00020820, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_ON,
        OCELEM_NONE,
    },
    { 15, 45, 45, { 0, 0, 0 } },
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 1000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneForward, 4000, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneScale, 800, ICHAIN_CONTINUE),
    ICHAIN_F32(uncullZoneDownward, 800, ICHAIN_STOP),
};

static s32 sLitTorchCount;

void Torch_Init(Actor* thisx, GlobalContext* globalCtx) {
    static u8 standColType[] = { COLTYPE_METAL, COLTYPE_WOOD, COLTYPE_WOOD };
    Torch* this = THIS;

    // Imitialize culling distances and scale
    Actor_ProcessInitChain(&this->actor, sInitChain);

    // Initialize actor shape
    ActorShape_Init(&this->actor.shape, 0.0f, NULL, 0.0f);

    // Get values from the spawn params
    this->torchType = TORCH_PARAMS_TO_TYPE(this->actor.params);
    this->cutsceneFlag = TORCH_PARAMS_TO_CUTSCENE_FLAG(this->actor.params);
    this->litFlag = TORCH_PARAMS_TO_LIT_FLAG(this->actor.params);
    this->puzzleTorchCount = TORCH_PARAMS_TO_PUZZLE_TORCH_COUNT(this->actor.params);
    this->switchFlag = TORCH_PARAMS_TO_PUZZLE_SWITCH_FLAG(this->actor.params);

    // Initialize collider for the torch stand
    Collider_InitCylinder(globalCtx, &this->colliderStand);
    Collider_SetCylinder(globalCtx, &this->colliderStand, &this->actor, &sCylInitStand);
    this->colliderStand.base.colType = standColType[this->torchType];

    // Intialize collider for the flame part of the torch
    Collider_InitCylinder(globalCtx, &this->colliderFlame);
    Collider_SetCylinder(globalCtx, &this->colliderFlame, &this->actor, &sCylInitFlame);

    // Disable the player from being able to push the torch around
    this->actor.colChkInfo.mass = MASS_IMMOVABLE;

    // Set the focus point
    Actor_SetFocus(&this->actor, 60.0f);

    // Set the timer amount
    if (this->litFlag || ((this->torchType != TORCH_WOODEN) && Flags_GetSwitch(globalCtx, this->switchFlag))) {
        this->litTimer = -1;
    }

    // Set up the color animation
    this->flameColorFrame = (s32)(Rand_ZeroOne() * 40.0f);

    this->prim.r = 255;
    this->prim.g = 249;
    this->prim.b = 187;
    this->prim.a = 255;

    this->env.r = 255;
    this->env.g = 114;
    this->env.b = 0;
    this->env.a = 255;

    // Add a light for the flame
    Lights_PointNoGlowSetInfo(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y + 70.0f,
                              this->actor.world.pos.z, 255, 249, 187, -1);
    this->lightNode = LightContext_InsertLight(globalCtx, &globalCtx->lightCtx, &this->lightInfo);

    sLitTorchCount = 0;
}

void Torch_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    Torch* this = THIS;

    // Destroy both of the torch's colliders
    Collider_DestroyCylinder(globalCtx, &this->colliderStand);
    Collider_DestroyCylinder(globalCtx, &this->colliderFlame);

    // Destroy the flame light
    LightContext_RemoveLight(globalCtx, &globalCtx->lightCtx, this->lightNode);
}

typedef struct {
    u16 startFrame;
    u16 endFrame;
    u8 color;
} ColorKeyFrame;

static ColorKeyFrame sFlameGreenKeyFrames[] = {
    { 0, 5, 160 },  { 5, 12, 114 },  { 12, 14, 136 }, { 14, 16, 96 },  { 16, 18, 124 },
    { 18, 25, 94 }, { 25, 30, 132 }, { 30, 35, 115 }, { 35, 40, 146 },
};

static ColorKeyFrame sFlameAlphaKeyFrames[] = {
    { 0, 5, 255 },   { 5, 12, 51 },   { 12, 14, 255 }, { 14, 16, 153 }, { 16, 18, 255 },
    { 18, 25, 179 }, { 25, 31, 204 }, { 31, 35, 102 }, { 35, 40, 255 },
};

/**
 * LERP formula, returns a u8 since it is intended to be used with color values
 */
u8 Torch_Lerp(f32 norm, f32 min, f32 max) {
    return (u8)(((max - min) * norm) + min);
}

/**
 * Gets the color value in a list of key frames with the given frame value
 */
u8 Torch_GetFrameColor(ColorKeyFrame* keyFrames, s16 entryCount, s16 colorFrame) {
    ColorKeyFrame* entry;
    s32 i;

    for (i = 0; i < entryCount; i++) {
        entry = &keyFrames[i];

        if ((colorFrame >= entry->startFrame) && (colorFrame < entry->endFrame || entry->endFrame == 40)) {
            register s16 totalFrames = entry->endFrame - entry->startFrame;
            register s16 relativeFrames = colorFrame - entry->startFrame;
            register f32 norm = (f32)relativeFrames / totalFrames;
            register f32 minCol = entry->color;
            register f32 maxCol = (entry->endFrame == 40) ? entry->color : entry[1].color;

            return Torch_Lerp(norm, minCol, maxCol);
        }
    }
}

/**
 * Updates the color animation for env green and prim alpha
 */
void Torch_UpdateColorAnimation(Torch* this) {
    // LERP env green
    this->env.g = Torch_GetFrameColor(sFlameGreenKeyFrames, ARRAY_COUNT(sFlameGreenKeyFrames), this->flameColorFrame);

    // LERP prim alpha
    this->prim.a = Torch_GetFrameColor(sFlameAlphaKeyFrames, ARRAY_COUNT(sFlameAlphaKeyFrames), this->flameColorFrame);

    // Increment the key frame
    this->flameColorFrame++;
    if (this->flameColorFrame >= 40) {
        this->flameColorFrame = 0;
    }
}

/**
 * Updates interactions with the torch
 */
void Torch_UpdateInteractions(Torch* this, GlobalContext* globalCtx) {
    s32 litTimeScale = this->puzzleTorchCount;
    WaterBox* dummy;
    f32 waterSurface;
    s32 lightRadius = -1;
    u8 brightness = 0;
    Player* player;
    EnArrow* arrow;
    s32 interactionType;
    u32 dmgFlags;
    Vec3f tipToFlame;

    // Check if the torch has become submerged in water, if so unlight it
    if (WaterBox_GetSurfaceImpl(globalCtx, &globalCtx->colCtx, this->actor.world.pos.x, this->actor.world.pos.z,
                                &waterSurface, &dummy) &&
        ((waterSurface - this->actor.world.pos.y) > 52.0f)) {
        this->litTimer = 0;

        if (this->torchType == TORCH_TIMED) {
            Flags_UnsetSwitch(globalCtx, this->switchFlag);

            if (this->puzzleTorchCount != 0) {
                this->litTimer = 1;
            }
        }
    } else {
        player = GET_PLAYER(globalCtx);
        interactionType = 0;

        if (this->litFlag) {
            this->litTimer = -1;
        }

        if (this->puzzleTorchCount != 0) {
            if (Flags_GetSwitch(globalCtx, this->switchFlag)) {
                if (this->litTimer == 0) {
                    this->litTimer = -1;
                    if (this->torchType == TORCH_TRIGGER && this->cutsceneFlag) {
                        OnePointCutscene_Attention(globalCtx, &this->actor);
                    }
                } else if (this->litTimer > 0) {
                    this->litTimer = -1;
                }
            } else if (this->litTimer < 0) {
                this->litTimer = 20;
            }
        }

        // Check if an arrow or stick collides with the torch flame collider
        if (this->colliderFlame.base.acFlags & AC_HIT) {
            dmgFlags = this->colliderFlame.info.acHitInfo->toucher.dmgFlags;
            if (dmgFlags & 0x20820) {
                interactionType = 1;
            }
        } else if (player->heldItemActionParam == 6) {
            Math_Vec3f_Diff(&player->swordInfo[0].tip, &this->actor.world.pos, &tipToFlame);
            tipToFlame.y -= 67.0f;

            if ((SQ(tipToFlame.x) + SQ(tipToFlame.y) + SQ(tipToFlame.z)) < SQ(20.0f)) {
                interactionType = -1;
            }
        }

        if (interactionType != 0) {
            if (this->litTimer != 0) {
                if (interactionType < 0) {
                    if (player->unk_860 == 0) {
                        player->unk_860 = 210;
                        Audio_PlaySoundGeneral(NA_SE_EV_FLAME_IGNITION, &this->actor.projectedPos, 4, &D_801333E0,
                                               &D_801333E0, &D_801333E8);
                    } else if (player->unk_860 < 200) {
                        player->unk_860 = 200;
                    }
                } else if (dmgFlags & 0x20) {
                    arrow = (EnArrow*)this->colliderFlame.base.ac;
                    if ((arrow->actor.update != NULL) && (arrow->actor.id == ACTOR_EN_ARROW)) {
                        arrow->actor.params = 0;
                        arrow->collider.info.toucher.dmgFlags = 0x800;
                    }
                }
                if ((0 <= this->litTimer) && (this->litTimer < (50 * litTimeScale + 100)) &&
                    (this->torchType != TORCH_TRIGGER)) {
                    this->litTimer = 50 * litTimeScale + 100;
                }
            } else if ((this->torchType != TORCH_TRIGGER) && (((interactionType > 0) && (dmgFlags & 0x20800)) ||
                                                              ((interactionType < 0) && (player->unk_860 != 0)))) {

                if ((interactionType < 0) && (player->unk_860 < 200)) {
                    player->unk_860 = 200;
                }
                if (this->puzzleTorchCount == 0) {
                    this->litTimer = -1;
                    // if (this->torchType != TORCH_WOODEN) {
                    Flags_SetSwitch(globalCtx, this->switchFlag);
                    if (this->cutsceneFlag) {
                        OnePointCutscene_Attention(globalCtx, &this->actor);
                    }
                    // }
                } else {
                    sLitTorchCount++;
                    if (sLitTorchCount >= this->puzzleTorchCount) {
                        Flags_SetSwitch(globalCtx, this->switchFlag);
                        if (this->cutsceneFlag) {
                            OnePointCutscene_Attention(globalCtx, &this->actor);
                        }
                        this->litTimer = -1;
                    } else {
                        this->litTimer = (litTimeScale * 50) + 110;
                    }
                }

                // Play torch lighting SFX
                Audio_PlaySoundGeneral(NA_SE_EV_FLAME_IGNITION, &this->actor.projectedPos, 4, &D_801333E0, &D_801333E0,
                                       &D_801333E8);
            }
        }
    }
}

void Torch_Update(Actor* thisx, GlobalContext* globalCtx) {
    Torch* this = THIS;
    s32 lightRadius = -1;
    f32 colorNorm = 0;

    // Check for interactions
    Torch_UpdateInteractions(this, globalCtx);

    // Update the stand collider
    Collider_UpdateCylinder(&this->actor, &this->colliderStand);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &this->colliderStand.base);
    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &this->colliderStand.base);

    // Update the flame collider
    Collider_UpdateCylinder(&this->actor, &this->colliderFlame);
    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &this->colliderFlame.base);

    // Animate the colors
    Torch_UpdateColorAnimation(this);

    // Update the flame lit timer for torches that do not stay lit
    if (this->litTimer > 0) {
        this->litTimer--;
        if ((this->litTimer == 0) && (this->torchType != TORCH_TRIGGER)) {
            sLitTorchCount--;
        }
    }

    if (this->litTimer != 0) {
        // Set the radius of the light based on the
        if ((this->litTimer < 0) || (this->litTimer >= 20)) {
            lightRadius = 200;
        } else {
            lightRadius = (this->litTimer * 200.0f) / 20.0f;
        }

        // Set the color of the light
        colorNorm = Rand_ZeroOne();

        // Play torch crackling SFX
        func_8002F974(&this->actor, NA_SE_EV_TORCH - SFX_FLAG);
    }

    // Update the flame light
    Lights_PointSetColorAndRadius(&this->lightInfo, 128.0f + (colorNorm * 127.0f), 128.0f + (colorNorm * 127.0f), 0,
                                  lightRadius);
}

/**
 * Draws the flame part of the torch
 */
void Torch_DrawFlame(Torch* this, GlobalContext* globalCtx) {
    s32 timerMax = (this->puzzleTorchCount * 50) + 100;
    f32 flameScale = 1.0f;

    OPEN_DISPS(globalCtx->state.gfxCtx, "", 0);

    // Determine the scale to use for the flame
    if (this->litTimer > timerMax) {
        flameScale = (timerMax - this->litTimer + 10) / 10.0f;
    } else if ((this->litTimer > 0) && (this->litTimer < 20)) {
        flameScale = this->litTimer / 20.0f;
    }
    flameScale *= 0.0030f;

    // Translate the flame to be above the torch
    Matrix_Translate(0.0f, 52.0f, 0.0f, MTXMODE_APPLY);

    // Rotate the flame to face towards the camera (billboard)
    Matrix_RotateY((s16)(Camera_GetCamDirYaw(GET_ACTIVE_CAM(globalCtx)) - this->actor.shape.rot.y + 0x8000) * (M_PI / 0x8000),
                   MTXMODE_APPLY);

    // Scale the flame
    Matrix_Scale(flameScale, flameScale, flameScale, MTXMODE_APPLY);

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "", 0),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    // Generate and draw a flame model
    FlameGenerator_DrawFlame(&globalCtx->state, this->prim.r, this->prim.g, this->prim.b, this->prim.a, this->env.r,
                             this->env.g, this->env.b, this->env.a);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "", 0);
}

void Torch_Draw(Actor* thisx, GlobalContext* globalCtx) {
    static Gfx* torchDLists[] = { gTriggerTorchDL, gTimerTorchDL, gWoodTorchDL };
    Torch* this = THIS;

    // Draw the torch stand
    Gfx_DrawDListOpa(globalCtx, torchDLists[this->torchType]);

    // Draw the flame
    if (this->litTimer != 0) {
        Torch_DrawFlame(this, globalCtx);
    }
}
