#ifndef FW_DRAW_H
#define FW_DRAW_H

#include "types.h"

void fw_draw_pixel(int x, int y, fw_vec4i *color, unsigned char *pixels, int width, int height, int bpp);
void fw_draw_fill_box(int x1, int y1, int x2, int y2, fw_vec4i *color, unsigned char *pixels, int width, int height, int bpp);
void fw_draw_fill(int x, int y, fw_vec4i *color, unsigned char *pixels, int width, int height, int bpp);
void fw_draw_circle(int x, int y, int radius, fw_vec4i *color, unsigned char *pixels, int width, int height, int bpp);
void fw_draw_line(int x0, int y0, int x1, int y1, fw_vec4i *color, unsigned char *pixels, int width, int height, int bpp);
void fw_draw_gradient_oval(unsigned char *pixels, int width, int height, int bpp);

#endif
