#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "platform.h"
#include "sounds.h"

sound_t* sounds_sound_new(uint64_t frame_count, uint16_t channel_count, sample_t* pcm) {
    size_t pcm_size = frame_count * channel_count * sizeof(sample_t);
    sound_t* sound = (sound_t*)malloc(sizeof(sound_t) + pcm_size);

    if (!sound) {
        log_error("Failed to create sound");
        return NULL;
    }

    sound->frame_count = frame_count;
    sound->channel_count = channel_count;
    memset(sound->pcm, 0, pcm_size);

    if (pcm) {
        memmove(sound->pcm, pcm, pcm_size);
    }

    return sound;
}

void sounds_sound_free(sound_t* sound) {
    free(sound);
    sound = NULL;
}

sound_t* sounds_sound_copy(sound_t* sound) {
    return sounds_sound_new(
        sound->frame_count,
        sound->channel_count,
        sound->pcm
    );
}

void sounds_sound_play(sound_t* sound) {
    platform_play_sound(sound);
}
