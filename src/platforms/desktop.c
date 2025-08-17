#include <stdbool.h>

#include <pthread.h>

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_error.h>

#include "../arguments.h"
#include "../assets.h"
#include "../configuration.h"
#include "../core.h"
#include "../event.h"
#include "../graphics.h"
#include "../input.h"
#include "../log.h"
#include "../math.h"
#include "../platform.h"
#include "../sounds.h"

#include "../modules/platforms/desktop.h"

#define FPS 60
#define FRAME_TIME_LENGTH (1000 / FPS)

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* render_buffer_texture = NULL;
static uint32_t* pixels = NULL;
static int ticks_last_frame;
static SDL_Rect display_rect;

static Mix_Chunk chunks[MIX_CHANNELS];
static bool audio_disabled = false;

static void sdl_init(void);
static void sdl_destroy(void);
static void sdl_handle_events(void);
static void sdl_fix_frame_rate(void);
static void sdl_pixels_resize(int width, int height);

static void mixer_init(void);
static void mixer_destroy(void);

static void log_platform_info(void);

int platform_main(int argc, char* argv[]) {
    if (arguments_check("-v") || arguments_check("--version")) {
       log_info(ENGINE_COPYRIGHT);

       return 0;
    }

    core_init();
    core_run();
    core_destroy();

    return 0;
}

void platform_init(void) {
    mixer_init();
    sdl_init();
    log_platform_info();
}

void platform_destroy(void) {
    mixer_destroy();
    sdl_destroy();
}

void platform_reload(void) {
    Mix_HaltChannel(-1);
    Mix_Volume(-1, MIX_MAX_VOLUME);
}

void platform_update(void) {
    sdl_handle_events();
    sdl_fix_frame_rate();
}

void platform_draw(void) {
    texture_t* render_texture = graphics_render_texture_get();
    uint32_t* palette = graphics_palette_get();

    // Convert core render buffer from indexed to rgba pixels
    for (int i = 0; i < render_texture->width * render_texture->height; i++) {
        pixels[i] = palette[render_texture->pixels[i]];
    }

    // Maintain aspect ratio and center in window
    int width;
    int height;
    graphics_resolution_get(&width, &height);

    int window_width;
    int window_height;

    SDL_GetWindowSize(window, &window_width, &window_height);

    float window_aspect = window_width / (float)window_height;
    float buffer_aspect = width / (float)height * config->display.aspect;

    display_rect.w = window_width;
    display_rect.h = window_height;

    if (buffer_aspect < window_aspect) {
        display_rect.w = display_rect.h * buffer_aspect;
    }
    else {
        display_rect.h = display_rect.w / buffer_aspect;
    }

    display_rect.x = (window_width - display_rect.w) / 2;
    display_rect.y = (window_height - display_rect.h) / 2;

    SDL_UpdateTexture(
        render_buffer_texture,
        NULL,
        pixels,
        render_texture->width * sizeof(uint32_t)
    );

    SDL_RenderClear(renderer);

    SDL_RenderCopy(
        renderer,
        render_buffer_texture,
        NULL,
        &display_rect
    );

    SDL_RenderPresent(renderer);
}

bool platform_handle_event(event_t* event) {
    if (event->type == EVENT_GRAPHICSRESOLUTIONCHANGED) {
        int width = event->graphics_resolution_change.width;
        int height = event->graphics_resolution_change.height;

        sdl_pixels_resize(width, height);

        SDL_DestroyTexture(render_buffer_texture);

        render_buffer_texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA32,
            SDL_TEXTUREACCESS_STREAMING,
            width,
            height
        );

        if (!render_buffer_texture) {
            log_fatal("Error creating SDL frame buffer texture");
        }

        return true;
    }

    return false;
}

