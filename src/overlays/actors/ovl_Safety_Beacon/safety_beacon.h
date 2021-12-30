#ifndef _SAFETY_BEACON_H_
#define _SAFETY_BEACON_H_

#include "ultra64.h"
#include "global.h"

struct SafetyBeacon;

typedef void (*SafetyBeaconActionFunc)(struct SafetyBeacon*, GlobalContext*);

typedef struct SafetyBeacon {
    Actor actor;
    ColliderCylinder collider;
    SafetyBeaconActionFunc actionFunc;
    LightNode* lightNode;
    LightInfo lightInfo;
    f32 sphereRot;
    s16 glowIntensity;
} SafetyBeacon;

extern const ActorInit Safety_Beacon_InitVars;

#endif
