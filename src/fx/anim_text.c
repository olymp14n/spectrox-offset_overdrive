#include "anim_text.h"
#include "../fw/projection.h"

static void fx_anim_text_renderGlyph(fw_font_glyph *glyph, int x, int y, fx_anim_text_modifier *modifier, fw_vec3f *color) {
    int left = x + glyph->xoffset;
    int top = y + glyph->yoffset;
    int right = left + glyph->width;
    int bottom = top + glyph->height;

    if (color) {
        glColor4f(color->x, color->y, color->z, modifier->color.w);
    } else {
        glColor4f(modifier->color.x, modifier->color.y, modifier->color.z, modifier->color.w);
    }

    glPushMatrix();
    {
        glTranslatef(modifier->translate.x, modifier->translate.y, 0);

        glTranslatef(left + glyph->width/2, top + glyph->height/2, 0);
        glScalef(modifier->scale.x, modifier->scale.y, 1);
        glRotatef(modifier->rotate.x,0,0,1);
        glTranslatef(-left - glyph->width/2, -top - glyph->height/2, 0);


        glBegin(GL_QUADS);
        {
            glTexCoord2d(glyph->left, glyph->top);
            glVertex2i(left, top);

            glTexCoord2d(glyph->left, glyph->bottom);
            glVertex2i(left, bottom);

            glTexCoord2d(glyph->right, glyph->bottom);
            glVertex2i(right, bottom);

            glTexCoord2d(glyph->right, glyph->top);
            glVertex2i(right, top);
        }
        glEnd();
    }
    glPopMatrix();
}

static void fx_anim_text_renderLine(const char string[], fw_font_face *fontFace, int y, fx_anim_text_modifier *modifier, fw_vec3f *color) {
    int xadvanceTotal = 0;
    fw_font_glyph *glyph;

    for (int i=0; i<strlen(string); i++) {
        glyph = &fontFace->glyphs[(unsigned char)string[i]];

        if (string[i] != ' ') {
            fx_anim_text_renderGlyph(glyph, xadvanceTotal, y, &modifier[i], color);
        }
        xadvanceTotal += glyph->xadvance + fontFace->outline;
    }
}

static void fx_anim_text_renderLines(char *strings[], int numLines, fw_font_face *fontFace, fx_anim_text_modifier **modifiers, fw_vec3f *color) {
    for (int i=0; i<numLines; i++) {
        fx_anim_text_renderLine(strings[i], fontFace, i * (fontFace->lineHeight + 2*fontFace->outline), modifiers[i], color);
    }
}

void fx_anim_text_render(fx_anim_text_meta *meta, fw_font_face *fontFace, float x, float y, float scale, fw_vec3f *color) {
    fw_projection_ortho();

    glBindTexture(GL_TEXTURE_2D, fontFace->image->textureID);
    glEnable(GL_TEXTURE_2D);

    glPushMatrix();
    glTranslatef(x,y,0);
    glScalef(scale,scale,1);

    fx_anim_text_renderLines(meta->strings, meta->numLines, fontFace, meta->modifiers, color);

    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void fx_anim_text_init_meta(fx_anim_text_meta *meta) {
    int numChars = 0;
    meta->modifiers = calloc(meta->numLines, sizeof(fx_anim_text_modifier*));
    for (int i=0; i<meta->numLines; i++) {
        int lineLength = strlen(meta->strings[i]);
        meta->modifiers[i] = calloc(lineLength, sizeof(fx_anim_text_modifier));
        numChars += lineLength;
        for (int j=0; j<lineLength; j++) {
            meta->modifiers[i][j].scale.x=1;
            meta->modifiers[i][j].scale.y=1;
        }
    }
    meta->numChars = numChars;

    // Create empty keyframes.
    int numKeys = meta->numKeysPerChar * numChars;
    meta->keys = calloc(numKeys, sizeof(fw_keyframe));
    meta->numKeys = numKeys;

    // Init keyframes with function pointer callback.
    int totalKeyIdx=0;
    int totalCharIdx=0;
    for (int i=0; i<meta->numLines; i++) {
        int lineLength = strlen(meta->strings[i]);
        for (int j=0; j<lineLength; j++) {
            for (int k=0; k<meta->numKeysPerChar; k++) {
                meta->keys[totalKeyIdx] = meta->createKey(meta, k, totalKeyIdx, totalCharIdx, i, j);
                totalKeyIdx++;
            }
            totalCharIdx++;
        }
    }
}

void fx_anim_text_setDefaultColor(fx_anim_text_meta *meta, fw_vec4f color) {
    for (int i=0; i<meta->numLines; i++) {
        int lineLength = strlen(meta->strings[i]);
        for (int j=0; j<lineLength; j++) {
            meta->modifiers[i][j].color = color;
        }
    }
}

void colorizeChars(fx_anim_text_meta *meta, int line, int charFrom, int charTo, fw_vec4f color) {
    int keyIdx = 0;
    for (int i=0; i<meta->numLines; i++) {
        for (int j=0; j<strlen(meta->strings[i]); j++) {

            // Modify keyframe of the given line and its characters from `charFrom` to `charTo` (zero-based).
            if (i==line && j >= charFrom && j<=charTo) {
                // Keyframe 0 holds the color `to` value. See `createKey` callback function.
                meta->keys[keyIdx+0].to = color;
                // Keyframe 2 holds the color `from` value. See `createKey` callback function.
                meta->keys[keyIdx+2].from = color;
            }

            // Every character has `numKeysPerChar` keyframes.
            keyIdx += meta->numKeysPerChar;
        }
    }
}