void platform_sound_play(sound_t* sound, int channel, bool looping) {
    if (audio_disabled) return;

    if (channel >= MIX_CHANNELS) {
        log_error("Error playing sound: channel %i does not exist", channel);
        return;
    }

    // Search for a free channel if channel not specified
    if (channel == -1) {
        for (int i = 0; i < MIX_CHANNELS; i++) {
            if (!Mix_Playing(i)) {
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

    // Configure chunk with sound data
    Mix_Chunk* chunk = &chunks[channel];
    chunk->allocated = 0;
    chunk->alen = size;
    chunk->abuf = (uint8_t*)sound->pcm;
    chunk->volume = 128;

    int loops = looping ? -1 : 0;

    Mix_PlayChannel(channel, chunk, loops);
}

void platform_sound_stop(int channel) {
    if (channel < -1 || channel >= MIX_CHANNELS) {
        log_error("Error stopping channel: channel %i does not exist", channel);
        return;
    }

    Mix_HaltChannel(channel);
}

void platform_sound_volume(int channel, float volume) {
    if (channel < -1 || channel >= MIX_CHANNELS) {
        log_error("Error stopping channel: channel %i does not exist", channel);
        return;
    }

    volume = clamp(volume, 0.0f, 1.0f);

    Mix_Volume(channel, volume * MIX_MAX_VOLUME);
}

void platform_mouse_grabbed_set(bool grabbed) {
    SDL_SetRelativeMouseMode(grabbed);
}

bool platform_mouse_grabbed_get(void) {
    return SDL_GetRelativeMouseMode();
}

void platform_open_module(void* arg) {
    open_desktop_platform_module(arg, window);
}

struct thread {
    pthread_t pthread;
};

thread_t* platform_thread_new(void* (function)(void*), void* args) {
    thread_t* thread = (thread_t*)malloc(sizeof(thread_t));
    pthread_create(&thread->pthread, NULL, function, args);

    return thread;
}

void platform_thread_free(thread_t* thread) {
    if (thread == NULL) return;

    free(thread);
}

void platform_thread_detatch(thread_t* thread) {
    pthread_detach(thread->pthread);
}

void* platform_thread_join(thread_t* thread) {
    void* result = NULL;
    pthread_join(thread->pthread, result);
    free(thread);

    return result;
}

void platform_thread_exit(void* result) {
    pthread_exit(result);
}

struct thread_lock {
    pthread_mutex_t mutex;
};

thread_lock_t* platform_thread_lock_new(void) {
    thread_lock_t* lock = NULL;
    lock = (thread_lock_t*)malloc(sizeof(thread_lock_t));

    pthread_mutex_init(&lock->mutex, NULL);

    return lock;
}

void platform_thread_lock_free(thread_lock_t* lock) {
    free(lock);
}

void platform_thread_lock_lock(thread_lock_t* lock) {
    pthread_mutex_lock(&(lock->mutex));
}

void platform_thread_lock_unlock(thread_lock_t* lock) {
    pthread_mutex_unlock(&(lock->mutex));
}

struct thread_condition {
    pthread_cond_t cond;
};

thread_condition_t* platform_thread_condition_new(void) {
    thread_condition_t* condition = (thread_condition_t*)malloc(sizeof(thread_condition_t));

    pthread_cond_init(&(condition->cond), NULL);

    return condition;
}

void platform_thread_condition_free(thread_condition_t* condition) {
    if (condition == NULL) return;

    pthread_cond_destroy(&(condition->cond));
    free(condition);
}

void platform_thread_condition_wait(thread_condition_t* condition, thread_lock_t* lock) {
    pthread_cond_wait(&(condition->cond), &(lock->mutex));
}

void platform_thread_condition_notify(thread_condition_t* condition) {
    pthread_cond_signal(&(condition->cond));
}

static void sdl_init(void) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) != 0) {
        log_fatal("Error initializing SDL");
    }

    const int width = config->resolution.width;
    const int height = config->resolution.height;
    const int default_window_scale = 3;

    // Create SDL window
    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width * default_window_scale,
        height * default_window_scale,
        SDL_WINDOW_RESIZABLE
    );

    if (!window) {
        log_fatal("Error creating SDL window");
    }

    // Create SDL renderer
    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    if (!renderer) {
        log_fatal("Error creating SDL renderer");
    }

    sdl_pixels_resize(width, height);

    // Create render buffer texture
    render_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        width,
        height
    );

    if (!render_buffer_texture) {
        log_fatal("Error creating SDL frame buffer texture");
    }

    // Hide cursor by default
    SDL_ShowCursor(SDL_DISABLE);
}

