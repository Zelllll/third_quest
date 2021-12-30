/**
 * @file flame_generator.c
 * @author zel.
 * @brief Functions for handling generating an animated flame texture
 * @date 2021-12-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "global.h"
#include "custom/objects/gameplay_keep/flame_mesh.h"

#define FLAME_SCROLL_SPEED_LOWER 2
#define FLAME_SCROLL_SPEED_UPPER 4

static void* sGeneratedFlameTexPtr = NULL;
static u32 sLastFrame = 0;
static u32 sFlameTexScrollLowerLayerFrame = 0;
static u32 sFlameTexScrollUpperLayerFrame = 48; // Make the upper layer start at a different frame than the lower layer

// A inverted multiplied by B inverted, and the product is inverted
u8 FlameGenerator_ScreenColor(u8 a, u8 b) {
    // use shifting for extra speed
    return ((255 << 8) - ((255 - a) * (255 - b))) >> 8;
}

// A multiplied by B
u8 FlameGenerator_MultiplyColor(u8 a, u8 b) {
    // use shifting for extra speed
    return (a * b) >> 8;
}

void* FlameGenerator_GenerateTexture(GameState* state, u8 baseFlameTex[64][32], u8 baseFlameMaskTex[64][32],
                                     u8 baseFlameMaskScaledTex[128][32]) {
    if (sLastFrame != state->frames || sGeneratedFlameTexPtr == NULL) {
        u8(*tex)[32] = Graph_Alloc(state->gfxCtx, 32 * 64);
        u8(*work)[32] = Graph_Alloc(state->gfxCtx, 32 * 64);
        s32 i, j;

        // Store the pointer to the texture
        sGeneratedFlameTexPtr = tex;

        for (i = 0; i < 64; i++) {
            for (j = 0; j < 32; j++) {
                s32 row;

                // Generate the lower layer of the texture
                row = i + sFlameTexScrollLowerLayerFrame;
                if (row >= 64) {
                    row -= (row / 64) * 64;
                }
                tex[i][j] = FlameGenerator_ScreenColor(baseFlameTex[i][j], baseFlameMaskTex[row][j]);
                tex[i][j] = FlameGenerator_MultiplyColor(tex[i][j], baseFlameTex[i][j]);

                // Generate the upper layer of the texture
                row = i + sFlameTexScrollUpperLayerFrame;
                if (row >= 128) {
                    row -= (row / 128) * 128;
                }
                work[i][j] = FlameGenerator_ScreenColor(baseFlameTex[i][j], baseFlameMaskScaledTex[row][j]);

                // Multiply the upper and lower layers into the final texture
                tex[i][j] = FlameGenerator_MultiplyColor(tex[i][j], work[i][j]);
            }
        }

        // Increment the texture scroll animation
        sFlameTexScrollLowerLayerFrame += FLAME_SCROLL_SPEED_LOWER;
        if (sFlameTexScrollLowerLayerFrame >= 64) {
            sFlameTexScrollLowerLayerFrame -= (sFlameTexScrollLowerLayerFrame / 64) * 64;
        }

        sFlameTexScrollUpperLayerFrame += FLAME_SCROLL_SPEED_UPPER;
        if (sFlameTexScrollUpperLayerFrame >= 128) {
            sFlameTexScrollUpperLayerFrame -= (sFlameTexScrollUpperLayerFrame / 128) * 128;
        }

        // Set the last frame to prevent running twice in a single frame
        sLastFrame = state->frames;
    }

    return sGeneratedFlameTexPtr;
}

void FlameGenerator_DrawFlame(GameState* state, u8 primR, u8 primG, u8 primB, u8 primA, u8 envR, u8 envG, u8 envB,
                              u8 envA) {
    void* tex = FlameGenerator_GenerateTexture(state, Lib_SegmentedToVirtual(gFlameBaseTex),
                                               Lib_SegmentedToVirtual(gFlameMaskBaseTex),
                                               Lib_SegmentedToVirtual(gFlameMaskScaledBaseTex));

    if (tex != NULL) {
        OPEN_DISPS(state->gfxCtx, "", 0);

        gDPPipeSync(POLY_XLU_DISP++);

        // Store the generated texture in segment 8 to use with the flame model
        gSPSegment(POLY_XLU_DISP++, 0x08, tex);

        // Set up the displaylist
        POLY_XLU_DISP = Gfx_CallSetupDL(POLY_XLU_DISP, 0);
        gDPPipeSync(POLY_XLU_DISP++);

        // Set the color
        gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, primR, primG, primB, primA);
        gDPSetEnvColor(POLY_XLU_DISP++, envR, envG, envB, envA);

        // Render the flame
        gSPDisplayList(POLY_XLU_DISP++, gGeneratedFlameDL);

        CLOSE_DISPS(state->gfxCtx, "", 0);
    }
}
