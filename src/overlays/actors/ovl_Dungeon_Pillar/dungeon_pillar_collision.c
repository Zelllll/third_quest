#include "ultra64.h"
#include "z64.h"
#include "macros.h"

u32 dungeon_pillar_collision_polygonTypes[] = {
    0x00000000,
    0x00000002,
};

CollisionPoly dungeon_pillar_collision_polygons[] = {
    { 0x0000, 0x0000, 0x0001, 0x0002, 0x0000, 0x0000, 0x7fff, 0xfffb },
    { 0x0000, 0x0000, 0x0002, 0x0003, 0x0000, 0x0000, 0x7fff, 0xfffb },
    { 0x0000, 0x0004, 0x0002, 0x0001, 0x7fff, 0x0000, 0x0000, 0xfffb },
    { 0x0000, 0x0004, 0x0001, 0x0005, 0x7fff, 0x0000, 0x0000, 0xfffb },
    { 0x0000, 0x0000, 0x0003, 0x0006, 0x8000, 0x0000, 0x0000, 0xfffb },
    { 0x0000, 0x0000, 0x0006, 0x0007, 0x8000, 0x0000, 0x0000, 0xfffb },
    { 0x0000, 0x0006, 0x0003, 0x0002, 0x0000, 0x7fff, 0x0000, 0x0000 },
    { 0x0000, 0x0006, 0x0002, 0x0004, 0x0000, 0x7fff, 0x0000, 0x0000 },
    { 0x0000, 0x0005, 0x0007, 0x0006, 0x0000, 0x0000, 0x8000, 0xfffb },
    { 0x0000, 0x0005, 0x0006, 0x0004, 0x0000, 0x0000, 0x8000, 0xfffb },
};

Vec3s dungeon_pillar_collision_vertices[8] = {
    { -5, -130, 5 }, { 5, -130, 5 },  { 5, 0, 5 },   { -5, 0, 5 },
    { 5, 0, -5 },    { 5, -130, -5 }, { -5, 0, -5 }, { -5, -130, -5 },
};

CollisionHeader dungeon_pillar_collision_collisionHeader = { -5,
                                                             -130,
                                                             -5,
                                                             5,
                                                             0,
                                                             5,
                                                             8,
                                                             dungeon_pillar_collision_vertices,
                                                             10,
                                                             dungeon_pillar_collision_polygons,
                                                             dungeon_pillar_collision_polygonTypes,
                                                             0,
                                                             0,
                                                             0 };
