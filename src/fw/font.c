#include "font.h"
#include "../config/config.h"
#include "projection.h"

void fw_font_initFontFace(fw_font_face *fontFace, fw_image *image) {
    fontFace->image = image;
    for (int i=0; i<fontFace->numGlyphs; i++) {
        fw_font_glyph *g = &fontFace->glyphs[i];
        g->left = g->x / (float)fontFace->textureWidth;
        g->top = g->y / (float)fontFace->textureHeight;
        g->right = (g->x + g->width) / (float)fontFace->textureWidth;
        g->bottom = (g->y + g->height) / (float)fontFace->textureHeight;
    }
}

int fw_font_calcLineWidth(const char string[], fw_font_face *fontFace) {
    int xadvanceTotal = 0;
    fw_font_glyph *glyph;

    for (int i=0; i<strlen(string); i++) {
        glyph = &fontFace->glyphs[(unsigned char)string[i]];
        xadvanceTotal += glyph->xadvance;
    }

    return xadvanceTotal;
}

static void fw_font_renderGlyph(fw_font_glyph *glyph, int x, int y) {
    int left = x + glyph->xoffset;
    int top = y + glyph->yoffset;
    int right = left + glyph->width;
    int bottom = top + glyph->height;

    glTexCoord2d(glyph->left, glyph->top);
    glVertex2i(left, top);

    glTexCoord2d(glyph->left, glyph->bottom);
    glVertex2i(left, bottom);

    glTexCoord2d(glyph->right, glyph->bottom);
    glVertex2i(right, bottom);

    glTexCoord2d(glyph->right, glyph->top);
    glVertex2i(right, top);
}

static void fw_font_renderTextLine(const char string[], fw_font_face *fontFace, int x, int y, int numCharsProcessed, int totalChars, float completion) {
    int xadvanceTotal = 0;
    fw_font_glyph *glyph;

    for (int i=0; i<strlen(string); i++) {

        if ( (i + numCharsProcessed)/(float)totalChars >= completion) {
            return;
        }

        glyph = &fontFace->glyphs[(unsigned char)string[i]];
        fw_font_renderGlyph(glyph, x+xadvanceTotal, y);
        xadvanceTotal += glyph->xadvance;
    }
}

static void fw_font_renderTextLines(char *strings[], int numLines, fw_font_face *fontFace, int x, int y, float completion) {
    int totalChars = 0;
    for (int i=0; i<numLines; i++) {
        totalChars += strlen(strings[i]);
    }

    int numCharsProcessed = 0;
    for (int i=0; i<numLines; i++) {
        fw_font_renderTextLine(strings[i], fontFace, x, y + i * fontFace->lineHeight, numCharsProcessed, totalChars, completion);
        numCharsProcessed += strlen(strings[i]);
    }
}

void fw_font_renderTextBegin(fw_font_face *fontFace) {
    glBindTexture(GL_TEXTURE_2D, fontFace->image->textureID);
    glEnable(GL_TEXTURE_2D);
    fw_projection_ortho();
}

void fw_font_renderTextEnd(void) {
    glDisable(GL_TEXTURE_2D);
}


void fw_font_renderSingleTextLine(const char string[], fw_font_face *fontFace, int x, int y) {
    glBegin(GL_QUADS);
    int xadvanceTotal = 0;
    fw_font_glyph *glyph;

    for (int i=0; i<strlen(string); i++) {
        glyph = &fontFace->glyphs[(unsigned char)string[i]];
        fw_font_renderGlyph(glyph, x+xadvanceTotal, y);
        xadvanceTotal += glyph->xadvance;
    }
    glEnd();
}

void fw_font_renderText(char *strings[], int numLines, fw_font_face *fontFace, int x, int y, float completion) {
    glBegin(GL_QUADS);
    fw_font_renderTextLines(strings, numLines, fontFace, x, y, completion);
    glEnd();
}

