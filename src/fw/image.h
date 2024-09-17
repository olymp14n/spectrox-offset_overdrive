#ifndef FW_IMAGE_H
#define FW_IMAGE_H

#include <GL/gl.h>
#include "types.h"

typedef struct {
    int width;
    int height;
    fw_vec2i topLeft;
    fw_vec2i bottomRight;
    fw_vec2f texTopLeft;
    fw_vec2f texBottomRight;
} fw_sprite_image;

typedef struct {
    int width;
    int height;
    int bytesPerPixel;
    GLuint textureID;
    unsigned char *pixels;
    int numSprites;
    fw_sprite_image *sprites;
} fw_image;

void fw_image_initRGBA(fw_image *image, int width, int height, unsigned char *pixels);
void fw_image_clear(fw_image *image);
void fw_image_blit(fw_image *src, fw_image *dst, int srcRegionX, int srcRegionY, int srcRegionWidth, int srcRegionHeight, int dstX, int dstY);

/**
 * Blit image with additional constraints:
 *   - don't draw pixel if src pixel alpha is 0.
 *   - adjust color according to alpha channel of src image pixels.
 *   - don't draw pixel if dst pixel has a protected color.
 */
void fw_image_blitExt(fw_image *src, fw_image *dst,
                      int srcRegionX, int srcRegionY, int srcRegionWidth, int srcRegionHeight,
                      int srcScaleX, int srcScaleY,
                      int dstX, int dstY,
                      fw_vec3i *protectedColors, int protectedColorsLength);

void fw_image_initSpriteImage(fw_image *image, int spriteIndex, int x, int y, int width, int height);
void fw_image_putSpriteImage(fw_sprite_image *sprite);

void fw_image_putImageExt(fw_image *image, float xrepeat, float xoff, float yrepeat, float yoff);
void fw_image_putImage(fw_image *image);

void fw_image_renderBegin(fw_image *image, float alpha);
void fw_image_renderEnd();

void fw_image_scaleCenter(fw_image *image, float scale);

/**
 * Aktive Textur mit Pixeln aus dem übergebenen Image aktualisieren.
 */
void fw_image_updateTexturePixels(fw_image *image);

#endif
