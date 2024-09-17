#ifndef FX_OVERLAY_H
#define FX_OVERLAY_H

#include "../resource.h"

void fx_overlay_renderBayerBorderLeftRight(float xSpread, float yOff, unsigned char r, unsigned char g, unsigned char b);
void fx_overlay_renderBayerBorderTopBottom(float ySpread, float xOff, unsigned char r, unsigned char g, unsigned char b);

void fx_overlay_renderOverlayExt(enum ImagesEnum imageEnum,
                              unsigned char r, unsigned char g, unsigned char b, unsigned char a,
                              float translateX, float translateY, float scaleX, float scaleY,
                              float repeatX, float offX, float repeatY, float offY);
void fx_overlay_renderOverlay(enum ImagesEnum imageEnum,
                              unsigned char r, unsigned char g, unsigned char b, unsigned char a,
                              float translateX, float translateY, float scaleX, float scaleY);

#endif
