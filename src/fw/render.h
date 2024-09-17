#ifndef FW_RENDER_H
#define FW_RENDER_H

#include <windows.h>

void fw_render_init(RECT outputRect);
void fw_render_bindTextureBufferBegin();
void fw_render_bindTextureBufferEnd();
void fw_render_defaultBlendMode();
void fw_render_renderTextureBuffer();

void fw_render_viewportLowRes(int xOffset, int yOffset);
void fw_render_viewportHiRes(int xOffset, int yOffset);
void fw_render_clearHiRes(float r, float g, float b, float a);

#endif
