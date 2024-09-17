#ifndef FW_PIXELS_H
#define FW_PIXELS_H

#include "types.h"

void fw_pixels_clear(unsigned char *pixels, int width, int height, int bpp);
void fw_pixels_blit(unsigned char *src, int srcWidth, int srcHeight,
                    unsigned char *dst, int dstWidth, int dstHeight,
                    int bpp,
                    int srcRegionX, int srcRegionY, int srcRegionWidth, int srcRegionHeight,
                    int dstOffsetX, int dstOffsetY);

/**
 * Blit pixels with additional constraints:
 *   - don't draw pixel if src pixel alpha is 0.
 *   - adjust color according to alpha channel of src pixels.
 *   - don't draw pixel if dst pixel has a protected color.
 */
void fw_pixels_blitExt(unsigned char *src, int srcWidth, int srcHeight,
                    unsigned char *dst, int dstWidth, int dstHeight,
                    int bpp,
                    int srcRegionX, int srcRegionY, int srcRegionWidth, int srcRegionHeight,
                    int srcScaleX, int srcScaleY,
                    int dstOffsetX, int dstOffsetY,
                    fw_vec3i *protectedColors, int protectedColorsLength);

void fw_pixels_createMask(unsigned char *src, unsigned char *dst, int width, int height, int bpp);

#endif
