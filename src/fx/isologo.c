#include "isologo.h"
#include <math.h>
#include "../config/config.h"
#include "../fw/types.h"
#include "../fw/math.h"
#include "../fw/easing.h"
#include "../resource.h"


typedef struct {
    float x;
    float y;
    int isMirrored;
} fx_isologo_pattern_col;

static const float _pi2 = 2.f*M_PI;

static fx_isologo_letter _letters[8];
static const int _numLetters = LEN(_letters);

static fx_isologo_pattern_col _patternCols[FX_ISO_PATTERN_NUM_COLS];
static const int _numPatternCols = LEN(_patternCols);

static void fx_isologo_initPatternCols() {
    for (int i=0; i<_numPatternCols; i++) {
        _patternCols[i].isMirrored = i%2 != 0;
    }
}

void fx_isologo_init(void) {
    fw_image *img = &getImages()[RES_IMG_SPRITE_SHEET];

    int idx = 0;
    _letters[idx].image = img;
    _letters[idx].sprite = &img->sprites[RES_SPRITE_ISO_X];
    _letters[idx].y[FX_ISO_LOGO_MIN_Y] = 143;
    _letters[idx].y[FX_ISO_LOGO_FINAL_Y] = _letters[idx].y[FX_ISO_LOGO_MIN_Y] + 15;
    _letters[idx].y[FX_ISO_LOGO_MAX_Y] = _letters[idx].y[FX_ISO_LOGO_MIN_Y] + 38;

    idx++;
    _letters[idx].image = img;
    _letters[idx].sprite = &img->sprites[RES_SPRITE_ISO_O];
    _letters[idx].y[FX_ISO_LOGO_MIN_Y] = 124;
    _letters[idx].y[FX_ISO_LOGO_FINAL_Y] = _letters[idx].y[FX_ISO_LOGO_MIN_Y] + 10;
    _letters[idx].y[FX_ISO_LOGO_MAX_Y] = _letters[idx].y[FX_ISO_LOGO_MIN_Y] + 28;

    idx++;
    _letters[idx].image = img;
    _letters[idx].sprite = &img->sprites[RES_SPRITE_ISO_R];
    _letters[idx].y[FX_ISO_LOGO_MIN_Y]= 101;
    _letters[idx].y[FX_ISO_LOGO_FINAL_Y] = _letters[idx].y[FX_ISO_LOGO_MIN_Y] + 5;
    _letters[idx].y[FX_ISO_LOGO_MAX_Y] = _letters[idx].y[FX_ISO_LOGO_MIN_Y] + 18;

    idx++;
    _letters[idx].image = img;
    _letters[idx].sprite = &img->sprites[RES_SPRITE_ISO_T];
    _letters[idx].y[FX_ISO_LOGO_MIN_Y] = 77;
    _letters[idx].y[FX_ISO_LOGO_FINAL_Y] = _letters[idx].y[FX_ISO_LOGO_MIN_Y] + 4;
    _letters[idx].y[FX_ISO_LOGO_MAX_Y] = _letters[idx].y[FX_ISO_LOGO_MIN_Y] + 8;

    idx++;
    _letters[idx].image = img;
    _letters[idx].sprite = &img->sprites[RES_SPRITE_ISO_C];
    _letters[idx].y[FX_ISO_LOGO_MIN_Y] = 59;
    _letters[idx].y[FX_ISO_LOGO_FINAL_Y] = _letters[idx].y[FX_ISO_LOGO_MIN_Y] - 4;
    _letters[idx].y[FX_ISO_LOGO_MAX_Y] = _letters[idx].y[FX_ISO_LOGO_MIN_Y] - 8;

    idx++;
    _letters[idx].image = img;
    _letters[idx].sprite = &img->sprites[RES_SPRITE_ISO_E];
    _letters[idx].y[FX_ISO_LOGO_MIN_Y] = 41;
    _letters[idx].y[FX_ISO_LOGO_FINAL_Y] = _letters[idx].y[FX_ISO_LOGO_MIN_Y] - 12;
    _letters[idx].y[FX_ISO_LOGO_MAX_Y] = _letters[idx].y[FX_ISO_LOGO_MIN_Y] - 24;

    idx++;
    _letters[idx].image = img;
    _letters[idx].sprite = &img->sprites[RES_SPRITE_ISO_P];
    _letters[idx].y[FX_ISO_LOGO_MIN_Y] = 18;
    _letters[idx].y[FX_ISO_LOGO_FINAL_Y]= _letters[idx].y[FX_ISO_LOGO_MIN_Y] - 16;
    _letters[idx].y[FX_ISO_LOGO_MAX_Y]= _letters[idx].y[FX_ISO_LOGO_MIN_Y] - 32;

    idx++;
    _letters[idx].image = img;
    _letters[idx].sprite = &img->sprites[RES_SPRITE_ISO_S];
    _letters[idx].y[FX_ISO_LOGO_MIN_Y] = 0;
    _letters[idx].y[FX_ISO_LOGO_FINAL_Y] = _letters[idx].y[FX_ISO_LOGO_MIN_Y] - 24;
    _letters[idx].y[FX_ISO_LOGO_MAX_Y] = _letters[idx].y[FX_ISO_LOGO_MIN_Y] - 48;


    fx_isologo_initPatternCols();
}

