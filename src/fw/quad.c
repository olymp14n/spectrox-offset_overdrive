#include "quad.h"
#include <GL/gl.h>


void fw_quad_putTextured(float xrepeat, float xoff, float yrepeat, float yoff) {
    glBegin(GL_QUADS);
    glTexCoord2d(xoff, yoff);
    glVertex2i(-1, 1);

    glTexCoord2d(xoff, yoff + yrepeat);
    glVertex2i(-1, -1);

    glTexCoord2d(xoff + xrepeat, yoff + yrepeat);
    glVertex2i(1, -1);

    glTexCoord2d(xoff + xrepeat, yoff);
    glVertex2i(1, 1);
    glEnd();
}


void fw_quad_put(float x, float y, float w, float h) {
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x, y+h);
    glVertex2f(x+w, y+h);
    glVertex2f(x+w, y);
    glEnd();
}

void fw_quad_put2(float x1, float y1, float x2, float y2) {
    glBegin(GL_QUADS);
    glVertex2f(x1, y1);
    glVertex2f(x1, y2);
    glVertex2f(x2, y2);
    glVertex2f(x2, y1);
    glEnd();
}
