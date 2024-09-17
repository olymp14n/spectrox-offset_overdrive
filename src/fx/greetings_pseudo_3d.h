#ifndef FX_GREETINGS_PSEUDO_3D_H
#define FX_GREETINGS_PSEUDO_3D_H

#include "../fw/types.h"

void fx_greetings_pseudo_3d_renderText(float stencilSubtract1PosY, float stencilSubtract2PosY, fw_timer_data *time);
void fx_greetings_pseudo_3d_update(float pEven, float pOdd);

#endif
