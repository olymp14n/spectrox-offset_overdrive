#include "resource.h"
#include "fw/pixels.h"
#include "fw/image.h"
#include "fw/lut.h"
#include "fw/font.h"
#include "fw/draw.h"
#include "dat/spritesheet_pixels.h"
#include "dat/logo_pixels.h"
#include "dat/title_pixels.h"
#include "dat/title_pixels_2.h"
#include "dat/title_pixels_3.h"
#include "dat/title_outline_pixels.h"
#include "dat/title_outline_pixels_2.h"
#include "dat/iso_bg_pixels.h"
#include "dat/bayer_vertical_pixels.h"
#include "dat/bayer_horizontal_pixels.h"
#include "dat/parallax_text_pixels.h"
#include "dat/trellis_pixels.h"
#include "dat/mountains_pixels.h"
#include "dat/font_pixels.h"
#include "dat/font_meta.h"

static fw_image _images[IMAGES_ENUM_SIZE];
static fw_sprite_image _spriteImages[SPRITE_IMAGES_ENUM_SIZE];

static void generateImages(void) {
    fw_vec4i color = {255,255,255,255};
    int w, h, bpp=4;
    unsigned char *buffer;

    w = 97, h = 97, buffer = calloc(w*h*4, sizeof(unsigned char));
    fw_draw_circle(48,48,48, &color, buffer, w, h, bpp);
    fw_draw_fill(48,48, &color, buffer, w, h, bpp);
    fw_image_initRGBA(&_images[RES_IMG_PARALLAX_LAYER_1], w, h, buffer);

    w = 65, h = 65, buffer = calloc(w*h*4, sizeof(unsigned char));
    fw_draw_circle(32,32,32, &color, buffer, w, h, bpp);
    fw_draw_fill(32,32, &color, buffer, w, h, bpp);
    fw_image_initRGBA(&_images[RES_IMG_PARALLAX_LAYER_2], w, h, buffer);

    w = 33, h = 33, buffer = calloc(w*h*4, sizeof(unsigned char));
    fw_draw_circle(16,16,16, &color, buffer, w, h, bpp);
    fw_draw_fill(16,16, &color, buffer, w, h, bpp);
    fw_image_initRGBA(&_images[RES_IMG_PARALLAX_LAYER_3], w, h, buffer);

    w = 32, h = 32, buffer = calloc(w*h*4, sizeof(unsigned char));
    fw_draw_line(16,0,16,32,&color, buffer, w, h, bpp);
    fw_draw_fill(0,0, &color, buffer, w, h, bpp);
    fw_image_initRGBA(&_images[RES_IMG_STRIPE], w, h, buffer);

    // Beveled checker board.
    color = (fw_vec4i) {
        240,240,240,255
    };
    w = 64, h = 64, buffer = calloc(w*h*4, sizeof(unsigned char));

    fw_draw_fill_box(0,0,w/2-1,h/2-1, &color, buffer, w, h, bpp);
    fw_draw_fill_box(w/2,h/2,w-1,h-1, &color, buffer, w, h, bpp);

    color = (fw_vec4i) {
        150,150,150,255
    };
    fw_draw_line(w/2-1,0,w/2-1,h/2-1,&color, buffer, w, h, bpp);
    fw_draw_line(0,h/2-1,w/2-1,h/2-1,&color, buffer, w, h, bpp);
    fw_draw_line(w-1,h/2,w-1,h-1,&color, buffer, w, h, bpp);
    fw_draw_line(w/2,h-1,w-1,h-1,&color, buffer, w, h, bpp);

    color = (fw_vec4i) {
        255,255,255,255
    };
    fw_draw_line(0,0,h/2-1,0,&color, buffer, w, h, bpp);
    fw_draw_line(0,0,0,h/2-1,&color, buffer, w, h, bpp);
    fw_draw_line(w/2,h/2,w-1,h/2,&color, buffer, w, h, bpp);
    fw_draw_line(w/2,h/2,w/2,h-1,&color, buffer, w, h, bpp);

    fw_image_initRGBA(&_images[RES_IMG_CHECKER], w, h, buffer);

    // Checkerboard dot.
    color = (fw_vec4i) {
        240,240,240,255
    };
    w = 128, h = 128, buffer = calloc(w*h*4, sizeof(unsigned char));
    fw_draw_circle(32,32,8, &color, buffer, w, h, bpp);
    fw_draw_fill(32,32, &color, buffer, w, h, bpp);
    fw_image_initRGBA(&_images[RES_IMG_CHECKER_DOT], w, h, buffer);

    // Mountains sun.
    color = (fw_vec4i) {
        255,255,255,255
    };
    w = 32, h = 32, buffer = calloc(w*h*4, sizeof(unsigned char));
    fw_draw_circle(16,16,15, &color, buffer, w, h, bpp);
    fw_draw_fill(16,16, &color, buffer, w, h, bpp);
    fw_image_initRGBA(&_images[RES_IMG_MOUNTAINS_SUN], w, h, buffer);

    // Filled circle
    color = (fw_vec4i) {
        255,255,255,255
    };
    w = 128, h = 128, buffer = calloc(w*h*4, sizeof(unsigned char));
    fw_draw_circle(64,64,63, &color, buffer, w, h, bpp);
    fw_draw_fill(64,64, &color, buffer, w, h, bpp);
    fw_image_initRGBA(&_images[RES_IMG_CIRCLE_FILLED], w, h, buffer);

    // Hex filled.
    color = (fw_vec4i) {
        255,255,255,255
    };
    w = 16, h = 16, buffer = calloc(w*h*4, sizeof(unsigned char));
    fw_draw_line(8,1,1,4,&color, buffer, w, h, bpp);
    fw_draw_line(1,4,1,11,&color, buffer, w, h, bpp);
    fw_draw_line(1,11,8,14,&color, buffer, w, h, bpp);
    fw_draw_line(8,14,14,11,&color, buffer, w, h, bpp);
    fw_draw_line(14,11,14,4,&color, buffer, w, h, bpp);
    fw_draw_line(14,4,7,1,&color, buffer, w, h, bpp);
    fw_image_initRGBA(&_images[RES_IMG_HEX_OUTLINE], w, h, buffer);
    fw_draw_fill(8,8, &color, buffer, w, h, bpp);
    fw_image_initRGBA(&_images[RES_IMG_HEX_FILLED], w, h, buffer);


    // Ground gradient.
    w = 128, h = 128, buffer = calloc(w*h*4, sizeof(unsigned char));
    const int numSteps=8;
    const int lineWidth=8;
    int y=0;
    for (int i=0; i<numSteps; i++) {
        fw_vec4i c = {255,255,255,255-i*255/numSteps};
        for (int j=0; j<lineWidth; j++) {
            fw_draw_line(0,y,128,y,&c, buffer, w, h, bpp);
            y++;
        }

    }
    fw_image_initRGBA(&_images[RES_IMG_GRADIENT_GROUND], w, h, buffer);

    // Greetings gradient.
    w = 16, h = 16, buffer = calloc(w*h*4, sizeof(unsigned char));
    for (int i=0; i<h/2; i++) {
        int v = 255-i*255/h;
        fw_vec4i c = {v,v,v,255};
        fw_draw_line(0,i,16,i,&c, buffer, w, h, bpp);
    }
    for (int i=h/2; i<h; i++) {
        int v = i*255/h;
        fw_vec4i c = {v,v,v,255};
        fw_draw_line(0,i,16,i,&c, buffer, w, h, bpp);
    }
    fw_image_initRGBA(&_images[RES_IMG_GRADIENT_GREETINGS], w, h, buffer);

    // Tunnel BG gradient.
    fw_vec4i pal[] = {
        {69,15,63,255},
        {77,17,66,255},
        {85,18,68,255},
        {93,19,70,255},
        {101,21,72,255},
        {109,22,74,255},
        {117,23,76,255},
        {124,24,78,255},
    };
    w = 1, h = 270, buffer = calloc(w*h*4, sizeof(unsigned char));

    const int palSize = LEN(pal);
    const int gradHeight = 24;
    const int offY = (h - (palSize*gradHeight))/2;

    fw_draw_fill(0,0, &pal[0],buffer,w,h,bpp);
    for (int i=0; i<palSize; i++) {

        if (i < 2 || i > palSize-2) {
            fw_draw_pixel(0,offY+i*gradHeight-2, &pal[i],buffer,w,h,bpp);
            fw_draw_pixel(0,offY+i*gradHeight-3, &pal[i],buffer,w,h,bpp);
            fw_draw_pixel(0,offY+i*gradHeight-5, &pal[i],buffer,w,h,bpp);
        }

        fw_draw_fill_box(0,offY+i*gradHeight, 0,offY+(i*gradHeight)+gradHeight, &pal[i],buffer,w,h,bpp);
    }
    fw_draw_fill(0,h-1, &pal[palSize-1],buffer,w,h,bpp);
    fw_image_initRGBA(&_images[RES_IMG_GRADIENT_TUNNEL], w, h, buffer);

    // Bounce spheres ground flash gradient.
    w = 512, h = 128, buffer = calloc(w*h*4, sizeof(unsigned char));
    fw_draw_gradient_oval(buffer, w,h, bpp);
    fw_image_initRGBA(&_images[RES_IMG_BOUNCE_SPHERES_FLASH], w, h, buffer);

    // White mask for title logo.
    w = TITLE_PIXELS_WIDTH, h = TITLE_PIXELS_HEIGHT, buffer = calloc(w*h*4, sizeof(unsigned char));
    fw_pixels_createMask(_titlePixels, buffer, w, h, bpp);
    fw_image_initRGBA(&_images[RES_IMG_TITLE_MASK], w, h, buffer);

    // White mask for title logo top part.
    w = TITLE_PIXELS_2_WIDTH, h = TITLE_PIXELS_2_HEIGHT, buffer = calloc(w*h*4, sizeof(unsigned char));
    fw_pixels_createMask(_titlePixels2, buffer, w, h, bpp);
    fw_image_initRGBA(&_images[RES_IMG_TITLE_2_MASK], w, h, buffer);

    // White mask for title logo bottom part.
    w = TITLE_PIXELS_3_WIDTH, h = TITLE_PIXELS_3_HEIGHT, buffer = calloc(w*h*4, sizeof(unsigned char));
    fw_pixels_createMask(_titlePixels3, buffer, w, h, bpp);
    fw_image_initRGBA(&_images[RES_IMG_TITLE_3_MASK], w, h, buffer);
}