static void fx_isologo_renderLetter(int idx, float x, float y) {
    glPushMatrix();
    glTranslatef(x,y,0);
    fw_image_putSpriteImage(_letters[idx].sprite);
    glPopMatrix();
}

void fx_isologo_renderBegin(void) {
    fw_image_renderBegin(_letters[0].image,1);
}

void fx_isologo_renderEnd(void) {
    fw_image_renderEnd();
}

void fx_isologo_render(char isMirrored, float p, FxIsoLogoYTypeEnum src, FxIsoLogoYTypeEnum dst,
                       unsigned char r, unsigned char g, unsigned char b,
                       unsigned char outlineAlpha) {
    float yAdd = 0;
    const float p1 = 1.f-p;
    for (int i=0; i<_numLetters; i++) {
        glPushMatrix();
        {
            if (isMirrored) {
                glTranslated(28,0,0);
                glScalef(-1,1,1);
            }

            yAdd = p1*_letters[i].y[src] + p*_letters[i].y[dst];
            glTranslatef(0,yAdd,0);

            if (outlineAlpha > 0) {
                glColor4ub(0,0,0,outlineAlpha);
                fx_isologo_renderLetter(i, -1,0);
                fx_isologo_renderLetter(i, 1,0);
                fx_isologo_renderLetter(i, 0,-1);
                fx_isologo_renderLetter(i, 0,1);
            }

            glColor3ub(r,g,b);
            fx_isologo_renderLetter(i, 0,0);
        }
        glPopMatrix();
    }
}



void fx_isologo_renderPatternUpdate(fx_isologo_render_meta *meta, fw_timer_data *time) {

    // Bring x coordinate in range.
    const float xoff = fmodf(meta->x, 27 * _numPatternCols);

    for (int i=0; i<_numPatternCols; i++) {
        _patternCols[i].x = xoff + i*27;
        _patternCols[i].y = meta->offsets[i].y + meta->amp*sinf((i/(float)_numPatternCols)*_pi2 + time->elapsed*4.f);
        if (_patternCols[i].x < -28) {
            _patternCols[i].x += 27 * _numPatternCols;
        }
        if (_patternCols[i].x > 27 * (_numPatternCols-1)) {
            _patternCols[i].x -= 27 * _numPatternCols;
        }
    }
}

void fx_isologo_renderPattern(fx_isologo_render_meta *meta) {

    for (int i=0; i<_numPatternCols; i++) {
        glPushMatrix();
        {
            float y = _patternCols[i].y - meta->offsets[i].y;
            y = 1.f-fabs(y/(40.f * 1.1));
            y = fw_math_clamp(y, 0,1);
            y = QuadraticEaseOut(y);
            //y = ((int)(.5+y*32.f))/32.f;


            unsigned char r  = 255*y;
            unsigned char g  = 255*y;
            unsigned char b  = 255*y;

            glPushMatrix();
            glTranslatef(_patternCols[i].x, _patternCols[i].y + meta->patternVerticalSpread, 0);
            fx_isologo_render(_patternCols[i].isMirrored, meta->p, meta->src, meta->dst, r,g,b, meta->outlineAlpha);
            glPopMatrix();

            glPushMatrix();
            glTranslatef(_patternCols[i].x, _patternCols[i].y, 0);
            fx_isologo_render(_patternCols[i].isMirrored, meta->p, meta->src, meta->dst, r,g,b, meta->outlineAlpha);
            glPopMatrix();

            glPushMatrix();
            glTranslatef(_patternCols[i].x, _patternCols[i].y - meta->patternVerticalSpread, 0);
            fx_isologo_render(_patternCols[i].isMirrored, meta->p, meta->src, meta->dst, r,g,b, meta->outlineAlpha);
            glPopMatrix();
        }
        glPopMatrix();
    }
}
