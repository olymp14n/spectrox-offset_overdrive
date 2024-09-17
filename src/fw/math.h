#ifndef FW_MATH_H
#define FW_MATH_H

#include "types.h"

float fw_math_randf(float min, float max);
int fw_math_rand(int min, int max);
void fw_math_normalize(fw_vec3f *v);
float fw_math_dot(fw_vec3f *v1, fw_vec3f *v2);
void fw_math_scale(fw_vec3f *v, float s);
float fw_math_len(fw_vec3f *v);
float fw_math_lenSquared(fw_vec3f *v);
float fw_math_dist(fw_vec3f *v1, fw_vec3f *v2);
float fw_math_sign(float f);
float fw_math_clamp(float d, float min, float max);
float fw_math_lerp(float src, float dest, float t);
float fw_math_damp(float src, float dest, float lambda, float dt);

#endif
