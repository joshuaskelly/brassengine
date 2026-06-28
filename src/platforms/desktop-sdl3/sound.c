/**
 * SDL3_mixer platform sound implementation
 */

#include <stdbool.h>
#include <stdio.h>

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3/SDL_error.h>

#include "../../log.h"
#include "../../math.h"
#include "../../sounds.h"

#define MIX_CHANNELS 8
static MIX_Track* tracks[MIX_CHANNELS];
static bool audio_disabled = false;
static MIX_Mixer* mixer = NULL;

SDL_AudioSpec spec = {
    .format = SDL_AUDIO_U8,
    .channels = 1,
    .freq = 11025
};

void platform_sound_play(sound_t* sound, int channel, bool looping) {
    if (audio_disabled) return;

    if (channel >= MIX_CHANNELS) {
        log_error("Error playing sound: channel %i does not exist", channel);
        return;
    }

    // Search for a free channel if channel not specified
    if (channel == -1) {
        for (int i = 0; i < MIX_CHANNELS; i++) {
            MIX_Track* track = tracks[i];
            if (!MIX_TrackPlaying(track)) {
                channel = i;
                break;
            }
        }
    }

    if (channel == -1) {
        log_error("Error playing sound: no free channels available");
        return;
    }

    size_t size = sound->frame_count * sound->channel_count * sizeof(sample_t);

    MIX_Audio* audio = MIX_LoadRawAudioNoCopy(
        mixer,
        sound->pcm,
        size,
        &spec,
        false
    );

    int loops = looping ? -1 : 0;
    MIX_Track* track = tracks[channel];

    MIX_SetTrackAudio(track, audio);
    MIX_PlayTrack(track, 0);
    MIX_SetTrackLoops(track, loops);
}

void platform_sound_stop(int channel) {
    if (channel < -1 || channel >= MIX_CHANNELS) {
        log_error("Error stopping channel: channel %i does not exist", channel);
        return;
    }

    MIX_Track* track = tracks[channel];
    MIX_StopTrack(track, 0);
}

void platform_sound_volume(int channel, float volume) {
    if (channel < -1 || channel >= MIX_CHANNELS) {
        log_error("Error stopping channel: channel %i does not exist", channel);
        return;
    }

    volume = clamp(volume, 0.0f, 1.0f);

    MIX_Track* track = tracks[channel];
    MIX_SetTrackGain(track, volume);
}

void sound_get_version(char* s) {
    const int mix_version = MIX_Version();

    snprintf(
        s,
        32,
        "SDL Mixer %i.%i.%i",
        SDL_VERSIONNUM_MAJOR(mix_version),
        SDL_VERSIONNUM_MINOR(mix_version),
        SDL_VERSIONNUM_MICRO(mix_version)
    );
}

void sound_init(void) {
    // Validate SDL version
    const int sdl_version = SDL_GetVersion();
    const int version_major = SDL_VERSIONNUM_MAJOR(sdl_version);
    const int version_minor = SDL_VERSIONNUM_MINOR(sdl_version);
    if (version_major != 3 && version_minor < 4) {
        log_fatal("SDL_mixer 3 requires SDL 3.4.0 or later");
    }

    if (!MIX_Init()) {
        log_error("Error intializing SDL Mixer");
        log_error(SDL_GetError());
        log_info("Sound playback will be disabled");
        audio_disabled = true;
        return;
    }

    // Create mixer device
    mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
    if (!mixer) {
        log_error("Error creating mixer device");
        log_error(SDL_GetError());
        log_info("Sound playback will be disabled");
        audio_disabled = true;
        return;
    }

    // Create mixer tracks
    for (int i = 0; i < MIX_CHANNELS; i++) {
        tracks[i] = MIX_CreateTrack(mixer);

        if (!tracks[i]) {
            log_error("Error creating track %i", i);
            log_error(SDL_GetError());
            log_info("Sound playback will be disabled");
            audio_disabled = true;
            return;
        }
    }
}

void sound_reload(void) {
    MIX_StopAllTracks(mixer, 0);
    //MIX_Volume(-1, MIX_MAX_VOLUME);
}

void sound_destroy(void) {
    MIX_StopAllTracks(mixer, 0);

    for (int i = 0; i < MIX_CHANNELS; i++) {
        MIX_DestroyTrack(tracks[i]);
        tracks[i] = NULL;
    }

    MIX_DestroyMixer(mixer);

    MIX_Quit();
}
