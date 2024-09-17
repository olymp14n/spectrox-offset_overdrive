#include "stencil.h"
#include <GL/gl.h>

void fw_stencil_begin() {
    // FROM https://stackoverflow.com/a/57945246/891846
    glClear(GL_STENCIL_BUFFER_BIT);
    glEnable(GL_STENCIL_TEST);
}

void fw_stencil_end() {
    glDisable(GL_STENCIL_TEST);
}

void fw_stencil_activateStencilBuffer() {
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);

    // Only write fragments to stencil buffer if alpha is 1.
    // Needed when masking with transparent textures.
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GEQUAL, 1);

    // Everything drawn in stencil buffer needs to have full opacity of any color.
    glColor4f(0,0,0,1);
}

void fw_stencil_activateColorBuffer(int isInverted) {
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilFunc(GL_EQUAL, !isInverted, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glDisable(GL_ALPHA_TEST);
}
