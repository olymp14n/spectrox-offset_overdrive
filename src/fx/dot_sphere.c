#include "dot_sphere.h"
#include <GL/glu.h>
#include <stdlib.h>
#include <math.h>
#include "../fw/math.h"


static GLUquadric *_quadricPoints;
static GLUquadric *_quadricFilled;


void fx_dot_sphere_init() {
    _quadricPoints = gluNewQuadric();
    gluQuadricDrawStyle(_quadricPoints,   GLU_POINT);

    _quadricFilled = gluNewQuadric();
}

void fx_dot_sphere_initSphere(fx_dot_sphere *sphere, float radius) {
    sphere->radius = radius;
    sphere->intensity = .3f;
}

void fx_dot_sphere_renderShadow(fx_dot_sphere *sphere, float scaleX, float scaleY, float groundY) {

    glPushMatrix();
    glEnable(GL_CULL_FACE);
    glColor4f(0,0,0,.2f);

    // Shadow scale depending on distance.
    float f = 0;

    float dist = sphere->center.y - sphere->radius - groundY;

    f = 1.f - dist/3.f;
    f = f<0?0:f;

    glTranslatef(sphere->center.x, groundY, sphere->center.z);
    glScalef(f * scaleX, .1f * f * scaleY, f);

    gluSphere(_quadricFilled, sphere->radius, 12, 12);
    glDisable(GL_CULL_FACE);
    glPopMatrix();
}

void fx_dot_sphere_render(fx_dot_sphere *sphere, float scaleX, float scaleY, float pointSize) {
    glPointSize(pointSize);

    glEnable(GL_DEPTH_TEST);
    glPushMatrix();

    glTranslatef(sphere->center.x, sphere->center.y, sphere->center.z);
    glScalef(scaleX, scaleY, 1);
    glRotatef(sphere->rotation.w, sphere->rotation.x, sphere->rotation.y, sphere->rotation.z);
    glEnable(GL_CULL_FACE);
    glColor4f(.9,0,.6,sphere->intensity);

    gluSphere(_quadricFilled, sphere->radius-.01f, 12, 12);
    glColor4f(1,1,1,1);
    gluSphere(_quadricPoints, sphere->radius, 24, 24);
    glDisable(GL_CULL_FACE);
    glPopMatrix();
    glDisable(GL_DEPTH_TEST);
}


