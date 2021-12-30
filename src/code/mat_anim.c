/**
 * Modified from: https://github.com/zeldaret/mm/blob/master/src/code/z_scene_proc.c
 */
#include "global.h"

void Scene_DrawConfigDefault(GlobalContext* globalCtx);
void Scene_DrawConfigMatAnim(GlobalContext* globalCtx);

static s32 sMatAnimStep;
static u32 sMatAnimFlags;
static f32 sMatAnimAlphaRatio;

// Default displaylist that sets a valid displaylist into all of the segments.
static Gfx sSceneDrawDefaultDL[] = {
    gsSPSegment(0x08, gEmptyDL),
    gsSPSegment(0x09, gEmptyDL),
    gsSPSegment(0x0A, gEmptyDL),
    gsSPSegment(0x0B, gEmptyDL),
    gsSPSegment(0x0C, gEmptyDL),
    gsSPSegment(0x0D, gEmptyDL),
    gsSPSegment(0x06, gEmptyDL),
    gsDPPipeSync(),
    gsDPSetPrimColor(0, 0, 128, 128, 128, 128),
    gsDPSetEnvColor(128, 128, 128, 128),
    gsSPEndDisplayList(),
};

/**
 * Executes the current scene draw config handler.
 */
void Scene_ExecuteDrawConfig(GlobalContext* globalCtx) {
    static void (*sceneDrawConfigHandlers[])(GlobalContext*) = {
        Scene_DrawConfigDefault,
        Scene_DrawConfigMatAnim,
    };

    sceneDrawConfigHandlers[globalCtx->sceneConfig](globalCtx);
}

/**
 * Scene Draw Config 0:
 * Default scene draw config function. This just executes `sSceneDrawDefaultDL`.
 */
void Scene_DrawConfigDefault(GlobalContext* globalCtx) {
    OPEN_DISPS(globalCtx->state.gfxCtx, "", 0);

    gSPDisplayList(POLY_OPA_DISP++, sSceneDrawDefaultDL);
    gSPDisplayList(POLY_XLU_DISP++, sSceneDrawDefaultDL);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "", 0);
}

/**
 * Returns a pointer to a single layer texture scroll displaylist.
 */
Gfx* AnimatedMat_TexScroll(GlobalContext* globalCtx, AnimatedMatTexScrollParams* params) {
    return Gfx_TexScroll(globalCtx->state.gfxCtx, params->xStep * sMatAnimStep, -(params->yStep * sMatAnimStep),
                         params->width, params->height);
}

/**
 * Animated Material Type 0:
 * Scrolls a single layer texture using the provided `AnimatedMatTexScrollParams`.
 */
void AnimatedMat_DrawTexScroll(GlobalContext* globalCtx, s32 segment, void* params) {
    AnimatedMatTexScrollParams* texScrollParams = (AnimatedMatTexScrollParams*)params;
    Gfx* texScrollDList = AnimatedMat_TexScroll(globalCtx, texScrollParams);

    OPEN_DISPS(globalCtx->state.gfxCtx, "", 0);

    if (sMatAnimFlags & 1) {
        gSPSegment(POLY_OPA_DISP++, segment, texScrollDList);
    }

    if (sMatAnimFlags & 2) {
        gSPSegment(POLY_XLU_DISP++, segment, texScrollDList);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "", 0);
}

/**
 * Returns a pointer to a two layer texture scroll displaylist.
 */
Gfx* AnimatedMat_TwoLayerTexScroll(GlobalContext* globalCtx, AnimatedMatTexScrollParams* params) {
    return Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, params[0].xStep * sMatAnimStep,
                            -(params[0].yStep * sMatAnimStep), params[0].width, params[0].height, 1,
                            params[1].xStep * sMatAnimStep, -(params[1].yStep * sMatAnimStep), params[1].width,
                            params[1].height);
}

/**
 * Animated Material Type 1:
 * Scrolls a two layer texture using the provided `AnimatedMatTexScrollParams`.
 */
void AnimatedMat_DrawTwoTexScroll(GlobalContext* globalCtx, s32 segment, void* params) {
    AnimatedMatTexScrollParams* texScrollParams = (AnimatedMatTexScrollParams*)params;
    Gfx* texScrollDList = AnimatedMat_TwoLayerTexScroll(globalCtx, texScrollParams);

    OPEN_DISPS(globalCtx->state.gfxCtx, "", 0);

    if (sMatAnimFlags & 1) {
        gSPSegment(POLY_OPA_DISP++, segment, texScrollDList);
    }

    if (sMatAnimFlags & 2) {
        gSPSegment(POLY_XLU_DISP++, segment, texScrollDList);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "", 0);
}

