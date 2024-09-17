#include "render.h"
#include <GL/glu.h>
#include "glx.h"
#include "../config/config.h"
#include "projection.h"

static GLuint _fw_renderTexture = 0;
static RECT _fw_outputRect;
static RECT _fw_textureRect;

static void fw_render_initTexture() {
    glGenTextures(1, &_fw_renderTexture);
    glBindTexture(GL_TEXTURE_2D, _fw_renderTexture);

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, FW_RENDER_WIDTH, FW_RENDER_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}

static void fw_render_calcCoords() {
    if ( (FW_RENDER_WIDTH / (float)FW_RENDER_HEIGHT) < (_fw_outputRect.right / (float)_fw_outputRect.bottom) ) {
        // black bars left and right
        int w = (FW_RENDER_WIDTH*_fw_outputRect.bottom)/FW_RENDER_HEIGHT;
        int x = (_fw_outputRect.right - w)/2;

        _fw_textureRect.left = x;
        _fw_textureRect.top = 0;
        _fw_textureRect.right = x + w;
        _fw_textureRect.bottom = _fw_outputRect.bottom;

    } else {
        // black bars top and bottom
        int h = (FW_RENDER_HEIGHT*_fw_outputRect.right)/FW_RENDER_WIDTH;
        int y = (_fw_outputRect.bottom - h)/2;

        _fw_textureRect.left = 0;
        _fw_textureRect.top = y;
        _fw_textureRect.right = _fw_outputRect.right;
        _fw_textureRect.bottom = y+h;
    }
}

void fw_render_init(RECT outputRect) {
    _fw_outputRect = outputRect;
    fw_render_calcCoords();
    fw_render_initTexture();
}

void fw_render_bindTextureBufferBegin() {
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, FW_RENDER_WIDTH, FW_RENDER_HEIGHT);
}

void fw_render_bindTextureBufferEnd() {
    glBindTexture(GL_TEXTURE_2D, _fw_renderTexture);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, FW_RENDER_WIDTH, FW_RENDER_HEIGHT);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1,1,1);
}

void fw_render_defaultBlendMode() {
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void fw_render_renderTextureBuffer() {
    glEnable(GL_BLEND);
    fw_render_defaultBlendMode();

    glBindTexture(GL_TEXTURE_2D, _fw_renderTexture);
    glViewport(0, 0, _fw_outputRect.right, _fw_outputRect.bottom);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, _fw_outputRect.right, 0, _fw_outputRect.bottom);

    // https://stackoverflow.com/a/643787/891846
    glColor3f(1, 1, 1);

    glEnable(GL_TEXTURE_2D);
    glPushMatrix();

    glBegin(GL_QUADS);

    glTexCoord2d(0, 0);
    glVertex2i(_fw_textureRect.left, _fw_textureRect.top); // top-left

    glTexCoord2d(0, 1);
    glVertex2i(_fw_textureRect.left, _fw_textureRect.bottom); // bottom-left

    glTexCoord2d(1, 1);
    glVertex2i(_fw_textureRect.right, _fw_textureRect.bottom); // bottom-right

    glTexCoord2d(1, 0);
    glVertex2i(_fw_textureRect.right, _fw_textureRect.top); // top-right

    glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void fw_render_viewportLowRes(int xOffset, int yOffset) {
    glViewport(xOffset, yOffset, FW_RENDER_WIDTH, FW_RENDER_HEIGHT);
}

void fw_render_viewportHiRes(int xOffset, int yOffset) {
    glViewport(xOffset + _fw_textureRect.left, yOffset + _fw_textureRect.top, _fw_textureRect.right-_fw_textureRect.left, _fw_textureRect.bottom-_fw_textureRect.top);
}

void fw_render_clearHiRes(float r, float g, float b, float a) {
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    fw_projection_ortho();
    glColor4f(r,g,b,a);
    glPushMatrix();
    glBegin(GL_QUADS);
    glVertex2i(0, 0);
    glVertex2i(0, FW_RENDER_HEIGHT);
    glVertex2i(FW_RENDER_WIDTH, FW_RENDER_HEIGHT);
    glVertex2i(FW_RENDER_WIDTH, 0);
    glEnd();
    glPopMatrix();
}

