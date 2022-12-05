#ifndef SOUNDS_H
#define SOUNDS_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    int16_t* pcm;
    uint64_t frame_count;
    uint16_t channel_count;
} sound_t;

sound_t* sounds_sound_new(uint64_t frame_count, uint16_t channel_count, int16_t* data);
void sounds_sound_free(sound_t* sound);

#endif