/**
 * Generates a displaylist that sets the prim and env color, and stores it in the provided segment ID.
 */
void AnimatedMat_SetColor(GlobalContext* globalCtx, s32 segment, F3DPrimColor* primColorResult, F3DEnvColor* envColor) {
    Gfx* colorDList = (Gfx*)Graph_Alloc(globalCtx->state.gfxCtx, sizeof(Gfx) * 4);

    OPEN_DISPS(globalCtx->state.gfxCtx, "", 0);

    // clang-format off
    if (sMatAnimFlags & 1) { gSPSegment(POLY_OPA_DISP++, segment, colorDList); }
    if (sMatAnimFlags & 2) { gSPSegment(POLY_XLU_DISP++, segment, colorDList); }
    // clang-format on

    gDPSetPrimColor(colorDList++, 0, primColorResult->lodFrac, primColorResult->r, primColorResult->g,
                    primColorResult->b, (u8)(primColorResult->a * sMatAnimAlphaRatio));

    if (envColor != NULL) {
        gDPSetEnvColor(colorDList++, envColor->r, envColor->g, envColor->b, envColor->a);
    }

    gSPEndDisplayList(colorDList++);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "", 0);
}

/**
 * Animated Material Type 2:
 * Color key frame animation without linear interpolation.
 */
void AnimatedMat_DrawColor(GlobalContext* globalCtx, s32 segment, void* params) {
    AnimatedMatColorParams* colorAnimParams = (AnimatedMatColorParams*)params;
    F3DPrimColor* primColor;
    F3DEnvColor* envColor;
    s32 curFrame;

    primColor = (F3DPrimColor*)Lib_SegmentedToVirtual(colorAnimParams->primColors);

    curFrame = sMatAnimStep % colorAnimParams->keyFrameLength;
    primColor += curFrame;
    envColor = (colorAnimParams->envColors != NULL)
                   ? (F3DEnvColor*)Lib_SegmentedToVirtual(colorAnimParams->envColors) + curFrame
                   : NULL;

    AnimatedMat_SetColor(globalCtx, segment, primColor, envColor);
}

/**
 * Linear Interpolation
 */
s32 AnimatedMat_Lerp(s32 min, s32 max, f32 norm) {
    return (s32)((max - min) * norm) + min;
}

/**
 * Animated Material Type 3:
 * Color key frame animation with linear interpolation.
 */
void AnimatedMat_DrawColorLerp(GlobalContext* globalCtx, s32 segment, void* params) {
    AnimatedMatColorParams* colorAnimParams = (AnimatedMatColorParams*)params;
    F3DPrimColor* primColorMax;
    F3DEnvColor* envColorMax;
    u16* keyFrames;
    s32 curFrame;
    s32 endFrame;
    s32 relativeFrame; // relative to the start frame
    s32 startFrame;
    f32 norm;
    F3DPrimColor* primColorMin;
    F3DPrimColor primColorResult;
    F3DEnvColor* envColorMin;
    F3DEnvColor envColorResult;
    s32 i;

    primColorMax = (F3DPrimColor*)Lib_SegmentedToVirtual(colorAnimParams->primColors);
    keyFrames = (u16*)Lib_SegmentedToVirtual(colorAnimParams->keyFrames);
    curFrame = sMatAnimStep % colorAnimParams->keyFrameLength;
    keyFrames++;
    i = 1;

    while (colorAnimParams->keyFrameCount > i) {
        if (curFrame < *keyFrames) {
            break;
        }
        i++;
        keyFrames++;
    }

    startFrame = keyFrames[-1];
    endFrame = keyFrames[0] - startFrame;
    relativeFrame = curFrame - startFrame;
    norm = (f32)relativeFrame / (f32)endFrame;

    primColorMax += i;
    primColorMin = primColorMax - 1;
    primColorResult.r = AnimatedMat_Lerp(primColorMin->r, primColorMax->r, norm);
    primColorResult.g = AnimatedMat_Lerp(primColorMin->g, primColorMax->g, norm);
    primColorResult.b = AnimatedMat_Lerp(primColorMin->b, primColorMax->b, norm);
    primColorResult.a = AnimatedMat_Lerp(primColorMin->a, primColorMax->a, norm);
    primColorResult.lodFrac = AnimatedMat_Lerp(primColorMin->lodFrac, primColorMax->lodFrac, norm);

    if (colorAnimParams->envColors) {
        envColorMax = (F3DEnvColor*)Lib_SegmentedToVirtual(colorAnimParams->envColors);
        envColorMax += i;
        envColorMin = envColorMax - 1;
        envColorResult.r = AnimatedMat_Lerp(envColorMin->r, envColorMax->r, norm);
        envColorResult.g = AnimatedMat_Lerp(envColorMin->g, envColorMax->g, norm);
        envColorResult.b = AnimatedMat_Lerp(envColorMin->b, envColorMax->b, norm);
        envColorResult.a = AnimatedMat_Lerp(envColorMin->a, envColorMax->a, norm);
    } else {
        envColorMax = NULL;
    }

    AnimatedMat_SetColor(globalCtx, segment, &primColorResult, (envColorMax != NULL) ? &envColorResult : NULL);
}

