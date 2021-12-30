/**
 * @file 3q_skybox.c
 * @author zel.
 * @brief Skybox drawing
 * @date 2021-12-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "global.h"
#include "skybox/sky_model/sky_model.h"
#include "skybox/clear_sky_textures/clear_sky_textures.h"
#include "skybox/cloudy_sky_textures/cloudy_sky_textures.h"

void* skyboxSegments[3];

Mtx* skyboxMatrix;

Color_RGBA8 skyboxCurPrim, skyboxCurEnv;

SkyColorBlendInfo skyboxColorBlendInfo[] = {
    { TIME_00, TIME_04, false, SKY_STATE_NIGHT, SKY_STATE_NIGHT },
    { TIME_04, TIME_05, true, SKY_STATE_NIGHT, SKY_STATE_MORNING },
    { TIME_05, TIME_06, false, SKY_STATE_MORNING, SKY_STATE_MORNING },
    { TIME_06, TIME_08, true, SKY_STATE_MORNING, SKY_STATE_DAY },
    { TIME_08, TIME_16, false, SKY_STATE_DAY, SKY_STATE_DAY },
    { TIME_16, TIME_17, true, SKY_STATE_DAY, SKY_STATE_EVENING },
    { TIME_17, TIME_18, false, SKY_STATE_EVENING, SKY_STATE_EVENING },
    { TIME_18, TIME_19, true, SKY_STATE_EVENING, SKY_STATE_NIGHT },
    { TIME_19, TIME_24, false, SKY_STATE_NIGHT, SKY_STATE_NIGHT },
};

SkyColor skyboxColors[] = {
    {
        // Prim Color
        {
            /* Morning */ { 186, 146, 114, 0 },
            /* Daytime */ { 255, 255, 255, 0 },
            /* Evening */ { 186, 107, 70, 60 },
            /* Night   */ { 25, 53, 235, 120 },
        },
        // Env Color
        {
            /* Morning */ { 113, 73, 128, 0 },
            /* Daytime */ { 0, 0, 160, 0 },
            /* Evening */ { 60, 0, 0, 0 },
            /* Night   */ { 26, 7, 0, 0 },
        },
    },
};

/**
 * Allocates ram for a skybox file, copies a file from ROM into that space, and returns a pointer to its new location in
 * ram.
 */
void* Skybox_LoadFile(GameState* state, u32 romStart, u32 romEnd) {
    void* dest;
    u32 size = romEnd - romStart;

    dest = GameState_Alloc(state, size, __FILE__, __LINE__);

    if (dest != NULL) {
        DmaMgr_SendRequest1(dest, romStart, size, __FILE__, __LINE__);
    }

    return dest;
}

/**
 * Main skybox initialization function
 */
void Skybox_Init(GlobalContext* globalCtx, SkyboxContext* skyboxCtx, short skyId) {
    // Initialize variables
    skyboxSegments[0] = skyboxSegments[1] = skyboxSegments[2] = NULL;
    skyboxCtx->rot.x = skyboxCtx->rot.y = skyboxCtx->rot.z = 0.0f;
    skyboxCurPrim.r = skyboxCurPrim.g = skyboxCurPrim.b = skyboxCurPrim.a = skyboxCurEnv.r = skyboxCurEnv.g =
        skyboxCurEnv.b = skyboxCurEnv.a = 0;

    if (skyId == SKYBOX_SKY) {
        skyboxSegments[0] =
            Skybox_LoadFile(&globalCtx->state, (u32)_sky_modelSegmentRomStart, (u32)_sky_modelSegmentRomEnd);
        skyboxSegments[1] = Skybox_LoadFile(&globalCtx->state, (u32)_clear_sky_texturesSegmentRomStart,
                                            (u32)_clear_sky_texturesSegmentRomEnd);
        skyboxSegments[2] = Skybox_LoadFile(&globalCtx->state, (u32)_cloudy_sky_texturesSegmentRomStart,
                                            (u32)_cloudy_sky_texturesSegmentRomEnd);
    }
}

