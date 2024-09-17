#include "text_3d.h"

#include <math.h>
#include "../fw/projection.h"

void fx_text_3d_render(fx_text_3d_params *params) {

    const float textWidth = fw_font_calcLineWidth(params->text, params->fontFace);

    fw_font_renderTextBegin(params->fontFace);
    fw_projection_perspective();

    glPushMatrix();
    {
        glTranslatef(params->pos.x,params->pos.y,params->pos.z);
        glRotatef(params->rot.w,params->rot.x,params->rot.y,params->rot.z);
        glScalef(params->scale,-params->scale,params->scale);
        glTranslatef(-.5f*textWidth,-.5*params->fontFace->lineHeight,0);


        // GL_DEPTH_TEST is not needed if rotation is always < 90 degrees.
        glEnable(GL_DEPTH_TEST);
        // Text shadows write to depth buffer, but ignore it when writing to color buffer.
        glDepthFunc(GL_ALWAYS);

        glColor3ub(0,0,0);
        for (int i=1; i<=10; i++) {
            glPushMatrix();
            glTranslatef(0,0,-i);
            fw_font_renderSingleTextLine(params->text, params->fontFace, 0, 0);
            glPopMatrix();
        }

        // Reset depth compare function to default for nearest text plane.
        // Thus don't overwrite text shadows.
        glDepthFunc(GL_LEQUAL);

        if (!params->isPaletteEnabled) {
            glColor4ub(params->color.x,params->color.y,params->color.z, params->color.w);
        } else {
            int palIdx = params->paletteIndex;
            glColor4ub(params->palette[palIdx].x,
                       params->palette[palIdx].y,
                       params->palette[palIdx].z,
                       params->palette[palIdx].w);
        }

        fw_font_renderSingleTextLine(params->text, params->fontFace, 0, 0);

        glDisable(GL_DEPTH_TEST);

    }
    glPopMatrix();
    fw_font_renderTextEnd();
}
