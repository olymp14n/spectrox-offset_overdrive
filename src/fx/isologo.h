#ifndef FX_ISOLOGO_H
#define FX_ISOLOGO_H

#include "../fw/image.h"

#define FX_ISO_PATTERN_NUM_COLS 18

typedef enum {
    FX_ISO_LOGO_MIN_Y,
    FX_ISO_LOGO_FINAL_Y,
    FX_ISO_LOGO_MAX_Y,
} FxIsoLogoYTypeEnum;

typedef struct {
    fw_image *image;
    fw_sprite_image *sprite;
    float y[3]; // See FxIsoLogoYTypeEnum.
} fx_isologo_letter;

typedef struct {
    float x;
    float amp;
    fw_vec4f offsets[FX_ISO_PATTERN_NUM_COLS];
    int numOffsets;
    float patternVerticalSpread;
    unsigned char outlineAlpha;
    FxIsoLogoYTypeEnum src;
    FxIsoLogoYTypeEnum dst;
    float p;
} fx_isologo_render_meta;

void fx_isologo_init(void);
void fx_isologo_renderBegin(void);
void fx_isologo_renderEnd(void);
void fx_isologo_render(char isMirrored, float p, FxIsoLogoYTypeEnum src, FxIsoLogoYTypeEnum dst, unsigned char r, unsigned char g, unsigned char b, unsigned char outlineAlpha);
void fx_isologo_renderPattern(fx_isologo_render_meta *meta);
void fx_isologo_renderPatternUpdate(fx_isologo_render_meta *meta, fw_timer_data *time);

#endif
