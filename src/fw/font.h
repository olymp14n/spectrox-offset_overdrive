#ifndef FW_FONT_H
#define FW_FONT_H

#include "types.h"
#include "image.h"

typedef struct {
    int x;
    int y;
    int width;
    int height;
    int xoffset;
    int yoffset;
    int xadvance;
    float top;
    float left;
    float bottom;
    float right;
} fw_font_glyph;

typedef struct {
    int outline;
    int lineHeight;
    int textureWidth;
    int textureHeight;
    fw_font_glyph *glyphs;
    int numGlyphs;
    fw_image *image;
} fw_font_face;

void fw_font_initFontFace(fw_font_face *fontFace, fw_image *image);
int fw_font_calcLineWidth(const char string[], fw_font_face *fontFace);

void fw_font_renderTextBegin(fw_font_face *fontFace);
void fw_font_renderText(char *strings[], int numLines, fw_font_face *fontFace, int x, int y, float completion);
void fw_font_renderSingleTextLine(const char string[], fw_font_face *fontFace, int x, int y);
void fw_font_renderTextEnd(void);

#endif