/**
 * Math LERP function, returns an absolute value
 */
float Skybox_LerpAbsolute(float norm, float min, float max) {
    return fabsf((max - min) * norm + min);
}

/**
 * Main skybox update function.
 *
 * Handles updating the color of the sky based on the current time and day of the week.
 */
void Skybox_UpdateColors(SkyboxContext* skyboxCtx) {
    SkyColorBlendInfo* entry;
    int i;

    for (i = 0; i < ARRAY_COUNT(skyboxColorBlendInfo); i++) {
        entry = &skyboxColorBlendInfo[i];

        if ((gSaveContext.dayTime >= entry->startTime) &&
            (gSaveContext.dayTime < entry->endTime || entry->endTime == TIME_24)) {
            if (!entry->blendFlag) {
                // Set the prim color
                skyboxCurPrim.r = skyboxColors[GetWeekDay(gSaveContext)].prim[entry->destState].r;
                skyboxCurPrim.g = skyboxColors[GetWeekDay(gSaveContext)].prim[entry->destState].g;
                skyboxCurPrim.b = skyboxColors[GetWeekDay(gSaveContext)].prim[entry->destState].b;
                skyboxCurPrim.a = skyboxColors[GetWeekDay(gSaveContext)].prim[entry->destState].a;

                // Set the env color
                skyboxCurEnv.r = skyboxColors[GetWeekDay(gSaveContext)].env[entry->destState].r;
                skyboxCurEnv.g = skyboxColors[GetWeekDay(gSaveContext)].env[entry->destState].g;
                skyboxCurEnv.b = skyboxColors[GetWeekDay(gSaveContext)].env[entry->destState].b;
                skyboxCurEnv.a = skyboxColors[GetWeekDay(gSaveContext)].env[entry->destState].a;
            } else {
                short totalTime = ABS(entry->endTime - entry->startTime);
                short relativeTime = gSaveContext.dayTime - entry->startTime;
                float norm = (float)relativeTime / (float)totalTime;
                Color_RGBA8* prevColor;
                Color_RGBA8* destColor;

                // Prim color
                prevColor = &skyboxColors[GetWeekDay(gSaveContext)].prim[entry->prevState];
                destColor = &skyboxColors[GetWeekDay(gSaveContext)].prim[entry->destState];
                
                skyboxCurPrim.r = Skybox_LerpAbsolute(norm, prevColor->r, destColor->r);
                skyboxCurPrim.g = Skybox_LerpAbsolute(norm, prevColor->g, destColor->g);
                skyboxCurPrim.b = Skybox_LerpAbsolute(norm, prevColor->b, destColor->b);
                skyboxCurPrim.a = Skybox_LerpAbsolute(norm, prevColor->a, destColor->a);

                // Env color
                prevColor = &skyboxColors[GetWeekDay(gSaveContext)].env[entry->prevState];
                destColor = &skyboxColors[GetWeekDay(gSaveContext)].env[entry->destState];
                
                skyboxCurEnv.r = Skybox_LerpAbsolute(norm, prevColor->r, destColor->r);
                skyboxCurEnv.g = Skybox_LerpAbsolute(norm, prevColor->g, destColor->g);
                skyboxCurEnv.b = Skybox_LerpAbsolute(norm, prevColor->b, destColor->b);
                skyboxCurEnv.a = Skybox_LerpAbsolute(norm, prevColor->a, destColor->a);
            }

            break;
        }
    }
}

/**
 * Updates the skybox matrix
 */
Mtx* Skybox_UpdateMatrix(SkyboxContext* skyboxCtx, float x, float y, float z) {
    Matrix_Translate(x, y, z, MTXMODE_NEW);
    Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
    Matrix_RotateX(skyboxCtx->rot.x, MTXMODE_APPLY);
    Matrix_RotateY(skyboxCtx->rot.y, MTXMODE_APPLY);
    Matrix_RotateZ(skyboxCtx->rot.z, MTXMODE_APPLY);

    return Matrix_ToMtx(skyboxMatrix, __FILE__, __LINE__);
}

