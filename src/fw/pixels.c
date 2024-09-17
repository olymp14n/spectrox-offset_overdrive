#include "pixels.h"
#include <string.h>

void fw_pixels_clear(unsigned char *pixels, int width, int height, int bpp) {
    memset(pixels, 0, width * height * bpp);
}

void fw_pixels_blit(unsigned char *src, int srcWidth, int srcHeight,
                    unsigned char *dst, int dstWidth, int dstHeight,
                    int bpp,
                    int srcRegionX, int srcRegionY, int srcRegionWidth, int srcRegionHeight,
                    int dstOffsetX, int dstOffsetY) {

    if (srcWidth == dstWidth
            && srcHeight == dstHeight
            && srcRegionX == 0
            && srcRegionY == 0
            && srcRegionWidth == srcWidth
            && srcRegionHeight == srcHeight
            && dstOffsetX == 0
            && dstOffsetY == 0) {
        memcpy(dst, src, srcWidth * srcHeight * bpp);
        return;
    }

    for (int y=0; y<srcRegionHeight; y++) {

        const int dstY = dstOffsetY + y;
        if (dstY < 0 || dstY >= dstHeight) {
            continue;
        }
        const int srcY = srcRegionY + y;
        if (srcY < 0 || srcY >= srcHeight) {
            continue;
        }

        for (int x=0; x<srcRegionWidth; x++) {

            const int dstX = dstOffsetX + x;
            if (dstX < 0 || dstX >= dstWidth) {
                continue;
            }
            const int srcX = srcRegionX + x;
            if (srcX < 0 || srcX >= srcWidth) {
                continue;
            }

            const int srcIdx = (srcX + srcY*srcWidth) * bpp;
            const int dstIdx = (dstX + dstY*dstWidth) * bpp;

            for (int i=0; i<bpp; i++) {
                dst[dstIdx+i] = src[srcIdx+i];
            }
        }
    }
}

void fw_pixels_blitExt(unsigned char *src, int srcWidth, int srcHeight,
                       unsigned char *dst, int dstWidth, int dstHeight,
                       int bpp,
                       int srcRegionX, int srcRegionY, int srcRegionWidth, int srcRegionHeight,
                       int srcScaleX, int srcScaleY,
                       int dstOffsetX, int dstOffsetY,
                       fw_vec3i *protectedColors, int protectedColorsLength) {

    for (int y=0; y<srcRegionHeight*srcScaleY; y++) {

        const int dstY = dstOffsetY + y;
        if (dstY < 0 || dstY >= dstHeight) {
            continue;
        }
        const int srcY = srcRegionY + y/srcScaleY;
        if (srcY < 0 || srcY >= srcHeight) {
            continue;
        }

        for (int x=0; x<srcRegionWidth*srcScaleX; x++) {

            const int dstX = dstOffsetX + x;
            if (dstX < 0 || dstX >= dstWidth) {
                continue;
            }
            const int srcX = srcRegionX + x/srcScaleX;
            if (srcX < 0 || srcX >= srcWidth) {
                continue;
            }

            const int srcIdx = (srcX + srcY*srcWidth) * bpp;

            // Don't draw fully transparent pixels.
            if (bpp == 4 && src[srcIdx+3] == 0) {
                continue;
            }

            const int dstIdx = (dstX + dstY*dstWidth) * bpp;

            for (int i=0; i<protectedColorsLength; i++) {
                if (bpp >= 3 && dst[dstIdx] == protectedColors[i].x
                        && dst[dstIdx+1] == protectedColors[i].y
                        && dst[dstIdx+2] == protectedColors[i].z) {
                    goto skip;
                }
            }

            // No alpha channel or alpha is fully opaque: just copy all channels of pixel
            if (bpp <= 3 || src[srcIdx+3] == 255) {
                for (int i=0; i<bpp; i++) {
                    dst[dstIdx+i] = src[srcIdx+i];
                }
                continue;
            }

            // If we get here there is an alpha channel to be considered in the source bitmap.
            const float p = src[srcIdx+3]/255.f;
            dst[dstIdx+0] = p*src[srcIdx+0] + (1.f-p)*dst[dstIdx+0];
            dst[dstIdx+1] = p*src[srcIdx+1] + (1.f-p)*dst[dstIdx+1];
            dst[dstIdx+2] = p*src[srcIdx+2] + (1.f-p)*dst[dstIdx+2];
            dst[dstIdx+3] = 255;

            // Skip label to exit nested loop.
            skip:;
        }
    }
}

void fw_pixels_createMask(unsigned char *src, unsigned char *dst, int width, int height, int bpp) {
    for (int y=0; y < height; y++) {
        for (int x=0; x < width; x++) {
            int idx = (x + y*width) * bpp;
            if (src[idx+3] != 0) {
                dst[idx+0] = 255;
                dst[idx+1] = 255;
                dst[idx+2] = 255;
                dst[idx+3] = 255;
            } else {
                dst[idx+0] = 0;
                dst[idx+1] = 0;
                dst[idx+2] = 0;
                dst[idx+3] = 0;
            }
        }
    }
}
