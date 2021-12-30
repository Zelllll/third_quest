#ifndef _3Q_SKYBOX_H_
#define _3Q_SKYBOX_H_

void Skybox_Init(GlobalContext* globalCtx, SkyboxContext* skyboxCtx, short skyId);
Mtx* Skybox_UpdateMatrix(SkyboxContext* skyboxCtx, float x, float y, float z);
void Skybox_Draw(SkyboxContext* skyboxCtx, GraphicsContext* gfxCtx, short skyId, short alpha, float x, float y, float z);

extern void* skyboxSegments[3];

#define MORNING_TIME 0x4555
#define NIGHT_TIME 0xC001
#define TIME_00 0x0000
#define TIME_01 0x0AAB
#define TIME_02 0x1556
#define TIME_03 0x2001
#define TIME_04 0x2AAC
#define TIME_05 0x3556
#define TIME_06 0x4000
#define TIME_07 0x4AAB
#define TIME_08 0x5556
#define TIME_09 0x6000
#define TIME_10 0x6AAb
#define TIME_11 0x7556
#define TIME_12 0x8001
#define TIME_13 0x8AAc
#define TIME_14 0x9556
#define TIME_15 0xA000
#define TIME_16 0xAAAB
#define TIME_17 0xB556
#define TIME_18 0xC001
#define TIME_19 0xCAAC
#define TIME_20 0xD556
#define TIME_21 0xE000
#define TIME_22 0xEAAB
#define TIME_23 0xF556
#define TIME_24 0xFFFF

// @TODO: Add week system!
#define GetWeekDay(saveCtx) 0

enum {
    SKYBOX_NONE,
    SKYBOX_SKY,
    SKYBOX_FOG_COLOR = 0x1D
};

enum {
    SKY_STATE_MORNING,
    SKY_STATE_DAY,
    SKY_STATE_EVENING,
    SKY_STATE_NIGHT
};

typedef struct {
    Color_RGBA8 prim[4];
    Color_RGBA8 env[4];
} SkyColor;

typedef struct {
    unsigned short startTime; // start of this interval in world time
    unsigned short endTime; // end of this interval in world time
    unsigned char blendFlag; // if true, blend to the next state
    short prevState; // previous state (morning, day, evening, night)
    short destState; // destination state (morning, day, evening, night)
} SkyColorBlendInfo;

#endif
