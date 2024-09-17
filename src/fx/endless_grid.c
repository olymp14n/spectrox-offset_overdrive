#include "endless_grid.h"
#include <math.h>
#include "../fw/quad.h"

#define NUM_CHECKER_PLANES 50
#define CHECKER_PLANES_DIST .03f
const float _checkerPlanesMaxDist = NUM_CHECKER_PLANES * CHECKER_PLANES_DIST;

typedef struct {
    fw_vec3f pos;
    fw_vec4i color;
} checker_plane;

static checker_plane _checkerPlanes[NUM_CHECKER_PLANES];

void fx_endless_grid_init() {

    fw_vec4i checkerColors[] = {
        {250, 253, 255, 1},
        {255, 209, 0, 1},
        {255, 132, 38, 1},
        {214, 36, 17, 1},
        {127, 6, 36, 1},
        {250, 253, 255, 1},
        {255, 128, 164, 1},
        {255, 38, 116, 1},
        {148, 33, 106, 1},
        {67, 0, 103, 1},
    };
    const int checkerColorsLen = LEN(checkerColors);

    for (int i=0; i<NUM_CHECKER_PLANES; i++) {
        _checkerPlanes[i].pos.z = -CHECKER_PLANES_DIST*i;
        _checkerPlanes[i].color = checkerColors[i%checkerColorsLen];
    }
}

void fx_endless_grid_update(fw_timer_data *time) {
    for (int i=0; i<NUM_CHECKER_PLANES; i++) {

        _checkerPlanes[i].pos.z += 0.35f*time->delta;
        if (_checkerPlanes[i].pos.z > 0) {
            _checkerPlanes[i].pos.z -= _checkerPlanesMaxDist;
        }
        if (_checkerPlanes[i].pos.z < -_checkerPlanesMaxDist) {
            _checkerPlanes[i].pos.z += _checkerPlanesMaxDist;
        }
    }
}

void fx_endless_grid_render(GLuint textureID, char isMonochrome, unsigned char alpha, fw_timer_data *time) {
    float fogColor[] = {0,0,0,1};
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, _checkerPlanesMaxDist*0.5);
    glFogf(GL_FOG_END, _checkerPlanesMaxDist*0.8);

    // Don' write transparent fragments to depth buffer.
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_NOTEQUAL, 0);
    glEnable(GL_FOG);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glPushMatrix();
    {
        for (int i=0; i<NUM_CHECKER_PLANES; i++) {
            glPushMatrix();
            {
                if (isMonochrome) {
                    const unsigned char gray = 0.299f*_checkerPlanes[i].color.x + 0.587f*_checkerPlanes[i].color.y + 0.114f*_checkerPlanes[i].color.z;
                    glColor4ub(gray,gray,gray, alpha);
                } else {
                    glColor4ub(_checkerPlanes[i].color.x, _checkerPlanes[i].color.y, _checkerPlanes[i].color.z, alpha);
                }
                glTranslatef(_checkerPlanes[i].pos.x, _checkerPlanes[i].pos.y, _checkerPlanes[i].pos.z);
                glRotatef(180*sinf(time->elapsed*.25) + 30*cosf(time->elapsed*.5), 0,0,1);
                glScalef(.5,.5,1);
                fw_quad_putTextured(32,0.25  + 1.2*sinf(time->elapsed*M_PI*.5f*.5), 32,-0.25);
            }
            glPopMatrix();
        }
    }
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_FOG);
    glDisable(GL_ALPHA_TEST);
}
