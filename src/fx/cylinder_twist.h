#ifndef FX_CYLINDER_TWIST_H
#define FX_CYLINDER_TWIST_H

#include "../fw/types.h"

#define NUM_CYLINDERS 3

typedef struct {
    float yDelta;
    float baseRadius;
    float topRadius;
    int slices;
    fw_vec3f position;
    fw_vec4i color;
    fw_vec4i shadowColor;
} fx_cylinder;

typedef struct {
    float distance;
    float twistVariance;
    fx_cylinder cylinders[NUM_CYLINDERS];
} fx_cylinder_params;

void fx_cylinder_twist_init(void);
void fx_cylinder_twist_initParams(fx_cylinder_params *params);
void fx_cylinder_twist_render(fx_cylinder_params *params, fw_timer_data *time);

#endif
