#include "love.h"

#include <math.h>
#include "../config/config.h"
#include "../resource.h"
#include "../fw/quad.h"
#include "../fw/projection.h"

#define FX_LOVE_TEXT_DELAY_SECONDS 12
#define FX_LOVE_RAINBOW_DELAY_SECONDS -12

static char *_texts[] = {
    "LOVE",
    "AND",
    "RESPECT",
    "GO OUT",
    "TO...",
};
static const int _textsLength = LEN(_texts);

static const fw_vec3i _palette[] = {
    {0, 36, 71},
    {10, 51, 82},
    {69, 68, 100},
    {99, 82, 122},
    {130, 98, 150},
    {140, 155, 190},
    {138, 179, 199},
    {149, 213, 212},
    {188, 239, 205},
    {242, 240, 179},
    {237, 237, 237},
    {214, 214, 214},
    {190, 190, 190},
    {140, 140, 140},
    {100, 100, 100},
    {70, 70, 70},
};
static const int _paletteLengh = LEN(_palette);

static const unsigned char _fadePaletteMap[] = {
    15,14,13,5,6,7,8,9,10
};

static const int _fadePaletteMapLength = LEN(_fadePaletteMap);

static void fx_love_renderText(const char string[], int x, int y, int palIdx) {
    glColor3ub(0,0,0);
    fw_font_renderSingleTextLine(string, getFontFace(), x+1,y+1);
    fw_font_renderSingleTextLine(string, getFontFace(), x+2,y+2);
    fw_font_renderSingleTextLine(string, getFontFace(), x+2,y+1);

    glColor3ub(_palette[palIdx].x,_palette[palIdx].y,_palette[palIdx].z);
    fw_font_renderSingleTextLine(string, getFontFace(), x,y);
}

void fx_love_renderTexts(float xoff, float time) {
    float delay = 10 + FX_LOVE_TEXT_DELAY_SECONDS;
    float speed = 12;

    int fadeIdxs[_textsLength];
    for (int i=0; i<_textsLength; i++) {
        fadeIdxs[i] = (-delay - i + time) * speed;
        if (fadeIdxs[i] >= _fadePaletteMapLength-1) {
            fadeIdxs[i] = _fadePaletteMapLength-1;
        }
    }

    fw_font_renderTextBegin(getFontFace());
    glTranslatef(xoff,70,0);


    if (fadeIdxs[0]>=0) {
        glPushMatrix();
        glTranslatef(20*sinf(time*2),10*sinf(1+time),1);
        fx_love_renderText(_texts[0],145,0,_fadePaletteMap[fadeIdxs[0]]);
        glPopMatrix();
    }

    if (fadeIdxs[1]>=0) {
        glPushMatrix();
        glTranslatef(20*sinf(2+time),10*sinf(1.3+time*1),1);
        fx_love_renderText(_texts[1],152,30,_fadePaletteMap[fadeIdxs[1]]);
        glPopMatrix();
    }

    if (fadeIdxs[2]>=0) {
        glPushMatrix();
        glTranslatef(30*sinf(1+time),10*sinf(1.6+time*1),1);
        fx_love_renderText(_texts[2],125,60,_fadePaletteMap[fadeIdxs[2]]);
        glPopMatrix();
    }

    if (fadeIdxs[3]>=0) {
        glPushMatrix();
        glTranslatef(10*sinf(3+time*2),10*sinf(1.9+time*1),1);
        fx_love_renderText(_texts[3],138,90,_fadePaletteMap[fadeIdxs[3]]);
        glPopMatrix();
    }

    if (fadeIdxs[4]>=0) {
        glPushMatrix();
        glTranslatef(20*sinf(2+time*1),10*sinf(2.1+time*1),1);
        fx_love_renderText(_texts[4],160,120,_fadePaletteMap[fadeIdxs[4]]);
        glPopMatrix();
    }

    fw_font_renderTextEnd();
}

void fx_love_renderBG(float xoff, float time) {
    glColor3ub(0,22,41);
    fw_projection_ortho();
    fw_quad_put(0,0,FW_RENDER_WIDTH, FW_RENDER_HEIGHT);

    fw_projection_perspective();
    glEnable(GL_SCISSOR_TEST);
    glTranslatef(0,0,-5);
    const int scissorHeight=5;
    const float t1 = time + FX_LOVE_RAINBOW_DELAY_SECONDS; // Time modifizierten damit die gewünschte Phase im Gesamtablauf passt.
    for (int i=0; i<FW_RENDER_HEIGHT; i+=scissorHeight) {
        glScissor(0,i,FW_RENDER_WIDTH,scissorHeight);
        const float f1 = sinf(i*.01 + t1*2);
        const float f2 = sinf(i*.005 + t1*1.5);

        glPushMatrix();
        glTranslatef(xoff,0,0);
        glScalef(2,1,1);
        glScalef(.5*(2+f1),1,1);
        glScalef(.5*(2+f2*(.25+f1)),3,1);

        for (int j=0; j<_paletteLengh; j++) {
            glColor3ub(_palette[j].x, _palette[j].y, _palette[j].z);
            fw_quad_put(-.8+j*.1, -1, 16+j*.1, 3);
        }

        glPopMatrix();
    }
    glDisable(GL_SCISSOR_TEST);
}
