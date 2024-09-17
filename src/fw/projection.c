#include "projection.h"
#include <GL/glu.h>
#include "../config/config.h"

const float _aspect = FW_RENDER_WIDTH/(float)FW_RENDER_HEIGHT;

void fw_projection_perspective() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, _aspect, 0.01f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void fw_projection_ortho() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, FW_RENDER_WIDTH, FW_RENDER_HEIGHT, 0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

float fw_projection_getAspect() {
    return _aspect;
}
