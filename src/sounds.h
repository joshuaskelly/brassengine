#ifndef SOUNDS_H
#define SOUNDS_H

#include <stddef.h>
#include <stdint.h>

typedef int16_t sample_t;

typedef struct {
    sample_t* pcm;
    uint64_t frame_count;
    uint16_t channel_count;
} sound_t;

sound_t* sounds_sound_new(uint64_t frame_count, uint16_t channel_count, sample_t* data);
void sounds_sound_free(sound_t* sound);

#endif
