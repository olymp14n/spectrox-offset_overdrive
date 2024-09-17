#include "dot_tunnel.h"
#include <GL/glu.h>
#include <stdlib.h>
#include <math.h>
#include "../fw/math.h"

static void fx_dot_tunnel_init_ringDots(fx_dot_tunnel *dotTunnel) {
    for (int j=0; j<dotTunnel->numDotsPerRing; j++) {
        float k = j * M_PI * 2.f / dotTunnel->numDotsPerRing;
        dotTunnel->ringDots[j] = (fw_vec3f) {
            dotTunnel->radiusX * sinf(k), dotTunnel->radiusY * cosf(k), 0
        };
    }
}

fx_dot_tunnel_dot_color *fx_dot_tunnel_createColorsDefault(int numDotsPerRing) {
    fx_dot_tunnel_dot_color *dotColors = calloc(numDotsPerRing, sizeof(fx_dot_tunnel_dot_color));

    for (int i=0; i<numDotsPerRing; i++) {
        dotColors[i].isCullable = 1;
        dotColors[i].color = (fw_vec4i) {
            255,255,255,255
        };
    }

    return dotColors;
}

void *fx_dot_tunnel_initRainbowColors(fx_dot_tunnel_dot_color *dotColors, int numDotsPerRing) {
    fw_vec3i palette[] = {
        {20, 52, 100},
        {30, 72, 148},
        {40, 92, 196},
        {36, 159, 222},
        {32, 214, 199},
        {166, 252, 219},
        {255, 255, 255},
        {255, 255, 255},
        {254, 243, 192},
        {254, 243, 192},
        {250, 214, 184},
        {245, 160, 151},
        {232, 106, 115},
        {188, 74, 155},
        {121, 58, 128},
    };

    int idx = 0;
    for (int i=0; i<LEN(palette); i++) {
        dotColors[idx%numDotsPerRing].isCullable = 0;
        dotColors[idx%numDotsPerRing].color.x = palette[i].x;
        dotColors[idx%numDotsPerRing].color.y = palette[i].y;
        dotColors[idx%numDotsPerRing].color.z = palette[i].z;
        dotColors[idx%numDotsPerRing].color.w = 255;
        idx+=1;
    }

    return dotColors;
}

static void fx_dot_tunnel_init_ringCenters(fx_dot_tunnel *dotTunnel) {
    for (int i=0; i<dotTunnel->numRings; i++) {
        dotTunnel->ringCenters[i] = (fw_vec3f) {
            0, 0, -i * dotTunnel->ringDistance
        };
    }
}

void fx_dot_tunnel_init(fx_dot_tunnel *dotTunnel, int numRings, int numDotsPerRing, float ringDistance, float radiusX, float radiusY) {
    dotTunnel->numRings = numRings;
    dotTunnel->numDotsPerRing = numDotsPerRing;
    dotTunnel->ringDistance = ringDistance;
    dotTunnel->maxDistance = numRings * ringDistance;
    dotTunnel->radiusX = radiusX;
    dotTunnel->radiusY = radiusY;
    dotTunnel->ringCenters = calloc(numRings, sizeof(fw_vec3f));
    dotTunnel->ringDots = calloc(numDotsPerRing, sizeof(fw_vec3f));
    dotTunnel->dotColors = fx_dot_tunnel_createColorsDefault(numDotsPerRing);

    dotTunnel->eyeIdx = 0;
    dotTunnel->targetIdx = 1;
    dotTunnel->target.z = -10;

    fx_dot_tunnel_init_ringDots(dotTunnel);
    fx_dot_tunnel_init_ringCenters(dotTunnel);
}

