#include "draw.h"
#include <math.h>


void fw_draw_pixel(int x, int y, fw_vec4i *color, unsigned char *pixels, int width, int height, int bpp) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return;
    }

    int idx = (x + y * width) * bpp;
    pixels[idx] = color->x;
    pixels[idx+1] = color->y;
    pixels[idx+2] = color->z;
    pixels[idx+3] = color->w;
}

void fw_draw_fill_box(int x1, int y1, int x2, int y2, fw_vec4i *color, unsigned char *pixels, int width, int height, int bpp) {
    for (int j=y1; j<=y2; j++) {
        for (int i=x1; i<=x2; i++) {
            fw_draw_pixel(i,j, color, pixels, width, height, bpp);
        }
    }
}


static void fw_draw_flood(int x, int y, fw_vec4i *newColor, fw_vec4i *oldColor, unsigned char *pixels, int width, int height, int bpp) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return;
    }

    int idx = (x + y * width) * bpp;

    if (pixels[idx] != oldColor->x
            || pixels[idx+1] != oldColor->y
            || pixels[idx+2] != oldColor->z
            || pixels[idx+3] != oldColor->w) {
        return;
    }

    fw_draw_pixel(x,y,newColor, pixels,width,height, bpp);
    fw_draw_flood(x+1,y,newColor, oldColor, pixels,width,height, bpp);
    fw_draw_flood(x-1,y,newColor, oldColor, pixels,width,height, bpp);
    fw_draw_flood(x,y+1,newColor, oldColor, pixels,width,height, bpp);
    fw_draw_flood(x,y-1,newColor, oldColor, pixels,width,height, bpp);
}

void fw_draw_fill(int x, int y, fw_vec4i *color, unsigned char *pixels, int width, int height, int bpp) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return;
    }

    int idx = (x + y * width) * bpp;
    fw_vec4i oldColor = {pixels[idx+0], pixels[idx+1], pixels[idx+2], pixels[idx+3]};

    // Don't allow filling a start pixel with the same color to avoid endless recursion.
    if (color->x == oldColor.x
            && color->y == oldColor.y
            && color->z == oldColor.z
            && color->w == oldColor.w
       ) {
        return;
    }

    fw_draw_flood(x,y,color, &oldColor, pixels,width,height, bpp);
}

/**
 * FROM http://fredericgoset.ovh/mathematiques/courbes/en/bresenham_circle.html
 */
void fw_draw_circle(int cx, int cy, int radius, fw_vec4i *color, unsigned char *pixels, int width, int height, int bpp) {

    int x = 0;
    int y = radius;
    int m = 5 - 4 * radius;

    while (x <= y) {
        fw_draw_pixel(cx + x, cy + y, color, pixels, width, height, bpp);
        fw_draw_pixel(cx + x, cy - y, color, pixels, width, height, bpp);
        fw_draw_pixel(cx - x, cy + y, color, pixels, width, height, bpp);
        fw_draw_pixel(cx - x, cy - y, color, pixels, width, height, bpp);
        fw_draw_pixel(cx + y, cy + x, color, pixels, width, height, bpp);
        fw_draw_pixel(cx + y, cy - x, color, pixels, width, height, bpp);
        fw_draw_pixel(cx - y, cy + x, color, pixels, width, height, bpp);
        fw_draw_pixel(cx - y, cy - x, color, pixels, width, height, bpp);

        if (m > 0) {
            y--;
            m -= 8 * y;
        }

        x++;
        m += 8 * x + 4;
    }
}

/**
 * FROM https://gist.github.com/bert/1085538
 */
void fw_draw_line(int x0, int y0, int x1, int y1, fw_vec4i *color, unsigned char *pixels, int width, int height, int bpp) {
    int dx =  abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    for (;;) {
        fw_draw_pixel(x0, y0, color, pixels, width, height, bpp);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}


/**
 * Fill buffer with an oval that is filled with a white gradient that is fully opaque in center and transparent at edge.
 */
void fw_draw_gradient_oval(unsigned char *pixels, int width, int height, int bpp) {
    for (int y=0; y<height; y++) {
        for (int x=0; x<width; x++) {
            float centerX = width/2.f;
            float centerY = height/2.f;
            float radiusX = width/2.f;
            float radiusY = height/2.f;

            float distanceX = (float)(x - centerX) / radiusX;
            float distanceY = (float)(y - centerY) / radiusY;
            float distance = sqrt(distanceX * distanceX + distanceY * distanceY);

            if (distance <= 1.0) {
                // Normalize distance to [0, 1], with 0 at the center and 1 at the edge
                float intensity = 1 - distance;
                if (intensity < 0) {
                    intensity = 0;
                }

                //fw_vec4i c = {intensity * 255, intensity * 255, intensity * 255, 255};
                fw_vec4i c = {255,255,255,intensity * 255};
                fw_draw_pixel(x,y, &c,pixels,width,height,bpp);
            }
        }
    }

}
