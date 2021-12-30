#ifndef _LILYPAD_H_
#define _LILYPAD_H_

#include "ultra64.h"
#include "global.h"

struct Lilypad;

typedef void (*LilypadActionFunc)(struct Lilypad*, GlobalContext*);

typedef struct Lilypad {
    DynaPolyActor dyna;
    LilypadActionFunc actionFunc;
    f32 height;
    u8 hasSpawnedRipples;
    s16 timer;
    s16 timer2;
} Lilypad;

extern const ActorInit Lilypad_InitVars;

#endif
