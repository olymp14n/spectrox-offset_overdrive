#ifndef FW_RESOURCE_H
#define FW_RESOURCE_H

#include "fw/font.h"
#include "fw/image.h"

enum ImagesEnum {
    RES_IMG_FONT,
    RES_IMG_SPRITE_SHEET,
    RES_IMG_MOUNTAINS_LOGO,
    RES_IMG_TITLE,
    RES_IMG_TITLE_MASK,
    RES_IMG_TITLE_2,
    RES_IMG_TITLE_2_MASK,
    RES_IMG_TITLE_3,
    RES_IMG_TITLE_3_MASK,
    RES_IMG_TITLE_OUTLINE,
    RES_IMG_TITLE_2_OUTLINE,
    RES_IMG_TITLE_BG,
    RES_IMG_PARALLAX_LAYER_1,
    RES_IMG_PARALLAX_LAYER_2,
    RES_IMG_PARALLAX_LAYER_3,
    RES_IMG_STRIPE,
    RES_IMG_CHECKER,
    RES_IMG_CHECKER_DOT,
    RES_IMG_GRADIENT_GROUND,
    RES_IMG_GRADIENT_GREETINGS,
    RES_IMG_BAYER_VERTICAL,
    RES_IMG_BAYER_HORIZONTAL,
    RES_IMG_MOUNTAINS_TEXT,
    RES_IMG_TRELLIS,
    RES_IMG_MOUNTAINS,
    RES_IMG_MOUNTAINS_SUN,
    RES_IMG_HEX_FILLED,
    RES_IMG_HEX_OUTLINE,
    RES_IMG_CIRCLE_FILLED,
    RES_IMG_GRADIENT_TUNNEL,
    RES_IMG_BOUNCE_SPHERES_FLASH,

    // See https://stackoverflow.com/a/11090007/891846
    IMAGES_ENUM_SIZE
};

enum SpriteImagesEnum {
    RES_SPRITE_ISO_S,
    RES_SPRITE_ISO_P,
    RES_SPRITE_ISO_E,
    RES_SPRITE_ISO_C,
    RES_SPRITE_ISO_T,
    RES_SPRITE_ISO_R,
    RES_SPRITE_ISO_O,
    RES_SPRITE_ISO_X,

    SPRITE_IMAGES_ENUM_SIZE
};


void fw_resource_init();

fw_image *getImages();
fw_font_face *getFontFace();

#endif
