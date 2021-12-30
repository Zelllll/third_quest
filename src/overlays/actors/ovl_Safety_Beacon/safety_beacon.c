/**
 * @file safety_beacon.c
 * @author zel.
 * @brief UNFINISHED
 * @date 2021-12-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "safety_beacon.h"

#define FLAGS 0x00000010

#define THIS ((SafetyBeacon*)thisx)

#define SB_GLOW_INTENSITY_MIN 0
#define SB_GLOW_INTENSITY_MAX 160
#define SB_OCARINA_ACTIVATE_RANGE 75
#define SB_SECONDS_MAX 15
#define SB_FRAMES_MAX ((60 / R_UPDATE_RATE) * SB_SECONDS_MAX) // don't touch

void SafetyBeacon_Init(Actor* thisx, GlobalContext* globalCtx);
void SafetyBeacon_Destroy(Actor* thisx, GlobalContext* globalCtx);
void SafetyBeacon_Update(Actor* thisx, GlobalContext* globalCtx);
void SafetyBeacon_Draw(Actor* thisx, GlobalContext* globalCtx);

void SafetyBeacon_SetupProtectAnim(SafetyBeacon* this);
void SafetyBeacon_ProtectAnim(SafetyBeacon* this, GlobalContext* globalCtx);

extern Gfx safety_beacon_base_shape[];
extern Gfx safety_beacon_crystal_inner_shape[];
extern Gfx safety_beacon_crystal_outer_shape[];
extern Gfx safety_beacon_sphere_shape[];

const ActorInit Safety_Beacon_InitVars = {
    ACTOR_SAFETY_BEACON,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_SAFETY_BEACON,
    sizeof(SafetyBeacon),
    (ActorFunc)SafetyBeacon_Init,
    (ActorFunc)SafetyBeacon_Destroy,
    (ActorFunc)SafetyBeacon_Update,
    (ActorFunc)SafetyBeacon_Draw,
};

static ColliderCylinderInit sCylinderInit = {
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
        BUMP_ON,
        OCELEM_ON,
    },
    { 12, 50, 0, { 0, 0, 0 } },
};

void SafetyBeacon_Init(Actor* thisx, GlobalContext* globalCtx) {
    SafetyBeacon* this = THIS;

    // Initialize collider for the beacon's base
    Collider_InitCylinder(globalCtx, &this->collider);
    Collider_SetCylinder(globalCtx, &this->collider, &this->actor, &sCylinderInit);

#if 0
    Lights_PointGlowSetInfo(&this->lightInfo, this->actor.world.pos.x, this->actor.world.pos.y + 60.0f,
                           this->actor.world.pos.z, 0, 0, 255, -1);
    this->lightNode = LightContext_InsertLight(globalCtx, &globalCtx->lightCtx, &this->lightInfo);
#endif

    SafetyBeacon_SetupProtectAnim(this);
}

void SafetyBeacon_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    SafetyBeacon* this = THIS;

    // Destroy collider for the beacon's base
    Collider_DestroyCylinder(globalCtx, &this->collider);

#if 0
    LightContext_RemoveLight(globalCtx, &globalCtx->lightCtx, this->lightNode);
#endif
}

void SafetyBeacon_SetupProtectAnim(SafetyBeacon* this) {
    this->actionFunc = SafetyBeacon_ProtectAnim;
    this->sphereRot = 0.0f;
}

void SafetyBeacon_ProtectAnim(SafetyBeacon* this, GlobalContext* globalCtx) {
}

void SafetyBeacon_Update(Actor* thisx, GlobalContext* globalCtx) {
    SafetyBeacon* this = THIS;

    // Enable collision checking every frame
    Collider_UpdateCylinder(&this->actor, &this->collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
    CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &this->collider.base);
}

void SafetyBeacon_Draw(Actor* thisx, GlobalContext* globalCtx) {
    SafetyBeacon* this = THIS;

    OPEN_DISPS(globalCtx->state.gfxCtx, "", 0);

    // Clear attributes on OPA before drawing
    func_80093D18(globalCtx->state.gfxCtx);

    // Set color for the glowing base; the prim red color controls the intensity of the glow
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, SB_GLOW_INTENSITY_MAX, 0, 0, 255);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 0, 255);

    // Draw base and inner crystal
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "", 0), G_MTX_MODELVIEW | G_MTX_LOAD);
    gSPDisplayList(POLY_OPA_DISP++, safety_beacon_base_shape);
    gSPDisplayList(POLY_OPA_DISP++, safety_beacon_crystal_inner_shape);

    // Clear attributes on XLU before drawing
    func_80093D84(globalCtx->state.gfxCtx);

    // Draw outer crystal
    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "", 0), G_MTX_MODELVIEW | G_MTX_LOAD);
    gSPDisplayList(POLY_XLU_DISP++, safety_beacon_crystal_outer_shape);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "", 0);
}
