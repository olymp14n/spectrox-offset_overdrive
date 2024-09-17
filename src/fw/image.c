#include "image.h"
#include "types.h"
#include "../config/config.h"
#include "projection.h"
#include "pixels.h"

void fw_image_initRGBA(fw_image *image, int width, int height, unsigned char *pixels) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    image->width = width;
    image->height = height;
    image->bytesPerPixel = 4;
    image->textureID = textureID;
    image->pixels = pixels;
}

void fw_image_clear(fw_image *image) {
    fw_pixels_clear(image->pixels, image->width, image->height, image->bytesPerPixel);
}

void fw_image_blit(fw_image *src, fw_image *dst, int srcRegionX, int srcRegionY, int srcRegionWidth, int srcRegionHeight, int dstX, int dstY) {
    fw_pixels_blit(src->pixels, src->width, src->height,
                   dst->pixels, dst->width, dst->height,
                   src->bytesPerPixel,
                   srcRegionX,srcRegionY, srcRegionWidth, srcRegionHeight,
                   dstX,dstY);
}

void fw_image_blitExt(fw_image *src, fw_image *dst,
                      int srcRegionX, int srcRegionY, int srcRegionWidth, int srcRegionHeight,
                      int srcScaleX, int srcScaleY,
                      int dstX, int dstY, fw_vec3i *protectedColors, int protectedColorsLength) {
    fw_pixels_blitExt(src->pixels, src->width, src->height,
                   dst->pixels, dst->width, dst->height,
                   src->bytesPerPixel,
                   srcRegionX,srcRegionY, srcRegionWidth, srcRegionHeight,
                   srcScaleX, srcScaleY,
                   dstX,dstY,
                   protectedColors, protectedColorsLength);
}


void fw_image_initSpriteImage(fw_image *image, int spriteIndex, int x, int y, int width, int height) {
    fw_sprite_image *sprite = &image->sprites[spriteIndex];
    sprite->width = width;
    sprite->height = height;
    sprite->topLeft.x = x;
    sprite->topLeft.y = y;
    sprite->bottomRight.x = x + width;
    sprite->bottomRight.y = y + height;
    sprite->texTopLeft.x = x / (float)image->width;
    sprite->texTopLeft.y = y / (float)image->height;
    sprite->texBottomRight.x = (x + width)/(float)image->width;
    sprite->texBottomRight.y = (y + height)/(float)image->height;
}

void fw_image_putSpriteImage(fw_sprite_image *sprite) {
    glBegin(GL_QUADS);

    glTexCoord2f(sprite->texTopLeft.x, sprite->texTopLeft.y);
    glVertex2i(0, 0);

    glTexCoord2f(sprite->texTopLeft.x, sprite->texBottomRight.y);
    glVertex2i(0, sprite->height);

    glTexCoord2f(sprite->texBottomRight.x, sprite->texBottomRight.y);
    glVertex2i(sprite->width, sprite->height);

    glTexCoord2f(sprite->texBottomRight.x, sprite->texTopLeft.y);
    glVertex2i(sprite->width, 0);

    glEnd();
}

void fw_image_putImageExt(fw_image *image, float xrepeat, float xoff, float yrepeat, float yoff) {
    glBegin(GL_QUADS);
    glTexCoord2d(xoff, yoff);
    glVertex2i(0, 0);

    glTexCoord2d(xoff, yrepeat + yoff);
    glVertex2i(0, image->height * yrepeat);

    glTexCoord2d(xoff + xrepeat, yrepeat + yoff);
    glVertex2i(image->width * xrepeat, image->height * yrepeat);

    glTexCoord2d(xoff + xrepeat, yoff);
    glVertex2i(image->width * xrepeat, 0);
    glEnd();
}

void fw_image_putImage(fw_image *image) {
    fw_image_putImageExt(image, 1,0,1,0);
}

void fw_image_renderBegin(fw_image *image, float alpha) {
    glColor4f(1,1,1, alpha);
    fw_projection_ortho();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, image->textureID);
    glPushMatrix();
}

void fw_image_renderEnd() {
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void fw_image_scaleCenter(fw_image *image, float scale) {
    const float w2 = image->width * .5;
    const float h2 = image->height * .5;
    glTranslatef(w2,h2,0);
    glScalef(scale, scale, 1);
    glTranslatef(-w2,-h2,0);
}

void fw_image_updateTexturePixels(fw_image *image) {
    glTexSubImage2D(GL_TEXTURE_2D, 0,0,0, image->width, image->height, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);
}
