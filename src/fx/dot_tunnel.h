#ifndef FX_DOT_TUNNEL_H
#define FX_DOT_TUNNEL_H

#include "../fw/types.h"

typedef struct {
    int isCullable;
    fw_vec4i color;
} fx_dot_tunnel_dot_color;

typedef struct {
    int numRings;
    int numDotsPerRing;
    float ringDistance;
    float maxDistance;
    float radiusX;
    float radiusY;

    fw_vec3f *ringCenters;
    fw_vec3f *ringDots;
    fx_dot_tunnel_dot_color *dotColors;

    fw_vec3f eye;
    fw_vec3f target;
    int eyeIdx;
    int targetIdx;
} fx_dot_tunnel;

void fx_dot_tunnel_init(fx_dot_tunnel *dotTunnel, int numRings, int numDotsPerRing, float ringDistance, float radiusX, float radiusY);
fx_dot_tunnel_dot_color *fx_dot_tunnel_createColorsDefault(int numDotsPerRing);
void *fx_dot_tunnel_initRainbowColors(fx_dot_tunnel_dot_color *dotColors, int numDotsPerRing);
void fx_dot_tunnel_update(fx_dot_tunnel *dotTunnel, float tunnelSpeed, float eyeLerpSpeed, fw_vec4f *amp, fw_timer_data *time);
void fx_dot_tunnel_render(fx_dot_tunnel *dotTunnel, fw_timer_data *time);

#endif
