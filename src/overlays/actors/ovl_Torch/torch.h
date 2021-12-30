#ifndef _TORCH_H_
#define _TORCH_H_

#include "ultra64.h"
#include "global.h"

struct Torch;

typedef void (*TorchActionFunc)(struct Torch*, GlobalContext*);

#define TORCH_PARAMS_TO_TYPE(params) ((params >> 13) & 3)
#define TORCH_PARAMS_TO_CUTSCENE_FLAG(params) ((params >> 12) & 1)
#define TORCH_PARAMS_TO_LIT_FLAG(params) ((params >> 11) & 1)
#define TORCH_PARAMS_TO_PUZZLE_TORCH_COUNT(params) ((params >> 6) & 0x3F)
#define TORCH_PARAMS_TO_PUZZLE_SWITCH_FLAG(params) (params & 0x3F)

typedef struct Torch {
    Actor actor;
    ColliderCylinder colliderStand;
    ColliderCylinder colliderFlame;
    LightNode* lightNode;
    LightInfo lightInfo;
    Color_RGBA8 prim;
    Color_RGBA8 env;
    s16 flameColorFrame;
    s16 litTimer;
    u8 torchType;
    u8 cutsceneFlag;
    u8 litFlag;
    u8 puzzleTorchCount;
    u8 switchFlag;
} Torch;

extern const ActorInit Torch_InitVars;

#endif