void fw_resource_init() {
    fw_lut_init();

    // Image.
    fw_image_initRGBA(&_images[RES_IMG_SPRITE_SHEET], SPRITESHEET_PIXELS_WIDTH, SPRITESHEET_PIXELS_HEIGHT, _spritesheetPixels);
    _images[RES_IMG_SPRITE_SHEET].sprites = _spriteImages;
    _images[RES_IMG_SPRITE_SHEET].numSprites = LEN(_spriteImages);
    fw_image_initSpriteImage(&_images[RES_IMG_SPRITE_SHEET], RES_SPRITE_ISO_S, 1,1, 28, 32);
    fw_image_initSpriteImage(&_images[RES_IMG_SPRITE_SHEET], RES_SPRITE_ISO_P, 30,1, 28, 32);
    fw_image_initSpriteImage(&_images[RES_IMG_SPRITE_SHEET], RES_SPRITE_ISO_E, 59,1, 28, 32);
    fw_image_initSpriteImage(&_images[RES_IMG_SPRITE_SHEET], RES_SPRITE_ISO_C, 88,1, 28, 32);
    fw_image_initSpriteImage(&_images[RES_IMG_SPRITE_SHEET], RES_SPRITE_ISO_T, 1,34, 28, 32);
    fw_image_initSpriteImage(&_images[RES_IMG_SPRITE_SHEET], RES_SPRITE_ISO_R, 30,34, 28, 32);
    fw_image_initSpriteImage(&_images[RES_IMG_SPRITE_SHEET], RES_SPRITE_ISO_O, 59,34, 28, 32);
    fw_image_initSpriteImage(&_images[RES_IMG_SPRITE_SHEET], RES_SPRITE_ISO_X, 88,34, 28, 32);

    fw_image_initRGBA(&_images[RES_IMG_MOUNTAINS_LOGO], LOGO_PIXELS_WIDTH, LOGO_PIXELS_HEIGHT, _logoPixels);
    fw_image_initRGBA(&_images[RES_IMG_TITLE], TITLE_PIXELS_WIDTH, TITLE_PIXELS_HEIGHT, _titlePixels);
    fw_image_initRGBA(&_images[RES_IMG_TITLE_2], TITLE_PIXELS_2_WIDTH, TITLE_PIXELS_2_HEIGHT, _titlePixels2);
    fw_image_initRGBA(&_images[RES_IMG_TITLE_3], TITLE_PIXELS_3_WIDTH, TITLE_PIXELS_3_HEIGHT, _titlePixels3);
    fw_image_initRGBA(&_images[RES_IMG_TITLE_BG], ISO_BG_PIXELS_WIDTH, ISO_BG_PIXELS_HEIGHT, _isoBGPixels);
    fw_image_initRGBA(&_images[RES_IMG_TITLE_OUTLINE], TITLE_OUTLINE_PIXELS_WIDTH, TITLE_OUTLINE_PIXELS_HEIGHT, _titleOutlinePixels);
    fw_image_initRGBA(&_images[RES_IMG_TITLE_2_OUTLINE], TITLE_OUTLINE_PIXELS_2_WIDTH, TITLE_OUTLINE_PIXELS_2_HEIGHT, _titleOutlinePixels2);
    fw_image_initRGBA(&_images[RES_IMG_BAYER_VERTICAL], BAYER_VERTICAL_PIXELS_WIDTH, BAYER_VERTICAL_PIXELS_HEIGHT, _bayerVerticalPixels);

    // Clamp vertically to avoid repeat artifacts when translating to non-integer positions.
    fw_image_initRGBA(&_images[RES_IMG_BAYER_HORIZONTAL], BAYER_HORIZONTAL_PIXELS_WIDTH, BAYER_HORIZONTAL_PIXELS_HEIGHT, _bayerHorizontalPixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    fw_image_initRGBA(&_images[RES_IMG_MOUNTAINS_TEXT], PARALLAX_TEXT_PIXELS_WIDTH, PARALLAX_TEXT_PIXELS_HEIGHT, _parallaxTextPixels);
    fw_image_initRGBA(&_images[RES_IMG_TRELLIS], TRELLIS_PIXELS_WIDTH, TRELLIS_PIXELS_HEIGHT, _trellisPixels);
    fw_image_initRGBA(&_images[RES_IMG_MOUNTAINS], MOUNTAINS_PIXELS_WIDTH, MOUNTAINS_PIXELS_HEIGHT, _mountainsPixels);

    // Font.
    fw_image_initRGBA(&_images[RES_IMG_FONT], FONT_PIXELS_WIDTH, FONT_PIXELS_HEIGHT, _fontPixels);
    fw_font_initFontFace(&_fontFace, &_images[RES_IMG_FONT]);

    // Generate images.
    generateImages();
}

fw_image *getImages() {
    return _images;
}


fw_font_face *getFontFace() {
    return &_fontFace;
}
