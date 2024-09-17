#include "cylinder_twist.h"
#include <GL/glu.h>
#include <math.h>
#include "../config/config.h"

#define SCISSOR_HEIGHT 2
#define CYLINDER_HEIGHT 6.4

static GLUquadric *_quadric;

void fx_cylinder_twist_init(void) {
    _quadric = gluNewQuadric();
    gluQuadricDrawStyle(_quadric, GLU_FLAT);
    gluQuadricNormals(_quadric, GLU_FLAT);
}

void fx_cylinder_twist_initParams(fx_cylinder_params *params) {
    params->cylinders[0].color = (fw_vec4i){225, 50, 66, 255};
    params->cylinders[1].color = (fw_vec4i){55, 183, 55, 255};
    params->cylinders[2].color = (fw_vec4i){253, 197, 48, 255};

    params->cylinders[0].shadowColor = (fw_vec4i){0, 0, 0, 127};
    params->cylinders[1].shadowColor = (fw_vec4i){0, 0, 0, 127};
    params->cylinders[2].shadowColor = (fw_vec4i){0, 0, 0, 127};

    params->cylinders[0].slices = 4;
    params->cylinders[1].slices = 4;
    params->cylinders[2].slices = 4;

    params->cylinders[0].baseRadius = .3f;
    params->cylinders[1].baseRadius = .3f;
    params->cylinders[2].baseRadius = .3f;

    params->cylinders[0].topRadius = .3f;
    params->cylinders[1].topRadius = .3f;
    params->cylinders[2].topRadius = .3f;

    params->twistVariance = .005f;
}

static void fx_cylinder_twist_putCylinder(float x, float y, float z, float baseRadius, float topRadius, float height, int slices, fw_vec4i *color) {
    glColor4ub(color->x, color->y, color->z, color->w);
    glPushMatrix();
    {
        glTranslatef(x,y-.4f,z); // Hack: move everything down by -0.4.
        glRotatef(-90,1,0,0);
        gluCylinder(_quadric, baseRadius,topRadius,height,slices,1);
    }
    glPopMatrix();
}

static void fx_cylinder_twist_putCylinders(float angle, fx_cylinder_params *params, int isShadow) {
    glPushMatrix();
    {
        glTranslatef(0,0,-7);
        glRotatef(-10,1,0,0); // Viewing angle.
        glRotatef(angle, 0,1,0);

        for (int i=0; i<NUM_CYLINDERS; i++) {
            fx_cylinder_twist_putCylinder(
                params->cylinders[i].position.x,
                params->cylinders[i].position.y,
                params->cylinders[i].position.z,
                params->cylinders[i].baseRadius,
                params->cylinders[i].topRadius,
                CYLINDER_HEIGHT,
                params->cylinders[i].slices,
                isShadow ? &params->cylinders[i].shadowColor : &params->cylinders[i].color);
        }
    }
    glPopMatrix();
}

void fx_cylinder_twist_render(fx_cylinder_params *params, fw_timer_data *time) {
    const float yOff = -CYLINDER_HEIGHT*.5f;

    params->cylinders[0].position.x = params->distance;
    params->cylinders[0].position.y = yOff + params->cylinders[0].yDelta;
    params->cylinders[0].position.z = 0;
    params->cylinders[1].position.x = -.5f*params->distance;
    params->cylinders[1].position.y = yOff  + params->cylinders[1].yDelta;
    params->cylinders[1].position.z = 0.866f*params->distance;
    params->cylinders[2].position.x = -.5f*params->distance;
    params->cylinders[2].position.y = yOff  + params->cylinders[2].yDelta;
    params->cylinders[2].position.z = -0.866f*params->distance;

    glEnable(GL_CULL_FACE);
    glEnable(GL_SCISSOR_TEST);

    #define CYLINDER_ROTATION_OFFSET 1.5f

    // Draw shadows.
    glPushMatrix();
    glTranslatef(.2f,-.2f,0);
    for (int i=0; i<FW_RENDER_HEIGHT; i+=SCISSOR_HEIGHT) {
        glScissor(0,i,FW_RENDER_WIDTH,SCISSOR_HEIGHT);
        fx_cylinder_twist_putCylinders(sinf(CYLINDER_ROTATION_OFFSET+time->elapsed + i*params->twistVariance) * 360, params, 1);
    }
    glPopMatrix();

    // Draw colored cylinders.
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    for (int i=0; i<FW_RENDER_HEIGHT; i+=SCISSOR_HEIGHT) {
        glScissor(0,i,FW_RENDER_WIDTH,SCISSOR_HEIGHT);
        fx_cylinder_twist_putCylinders(sinf(CYLINDER_ROTATION_OFFSET+time->elapsed + i*params->twistVariance) * 360, params, 0);
    }
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    glDisable(GL_SCISSOR_TEST);

    glDisable(GL_CULL_FACE);
}
