#ifndef FW_ANIMATION_H
#define FW_ANIMATION_H

#include "types.h"

typedef struct {
    float timeStart;
    float timeEnd;
    fw_vec4f from;
    fw_vec4f to;
    float (*easing_func)(float);
    fw_vec4f *value;
} fw_keyframe;

enum fw_schedule_state {
    INITIALIZED,
    RUNNING,
    COMPLETED,
};

typedef struct fw_animation_struct {
    int isAutostart;
    fw_keyframe *keyframes;
    int numKeyframes;
    void (*started_proc)(struct fw_animation_struct*, fw_timer_data*);
    void (*completed_proc)(struct fw_animation_struct*, fw_timer_data*);
    enum fw_schedule_state state;
    float timeStartedAt;
} fw_animation;

typedef struct {
    fw_animation *animations;
    int numAnimations;
} fw_animation_schedule;


typedef struct fw_fw_scene_struct {
    int isAutostart;
    float duration;
    void (*init_proc)(void);
    void (*execute_proc)(fw_timer_data*);
    void (*completed_proc)(struct fw_fw_scene_struct*, fw_timer_data*);
    enum fw_schedule_state state;
    float timeStartedAt;
} fw_scene;

typedef struct {
    fw_scene *scenes;
    int numScenes;
} fw_scene_schedule;



void fw_animation_start(fw_animation *anim, fw_timer_data *time);
void fw_animation_processAnimationSchedule(fw_animation_schedule *animSchedule, fw_timer_data *time);
void fw_animation_resetAnimationSchedule(fw_animation_schedule *animSchedule);

void fw_animation_startScene(fw_scene *scene, fw_timer_data *time);
void fw_animation_stopScene(fw_scene *scene, int isTriggerCompletedProc, fw_timer_data *time);
void fw_animation_processSceneSchedule(fw_scene_schedule *sceneSchedule, fw_timer_data *time);

#endif
