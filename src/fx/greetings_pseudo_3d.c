#include "greetings_pseudo_3d.h"

#include <math.h>
#include "../config/config.h"
#include "../resource.h"
#include "../fw/stencil.h"
#include "../fw/quad.h"
#include "../fw/projection.h"
#include "../fw/render.h"

static char *_greetingsTexts[] = {
    "ABYSS * CENSOR DESIGN * ISTARI * RAZOR 1911 * TEK",
    "ALCATRAZ * COMPLEX * LEMON. * REBELS * TITAN",
    "ANDROMEDA * DESIRE * LOGICOMA * SCOOPEX * TPOLM",
    "ARTSTATE * FAIRLIGHT * MELON * SMFX * TRBL * TRSI",
    "ASD * FARBRAUSCH * MFX * SPACEBALLS * TRIAD",
    "BONZAI * FROZAK * MOODS PLATEAU * SPREADPOINT",
    "CALODOX * HAUJOBB * OXYGENE * SCA * VANTAGE",
};
static int _numGreetingsTexts = LEN(_greetingsTexts);

typedef struct {
    float startX;
    float x;
    float y;
    float scale;
    int direction;
    float speed;
} greeting_line_meta;

const float sl = .5;
static greeting_line_meta _greetingLineMetas[] = {
    {FW_RENDER_WIDTH  , 0 ,   -5+0*5   ,  1.0  ,  -1  ,  sl*97  }  ,
    {FW_RENDER_WIDTH  , 0 ,   18+1*5   ,  1.5  ,  -1  ,  sl*119 }  ,
    {FW_RENDER_WIDTH  , 0 ,   45+2*5   ,  2.0  ,  -1  ,  sl*160 }  ,
    {FW_RENDER_WIDTH  , 0 ,   82+3*5   ,  2.5  ,  -1  ,  sl*186 }  ,
    {FW_RENDER_WIDTH  , 0 ,   128+4*5  ,  2.0  ,  -1  ,  sl*151 }  ,
    {FW_RENDER_WIDTH  , 0 ,   165+5*5  ,  1.5  ,  -1  ,  sl*132 }  ,
    {FW_RENDER_WIDTH  , 0 ,   198+6*5  ,  1.0  ,  -1  ,  sl*98  }  ,
};

static fw_vec4i _shadowColorSilver   = {76   ,   17   ,   56   ,   255};
static fw_vec4i _baseColorSilver     = {168  ,   181  ,   178  ,   255};
static fw_vec4i _blendColorSilver    = {168  ,   181  ,   178  ,   150};
static fw_vec4i _shadowColorGold     = {165  ,   48   ,   48   ,   255};
static fw_vec4i _baseColorGold       = {255  ,   187  ,   49   ,   255};
static fw_vec4i _blendColorGold      = {255  ,   187  ,   49   ,   100};

static void renderGreetingsLine(int idx, fw_vec4i *color, int count, float xoff, float yoff) {
    fw_font_renderTextBegin(getFontFace());
    {
        glTranslatef(20,13,0);
        glRotatef( -4*(idx-3) ,0,0,1);

        const greeting_line_meta *g = &_greetingLineMetas[idx];
        glPushMatrix();
        {
            glTranslatef(g->x, g->y, 0);
            glScalef(g->scale,g->scale,1);

            // Draw text line `count` times with an offset.
            glColor4ub(color->x, color->y, color->z, color->w);
            for (int j=1; j<=count; j++) {
                glPushMatrix();
                glTranslatef(j*xoff,j*yoff,0);
                fw_font_renderSingleTextLine(_greetingsTexts[idx], getFontFace(), 0,0);
                glPopMatrix();
            }

        }
        glPopMatrix();
    }
    fw_font_renderTextEnd();
}

static void renderStencilSubtract(float y) {
    fw_projection_ortho();
    glPushMatrix();
    fw_quad_put(0,0,FW_RENDER_WIDTH,y);
    glPopMatrix();

    fw_image_renderBegin(&getImages()[RES_IMG_BAYER_HORIZONTAL], 1);
    {
        glColor3ub(0,0,0);
        glTranslatef(0,y,0);
        fw_quad_put(0,0,FW_RENDER_WIDTH,0);
        fw_image_putImageExt(&getImages()[RES_IMG_BAYER_HORIZONTAL], 44,0, 1,0);
    }
    fw_image_renderEnd();
}

static void renderGreetingsStenciledBegin(int idx, float stencilSubtractPosY) {
    fw_stencil_begin();
    fw_stencil_activateStencilBuffer();

    fw_vec4i color = {255,255,255,255};
    renderGreetingsLine(idx, &color, 1,0,0);

    glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);
    renderStencilSubtract(stencilSubtractPosY);

    fw_stencil_activateColorBuffer(0);
}

static void renderGreetingsStenciledFill(fw_vec4i *color, fw_timer_data *time) {
    fw_image_renderBegin(&getImages()[RES_IMG_GRADIENT_GREETINGS], 1);
    {
        glColor4ub(color->x, color->y, color->z, color->w);
        glScalef(44,2.2,1);
        fw_image_putImageExt(&getImages()[RES_IMG_GRADIENT_GREETINGS], 1,0,8,-time->elapsed*1);
    }
    fw_image_renderEnd();
}

static void renderGreetingsStenciledEnd() {
    fw_stencil_end();
}

static void renderGreetingLinePseudo3D(int idx, fw_vec4i *shadowColor, fw_vec4i *stencilColorBase, fw_vec4i *stencilColorBlend, float stencilSubtractPosY, int count, float xoff, float yoff, fw_timer_data *time) {
    // Shadow.
    renderGreetingsLine(idx,shadowColor, count, xoff, yoff);

    // FG.
    renderGreetingsStenciledBegin(idx, stencilSubtractPosY);
    renderGreetingsStenciledFill(stencilColorBase, time);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    renderGreetingsStenciledFill(stencilColorBlend, time);
    fw_render_defaultBlendMode();
    renderGreetingsStenciledEnd();
}

void fx_greetings_pseudo_3d_renderText(float stencilSubtract1PosY, float stencilSubtract2PosY, fw_timer_data *time) {
    fw_vec4i *shadowCol;
    fw_vec4i *baseCol;
    fw_vec4i *blendCol;
    float stencilSubtractPosY;

    for (int i=0; i<_numGreetingsTexts; i++) {
        if (i%2==0) {
            shadowCol = &_shadowColorSilver;
            baseCol = &_baseColorSilver;
            blendCol = &_blendColorSilver;
            stencilSubtractPosY = stencilSubtract1PosY;
        } else {
            shadowCol = &_shadowColorGold;
            baseCol = &_baseColorGold;
            blendCol = &_blendColorGold;
            stencilSubtractPosY = stencilSubtract2PosY;
        }
        renderGreetingLinePseudo3D(i, shadowCol, baseCol, blendCol, stencilSubtractPosY, 8, -.5f,.75f, time);
    }
}

void fx_greetings_pseudo_3d_update(float pEven, float pOdd) {
    const float f = 21.f;

    for (int i=0; i<_numGreetingsTexts; i++) {
        greeting_line_meta *g = &_greetingLineMetas[i];

        if (i%2 == 0) {
            g->x = g->startX + g->direction * pEven*f * g->speed;
        } else {
            g->x = g->startX + g->direction * pOdd*f * g->speed;
        }

    }
}

