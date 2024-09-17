#ifndef FW_GLX_H
#define FW_GLX_H

#include <GL/gl.h>

// https://stackoverflow.com/a/2081438/891846
#define APIENTRYP APIENTRY *

void fw_glx_init();

// Copied from <GL/glext.h>
typedef void (APIENTRYP PFNGLBLENDFUNCSEPARATEPROC) (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
PFNGLBLENDFUNCSEPARATEPROC glBlendFuncSeparate;

#endif