/**
 * Lagrange interpolation
 */
f32 Scene_LagrangeInterp(s32 n, f32 x[], f32 fx[], f32 xp) {
    f32 weights[50];
    f32 xVal;
    f32 m;
    f32 intp;
    f32* xPtr1;
    f32* fxPtr;
    f32* weightsPtr;
    f32* xPtr2;
    s32 i;
    s32 j;

    for (i = 0, weightsPtr = weights, xPtr1 = x, fxPtr = fx, weightsPtr = weightsPtr; i < n; i++) {
        for (xVal = *xPtr1, m = 1.0f, j = 0, xPtr2 = x; j < n; j++) {
            if (j != i) {
                m *= xVal - (*xPtr2);
            }
            xPtr2++;
        }

        xPtr1++;
        *weightsPtr = (*fxPtr) / m;
        fxPtr++;
        weightsPtr++;
    }

    for (intp = 0.0f, i = 0, weightsPtr = weights; i < n; i++) {
        for (m = 1.0f, j = 0, xPtr2 = x; j < n; j++) {
            if (j != i) {
                m *= xp - (*xPtr2);
            }
            xPtr2++;
        }

        intp += (*weightsPtr) * m;
        weightsPtr++;
    }

    return intp;
}

/**
 * Lagrange interpolation specifically for colors
 */
u8 Scene_LagrangeInterpColor(s32 n, f32 x[], f32 fx[], f32 xp) {
    s32 intp = Scene_LagrangeInterp(n, x, fx, xp);

    // Clamp between 0 and 255 to ensure the color value does not overflow in either direction
    return CLAMP(intp, 0, 255);
}

/**
 * Animated Material Type 4:
 * Color key frame animation with non-linear interpolation.
 */
