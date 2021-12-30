#ifndef _Z64_INTERFACE_H_
#define _Z64_INTERFACE_H_

/**
 * Button HUD Positions
 */
#define A_BUTTON_X 190
#define A_BUTTON_Y 23

#define B_BUTTON_X 181
#define B_BUTTON_Y 31

#define C_LEFT_BUTTON_X 240
#define C_LEFT_BUTTON_Y 31

#define C_DOWN_BUTTON_X 262
#define C_DOWN_BUTTON_Y 47

#define C_RIGHT_BUTTON_X 284
#define C_RIGHT_BUTTON_Y 31

// #define C_UP_BUTTON_X 254
// #define C_UP_BUTTON_Y 16

#define START_BUTTON_X 148
#define START_BUTTON_Y 29

/**
 * Counter HUD Positions
 */
#define KEY_X 280
#define KEY_Y 188

#define RUPEE_DIGIT_X 296
#define RUPEE_DIGIT_Y 206

#define HEARTS_X 24
#define HEARTS_Y 21

#define MAGIC_METER_X 23
#define MAGIC_METER_Y(rows) (33 + (rows * 11))

//! @TODO: Refactor all of this!
#define AREA_TITLE_BEGUNA_R 52
#define AREA_TITLE_BEGUNA_G 72
#define AREA_TITLE_BEGUNA_B 22
#define AREA_TITLE_GRAD_WIDTH 128
#define AREA_TITLE_GRAD_HEIGHT 1
#define AREA_TITLE_X_CENTER 160
#define AREA_TITLE_Y_CENTER 78
#define AREA_TITLE_GRAD_TILE_WIDTH 290
#define AREA_TITLE_GRAD_TILE_HEIGHT 22
#define AREA_TITLE_ALPHA 220
#define AREA_TITLE_Y (AREA_TITLE_Y_CENTER - (AREA_TITLE_GRAD_TILE_HEIGHT / 2))
#define AREA_TITLE_BORDER_TILE_WIDTH 320
#define AREA_TITLE_BORDER_TILE_HEIGHT 1
#define AREA_TITLE_WIDTH_MIN 120
#define AREA_TITLE_BORDER_MIN 60
#define AREA_TITLE_BORDER_UPPER_Y ((AREA_TITLE_Y_CENTER - (AREA_TITLE_GRAD_TILE_HEIGHT / 2)) - 1)
#define AREA_TITLE_BORDER_LOWER_Y (AREA_TITLE_Y_CENTER + (AREA_TITLE_GRAD_TILE_HEIGHT / 2))
typedef struct {
    void* buttonTex;
    void* emptyItemTex;
    s16 x, y;
    s16 sizeX, sizeY;
} InterfaceButtonInfo;
typedef enum {
    HUD_ITEM_BTN_B,
    HUD_ITEM_BTN_C_LEFT,
    HUD_ITEM_BTN_C_DOWN,
    HUD_ITEM_BTN_C_RIGHT,
    HUD_ITEM_BTN_MAX
} HUDItemButtonIndex;

// Draw helpers
#define DRAW_ANCHOR_C 0
#define DRAW_ANCHOR_U 1
#define DRAW_ANCHOR_R 2
#define DRAW_ANCHOR_D 4
#define DRAW_ANCHOR_L 8
#define DRAW_ANCHOR_UR DRAW_ANCHOR_U | DRAW_ANCHOR_R
#define DRAW_ANCHOR_DR DRAW_ANCHOR_D | DRAW_ANCHOR_R
#define DRAW_ANCHOR_DL DRAW_ANCHOR_D | DRAW_ANCHOR_L
#define DRAW_ANCHOR_UL DRAW_ANCHOR_U | DRAW_ANCHOR_L

typedef enum {
    DIGIT_DRAW_TYPE_AMMO,
    DIGIT_DRAW_TYPE_COUNTER
} DigitDrawType;

extern u16 _gfxTextureLoadWidth;
extern u16 _gfxTextureLoadHeight;

#define _gfxTextureLoad(pkt, timg, fmt, siz, width, height, empty)                                 \
    _gfxTextureLoadWidth = width;                                                                  \
    _gfxTextureLoadHeight = height;                                                                \
    gDPLoadTextureBlock(pkt++, timg, fmt, siz##empty, width, height, 0, G_TX_NOMIRROR | G_TX_WRAP, \
                        G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

#define _gfxTextureLoad_4b(pkt, timg, fmt, width, height, empty)                                                     \
    _gfxTextureLoadWidth = width;                                                                                    \
    _gfxTextureLoadHeight = height;                                                                                  \
    gDPLoadTextureBlock_4b(pkt++, timg, fmt, width, height, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, \
                           G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

#endif
