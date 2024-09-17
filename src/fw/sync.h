#ifndef FW_SYNC_H
#define FW_SYNC_H

#include "types.h"

typedef struct {

    // How long to wait for next pulse.
    float beatDuration;

    // Total elapsed duration as a multiple of `beatDuration`.
    float elapsedBeatDuration;

    // The number of processed pulses.
    int beatNumber;

    void (*trigger_proc)(int beatNumber, fw_timer_data*);
} fw_sync_data;

void fw_sync_init(fw_sync_data* config, float beatDuration, void (*trigger_proc)(int beatNumber, fw_timer_data*));
void fw_sync_init2(fw_sync_data* config, int bpm, int speed, int rowsPerBeat, void (*trigger_proc)(int beatNumber, fw_timer_data*));
void fw_sync_process(fw_sync_data* config, fw_timer_data *time);

#endif