void AnimatedMat_DrawColorNonLinearInterp(GlobalContext* globalCtx, s32 segment, void* params) {
    AnimatedMatColorParams* colorAnimParams = (AnimatedMatColorParams*)params;
    F3DPrimColor* primColorCur = Lib_SegmentedToVirtual(colorAnimParams->primColors);
    F3DEnvColor* envColorCur = Lib_SegmentedToVirtual(colorAnimParams->envColors);
    u16* keyFrames = Lib_SegmentedToVirtual(colorAnimParams->keyFrames);
    f32 curFrame = sMatAnimStep % colorAnimParams->keyFrameLength;
    F3DPrimColor primColorResult;
    F3DEnvColor envColorResult;
    f32 x[50];
    f32 fxPrimR[50];
    f32 fxPrimG[50];
    f32 fxPrimB[50];
    f32 fxPrimA[50];
    f32 fxPrimLodFrac[50];
    f32 fxEnvR[50];
    f32 fxEnvG[50];
    f32 fxEnvB[50];
    f32 fxEnvA[50];
    f32* xPtr = x;
    f32* fxPrimRPtr = fxPrimR;
    f32* fxPrimGPtr = fxPrimG;
    f32* fxPrimBPtr = fxPrimB;
    f32* fxPrimAPtr = fxPrimA;
    f32* fxPrimLodFracPtr = fxPrimLodFrac;
    f32* fxEnvRPtr = fxEnvR;
    f32* fxEnvGPtr = fxEnvG;
    f32* fxEnvBPtr = fxEnvB;
    f32* fxEnvAPtr = fxEnvA;
    s32 i;

    for (i = 0; i < colorAnimParams->keyFrameCount; i++) {
        *xPtr = *keyFrames;
        *fxPrimRPtr = primColorCur->r;
        *fxPrimGPtr = primColorCur->g;
        *fxPrimBPtr = primColorCur->b;
        *fxPrimAPtr = primColorCur->a;
        *fxPrimLodFracPtr = primColorCur->lodFrac;

        primColorCur++;
        fxPrimRPtr++;
        fxPrimGPtr++;
        fxPrimBPtr++;
        fxPrimAPtr++;
        fxPrimLodFracPtr++;

        if (envColorCur != NULL) {
            *fxEnvRPtr = envColorCur->r;
            *fxEnvGPtr = envColorCur->g;
            *fxEnvBPtr = envColorCur->b;
            *fxEnvAPtr = envColorCur->a;

            envColorCur++;
            fxEnvRPtr++;
            fxEnvGPtr++;
            fxEnvBPtr++;
            fxEnvAPtr++;
        }

        keyFrames++;
        xPtr++;
    }

    primColorResult.r = Scene_LagrangeInterpColor(colorAnimParams->keyFrameCount, x, fxPrimR, curFrame);
    primColorResult.g = Scene_LagrangeInterpColor(colorAnimParams->keyFrameCount, x, fxPrimG, curFrame);
    primColorResult.b = Scene_LagrangeInterpColor(colorAnimParams->keyFrameCount, x, fxPrimB, curFrame);
    primColorResult.a = Scene_LagrangeInterpColor(colorAnimParams->keyFrameCount, x, fxPrimA, curFrame);
    primColorResult.lodFrac = Scene_LagrangeInterpColor(colorAnimParams->keyFrameCount, x, fxPrimLodFrac, curFrame);

    if (colorAnimParams->envColors != NULL) {
        envColorCur = Lib_SegmentedToVirtual(colorAnimParams->envColors);
        envColorResult.r = Scene_LagrangeInterpColor(colorAnimParams->keyFrameCount, x, fxEnvR, curFrame);
        envColorResult.g = Scene_LagrangeInterpColor(colorAnimParams->keyFrameCount, x, fxEnvG, curFrame);
        envColorResult.b = Scene_LagrangeInterpColor(colorAnimParams->keyFrameCount, x, fxEnvB, curFrame);
        envColorResult.a = Scene_LagrangeInterpColor(colorAnimParams->keyFrameCount, x, fxEnvA, curFrame);
    } else {
        envColorCur = NULL;
    }

    AnimatedMat_SetColor(globalCtx, segment, &primColorResult, (envColorCur != NULL) ? &envColorResult : NULL);
}

/**
 * Animated Material Type 5:
 * Cycles between a list of textures (imagine like a GIF)
 */
void AnimatedMat_DrawTexCycle(GlobalContext* globalCtx, s32 segment, void* params) {
    AnimatedMatTexCycleParams* texAnimParams = params;
    void** texList = (void**)Lib_SegmentedToVirtual(texAnimParams->textureList);
    u8* texId = (u8*)Lib_SegmentedToVirtual(texAnimParams->textureIndexList);
    s32 curFrame = sMatAnimStep % texAnimParams->keyFrameLength;
    void* tex = Lib_SegmentedToVirtual(texList[texId[curFrame]]);

    OPEN_DISPS(globalCtx->state.gfxCtx, "", 0);

    if (sMatAnimFlags & 1) {
        gSPSegment(POLY_OPA_DISP++, segment, tex);
    }

    if (sMatAnimFlags & 2) {
        gSPSegment(POLY_XLU_DISP++, segment, tex);
    }

    CLOSE_DISPS(globalCtx->state.gfxCtx, "", 0);
}

/**
 * This is the main function that handles the animated material system.
 * There are six different animated material types, which should be set in the provided `AnimatedMaterial`.
 */
void AnimatedMat_DrawMain(GlobalContext* globalCtx, AnimatedMaterial* matAnim, f32 alphaRatio, u32 step, u32 flags) {
    static void (*matAnimDrawHandlers[])(GlobalContext*, s32 segment, void* params) = {
        AnimatedMat_DrawTexScroll, AnimatedMat_DrawTwoTexScroll,         AnimatedMat_DrawColor,
        AnimatedMat_DrawColorLerp, AnimatedMat_DrawColorNonLinearInterp, AnimatedMat_DrawTexCycle,
    };
    s32 segmentAbs;
    s32 segment;

    sMatAnimAlphaRatio = alphaRatio;
    sMatAnimStep = step;
    sMatAnimFlags = flags;

    if ((matAnim != NULL) && (matAnim->segment != 0)) {
        do {
            segment = matAnim->segment;
            segmentAbs = ((segment < 0) ? -segment : segment) + 7;
            matAnimDrawHandlers[matAnim->type](globalCtx, segmentAbs, (void*)Lib_SegmentedToVirtual(matAnim->params));
            matAnim++;
        } while (segment > -1);
    }
}

