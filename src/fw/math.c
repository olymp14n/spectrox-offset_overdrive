#include "math.h"

#include <math.h>
#include <stdlib.h>

float fw_math_randf(float min, float max) {
    // From: https://stackoverflow.com/a/5289624/891846
    float rnd = ((float) rand()) / (float) RAND_MAX;
    return min + rnd * (max - min);
}

int fw_math_rand(int min, int max) {
    // From: https://c-faq.com/lib/randrange.html
    // and   https://stackoverflow.com/a/18386648/891846
    return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

void fw_math_normalize(fw_vec3f *v) {
    float len = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
    v->x /= len;
    v->y /= len;
    v->z /= len;
}

float fw_math_dot(fw_vec3f *v1, fw_vec3f *v2) {
    return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}

void fw_math_scale(fw_vec3f *v, float s) {
    v->x *= s;
    v->y *= s;
    v->z *= s;
}

float fw_math_len(fw_vec3f *v) {
    return sqrt(fw_math_lenSquared(v));
}

float fw_math_lenSquared(fw_vec3f *v) {
    return v->x*v->x + v->y*v->y + v->z*v->z;
}

float fw_math_dist(fw_vec3f *v1, fw_vec3f *v2) {
    return sqrt(
               (v1->x - v2->x)*(v1->x - v2->x)
               + (v1->y - v2->y)*(v1->y - v2->y)
               + (v1->z - v2->z)*(v1->z - v2->z)
           );
}

float fw_math_sign(float f) {
    return copysignf(1.f,f);
}

float fw_math_clamp(float d, float min, float max) {
    // https://stackoverflow.com/a/16659263/891846
    const float t = d < min ? min : d;
    return t > max ? max : t;
}

float fw_math_lerp(float src, float dest, float t) {
    if (t >= 1) {
        return dest;
    }
    return src + t * (dest - src);
}

/**
 * lambda - Recommended range is 1..25, from slow to fast approximation.
 */
float fw_math_damp(float src, float dest, float lambda, float dt) {
    // https://www.rorydriscoll.com/2016/03/07/frame-rate-independent-damping-using-lerp/
    return fw_math_lerp(src, dest, 1.f-exp(-lambda*dt));
}
