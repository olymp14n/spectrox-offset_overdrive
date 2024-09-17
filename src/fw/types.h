#ifndef FW_TYPES_H
#define FW_TYPES_H

#define LEN(x)  (sizeof(x) / sizeof(x[0]))

typedef struct {
    long start;
    long current;
    long previousFrame;
    float elapsed;
    float delta;
} fw_timer_data;

typedef struct {
    float x;
    float y;
} fw_vec2f;

typedef struct {
    int x;
    int y;
} fw_vec2i;

typedef struct {
    float x;
    float y;
    float z;
} fw_vec3f;

typedef struct {
    int x;
    int y;
    int z;
} fw_vec3i;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} fw_vec4f;

typedef struct {
    int x;
    int y;
    int z;
    int w;
} fw_vec4i;

#endif
