#include "sync.h"

void fw_sync_init(fw_sync_data* config, float beatDuration, void (*trigger_proc)(int beatNumber, fw_timer_data*)) {
    config->beatDuration = beatDuration;
    config->elapsedBeatDuration = -beatDuration;
    config->trigger_proc = trigger_proc;
    config->beatNumber = -1;
}

void fw_sync_init2(fw_sync_data* config, int bpm, int speed, int rowsPerBeat, void (*trigger_proc)(int beatNumber, fw_timer_data*)) {
    // See: https://wiki.openmpt.org/Manual:_Song_Properties#Tempo_Mode
    // Tick duration =  2.5 seconds / BPM
    // Row duration  = (2.5 seconds / BPM) * Speed = Tick duration * Speed
    // Beat Duration = (2.5 seconds / BPM) * Speed × Rows per Beat = Row duration * Rows per Beat
    // E.g. for BPM=125, Speed=6, Rows per Beat=4: (2.5 / 125) * 6 * 4 = 0.48
    fw_sync_init(config, (2.5/bpm)*speed*rowsPerBeat, trigger_proc);
}

void fw_sync_process(fw_sync_data* config, fw_timer_data *time) {
    // See: https://www.reddit.com/r/gamedev/comments/4chrz7/how_to_sync_music_in_a_rhythm_game/
    if (time->elapsed >= config->elapsedBeatDuration + config->beatDuration) {

        // Add up accumulated duration until current elapsed time is reached.
        // This will keep the values in sync even in case of massive frame drops.
        while ((config->elapsedBeatDuration + config->beatDuration) <= time->elapsed) {
            config->elapsedBeatDuration += config->beatDuration;
            config->beatNumber++;
        }

        if (config->trigger_proc) {
            config->trigger_proc(config->beatNumber, time);
        }

        //printf("time->elapsed = %f, elapsedBeatDuration=%f, beatNumber=%d, beatDuration=%f\n", time->elapsed, config->elapsedBeatDuration, config->beatNumber, config->beatDuration);
    }
}