/**
 * Draws an animated material to both OPA and XLU buffers.
 */
void AnimatedMat_Draw(GlobalContext* globalCtx, AnimatedMaterial* matAnim) {
    AnimatedMat_DrawMain(globalCtx, matAnim, 1, globalCtx->gameplayFrames, 3);
}

/**
 * Draws an animated material to only the OPA buffer.
 */
void AnimatedMat_DrawOpa(GlobalContext* globalCtx, AnimatedMaterial* matAnim) {
    AnimatedMat_DrawMain(globalCtx, matAnim, 1, globalCtx->gameplayFrames, 1);
}

/**
 * Draws an animated material to only the XLU buffer.
 */
void AnimatedMat_DrawXlu(GlobalContext* globalCtx, AnimatedMaterial* matAnim) {
    AnimatedMat_DrawMain(globalCtx, matAnim, 1, globalCtx->gameplayFrames, 2);
}

/**
 * Draws an animated material with an alpha ratio (0.0 - 1.0) both OPA and XLU buffers.
 */
void AnimatedMat_DrawAlpha(GlobalContext* globalCtx, AnimatedMaterial* matAnim, f32 alphaRatio) {
    AnimatedMat_DrawMain(globalCtx, matAnim, alphaRatio, globalCtx->gameplayFrames, 3);
}

/**
 * Draws an animated material with an alpha ratio (0.0 - 1.0) to only the OPA buffer.
 */
void AnimatedMat_DrawAlphaOpa(GlobalContext* globalCtx, AnimatedMaterial* matAnim, f32 alphaRatio) {
    AnimatedMat_DrawMain(globalCtx, matAnim, alphaRatio, globalCtx->gameplayFrames, 1);
}

/**
 * Draws an animated material with an alpha ratio (0.0 - 1.0) to only the XLU buffer.
 */
void AnimatedMat_DrawAlphaXlu(GlobalContext* globalCtx, AnimatedMaterial* matAnim, f32 alphaRatio) {
    AnimatedMat_DrawMain(globalCtx, matAnim, alphaRatio, globalCtx->gameplayFrames, 2);
}

/**
 * Draws an animated material with a step to both the OPA and XLU buffers.
 */
void AnimatedMat_DrawStep(GlobalContext* globalCtx, AnimatedMaterial* matAnim, u32 step) {
    AnimatedMat_DrawMain(globalCtx, matAnim, 1, step, 3);
}

/**
 * Draws an animated material with a step to only the OPA buffer.
 */
void AnimatedMat_DrawStepOpa(GlobalContext* globalCtx, AnimatedMaterial* textures, u32 step) {
    AnimatedMat_DrawMain(globalCtx, textures, 1, step, 1);
}

/**
 * Draws an animated material with a step to only the XLU buffer.
 */
void AnimatedMat_DrawStepXlu(GlobalContext* globalCtx, AnimatedMaterial* matAnim, u32 step) {
    AnimatedMat_DrawMain(globalCtx, matAnim, 1, step, 2);
}

/**
 * Draws an animated material with an alpha ratio (0.0 - 1.0) and a step to both the OPA and XLU buffers.
 */
void AnimatedMat_DrawAlphaStep(GlobalContext* globalCtx, AnimatedMaterial* matAnim, f32 alphaRatio, u32 step) {
    AnimatedMat_DrawMain(globalCtx, matAnim, alphaRatio, step, 3);
}

/**
 * Draws an animated material with an alpha ratio (0.0 - 1.0) and a step to only the OPA buffer.
 */
void AnimatedMat_DrawAlphaStepOpa(GlobalContext* globalCtx, AnimatedMaterial* matAnim, f32 alphaRatio, u32 step) {
    AnimatedMat_DrawMain(globalCtx, matAnim, alphaRatio, step, 1);
}

/**
 * Draws an animated material with an alpha ratio (0.0 - 1.0) and a step to only the XLU buffer.
 */
void AnimatedMat_DrawAlphaStepXlu(GlobalContext* globalCtx, AnimatedMaterial* matAnim, f32 alphaRatio, u32 step) {
    AnimatedMat_DrawMain(globalCtx, matAnim, alphaRatio, step, 2);
}

/**
 * Scene Draw Config 1:
 * Allows the usage of the animated material system in scenes.
 */
void Scene_DrawConfigMatAnim(GlobalContext* globalCtx) {
    AnimatedMat_Draw(globalCtx, globalCtx->sceneMaterialAnims);
}
