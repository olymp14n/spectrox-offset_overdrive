#include "sound.h"
#include <windows.h>
#include "micromod.h"
#include "../config/config.h"
#include "../dat/song_data.h"

#if FW_SOUND_READ_FILE_ENABLED
#include "file.h"
#endif

#define REVERB_ENABLED FALSE
#define SAMPLING_FREQ  48000  /* 48khz. */
#define REVERB_BUF_LEN 4800   /* 50ms. */
#define OVERSAMPLE     2      /* 2x oversampling. */
#define NUM_CHANNELS   2      /* Stereo. */
#define BUFFER_SAMPLES 16384  /* 64k per buffer. */
#define NUM_BUFFERS    4      /* 4 buffers (256k). */

static HWAVEOUT _waveOutHandle;
static WAVEHDR _waveHeaders[NUM_BUFFERS];
static short _reverbBuffer[REVERB_BUF_LEN];
static short _mixBuffer[BUFFER_SAMPLES * NUM_CHANNELS * OVERSAMPLE];
static short _soundBuffers[NUM_BUFFERS][BUFFER_SAMPLES * NUM_CHANNELS];

static long _reverbIdx, _filtL, _filtR;
static BOOL _isReady = FALSE;
static DWORD _threadID;


/*
	2:1 downsampling with simple but effective anti-aliasing.
	Count is the number of stereo samples to process, and must be even.
	input may point to the same buffer as output.
*/
static void fw_sound_downsample(short *input, short *output, long count) {
    long inIdx, outIdx, outL, outR;
    inIdx = outIdx = 0;
    while(outIdx < count) {
        outL = _filtL + (input[inIdx++] >> 1);
        outR = _filtR + (input[inIdx++] >> 1);
        _filtL = input[inIdx++] >> 2;
        _filtR = input[inIdx++] >> 2;
        output[outIdx++] = outL + _filtL;
        output[outIdx++] = outR + _filtR;
    }
}

/* Simple stereo cross delay with feedback. */
static void fw_sound_reverb(short *soundBuffer, long count) {
    long bufferIdx, bufferEnd;
    bufferIdx = 0;
    bufferEnd = bufferIdx + (count << 1);
    while(bufferIdx < bufferEnd) {
        soundBuffer[bufferIdx] = (soundBuffer[bufferIdx] * 3 + _reverbBuffer[_reverbIdx + 1]) >> 2;
        soundBuffer[bufferIdx + 1] = (soundBuffer[bufferIdx + 1] * 3 + _reverbBuffer[_reverbIdx]) >> 2;
        _reverbBuffer[_reverbIdx] = soundBuffer[bufferIdx];
        _reverbBuffer[_reverbIdx + 1] = soundBuffer[bufferIdx + 1];
        _reverbIdx += 2;
        if(_reverbIdx >= REVERB_BUF_LEN) {
            _reverbIdx = 0;
        }
        bufferIdx += 2;
    }
}

/* Reduce stereo-separation of count samples. */
static void fw_sound_crossfeed(short *soundBuffer, int count) {
    int l, r, offset = 0, end = count << 1;
    while(offset < end) {
        l = soundBuffer[offset];
        r = soundBuffer[offset + 1];
        soundBuffer[offset++] = (l + l + l + r) >> 2;
        soundBuffer[offset++] = (r + r + r + l) >> 2;
    }
}

static void fw_sound_fillSoundBuffer(short *soundBuffer) {
    memset(_mixBuffer, 0, BUFFER_SAMPLES * NUM_CHANNELS * OVERSAMPLE * sizeof(short));
    micromod_get_audio(_mixBuffer, BUFFER_SAMPLES * OVERSAMPLE);
    fw_sound_downsample(_mixBuffer, soundBuffer, BUFFER_SAMPLES * OVERSAMPLE);
    fw_sound_crossfeed(soundBuffer, BUFFER_SAMPLES);
    if (REVERB_ENABLED) {
        fw_sound_reverb(soundBuffer, BUFFER_SAMPLES);
    }
}

