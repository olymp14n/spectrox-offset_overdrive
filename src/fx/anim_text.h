#ifndef FX_ANIM_TEXT_H
#define FX_ANIM_TEXT_H

#include "../fw/font.h"
#include "../fw/animation.h"

typedef struct {
    fw_vec4f translate;
    fw_vec4f rotate;
    fw_vec4f scale;
    fw_vec4f color;
} fx_anim_text_modifier;

typedef struct fx_anim_text_meta {
    // Lines of text.
    char **strings;
    int numLines;

    // Total number of characters across all lines.
    int numChars;

    // Modifiers for every character in a line.
    fx_anim_text_modifier **modifiers;

    // Multiple keyframess per character.
    fw_keyframe *keys;
    int numKeys;

    // Number of keyframes per character.
    int numKeysPerChar;

    float delay;
    float duration;

    fw_keyframe (*createKey)(struct fx_anim_text_meta *meta, int keyType, int totalKeyIdx, int totalCharIdx, int lineIdx, int charIdx);

} fx_anim_text_meta;

/**
 * color - If not NULL, use this RGB instead of the calculated color from `modifiers`.
 */
void fx_anim_text_render(fx_anim_text_meta *meta, fw_font_face *fontFace, float x, float y, float scale, fw_vec3f *color);


/**
 * Init the `meta` structure.
 * `fx_anim_text_meta.strings`, `fx_anim_text_meta.numLines` and `fx_anim_text_meta.numKeysPerChar` have to be already set before calling this procedure.
 */
void fx_anim_text_init_meta(fx_anim_text_meta *meta);

void fx_anim_text_setDefaultColor(fx_anim_text_meta *meta, fw_vec4f color);

void colorizeChars(fx_anim_text_meta *meta, int line, int charFrom, int charTo, fw_vec4f color);

#endif
