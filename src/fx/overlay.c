#include "overlay.h"
#include "../resource.h"
#include "../config/config.h"
#include "../fw/quad.h"


void fx_overlay_renderBayerBorderLeftRight(float xSpread, float yOff, unsigned char r, unsigned char g, unsigned char b) {
    // Bayer texture is 27x8.
    fw_image_renderBegin(&getImages()[RES_IMG_BAYER_VERTICAL], 1);
    {
        glColor3ub(r,g,b);
        glPushMatrix();
        glTranslatef(-xSpread,yOff,0);
        fw_image_putImageExt(&getImages()[RES_IMG_BAYER_VERTICAL], 1,0, 35,0);
        glPopMatrix();

        glTranslatef(FW_RENDER_WIDTH+xSpread,yOff,0);
        glScalef(-1,1,1);
        fw_image_putImageExt(&getImages()[RES_IMG_BAYER_VERTICAL], 1,0, 35,0);
    }
    fw_image_renderEnd();
}

void fx_overlay_renderBayerBorderTopBottom(float ySpread, float xOff, unsigned char r, unsigned char g, unsigned char b) {
    // Bayer texture is 8x27.
    fw_image_renderBegin(&getImages()[RES_IMG_BAYER_HORIZONTAL], 1);
    {
        glColor3ub(r,g,b);
        glPushMatrix();
        glTranslatef(xOff,-ySpread,0);
        fw_image_putImageExt(&getImages()[RES_IMG_BAYER_HORIZONTAL], 44,0, 1,0);
        glPopMatrix();

        glTranslatef(xOff,FW_RENDER_HEIGHT+ySpread,0);
        glScalef(1,-1,1);
        fw_image_putImageExt(&getImages()[RES_IMG_BAYER_HORIZONTAL], 44,0, 11,0);
    }
    fw_image_renderEnd();
}


void fx_overlay_renderOverlayExt(enum ImagesEnum imageEnum,
                              unsigned char r, unsigned char g, unsigned char b, unsigned char a,
                              float translateX, float translateY, float scaleX, float scaleY,
                              float repeatX, float offX, float repeatY, float offY) {
    fw_image_renderBegin(&getImages()[imageEnum], 1);
    glColor4ub(r, g, b, a);
    glTranslatef(translateX, translateY, 0);
    glScalef(scaleX,scaleY,1);
    fw_image_putImageExt(&getImages()[imageEnum], repeatX, offX, repeatY, offY);
    fw_image_renderEnd();
}

void fx_overlay_renderOverlay(enum ImagesEnum imageEnum,
                              unsigned char r, unsigned char g, unsigned char b, unsigned char a,
                              float translateX, float translateY, float scaleX, float scaleY) {
    fx_overlay_renderOverlayExt(imageEnum, r,g,b,a, translateX,translateY,scaleX, scaleY, 1,0, 1,0);
}