static void fw_sound_threadProc() {
    _isReady = TRUE;
    MSG msg;
    while (_isReady && GetMessage(&msg, 0, 0, 0)) {

        switch (msg.message) {
        case MM_WOM_OPEN:
            break;
        case MM_WOM_CLOSE:
            _isReady = FALSE;
            break;
        case MM_WOM_DONE: {
            if (_isReady) {
                WAVEHDR *wh =  (WAVEHDR*)msg.lParam;
                fw_sound_fillSoundBuffer((short *)wh->lpData);
                waveOutWrite(_waveOutHandle, wh, sizeof(WAVEHDR));
            }
            break;
        }
        default:
            break;
        }
    }
}

static void fw_sound_initWaveFormat(WAVEFORMATEX *waveFormat) {
    waveFormat->wFormatTag = WAVE_FORMAT_PCM;
    waveFormat->nChannels = NUM_CHANNELS;
    waveFormat->nSamplesPerSec = SAMPLING_FREQ;
    waveFormat->nAvgBytesPerSec = SAMPLING_FREQ * NUM_CHANNELS * 2;
    waveFormat->nBlockAlign = NUM_CHANNELS * 2;
    waveFormat->wBitsPerSample = 16;
    waveFormat->cbSize = 0;
}

static void fw_sound_initWaveHeader(WAVEHDR *waveHeader, char *soundBuffer, DWORD soundBufferSize) {
    waveHeader->lpData=soundBuffer;
    waveHeader->dwBufferLength=soundBufferSize;
    waveHeader->dwFlags=0;
    waveHeader->dwLoops=0;
}

void fw_sound_init() {
    HANDLE threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) fw_sound_threadProc, NULL, 0, &_threadID);
    CloseHandle(threadHandle);

#if FW_SOUND_READ_FILE_ENABLED
    char *fileBytes = fw_file_readBytes(FW_SOUND_FILE_PATH);
    if (micromod_initialise((signed char *)fileBytes, SAMPLING_FREQ * OVERSAMPLE) != 0) {
        exit(55001);
    }
#else
    if (micromod_initialise((signed char *)_songData, SAMPLING_FREQ * OVERSAMPLE) != 0) {
        exit(55001);
    }
#endif

    for (int i=0; i<NUM_BUFFERS; i++) {
        fw_sound_fillSoundBuffer(_soundBuffers[i]);
    }

    WAVEFORMATEX waveFormat = {0};
    fw_sound_initWaveFormat(&waveFormat);

    // Open audio device and start passing messages to the callback thread.
    if (waveOutOpen(&_waveOutHandle, WAVE_MAPPER, &waveFormat, _threadID, 0, CALLBACK_THREAD) != MMSYSERR_NOERROR) {
        exit(55002);
    }

    for (int i=0; i<NUM_BUFFERS; i++) {
        fw_sound_initWaveHeader(&_waveHeaders[i], (char*)_soundBuffers[i], sizeof(_soundBuffers[i]));

        if (waveOutPrepareHeader(_waveOutHandle, &_waveHeaders[i], sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
            waveOutClose(_waveOutHandle);
            exit(55003);
        }
    }
}

void fw_sound_play() {
    while (!_isReady) {
        Sleep(100);
    }

    for (int i=0; i<NUM_BUFFERS; i++) {
        waveOutWrite(_waveOutHandle, &_waveHeaders[i], sizeof(WAVEHDR));
    }
}

void fw_sound_shutdown() {
    _isReady = FALSE;

    waveOutReset(_waveOutHandle);

    for (int i=0; i<NUM_BUFFERS; i++) {
        waveOutUnprepareHeader(_waveOutHandle, &_waveHeaders[i], sizeof(WAVEHDR));
    }

    waveOutClose(_waveOutHandle);
}

