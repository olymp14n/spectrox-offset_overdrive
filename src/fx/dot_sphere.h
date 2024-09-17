#ifndef FX_DOT_SPHERE_H
#define FX_DOT_SPHERE_H

#include "../fw/types.h"

typedef struct {
    fw_vec3f center;
    float radius;
    fw_vec3f velocity;

    float intensity;
    fw_vec4f rotation;

} fx_dot_sphere;

void fx_dot_sphere_init();
void fx_dot_sphere_initSphere(fx_dot_sphere *sphere, float radius);
void fx_dot_sphere_renderShadow(fx_dot_sphere *sphere, float scaleX, float scaleY, float groundY);
void fx_dot_sphere_render(fx_dot_sphere *sphere, float scaleX, float scaleY, float pointSize);

#endif
