#include "lut.h"
#include <math.h>

#define SIN_LUT_SIZE 1024

static float _sinLUT[SIN_LUT_SIZE];
static float _cosLUT[SIN_LUT_SIZE];
static float _sinScale;
static int _sinMask;

void fw_lut_init() {
    _sinScale = SIN_LUT_SIZE / (float)(2.f*M_PI);
    _sinMask = SIN_LUT_SIZE - 1;
    for (int i=0; i<SIN_LUT_SIZE; i++) {
        _sinLUT[i] = sin(i * 2.f * M_PI / (float)SIN_LUT_SIZE);
        _cosLUT[i] = cos(i * 2.f * M_PI / (float)SIN_LUT_SIZE);
    }
}

float fw_lut_sin(float x) {
    return _sinLUT[(int)(x * _sinScale) & _sinMask];
}

float fw_lut_cos(float x) {
    return _cosLUT[(int)(x * _sinScale) & _sinMask];
}