/**
 * Main skybox drawing function
 */
void Skybox_Draw(SkyboxContext* skyboxCtx, GraphicsContext* gfxCtx, short skyId, short alpha, float x, float y,
                 float z) {
    OPEN_DISPS(gfxCtx, __FILE__, __LINE__);

    // Clear attributes before drawing
    func_800945A0(gfxCtx);

    // Sky model bank
    gSPSegment(POLY_OPA_DISP++, 0x07, skyboxSegments[0]);

    // Cloudy sky texture bank
    gSPSegment(POLY_OPA_DISP++, 0x08, skyboxSegments[2]);

    // Clear sky texture bank
    gSPSegment(POLY_OPA_DISP++, 0x09, skyboxSegments[1]);

    if (skyId == SKYBOX_SKY) {
        // Update the sky colors
        Skybox_UpdateColors(skyboxCtx);

        gSPTexture(POLY_OPA_DISP++, 0x8000, 0x8000, 0, G_TX_RENDERTILE, G_ON);

        // Set sky matrix
        skyboxMatrix = Graph_Alloc(gfxCtx, sizeof(Mtx));
        Matrix_Translate(x, y, z, MTXMODE_NEW);
        Matrix_Scale(1.0f, 1.0f, 1.0f, MTXMODE_APPLY);
        Matrix_RotateX(skyboxCtx->rot.x, MTXMODE_APPLY);
        Matrix_RotateY(skyboxCtx->rot.y, MTXMODE_APPLY);
        Matrix_RotateZ(skyboxCtx->rot.z, MTXMODE_APPLY);
        Matrix_ToMtx(skyboxMatrix, __FILE__, __LINE__);
        gSPMatrix(POLY_OPA_DISP++, skyboxMatrix, G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

        // Load palette
        gDPSetColorDither(POLY_OPA_DISP++, G_CD_MAGICSQ);
        gDPSetTextureFilter(POLY_OPA_DISP++, G_TF_BILERP);
        gDPLoadTLUT_pal256(POLY_OPA_DISP++, skyPaletteTex);
        gDPSetTextureLUT(POLY_OPA_DISP++, G_TT_RGBA16);
        gDPSetTextureConvert(POLY_OPA_DISP++, G_TC_FILT);

        // Set combiner
        gDPSetCombineLERP(POLY_OPA_DISP++, TEXEL1, TEXEL0, PRIMITIVE_ALPHA, TEXEL0, TEXEL1, TEXEL0, PRIMITIVE, TEXEL0,
                          PRIMITIVE, ENVIRONMENT, COMBINED, ENVIRONMENT, 0, 0, 0, COMBINED);

        // Set the current color of the sky
        gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, skyboxCurPrim.r, skyboxCurPrim.g, skyboxCurPrim.b, skyboxCurPrim.a);
        gDPSetEnvColor(POLY_OPA_DISP++, skyboxCurEnv.r, skyboxCurEnv.g, skyboxCurEnv.b, skyboxCurEnv.a);

        // Draw each side of the skybox
        gSPDisplayList(POLY_OPA_DISP++, skyboxDisplaylist1);
        gSPDisplayList(POLY_OPA_DISP++, skyboxDisplaylist2);
        gSPDisplayList(POLY_OPA_DISP++, skyboxDisplaylist3);
        gSPDisplayList(POLY_OPA_DISP++, skyboxDisplaylist4);
        gSPDisplayList(POLY_OPA_DISP++, skyboxDisplaylist5);
    }

    gDPPipeSync(POLY_OPA_DISP++);

    CLOSE_DISPS(gfxCtx, __FILE__, __LINE__);
}
