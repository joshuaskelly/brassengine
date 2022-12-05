#include <stdlib.h>

#include "log.h"

#include "sounds.h"

sound_t* sounds_sound_new(uint64_t frame_count, uint16_t channel_count, int16_t* data) {
    sound_t* sound = (sound_t*)malloc(sizeof(sound_t));

    if (!sound) {
        log_error("Failed to create sound");
        return NULL;
    }

    sound->frame_count = frame_count;
    sound->channel_count = channel_count;
    sound->pcm = data;

    return sound;
}

void sounds_sound_free(sound_t* sound) {
    free(sound->pcm);
    sound->pcm = NULL;
    free(sound);
    sound = NULL;
}
