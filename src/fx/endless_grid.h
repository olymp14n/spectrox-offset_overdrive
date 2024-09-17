#ifndef FX_ENDLESS_GRID_H
#define FX_ENDLESS_GRID_H

#include <GL/gl.h>
#include "../fw/types.h"

void fx_endless_grid_init();
void fx_endless_grid_update(fw_timer_data *time);
void fx_endless_grid_render(GLuint textureID, char isMonochrome, unsigned char alpha, fw_timer_data *time);

#endif

