/**
 * @file sounds.h
 * Sounds module. Responsible for working with sound.
 */

#ifndef SOUNDS_H
#define SOUNDS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef uint8_t sample_t;

typedef struct {
    uint64_t frame_count;
    uint16_t channel_count;
    sample_t pcm[];
} sound_t;

/**
 * Create a new sound.
 *
 * @param frame_count Number of frames.
 * @param channel_count Number of channels.
 * @param pcm PCM data to copy or NULL.
 * @return New sound if successful, NULL otherwise.
 */
sound_t* sounds_sound_new(uint64_t frame_count, uint16_t channel_count, sample_t* pcm);

/**
 * Frees a sound.
 *
 * @param sound Sound to free
 */
void sounds_sound_free(sound_t* sound);

/**
 * Copy given sound.
 *
 * @param sound Sound to copy.
 * @return New sound if successful, NULL otherwise.
 */
sound_t* sounds_sound_copy(sound_t* sound);

/**
 * Plays a sound.
 *
 * @param sound Sound to play
 * @param channel Channel to play sound on
*/
void sounds_sound_play(sound_t* sound, int channel, bool looping);

/**
 * Stop playback for given channel.
 *
 * @param channel Channel to stop
 */
void sounds_sound_stop(int channel);

/**
 * Set volume level for given channel and volume.
 *
 * @param channel Channel to set
 * @param volume Volume level. Range 0.0 to 1.0
 */
void sounds_sound_volume(int channel, float volume);

#endif
