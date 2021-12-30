/*
 * Modified from: https://github.com/zeldaret/mm/blob/master/src/overlays/actors/ovl_Bg_Lotus/z_bg_lotus.c
 */

#include "lilypad.h"
#include "custom/objects/object_lilypad/object_lilypad.h"

#define FLAGS 0x00000000

#define THIS ((Lilypad*)thisx)

void Lilypad_Init(Actor* thisx, GlobalContext* globalCtx);
void Lilypad_Destroy(Actor* thisx, GlobalContext* globalCtx);
void Lilypad_Update(Actor* thisx, GlobalContext* globalCtx);
void Lilypad_Draw(Actor* thisx, GlobalContext* globalCtx);

void Lilypad_Wait(Lilypad* this, GlobalContext* globalCtx);
void Lilypad_Sink(Lilypad* this, GlobalContext* globalCtx);
void Lilypad_WaitToAppear(Lilypad* this, GlobalContext* globalCtx);

const ActorInit Lilypad_InitVars = {
    ACTOR_LILYPAD,
    ACTORCAT_BG,
    FLAGS,
    OBJECT_LILYPAD,
    sizeof(Lilypad),
    (ActorFunc)Lilypad_Init,
    (ActorFunc)Lilypad_Destroy,
    (ActorFunc)Lilypad_Update,
    (ActorFunc)Lilypad_Draw,
};

static InitChainEntry sInitChain[] = {
    ICHAIN_VEC3F_DIV1000(scale, 100, ICHAIN_STOP),
};

void Lilypad_Init(Actor* thisx, GlobalContext* globalCtx) {
    Lilypad* this = THIS;
    CollisionHeader* colHeader;
    void* sp2C;

    Actor_ProcessInitChain(&this->dyna.actor, sInitChain);
    DynaPolyActor_Init(&this->dyna, DPM_PLAYER);
    CollisionHeader_GetVirtual(&gLilypadCol, &colHeader);
    this->dyna.bgId = DynaPoly_SetBgActor(globalCtx, &globalCtx->colCtx.dyna, &this->dyna.actor, colHeader);

    this->dyna.actor.floorHeight = BgCheck_EntityRaycastFloor7(&globalCtx->colCtx, &this->dyna.actor.floorPoly, &sp2C,
                                                               &this->dyna.actor, &this->dyna.actor.world.pos);

    this->timer2 = 96;
    this->dyna.actor.world.rot.y = Rand_Next() >> 0x10;
    this->actionFunc = Lilypad_Wait;
}

void Lilypad_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    Lilypad* this = THIS;

    DynaPoly_DeleteBgActor(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
}

void Lilypad_SetScaleXZ(Lilypad* this) {
    f32 scale;

    if (this->dyna.actor.params == 0) {
        scale = Math_SinF(this->timer * 0.7853982f) * ((0.014f * ((f32)this->timer / 8)) + 0.01f);
        this->dyna.actor.scale.x = (1.0f + scale) * 0.1f;
        this->dyna.actor.scale.z = (1.0f - scale) * 0.1f;
    }
}

void Lilypad_Wait(Lilypad* this, GlobalContext* globalCtx) {
    f32 moveDist; // distance for the xz position of the lilypad to move

    this->timer2--;

    moveDist = Math_SinF(this->timer2 * 0.06544985f) * 6.0f;

    if (this->dyna.actor.params == 0) {
        this->dyna.actor.world.pos.x =
            (Math_SinS(this->dyna.actor.world.rot.y) * moveDist) + this->dyna.actor.home.pos.x;
        this->dyna.actor.world.pos.z =
            (Math_CosS(this->dyna.actor.world.rot.y) * moveDist) + this->dyna.actor.home.pos.z;

        if (this->timer2 == 0) {
            this->timer2 = 96;
            this->dyna.actor.world.rot.y += (s16)(Rand_Next() >> 0x12);
        }
    }

    if (this->height < this->dyna.actor.floorHeight) {
        this->dyna.actor.world.pos.y = this->dyna.actor.floorHeight;
    } else {
        this->dyna.actor.world.pos.y = this->height;

        if (func_8004356C(&this->dyna)) {
            if (this->hasSpawnedRipples == 0) {
                EffectSsGRipple_Spawn(globalCtx, &this->dyna.actor.world.pos, 1000, 1400, 0);
                EffectSsGRipple_Spawn(globalCtx, &this->dyna.actor.world.pos, 1000, 1400, 8);
                this->timer = 40;
            }
            // Seems to be checking if the current mask form is Deku Link
            // if (gSaveContext.perm.unk20 != 3) {
            this->timer = 40;
            this->timer2 = 15; // amount of frames to wait before sinking
            this->dyna.actor.flags |= 0x10;
            this->actionFunc = Lilypad_Sink;
            return;
            // }

            this->hasSpawnedRipples = 1;
        } else {
            this->hasSpawnedRipples = 0;
        }
    }

    if (this->timer > 0) {
        this->timer--;
    }

    Lilypad_SetScaleXZ(this);
}

void Lilypad_Sink(Lilypad* this, GlobalContext* globalCtx) {
    if (this->timer2 == 0) {
        if (this->height < this->dyna.actor.world.pos.y) {
            this->dyna.actor.world.pos.y = this->height;
        }

        this->dyna.actor.world.pos.y -= 1.0f;

        if (this->dyna.actor.world.pos.y <= this->dyna.actor.floorHeight) {
            this->dyna.actor.world.pos.y = this->dyna.actor.floorHeight;
            this->timer = 0;
        }

        if (this->timer > 0) {
            this->timer--;
            Lilypad_SetScaleXZ(this);
        } else {
            if (Math_StepToF(&this->dyna.actor.scale.x, 0, 0.0050000003539f)) {
                this->dyna.actor.draw = NULL;
                this->timer = 100;
                func_8003EBF8(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
                this->actionFunc = Lilypad_WaitToAppear;
            }
            this->dyna.actor.scale.z = this->dyna.actor.scale.x;
        }
    } else {
        this->timer2--;
    }
}

void Lilypad_WaitToAppear(Lilypad* this, GlobalContext* globalCtx) {
    if (this->timer > 0) {
        this->timer--;
    } else if ((this->dyna.actor.xzDistToPlayer > 100.0f) && (this->dyna.actor.projectedPos.z < 0.0f)) {
        this->dyna.actor.draw = Lilypad_Draw;
        func_8003EC50(globalCtx, &globalCtx->colCtx.dyna, this->dyna.bgId);
        Actor_SetScale(&this->dyna.actor, 0.1f);
        this->dyna.actor.world.pos.y = CLAMP_MIN(this->height, this->dyna.actor.floorHeight);
        this->dyna.actor.flags &= ~0x10;
        this->timer2 = 96;
        this->actionFunc = Lilypad_Wait;
        this->dyna.actor.world.pos.x = this->dyna.actor.home.pos.x;
        this->dyna.actor.world.pos.z = this->dyna.actor.home.pos.z;
    }
}

void Lilypad_Update(Actor* thisx, GlobalContext* globalCtx) {
    Lilypad* this = THIS;
    WaterBox* sp2C;

    // Might be the wrong function here
    WaterBox_GetSurfaceImpl(globalCtx, &globalCtx->colCtx, this->dyna.actor.world.pos.x, this->dyna.actor.world.pos.z,
                            &this->height, &sp2C);
    this->actionFunc(this, globalCtx);
}

void Lilypad_Draw(Actor* thisx, GlobalContext* globalCtx) {
    Gfx_DrawDListOpa(globalCtx, gLilypadDL);
}
