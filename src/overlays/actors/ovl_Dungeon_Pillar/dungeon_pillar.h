#ifndef _DUNGEON_PILLAR_H_
#define _DUNGEON_PILLAR_H_

#include "ultra64.h"
#include "global.h"

struct DungeonPillar;

typedef void (*DungeonPillarActionFunc)(struct DungeonPillar*, GlobalContext*);

#define DP_PARAMS_TO_SKIN(params) (((params) & 0xF000) >> 12)
#define DP_PARAMS_TO_INIT_DIR(params) (((params) & 0x0800) >> 11)
#define DP_PARAMS_TO_SPEED(params) ((((params) & 0x0700) >> 8) + 1)
#define DP_PARAMS_TO_REVERSE_FLAG(params) (((params) & 0x0080) >> 7)
#define DP_PARAMS_TO_PLAYER_CHECK_FLAG(params) (((params) & 0x0040) >> 6)
#define DP_PARAMS_TO_SWITCH_FLAG(params) ((params) & 0x003F)

#define DP_X_ROT_TO_SCALE(x_rot) ((((x_rot) & 0xFF00) >> 8) + 1)
#define DP_X_ROT_TO_WAIT_FRAMES(x_rot) ((x_rot) & 0x00FF)

#define DP_Z_ROT_TO_HEIGHT(z_rot) (z_rot)

typedef struct DungeonPillar {
    DynaPolyActor dyna;
    DungeonPillarActionFunc actionFunc;
    s8 bankIndex;
    u8 skin;
    u8 initDir;
    u16 speed;
    u8 reverseFlag;
    u8 playerCheckFlag;
    u8 switchFlag;
    u8 waitFrames;
    u16 height;
    f32 targetY; // current target position being stepped towards
    s16 timer;
    s16 actionState;
} DungeonPillar;

extern const ActorInit Dungeon_Pillar_InitVars;

#endif