static void sdl_destroy(void) {
    SDL_DestroyTexture(render_buffer_texture);
    free(pixels);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

static void sdl_handle_events(void) {
    SDL_Event sdl_event;
    event_t event;
    SDL_GameController* controller = NULL;

    int width;
    int height;
    graphics_resolution_get(&width, &height);

    float aspect_width = width / (float)display_rect.w;
    float aspect_height = height / (float)display_rect.h;

    while (SDL_PollEvent(&sdl_event)) {
        switch (sdl_event.type) {
            case SDL_QUIT:
                event.type = EVENT_QUIT;
                event_post(&event);
                break;

            case SDL_KEYDOWN:
                event.type = EVENT_KEYDOWN;
                event.key.type = EVENT_KEYDOWN;
                event.key.code = (key_code_t)sdl_event.key.keysym.scancode;
                event.key.symbol = (key_symbol_t)sdl_event.key.keysym.sym;
                event_post(&event);
                break;

            case SDL_KEYUP:
                event.type = EVENT_KEYUP;
                event.key.type = EVENT_KEYUP;
                event.key.code = (key_code_t)sdl_event.key.keysym.scancode;
                event.key.symbol = (key_symbol_t)sdl_event.key.keysym.sym;
                event_post(&event);
                break;

            case SDL_MOUSEMOTION:
                event.type = EVENT_MOUSEMOTION;
                event.motion.type = EVENT_MOUSEMOTION;
                event.motion.x = (sdl_event.motion.x - display_rect.x) * aspect_width;
                event.motion.y = (sdl_event.motion.y - display_rect.y) * aspect_height;
                event.motion.motion_x = (sdl_event.motion.xrel) * aspect_width;
                event.motion.motion_y = (sdl_event.motion.yrel) * aspect_height;

                event.motion.x = clamp(event.motion.x, 0, width - 1);
                event.motion.y = clamp(event.motion.y, 0, height - 1);

                event_post(&event);
                break;

            case SDL_MOUSEBUTTONDOWN:
                event.type = EVENT_MOUSEDOWN;
                event.button.type = EVENT_MOUSEDOWN;
                event.button.button = sdl_event.button.button;
                event_post(&event);
                break;

            case SDL_MOUSEBUTTONUP:
                event.type = EVENT_MOUSEUP;
                event.button.type = EVENT_MOUSEUP;
                event.button.button = sdl_event.button.button;
                event_post(&event);
                break;

            case SDL_MOUSEWHEEL:
                event.type = EVENT_MOUSEWHEEL;
                event.wheel.wheel_x = sdl_event.wheel.x;
                event.wheel.wheel_y = sdl_event.wheel.y;
                event_post(&event);
                break;

            case SDL_CONTROLLERDEVICEADDED:
                controller = SDL_GameControllerOpen(sdl_event.cdevice.which);
                int id = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller));
                input_controller_connect(id);
                break;

            case SDL_CONTROLLERDEVICEREMOVED:
                input_controller_disconnect(sdl_event.cdevice.which);
                SDL_GameControllerClose(SDL_GameControllerFromInstanceID(sdl_event.cdevice.which));
                break;

            case SDL_CONTROLLERBUTTONDOWN:
                event.type = EVENT_CONTROLLERBUTTONDOWN;
                event.controller_button.which = sdl_event.cdevice.which;
                event.controller_button.button = sdl_event.cbutton.button;
                event_post(&event);
                break;

            case SDL_CONTROLLERBUTTONUP:
                event.type = EVENT_CONTROLLERBUTTONUP;
                event.controller_button.which = sdl_event.cdevice.which;
                event.controller_button.button = sdl_event.cbutton.button;
                event_post(&event);
                break;

            case SDL_CONTROLLERAXISMOTION:
                event.type = EVENT_CONTROLLERAXISMOTION;
                event.controller_axis.which = sdl_event.cdevice.which;
                event.controller_axis.axis = sdl_event.caxis.axis;
                if (sdl_event.caxis.value < 0) {
                    event.controller_axis.value = sdl_event.caxis.value / 32768.0f;
                }
                else {
                    event.controller_axis.value = sdl_event.caxis.value / 32767.0f;
                }
                event_post(&event);
                break;
        }
    }
}

static void sdl_fix_frame_rate(void) {
    int time_to_wait = FRAME_TIME_LENGTH - (SDL_GetTicks() - ticks_last_frame);
    if (0 < time_to_wait && time_to_wait < FRAME_TIME_LENGTH) {
        SDL_Delay(time_to_wait);
    }

    ticks_last_frame = SDL_GetTicks();
}

static void sdl_pixels_resize(int width, int height) {
    if (sizeof(pixels) == width * height * sizeof(uint32_t)) return;

    // Free if not NULL
    if (pixels) {
        free(pixels);
        pixels = NULL;
    }

    pixels = calloc(width * height, sizeof(uint32_t));

    if (!pixels) {
        log_fatal("Error creating frame buffer.");
    }
}

static void mixer_init(void) {
    if (Mix_OpenAudioDevice(11025, AUDIO_U8, 1, 256, NULL, 0) < 0) {
        log_error("Error intializing SDL Mixer");
        log_error(SDL_GetError());
        log_info("Sound playback will be disabled");
        audio_disabled = true;
    }
}

static void mixer_destroy(void) {
    Mix_HaltChannel(-1);
    Mix_Quit();
}

static void log_platform_info(void) {
    // Get SDL version info
    SDL_version version;
    SDL_GetVersion(&version);

    // Get SDL Mixer version info
    const SDL_version* mix_version = Mix_Linked_Version();

    // Build info string
    char buffer[128];
    snprintf(
        buffer,
        sizeof(buffer),
        "platform init (SDL %i.%i.%i, SDL Mixer %i.%i.%i)",
        version.major, version.minor, version.patch,
        mix_version->major, mix_version->minor, mix_version->patch
    );

    log_info(buffer);
}
