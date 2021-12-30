#ifndef _EARTH_WATER_PLANE_H_
#define _EARTH_WATER_PLANE_H_

#include "ultra64.h"
#include "global.h"

struct EarthWaterPlane;

typedef struct EarthWaterPlane {
    Actor actor;
    u8 switchFlag;
} EarthWaterPlane;

extern const ActorInit Earth_Water_Plane_InitVars;

#endif
