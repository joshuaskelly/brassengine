#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_error.h>
#include <emscripten.h>

#include "../configuration.h"
#include "../core.h"
#include "../event.h"
#include "../graphics.h"
#include "../input.h"
#include "../log.h"
#include "../math.h"
#include "../platform.h"
#include "../sounds.h"

#include "../modules/platforms/web.h"

#define FPS 60
#define FRAME_TIME_LENGTH (1000 / FPS)

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* render_buffer_texture = NULL;
static uint32_t* render_buffer = NULL;
static int ticks_last_frame;
static SDL_Rect display_rect;

static Mix_Chunk chunks[MIX_CHANNELS];
static bool audio_disabled = false;

static void sdl_handle_events(void);
static void sdl_fix_frame_rate(void);

int platform_main(int argc, char* argv[]) {
    core_init();
    emscripten_set_main_loop(core_main_loop, 0, 1);

    return 0;
}

void platform_init(void) {
    // Get platform version info
    SDL_version version;
    SDL_GetVersion(&version);

    const SDL_version* mix_version = Mix_Linked_Version();

    char buffer[128];
    snprintf(
        buffer,
        sizeof(buffer),
        "platform init (Emscripten %i.%i.%i, SDL %i.%i.%i, SDL Mixer %i.%i.%i)",
        __EMSCRIPTEN_major__, __EMSCRIPTEN_minor__, __EMSCRIPTEN_tiny__,
        version.major, version.minor, version.patch,
        mix_version->major, mix_version->minor, mix_version->patch
    );

    log_info(buffer);

    const int window_width = config->resolution.width * 3;
    const int window_height = config->resolution.height * 3;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) != 0) {
        log_fatal("Error initializing SDL");
    }

    if (Mix_OpenAudioDevice(11025, AUDIO_U8, 1, 2048, NULL, 0) < 0) {
        log_error("Error intializing SDL Mixer");
        log_error(SDL_GetError());
        log_info("Sound playback will be disabled");
        audio_disabled = true;
    }

    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_RESIZABLE
    );

    if (!window) {
        log_fatal("Error creating SDL window");
    }

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    if (!renderer) {
        log_fatal("Error creating SDL renderer");
    }

    SDL_SetRenderDrawBlendMode(
        renderer,
        SDL_BLENDMODE_BLEND
    );

    render_buffer = calloc(config->resolution.width * config->resolution.height, sizeof(uint32_t));

    if (!render_buffer) {
        log_fatal("Error creating frame buffer.");
    }

    render_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        config->resolution.width,
        config->resolution.height
    );

    if (!render_buffer_texture) {
        log_fatal("Error creating SDL frame buffer texture");
    }

    display_rect.x = 0;
    display_rect.y = 0;
    display_rect.w = window_width;
    display_rect.h = window_height;

    SDL_ShowCursor(SDL_DISABLE);
}

void platform_destroy(void) {
    SDL_DestroyTexture(render_buffer_texture);
    free(render_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_Quit();
    SDL_Quit();
}

void platform_reload(void) {
}

void platform_update(void) {
    sdl_handle_events();
    sdl_fix_frame_rate();
}

void platform_draw(void) {
    texture_t* render_texture = graphics_render_texture_get();
    uint32_t* palette = graphics_palette_get();

    // Convert core render buffer from indexed to rgba
    for (int i = 0; i < render_texture->width * render_texture->height; i++) {
        render_buffer[i] = palette[render_texture->pixels[i]];
    }

    // Maintain aspect ratio and center in window
    int window_width;
    int window_height;

    SDL_GetWindowSize(window, &window_width, &window_height);

    float window_aspect = window_width / (float)window_height;
    float buffer_aspect = config->resolution.width / (float)config->resolution.height * config->display.aspect;

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
        render_buffer,
        render_texture->width * sizeof(uint32_t)
    );

    SDL_RenderCopy(
        renderer,
        render_buffer_texture,
        NULL,
        &display_rect
    );

    SDL_RenderPresent(renderer);
}

static void sdl_handle_events(void) {
    SDL_Event sdl_event;
    event_t event;
    SDL_GameController* controller = NULL;

    float aspect_width = config->resolution.width / (float)display_rect.w;
    float aspect_height = config->resolution.height / (float)display_rect.h;

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

                event.motion.x = clamp(event.motion.x, 0, config->resolution.width - 1);
                event.motion.y = clamp(event.motion.y, 0, config->resolution.height - 1);

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

void platform_sound_play(sound_t* sound, int channel) {
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

    Mix_PlayChannel(channel, chunk, 0);
}

void platform_mouse_grabbed_set(bool grabbed) {
    SDL_SetWindowMouseGrab(window, grabbed);
}

bool platform_mouse_grabbed_get(void) {
    return SDL_GetWindowMouseGrab(window);
}

void platform_open_module(void* arg) {
    open_web_platform_module(arg, window);
}