void fx_dot_tunnel_update(fx_dot_tunnel *dotTunnel, float tunnelSpeed, float eyeLerpSpeed, fw_vec4f *amp, fw_timer_data *time) {
    for (int i=0; i<dotTunnel->numRings; i++) {
        dotTunnel->ringCenters[i].z += tunnelSpeed * time->delta;
        if (dotTunnel->ringCenters[i].z >= 0) {
            dotTunnel->eyeIdx = (i+1) % dotTunnel->numRings;
            dotTunnel->targetIdx = (dotTunnel->eyeIdx + 48) % dotTunnel->numRings;

            dotTunnel->ringCenters[i].x = amp->x * sinf(time->elapsed);
            dotTunnel->ringCenters[i].y = amp->y * sinf(1.5f * time->elapsed);
            dotTunnel->ringCenters[i].z -= dotTunnel->maxDistance;
        }
    }

    const float s = eyeLerpSpeed * time->delta;
    const float ts = s * .25;
    dotTunnel->eye.x = fw_math_lerp(dotTunnel->eye.x, dotTunnel->ringCenters[dotTunnel->eyeIdx].x, s);
    dotTunnel->eye.y = fw_math_lerp(dotTunnel->eye.y, dotTunnel->ringCenters[dotTunnel->eyeIdx].y, s);

    dotTunnel->target.x = fw_math_lerp(dotTunnel->target.x, dotTunnel->ringCenters[dotTunnel->targetIdx].x, ts);
    dotTunnel->target.y = fw_math_lerp(dotTunnel->target.y, dotTunnel->ringCenters[dotTunnel->targetIdx].y, ts);
    dotTunnel->target.z = fw_math_lerp(dotTunnel->target.z, dotTunnel->ringCenters[dotTunnel->targetIdx].z, ts);
}

static void fx_dot_tunnel_renderRings(fx_dot_tunnel *dotTunnel) {
    for (int i=0; i<dotTunnel->numRings; i++) {

        float alpha = 2.f - 2.f*dotTunnel->ringCenters[i].z / -dotTunnel->maxDistance;
        alpha = alpha*170.f;
        if (alpha > 255) {
            alpha = 255;
        }

        glPushMatrix();

        glTranslatef(dotTunnel->ringCenters[i].x, dotTunnel->ringCenters[i].y, dotTunnel->ringCenters[i].z);
        glRotatef(360.f*i/(float)(dotTunnel->numRings-0), 0,0,1);
        glTranslatef(-dotTunnel->ringCenters[i].x, -dotTunnel->ringCenters[i].y, -dotTunnel->ringCenters[i].z);

        // Cullable
        if (dotTunnel->ringCenters[i].z < -2.5) {
            glPointSize(1);
        } else if  (dotTunnel->ringCenters[i].z < -1.5) {
            glPointSize(1);
        } else {
            glPointSize(2);
        }

        glBegin(GL_POINTS);
        for (int j=0; j<dotTunnel->numDotsPerRing; j++) {
            if (!dotTunnel->dotColors[j].isCullable) {
                continue;
            }
            if (dotTunnel->dotColors[j].color.w == 0) {
                continue;
            }
            if (i%6 != 0) {
                continue;
            }

            glColor4ub(dotTunnel->dotColors[j].color.x, dotTunnel->dotColors[j].color.y, dotTunnel->dotColors[j].color.z, alpha);
            glVertex3f(dotTunnel->ringDots[j].x + dotTunnel->ringCenters[i].x, dotTunnel->ringDots[j].y + dotTunnel->ringCenters[i].y, dotTunnel->ringCenters[i].z);
        }
        glEnd();

        // Non-cullable.
        if (dotTunnel->ringCenters[i].z < -5.5) {
            glPointSize(5);
        } else if  (dotTunnel->ringCenters[i].z < -1.9) {
            glPointSize(6);
        } else {
            glPointSize(5);
        }
        glBegin(GL_POINTS);
        for (int j=0; j<dotTunnel->numDotsPerRing; j++) {
            if (dotTunnel->dotColors[j].isCullable) {
                continue;
            }
            if (dotTunnel->dotColors[j].color.w == 0) {
                continue;
            }
            glColor4ub(dotTunnel->dotColors[j].color.x, dotTunnel->dotColors[j].color.y, dotTunnel->dotColors[j].color.z, 255);
            glVertex3f(dotTunnel->ringDots[j].x + dotTunnel->ringCenters[i].x, dotTunnel->ringDots[j].y + dotTunnel->ringCenters[i].y, dotTunnel->ringCenters[i].z);
        }
        glEnd();

        glPopMatrix();
    }
}

void fx_dot_tunnel_render(fx_dot_tunnel *dotTunnel, fw_timer_data *time) {
    glPushMatrix();
    glRotatef(180*sinf(.2f*time->elapsed), 0,0,1);

    gluLookAt(dotTunnel->eye.x, dotTunnel->eye.y, dotTunnel->eye.z, dotTunnel->target.x,dotTunnel->target.y,dotTunnel->target.z, 0,1,0);

    glEnable(GL_DEPTH_TEST);
    fx_dot_tunnel_renderRings(dotTunnel);
    glDisable(GL_DEPTH_TEST);

    glPopMatrix();
}
