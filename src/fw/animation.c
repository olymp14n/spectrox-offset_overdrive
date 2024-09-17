#include "animation.h"

static void fw_animation_processKeyframes(fw_keyframe *keys, int numKeyframes, fw_timer_data *time, float timeOffset) {
    for (int i=0; i<numKeyframes; i++) {

        float keyTimeStart = keys[i].timeStart + timeOffset;
        float keyTimeEnd = keys[i].timeEnd + timeOffset;

        if (time->elapsed >= keyTimeEnd) {
            keys[i].value->x = keys[i].to.x;
            keys[i].value->y = keys[i].to.y;
            keys[i].value->z = keys[i].to.z;
            keys[i].value->w = keys[i].to.w;
            continue;
        }

        if (time->elapsed >= keyTimeStart && time->elapsed < keyTimeEnd) {
            float t = (time->elapsed - keyTimeStart)/(keyTimeEnd - keyTimeStart);
            t = keys[i].easing_func(t);
            keys[i].value->x = keys[i].from.x + t * (keys[i].to.x - keys[i].from.x);
            keys[i].value->y = keys[i].from.y + t * (keys[i].to.y - keys[i].from.y);
            keys[i].value->z = keys[i].from.z + t * (keys[i].to.z - keys[i].from.z);
            keys[i].value->w = keys[i].from.w + t * (keys[i].to.w - keys[i].from.w);
        }
    }
}


void fw_animation_start(fw_animation *anim, fw_timer_data *time) {
    if (anim->started_proc) {
        anim->started_proc(anim, time);
    }

    anim->state = RUNNING;
    anim->timeStartedAt = time->elapsed;
}

static void fw_animation_resetAnimation(fw_animation *anim) {
    anim->state = INITIALIZED;
    anim->timeStartedAt = 0.0;
}

static void fw_animation_processAnimation(fw_animation *anim, fw_timer_data *time) {
    if (anim->isAutostart && anim->state==INITIALIZED) {
        fw_animation_start(anim, time);
    }

    if (anim->state != RUNNING) {
        return;
    }

    fw_animation_processKeyframes(anim->keyframes, anim->numKeyframes, time, anim->timeStartedAt);
}

static int fw_animation_isAnimationCompleted(fw_animation *anim, fw_timer_data *time) {
    float maxKeyTimeEnd = 0.f;

    for (int i=0; i<anim->numKeyframes; i++) {
        if (anim->keyframes[i].timeEnd > maxKeyTimeEnd) {
            maxKeyTimeEnd = anim->keyframes[i].timeEnd;
        }
    }

    return time->elapsed >= maxKeyTimeEnd + anim->timeStartedAt;
}

static void fw_animation_handleCompletedAnimation(fw_animation *anim, fw_timer_data *time) {
    if (anim->state == RUNNING && fw_animation_isAnimationCompleted(anim, time)) {

        // before setting completed state, process animation a last time
        // to ensure last target values are getting set.
        fw_animation_processAnimation(anim, time);

        anim->state = COMPLETED;

        if (anim->completed_proc) {
            anim->completed_proc(anim, time);
        }
    }
}

void fw_animation_processAnimationSchedule(fw_animation_schedule *animSchedule, fw_timer_data *time) {
    for (int i=0; i<animSchedule->numAnimations; i++) {
        fw_animation_handleCompletedAnimation(&animSchedule->animations[i], time);
    }

    for (int i=0; i<animSchedule->numAnimations; i++) {
        fw_animation_processAnimation(&animSchedule->animations[i], time);
    }
}

void fw_animation_resetAnimationSchedule(fw_animation_schedule *animSchedule) {
    for (int i=0; i<animSchedule->numAnimations; i++) {
        fw_animation_resetAnimation(&animSchedule->animations[i]);
    }
}

void fw_animation_startScene(fw_scene *scene, fw_timer_data *time) {
    if (scene->init_proc) {
        scene->init_proc();
    }

    scene->state = RUNNING;
    scene->timeStartedAt = time->elapsed;
}

void fw_animation_stopScene(fw_scene *scene, int isTriggerCompletedProc, fw_timer_data *time) {
    scene->state = COMPLETED;
    if (isTriggerCompletedProc && scene->completed_proc) {
        scene->completed_proc(scene, time);
    }
}

static void fw_animation_processScene(fw_scene *scene, fw_timer_data *time) {
    if (scene->isAutostart && scene->state == INITIALIZED) {
        fw_animation_startScene(scene, time);
    }

    if (scene->state != RUNNING) {
        return;
    }

    if (scene->execute_proc) {
        scene->execute_proc(time);
    }
}

static void fw_animation_handleCompletedScene(fw_scene *scene, fw_timer_data *time) {
    if (scene->state == RUNNING && time->elapsed > scene->timeStartedAt + scene->duration) {
        fw_animation_stopScene(scene, 1, time);
    }
}

void fw_animation_processSceneSchedule(fw_scene_schedule *sceneSchedule, fw_timer_data *time) {
    for (int i=0; i<sceneSchedule->numScenes; i++) {
        fw_animation_handleCompletedScene(&sceneSchedule->scenes[i], time);
    }

    for (int i=0; i<sceneSchedule->numScenes; i++) {
        fw_animation_processScene(&sceneSchedule->scenes[i], time);
    }
}

