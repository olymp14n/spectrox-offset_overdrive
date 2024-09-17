#ifndef FX_TEXT_3D_H
#define FX_TEXT_3D_H

#include "../fw/types.h"
#include "../fw/font.h"

typedef struct {
    char *text;
    fw_font_face *fontFace;
    fw_vec3f pos;
    fw_vec4f rot;
    float scale;
    fw_vec4i color;
    int isPaletteEnabled;
    fw_vec4i *palette;
    int paletteIndex;
} fx_text_3d_params;

void fx_text_3d_render(fx_text_3d_params *params);

#endif
